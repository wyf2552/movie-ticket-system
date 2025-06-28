#include "Database.h"
#include <stdexcept>

Database::Database(const std::string& host, const std::string& user,
            const std::string& password, const std::string& database)
    : _host(host), _user(user), _password(password), _database(database), _driver(nullptr) {
}

Database::~Database() {
    disconnect();
}

void Database::setSchema(const std::string& schema) {
    _conn->setSchema(schema);
}

bool Database::connect() {
    try {
        // 获取MySQL连接驱动
        _driver = get_driver_instance();

        // 创建连接
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