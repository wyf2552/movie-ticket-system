module;

#include <ctime>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>

import entities;
import viewhelper;
import orderservice;
import authview;
import movieservice;
import screeningservice;
import movieview;

export module ticketview;

export class TicketView {
private:
    MovieService& _movieService;
    ScreeningService& _screeningService;
    OrderService& _orderService;

    MovieUptr selectMovie();
    ScreeningUptr selectScreening(int movieId, const std::vector<ScreeningUptr>& screenings);
    std::vector<int> selectSeats(int screeningId);
    void displaySeatLayout(const std::vector<ScreeningSeatUptr>& seats);
    OrderUptr confirmOrder(const User& currentUser, int ScreeningId, const std::vector<int>& selectedSeats, const Movie& movie, const Screening& screening);
    bool processPayment(Order& order);

public:
    TicketView(MovieService& movieService, ScreeningService& screeningService, OrderService& orderService);

    void buyTicket(const User& currentUser);
};

TicketView::TicketView(MovieService& movieService, ScreeningService& screeningService, OrderService& orderService) : _movieService(movieService), _screeningService(screeningService), _orderService(orderService) {}

void TicketView::buyTicket(const User& currentUser) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("购票");

    auto movie = selectMovie();
    if (!movie) {
        ViewHelper::showError("未选择有效电影!");
        ViewHelper::waitForKeyPress();
        return;
    }

    if (movie->status != Movie::Status::NowPlaying) {
        ViewHelper::showError("该电影不在上映中!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto screenings = _screeningService.getScreeningByMovieId(movie->movieId);
    if (screening.empty()) {
        ViewHelper::showError("暂无排片信息!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto selectedSeats = selectSeats(screening->screeningId);
    if (selectedSeats.empty()) {
        ViewHelper::showError("未选择有效座位!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto order = confirmOrder(currentUser, screening->screeningId, selectedSeats, *movie, *screening);
    if (!order) {
        ViewHelper::showError("创建订单失败!作为可能已经被其他用户选择。");
        ViewHelper::waitForKeyPress();
        return;
    }

    if (procesPayMent(*order)) {
        ViewHelper::showSuccess("购票成功!");
    } else {
        ViewHelper::showInfo("您可以稍后在"我的订单"中完成支付。");
    }
    ViewHelper::waitForKeyPress();
}

MovieUptr TicketView::selectMovie() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("选择电影");

    auto movies = _movieService.getNowPlayingMovies();
    if (movies.empty()) {
        ViewHelper::showInfo("暂无正在上映的电影!");
        return nullptr;
    }

    std::cout << std::left << std::setw(5) << "ID" << "|"
              << std::setw(30) << "电影名称" << "|"
              << std::setw(10) << "类型" << "|"
              << std::setw(10) << "时长(分钟)" << "|"
              << std::setw(15) << "上映日期" << std::endl;

    ViewHelper::showSeparator();

    for (const auto& movie : movies) {
        std::cout << std::left << std::setw(5) << movie->movieId << "|"
                  << std::setw(30) << movie->title << "|"
                  << std::setw(10) << movie->movieType << "|"
                  << std::setw(10) << movie->duration << "|"
                  << std::setw(15) << movie->releaseDate << std::endl;
    }

    ViewHelper::showSeparator();

    int movieId = ViewHelper::readInt("请输入电影ID(0取消): ");
    if (movieId <= 0) {
        return nullptr;
    }

    for (auto& movie : movies) {
        if (movie->movieId == movieId) {
            return std::move(movie);
        }
    }
    return nullptr;
}

ScreeningUptr TicketView::selectScreening(int movieId, const std::vector<ScreeningUptr>& screenings) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("选择排片");

    std::cout << std::left << std::setw(5) << "ID" << "|"
              << std::setw(20) << "影院" << "|"
              << std::setw(10) << "影厅" << "|"
              << std::setw(20) << "放映时间" << "|"
              << std::setw(10) << "票价" << "|"
              << std::setw(10) << "语言版本" << std::endl;

    ViewHelper::showSeparator();

    for (const auto& screening : screenings) {
        sdt::cout << std::left << std::setw(5) << screening->screeningId << "|"
                  << std::setw(20) << screening->cinemaName << "|"
                  << std::setw(10) << screening->hallName << "|"
                  << std::setw(20) << screening->startTime << "|"
                  << std::setw(10) << screening->price << "|"
                  << std::setw(10) << screening->languageVersion << std::endl;
    }

    ViewHelper::showSeparator();

    int screeningId = ViewHelper::readInt("请输入排片Id(0取消):");
    if (screeningid <= 0) {
        return nullptr;
    }

    for (const auto& screening : screenings) {
        if (screening->screeningId == screeningId) {
            return _screeningService.getScreeningById(screeningId);
        }
    }
    return nullptr;
}