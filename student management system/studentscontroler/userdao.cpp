#include "userdao.h"
#include "NetworkManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

// ============================================================
// 插入新用户
// ============================================================
bool UserDao::insert(const User &u)
{
    QJsonObject data;
    data["username"] = u.username;
    data["password_hash"] = u.passwordHash;
    data["role"] = u.role;
    QJsonObject resp = NetworkManager::instance().sendRequest("insertUser", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 按用户名查询用户
// ============================================================
User UserDao::selectByUsername(const QString &username)
{
    QJsonObject data;
    data["username"] = username;
    QJsonObject resp = NetworkManager::instance().sendRequest("getUser", data);
    User u;
    if (resp.value("code").toInt() == 0) {
        QJsonObject userObj = resp.value("data").toObject().value("user").toObject();
        u.id = userObj.value("id").toInt();
        u.username = userObj.value("username").toString();
        u.passwordHash = userObj.value("password_hash").toString();
        u.role = userObj.value("role").toString();
        u.loginFailCount = userObj.value("login_fail_count").toInt();
        u.lockTime = userObj.value("lock_time").toString();
    }
    return u;
}

// ============================================================
// 更新用户角色
// ============================================================
bool UserDao::updateRole(int userId, const QString &newRole)
{
    QJsonObject data;
    data["user_id"] = userId;
    data["new_role"] = newRole;
    QJsonObject resp = NetworkManager::instance().sendRequest("updateRole", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 按ID查询用户
// ============================================================
User UserDao::selectById(int id)
{
    QJsonObject data;
    data["id"] = id;
    QJsonObject resp = NetworkManager::instance().sendRequest("getUserById", data);
    User u;
    if (resp.value("code").toInt() == 0) {
        QJsonObject userObj = resp.value("data").toObject().value("user").toObject();
        u.id = userObj.value("id").toInt();
        u.username = userObj.value("username").toString();
        u.passwordHash = userObj.value("password_hash").toString();
        u.role = userObj.value("role").toString();
        u.loginFailCount = userObj.value("login_fail_count").toInt();
        u.lockTime = userObj.value("lock_time").toString();
    }
    return u;
}

// ============================================================
// 修改密码
// ============================================================
bool UserDao::updatePassword(int userId, const QString &newPasswordHash)
{
    QJsonObject data;
    data["user_id"] = userId;
    data["password_hash"] = newPasswordHash;
    QJsonObject resp = NetworkManager::instance().sendRequest("updatePassword", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 修改用户名
// ============================================================
bool UserDao::updateUsername(int userId, const QString &newUsername)
{
    QJsonObject data;
    data["user_id"] = userId;
    data["new_username"] = newUsername;
    QJsonObject resp = NetworkManager::instance().sendRequest("updateUsername", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 检查用户名是否存在（排除指定用户ID）
// ============================================================
bool UserDao::existsUsername(const QString &username, int excludeUserId)
{
    QJsonObject data;
    data["username"] = username;
    data["exclude_user_id"] = excludeUserId;
    QJsonObject resp = NetworkManager::instance().sendRequest("existsUsername", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("exists").toBool();
    }
    return false;
}

// ============================================================
// 重置失败次数和锁定状态
// ============================================================
void UserDao::resetLoginState(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    NetworkManager::instance().sendRequest("resetLoginState", data);
}

// ============================================================
// 增加失败次数，返回是否已触发锁定
// ============================================================
bool UserDao::incrementFailCount(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("incrementFailCount", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("locked").toBool();
    }
    return false;
}

// ============================================================
// 获取剩余锁定分钟数（0表示未锁定）
// ============================================================
int UserDao::getLockRemainingMinutes(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("getLockRemainingMinutes", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("minutes").toInt();
    }
    return 0;
}

// ============================================================
// 获取剩余锁定秒数（0表示未锁定）
// ============================================================
int UserDao::getLockRemainingSeconds(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("getLockRemainingSeconds", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("seconds").toInt();
    }
    return 0;
}

// ============================================================
// 登录验证（含锁定逻辑）
// ============================================================
bool UserDao::validateLogin(const QString &username, const QString &passwordHash)
{
    QJsonObject data;
    data["username"] = username;
    data["password_hash"] = passwordHash;
    QJsonObject resp = NetworkManager::instance().sendRequest("validateLogin", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 删除用户
// ============================================================
bool UserDao::deleteById(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("deleteUser", data);
    return resp.value("code").toInt() == 0;
}
