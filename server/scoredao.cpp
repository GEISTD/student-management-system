#include "scoredao.h"
#include "databasemanager.h"  // 注意你的文件名拼写
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// 解析日期，兼容 yyyy-MM-dd 和 yyyy/MM/dd 两种格式
static QDate parseDate(const QString &str) {
    QDate d = QDate::fromString(str, "yyyy-MM-dd");
    if (d.isValid()) return d;
    return QDate::fromString(str, "yyyy/MM/dd");
}

bool ScoreDao::insert(const Score &s)
{
    QSqlQuery query;
    query.prepare("INSERT INTO scores (student_id, subject, score, exam_date) "
                  "VALUES (:student_id, :subject, :score, :exam_date)");
    query.bindValue(":student_id", s.studentId);
    query.bindValue(":subject", s.subject);
    query.bindValue(":score", s.score);
    query.bindValue(":exam_date", s.examDate.toString("yyyy/MM/dd"));

    if (!query.exec()) {
        qDebug() << "❌ 插入成绩失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 新增成绩成功: studentId=" << s.studentId << ", subject=" << s.subject;
    return true;
}

bool ScoreDao::deleteById(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM scores WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "❌ 删除成绩失败:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool ScoreDao::update(const Score &s)
{
    QSqlQuery query;
    query.prepare("UPDATE scores SET student_id=:student_id, subject=:subject, "
                  "score=:score, exam_date=:exam_date WHERE id=:id");
    query.bindValue(":student_id", s.studentId);
    query.bindValue(":subject", s.subject);
    query.bindValue(":score", s.score);
    query.bindValue(":exam_date", s.examDate.toString("yyyy/MM/dd"));
    query.bindValue(":id", s.id);

    if (!query.exec()) {
        qDebug() << "❌ 修改成绩失败:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ 修改成绩成功: id=" << s.id;
    return true;
}

QList<Score> ScoreDao::selectByStudentId(int studentId)
{
    QList<Score> list;
    QSqlQuery query;
    query.prepare("SELECT id, student_id, subject, score, exam_date FROM scores WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        while (query.next()) {
            Score s;
            s.id = query.value(0).toInt();
            s.studentId = query.value(1).toInt();
            s.subject = query.value(2).toString();
            s.score = query.value(3).toDouble();
            s.examDate = parseDate(query.value(4).toString());
            list.append(s);
        }
    }
    return list;
}

double ScoreDao::selectAverageByStudentId(int studentId)
{
    QSqlQuery query;
    query.prepare("SELECT AVG(score) FROM scores WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

