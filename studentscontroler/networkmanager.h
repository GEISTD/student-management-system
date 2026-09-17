#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrl>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    static NetworkManager& instance();
    void setServerUrl(const QString &url);
    QString serverUrl() const;
    QJsonObject sendRequest(const QString &cmd, const QJsonObject &data, int timeoutMs = 5000);

signals:
    void serverShutdown(const QString &message);

private:
    NetworkManager();
    QNetworkAccessManager *m_nam;
    QString m_serverUrl;
};

#endif // NETWORKMANAGER_H
