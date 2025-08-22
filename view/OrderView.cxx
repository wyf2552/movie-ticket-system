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
    void orderStatistics();
};

OrderView::OrderView(Orderservice& orderService) : _orderService(orderService) {}

void OrderView:showMyOrders(const User& currentUser) {
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

            if (order->orderStatus == Order::Status::Unpaid) {
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

    std::cout << "支付方式: " << order.getPayMethodDescription() << std::endl;
    std::cout << "订单状态: " << order.getOrderStatusDescription() << std::endl;

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
                orderStatistics();
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