#include <QApplication>
#include "serverwindow.h"
#include "databasemanager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::initDatabase()) {
        qDebug() << "数据库初始化失败，程序退出";
        return -1;
    }

    ServerWindow w;
    w.show();

    return a.exec();
}
