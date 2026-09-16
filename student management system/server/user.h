#ifndef USER_H
#define USER_H

#include <QString>

struct User {
    int id = 0;
    QString username;
    QString passwordHash;   // MD5 加密后的密文
    QString role;           // "admin" 或 "student"
    int loginFailCount = 0;
    QString lockTime;   // 锁定截止时间，空表示未锁定
};

#endif // USER_H
