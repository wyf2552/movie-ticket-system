#include <iostream>
#include <memory>

import service.user;
import view.auth;
import database;
import service.screening;
import service.movie;
import entities;
import view.movie;
import view.helper;

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status int, user_type varchar(20))");
        db.execute("create table if not exists movie(movie_id varchar(20), title varchar(100), director varchar(50), actors varchar(200), movie_type varchar(50), duration int, release_date date, language varchar(30), country varchar(50), synopsis text, poster varchar(225), rating decimal(3, 1), status int)");
        db.execute("create table if not exists screening(screening_id varchar(20), movie_id varchar(20), cinema_id varchar(20), hall_id varchar(20), start_time date, end_time date, price decimal(5,2), language_version varchar(20),  status int)");

        Movie movie(1, "盗梦空间", "克里斯托弗·诺兰",
                 "莱昂纳多·迪卡普里奥,玛丽昂·歌迪亚",
                 "科幻/动作", 148, "2010-07-16",
                 "英语", "美国",
                 "一个小偷通过梦境分享技术窃取企业机密...",
                 "inception_poster.jpg", 8.8,
                 Movie::Status::onShow);
        Movie movie1(2, "灵笼", "马克", "白冰", "末世", 132, "2025-5-23", "中文", "中国", "等等等", "inception_poster.jpg", 9.0, Movie::Status::onShow);

        auto movieservice = std::make_shared<MovieService>(db);
        auto screeningservice = std::make_shared<ScreeningService>(db);

        MovieView movieview (*movieservice, *screeningservice);

        movieview.browseMovies();
        movieview.showMovieDetails();
        movieview.searchMovies();
        movieview.showMovieManagement();
        // movieview.displayMovieList(movie);
        // movieview.displayMovieDetail(movie1);
        // movieview.displayScreenings(1);
        // movieview.addMovie();

    }
}