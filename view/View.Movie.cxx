module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>

import entities;
import view.helper;
import service.user;
import view.auth;
import service.movie;
import service.screening;

export module view.movie;

export class MovieView {
private:
    MovieService& _movieService;
    ScreeningService& _screeningService;
    void addMovie();
    void modifyMovie();
    void deleteMovie();
    void viewAllMovies();
    void displayMovieList(const std::vector<MovieUptr>& movies);
    void displayMovieDetail(const MovieUptr& movie);
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

    std::vector<MovieUptr> movies;

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
        std::cout << std::left << std::setw(5) << movie->movieId << "|"
                  << std::setw(30) << movie->title << "|"
                  << std::setw(10) << movie->movieType << "|"
                  << std::setw(10) << movie->duration << "|"
                  << std::setw(15) << movie->releaseDate << "|"
                  << std::setw(10) << static_cast<int>(movie->status)  << std::endl;
    }
    ViewHelper::showSeparator();
}

void MovieView::displayMovieDetail(const MovieUptr& movie) {
    std::cout << "电影ID: " << movie->movieId << std::endl;
    std::cout << "电影名称: " << movie->title << std::endl;
    std::cout << "导演: " << movie->director << std::endl;
    std::cout << "主演: " << movie->actors << std::endl;
    std::cout << "类型: " << movie->movieType << std::endl;
    std::cout << "时长: " << movie->duration << " 分钟" << std::endl;
    std::cout << "上映日期: " << movie->releaseDate << std::endl;
    std::cout << "语言: " << movie->language << std::endl;
    std::cout << "国家/地区: " << movie->country << std::endl;
    std::cout << "评分: " << movie->rating << std::endl;
    std::cout << "状态: " << static_cast<int>(movie->status) << std::endl;
    ViewHelper::showSeparator();
    std::cout << "剧情简介: " << std::endl << movie->synopsis << std::endl;
}

void MovieView::displayScreenings(int movieId) {
    ViewHelper::showSeparator();
    std::cout << "排片信息: " << std::endl;

    auto screenings = _screeningService.getScreeningsByMovieId(movieId);

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
            std::cout << std::left << std::setw(5) << screening->screeningId << "|"
                      << std::setw(20) << screening->cinemaName << "|"
                      << std::setw(10) << screening->hallName << "|"
                      << std::setw(20) << screening->startTime << "|"
                      << std::setw(10) << screening->price << "|"
                      << std::setw(10) << screening->languageVersion << std::endl;
        }
    }
}

void MovieView::addMovie() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("添加电影");

    std::string title = ViewHelper::readString("电影名称: ");
    if (title.empty()) {
        ViewHelper::showError("电影名称不能为空!");
        ViewHelper::waitForKeyPress();
        return;
    }

    std::string director = ViewHelper::readString("导演: ");
    std::string actors = ViewHelper::readString("主演: ");
    std::string movieType = ViewHelper::readString("类型: ");
    int duration = ViewHelper::readInt("时长(分钟): ");
    std::string releaseDate = ViewHelper::readString("上映日期 (YYYY-MM-DD): ");
    std::string language = ViewHelper::readString("语言: ");
    std::string country = ViewHelper::readString("国家/地区: ");
    std::string synopsis = ViewHelper::readString("剧情简介: ");
    std::string poster = ViewHelper::readString("海报URL: ");
    double rating = ViewHelper::readDouble("评分 (0-10): ");

    std::cout << "\n电影状态:" << std::endl;
    std::cout << "0 - 下架" << std::endl;
    std::cout << "1 - 在映" << std::endl;
    std::cout << "2 - 即将上映" << std::endl;
    int status = ViewHelper::readInt("请选择状态: ", 1);

    Movie movie;
    movie.title = title;
    movie.director = director;
    movie.actors = actors;
    movie.movieType = movieType;
    movie.duration = duration;
    movie.releaseDate = releaseDate;
    movie.language = language;
    movie.country = country;
    movie.synopsis = synopsis;
    movie.poster = poster;
    movie.rating = rating;
    movie.status = static_cast<Movie::Status>(status);

    if (_movieService.addMovie(movie)) {
        ViewHelper::showSuccess("电影添加成! ID: " + std::to_string(movie.movieId));
    } else {
        ViewHelper::showError("添加电影失败!");
    }
    ViewHelper::waitForKeyPress();
}

