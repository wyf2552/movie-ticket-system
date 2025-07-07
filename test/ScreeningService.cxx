import database;
import entities;
import userservice;
import movieservice;
import cinemaservice;
import screeningservice;

#include <iostream>
#include <cppconn/resultset.h>

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status int, user_type varchar(20))");
        db.execute("create table if not exists movie(movie_id varchar(20), title varchar(100), director varchar(50), actors varchar(200), movie_type varchar(50), duration int, release_date date, language varchar(30), country varchar(50), synopsis text, poster varchar(225), rating decimal(3, 1), status int)");
        db.execute("create table if not exists screening(screening_id varchar(20), movie_id varchar(20), cinema_id varchar(20), hall_id varchar(20), start_time date, end_time date, price decimal(5,2), language_version varchar(20),  status int)");
        db.execute("create table if not exists cinema(cinema_id int, cinema_name varchar(20), address varchar(20), phone varchar(20), introduction text, status int)");
        db.execute("create table if not exists hall(hall_id int, cinema_id int, hall_name varchar(20), seat_count int, hall_type varchar(20), status int)");
        db.execute("create table if not exists seat(seat_id int, hall_id int, row_num int, column_num int, seat_type int, status int)");
        db.execute("create table if not exists screening(screening_id int, movie_id int, cinema_id int, hall_id int, start_time date, end_time, date, price decimal(4, 2), language_version varchar(20), status int, movie_title varchar(20), cinema_name varchar(20), hall_name varchar(20))");
        db.execute("create table if not exists screeningseat(screening_seat_id int, screening_id int, seat_id int, status int, lock_time date, lock_user_id int, row_num int, column_num int)");

        Movie movie(1, "盗梦空间", "克里斯托弗·诺兰",
                 "莱昂纳多·迪卡普里奥,玛丽昂·歌迪亚",
                 "科幻/动作", 148, "2010-07-16",
                 "英语", "美国",
                 "一个小偷通过梦境分享技术窃取企业机密...",
                 "inception_poster.jpg", 8.8,
                 Movie::Status::onShow);
        Movie movie1(2, "灵笼", "马克", "白冰", "末世", 132, "2025-5-23", "中文", "中国", "等等等", "inception_poster.jpg", 9.0, Movie::Status::onShow);

        Cinema cinema(1, "万达影业", "长江路222号", "1234567", "等等等", Cinema::Status::open);
        Cinema cinema1(2, "中影影业", "大江路222号", "1234577", "等等等", Cinema::Status::open);

        Hall hall(1, 1, "1号厅", 2, "3D", Hall::Status::normal);
        Hall hall1(2, 2, "2号厅", 3, "3D", Hall::Status::normal);

        Seat seat(1, 1, 10, 10, Seat::SeatType::normal, Seat::Status::normal);

        Screening screening(1, 1, 1, 1, 2024-12-12, 2025-01-12, 34.52, "简体中文", Screening::Status::normal, "灵笼", "万达影业", "1号厅");

        ScreeningService screeningservice(db);
        screeningservice.addScreening(screening);
    }
}