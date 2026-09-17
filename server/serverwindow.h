#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QTableWidgetItem>
#include "tcpserver.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

private slots:
    void on_startServerBtn_clicked();
    void on_stopServerBtn_clicked();
    void on_clearLogBtn_clicked();

    void onClientConnected(int clientId, const QString &ip, const QString &time);
    void onClientDisconnected(int clientId);
    void onLogReceived(const QString &log);
    void onDataReceived(int clientId, const QString &data);

private:
    Ui::ServerWindow *ui;
    TcpServer *m_tcpServer;

    void appendLog(const QString &log);
    void updateClientTable(int clientId, const QString &ip, const QString &time, const QString &status);
    void removeClientFromTable(int clientId);
};

#endif
