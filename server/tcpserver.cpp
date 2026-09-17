#include "tcpserver.h"
#include "databasemanager.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QDate>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include "userdao.h"
#include "studentdao.h"
#include "scoredao.h"
#include "businesslogic.h"

// 解析日期，兼容 yyyy-MM-dd 和 yyyy/MM/dd 两种格式
static QDate parseDate(const QString &str) {
    QDate d = QDate::fromString(str, "yyyy-MM-dd");
    if (d.isValid()) return d;
    return QDate::fromString(str, "yyyy/MM/dd");
}

TcpServer::TcpServer(QObject *parent) : QObject(parent), m_nextClientId(1)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

TcpServer::~TcpServer()
{
    stopServer();
}

bool TcpServer::startServer(quint16 port)
{
    if (m_server->isListening()) {
        emit logMessage("服务器已经在运行中");
        return false;
    }

    if (m_server->listen(QHostAddress::Any, port)) {
        emit logMessage(QString("服务器启动成功，监听端口：%1").arg(port));
        return true;
    } else {
        emit logMessage(QString("服务器启动失败：%1").arg(m_server->errorString()));
        return false;
    }
}

void TcpServer::stopServer()
{
    // 先向所有已连接的客户端发送关闭通知
    QJsonObject shutdownMsg;
    shutdownMsg["cmd"] = "server_shutdown";
    shutdownMsg["message"] = "服务器已停止，请确认退出";
    for (auto it = m_buffer.begin(); it != m_buffer.end(); ++it) {
        QTcpSocket *socket = it.key();
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            sendResponse(socket, shutdownMsg);
            socket->flush();
            socket->disconnectFromHost();
        }
    }
    m_buffer.clear();

    if (m_server->isListening()) {
        m_server->close();
        emit logMessage("服务器已停止");
    }
}

bool TcpServer::isRunning() const
{
    return m_server->isListening();
}

void TcpServer::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;

    // 分配唯一ID
    int clientId = m_nextClientId++;
    socket->setProperty("clientId", clientId);

    QString ip = socket->peerAddress().toString();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    emit clientConnected(clientId, ip, time);
    emit logMessage(QString("新连接来自：%1，ID：%2").arg(ip).arg(clientId));
    qDebug() << "🔗 新连接建立，ID:" << clientId << ", IP:" << ip << ", Time:" << time;

    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);

    // 连接断开时：清理缓冲区并删除 socket
    connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
        int id = socket->property("clientId").toInt();   // 使用属性获取ID
        emit clientDisconnected(id);
        emit logMessage(QString("客户端 %1 断开连接").arg(id));
        qDebug() << "🔌 客户端断开，ID:" << id;
        m_buffer.remove(socket);   // 移除缓冲区
        socket->deleteLater();     // 稍后删除 socket
    });
}

void TcpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // 追加数据到该 socket 的缓冲区
    m_buffer[socket].append(socket->readAll());

    // 循环处理缓冲区中所有完整的请求（支持同一连接多个请求）
    while (true) {
        QByteArray &buf = m_buffer[socket];
        int headerEnd = buf.indexOf("\r\n\r\n");
        if (headerEnd == -1) {
            qDebug() << "⏳ 等待更多数据... 当前缓冲区大小:" << buf.size();
            return;
        }

        // 提取头部
        QByteArray headerData = buf.left(headerEnd);
        QString headerStr = QString::fromUtf8(headerData);
        QStringList lines = headerStr.split("\r\n");
        if (lines.isEmpty()) {
            sendErrorResponse(socket, "Empty header");
            buf.clear();
            continue;
        }

        QStringList firstLine = lines[0].split(" ");
        if (firstLine.size() < 3) {
            sendErrorResponse(socket, "Invalid HTTP request line");
            buf.clear();
            continue;
        }

        QString method = firstLine[0];
        int contentLength = 0;
        for (const QString &line : lines) {
            if (line.startsWith("Content-Length:", Qt::CaseInsensitive)) {
                contentLength = line.mid(15).trimmed().toInt();
                break;
            }
        }

        int totalSize = headerEnd + 4 + contentLength;
        if (buf.size() < totalSize) {
            qDebug() << "⏳ 等待主体数据... 已有" << buf.size() << "/" << totalSize << "字节";
            return;
        }

        QByteArray body = buf.mid(headerEnd + 4, contentLength);

        qDebug() << "✅ 完整 HTTP 请求接收完毕，Method:" << method << ", Body大小:" << body.size();

        if (method != "POST") {
            sendErrorResponse(socket, "Only POST allowed");
            buf.remove(0, totalSize);
            continue;
        }

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(body, &error);
        if (error.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "❌ JSON 解析失败:" << error.errorString();
            sendErrorResponse(socket, "Invalid JSON");
            buf.remove(0, totalSize);
            continue;
        }

        QJsonObject request = doc.object();
        QString cmd = request.value("cmd").toString();
        QJsonObject requestData = request.value("data").toObject();

        emit logMessage(QString("收到命令：%1").arg(cmd));
        qDebug() << "🟢 命令:" << cmd << ", data:" << requestData;

        QJsonObject response;
        if (cmd == "login") {
            response = handleLogin(requestData);
        } else if (cmd == "insertUser") {
            response = handleInsertUser(requestData);
        } else if (cmd == "getUser") {
            response = handleGetUser(requestData);
        } else if (cmd == "getUserById") {
            response = handleGetUserById(requestData);
        } else if (cmd == "updateRole") {
            response = handleUpdateRole(requestData);
        } else if (cmd == "updatePassword") {
            response = handleUpdatePassword(requestData);
        } else if (cmd == "updateUsername") {
            response = handleUpdateUsername(requestData);
        } else if (cmd == "deleteUser") {
            response = handleDeleteUser(requestData);
        } else if (cmd == "existsUsername") {
            response = handleExistsUsername(requestData);
        } else if (cmd == "resetLoginState") {
            response = handleResetLoginState(requestData);
        } else if (cmd == "incrementFailCount") {
            response = handleIncrementFailCount(requestData);
        } else if (cmd == "getLockRemainingMinutes") {
            response = handleGetLockRemainingMinutes(requestData);
        } else if (cmd == "getLockRemainingSeconds") {
            response = handleGetLockRemainingSeconds(requestData);
        } else if (cmd == "validateLogin") {
            response = handleValidateLogin(requestData);
        } else if (cmd == "getStudents") {
            response = handleGetStudents(requestData);
        } else if (cmd == "selectAllStudents") {
            response = handleGetStudents(requestData);
        } else if (cmd == "getStudentById") {
            response = handleGetStudentById(requestData);
        } else if (cmd == "selectStudentById") {
            response = handleGetStudentById(requestData);
        } else if (cmd == "getStudentsByClass") {
            response = handleGetStudentsByClass(requestData);
        } else if (cmd == "selectByClass") {
            response = handleGetStudentsByClass(requestData);
        } else if (cmd == "getStudentsByName") {
            response = handleGetStudentsByName(requestData);
        } else if (cmd == "selectByName") {
            response = handleGetStudentsByName(requestData);
        } else if (cmd == "getStudentByUserId") {
            response = handleGetStudentByUserId(requestData);
        } else if (cmd == "selectByUserId") {
            response = handleGetStudentByUserId(requestData);
        } else if (cmd == "existsStudentByUserId") {
            response = handleExistsStudentByUserId(requestData);
        } else if (cmd == "existsByUserId") {
            response = handleExistsStudentByUserId(requestData);
        } else if (cmd == "insertStudent") {
            response = handleAddStudent(requestData);
        } else if (cmd == "updateStudent") {
            response = handleUpdateStudent(requestData);
        } else if (cmd == "deleteStudent") {
            response = handleDeleteStudent(requestData);
        } else if (cmd == "insertScore") {
            response = handleAddScore(requestData);
        } else if (cmd == "updateScore") {
            response = handleUpdateScore(requestData);
        } else if (cmd == "deleteScore") {
            response = handleDeleteScore(requestData);
        } else if (cmd == "selectScoresByStudent") {
            response = handleGetScores(requestData);
        } else if (cmd == "selectAverageScore") {
            response = handleSelectAverageScore(requestData);
        } else if (cmd == "getClasses") {
            response = handleGetClasses(requestData);
        } else if (cmd == "getClassRanking") {
            response = handleGetClassRanking(requestData);
        } else if (cmd == "batchImportStudents") {
            response = handleBatchImportStudents(requestData);
        } else {
            response["code"] = -2;
            response["message"] = "未知命令";
        }

        sendResponse(socket, response);

        // 移除已处理完成的请求数据，保留后续数据
        buf.remove(0, totalSize);
    }
}

