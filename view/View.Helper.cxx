module;

#include <string>
#include <iostream>

export module view.helper;

export class ViewHelper {
public:
    static void showSeparator(char c = '-', int length = 80);

    static void showMenuTitle(const std::string& title);

    static void showError(const std::string& message);

    static void showSuccess(const std::string& message);

    static void showInfo(const std::string& message);

    static void waitForKeyPress();

    static void clearScreen();

    static int readInt(const std::string& prompt, int defaultValue = -1);

    static double readDouble(const std::string& prompt, double defaultValue = -1.0);

    static std::string readString(const std::string& prompt, const std::string& defaultValue = "");

    static std::string readPassword(const std::string& prompt);

    static bool confirm(const std::string& prompt);
};

void ViewHelper::showSeparator(char c, int length) {
    std::cout << std::string(length, c) << std::endl;
}

void ViewHelper::showMenuTitle(const std::string& title) {
    showSeparator('=');
    std::cout << " " << title << std::endl;
    showSeparator('=');
}

void ViewHelper::showError(const std::string& message) {
    std::cout << "\n[错误]" << message  << std::endl;
}

void ViewHelper::showSuccess(const std::string& message) {
    std::cout << "\n[成功]" << message << std::endl;
}

void ViewHelper::showInfo(const std::string& message) {
    std::cout << "\n[信息]" << message << std::endl;
}

void ViewHelper::waitForKeyPress() {
    std::cout << "\n按任意键继续···";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void ViewHelper::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int ViewHelper::readInt(const std::string& prompt, int defaultValue) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    try {
        return std::stoi(input);
    } catch (...) {
        return defaultValue;
    }
}

double ViewHelper::readDouble(const std::string& prompt, double defaultValue) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }

    try {
        return std::stod(input);
    } catch (...) {
        return defaultValue;
    }
}

std::string ViewHelper::readString(const std::string& prompt, const std::string& defaultValue) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);

    if (input.empty()) {
        return defaultValue;
    }
    return input;
}

std::string ViewHelper::readPassword(const std::string& prompt) {
    std::string password;
    char ch;

    std::cout << prompt;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        password += ch;
    }

    return password;
}

bool ViewHelper::confirm(const std::string& prompt) {
    std::string input;
    std::cout << prompt << " (y/n): ";
    std::getline(std::cin, input);

    return input == "y" || input == "Y";
}