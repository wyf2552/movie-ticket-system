module;


#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

export module userservice;
import database;
import entities;


export class UserService {
private:
    Database& _db;

public:
    UserService(Database& database);

    bool registerUser(User& user);

    User* login(const std::string& username, const std::string& password);

    User* getUserById(int UserId);

    std::vector<User*> getAllUsers();

    bool updateUser(const User& user);

    bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);

    bool deleteUser(int UserId);
};

UserService::UserService(Database& database) : _db(database) {}

bool UserService::registerUser(User& user) {
    try {
        auto pstmt = _db.prepareStatement("SELECT COUNT(*) FROM User WHERE username = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, user.username);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "用户名已存在!" << std::endl;
            return false;
        }

        pstmt = _db.prepareStatement(
            "INSERT INTO user (username, password, real_name, gender, phone, email, reg_time, user_status, user_type) VALUES (?, ?, ?, ?, ?, ?, NOW(), 1, ?)"
        );
        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, user.username);
        pstmt->setString(2, user.password);
        pstmt->setString(3, user.realName);
        pstmt->setString(4, user.gender);
        pstmt->setString(5, user.phone);
        pstmt->setString(6, user.email);
        pstmt->setInt(7, user.userType);

        pstmt->executeUpdate();

        // user.userId = _db.getLastInsertId();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "User Registration Error: " << e.what() << std::endl;
        return false;
    }
}

// User* UserService::login(const std::string& username, const std::string& password) {
//     try {
//         auto pstmt = _db.prepareStatement
//     }
// }