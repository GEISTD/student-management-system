#ifndef FILEEXPORTER_H
#define FILEEXPORTER_H

#include <QList>
#include <QString>
#include <QStringList>
#include "student.h"
#include "score.h"

struct ImportStudentData {
    Student student;
    QString loginUsername;
    QList<Score> scores;   // 该学生对应的成绩列表
    QString errorMsg;
};

struct ImportResult {
    int studentCount = 0;
    int userCreated = 0;
    int scoreCount = 0;          // 成功导入的成绩总数
    QStringList createdAccounts;
    QStringList failedRows;
};

class FileExporter
{
public:
    static bool exportAllToCsv(const QString &filePath);
    static QString generateDefaultFileName(const QString &prefix = "学生数据");
    static QList<ImportStudentData> importFromCsv(const QString &filePath);
    static ImportResult importStudentsFromCsv(const QString &filePath);
};

#endif // FILEEXPORTER_H
