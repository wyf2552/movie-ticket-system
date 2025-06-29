module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

export module Database;

export class Database {
private:
    std::string _host;
    std::string _user;
    std::string _password;
    std::string _database;
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

Database::Database(const std::string& host, const std::string& user,
            const std::string& password, const std::string& database)
    : _host(host), _user(user), _password(password), _database(database), _driver(nullptr)
    { }

Database::~Database() {
    disconnect();
}

void Database::setSchema(const std::string& schema) {
    _conn->setSchema(schema);
}

bool Database::connect() {
    try {
        _driver = get_driver_instance();
        _conn.reset(_driver->connect(_host, _user, _password));
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Database Connection Error: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    }
}

void Database::disconnect() {
    if (_conn) {
        _conn.reset();
    }
}

bool Database::execute(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(_conn->createStatement());
        return stmt->execute(query);
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Execute Error: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    }
}

std::unique_ptr<sql::ResultSet> Database::query(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(_conn->createStatement());
        return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Query Error: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<sql::PreparedStatement> Database::prepareStatement(const std::string& query) {
    try {
        return std::unique_ptr<sql::PreparedStatement>(_conn->prepareStatement(query));
    } catch (sql::SQLException &e) {
        std::cerr << "Prepare Statement Error: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return nullptr;
    }
}

void Database::beginTransaction() {
    execute("START TRANSACTION");
}

void Database::commit() {
    execute("COMMIT");
}

void Database::rollback() {
    execute("ROLLBACK");
}

int Database::getLastInsertId() {
    auto rs = query("SELECT LAST_INSERT_ID()");
    if (rs && rs->next()) {
        return rs->getInt(1);
    }
    return -1;
}