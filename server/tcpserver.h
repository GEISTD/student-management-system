#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    bool startServer(quint16 port);
    void stopServer();
    bool isRunning() const;

signals:
    void clientConnected(int clientId, const QString &ip, const QString &time);
    void clientDisconnected(int clientId);
    void logMessage(const QString &log);
    void dataReceived(int clientId, const QString &data);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *m_server;
    QMap<QTcpSocket*, QByteArray> m_buffer;
    int m_nextClientId;   // 用于生成唯一客户端ID

    // 所有命令处理函数声明（返回统一 JSON 格式）
    QJsonObject handleLogin(const QJsonObject &data);
    QJsonObject handleValidateLogin(const QJsonObject &data);
    QJsonObject handleInsertUser(const QJsonObject &data);
    QJsonObject handleGetUser(const QJsonObject &data);
    QJsonObject handleGetUserById(const QJsonObject &data);
    QJsonObject handleUpdateRole(const QJsonObject &data);
    QJsonObject handleUpdatePassword(const QJsonObject &data);
    QJsonObject handleUpdateUsername(const QJsonObject &data);
    QJsonObject handleExistsUsername(const QJsonObject &data);
    QJsonObject handleResetLoginState(const QJsonObject &data);
    QJsonObject handleIncrementFailCount(const QJsonObject &data);
    QJsonObject handleGetLockRemainingMinutes(const QJsonObject &data);
    QJsonObject handleGetLockRemainingSeconds(const QJsonObject &data);

    QJsonObject handleGetStudents(const QJsonObject &data);
    QJsonObject handleGetStudentById(const QJsonObject &data);
    QJsonObject handleGetStudentsByClass(const QJsonObject &data);
    QJsonObject handleGetStudentsByName(const QJsonObject &data);
    QJsonObject handleGetStudentByUserId(const QJsonObject &data);
    QJsonObject handleExistsStudentByUserId(const QJsonObject &data);
    QJsonObject handleAddStudent(const QJsonObject &data);
    QJsonObject handleUpdateStudent(const QJsonObject &data);
    QJsonObject handleDeleteStudent(const QJsonObject &data);


    QJsonObject handleGetScores(const QJsonObject &data);
    QJsonObject handleAddScore(const QJsonObject &data);
    QJsonObject handleUpdateScore(const QJsonObject &data);
    QJsonObject handleDeleteScore(const QJsonObject &data);
    QJsonObject handleSelectAverageScore(const QJsonObject &data);

    QJsonObject handleGetClasses(const QJsonObject &data);
    QJsonObject handleGetClassRanking(const QJsonObject &data);

    // 批量导入
    QJsonObject handleBatchImportStudents(const QJsonObject &data);

    void sendResponse(QTcpSocket *socket, const QJsonObject &response);
    void sendErrorResponse(QTcpSocket *socket, const QString &message);

    // 删除用户命令
    QJsonObject handleDeleteUser(const QJsonObject &data);
};

#endif // TCPSERVER_H
