#include "fileexpoter.h"
#include "studentdao.h"
#include "userdao.h"
#include "scoredao.h"
#include "NetworkManager.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>
#include <QtGlobal>
#include <QJsonArray>
#include <QJsonObject>

// 解析日期，支持多种常见格式
static bool parseDate(const QString &str, QDate &outDate) {
    if (str.trimmed().isEmpty()) {
        outDate = QDate();
        return false;
    }
    // 按优先级尝试各种格式
    static const QStringList formats = {
        "yyyy-MM-dd",   // 2026-07-20
        "yyyy/MM/dd",   // 2026/07/20
        "yyyy-M-d",     // 2026-7-20
        "yyyy/M/d",     // 2026/7/20
        "dd-MM-yyyy",   // 20-07-2026
        "dd/MM/yyyy",   // 20/07/2026
        "M/d/yyyy",     // 7/20/2026
        "yyyy年MM月dd日" // 2026年07月20日
    };
    for (const QString &fmt : formats) {
        QDate d = QDate::fromString(str.trimmed(), fmt);
        if (d.isValid()) {
            outDate = d;
            return true;
        }
    }
    outDate = QDate();
    return false;
}

// ============================================================
// 导出全部学生到 CSV
// ============================================================
bool FileExporter::exportAllToCsv(const QString &filePath)
{
    QList<Student> list = StudentDao::selectAll();
    if (list.isEmpty()) {
        qDebug() << "⚠️ 没有数据可导出";
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "❌ 无法创建文件:" << filePath;
        return false;
    }

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    if (!codec) {
        qDebug() << "❌ 无法获取 UTF-8 编码器";
        file.close();
        return false;
    }

    file.write("\xEF\xBB\xBF"); // UTF-8 BOM

    QString header = "ID,学号,姓名,专业,班级,性别,出生日期,用户ID,用户名";
    // 查找所有科目
    QStringList allSubjects;
    QMap<int, QList<Score>> scoresMap;
    for (const Student &s : list) {
        QList<Score> scores = ScoreDao::selectByStudentId(s.id);
        scoresMap[s.id] = scores;
        for (const Score &sc : scores) {
            if (!allSubjects.contains(sc.subject)) {
                allSubjects.append(sc.subject);
            }
        }
    }
    for (const QString &subj : allSubjects) {
        header += "," + subj + ",分数";
    }
    header += "\n";
    file.write(codec->fromUnicode(header));

    // 缓存用户名，避免重复查询
    QMap<int, QString> usernameCache;
    auto getUsername = [&](int userId) -> QString {
        if (userId == 0) return "";
        if (usernameCache.contains(userId)) return usernameCache[userId];
        User u = UserDao::selectById(userId);
        QString name = u.username;
        usernameCache[userId] = name;
        return name;
    };

    for (const Student &s : list) {
        auto escapeCsv = [](const QString &field) -> QString {
            if (field.contains(',') || field.contains('"') || field.contains('\n')) {
                QString escaped = field;
                escaped.replace("\"", "\"\"");
                return "\"" + escaped + "\"";
            }
            return field;
        };
        QString line = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
                       .arg(s.id)
                       .arg(escapeCsv(s.studentId))
                       .arg(escapeCsv(s.name))
                       .arg(escapeCsv(s.major))
                       .arg(escapeCsv(s.className))
                       .arg(escapeCsv(s.gender))
                       .arg(escapeCsv(s.birthDate.toString("yyyy/MM/dd")))
                       .arg(s.userId)
                       .arg(escapeCsv(getUsername(s.userId)));

        // 添加成绩数据
        QList<Score> studentScores = scoresMap.value(s.id);
        for (const QString &subj : allSubjects) {
            bool found = false;
            for (const Score &sc : studentScores) {
                if (sc.subject == subj) {
                    line += QString(",%1,%2").arg(escapeCsv(subj)).arg(sc.score, 0, 'f', 1);
                    found = true;
                    break;
                }
            }
            if (!found) {
                line += "," + escapeCsv(subj) + ",";
            }
        }
        line += "\n";
        file.write(codec->fromUnicode(line));
    }

    file.close();
    qDebug() << "✅ 导出成功:" << filePath << ", 共" << list.size() << "条";
    return true;
}

// ============================================================
// 生成带时间戳的默认文件名
// ============================================================
QString FileExporter::generateDefaultFileName(const QString &prefix)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return prefix + "_" + timestamp + ".csv";
}

