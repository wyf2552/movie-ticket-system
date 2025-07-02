import movieservice;
import database;
import entities;
import userservice;

#include <iostream>
#include <cppconn/resultset.h>

int main() {
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    if (db.connect()) {
        db.execute("create database if not exists moviesystem");
        db.setSchema("moviesystem");
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status varchar(20), user_type varchar(20))");
        db.execute("create table if not exists movie(movieid varchar(20), title varchar(100), director varchar(50), actors varchar(200), movietype varchar(50), duration int, releasedate date, language varchar(30), country varchar(50), synopsis text, poster varchar(225), rating decimal(3, 1), status varchar(20))");

        Movie movie(1, "盗梦空间", "克里斯托弗·诺兰",
                 "莱昂纳多·迪卡普里奥,玛丽昂·歌迪亚",
                 "科幻/动作", 148, "2010-07-16",
                 "英语", "美国",
                 "一个小偷通过梦境分享技术窃取企业机密...",
                 "inception_poster.jpg", 8.8,
                 Movie::Status::onShow);
        MovieService movieservice(db);
        movieservice.addMovie(movie);
        movieservice.getMovieById(1);
    }
}