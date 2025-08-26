module;

#include <ctime>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <limits>

import userservice;
import database;
import entities;
import movieservice;
import screeningservice;
import cinemaservice;
import orderservice;
import viewhelper;
import authview;
import movieview;
import orderview;
import ticketview;

export module maincontroller;

export class MainController {
private:
    Database _db;
    std::shared_ptr<UserService> _userService;
    std::shared_ptr<MovieService> _movieService;
    std::shared_ptr<CinemaService> _cinemaService;
    std::shared_ptr<ScreeningService> _screeningService;
    std::shared_ptr<OrderService> _orderService;
    AuthView _authView;
    MovieView _movieView;
    TicketView _ticketView;
    OrderView _orderView;
    UserUptr currentUser;

    void showMainMenu();
    void showUserMenu();
    void showAdminMenu();
    void showPersonalInfo();
    void showCinemaManagement();
    void showScreeningManagement();
    void login();
    void logout();
    void exitSystem();

public:
    MainController();
    ~MainController();

    bool initialize();
    void run();
};

MainController::MainController()
    : _db("tcp://localhost:3306", "root", "123456wyf", "")
    , _userService(std::make_shared<UserService>(_db))
    , _movieService(std::make_shared<MovieService>(_db))
    , _cinemaService(std::make_shared<CinemaService>(_db))
    , _screeningService(std::make_shared<ScreeningService>(_db))
    , _orderService(std::make_shared<OrderService>(_db))
    , _authView(_userService)
    , _movieView(*_movieService, *_screeningService)
    , _ticketView(*_movieService, *_screeningService, *_orderService)
    , _orderView(*_orderService)
{}

MainController::~MainController() {}

bool MainController::initialize() {
    if (!_db.connect()) {
        return false;
    }
    _db.setSchema("moviesystem"); // 设置数据库schema
    return true;
}

void MainController::run() {
    if (!initialize()) {
        std::cerr << "无法连接到数据库，系统退出!" << std::endl;
        return;
    }

    _screeningService->releaseTimeoutSeats();

    showMainMenu();
}

