#ifndef STUDENT_H
#define STUDENT_H

#include <QString>
#include <QDate>

struct Student {
    int id = 0;               // 数据库主键，自动生成
    QString studentId;        // 学号（如：2024010101）
    QString name;             // 姓名
    QString major;            // 专业
    QString className;        // 班级
    QString gender;           // 性别（"男" / "女"）
    QDate birthDate;          // 出生日期
    int userId = 0;           // 关联到 users 表的 id
};

#endif // STUDENT_H
