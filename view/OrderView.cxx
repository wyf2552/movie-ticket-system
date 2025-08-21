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


