#include "studentdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// 解析日期，兼容 yyyy-MM-dd 和 yyyy/MM/dd 两种格式
static QDate parseDate(const QString &str) {
    QDate d = QDate::fromString(str, "yyyy-MM-dd");
    if (d.isValid()) return d;
    return QDate::fromString(str, "yyyy/MM/dd");
}

int StudentDao::insert(const Student &s)
{
    // 先检查学号是否已存在
    if (!s.studentId.isEmpty()) {
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT id FROM students WHERE student_id = :student_id");
        checkQuery.bindValue(":student_id", s.studentId);
        if (checkQuery.exec() && checkQuery.next()) {
            qDebug() << "⚠️ 学号已存在:" << s.studentId << ", 已有ID:" << checkQuery.value(0).toInt();
            return -2; // -2 表示学号重复
        }
    }

    QSqlQuery query;
    query.prepare("INSERT INTO students (student_id, name, major, class_name, gender, birth_date, user_id) "
                  "VALUES (:student_id, :name, :major, :class_name, :gender, :birth_date, :user_id)");
    query.bindValue(":student_id", s.studentId);
    query.bindValue(":name", s.name);
    query.bindValue(":major", s.major);
    query.bindValue(":class_name", s.className);
    query.bindValue(":gender", s.gender);
    if (s.birthDate.isValid()) {
        query.bindValue(":birth_date", s.birthDate.toString("yyyy/MM/dd"));
    } else {
        query.bindValue(":birth_date", QVariant(QVariant::String));
    }
    query.bindValue(":user_id", s.userId);

    if (!query.exec()) {
        qDebug() << "❌ 新增学生失败:" << query.lastError().text();
        return -1;
    }
    int newId = query.lastInsertId().toInt();
    qDebug() << "✅ 新增学生成功:" << s.name << "学号:" << s.studentId << "ID:" << newId;
    return newId;
}

QList<Student> StudentDao::selectAll()
{
    QList<Student> list;
    QSqlQuery query("SELECT id, student_id, name, major, class_name, gender, birth_date, user_id FROM students");

    while (query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.studentId = query.value(1).toString();
        s.name = query.value(2).toString();
        s.major = query.value(3).toString();
        s.className = query.value(4).toString();
        s.gender = query.value(5).toString();
        s.birthDate = parseDate(query.value(6).toString());
        s.userId = query.value(7).toInt();
        list.append(s);
    }

    qDebug() << "📊 查询全部学生，共" << list.size() << "条";
    return list;
}

// 删除学生
bool StudentDao::deleteById(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ 删除失败:" << query.lastError().text();
        return false;
    }

    int affected = query.numRowsAffected();
    if (affected > 0) {
        qDebug() << "✅ 删除学生成功, id=" << id;
        return true;
    } else {
        qDebug() << "⚠️ 未找到要删除的学生, id=" << id;
        return false;
    }
}

// 修改学生
bool StudentDao::update(const Student &s)
{
    QSqlQuery query;
    query.prepare("UPDATE students SET "
                  "student_id = :student_id, "
                  "name = :name, "
                  "major = :major, "
                  "class_name = :class_name, "
                  "gender = :gender, "
                  "birth_date = :birth_date, "
                  "user_id = :user_id "
                  "WHERE id = :id");
    query.bindValue(":student_id", s.studentId);
    query.bindValue(":name", s.name);
    query.bindValue(":major", s.major);
    query.bindValue(":class_name", s.className);
    query.bindValue(":gender", s.gender);
    if (s.birthDate.isValid()) {
        query.bindValue(":birth_date", s.birthDate.toString("yyyy/MM/dd"));
    } else {
        query.bindValue(":birth_date", QVariant(QVariant::String));
    }
    query.bindValue(":user_id", s.userId);
    query.bindValue(":id", s.id);

    if (!query.exec()) {
        qDebug() << "❌ 修改失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ 修改学生成功, id=" << s.id;
    return true;
}

// ========== 按 ID 查询单个学生 ==========
Student StudentDao::selectById(int id)
{
    Student s;
    QSqlQuery query;
    query.prepare("SELECT id, student_id, name, major, class_name, gender, birth_date, user_id FROM students WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        s.id = query.value(0).toInt();
        s.studentId = query.value(1).toString();
        s.name = query.value(2).toString();
        s.major = query.value(3).toString();
        s.className = query.value(4).toString();
        s.gender = query.value(5).toString();
        s.birthDate = parseDate(query.value(6).toString());
        s.userId = query.value(7).toInt();
        qDebug() << "🔍 查询学生 id=" << id << ", 姓名=" << s.name;
    } else {
        qDebug() << "⚠️ 未找到学生 id=" << id;
    }
    return s;
}

// ========== 按班级查询学生列表 ==========
QList<Student> StudentDao::selectByClass(const QString &className)
{
    QList<Student> list;
    QSqlQuery query;
    query.prepare("SELECT id, student_id, name, major, class_name, gender, birth_date, user_id FROM students WHERE class_name = :class_name");
    query.bindValue(":class_name", className);

    if (query.exec()) {
        while (query.next()) {
            Student s;
            s.id = query.value(0).toInt();
            s.studentId = query.value(1).toString();
            s.name = query.value(2).toString();
            s.major = query.value(3).toString();
            s.className = query.value(4).toString();
            s.gender = query.value(5).toString();
            s.birthDate = parseDate(query.value(6).toString());
            s.userId = query.value(7).toInt();
            list.append(s);
        }
    }
    qDebug() << "📊 查询班级'" << className << "', 共" << list.size() << "人";
    return list;
}

// ========== 按姓名模糊查询 ==========
QList<Student> StudentDao::selectByName(const QString &keyword)
{
    QList<Student> list;
    QSqlQuery query;
    query.prepare("SELECT id, student_id, name, major, class_name, gender, birth_date, user_id FROM students WHERE name LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%");

    if (query.exec()) {
        while (query.next()) {
            Student s;
            s.id = query.value(0).toInt();
            s.studentId = query.value(1).toString();
            s.name = query.value(2).toString();
            s.major = query.value(3).toString();
            s.className = query.value(4).toString();
            s.gender = query.value(5).toString();
            s.birthDate = parseDate(query.value(6).toString());
            s.userId = query.value(7).toInt();
            list.append(s);
        }
    }
    qDebug() << "🔍 模糊查询'" << keyword << "', 共" << list.size() << "人";
    return list;
}

// ========== 按用户ID查询学生 ==========
Student StudentDao::selectByUserId(int userId)
{
    Student s;
    QSqlQuery query;
    query.prepare("SELECT id, student_id, name, major, class_name, gender, birth_date, user_id FROM students WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec() && query.next()) {
        s.id = query.value(0).toInt();
        s.studentId = query.value(1).toString();
        s.name = query.value(2).toString();
        s.major = query.value(3).toString();
        s.className = query.value(4).toString();
        s.gender = query.value(5).toString();
        s.birthDate = parseDate(query.value(6).toString());
        s.userId = query.value(7).toInt();
        qDebug() << "🔍 通过用户ID查询学生: userId=" << userId << ", 姓名=" << s.name;
    } else {
        qDebug() << "⚠️ 未找到 userId=" << userId << " 对应的学生";
    }
    return s;
}
//去重
bool StudentDao::existsByUserId(int userId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM students WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