void MovieView::modifyMovie() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("修改电影");

    int movieId = ViewHelper::readInt("请输入电影ID: ");

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
    ViewHelper::showMenuTitle("修改电影 -" + movie->title);

    std::string title = ViewHelper::readString("电影名称 [" + movie->title + "]: ", movie->title);
    std::string director = ViewHelper::readString("导演 [" + movie->director + "]: ", movie->director);
    std::string actors = ViewHelper::readString("主演 [" + movie->actors + "]: ", movie->actors);
    std::string movieType = ViewHelper::readString("类型 [" + movie->movieType + "]: ", movie->movieType);
    int duration = ViewHelper::readInt("时长(分钟) [" + std::to_string(movie->duration) + "]: ", movie->duration);
    std::string releaseDate = ViewHelper::readString("上映日期 [" + movie->releaseDate + "]: ", movie->releaseDate);
    std::string language = ViewHelper::readString("语言 [" + movie->language + "]: ", movie->language);
    std::string country = ViewHelper::readString("国家/地区 [" + movie->country + "]: ", movie->country);
    std::string synopsis = ViewHelper::readString("剧情简介 [" + movie->synopsis + "]: ", movie->synopsis);
    std::string poster = ViewHelper::readString("海报URL [" + movie->poster + "]: ", movie->poster);
    double rating = ViewHelper::readDouble("评分 [" + std::to_string(movie->rating) + "]: ", movie->rating);

    std::cout << "\n电影状态:" << std::endl;
    std::cout << "0 - 下架" << std::endl;
    std::cout << "1 - 在映" << std::endl;
    std::cout << "2 - 即将上映" << std::endl;
    int status = ViewHelper::readInt("请选择状态 [" + std::to_string(static_cast<int>(movie->status)) + "]: ",  static_cast<int>(movie->status));

    movie->title = title;
    movie->director = director;
    movie->actors = actors;
    movie->movieType = movieType;
    movie->duration = duration;
    movie->releaseDate = releaseDate;
    movie->language = language;
    movie->country = country;
    movie->synopsis = synopsis;
    movie->poster = poster;
    movie->rating = rating;
    movie->status = static_cast<Movie::Status>(status);

    if (_movieService.updateMovie(*movie)) {
        ViewHelper::showSuccess("电影信息已更新!");
    } else {
        ViewHelper::showError("更新电影信息失败!");
    }

    ViewHelper::waitForKeyPress();
}

void MovieView::deleteMovie() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("删除电影");

    int movieId = ViewHelper::readInt("请输入电影Id: ");

    if (movieId <= 0) {
        ViewHelper::showError("无效的电影IID!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto movie = _movieService.getMovieById(movieId);

    if (!movie) {
        ViewHelper::showError("未找到电影!");
        ViewHelper::waitForKeyPress();
        return;
    }

    std::cout << "即将删除电影: " << movie->title << std::endl;

    // delete movie;

    if (!ViewHelper::confirm("确定要删除这部电影吗?")) {
        ViewHelper::showInfo("删除操作已取消。");
        ViewHelper::waitForKeyPress();
        return;
    }

    if (_movieService.deleteMovie(movieId)) {
        ViewHelper::showSuccess("电影已删除!");
    } else {
        ViewHelper::showError("删除电影失败!可能存在关联排片。");

    }
    ViewHelper::waitForKeyPress();
}

void MovieView::viewAllMovies() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("所有电影");

    auto movies = _movieService.getAllMovies();

    if (movies.empty()) {
        ViewHelper::showInfo("没有电影记录!");
        ViewHelper::waitForKeyPress();
        return;
    }

    std::cout << std::setw(30) << std::setw(5) << "ID" << "|"
              << std::setw(30) << "电影名称" << "|"
              << std::setw(15) << "导演" << "|"
              << std::setw(10) << "类型" << "|"
              << std::setw(10) << "时长(分钟)" << "|"
              << std::setw(12) << "上映日期" << "|"
              << std::setw(10) << "评分" << "|"
              << std::setw(10) << "状态" << std::endl;

    ViewHelper::showSeparator();

    for (const auto& movie : movies) {
        std::cout << std::left << std::setw(5) << movie->movieId << "|"
                  << std::setw(30) << movie->title << "|"
                  << std::setw(15) << movie->director << "|"
                  << std::setw(10) << movie->movieType << "|"
                  << std::setw(10) << movie->duration << "|"
                  << std::setw(12) << movie->releaseDate << "|"
                  << std::setw(10) << movie->rating << "|"
                  << std::setw(10) << static_cast<int>(movie->status) << std::endl;
    }
    ViewHelper::showSeparator();

    ViewHelper::waitForKeyPress();
}