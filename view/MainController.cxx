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
    CinemaView _cinemaView;
    UserView _userView;
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

MainController::MainController() : _db("localhost", "root", "password", "movie_ticket_system"), _userService(db), _movieService(db), _cinemaService(db), _screeningService(db), _orderService(db), _authView(userService), _movieView(movieService, screeningService), _ticketView(movieService, screeningService, orderService), _orderView(orderService), _cinemaView(cinemaService), _userView(userService), currentUser(nullptr) {}

MainController::~MainController() {
    if (currentUser) {
        delete currentuser;
    }
}

