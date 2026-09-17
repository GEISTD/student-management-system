#include <QApplication>
#include "UIManager.h"
#include "userdao.h"
#include "NetworkManager.h"   // 新增
#include <QDebug>
#include <QDir>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StyleHelper::instance().applyGlobalStyle();

    // 设置服务器地址（请根据实际部署修改 IP 和端口）
    NetworkManager::instance().setServerUrl("http://127.0.0.1:8888/api");

    // 不再初始化本地数据库
    // if (!DatabaseManager::initDatabase()) { ... }

    MainWidget w;

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        qDebug() << "❌ 用户取消登录";
        return 0;
    }

    QString username = login.getUsername();
    User user = UserDao::selectByUsername(username);

    w.setCurrentUser(user.id, user.username, user.role);
    w.show();

    return a.exec();
}
