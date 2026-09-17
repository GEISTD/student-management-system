#include "studentdao.h"
#include "NetworkManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDate>

// 解析日期，兼容 yyyy-MM-dd 和 yyyy/MM/dd 两种格式
static QDate parseDate(const QString &str) {
    QDate d = QDate::fromString(str, "yyyy-MM-dd");
    if (d.isValid()) return d;
    return QDate::fromString(str, "yyyy/MM/dd");
}

// ============================================================
// 新增学生，返回新ID（0表示失败）
// ============================================================
int StudentDao::insert(const Student &s)
{
    QJsonObject data;
    data["student_id"] = s.studentId;
    data["name"] = s.name;
    data["major"] = s.major;
    data["class_name"] = s.className;
    data["gender"] = s.gender;
    data["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
    data["user_id"] = s.userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("insertStudent", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("new_id").toInt();
    }
    return 0;
}

// ============================================================
// 删除学生
// ============================================================
bool StudentDao::deleteById(int id)
{
    QJsonObject data;
    data["id"] = id;
    QJsonObject resp = NetworkManager::instance().sendRequest("deleteStudent", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 更新学生数据
// ============================================================
bool StudentDao::update(const Student &s)
{
    QJsonObject data;
    data["id"] = s.id;
    data["student_id"] = s.studentId;
    data["name"] = s.name;
    data["major"] = s.major;
    data["class_name"] = s.className;
    data["gender"] = s.gender;
    data["birth_date"] = s.birthDate.toString("yyyy/MM/dd");
    data["user_id"] = s.userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("updateStudent", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 查询全部学生
// ============================================================
QList<Student> StudentDao::selectAll()
{
    QList<Student> list;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectAllStudents", QJsonObject());
    if (resp.value("code").toInt() == 0) {
        QJsonArray studentsArray = resp.value("data").toObject().value("students").toArray();
        for (const QJsonValue &val : studentsArray) {
            QJsonObject obj = val.toObject();
            Student s;
            s.id = obj.value("id").toInt();
            s.studentId = obj.value("student_id").toString();
            s.name = obj.value("name").toString();
            s.major = obj.value("major").toString();
            s.className = obj.value("class_name").toString();
            s.gender = obj.value("gender").toString();
            s.birthDate = parseDate(obj.value("birth_date").toString());
            s.userId = obj.value("user_id").toInt();
            list.append(s);
        }
    }
    return list;
}

// ============================================================
// 按 ID 查询单个学生
// ============================================================
Student StudentDao::selectById(int id)
{
    QJsonObject data;
    data["id"] = id;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectStudentById", data);
    Student s;
    if (resp.value("code").toInt() == 0) {
        QJsonObject obj = resp.value("data").toObject().value("student").toObject();
        s.id = obj.value("id").toInt();
        s.studentId = obj.value("student_id").toString();
        s.name = obj.value("name").toString();
        s.major = obj.value("major").toString();
        s.className = obj.value("class_name").toString();
        s.gender = obj.value("gender").toString();
        s.birthDate = parseDate(obj.value("birth_date").toString());
        s.userId = obj.value("user_id").toInt();
    }
    return s;
}

// ============================================================
// 按班级查询学生列表
// ============================================================
QList<Student> StudentDao::selectByClass(const QString &className)
{
    QList<Student> list;
    QJsonObject data;
    data["class_name"] = className;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectByClass", data);
    if (resp.value("code").toInt() == 0) {
        QJsonArray studentsArray = resp.value("data").toObject().value("students").toArray();
        for (const QJsonValue &val : studentsArray) {
            QJsonObject obj = val.toObject();
            Student s;
            s.id = obj.value("id").toInt();
            s.studentId = obj.value("student_id").toString();
            s.name = obj.value("name").toString();
            s.major = obj.value("major").toString();
            s.className = obj.value("class_name").toString();
            s.gender = obj.value("gender").toString();
            s.birthDate = parseDate(obj.value("birth_date").toString());
            s.userId = obj.value("user_id").toInt();
            list.append(s);
        }
    }
    return list;
}

// ============================================================
// 按姓名模糊查询
// ============================================================
QList<Student> StudentDao::selectByName(const QString &keyword)
{
    QList<Student> list;
    QJsonObject data;
    data["keyword"] = keyword;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectByName", data);
    if (resp.value("code").toInt() == 0) {
        QJsonArray studentsArray = resp.value("data").toObject().value("students").toArray();
        for (const QJsonValue &val : studentsArray) {
            QJsonObject obj = val.toObject();
            Student s;
            s.id = obj.value("id").toInt();
            s.studentId = obj.value("student_id").toString();
            s.name = obj.value("name").toString();
            s.major = obj.value("major").toString();
            s.className = obj.value("class_name").toString();
            s.gender = obj.value("gender").toString();
            s.birthDate = parseDate(obj.value("birth_date").toString());
            s.userId = obj.value("user_id").toInt();
            list.append(s);
        }
    }
    return list;
}

// ============================================================
// 按用户ID查询学生
// ============================================================
Student StudentDao::selectByUserId(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectByUserId", data);
    Student s;
    if (resp.value("code").toInt() == 0) {
        QJsonObject obj = resp.value("data").toObject().value("student").toObject();
        s.id = obj.value("id").toInt();
        s.studentId = obj.value("student_id").toString();
        s.name = obj.value("name").toString();
        s.major = obj.value("major").toString();
        s.className = obj.value("class_name").toString();
        s.gender = obj.value("gender").toString();
        s.birthDate = parseDate(obj.value("birth_date").toString());
        s.userId = obj.value("user_id").toInt();
    }
    return s;
}

// ============================================================
// 检查某个 user_id 是否已被其他学生占用
// ============================================================
bool StudentDao::existsByUserId(int userId)
{
    QJsonObject data;
    data["user_id"] = userId;
    QJsonObject resp = NetworkManager::instance().sendRequest("existsByUserId", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("exists").toBool();
    }
    return false;
}