void TcpServer::sendResponse(QTcpSocket *socket, const QJsonObject &response)
{
    if (!socket) {
        qDebug() << "❌ sendResponse: socket 为空";
        return;
    }

    qDebug() << "📤 [sendResponse] 响应内容:" << response;

    QByteArray json = QJsonDocument(response).toJson(QJsonDocument::Compact);
    QByteArray http = "HTTP/1.1 200 OK\r\n";
    http += "Content-Type: application/json\r\n";
    http += "Connection: keep-alive\r\n";   // 长连接
    http += "Content-Length: " + QByteArray::number(json.size()) + "\r\n";
    http += "\r\n";
    http += json;

    qDebug() << "📤 完整 HTTP 响应:\n" << http;

    qint64 written = socket->write(http);
    if (written == -1) {
        qDebug() << "❌ write 失败:" << socket->errorString();
        return;
    }
    qDebug() << "✅ 已写入" << written << "字节";

    if (!socket->flush()) {
        qDebug() << "⚠️ flush 失败";
    } else {
        qDebug() << "✅ flush 成功";
    }
}

void TcpServer::sendErrorResponse(QTcpSocket *socket, const QString &message)
{
    qDebug() << "⚠️ [sendErrorResponse] 错误信息:" << message;
    QJsonObject response;
    response["code"] = -1;
    response["message"] = message;
    sendResponse(socket, response);
}

// ============================================================
// 以下为所有命令处理函数（未改动，省略以节省篇幅，但实际文件中应保留全部）
// ============================================================
// ...（此处省略所有 handleXXX 函数，它们与之前版本完全一致）

QJsonObject TcpServer::handleLogin(const QJsonObject &data)
{
    QJsonObject response;
    QString username = data.value("username").toString();
    QString passwordHash = data.value("passwordHash").toString();

    bool success = UserDao::validateLogin(username, passwordHash);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "登录成功" : "用户名或密码错误";
    if (success) {
        User user = UserDao::selectByUsername(username);
        QJsonObject userObj;
        userObj["id"] = user.id;
        userObj["username"] = user.username;
        userObj["role"] = user.role;
        QJsonObject dataObj;
        dataObj["user"] = userObj;
        response["data"] = dataObj;
    }
    return response;
}

QJsonObject TcpServer::handleValidateLogin(const QJsonObject &data)
{
    QJsonObject response;
    QString username = data.value("username").toString();
    QString passwordHash = data.value("password_hash").toString();
    bool success = UserDao::validateLogin(username, passwordHash);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "验证通过" : "验证失败";
    return response;
}

QJsonObject TcpServer::handleInsertUser(const QJsonObject &data)
{
    QJsonObject response;
    User u;
    u.username = data.value("username").toString();
    u.passwordHash = data.value("password_hash").toString();
    u.role = data.value("role").toString();
    bool success = UserDao::insert(u);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "用户插入成功" : "用户插入失败";
    return response;
}

QJsonObject TcpServer::handleGetUser(const QJsonObject &data)
{
    QJsonObject response;
    QString username = data.value("username").toString();
    User u = UserDao::selectByUsername(username);
    if (u.id != 0) {
        QJsonObject userObj;
        userObj["id"] = u.id;
        userObj["username"] = u.username;
        userObj["password_hash"] = u.passwordHash;
        userObj["role"] = u.role;
        userObj["login_fail_count"] = u.loginFailCount;
        userObj["lock_time"] = u.lockTime;
        QJsonObject dataObj;
        dataObj["user"] = userObj;
        response["code"] = 0;
        response["data"] = dataObj;
        response["message"] = "查询成功";
    } else {
        response["code"] = -1;
        response["message"] = "用户不存在";
    }
    return response;
}

