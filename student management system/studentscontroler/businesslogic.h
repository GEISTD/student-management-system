#ifndef BUSINESSLOGIC_H
#define BUSINESSLOGIC_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
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

    // 根据学号获取年级（2026入学为大一）
    static QString getGradeByStudentId(const QString &studentId);

    // 根据专业和年级获取科目列表
    static QStringList getSubjectsByMajorAndGrade(const QString &major, const QString &grade);

    // 获取所有年级列表
    static QStringList getAllGrades();

    // 获取所有专业列表
    static QStringList getAllMajors();

    // 获取所有科目列表
    static QStringList getAllSubjects();

    // 计算班级某科目的平均分
    static double calcClassSubjectAverage(const QString &className, const QString &subject);

    // 计算班级某科目的最高分
    static double calcClassSubjectMax(const QString &className, const QString &subject);

    // 获取所有不同年级不同专业的班级列表
    static QMap<QString, QMap<QString, QStringList>> getGradeMajorClasses();
};

#endif // BUSINESSLOGIC_H
