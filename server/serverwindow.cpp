#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QDateTime>
#include <QTableWidgetItem>
#include <QHeaderView>

ServerWindow::ServerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);

    setFont(QFont("Microsoft YaHei", 14));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(700, 500);

    // 日志自动滚动到最新行
    connect(ui->logTextEdit, &QTextEdit::textChanged, this, [this]() {
        QTextCursor cursor = ui->logTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->logTextEdit->setTextCursor(cursor);
    });

    ui->clientTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->clientTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->clientTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->clientTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->clientTable->verticalHeader()->setVisible(false);
    ui->clientTable->setAlternatingRowColors(true);
    ui->clientTable->setSelectionBehavior(QTableWidget::SelectRows);

    m_tcpServer = new TcpServer(this);

    connect(m_tcpServer, &TcpServer::logMessage, this, &ServerWindow::onLogReceived);
    connect(m_tcpServer, &TcpServer::clientConnected, this, &ServerWindow::onClientConnected);
    connect(m_tcpServer, &TcpServer::clientDisconnected, this, &ServerWindow::onClientDisconnected);
}
ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::on_startServerBtn_clicked()
{
    quint16 port = static_cast<quint16>(ui->portSpinBox->value());

    if (m_tcpServer->startServer(port)) {
        ui->startServerBtn->setEnabled(false);
        ui->stopServerBtn->setEnabled(true);
        ui->portSpinBox->setEnabled(false);
        ui->statusLabel->setText("状态：运行中");
        ui->statusLabel->setStyleSheet("color: green; font-weight: bold;");
    }
}

void ServerWindow::on_stopServerBtn_clicked()
{
    m_tcpServer->stopServer();
    ui->startServerBtn->setEnabled(true);
    ui->stopServerBtn->setEnabled(false);
    ui->portSpinBox->setEnabled(true);
    ui->statusLabel->setText("状态：未启动");
    ui->statusLabel->setStyleSheet("color: red; font-weight: bold;");
}

void ServerWindow::on_clearLogBtn_clicked()
{
    ui->logTextEdit->clear();
}

void ServerWindow::onClientConnected(int clientId, const QString &ip, const QString &time)
{
    updateClientTable(clientId, ip, time, "已连接");
}

void ServerWindow::onClientDisconnected(int clientId)
{
    removeClientFromTable(clientId);
}

void ServerWindow::onLogReceived(const QString &log)
{
    appendLog(log);
}

void ServerWindow::onDataReceived(int clientId, const QString &data)
{
    Q_UNUSED(clientId);
    Q_UNUSED(data);
}

void ServerWindow::appendLog(const QString &log)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] %2").arg(time).arg(log));
}

void ServerWindow::updateClientTable(int clientId, const QString &ip, const QString &time, const QString &status)
{
    int row = ui->clientTable->rowCount();
    ui->clientTable->insertRow(row);

    ui->clientTable->setItem(row, 0, new QTableWidgetItem(QString::number(clientId)));
    ui->clientTable->setItem(row, 1, new QTableWidgetItem(ip));
    ui->clientTable->setItem(row, 2, new QTableWidgetItem(time));
    ui->clientTable->setItem(row, 3, new QTableWidgetItem(status));
}

void ServerWindow::removeClientFromTable(int clientId)
{
    for (int i = 0; i < ui->clientTable->rowCount(); ++i) {
        if (ui->clientTable->item(i, 0)->text().toInt() == clientId) {
            ui->clientTable->removeRow(i);
            break;
        }
    }
}
