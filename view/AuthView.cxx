module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>

import entities;
import viewhelper;
import userservice;

export module authview;

export class AuthView {
private:
    UserServiceSptr _userService;

public:
    explicit AuthView(UserServiceSptr userService);

    UserUptr showLoginMenu();

    bool showRegisterMenu();

    bool changePassword(int userId);
};



AuthView::AuthView(UserServiceSptr userService) : _userService(userService) {}

UserUptr AuthView::showLoginMenu() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("用户登录");

    std::string username = ViewHelper::readString("用户名: ");
    std::string password = ViewHelper::readString("密码: ");

    if (username.empty() || password.empty()) {
        ViewHelper::showError("用户名和密码不能为空!");
        ViewHelper::waitForKeyPress();
        return nullptr;
    }

    UserUptr currentUser = _userService->login(username, password);

    if (currentUser) {
        ViewHelper::showSuccess("登录成功! 欢迎, " + currentUser->username + "!");
        ViewHelper::waitForKeyPress();
    } else {
        ViewHelper::showError("登录失败! 用户名或密码错误!");
        ViewHelper::waitForKeyPress();
    }

    return currentUser;
}

bool AuthView::showRegisterMenu() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("用户注册");

    std::string username = ViewHelper::readString("用户名: ");
    if (username.empty()) {
        ViewHelper::showError("用户名不能为空!");
        ViewHelper::waitForKeyPress();
        return false;
    }

    std::string password = ViewHelper::readString("密码: ");
    std::string confirmPassword = ViewHelper::readString("确认密码: ");

    if (password.empty()) {
        ViewHelper::showError("密码不能为空!");
        ViewHelper::waitForKeyPress();
        return false;
    }

    if (password != confirmPassword) {
        ViewHelper::showError("两次输入的密码不一致!");
        ViewHelper::waitForKeyPress();
        return false;
    }

    std::string realName = ViewHelper::readString("姓名: ");
    std::string gender = ViewHelper::readString("性别 (男/女): ");
    std::string phone = ViewHelper::readString("手机号: ");
    std::string email = ViewHelper::readString("邮箱: ");

    User user(0, username, password, realName, gender, phone, email);

    if (_userService->registerUser(user)) {
        ViewHelper::showSuccess("注册成功! 请登录系统.");
        ViewHelper::waitForKeyPress();
        return true;
    } else {
        ViewHelper::showError("注册失败! 用户名可能已存在.");
        ViewHelper::waitForKeyPress();
        return false;
    }
}

