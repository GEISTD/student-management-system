#include "userdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

// ============================================================
// 插入新用户
// ============================================================
bool UserDao::insert(const User &u)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash, role) "
                  "VALUES (:username, :password_hash, :role)");
    query.bindValue(":username", u.username);
    query.bindValue(":password_hash", u.passwordHash);
    query.bindValue(":role", u.role);

    if (!query.exec()) {
        qDebug() << "❌ 插入用户失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 用户插入成功:" << u.username;
    return true;
}

// ============================================================
// 按用户名查询用户
// ============================================================
User UserDao::selectByUsername(const QString &username)
{
    User u;
    QSqlQuery query;
    query.prepare("SELECT id, username, password_hash, role FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        u.id = query.value(0).toInt();
        u.username = query.value(1).toString();
        u.passwordHash = query.value(2).toString();
        u.role = query.value(3).toString();
        qDebug() << "👤 查询用户:" << username << ", 角色=" << u.role;
    } else {
        qDebug() << "⚠️ 未找到用户:" << username;
    }
    return u;
}

// ============================================================
// 更新用户角色
// ============================================================
bool UserDao::updateRole(int userId, const QString &newRole)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET role = :role WHERE id = :id");
    query.bindValue(":role", newRole);
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "❌ 更新角色失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 更新用户角色成功, userId=" << userId << ", role=" << newRole;
    return true;
}

// ============================================================
// 按ID查询用户
// ============================================================
User UserDao::selectById(int id)
{
    User u;
    QSqlQuery query;
    query.prepare("SELECT id, username, password_hash, role FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        u.id = query.value(0).toInt();
        u.username = query.value(1).toString();
        u.passwordHash = query.value(2).toString();
        u.role = query.value(3).toString();
        qDebug() << "👤 查询用户ID:" << id << ", 用户名=" << u.username;
    } else {
        qDebug() << "⚠️ 未找到用户ID:" << id;
    }
    return u;
}

// ============================================================
// 修改密码
// ============================================================
bool UserDao::updatePassword(int userId, const QString &newPasswordHash)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET password_hash = :password_hash WHERE id = :id");
    query.bindValue(":password_hash", newPasswordHash);
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "❌ 修改密码失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 修改密码成功, userId=" << userId;
    return true;
}

// ============================================================
// 修改用户名
// ============================================================
bool UserDao::updateUsername(int userId, const QString &newUsername)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET username = :username WHERE id = :id");
    query.bindValue(":username", newUsername);
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "❌ 修改用户名失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 修改用户名成功, userId=" << userId << ", 新用户名=" << newUsername;
    return true;
}

