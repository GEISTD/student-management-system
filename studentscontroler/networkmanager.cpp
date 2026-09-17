#include "NetworkManager.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QTimer>
#include <QDebug>
#include <QNetworkProxy>
#include <QNetworkReply>

NetworkManager& NetworkManager::instance()
{
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager() : m_nam(new QNetworkAccessManager(this)) {
    m_nam->setProxy(QNetworkProxy::NoProxy);   // 禁用代理，避免拦截本地连接

}

void NetworkManager::setServerUrl(const QString &url)
{
    m_serverUrl = url;
}

QString NetworkManager::serverUrl() const
{
    return m_serverUrl;
}

QJsonObject NetworkManager::sendRequest(const QString &cmd, const QJsonObject &data, int timeoutMs)
{
    if (m_serverUrl.isEmpty()) {
        qWarning() << "服务器地址未设置";
        return QJsonObject{{"code", -1}, {"message", "服务器地址未配置"}};
    }

    QUrl url(m_serverUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestObj;
    requestObj["cmd"] = cmd;
    requestObj["data"] = data;

    // ========== 添加日志：发送请求 ==========
    qDebug() << "📤 发送请求到:" << m_serverUrl;
    qDebug() << "📤 请求 JSON:" << QJsonDocument(requestObj).toJson(QJsonDocument::Compact);

    QByteArray jsonData = QJsonDocument(requestObj).toJson();
    QNetworkReply *reply = m_nam->post(request, jsonData);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    QJsonObject response;
    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            // ========== 添加日志：收到响应 ==========
            qDebug() << "📥 收到响应（原始）:" << data;
            qDebug() << "📥 HTTP 状态码:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(data, &err);
            if (err.error == QJsonParseError::NoError && doc.isObject()) {
                response = doc.object();
                // 检查是否为服务器关闭通知
                if (response.value("cmd").toString() == "server_shutdown") {
                    QString msg = response.value("message").toString("服务器已停止");
                    emit serverShutdown(msg);
                }
            } else {
                response = QJsonObject{{"code", -1}, {"message", "响应解析失败"}};
            }
        } else {
            qDebug() << "❌ 网络错误:" << reply->errorString();
            response = QJsonObject{{"code", -1}, {"message", reply->errorString()}};
            // 检测连接断开错误（服务器关闭）
            QNetworkReply::NetworkError errCode = reply->error();
            if (errCode == QNetworkReply::ConnectionRefusedError ||
                errCode == QNetworkReply::RemoteHostClosedError ||
                errCode == QNetworkReply::HostNotFoundError) {
                emit serverShutdown("服务器已断开连接");
            }
        }
    } else {
        qDebug() << "⏰ 请求超时";
        response = QJsonObject{{"code", -1}, {"message", "请求超时"}};
        reply->abort();
    }
    reply->deleteLater();

    qDebug() << "📦 解析后的响应:" << response;
    return response;
}
