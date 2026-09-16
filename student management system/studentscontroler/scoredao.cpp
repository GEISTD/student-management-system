#include "scoredao.h"
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
// 插入成绩
// ============================================================
bool ScoreDao::insert(const Score &s)
{
    QJsonObject data;
    data["student_id"] = s.studentId;
    data["subject"] = s.subject;
    data["score"] = s.score;
    data["exam_date"] = s.examDate.toString("yyyy/MM/dd");
    QJsonObject resp = NetworkManager::instance().sendRequest("insertScore", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 删除成绩
// ============================================================
bool ScoreDao::deleteById(int id)
{
    QJsonObject data;
    data["id"] = id;
    QJsonObject resp = NetworkManager::instance().sendRequest("deleteScore", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 修改成绩
// ============================================================
bool ScoreDao::update(const Score &s)
{
    QJsonObject data;
    data["id"] = s.id;
    data["student_id"] = s.studentId;
    data["subject"] = s.subject;
    data["score"] = s.score;
    data["exam_date"] = s.examDate.toString("yyyy/MM/dd");
    QJsonObject resp = NetworkManager::instance().sendRequest("updateScore", data);
    return resp.value("code").toInt() == 0;
}

// ============================================================
// 按学生ID查询所有成绩
// ============================================================
QList<Score> ScoreDao::selectByStudentId(int studentId)
{
    QList<Score> list;
    QJsonObject data;
    data["student_id"] = studentId;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectScoresByStudent", data);
    if (resp.value("code").toInt() == 0) {
        QJsonArray scoresArray = resp.value("data").toObject().value("scores").toArray();
        for (const QJsonValue &val : scoresArray) {
            QJsonObject obj = val.toObject();
            Score s;
            s.id = obj.value("id").toInt();
            s.studentId = obj.value("student_id").toInt();
            s.subject = obj.value("subject").toString();
            s.score = obj.value("score").toDouble();
            s.examDate = parseDate(obj.value("exam_date").toString());
            list.append(s);
        }
    }
    return list;
}

// ============================================================
// 按学生ID计算平均分
// ============================================================
double ScoreDao::selectAverageByStudentId(int studentId)
{
    QJsonObject data;
    data["student_id"] = studentId;
    QJsonObject resp = NetworkManager::instance().sendRequest("selectAverageScore", data);
    if (resp.value("code").toInt() == 0) {
        return resp.value("data").toObject().value("average").toDouble();
    }
    return 0.0;
}
