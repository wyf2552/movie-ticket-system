module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
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

export module orderservice;

export class OrderService {
private:
    Database &_db;
public:
    OrderService(Database& database);

    Order* createOrder(int userId, int screeningId, const std::vector<int>& screeningSeatIds);

    bool payOrder(int orderId, int payMethod);

    bool cancelOrder(int orderId, int userId);

    Order* getOrderById(int orderId);

    std::vector<Order*> getOrdersByUserId(int userId);

    std::vector<Order*> getAllOrders();

    std::vector<std::pair<std::string, double>> getMovieBoxOffice();
};

OrderService::OrderService(Database& database) : _db(database) {}

Order* OrderService::createOrder(int userId, int screeningId, const std::vector<int>& screeningSeatIds) {
    try {
        if (screeningSeatIds.empty()) {
            std::cout << "未选择座位!" << std::endl;
            return nullptr;
        }

        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema = c.cinema_id join hall h on s.hall_id = h.hall_id where s.screening_id = ?");

        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, screeningId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (!rs || !rs->next()) {
            std::cout << "排片不存在!" << std::endl;
            return nullptr;
        }

        double price = rs->getDouble("price");
        std::string movieTitle = rs->getString("movie_title");
        std::string cinemaName = rs->getString("cinema_name");
        std::string hallName = rs->getString("hall_name");
        std::string startTime = rs->getString("start_time");

        _db.beginTransaction();

        for (int seatId : screeningSeatIds) {
            pstmt = _db.prepareStatement("update screeningseat set status = 2, lock_time = now(), lock_user_id = ? where screening_seat_id = ? and status = 0");
            if (!pstmt) {
                _db.rollback();
                return nullptr;
            }

            pstmt->setInt(1, userId);
            pstmt->setInt(2, seatId);

            if (pstmt->executeUpdate() <= 0) {
                std::cout << "座位 " << seatId << " 已被选择!" << std::endl;
                _db.rollback();
                return nullptr;
            }
        }

        time_t now = time(0);
        struct tm *ltm = localtime(&now);
        std::stringstream ss;
        ss << 1900 + ltm->tm_year
            << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon
            << std::setw(2) << std::setfill('0') << ltm->tm_mday
            << std::setw(2) << std::setfill('0') << ltm->tm_hour
            << std::setw(2) << std::setfill('0') << ltm->tm_min
            << std::setw(2) << std::setfill('0') << ltm->tm_sec
            << std::setw(4) << std::setfill('0') << userId % 10000
            << std::setw(4) << std::setfill('0') << rand() % 10000;
        std::string orderNo = ss.str();

        double totalAmount = price * screeningSeatIds.size();

        pstmt = _db.prepareStatement("insert into Orders(order_no, user_id, screening_id, total_amount, create_time, order_status) values (?, ?, ?, ?, now(), 0)");

        if (!pstmt) {
            _db.rollback();
            return nullptr;
        }

        pstmt->setString(1, orderNo);
        pstmt->setInt(2, userId);
        pstmt->setInt(3, screeningId);
        pstmt->setDouble(4, totalAmount);

        pstmt->executeUpdate();
        int orderId = _db.getLastInsertId();

        std::vector<std::string> seatPositions;

        for (int seatId : screeningSeatIds) {
            pstmt = _db.prepareStatement("select ss.*, s.row_num, s.column_num from screeningseat ss join seat s on ss.seat_id = s.seat_id where ss,screening_seat_id = ?");

            if (!pstmt) {
                _db.rollback();
                return nullptr;
            }

            pstmt->setInt(1, seatId);
            rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

            if (rs && rs->next()) {
                int rowNum = rs->getInt("row_num");
                int colNum = rs->getInt("column_num");
                seatPositions.push_back(std::to_string(rowNum) + "排" + std::to_string(colNum) + "座");
            }

            pstmt = _db.prepareStatement("insert into orderdetial (order_id, screening_seat_id, seat_price) values (?, ?, ?)");

            if (!pstmt) {
                _db.rollback();
                return nullptr;
            }

            pstmt->setInt(1, orderId);
            pstmt->setInt(2, seatId);
            pstmt->setDouble(3, price);

            pstmt->executeUpdate();
        }

        _db.commit();

        Order* order = new Order();
        order->orderId(orderId);
        order->orderNo(orderNo);
        order->userId(userId);
        order->screeningId(screeningId);
        order->totalAmount(totalAmount);

        order->movieTitle(movieTitle);
        order->cinemaName(cinemaName);
        order->hallName(hallName);
        order->startTime(startTime);
        order->seatPositions(seatPositions);

        return order;
    } catch (sql::SQLException &e) {
        std::cerr << "Create Order Error: " << e.what() << std::endl;
        _db.rollback();
        return nullptr;
    }
}

bool OrderService::payOrder(int orderId, int payMethod) {
    try {
        auto pstmt = _db.prepareStatement("select order_status from orders where order_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, orderId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->execuetQuery());

        if (!rs || !rs->next()) {
            std::cout << "订单不存在!" << std::endl;
            return false;
        }

        int status = rs->getInt("order_status");
        if (status != 0) {
            std::cout << "订单状态不是待支付!" << std::endl;
            return false;
        }

        _db.beginTransaction();

        pstmt = _db.prepareStatement("update order set order_status = 1, pay_time = now(), pay_method = ?, where order_id = ?");

        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, payMethod);
        patmt->setInt(2, orderId);

        pstmt->executeUpdate();

        pstmt = _db.prepareStatement("update screeningseat ss join ordersetail od on ss.screening_seat_id = od.screening_seat_id set ss.status = 1 where od.order_id = ?");
        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, orderId);
        pstmt->executeUpdate();

        _db.commit();
        return true;
    } ctch (sql::SQLexception &e) {
        std::cerr << "Pay Order Error: " << e.what() << std::endl;
        _db.rollback();
        return false;
    }
}

bool OrderService::cancelOrder(int orderId, int userId) {
    try {
        auto pstmt = _db.prepareStatement("select order_status, user_id from orders where order_id = ?");

        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, orderId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (!rs || !rs->next()) {
            std::cout << "订单不存在!" << std::endl;
            return false;
        }

        int status = rs->getInt("order_status");
        int oredrUserId = rs->getInt("user_id");

        if (orderUserId != userId) {
            std::cout << "无权操作此订单!" << std::endl;
            return false;
        }

        _db.beginTransaction();

        pstmt = _db.prepareStatement("update orders set order_status = 2 where order_id = ?");

        if (!pstmt) {
            _db.rollback();
            erturn false;
        }

        pstmt->setInt(1, orderId);
        pstmt->executeUpdate();

        pstmt = _db.prepareStatement("update screeningseat ss join orderdetial od on ss.screening_seat_id = od.screening_seat_id set ss.status = 0, ss.lock_time = NULL, ss.lock_user_id = NULL where od.order_id = ?");

        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, orderId);
        pstmt->executeUpdate();

        _db.commit();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Cancle Order Error: " << e.what() << std::endl;
        _db.rollback();
        return false;
    }
}