// ============================================================
// 正确的 CSV 行解析，支持双引号转义
// ============================================================
static QStringList parseCsvLine(const QString &line)
{
    QStringList fields;
    QString field;
    bool inQuotes = false;
    for (int i = 0; i < line.size(); ++i) {
        QChar c = line[i];
        if (inQuotes) {
            if (c == '"') {
                // 检查是否是转义的双引号（"")
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i; // 跳过下一个引号
                } else {
                    inQuotes = false; // 结束引号
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inQuotes = true; // 开始引号
            } else if (c == ',') {
                fields.append(field.trimmed());
                field.clear();
            } else {
                field += c;
            }
        }
    }
    fields.append(field.trimmed());
    return fields;
}

// ============================================================
// 从 CSV 文件读取学生数据（支持第7列：用户名；第10列开始：科目,分数）
// ============================================================
static ImportStudentData parseImportFields(const QStringList &fields, int lineNumber, const QString &filePath);

QList<ImportStudentData> FileExporter::importFromCsv(const QString &filePath)
{
    QList<ImportStudentData> dataList;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "❌ 无法打开文件:" << filePath;
        return dataList;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    if (in.atEnd()) {
        qDebug() << "⚠️ 文件为空";
        file.close();
        return dataList;
    }

    QString firstLine = in.readLine();

    if (firstLine.startsWith("\xEF\xBB\xBF")) {
        firstLine = firstLine.mid(3);
        qDebug() << "📋 检测到 UTF-8 BOM，已移除";
    }

    // 校验表头是否包含预期关键词
    QStringList headerFields = parseCsvLine(firstLine);
    bool hasValidHeader = false;
    for (const QString &field : headerFields) {
        if (field.contains("学号") || field.contains("姓名") || field.contains("ID")) {
            hasValidHeader = true;
            break;
        }
    }

    qDebug() << "📋" << (hasValidHeader ? "已跳过表头" : "未检测到表头，第一行作为数据处理");

    int lineNumber = 0;
    // 如果表头不匹配，先处理第一行
    if (!hasValidHeader) {
        // 直接处理 firstLine，当做数据行
        QStringList fields = parseCsvLine(firstLine);
        ImportStudentData data = parseImportFields(fields, 1, filePath);
        if (data.student.id >= 0) dataList.append(data);
        lineNumber = 1;
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        if (line.trimmed().isEmpty()) continue;

        QStringList fields = parseCsvLine(line);
        ImportStudentData data = parseImportFields(fields, lineNumber, filePath);
        if (data.student.id >= 0) dataList.append(data);
    }

    file.close();
    return dataList;
}

// ============================================================
// 解析导入行字段
// ============================================================
static ImportStudentData parseImportFields(const QStringList &fields, int lineNumber, const QString &filePath)
{
    ImportStudentData data;
    Student &s = data.student;
    s.id = 0;
    Q_UNUSED(filePath);

    if (fields.size() < 6) {
        qDebug() << "⚠️ 第" << lineNumber << "行格式错误（至少6列），跳过";
        s.id = -1; // 标记为无效
        return data;
    }

    // ===== 根据列数判断格式 =====
    if (fields.size() >= 8) {
        // 新格式：ID,学号,姓名,专业,班级,性别,出生日期,用户ID,用户名(可选),科目1,分数1,科目2,分数2,...
        s.studentId = fields[1].trimmed();
        s.name = fields[2].trimmed();
        s.major = fields[3].trimmed();
        s.className = fields[4].trimmed();
        s.gender = fields[5].trimmed();

        QString dateStr = fields[6].trimmed();
        if (dateStr.isEmpty()) {
            s.birthDate = QDate();
        } else {
            if (!parseDate(dateStr, s.birthDate)) {
                data.errorMsg = QString("第%1行日期格式错误: %2").arg(lineNumber).arg(dateStr);
                s.birthDate = QDate();
            }
        }

        s.userId = fields[7].trimmed().toInt();

        if (fields.size() >= 9) {
            data.loginUsername = fields[8].trimmed();
        } else {
            data.loginUsername.clear();
        }

        // ===== 解析成绩（从第10列开始，每两列一组：科目,分数） =====
        for (int i = 9; i + 1 < fields.size(); i += 2) {
            QString subject = fields[i].trimmed();
            QString scoreStr = fields[i+1].trimmed();
            if (subject.isEmpty() || scoreStr.isEmpty()) continue;
            bool ok;
            double score = scoreStr.toDouble(&ok);
            if (!ok) {
                qDebug() << "⚠️ 第" << lineNumber << "行成绩格式错误:" << subject << "->" << scoreStr << "，跳过该成绩";
                continue;
            }
            Score sc;
            sc.subject = subject;
            sc.score = score;
            sc.examDate = QDate::currentDate();  // 考试日期默认为今天
            data.scores.append(sc);
        }

    } else {
        // 旧格式：ID,姓名,班级,性别,出生日期,用户ID,用户名(可选)
        s.name = fields[1].trimmed();
        s.className = fields[2].trimmed();
        s.gender = fields[3].trimmed();

        QString dateStr = fields[4].trimmed();
        if (dateStr.isEmpty()) {
            s.birthDate = QDate();
        } else {
            if (!parseDate(dateStr, s.birthDate)) {
                data.errorMsg = QString("第%1行日期格式错误: %2").arg(lineNumber).arg(dateStr);
                s.birthDate = QDate();
            }
        }

        s.userId = fields[5].trimmed().toInt();

        if (fields.size() >= 7) {
            data.loginUsername = fields[6].trimmed();
        } else {
            data.loginUsername.clear();
        }
        // 旧格式无成绩列，留空
    }

    if (s.name.isEmpty()) {
        qDebug() << "⚠️ 第" << lineNumber << "行姓名为空，跳过";
        s.id = -1; // 标记为无效
        return data;
    }

    return data;
}

