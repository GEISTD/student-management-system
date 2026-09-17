#ifndef USERDAO_H
#define USERDAO_H

#include "user.h"

class UserDao
{
public:
    // 插入新用户（注册用）
    static bool insert(const User &u);

    // 登录验证（含锁定逻辑）
    static bool validateLogin(const QString &username, const QString &passwordHash);

    // 按用户名查询用户信息
    static User selectByUsername(const QString &username);

    // 更新用户角色
    static bool updateRole(int userId, const QString &newRole);

    // 按ID查询用户
    static User selectById(int id);

    // 修改密码
    static bool updatePassword(int userId, const QString &newPasswordHash);

    // 修改用户名
    static bool updateUsername(int userId, const QString &newUsername);

    // 检查用户名是否存在（排除指定用户ID）
    static bool existsUsername(const QString &username, int excludeUserId = 0);

    // 重置失败次数和锁定状态（登录成功时调用）
    static void resetLoginState(int userId);

    // 增加失败次数（登录失败时调用），返回是否已锁定
    static bool incrementFailCount(int userId);

    // 检查账号是否被锁定，返回剩余锁定分钟数（0表示未锁定）
    static int getLockRemainingMinutes(int userId);
    // 返回剩余秒数（0 表示未锁定）
    static int getLockRemainingSeconds(int userId);

    // 删除用户
    static bool deleteById(int userId);
};

#endif // USERDAO_H
