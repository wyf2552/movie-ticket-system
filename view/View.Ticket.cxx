module;

#include <ctime>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <map>

import entities;
import service.user;
import database;
import view.helper;
import service.order;
import view.auth;
import service.movie;
import service.screening;
import view.movie;

export module view.ticket;

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

    if (movie->status != Movie::Status::onShow) {
        ViewHelper::showError("该电影不在上映中!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto screenings = _screeningService.getScreeningsByMovieId(movie->movieId);
    if (screenings.empty()) {
        ViewHelper::showError("暂无排片信息!");
        ViewHelper::waitForKeyPress();
        return;
    }

    auto screening = selectScreening(movie->movieId, screenings);
    if (!screening) {
        ViewHelper::showError("未选择有效排片！");
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

    if (processPayment(*order)) {
        ViewHelper::showSuccess("购票成功!");
    } else {
        ViewHelper::showInfo("您可以稍后在'我的订单'中完成支付。");
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
        std::cout << std::left << std::setw(5) << screening->screeningId << "|"
                  << std::setw(20) << screening->cinemaName << "|"
                  << std::setw(10) << screening->hallName << "|"
                  << std::setw(20) << screening->startTime << "|"
                  << std::setw(10) << screening->price << "|"
                  << std::setw(10) << screening->languageVersion << std::endl;
    }

    ViewHelper::showSeparator();

    int screeningId = ViewHelper::readInt("请输入排片Id(0取消):");
    if (screeningId <= 0) {
        return nullptr;
    }

    for (const auto& screening : screenings) {
        if (screening->screeningId == screeningId) {
            return _screeningService.getScreeningById(screeningId);
        }
    }
    return nullptr;
}

std::vector<int> TicketView::selectSeats(int screeningId) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("选择座位");

    auto seats = _screeningService.getScreeningSeats(screeningId);
    if (seats.empty()) {
        ViewHelper::showError("获取座位信息失败!");
        return {};
    }

    displaySeatLayout(seats);

    std::cout << "\n请选择座位(格式: 行, 列 例如 3, 5, 多个座位用空格分割): ";
    std::string seatInput;
    std::getline(std::cin, seatInput);

    std::vector<int> selectedScreeningSeatIds;
    std::istringstream iss(seatInput);
    std::string position;

    int maxRow = 0, maxCol = 0;
    for (const auto& seat : seats) {
        maxRow = std::max(maxRow, seat->rowNum);
        maxCol = std::max(maxCol, seat->columnNum);
    }

    std::map<std::pair<int, int>, ScreeningSeat*> seatMap;
    for (auto& seat : seats) {
        seatMap[{seat->rowNum, seat->columnNum}] = seat.get();
    }

    while (iss >> position) {
        size_t commaPos = position.find(',');
        if (commaPos == std::string::npos) {
            continue;
        }

        try {
            int row = std::stoi(position.substr(0, commaPos));
            int col = std::stoi(position.substr(commaPos + 1));

            auto it = seatMap.find({row, col});
            if (it != seatMap.end() && static_cast<int>(it->second->status) == 0) {
                selectedScreeningSeatIds.push_back((it->second)->seatId);
            } else {
                ViewHelper::showError("座位" + std::to_string(row) + "," + std::to_string(col) + "不可选!");
            }
        } catch (...) {
            ViewHelper::showError("座位格式错误:" + position);
        }
    }
    return selectedScreeningSeatIds;
}

void TicketView::displaySeatLayout(const std::vector<ScreeningSeatUptr>& seats) {
    int maxRow = 0, maxCol = 0;
    for (const auto& seat : seats) {
        maxRow = std::max(maxRow, seat->rowNum);
        maxCol = std::max(maxCol, seat->columnNum);
    }

    std::vector<std::vector<const ScreeningSeat*>> seatMatrix(maxRow + 1, std::vector<const ScreeningSeat*>(maxCol + 1, nullptr));

    for (const auto& seat : seats) {
        seatMatrix[seat->rowNum][seat->columnNum] = seat.get();
    }

    std::cout << "\n座位布局图(O: 可选, X: 已售, L: 已锁定):\n" << std::endl;

    std::cout << " ";
    for (int col = 1; col <= maxCol; col++) {
        std::cout << col % 10 << " ";
    }
    std::cout << std::endl;

    for (int row = 1; row <= maxRow; row++) {
        std::cout << row << " ";
        for (int col = 1; col <= maxCol; col++) {
            auto seat = seatMatrix[row][col];
            if (seat) {
                if (seat->status == ScreeningSeat::Status::avilable) {
                    std::cout << "O";
                } else if (seat->status == ScreeningSeat::Status::sold) {
                    std::cout << "X";
                } else {
                    std::cout << "L";
                }
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

OrderUptr TicketView::confirmOrder(const User& currentUser, int screeningId, const std::vector<int>& selectSeats, const Movie& movie, const Screening& screening) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("确认订单");

    std::cout << "电影:" << movie.title << std::endl;
    std::cout << "影院:" << screening.cinemaName << std::endl;
    std::cout << "影厅:" << screening.hallName << std::endl;
    std::cout << "时间:" << screening.startTime << std::endl;
    std::cout << "座位数量:" << selectSeats.size() << std::endl;

    double totalPrice = selectSeats.size() *screening.price;
    std::cout << "总价:" << totalPrice << "元" << std::endl;

    if (!ViewHelper::confirm("确认购买?")) {
        return nullptr;
    }
    return _orderService.createOrder(currentUser.userId, screeningId, selectSeats);
}

bool TicketView::processPayment(Order& order) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("支付订单");

    std::cout << "订单号:" << order.orderNo << std::endl;
    std::cout << "总金额:" << order.totalAmount << "元" << std::endl;

    std::cout << "\n请选择支付方式:" << std::endl;
    std::cout << "1.支付宝" << std::endl;
    std::cout << "2.微信支付" << std::endl;
    std::cout << "3.银行卡" << std::endl;
    std::cout << "0.稍后支付" << std::endl;

    int payMethod = ViewHelper::readInt("请选择:");

    if (payMethod >= 1 && payMethod <= 3) {
        return _orderService.payOrder(order.orderId, payMethod);
    }
    return false;
}