module;

#include <ctime>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>


import database;
import entities;
import viewhelper;
import userservice;
import cinemaservice;
import orderservice;
import authview;
import movieservice;
import screeningservice;
import movieview;
import ticketview;

export module orderview;

export class OrderView {
private:
    OrderService& _orderService;
    void viewAllOrders();
    void searchOrder();
    void displayOrderList(const std::vector<OrderUptr>& orders);

public:
    explicit OrderView(OrderService& orderService);
    void showMyOrders(const User& currentUser);
    void showOrderDetail(const Order& order);
    void showOrderManagement();
    void orderStatustics();
};

OrderView::OrderView(OrderService& orderService) : _orderService(orderService) {}

void OrderView::showMyOrders(const User& currentUser) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("我的订单");

    auto orders = _orderService.getOrdersByUserId(currentUser.userId);

    if (orders.empty()) {
        ViewHelper::showInfo("您还没有订单!");
        ViewHelper::waitForKeyPress();
        return;
    }

    displayOrderList(orders);

    int orderId = ViewHelper::readInt("\n请输入订单ID查看详情(0返回):");
    if (orderId <= 0) {
        return;
    }

    for (const auto& order : orders) {
        if (order->orderId == orderId) {
            showOrderDetail(*order);

            if (order->status == Order::Status::unpaid) {
                if (ViewHelper::confirm("是否现在支付?")) {
                    std::cout << "\n请选择支付方式:" << std::endl;
                    std::cout << "1. 支付宝" << std::endl;
                    std::cout << "2. 微信支付" << std::endl;
                    std::cout << "3. 银行卡" << std::endl;

                    int payMethod = ViewHelper::readInt("请选择: ");
                    if (payMethod >= 1 && payMethod <= 3) {
                        if (_orderService.payOrder(orderId, payMethod)) {
                            ViewHelper::showSuccess("支付成功！");
                        } else {
                            ViewHelper::showError("支付失败！");
                        }
                    }
                } else if (ViewHelper::confirm("是否取消订单？")) {
                    if (_orderService.cancelOrder(orderId, currentUser.userId)) {
                        ViewHelper::showSuccess("订单已取消！");
                    } else {
                        ViewHelper::showError("取消订单失败！");
                    }
                }
            }

            ViewHelper::waitForKeyPress();
            return;
        }
    }

    ViewHelper::showError("未找到订单！");
    ViewHelper::waitForKeyPress();
}

void OrderView::showOrderDetail(const Order& order) {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("订单详情 - " + order.orderNo);

    std::cout << "订单ID: " << order.orderId << std::endl;
    std::cout << "订单号: " << order.orderNo << std::endl;
    std::cout << "电影: " << order.movieTitle << std::endl;
    std::cout << "影院: " << order.cinemaName << std::endl;
    std::cout << "影厅: " << order.hallName << std::endl;
    std::cout << "放映时间: " << order.startTime << std::endl;

    std::cout << "座位: ";
    for (const auto& seat : order.seatPositions) {
        std::cout << seat << " ";
    }
    std::cout << std::endl;

    std::cout << "总金额: " << order.totalAmount << " 元" << std::endl;
    std::cout << "创建时间: " << order.createTime << std::endl;

    if (!order.payTime.empty()) {
        std::cout << "支付时间: " << order.payTime << std::endl;
    }

    std::cout << "支付方式: " << order.getPayMethodStr() << std::endl;
    std::cout << "订单状态: " << order.getOrderStatusStr() << std::endl;

    ViewHelper::showSeparator();
}

void OrderView::showOrderManagement() {
    while (true) {
        ViewHelper::clearScreen();
        ViewHelper::showMenuTitle("订单管理");

        std::cout << "1. 查看所有订单" << std::endl;
        std::cout << "2. 搜索订单" << std::endl;
        std::cout << "3. 订单统计" << std::endl;
        std::cout << "0. 返回" << std::endl;

        int choice = ViewHelper::readInt("\n请选择: ");

        switch (choice) {
            case 1:
                viewAllOrders();
                break;
            case 2:
                searchOrder();
                break;
            case 3:
                orderStatustics();
                break;
            case 0:
                return;
            default:
                ViewHelper::showError("无效的选择！");
                ViewHelper::waitForKeyPress();
                break;
        }
    }
}

