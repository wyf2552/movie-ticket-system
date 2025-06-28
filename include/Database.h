#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

class Database {
private:
    std::string _host;
    std::string _user;
    std::string _password;
    std::string _database;

    // 数据库连接对象
    sql::Driver *_driver;
    std::unique_ptr<sql::Connection> _conn;
public:
    Database(const std::string& host, const std::string& user,
             const std::string& password, const std::string& database);

    ~Database();

    bool connect();

    void setSchema(const std::string& schema);

    void disconnect();

    bool execute(const std::string& query);

    std::unique_ptr<sql::ResultSet> query(const std::string& query);

    std::unique_ptr<sql::PreparedStatement>prepareStatement(const std::string& query);

    void beginTransaction();

    void commit();

    void rollback();

    int getLastInsertId();
};

#endif