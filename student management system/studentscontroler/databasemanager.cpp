#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static QSqlDatabase s_db;

bool DatabaseManager::initDatabase(const QString &dbPath)
{
    s_db = QSqlDatabase::addDatabase("QSQLITE");
    s_db.setDatabaseName(dbPath);

    if (!s_db.open()) {
        qDebug() << "❌ 数据库打开失败:" << s_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("PRAGMA encoding = \"UTF-8\"");

    // 创建用户表
    QString createUsers = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            role TEXT NOT NULL
        )
    )";
    if (!query.exec(createUsers)) {
        qDebug() << "❌ 创建 users 表失败:" << query.lastError().text();
        return false;
    }

    // ========== ✅ 添加锁定相关列（兼容旧数据库） ==========
    query.exec("ALTER TABLE users ADD COLUMN login_fail_count INTEGER DEFAULT 0");
    query.exec("ALTER TABLE users ADD COLUMN lock_time TEXT DEFAULT ''");
    // ============================================================

    // 创建学生表
    QString createStudents = R"(
        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            class_name TEXT,
            gender TEXT,
            birth_date TEXT,
            user_id INTEGER,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    )";
    if (!query.exec(createStudents)) {
        qDebug() << "❌ 创建 students 表失败:" << query.lastError().text();
        return false;
    }

    // 创建成绩表
    QString createScores = R"(
        CREATE TABLE IF NOT EXISTS scores (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            subject TEXT NOT NULL,
            score REAL NOT NULL,
            exam_date TEXT,
            FOREIGN KEY(student_id) REFERENCES students(id)
        )
    )";
    if (!query.exec(createScores)) {
        qDebug() << "❌ 创建 scores 表失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ 数据库初始化成功:" << dbPath;

    // 插入默认管理员账号
    query.prepare("SELECT COUNT(*) FROM users WHERE username = 'admin'");
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        if (count == 0) {
            query.prepare("INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
            query.addBindValue("admin");
            query.addBindValue("0192023a7bbd73250516f069df18b500"); // admin123
            query.addBindValue("admin");
            query.exec();
            qDebug() << "✅ 已创建默认管理员账号: admin / admin123";
        }
    }

    return true;
}

QSqlDatabase DatabaseManager::getDatabase()
{
    return s_db;
}
