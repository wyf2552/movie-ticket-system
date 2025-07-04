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
        db.execute("create table if not exists user(user_id varchar(20), username varchar(20), password varchar(20), real_name varchar(20), gender varchar(5), phone varchar(11), email varchar(20), reg_time date, last_login date, user_status int, user_type varchar(20))");
        db.execute("create table if not exists movie(movie_id varchar(20), title varchar(100), director varchar(50), actors varchar(200), movie_type varchar(50), duration int, release_date date, language varchar(30), country varchar(50), synopsis text, poster varchar(225), rating decimal(3, 1), status int)");
        db.execute("create table if not exists screening(screening_id varchar(20), movie_id varchar(20), cinema_id varchar(20), hall_id varchar(20), start_time date, end_time date, price decimal(5,2), language_version varchar(20),  status int)");


        MovieService movieservice(db);
        movieservice.addMovie(movie);
        movieservice.getMovieById(1);
        movieservice.getAllMovies();
        movieservice.getNowPlayingMovies();
        movieservice.getComingSoonMovies();
        movieservice.updateMovie(movie1);
        movieservice.deleteMovie(1);
        movieservice.searchMovies("马克");
    }
}