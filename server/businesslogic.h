#ifndef BUSINESSLOGIC_H
#define BUSINESSLOGIC_H

#include <QList>
#include <QString>
#include <QStringList>
#include "student.h"

class BusinessLogic
{
public:
    // 计算班级平均分（所有学生的各科平均分的平均值）
    static double calcClassAverage(const QString &className);

    // 计算班级及格率（≥60分算及格，返回百分比）
    static double calcPassRate(const QString &className);

    // 获取全班排名（按平均分降序排列）
    static QList<Student> getClassRanking(const QString &className);

    // 获取所有班级列表（去重）
    static QStringList getAllClasses();

    // 辅助函数：获取某个学生的各科平均分
    static double getStudentAverageScore(int studentId);
};

#endif // BUSINESSLOGIC_H
