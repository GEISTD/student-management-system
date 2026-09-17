#ifndef SCORE_H
#define SCORE_H

#include <QString>
#include <QDate>

struct Score {
    int id = 0;
    int studentId = 0;      // 关联 students 表的 id
    QString subject;        // 科目名称（语文、数学、英语等）
    double score = 0.0;
    QDate examDate;         // 考试日期
};

#endif // SCORE_H
