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

void MovieView::showMovieManagement() {
    while(true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("电影管理");

        std::cout << "1.添加电影" << std::endl;
        std::cout << "2.修改电影" << std::endl;
        std::cout << "3.删除电影" << std::endl;
        std::cout << "4.查看所有电影" << std::endl;
        std::cout << "0.返回" << std::endl;

        int choice = ViewHelper::readInt("\n请选择: ");

        switch(choice) {
            case 1:
                addMovie();
                break;
            case 2:
                modifyMovie();
                break;
            case 3:
                deleteMovie();
                break;
            case 4:
                viewAllMovies();
                break;

            case 0:
                return;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MovieView::displayMovieList(const std::vector<MovieUptr>& movies) {
    std::cout << std::left << std::setw(5) << "ID" << "|"
              << std::setw(30) << "电影名称" << "|"
              << std::setw(10) << "类型" << "|"
              << std::setw(10) << "时长(分钟)" << "|"
              << std::setw(15) << "上映日期" << "|"
              << std::setw(10) << "状态" << std::endl;
    ViewHelper::showSeparator();

    for (const auto& movie : movies) {
        std::cout << std::left << std::setw(5) << movie->getMovieId() << "|"
                  << std::setw(30) << movie->getTitle() << "|"
                  << std::setw(10) << movie->getMovieType() << "|"
                  << std::setw(10) << movie->getDuration() << "|"
                  << std::setw(15) << movie->getReleaseDate() << "|"
                  << std::setw(10) << movie->getStatusDescription() << std::endl;
    }
    ViewHelper::showSeparator();
}

void MovieView::displayMovieDetail(const Movie& movie) {
    std::cout << "电影ID: " << movie.movieId << std::endl;
    std::cout << "电影名称: " << movie.title << std::endl;
    std::cout << "导演: " << movie.director << std::endl;
    std::cout << "主演: " << movie.actors << std::endl;
    std::cout << "类型: " << movie.movieType << std::endl;
    std::cout << "时长: " << movie.duration << " 分钟" << std::endl;
    std::cout << "上映日期: " << movie.releaseDate << std::endl;
    std::cout << "语言: " << movie.language << std::endl;
    std::cout << "国家/地区: " << movie.country << std::endl;
    std::cout << "评分: " << movie.rating << std::endl;
    std::cout << "状态: " << movie.statusDescription << std::endl;
    ViewHelper::showSeparator();
    std::cout << "剧情简介: " << std::endl << movie.synopsis() << std::endl;
}

void MovieView::displayScreenings(int movieId) {
    ViewHelper::showSeparator();
    std::cout << "排片信息: " << std::endl;

    auto screenings = screeningService.getScreeningsByMovieId(movieId);

    if (screenings.empty()) {
        std::cout << "暂无排片信息!" << std::endl;
    } else {
        std::cout << std::left << std::setw(5) << "ID" << "|"
                  << std::setw(20) << "影院" << "|"
                  << std::setw(10) << "影厅" << "|"
                  << std::setw(20) << "放映时间" << "|"
                  << std::setw(10) << "票价" << "|"
                  << std::setw(10) << "语言版本" << std::endl;

        ViewHelper::showSeparator();

        for (const auto& screening : screenings) {
            std::cout << std::left << std::setw(5) << screening->getScreeningId() << "|"
                      << std::setw(20) << screening->getCinemaName() << "|"
                      << std::setw(10) << screening->getHallName() << "|"
                      << std::setw(20) << screening->getStartTime() << "|"
                      << std::setw(10) << screening->getPrice() << "|"
                      << std::setw(10) << screening->getLanguageVersion() << std::endl;
        }
    }
}