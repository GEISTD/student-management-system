#ifndef STUDENTDAO_H
#define STUDENTDAO_H

#include <QList>
#include "student.h"

class StudentDao
{
public:
    // 新增学生，返回新ID（0表示失败）
    static int insert(const Student &s);
    // 删去学生
    static bool deleteById(int id);
    // 更新学生数据
    static bool update(const Student &s);
    // 查询全部学生
    static QList<Student> selectAll();
    // ========== 新增：三种查询方式 ==========
    static Student selectById(int id);                    // 按 ID 查单个
    static QList<Student> selectByClass(const QString &className);  // 按班级查
    static QList<Student> selectByName(const QString &keyword);     // 按姓名模糊查
    // 按用户ID查询学生
    static Student selectByUserId(int userId);
    // 检查某个 user_id 是否已被其他学生占用（即已有学生记录使用该 user_id）
    static bool existsByUserId(int userId);
};

#endif // STUDENTDAO_H
