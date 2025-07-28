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

    UserUptr login(const std::string& username, const std::string& password);

    UserUptr getUserById(int UserId);

    std::vector<UserUptr> getAllUsers();

    bool updateUser(const User& user);

    bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);

    bool deleteUser(int UserId);
};

export using UserServiceSptr = std::shared_ptr<UserService>;

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

UserUptr UserService::login(const std::string& username, const std::string& password) {
    try {
        auto pstmt = _db.prepareStatement(
            "select * from User where username = ? and password = ? and user_status = 1"
        );
        if (!pstmt) {
            return nullptr;
        }
        pstmt->setString(1, username);
        pstmt->setString(2, password);

        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            UserUptr user = std::make_unique<User>();
            user->userId = rs->getInt("user_id");
            user->password = rs->getString("password");
            user->realName = rs->getString("real_name");
            user->gender = rs->getString("gender");
            user->phone = rs->getString("phone");
            user->email = rs->getString("email");
            user->regTime = rs->getString("reg_time");
            user->lastLogin = rs->getString("last_login");
            user->userStatus = rs->getInt("user_status");
            user->userType = rs->getInt("user_type");

            pstmt = _db.prepareStatement("update User set last_login = NOW() where user_id = ?");
            if (pstmt) {
                pstmt->setInt(1, user->userId);
                pstmt->executeUpdate();
            }
            return user;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "User Login Error: " << e.what() << std::endl;
    }

    return nullptr;
}

UserUptr UserService::getUserById(int userId) {
    try {
        auto pstmt = _db.prepareStatement("select * from User where user_id = ?");
        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, userId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            auto user = std::make_unique<User>();
            user->userId = rs->getInt("user_id");
            user->username = rs->getString("username");
            user->password = rs->getString("password");
            user->realName = rs->getString("real_name");
            user->gender = rs->getString("gender");
            user->phone = rs->getString("phone");
            user->email = rs->getString("email");
            user->regTime = rs->getString("reg_time");
            user->lastLogin = rs->getString("last_login");
            user->userStatus = rs->getInt("user_status");
            user->userType = rs->getInt("user_type");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "Get User Error: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<UserUptr> UserService::getAllUsers() {
    std::vector<UserUptr> users;
    try {
        auto rs = _db.query("select * from User order by user_id");

        if (rs) {
            while (rs->next()) {
                auto user = std::make_unique<User>();
                user->userId = rs->getInt("user_id");
                user->username = rs->getString("username");
                user->password = rs->getString("password");
                user->realName = rs->getString("real_name");
                user->gender = rs->getString("gender");
                user->phone = rs->getString("phone");
                user->email = rs->getString("email");
                user->regTime = rs->getString("reg_time");
                user->lastLogin = rs->getString("last_login");
                user->userStatus = rs->getInt("user_status");
                user->userType = rs->getInt("user_type");
                users.push_back(user);
            }
        }
    } catch (sql::SQLException &e) {
        std ::cerr << "Get All Users Error: " << e.what() << std::endl;
    }
    return users;
}

bool UserService::updateUser(const User& user) {
    try {
        auto pstmt = _db.prepareStatement(
            "update User set real_name = ?, gender = ?, phone = ?, email = ?, user_status = ?, user_type = ? where user_id = ?"
        );
        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, user.realName);
        pstmt->setString(2, user.gender);
        pstmt->setString(3, user.phone);
        pstmt->setString(4, user.email);
        pstmt->setInt(5, user.userStatus);
        pstmt->setInt(6, user.userType);
        pstmt->setInt(7, user.userId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update User Error: " << e.what() << std::endl;
        return false;
    }
}

bool UserService::changePassword(int userId, const std::string& oldPassword, const std::string& newPassword) {
    try {
        auto pstmt = _db.prepareStatement("select password from User where user_id = ?");
        if (!pstmt) {
            return false;
        }
        pstmt->setInt(1, userId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            std::string currentPassword = rs->getString("password");
            if (currentPassword != oldPassword) {
                std::cout << "旧密码不正确！" << std::endl;
                return false;

            }

            pstmt = _db.prepareStatement("update User set password = ? where user_id = ?");
            if (!pstmt) {
                return false;
            }

            pstmt->setString(1, newPassword);
            pstmt->setInt(2, userId);

            return pstmt->executeUpdate() > 0;
        }
        return false;
    } catch (sql::SQLException &e) {
        std::cerr << "Change Password Error: " << e.what() << std::endl;
        return false;
    }
}

bool UserService::deleteUser(int userId) {
    try {
        auto pstmt = _db.prepareStatement("select count(*) from Orders where user_id = ?");
        if (!pstmt) {
            return false;
        }
        pstmt->setInt(1, userId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "用户有关联订单,无法删除!" << std::endl;
            return false;
        }

        pstmt = _db.prepareStatement("delete from User where User_id = ?");
        if (!pstmt) {
            return false;
        }
        pstmt->setInt(1, userId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Delete User Error: " << e.what() << std::endl;
        return false;
    }
}