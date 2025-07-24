#include <iostream>
#include <memory>

import userservice;
import authview;
import database;
int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status int, user_type varchar(20))");
    }
    auto userservice = std::make_shared<UserService>(db);

    AuthView authview(userservice);

    authview.showLoginMenu();
    authview.showRegisterMenu();
}