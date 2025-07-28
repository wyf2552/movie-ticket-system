module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>

import entities;
import viewhelper;
import userservice;
import authview;
import movieservice;
import screeningservice;

export module movieview;

export class MovieView {
private:
    MovieService& _movieService;
    ScreeningService& _screeningService;
    void addMovie();
    void modifyMovie();
    void deleteMovie();
    void viewAllMovies();
    void displayMovieList(const std::vector<MovieSptr>& movies);
    void displayMovieDetail(const MovieSptr& movie);
    void displayScreenings(int movieId);

public:
    MovieView(MovieService& movieservice, ScreeningService& screeningService);

    void browseMovies();
    void showMovieDetails();
    void searchMovies();
    void showMovieManagement();
};

MovieView::MovieView(MovieService& movieService, ScreeningService& screeningService) : _movieService(movieService), _screeningService(screeningService) {}

void MovieView::browseMovies() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("浏览电影");

    std::cout << "1.正在上映" << std::endl;
    std::cout << "2.即将上映" << std::endl;
    std::cout << "3.所有电影" << std::endl;
    std::cout << "0.返回" << std::endl;

    int choice = ViewHelper::readInt("\n请选择:");

    std::vector<MovieSptr> movies;

    switch(choice) {
        case 1:
             movies = _movieService.getNowPlayingMovies();
            break;
        case 2:
            movies = _movieService.getComingSoonMovies();
            break;
        case 3:
            movies = _movieService.getAllMovies();
            break;
        case 0:
            return;
        default:
            ViewHelper::showError("无效的选择！");
            ViewHelper::waitForKeyPress();
            browseMovies();
            return;
    }

    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("电影列表");

    if (movies.empty()) {
        ViewHelper::showInfo("没有找到电影!");
        ViewHelper::waitForKeyPress();
        return;
    }

    displayMovieList(movies);

    ViewHelper::waitForKeyPress();
}

void MovieView::showMovieDetails() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("电影详情");

    int movieId = ViewHelper::readInt("请输入电影ID:");

    if (movieId <= 0) {
        ViewHelper::showError("无效的电影ID!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto movie = _movieService.getMovieById(movieId);

    if (!movie) {
        ViewHelper::showError("未找到电影!");
        ViewHelper::waitForKeyPress();
        return;
    }

    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("电影详情 - " + movie->title);

    displayMovieDetail(movie);

    displayScreenings(movieId);

    ViewHelper::waitForKeyPress();
}

void MovieView::searchMovies() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("搜索电影");

    std::string keyword = ViewHelper::readString("请输入搜索关键词:");

    if (keyword.empty()) {
        ViewHelper::showError("关键词不能为空!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto movies = _movieService.searchMovies(keyword);

    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("搜索结果 - 关键词:" + keyword);

    if (movies.empty()) {
        ViewHelper::showInfo("没有找到匹配的电影!");
        ViewHelper::waitForKeyPress();
        return;
    }

    displayMovieList(movies);

    ViewHelper::waitForKeyPress();
}

