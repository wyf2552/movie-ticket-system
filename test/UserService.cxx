import userservice;
import database;
import entities;

#include <iostream>
#include <cppconn/resultset.h>

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(uerid varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, user_status varchar(20), user_type varchar(20))");
        UserService userservice(db);
        User user;
        // User user(1, "tom", "pwd", "tom", "f", "phone", "email");
        userservice.registerUser(user);
    }
}