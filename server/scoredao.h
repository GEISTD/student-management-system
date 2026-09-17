#ifndef SCOREDAO_H
#define SCOREDAO_H

#include <QList>
#include "score.h"

class ScoreDao
{
public:
    // 增
    static bool insert(const Score &s);
    // 删
    static bool deleteById(int id);
    // 改
    static bool update(const Score &s);
    // 查：按学生ID查询所有成绩
    static QList<Score> selectByStudentId(int studentId);
    // 查：按学生ID计算平均分
    static double selectAverageByStudentId(int studentId);

};

#endif // SCOREDAO_H