QJsonObject TcpServer::handleGetUserById(const QJsonObject &data)
{
    QJsonObject response;
    int id = data.value("id").toInt();
    User u = UserDao::selectById(id);
    if (u.id != 0) {
        QJsonObject userObj;
        userObj["id"] = u.id;
        userObj["username"] = u.username;
        userObj["password_hash"] = u.passwordHash;
        userObj["role"] = u.role;
        userObj["login_fail_count"] = u.loginFailCount;
        userObj["lock_time"] = u.lockTime;
        QJsonObject dataObj;
        dataObj["user"] = userObj;
        response["code"] = 0;
        response["data"] = dataObj;
        response["message"] = "查询成功";
    } else {
        response["code"] = -1;
        response["message"] = "用户不存在";
    }
    return response;
}

QJsonObject TcpServer::handleUpdateRole(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    QString newRole = data.value("new_role").toString();
    bool success = UserDao::updateRole(userId, newRole);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "更新角色成功" : "更新角色失败";
    return response;
}

QJsonObject TcpServer::handleUpdatePassword(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    QString newPasswordHash = data.value("password_hash").toString();

    bool success = UserDao::updatePassword(userId, newPasswordHash);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "密码修改成功" : "密码修改失败";
    return response;
}

QJsonObject TcpServer::handleUpdateUsername(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    QString newUsername = data.value("new_username").toString();

    bool success = UserDao::updateUsername(userId, newUsername);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "用户名修改成功" : "用户名修改失败";
    return response;
}

