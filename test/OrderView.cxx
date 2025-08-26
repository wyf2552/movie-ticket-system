#include <iostream>
#include <memory>

import database;
import entities;
import viewhelper;
import userservice;
import cinemaservice;
import orderservice;
import authview;
import movieservice;
import screeningservice;
import movieview;
import ticketview;
import orderview;

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status int, user_type varchar(20))");
        db.execute("create table if not exists movie(movie_id varchar(20), title varchar(100), director varchar(50), actors varchar(200), movie_type varchar(50), duration int, release_date date, language varchar(30), country varchar(50), synopsis text, poster varchar(225), rating decimal(3, 1), status int)");
        db.execute("create table if not exists screening(screening_id varchar(20), movie_id varchar(20), cinema_id varchar(20), hall_id varchar(20), start_time date, end_time date, price decimal(5,2), language_version varchar(20),  status int)");
        db.execute("create table if not exists `order` (order_id int, order_no varchar(20), user_id int, screening_id int, total_amount decimal(4, 2), create_time date, pay_time date, pay_method int, status int, user_name varchar(20), movie_title varchar(20), cinema_name varchar(20), hall_name varchar(20), start_time varchar(20), seat_positions decimal(2,1))");

        Order order(1, "1001", 1, 1, 23.23, "2023-12-12", "2024-12-12", Order::PayMethod::alipay, Order::Status::paid);
        User currentuser(1, "tom", "pwd", "tom", "f", "phone", "email", User::Type::regular);

        auto orderservice = std::make_shared<OrderService>(db);

        OrderView orderview(*orderservice);
        orderview.showMyOrders(currentuser);
        orderview.showOrderDetail(order);
        orderview.showOrderManagement();
        orderview.orderStatustics();
    }
}