void OrderView::viewAllOrders() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("所有订单");

    auto orders = _orderService.getAllOrders();

    if (orders.empty()) {
        ViewHelper::showInfo("没有订单记录！");
        ViewHelper::waitForKeyPress();
        return;
    }

    displayOrderList(orders);

    int orderId = ViewHelper::readInt("\n请输入订单ID查看详情 (0返回): ");
    if (orderId <= 0) {
        return;
    }

    for (const auto& order : orders) {
        if (order->orderId == orderId) {
            showOrderDetail(*order);
            ViewHelper::waitForKeyPress();
            return;
        }
    }

    ViewHelper::showError("未找到订单！");
    ViewHelper::waitForKeyPress();
}

void OrderView::searchOrder() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("搜索订单");

    std::string orderId = ViewHelper::readString("请输入订单号:");
    if (orderId.empty() || orderId == "0") {
        ViewHelper::showError("订单号不能为空!");
        ViewHelper::waitForKeyPress();
        return;
    }

    int orderIdInt = 0;
    try {
        int orderIdInt = std::stoi(orderId);
        if (orderIdInt <= 0) {
            std::cout << "错误：订单ID必须为正整数" << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        std::cout << "错误: 订单ID格式无效" << std::endl;
        return;
    }

    auto order = _orderService.getOrderById(orderIdInt);

    if (order) {
        showOrderDetail(*order);
    } else {
        ViewHelper::showError("未找到订单!");
    }

    ViewHelper::waitForKeyPress();
}

void OrderView::displayOrderList(const std::vector<OrderUptr>& orders) {
    std::cout << std::left << std::setw(8) << "订单ID" << "|"
              << std::setw(15) << "订单号" << "|"
              << std::setw(15) << "用户名" << "|"
              << std::setw(20) << "电影" << "|"
              << std::setw(20) << "影院" << "|"
              << std::setw(20) << "时间" << "|"
              << std::setw(10) << "金额" << "|"
              << std::setw(10) << "状态" << std::endl;
    ViewHelper::showSeparator();

    for (const auto& order : orders) {
        std::cout << std::left << std::setw(8) << order->orderNo << "|"
                  << std::setw(15) << order->orderNo << "|"
                  << std::setw(15) << order->username << "|"
                  << std::setw(20) << order->movieTitle << "|"
                  << std::setw(20) << order->cinemaName << "|"
                  << std::setw(20) << order->startTime << "|"
                  << std::setw(10) << order->totalAmount << "|"
                  << std::setw(10) << order->getOrderStatusStr() << std::endl;
    }
    ViewHelper::showSeparator();
}

void OrderView::orderStatustics() {
    ViewHelper::clearScreen();
    ViewHelper::showMenuTitle("订单统计");

    auto boxOffice = _orderService.getMovieBoxOffice();

    if (boxOffice.empty()) {
        ViewHelper::showInfo("暂无票房数据！");
        ViewHelper::waitForKeyPress();
        return;
    }

    std::cout << "电影票房统计:" << std::endl;
    ViewHelper::showSeparator();

    std::cout << std::left << std::setw(30) << "电影" << "|"
              << std::setw(15) << "票房(元)" << "|"
              << std::setw(10) << "票数" << std::endl;
    ViewHelper::showSeparator();

    double totalBoxOffice = 0.0;
    int totalTickets = 0;

    for (const auto& item : boxOffice) {
        std::cout << std::left << std::setw(30) << item.first << "|"
                  << std::setw(15) << item.second << "|"
                  << std::setw(10) << "N/A" << std::endl;
        totalBoxOffice += item.second;
    }

    ViewHelper::showSeparator();
    std::cout << "总票房: " << totalBoxOffice << " 元" << std::endl;
    std::cout << "总票数: " << totalTickets << " 张" << std::endl;

    ViewHelper::waitForKeyPress();
}