void MainController::showMainMenu() {
    while (true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("电影票务管理系统");

        std::cout << "1. 用户登录" << std::endl;
        std::cout << "2. 用户注册" << std::endl;
        std::cout << "3. 退出系统" << std::endl;

        int choice = ViewHelper::readInt("\n请选择功能: ");

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                if (_authView.showRegisterMenu()) {
                    ViewHelper::showSuccess("注册成功，请登录!");
                    ViewHelper::waitForKeyPress();
                }
                break;
            case 3:
                exitSystem();
                break;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::login() {
    currentUser = _authView.showLoginMenu();

    if (currentUser) {
        if (currentUser->isAdmin()) {
            showAdminMenu();
        } else {
            showUserMenu();
        }
    }
}

void MainController::showUserMenu() {
    while (currentUser) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("用户菜单 - " + currentUser->username);

        std::cout << "1. 浏览电影" << std::endl;
        std::cout << "2. 电影详情" << std::endl;
        std::cout << "3. 搜索电影" << std::endl;
        std::cout << "4. 购票" << std::endl;
        std::cout << "5. 我的订单" << std::endl;
        std::cout << "6. 个人信息" << std::endl;
        std::cout << "0. 退出登录" << std::endl;

        int choice = ViewHelper::readInt("\n请选择功能: ");

        switch (choice) {
            case 1:
                _movieView.browseMovies();
                break;
            case 2:
                _movieView.showMovieDetails();
                break;
            case 3:
                _movieView.searchMovies();
                break;
            case 4:
                _ticketView.buyTicket(*currentUser);
                break;
            case 5:
                _orderView.showMyOrders(*currentUser);
                break;
            case 6:
                showPersonalInfo();
                break;
            case 0:
                logout();
                return;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::showAdminMenu() {
    while (currentUser) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("管理员菜单 - " + currentUser->username);

        std::cout << "1. 电影管理" << std::endl;
        std::cout << "2. 影院管理" << std::endl;
        std::cout << "3. 排片管理" << std::endl;
        std::cout << "4. 订单管理" << std::endl;
        std::cout << "5. 个人信息" << std::endl;
        std::cout << "0. 退出登录" << std::endl;

        int choice = ViewHelper::readInt("\n请选择功能: ");

        switch (choice) {
            case 1:
                _movieView.showMovieManagement();
                break;
            case 2:
                showCinemaManagement();
                break;
            case 3:
                showScreeningManagement();
                break;
            case 4:
                _orderView.showOrderManagement();
                break;
            case 5:
                showPersonalInfo();
                break;
            case 0:
                logout();
                return;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::showCinemaManagement() {
    while (true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("影院管理");

        std::cout << "1. 查看所有影院" << std::endl;
        std::cout << "2. 添加影院" << std::endl;
        std::cout << "3. 修改影院" << std::endl;
        std::cout << "4. 删除影院" << std::endl;
        std::cout << "5. 影厅管理" << std::endl;
        std::cout << "0. 返回" << std::endl;

        int choice = ViewHelper::readInt("\n请选择: ");

        switch (choice) {
            case 1: {
                ViewHelper::clearScreen();
                ViewHelper::showMenuTitle("所有影院");
                auto cinemas = _cinemaService->getAllCinema();
                if (cinemas.empty()) {
                    ViewHelper::showInfo("暂无影院信息!");
                } else {
                    std::cout << std::left << std::setw(5) << "ID" << "|"
                              << std::setw(20) << "影院名称" << "|"
                              << std::setw(30) << "地址" << "|"
                              << std::setw(15) << "电话" << "|"
                              << std::setw(10) << "状态" << std::endl;
                    ViewHelper::showSeparator();
                    for (const auto& cinema : cinemas) {
                        std::cout << std::left << std::setw(5) << cinema->cinemaId << "|"
                                  << std::setw(20) << cinema->cinemaName << "|"
                                  << std::setw(30) << cinema->address << "|"
                                  << std::setw(15) << cinema->phone << "|"
                                  << std::setw(10) << cinema->getStatus() << std::endl;
                    }
                }
                ViewHelper::waitForKeyPress();
                break;
            }
            case 2: {
                ViewHelper::clearScreen();
                ViewHelper::showMenuTitle("添加影院");

                std::string name = ViewHelper::readString("影院名称: ");
                std::string address = ViewHelper::readString("地址: ");
                std::string phone = ViewHelper::readString("电话: ");
                std::string introduction = ViewHelper::readString("简介: ");

                Cinema cinema;
                cinema.cinemaName = name;
                cinema.address = address;
                cinema.phone = phone;
                cinema.introduction = introduction;
                cinema.status = Cinema::Status::open;

                if (_cinemaService->addCinema(cinema)) {
                    ViewHelper::showSuccess("影院添加成功!");
                } else {
                    ViewHelper::showError("影院添加失败!");
                }
                ViewHelper::waitForKeyPress();
                break;
            }
            case 0:
                return;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::showScreeningManagement() {
    while (true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("排片管理");

        std::cout << "1. 查看所有排片" << std::endl;
        std::cout << "2. 添加排片" << std::endl;
        std::cout << "3. 修改排片" << std::endl;
        std::cout << "4. 删除排片" << std::endl;
        std::cout << "0. 返回" << std::endl;

        int choice = ViewHelper::readInt("\n请选择: ");

        switch (choice) {
            case 1: {
                ViewHelper::clearScreen();
                ViewHelper::showMenuTitle("所有排片");
                // 这里可以实现查看所有排片的功能
                ViewHelper::showInfo("查看排片功能待实现!");
                ViewHelper::waitForKeyPress();
                break;
            }
            case 2: {
                ViewHelper::clearScreen();
                ViewHelper::showMenuTitle("添加排片");
                // 这里可以实现添加排片的功能
                ViewHelper::showInfo("添加排片功能待实现!");
                ViewHelper::waitForKeyPress();
                break;
            }
            case 0:
                return;
            default:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::showPersonalInfo() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("个人信息");

    std::cout << "用户ID: " << currentUser->userId << std::endl;
    std::cout << "用户名: " << currentUser->username << std::endl;
    std::cout << "姓名: " << currentUser->realName << std::endl;
    std::cout << "性别: " << currentUser->gender << std::endl;
    std::cout << "手机号: " << currentUser->phone << std::endl;
    std::cout << "邮箱: " << currentUser->email << std::endl;
    std::cout << "注册时间: " << currentUser->regTime << std::endl;
    std::cout << "最后登录: " << currentUser->lastLogin << std::endl;
    std::cout << "用户类型: " << (currentUser->isAdmin() ? "管理员" : "普通用户") << std::endl;

    std::cout << "\n1. 修改个人信息" << std::endl;
    std::cout << "2. 修改密码" << std::endl;
    std::cout << "0. 返回" << std::endl;

    int choice = ViewHelper::readInt("\n请选择: ");

    switch(choice) {
        case 1: {
            std::string realName = ViewHelper::readString("姓名 [" + currentUser->realName + "]: ", currentUser->realName);
            std::string gender = ViewHelper::readString("性别 [" + currentUser->gender + "]: ", currentUser->gender);
            std::string phone = ViewHelper::readString("手机 [" + currentUser->phone + "]: ", currentUser->phone);
            std::string email = ViewHelper::readString("邮箱 [" + currentUser->email + "]: ", currentUser->email);

            currentUser->realName = realName;
            currentUser->gender = gender;
            currentUser->phone = phone;
            currentUser->email = email;

            if (_userService->updateUser(*currentUser)) {
                ViewHelper::showSuccess("个人信息已更新!");
            } else {
                ViewHelper::showError("更新个人信息失败!");
            }
            break;
        }
        case 2:
            if (_authView.changePassword(currentUser->userId)) {
                ViewHelper::showSuccess("密码修改成功!");
            }
            break;
        case 0:
            return;
        default:
            ViewHelper::showError("无效的选择!");
            break;
    }
    ViewHelper::waitForKeyPress();
}

void MainController::logout() {
    currentUser.reset();  // 使用智能指针的reset方法
}

void MainController::exitSystem() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("退出系统");

    if (ViewHelper::confirm("确定退出系统吗?")) {
        ViewHelper::clearScreen();
        std::cout << "感谢使用电影票务管理系统，再见!" << std::endl;
        exit(0);
    }
}