// ============================================================
// 检查用户名是否存在（排除指定用户ID）
// ============================================================
bool UserDao::existsUsername(const QString &username, int excludeUserId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND id != :exclude_id");
    query.bindValue(":username", username);
    query.bindValue(":exclude_id", excludeUserId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// ============================================================
// 重置失败次数和锁定状态
// ============================================================
void UserDao::resetLoginState(int userId)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET login_fail_count = 0, lock_time = '' WHERE id = :id");
    query.bindValue(":id", userId);
    if (query.exec()) {
        qDebug() << "🔄 重置登录状态, userId=" << userId;
    }
}

// ============================================================
// 增加失败次数，返回是否已触发锁定
// ============================================================
bool UserDao::incrementFailCount(int userId)
{
    const int MAX_FAIL = 5;
    const int LOCK_MINUTES = 1;   // 锁定1分钟

    QSqlQuery query;
    query.prepare("UPDATE users SET login_fail_count = login_fail_count + 1 WHERE id = :id");
    query.bindValue(":id", userId);
    query.exec();

    query.prepare("SELECT login_fail_count FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        qDebug() << "🔢 当前失败次数:" << count;
        if (count >= MAX_FAIL) {
            QDateTime lockUntil = QDateTime::currentDateTime().addSecs(LOCK_MINUTES * 60);
            QSqlQuery lockQuery;
            lockQuery.prepare("UPDATE users SET lock_time = :lock_time WHERE id = :id");
            lockQuery.bindValue(":lock_time", lockUntil.toString(Qt::ISODate));
            lockQuery.bindValue(":id", userId);
            lockQuery.exec();
            qDebug() << "🔒 账号已锁定，直到:" << lockUntil.toString();
            return true;
        }
    }
    return false;
}

// ============================================================
// 获取剩余锁定分钟数（0表示未锁定）
// ============================================================
int UserDao::getLockRemainingMinutes(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT lock_time FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    if (query.exec() && query.next()) {
        QString lockTimeStr = query.value(0).toString();
        if (!lockTimeStr.isEmpty()) {
            QDateTime lockTime = QDateTime::fromString(lockTimeStr, Qt::ISODate);
            if (lockTime.isValid() && lockTime > QDateTime::currentDateTime()) {
                int remain = QDateTime::currentDateTime().secsTo(lockTime) / 60 + 1;
                qDebug() << "⏳ 剩余锁定分钟数:" << remain;
                return remain;
            }
        }
    }
    return 0;
}

// ============================================================
// ✅ 新增：获取剩余锁定秒数（0表示未锁定）
// ============================================================
int UserDao::getLockRemainingSeconds(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT lock_time FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    if (query.exec() && query.next()) {
        QString lockTimeStr = query.value(0).toString();
        if (!lockTimeStr.isEmpty()) {
            QDateTime lockTime = QDateTime::fromString(lockTimeStr, Qt::ISODate);
            if (lockTime.isValid() && lockTime > QDateTime::currentDateTime()) {
                int remain = QDateTime::currentDateTime().secsTo(lockTime);
                qDebug() << "⏳ 剩余锁定秒数:" << remain;
                return remain;
            }
        }
    }
    return 0;
}

// ============================================================
// 登录验证（含锁定逻辑）
// ============================================================
// ============================================================
// 删除用户
// ============================================================
bool UserDao::deleteById(int userId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "❌ 删除用户失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 删除用户成功, userId=" << userId;
    return true;
}

bool UserDao::validateLogin(const QString &username, const QString &passwordHash)
{
    qDebug() << "🔍 [validateLogin] 开始验证，用户名:" << username << ", 密码哈希:" << passwordHash;

    QSqlQuery query;
    query.prepare("SELECT id, password_hash, lock_time, login_fail_count FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        qDebug() << "⚠️ [validateLogin] 用户不存在:" << username;
        return false;
    }

    int userId = query.value(0).toInt();
    QString storedHash = query.value(1).toString();
    QString lockTimeStr = query.value(2).toString();
    int failCount = query.value(3).toInt();

    qDebug() << "🔍 [validateLogin] 找到用户 ID:" << userId
             << ", 存储的哈希:" << storedHash
             << ", lock_time:" << lockTimeStr
             << ", failCount:" << failCount;

    // 1. 检查锁定
    if (!lockTimeStr.isEmpty()) {
        QDateTime lockTime = QDateTime::fromString(lockTimeStr, Qt::ISODate);
        if (lockTime.isValid() && lockTime > QDateTime::currentDateTime()) {
            int remainSec = QDateTime::currentDateTime().secsTo(lockTime);
            qDebug() << "🔒 [validateLogin] 账号被锁定，剩余秒数:" << remainSec;
            return false; // 锁定中
        } else {
            qDebug() << "🔓 [validateLogin] 锁定已过期，自动解锁";
            resetLoginState(userId);
        }
    }

    // 2. 验证密码
    qDebug() << "🔍 [validateLogin] 比较哈希: 传入=" << passwordHash << ", 存储=" << storedHash;
    if (storedHash == passwordHash) {
        resetLoginState(userId);
        qDebug() << "✅ [validateLogin] 登录成功:" << username;
        return true;
    } else {
        bool locked = incrementFailCount(userId);
        if (locked) {
            qDebug() << "🔒 [validateLogin] 账号已被锁定:" << username;
        } else {
            qDebug() << "❌ [validateLogin] 密码错误:" << username;
        }
        return false;
    }
}