QJsonObject TcpServer::handleExistsUsername(const QJsonObject &data)
{
    QJsonObject response;
    QString username = data.value("username").toString();
    int excludeUserId = data.value("exclude_user_id").toInt();
    bool exists = UserDao::existsUsername(username, excludeUserId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["exists"] = exists;
    response["data"] = dataObj;
    response["message"] = "检查完成";
    return response;
}

QJsonObject TcpServer::handleResetLoginState(const QJsonObject &data)
{
    int userId = data.value("user_id").toInt();
    UserDao::resetLoginState(userId);
    QJsonObject response;
    response["code"] = 0;
    response["message"] = "重置成功";
    return response;
}

QJsonObject TcpServer::handleIncrementFailCount(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    bool locked = UserDao::incrementFailCount(userId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["locked"] = locked;
    response["data"] = dataObj;
    response["message"] = locked ? "已锁定" : "失败次数增加";
    return response;
}

QJsonObject TcpServer::handleGetLockRemainingMinutes(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    int minutes = UserDao::getLockRemainingMinutes(userId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["minutes"] = minutes;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleGetLockRemainingSeconds(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    int seconds = UserDao::getLockRemainingSeconds(userId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["seconds"] = seconds;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

// ===== 学生相关 =====
QJsonObject TcpServer::handleGetStudents(const QJsonObject &data)
{
    Q_UNUSED(data);
    QJsonObject response;
    QList<Student> students = StudentDao::selectAll();
    QJsonArray studentsArray;
    foreach (const Student &s, students) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["name"] = s.name;
        obj["major"] = s.major;
        obj["class_name"] = s.className;
        obj["gender"] = s.gender;
        obj["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
        obj["user_id"] = s.userId;
        studentsArray.append(obj);
    }
    QJsonObject dataObj;
    dataObj["students"] = studentsArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleGetStudentById(const QJsonObject &data)
{
    QJsonObject response;
    int id = data.value("id").toInt();
    Student s = StudentDao::selectById(id);
    if (s.id != 0) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["name"] = s.name;
        obj["major"] = s.major;
        obj["class_name"] = s.className;
        obj["gender"] = s.gender;
        obj["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
        obj["user_id"] = s.userId;
        QJsonObject dataObj;
        dataObj["student"] = obj;
        response["code"] = 0;
        response["data"] = dataObj;
        response["message"] = "查询成功";
    } else {
        response["code"] = -1;
        response["message"] = "学生不存在";
    }
    return response;
}

QJsonObject TcpServer::handleGetStudentsByClass(const QJsonObject &data)
{
    QJsonObject response;
    QString className = data.value("class_name").toString();
    QList<Student> list = StudentDao::selectByClass(className);
    QJsonArray studentsArray;
    foreach (const Student &s, list) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["name"] = s.name;
        obj["major"] = s.major;
        obj["class_name"] = s.className;
        obj["gender"] = s.gender;
        obj["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
        obj["user_id"] = s.userId;
        studentsArray.append(obj);
    }
    QJsonObject dataObj;
    dataObj["students"] = studentsArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleGetStudentsByName(const QJsonObject &data)
{
    QJsonObject response;
    QString keyword = data.value("keyword").toString();
    QList<Student> list = StudentDao::selectByName(keyword);
    QJsonArray studentsArray;
    foreach (const Student &s, list) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["name"] = s.name;
        obj["major"] = s.major;
        obj["class_name"] = s.className;
        obj["gender"] = s.gender;
        obj["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
        obj["user_id"] = s.userId;
        studentsArray.append(obj);
    }
    QJsonObject dataObj;
    dataObj["students"] = studentsArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleGetStudentByUserId(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    Student s = StudentDao::selectByUserId(userId);
    if (s.id != 0) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["name"] = s.name;
        obj["major"] = s.major;
        obj["class_name"] = s.className;
        obj["gender"] = s.gender;
        obj["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
        obj["user_id"] = s.userId;
        QJsonObject dataObj;
        dataObj["student"] = obj;
        response["code"] = 0;
        response["data"] = dataObj;
        response["message"] = "查询成功";
    } else {
        response["code"] = -1;
        response["message"] = "未找到关联的学生";
    }
    return response;
}

QJsonObject TcpServer::handleExistsStudentByUserId(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    bool exists = StudentDao::existsByUserId(userId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["exists"] = exists;
    response["data"] = dataObj;
    response["message"] = "检查完成";
    return response;
}

QJsonObject TcpServer::handleAddStudent(const QJsonObject &data)
{
    QJsonObject response;
    Student s;
    s.studentId = data.value("student_id").toString();
    s.name = data.value("name").toString();
    s.major = data.value("major").toString();
    s.className = data.value("class_name").toString();
    s.gender = data.value("gender").toString();
    s.birthDate = parseDate(data.value("birth_date").toString());
    s.userId = data.value("user_id").toInt();
    int newId = StudentDao::insert(s);
    if (newId >= 0) {
        response["code"] = 0;
        QJsonObject dataObj;
        dataObj["new_id"] = newId;
        response["data"] = dataObj;
        response["message"] = "添加成功";
    } else if (newId == -2) {
        response["code"] = -1;
        response["message"] = "学号已存在";
    } else {
        response["code"] = -1;
        response["message"] = "添加失败";
    }
    return response;
}

QJsonObject TcpServer::handleUpdateStudent(const QJsonObject &data)
{
    QJsonObject response;
    Student s;
    s.id = data.value("id").toInt();
    s.studentId = data.value("student_id").toString();
    s.name = data.value("name").toString();
    s.major = data.value("major").toString();
    s.className = data.value("class_name").toString();
    s.gender = data.value("gender").toString();
    s.birthDate = parseDate(data.value("birth_date").toString());
    s.userId = data.value("user_id").toInt();
    bool success = StudentDao::update(s);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "更新成功" : "更新失败";
    return response;
}

QJsonObject TcpServer::handleDeleteStudent(const QJsonObject &data)
{
    QJsonObject response;
    int id = data.value("id").toInt();
    // 先删除关联成绩，再删除学生，最后删除关联用户（彻底清除数据）
    QList<Score> scores = ScoreDao::selectByStudentId(id);
    for (const Score &s : scores) {
        ScoreDao::deleteById(s.id);
    }
    // 获取学生关联的用户ID（先查用户，再删学生，防止查不到）
    Student stu = StudentDao::selectById(id);
    int userId = stu.userId;
    // 删除学生
    bool success = StudentDao::deleteById(id);
    if (success && userId > 0) {
        // 删除关联用户（彻底清除，释放用户名）
        UserDao::deleteById(userId);
    }
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "删除成功（含关联用户）" : "删除失败";
    return response;
}

QJsonObject TcpServer::handleDeleteUser(const QJsonObject &data)
{
    QJsonObject response;
    int userId = data.value("user_id").toInt();
    // 先删除关联学生（包括其成绩），再删除用户
    Student stu = StudentDao::selectByUserId(userId);
    if (stu.id != 0) {
        QList<Score> scores = ScoreDao::selectByStudentId(stu.id);
        for (const Score &sc : scores) {
            ScoreDao::deleteById(sc.id);
        }
        StudentDao::deleteById(stu.id);
    }
    bool success = UserDao::deleteById(userId);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "删除成功" : "删除失败";
    return response;
}

// ===== 成绩相关 =====
QJsonObject TcpServer::handleGetScores(const QJsonObject &data)
{
    QJsonObject response;
    int studentId = data.value("student_id").toInt();
    QList<Score> scores = ScoreDao::selectByStudentId(studentId);
    QJsonArray scoresArray;
    foreach (const Score &s, scores) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["student_id"] = s.studentId;
        obj["subject"] = s.subject;
        obj["score"] = s.score;
        obj["exam_date"] = s.examDate.toString("yyyy/MM/dd");
        scoresArray.append(obj);
    }
    QJsonObject dataObj;
    dataObj["scores"] = scoresArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleAddScore(const QJsonObject &data)
{
    QJsonObject response;
    Score s;
    s.studentId = data.value("student_id").toInt();
    s.subject = data.value("subject").toString();
    s.score = data.value("score").toDouble();
    s.examDate = parseDate(data.value("exam_date").toString());

    // 检查同一学生同一科目是否已存在
    QList<Score> existingScores = ScoreDao::selectByStudentId(s.studentId);
    for (const Score &es : existingScores) {
        if (es.subject == s.subject) {
            response["code"] = -1;
            response["message"] = QString("科目\"%1\"成绩已存在，请先删除再重新录入").arg(s.subject);
            return response;
        }
    }

    bool success = ScoreDao::insert(s);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "添加成功" : "添加失败";
    return response;
}

QJsonObject TcpServer::handleUpdateScore(const QJsonObject &data)
{
    QJsonObject response;
    Score s;
    s.id = data.value("id").toInt();
    s.studentId = data.value("student_id").toInt();
    s.subject = data.value("subject").toString();
    s.score = data.value("score").toDouble();
    s.examDate = parseDate(data.value("exam_date").toString());
    bool success = ScoreDao::update(s);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "更新成功" : "更新失败";
    return response;
}

QJsonObject TcpServer::handleDeleteScore(const QJsonObject &data)
{
    QJsonObject response;
    int id = data.value("id").toInt();
    bool success = ScoreDao::deleteById(id);
    response["code"] = success ? 0 : -1;
    response["message"] = success ? "删除成功" : "删除失败";
    return response;
}

QJsonObject TcpServer::handleSelectAverageScore(const QJsonObject &data)
{
    QJsonObject response;
    int studentId = data.value("student_id").toInt();
    double avg = ScoreDao::selectAverageByStudentId(studentId);
    response["code"] = 0;
    QJsonObject dataObj;
    dataObj["average"] = avg;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

// ===== 班级统计 =====
QJsonObject TcpServer::handleGetClasses(const QJsonObject &data)
{
    Q_UNUSED(data);
    QJsonObject response;
    QStringList classes = BusinessLogic::getAllClasses();
    QJsonArray classesArray;
    foreach (const QString &c, classes) {
        classesArray.append(c);
    }
    QJsonObject dataObj;
    dataObj["classes"] = classesArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

QJsonObject TcpServer::handleGetClassRanking(const QJsonObject &data)
{
    QJsonObject response;
    QString className = data.value("class_name").toString();
    QList<Student> ranking = BusinessLogic::getClassRanking(className);
    QJsonArray rankingArray;
    foreach (const Student &s, ranking) {
        QJsonObject obj;
        obj["id"] = s.id;
        obj["name"] = s.name;
        obj["class_name"] = s.className;
        obj["averageScore"] = BusinessLogic::getStudentAverageScore(s.id);
        rankingArray.append(obj);
    }
    QJsonObject dataObj;
    dataObj["ranking"] = rankingArray;
    response["code"] = 0;
    response["data"] = dataObj;
    response["message"] = "查询成功";
    return response;
}

// ============================================================
// 批量导入学生（含用户创建、成绩处理），使用事务提高效率
// ============================================================
QJsonObject TcpServer::handleBatchImportStudents(const QJsonObject &data)
{
    QJsonObject response;
    QJsonArray studentsArray = data.value("students").toArray();
    if (studentsArray.isEmpty()) {
        response["code"] = -1;
        response["message"] = "没有可导入的数据";
        return response;
    }

    QSqlDatabase db = DatabaseManager::getDatabase();
    bool inTransaction = false;
    int successCount = 0;
    int userCreated = 0;
    int scoreCount = 0;
    QStringList createdAccounts;
    QStringList failedRows;

    // 先查询所有现有学号，用于快速判断重复
    QMap<QString, int> existingStudentIdMap;
    {
        QSqlQuery q("SELECT student_id, id FROM students WHERE student_id IS NOT NULL AND student_id != ''");
        while (q.next()) {
            existingStudentIdMap[q.value(0).toString()] = q.value(1).toInt();
        }
    }

    for (int idx = 0; idx < studentsArray.size(); ++idx) {
        QJsonObject stuObj = studentsArray[idx].toObject();
        QString studentId = stuObj.value("student_id").toString();
        QString name = stuObj.value("name").toString();
        QString major = stuObj.value("major").toString();
        QString className = stuObj.value("class_name").toString();
        QString gender = stuObj.value("gender").toString();
        QString birthDate = stuObj.value("birth_date").toString();
        int userId = stuObj.value("user_id").toInt();
        QString loginUsername = stuObj.value("login_username").toString();
        QJsonArray scoresArray = stuObj.value("scores").toArray();

        int actualLine = idx + 2; // 行号（从第2行开始，第1行是表头）

        if (name.isEmpty()) {
            failedRows << QString("第%1行: 姓名为空，跳过").arg(actualLine);
            continue;
        }

        // 检查学号重复
        if (!studentId.isEmpty() && existingStudentIdMap.contains(studentId)) {
            failedRows << QString("第%1行: 学号\"%2\"已存在，跳过").arg(actualLine).arg(studentId);
            continue;
        }

        // ---------- 事务：每100条提交一次 ----------
        if (!inTransaction) {
            db.transaction();
            inTransaction = true;
        }

        // ---------- 处理用户 ----------
        int targetUserId = userId;

        // 自动生成用户名：如果未提供用户名，使用学号作为用户名（初始密码123456）
        if (loginUsername.isEmpty() && targetUserId == 0 && !studentId.isEmpty()) {
            loginUsername = studentId;
        }
        if (!loginUsername.isEmpty()) {
            // 检查用户名是否已存在
            {
                QSqlQuery uq;
                uq.prepare("SELECT id, role FROM users WHERE username = :username");
                uq.bindValue(":username", loginUsername);
                if (uq.exec() && uq.next()) {
                    int existingUserId = uq.value(0).toInt();
                    QString existingRole = uq.value(1).toString();
                    if (existingRole == "student") {
                        // 检查该用户是否已被其他学生关联
                        QSqlQuery eq;
                        eq.prepare("SELECT COUNT(*) FROM students WHERE user_id = :uid");
                        eq.bindValue(":uid", existingUserId);
                        if (eq.exec() && eq.next() && eq.value(0).toInt() == 0) {
                            targetUserId = existingUserId;
                        } else {
                            // 用户名冲突，生成新用户名
                            loginUsername = loginUsername + "_dup";
                            // 递归查找可用用户名
                            QSqlQuery cq;
                            cq.prepare("SELECT COUNT(*) FROM users WHERE username = :un");
                            cq.bindValue(":un", loginUsername);
                            int suffix = 1;
                            while (cq.exec() && cq.next() && cq.value(0).toInt() > 0) {
                                suffix++;
                                loginUsername = loginUsername.left(loginUsername.lastIndexOf('_')) + "_" + QString::number(suffix);
                                cq.prepare("SELECT COUNT(*) FROM users WHERE username = :un");
                                cq.bindValue(":un", loginUsername);
                            }
                        }
                    } else {
                        // 非学生角色占用，生成新用户名
                        int suffix = 1;
                        QString base = loginUsername;
                        loginUsername = base + "_" + QString::number(suffix);
                        QSqlQuery cq;
                        cq.prepare("SELECT COUNT(*) FROM users WHERE username = :un");
                        cq.bindValue(":un", loginUsername);
                        while (cq.exec() && cq.next() && cq.value(0).toInt() > 0) {
                            suffix++;
                            loginUsername = base + "_" + QString::number(suffix);
                            cq.prepare("SELECT COUNT(*) FROM users WHERE username = :un");
                            cq.bindValue(":un", loginUsername);
                        }
                    }
                }
            }

            // 创建新用户（如果尚未找到可用用户ID）
            if (targetUserId == userId || targetUserId == 0) {
                QString password = "123456";
                QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
                QSqlQuery iq;
                iq.prepare("INSERT INTO users (username, password_hash, role) VALUES (:un, :ph, 'student')");
                iq.bindValue(":un", loginUsername);
                iq.bindValue(":ph", hash.toHex());
                if (iq.exec()) {
                    targetUserId = iq.lastInsertId().toInt();
                    userCreated++;
                    createdAccounts << QString("%1 -> %2").arg(name).arg(loginUsername);
                } else {
                    failedRows << QString("第%1行: 创建用户失败").arg(actualLine);
                    continue;
                }
            }
        } else if (targetUserId > 0) {
            // 检查用户ID是否存在且是学生角色
            QSqlQuery uq;
            uq.prepare("SELECT id, role FROM users WHERE id = :uid");
            uq.bindValue(":uid", targetUserId);
            if (!(uq.exec() && uq.next() && uq.value(1).toString() == "student")) {
                targetUserId = 0;
            } else {
                // 检查是否已被其他学生关联
                QSqlQuery eq;
                eq.prepare("SELECT COUNT(*) FROM students WHERE user_id = :uid");
                eq.bindValue(":uid", targetUserId);
                if (eq.exec() && eq.next() && eq.value(0).toInt() > 0) {
                    targetUserId = 0;
                }
            }
        }

        // ---------- 插入学生 ----------
        QSqlQuery sq;
        sq.prepare("INSERT INTO students (student_id, name, major, class_name, gender, birth_date, user_id) "
                    "VALUES (:sid, :name, :major, :class, :gender, :bd, :uid)");
        sq.bindValue(":sid", studentId);
        sq.bindValue(":name", name);
        sq.bindValue(":major", major);
        sq.bindValue(":class", className);
        sq.bindValue(":gender", gender);
        sq.bindValue(":bd", birthDate.isEmpty() ? QVariant(QVariant::String) : birthDate);
        sq.bindValue(":uid", targetUserId > 0 ? targetUserId : QVariant(QVariant::Int));

        if (!sq.exec()) {
            failedRows << QString("第%1行: 插入学生失败 - %2").arg(actualLine).arg(sq.lastError().text());
            continue;
        }

        int newStudentId = sq.lastInsertId().toInt();
        existingStudentIdMap[studentId] = newStudentId;

        // ---------- 插入成绩 ----------
        int insertedScores = 0;
        for (int si = 0; si < scoresArray.size(); ++si) {
            QJsonObject scObj = scoresArray[si].toObject();
            QString subject = scObj.value("subject").toString();
            double score = scObj.value("score").toDouble();
            QString examDate = scObj.value("exam_date").toString();

            if (subject.isEmpty()) continue;

            QSqlQuery scq;
            scq.prepare("INSERT INTO scores (student_id, subject, score, exam_date) "
                        "VALUES (:sid, :subj, :score, :ed)");
            scq.bindValue(":sid", newStudentId);
            scq.bindValue(":subj", subject);
            scq.bindValue(":score", score);
            scq.bindValue(":ed", examDate.isEmpty() ? QDate::currentDate().toString("yyyy/MM/dd") : examDate);
            if (scq.exec()) {
                insertedScores++;
            }
        }
        scoreCount += insertedScores;
        successCount++;

        // ---------- 每100条提交一次事务 ----------
        if (successCount % 100 == 0) {
            db.commit();
            inTransaction = false;
        }
    }

    // 提交剩余事务
    if (inTransaction) {
        db.commit();
    }

    QJsonObject resultData;
    resultData["student_count"] = successCount;
    resultData["user_created"] = userCreated;
    resultData["score_count"] = scoreCount;
    QJsonArray accountsArray;
    for (const QString &acc : createdAccounts) {
        accountsArray.append(acc);
    }
    resultData["created_accounts"] = accountsArray;
    QJsonArray failedArray;
    for (const QString &fr : failedRows) {
        failedArray.append(fr);
    }
    resultData["failed_rows"] = failedArray;
    resultData["total"] = studentsArray.size();

    response["code"] = 0;
    response["data"] = resultData;
    response["message"] = QString("导入完成：成功%1个学生，%2门成绩，%3个新账号，%4个失败")
                             .arg(successCount).arg(scoreCount).arg(userCreated).arg(failedRows.size());
    return response;
}
