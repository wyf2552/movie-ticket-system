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
import userservice;
import authview;
import movieservice;
import screeningservice;
import movieview;
import ticketview;
import orderview;
import database;
import cinemaservice;
import orderservice;

export module maincontroller;

export class MainController {
private:
    Database _db;
    UserService _userService;
    MovieService _movieService;
    CinemaService _cinemaService;
    ScreeningService _screeningService;
    OrderService _orderService;
    AuthView _authView;
    MovieView _movieView;
    OrderView _orderView;
    UserUptr currentUser;

    void showMainMenu();
    void showUserMenu();
    void showAdminMenu();
    void login();
    void logout();
    void exitSystem();

public:
    MainController();
    ~MainController();

    bool initialize();

    void run();
};

MainController::MainController() : _db("tcp://localhost:3306", "root", "123456wyf", ""), _userService(db), _movieService(db), _cinemaService(db), _screeningService(db), _orderService(db), _authView(userService), _movieView(movieService, screeningService), _ticketView(movieService, screeningService, orderService), _orderView(orderService), _cinemaView(cinemaService), _userView(userService), currentUser(nullptr) {}

MainController::~MainController() {
    if (currentUser) {
        delete currentuser;
    }
}

bool MainController::initialize() {
    return db.connect();
}

void MainController::run() {
    if (!initialize()) {
        std::cerr << "无法连接到数据库，系统退出!" << std::endl;
        return;
    }

    screeningService.releaseTimeoutSeats();

    showMainMenu();
}

void MainController::showMainMenu() {
    while (true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("电影票务管理系统");

        std::cout << "1.用户登陆" << std::endl;
        std::cout << "2.用户注册" << std::endl;
        std::cout << "3.退出系统" << std::endl;

        int choice = ViewHelper::readInt("\n请选择功能: ");

        switch (choice) {
            case 1:
                login();
                break;
            case 2:
                if (authView.showRegisterMenu()) {
                    ViewHelper::showSuccess("注册成功，请登录!");
                    ViewHelper::waitForKeyPress();
                }
                break;
            case 0:
                ViewHelper::showError("无效的选择!");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void MainController::login() {
    currentUser = authView.showLoginMenu();

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
                movieView.browseMovies();
                break;
            case 2:
                movieView.showMovieDetails();
                break;
            case 3:
                movieView.searchMovies();
                break;
            case 4:
                ticketView.buyTicket(*currentUser);
                break;
            case 5:
                orderView.showMyOrders(*currentUser);
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
        ViewHelper::showMenuTitle("管理员 - " + currentUser->username);

        std::cout << "1. 电影管理" << std::endl;
        std::cout << "2. 影院管理" << std::endl;
        std::cout << "3. 排片管理" << std::endl;
        std::cout << "4. 订单管理" << std::endl;
        std::cout << "5. 用户管理" << std::endl;
        std::cout << "6. 统计报表" << std::endl;
        std::cout << "7. 个人信息" << std::endl;
        std::cout << "0. 退出登录" << std::endl;

        int choice = ViewHelper::readInt("\n请选择功能: ");

        switch (choice) {
            case 1:
                movieView.showMovieManagement();
                break;
            case 2:
                cinemaView.showCinemaManagement();
                break;
            case 3:
                screeningView.showScreeningManagement();
                break;
            case 4:
                orderView.showOrderManagement();
                break;
            case 5:
                userView.showUserManagement();
                break;
            case 6:
                orderView.orderStatistics();
                break;
            case 7:
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

void MainController::logout() {
    if (currentUser) {
        delete currentUser;
        currentUser = nullptr;
    }
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


