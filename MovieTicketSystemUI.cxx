module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iomanip>
#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

import database;
import entities;
import userservice;
import movieservice;
import cinemaservice;
import screeningservice;
import orderservice;

export module movieticketsystemui;

export class MovieTicketSystemUI {
private:
    Database _db;
    UserService _userService;
    MovieService _movieService;
    CinemaService _cinemaService;
    ScreeningService _screeningService;
    OrderService _orderService;
    User* _currentUser;

    void showSeparator(char c = '-', int length = 80);

    void showMenuTitle(const std::string& title);

    void showSuccess(const std::string& message);

    void showInfo(const std::string& message);

    void waitForKeyPress();

    void clearnScreen();

    int readInt(const std::string& prompt, int defaultValue = -1);

    double readDouble(const std::string& prompt, double defaultValue = -1.0);

    std::string readString(const std::string& prompt, const std::string& defaultValue = "");

    std::string readPassword(const std::string& prompt);

    bool confirm(const std::string& prompt);

public:
    MovieTicketSystemUI();

    ~MovieTicketSystemUI();

    bool initialize();

    void showMainMenu();

    void loginMenu();

    void registerMenu();

    void showUserMenu();

    void showAdminMenu();

    void browseMovies();

    void showMovieDetails();

    void searchMovies();

    void buyTicket();

    void showMyOrders();

    void showPersonalInfo();

    void modifyPersonalInfo();

    void changePassword();

    void showMovieManagement();

    void addMovie();

    void modifyMovie();

    void deleteMovie();

    void viewAllMovie();

    void showCinemaManagement();

    void manageCinemas();

    void addCinema();

    void modifyCinema();

    void deleteCinema();

    void viewAllCinemas();

    void manageHalls();

    void addHall(int cinemaId, const std::string& cinemaName);

    void modifyHall(const std::vector<Hall*>& halls);

    void deletehall(const std::vector<Hall*>& halls);

    void manageSeats();

    void showScreeningManagement();

    void addScreening();

    void modifyScreening();

    void deleteScreening();

    void viewScreenings();

    void viewScreeningsByMovie();

    void viewScreeningsByCinema();

    void showOrderManagement();

    void viewAllOrders();

    void viewOrderDetail(const Order& order);

    void searchOrder();

    void orderStatistics();

    void showUserManagement();

    void viewAllUsers();

    void addUser();

    void modifyUser();

    void deleteUser();

    void showStatistics();

    void logout();

    void exitSystem();

    void run();
};

void MovieTicketSystemUI::showSeparator(char c = '-', int length = 80) {
    std::cout << std::string(length, c) << std::endl;
}

void MovieTicketSystemUI::showMenuTitle(const std::string& title) {
    showSeparator('=');
    std::cout << " " << title << std::endl;
    showSeparator('=');
}

void MovieTicketSystemUI::showError(const std::string& message) {
    std::cout << "\n[错误]" << message << std::endl;
}

void MovieTicketSystemUI::showSuccess(const std::string& message) {
    std::cout << "\n[成功] " << message << std::endl;
}

void MovieTicketSystemUI::showInfo(const std::string& message) {
    std::cout << "\n[信息] " << message << std::endl;
}

void MovieTicketSystemUI::waitForKeyPress() {
    std::cout << "\n按任意键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void MovieTicketSystemUI::clearScreen() {
    #ifdef _WIN32
            system("cls");
    #else
            system("clear");
    #endif
}

int MovieTicketSystemUI::readInt(const std::string& prompt, int defaultValue = -1) {
    std::string input;
    int value;

    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) return defaultValue;

    try {
        value = std::stoi(input);
        return value;
    } catch (...) {
        return defaultValue;
    }
}

double MovieTicketSystemUI::readDouble(const std::string& prompt, double defaultValue = -1.0) {
    std::string input;
    double value;

    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) return defaultValue;

    try {
        value = std::stod(input);
        return value;
    } catch (...) {
        return defaultValue;
    }
}

std::string MovieTicketSystemUI::readString(const std::string& prompt, const std::string& defaultValue = "") {
    std::string input;

    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) return defaultValue;
    return input;
}

std::string MovieTicketSystemUI::readPassword(const std::string& prompt) {
    std::string password;
    char ch;

    std::cout << prompt;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        password += ch;
    }

    return password;
}

bool MovieTicketSystemUI::confirm(const std::string& prompt) {
    std::string input;

    std::cout << prompt << " (y/n): ";
    std::getline(std::cin, input);

    return input == "y" || input == "Y";
}
