#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QString>
#include <QSqlDatabase>

class DatabaseManager
{
public:
    DatabaseManager();
    static bool initDatabase(const QString &dbPath = "students.db");
    static QSqlDatabase getDatabase();
};

#endif // DATABASEMANAGER_H
