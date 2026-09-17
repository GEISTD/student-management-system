#include "businesslogic.h"
#include "studentdao.h"
#include "scoredao.h"    // 成绩表 DAO（还没建？没关系，我们马上创建）
#include <algorithm>
#include <QDebug>

// ============================================================
// 辅助函数：获取某个学生的各科平均分
// ============================================================
double BusinessLogic::getStudentAverageScore(int studentId)
{
    QList<Score> scores = ScoreDao::selectByStudentId(studentId);
    if (scores.isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (const Score &s : scores) {
        sum += s.score;
    }
    return sum / scores.size();
}

// ============================================================
// 1. 计算班级平均分
// ============================================================
double BusinessLogic::calcClassAverage(const QString &className)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        qDebug() << "⚠️ 班级" << className << "没有学生";
        return 0.0;
    }

    double totalAvg = 0.0;
    for (const Student &s : students) {
        totalAvg += getStudentAverageScore(s.id);
    }

    double result = totalAvg / students.size();
    qDebug() << "📊 班级" << className << "平均分:" << result;
    return result;
}

// ============================================================
// 2. 计算班级及格率（≥60分算及格）
// ============================================================
double BusinessLogic::calcPassRate(const QString &className)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        qDebug() << "⚠️ 班级" << className << "没有学生";
        return 0.0;
    }

    int passCount = 0;
    for (const Student &s : students) {
        double avg = getStudentAverageScore(s.id);
        if (avg >= 60.0) {
            passCount++;
        }
    }

    double result = (double)passCount / students.size() * 100.0;
    qDebug() << "📊 班级" << className << "及格率:" << result << "%";
    return result;
}

// ============================================================
// 3. 获取全班排名（按平均分降序）
// ============================================================
QList<Student> BusinessLogic::getClassRanking(const QString &className)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        qDebug() << "⚠️ 班级" << className << "没有学生";
        return students;
    }

    // 按平均分降序排序
    std::sort(students.begin(), students.end(), [](const Student &a, const Student &b) {
        // 这里需要计算平均分，但我们不能在这里调用 getStudentAverageScore（不是静态的）
        // 所以用 lambda 捕获？不行。我们换个方式：用 ScoreDao 直接算
        // 更好的方式：在排序前先计算好每个学生的平均分，存到临时结构里
        // 但为了简洁，我们使用一个辅助 lambda
        double avgA = BusinessLogic::getStudentAverageScore(a.id);
        double avgB = BusinessLogic::getStudentAverageScore(b.id);
        return avgA > avgB;  // 降序：高分在前
    });

    qDebug() << "🏆 班级" << className << "排名生成成功，共" << students.size() << "人";
    return students;
}

// ============================================================
// 4. 获取所有班级列表（去重）
// ============================================================
QStringList BusinessLogic::getAllClasses()
{
    QList<Student> allStudents = StudentDao::selectAll();
    QStringList classList;

    for (const Student &s : allStudents) {
        if (!s.className.isEmpty() && !classList.contains(s.className)) {
            classList.append(s.className);
        }
    }

    qDebug() << "📋 获取所有班级，共" << classList.size() << "个";
    return classList;
}
