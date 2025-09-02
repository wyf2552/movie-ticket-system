import service.user;
import database;
import entities;

#include <iostream>
#include <cppconn/resultset.h>

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status varchar(20), user_type varchar(20))");
        db.execute("CREATE TABLE IF NOT EXISTS Orders(order_id INT PRIMARY KEY AUTO_INCREMENT, order_no VARCHAR(50) UNIQUE NOT NULL, user_id INT NOT NULL, screening_id INT NOT NULL, total_amount DECIMAL(10,2) NOT NULL, create_time DATETIME NOT NULL, pay_time DATETIME, pay_method TINYINT NOT NULL, order_status TINYINT NOT NULL )");
        UserService userservice(db);
        User user1(1, "tom", "pwd", "tom", "f", "phone", "email", User::Type::regular);
        User user2(2, "bob", "pwd", "bob", "m", "mphone", "gmail", User::Type::regular);
        Order order(0, "ORD20221212001", 1001, 2001, 99.50, "2022-12-12 10:30:00", "2022-12-12 10:35:00", Order::PayMethod::alipay,Order::Status::paid);
        userservice.registerUser(user1);


        userservice.login("bob", "pwd");
        userservice.getUserById(1);
        userservice.getAllUsers();
        userservice.updateUser(user2);
        userservice.changePassword(1, "pwd", "pad");
        userservice.deleteUser(1);
    }
}