// ============================================================
// 从 CSV 文件读取并批量插入/更新学生及成绩（使用服务端批量接口）
// ============================================================
ImportResult FileExporter::importStudentsFromCsv(const QString &filePath)
{
    ImportResult result;
    QList<ImportStudentData> dataList = importFromCsv(filePath);
    if (dataList.isEmpty()) {
        qDebug() << "⚠️ 没有可导入的数据";
        result.failedRows << "文件无有效数据";
        return result;
    }

    // 将所有数据打包为 JSON 数组，一次性发送到服务端
    QJsonArray studentsArray;
    for (int idx = 0; idx < dataList.size(); ++idx) {
        const ImportStudentData &data = dataList[idx];
        const Student &s = data.student;

        // 收集日期错误信息（如果存在）
        if (!data.errorMsg.isEmpty()) {
            result.failedRows << data.errorMsg;
        }

        QJsonObject stuObj;
        stuObj["student_id"] = s.studentId;
        stuObj["name"] = s.name;
        stuObj["major"] = s.major;
        stuObj["class_name"] = s.className;
        stuObj["gender"] = s.gender;
        stuObj["birth_date"] = s.birthDate.isValid() ? s.birthDate.toString("yyyy/MM/dd") : QString();
        stuObj["user_id"] = s.userId;
        stuObj["login_username"] = data.loginUsername;

        // 打包成绩
        QJsonArray scoresArray;
        for (const Score &sc : data.scores) {
            QJsonObject scObj;
            scObj["subject"] = sc.subject;
            scObj["score"] = sc.score;
            scObj["exam_date"] = sc.examDate.isValid() ? sc.examDate.toString("yyyy/MM/dd") : QString();
            scoresArray.append(scObj);
        }
        stuObj["scores"] = scoresArray;

        studentsArray.append(stuObj);
    }

    QJsonObject batchData;
    batchData["students"] = studentsArray;

    // 发送批量导入请求（一次性，减少网络开销）
    qDebug() << "📤 发送批量导入请求，共" << dataList.size() << "条学生数据";
    QJsonObject resp = NetworkManager::instance().sendRequest("batchImportStudents", batchData, 60000);

    if (resp.value("code").toInt() != 0) {
        qDebug() << "❌ 批量导入失败:" << resp.value("message").toString();
        result.failedRows << "批量导入请求失败: " + resp.value("message").toString();
        return result;
    }

    QJsonObject respData = resp.value("data").toObject();
    result.studentCount = respData.value("student_count").toInt();
    result.userCreated = respData.value("user_created").toInt();
    result.scoreCount = respData.value("score_count").toInt();

    QJsonArray accountsArray = respData.value("created_accounts").toArray();
    for (const QJsonValue &v : accountsArray) {
        result.createdAccounts.append(v.toString());
    }

    QJsonArray failedArray = respData.value("failed_rows").toArray();
    for (const QJsonValue &v : failedArray) {
        result.failedRows.append(v.toString());
    }

    qDebug() << "✅ 批量导入完成，成功" << result.studentCount << "个学生，"
             << result.scoreCount << "门成绩，"
             << result.userCreated << "个新账号，"
             << result.failedRows.size() << "个失败";
    return result;
}
