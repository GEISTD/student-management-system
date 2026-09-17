#include "businesslogic.h"
#include "studentdao.h"
#include "scoredao.h"
#include <algorithm>
#include <QDebug>
#include <QSet>

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

QString BusinessLogic::getGradeByStudentId(const QString &studentId)
{
    if (studentId.length() < 4) return "未知年级";
    int enrollmentYear = studentId.left(4).toInt();
    int currentYear = QDate::currentDate().year();
    int gradeNum = currentYear - enrollmentYear + 1;
    if (gradeNum <= 0) return "未知年级";
    if (gradeNum == 1) return "大一";
    if (gradeNum == 2) return "大二";
    if (gradeNum == 3) return "大三";
    if (gradeNum == 4) return "大四";
    return "研究生";
}

QStringList BusinessLogic::getSubjectsByMajorAndGrade(const QString &major, const QString &grade)
{
    if (major == "计算机科学与技术") {
        if (grade == "大一") {
            return {"高等数学", "大学英语", "计算机导论", "程序设计基础", "线性代数", "思想道德修养"};
        } else if (grade == "大二") {
            return {"数据结构", "计算机组成原理", "操作系统", "概率论与数理统计", "大学物理", "离散数学"};
        } else if (grade == "大三") {
            return {"算法设计与分析", "数据库原理", "计算机网络", "软件工程", "编译原理", "人工智能导论"};
        } else if (grade == "大四") {
            return {"毕业设计", "专业实习", "云计算", "大数据技术", "区块链基础"};
        }
    } else if (major == "软件工程") {
        if (grade == "大一") {
            return {"高等数学", "大学英语", "计算机导论", "程序设计基础", "线性代数", "思想道德修养"};
        } else if (grade == "大二") {
            return {"数据结构", "面向对象程序设计", "操作系统", "概率论与数理统计", "软件工程导论", "数据库原理"};
        } else if (grade == "大三") {
            return {"软件体系结构", "软件测试", "项目管理", "Web开发", "移动应用开发", "人机交互"};
        } else if (grade == "大四") {
            return {"毕业设计", "专业实习", "云计算", "DevOps", "敏捷开发"};
        }
    } else if (major == "人工智能") {
        if (grade == "大一") {
            return {"高等数学", "大学英语", "计算机导论", "程序设计基础", "线性代数", "思想道德修养"};
        } else if (grade == "大二") {
            return {"数据结构", "概率论与数理统计", "机器学习基础", "统计学", "Python编程", "大学物理"};
        } else if (grade == "大三") {
            return {"深度学习", "神经网络", "自然语言处理", "计算机视觉", "数据挖掘", "强化学习"};
        } else if (grade == "大四") {
            return {"毕业设计", "专业实习", "AI工程实践", "大模型应用", "AI伦理"};
        }
    } else if (major == "数据科学") {
        if (grade == "大一") {
            return {"高等数学", "大学英语", "计算机导论", "程序设计基础", "线性代数", "思想道德修养"};
        } else if (grade == "大二") {
            return {"数据结构", "概率论与数理统计", "统计学", "SQL数据库", "Python编程", "大学物理"};
        } else if (grade == "大三") {
            return {"数据挖掘", "大数据分析", "机器学习", "数据可视化", "分布式计算", "数据工程"};
        } else if (grade == "大四") {
            return {"毕业设计", "专业实习", "数据治理", "商业智能", "数据安全"};
        }
    } else if (major == "信息管理") {
        if (grade == "大一") {
            return {"高等数学", "大学英语", "计算机导论", "管理学原理", "线性代数", "思想道德修养"};
        } else if (grade == "大二") {
            return {"信息系统概论", "数据库原理", "统计学", "会计学基础", "市场营销", "程序设计基础"};
        } else if (grade == "大三") {
            return {"信息安全", "电子商务", "供应链管理", "信息系统开发", "数据分析", "项目管理"};
        } else if (grade == "大四") {
            return {"毕业设计", "专业实习", "数字化转型", "企业信息化", "信息战略"};
        }
    }
    return {"语文", "数学", "英语", "物理", "化学", "生物"};
}

QStringList BusinessLogic::getAllGrades()
{
    return {"大一", "大二", "大三", "大四"};
}

QStringList BusinessLogic::getAllMajors()
{
    return {"计算机科学与技术", "软件工程", "人工智能", "数据科学", "信息管理"};
}

QStringList BusinessLogic::getAllSubjects()
{
    QStringList subjects;
    QStringList majors = getAllMajors();
    QStringList grades = getAllGrades();
    QSet<QString> subjectSet;
    for (const QString &major : majors) {
        for (const QString &grade : grades) {
            QStringList subj = getSubjectsByMajorAndGrade(major, grade);
            for (const QString &s : subj) {
                subjectSet.insert(s);
            }
        }
    }
    return subjectSet.toList();
}

// 修改：无成绩时返回 -1.0，以便区分真实 0 分
double BusinessLogic::calcClassSubjectAverage(const QString &className, const QString &subject)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        return -1.0;
    }

    double totalAvg = 0.0;
    int studentCount = 0;
    for (const Student &s : students) {
        QList<Score> scores = ScoreDao::selectByStudentId(s.id);
        double sum = 0.0;
        int count = 0;
        for (const Score &sc : scores) {
            if (sc.subject == subject) {
                sum += sc.score;
                count++;
            }
        }
        if (count > 0) {
            totalAvg += (sum / count);
            studentCount++;
        }
    }

    if (studentCount == 0) {
        return -1.0;
    }
    return totalAvg / studentCount;
}

double BusinessLogic::calcClassSubjectMax(const QString &className, const QString &subject)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        return -1.0;
    }

    double maxScore = -1.0;
    for (const Student &s : students) {
        QList<Score> scores = ScoreDao::selectByStudentId(s.id);
        for (const Score &sc : scores) {
            if (sc.subject == subject && sc.score > maxScore) {
                maxScore = sc.score;
            }
        }
    }

    return maxScore;
}

QMap<QString, QMap<QString, QStringList>> BusinessLogic::getGradeMajorClasses()
{
    QMap<QString, QMap<QString, QStringList>> result;
    QList<Student> allStudents = StudentDao::selectAll();

    for (const Student &s : allStudents) {
        QString grade = getGradeByStudentId(s.studentId);
        QString major = s.major;
        QString className = s.className;

        if (!result.contains(grade)) {
            result[grade] = QMap<QString, QStringList>();
        }
        if (!result[grade].contains(major)) {
            result[grade][major] = QStringList();
        }
        if (!result[grade][major].contains(className)) {
            result[grade][major].append(className);
        }
    }

    return result;
}

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

QList<Student> BusinessLogic::getClassRanking(const QString &className)
{
    QList<Student> students = StudentDao::selectByClass(className);
    if (students.isEmpty()) {
        qDebug() << "⚠️ 班级" << className << "没有学生";
        return students;
    }

    std::sort(students.begin(), students.end(), [](const Student &a, const Student &b) {
        double avgA = BusinessLogic::getStudentAverageScore(a.id);
        double avgB = BusinessLogic::getStudentAverageScore(b.id);
        return avgA > avgB;
    });

    qDebug() << "🏆 班级" << className << "排名生成成功，共" << students.size() << "人";
    return students;
}

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
