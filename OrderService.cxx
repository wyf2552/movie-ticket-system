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

export module orderservice;

export class OrderService {
private:
    Database &_db;
public:
    OrderService(Database& database);

    OrderUptr createOrder(int userId, int screeningId, const std::vector<int>& screeningSeatIds);

    bool payOrder(int orderId, int payMethod);

    bool cancelOrder(int orderId, int userId);

    OrderUptr getOrderById(int orderId);

    std::vector<OrderUptr> getOrdersByUserId(int userId);

    std::vector<OrderUptr> getAllOrders();

    std::vector<std::pair<std::string, double>> getMovieBoxOffice();
};

export using OrderServiceSptr = std::shared_ptr<OrderService>;

OrderService::OrderService(Database& database) : _db(database) {}

OrderUptr OrderService::createOrder(int userId, int screeningId, const std::vector<int>& screeningSeatIds) {
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

        auto order = std::make_Uptr<Order>();
        order->orderId = orderId;
        order->orderNo = orderNo;
        order->userId = userId;
        order->screeningId = screeningId;
        order->totalAmount = totalAmount;

        order->movieTitle = movieTitle;
        order->cinemaName = cinemaName;
        order->hallName = hallName;
        order->startTime = startTime;
        order->seatPositions = seatPositions;

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
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

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
        pstmt->setInt(2, orderId);

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
    } catch (sql::SQLException &e) {
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

        int status = rs->getInt("status");
        int orderUserId = rs->getInt("user_id");

        if (orderUserId != userId) {
            std::cout << "无权操作此订单!" << std::endl;
            return false;
        }

        _db.beginTransaction();

        pstmt = _db.prepareStatement("update orders set order_status = 2 where order_id = ?");

        if (!pstmt) {
            _db.rollback();
            return false;
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

OrderUptr OrderService::getOrderById(int orderId) {
    try {
        auto pstmt = _db.prepareStatement("select o.*, u.username, s.start_time as movie_title, c.cinema_name, h.hall_name from orders o join user u on o.user_id = u.user_id join screening s on o.screening_id = s.screening_id join movie m on s.movie_id = m.movie_id join cinema s on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where o.order_id = ?");

        if (!pstmt) {
            return nullptr;
        }
        pstmt->setInt(1, orderId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            auto order = std::make_unique<Order>();
            order->orderId = rs->getInt("order_id");
            order->orderNo = rs->getString("order_no");
            order->userId = rs->getInt("user_id");
            order->screeningId = rs->getInt("screening_id");
            order->totalAmount = rs->getDouble("total_amount");
            order->createTime = rs->getString("create_time");
            order->payTime = rs->getString("pay_time");
            order->payMethod = statusCast<int, Order::PayMethod>(rs->getInt("pay_method"));
            order->status = statusCast<int, Order::Status>(rs->getInt("status"));
            order->username = rs->getString("user_name");
            order->movieTitle = rs->getString("movie_title");
            order->cinemaName = rs->getString("cinema_name");
            order->hallName = rs->getString("hall_name");
            order->startTime = rs->getString("start_time");

            pstmt = _db.prepareStatement("select ss.screening_seat_id, s.row_num, s.column from orderdeltail od join screeningseat ss on od.screening_seat_id = ss.screening_seat_id join seat s on ss.seat_id = s.seat_id where od.order_id = ?");

            if (pstmt) {
                pstmt->setInt(1, orderId);
                auto seatRs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

                std::vector<std::string> seatPositions;
                while (seatRs && seatRs->next()) {
                    int rowNum = seatRs->getInt("row_num");
                    int colNum = seatRs->getInt("column_num");
                    seatPositions.push_back(std::to_string(rowNum) + "排" + std::to_string(colNum) + "座");
                }
                order->seatPositions = seatPositions;
            }
            return order;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "Get Order Error: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<OrderUptr> OrderService::getOrdersByUserId(int userId) {
    std::vector<OrderUptr> orders;
    try {
        auto pstmt = _db.prepareStatement("select o.*, u.username, s.start_time, m.title as movie_title, c.cinema_name, h.hall_name from orders o join user u on o.user_id = u.user_id join screening s on o.screening_id = s.screening_id join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where o.user_id = ? order by o.create_time DESC");
        if (!pstmt) {
            return orders;
        }

        pstmt->setInt(1, userId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto order = std::make_unique<Order>();
                order->orderId = rs->getInt("order_id");
                order->orderNo = rs->getString("order_no");
                order->userId = rs->getInt("user_id");
                order->screeningId = rs->getInt("screening_id");
                order->totalAmount = rs->getDouble("total_amount");
                order->createTime = rs->getString("create_time");
                order->payTime = rs->getString("pay_time");
                order->payMethod = statusCast<int, Order::PayMethod>(rs->getInt("pay_method"));
                order->status = statusCast<int, Order::Status>(rs->getInt("order_status"));

                // 设置关联信息
                order->username = rs->getString("username");
                order->movieTitle = rs->getString("movie_title");
                order->cinemaName = rs->getString("cinema_name");
                order->hallName = rs->getString("hall_name");
                order->startTime = rs->getString("start_time");

                orders.push_back(order);
            }
        }

        for (auto order : orders) {
            pstmt = _db.prepareStatement("select ss.screening_seat_id, s.row_num, s.column_num from orderdetail od join screeningseat_seat_id join seat s on ss.seat_id = ss.seat_id where od.order_id = ?");
            if (pstmt) {
                pstmt->setInt(1, order->orderId);
                auto seatRs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

                std::vector<std::string> seatPositions;
                while (seatRs && seatRs->next()) {
                    int rowNum = seatRs->getInt("row_num");
                    int colNum = seatRs->getInt("column_num");
                    seatPositions.push_back(std::to_string(rowNum) + "排" + std::to_string(colNum) + "座");
                }
                order->seatPositions = seatPositions;
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Orders By User Error: " << e.what() << std::endl;
    }
    return orders;
}

std::vector<OrderUptr> OrderService::getAllOrders() {
    std::vector<OrderUptr> orders;
    try {
        auto rs = _db.query("select o.*, u.username, s.start_time, as movie_title, c.cinema_name, h.hall_name from orders o join user u on o.user_id = u.user_id join screening s on o.screening_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id order by o.create_time DESC");

        if (rs) {
            while (rs->next()) {
                auto order = std::make_unique<Order>();
                order->orderId = rs->getInt("order_id");
                order->orderNo = rs->getString("order_no");
                order->userId = rs->getInt("user_id");
                order->screeningId = rs->getInt("screening_id");
                order->totalAmount = rs->getDouble("total_amount");
                order->createTime = rs->getString("create_time");
                order->payTime = rs->getString("pay_time");
                order->payMethod = statusCast<int, Order::PayMethod>(rs->getInt("pay_method"));
                order->status = statusCast<int, Order::Status>(rs->getInt("status"));
                order->username = rs->getString("username");
                order->movieTitle = rs->getString("movie_title");
                order->cinemaName = rs->getString("cinema_name");
                order->hallName = rs->getString("hall_name");
                order->startTime = rs->getString("start_time");
                orders.push_back(order);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get All Orders Error: " << e.what() << std::endl;
    }
    return orders;
}

std::vector<std::pair<std::string, double>> OrderService::getMovieBoxOffice() {
    std::vector<std::pair<std::string, double>> results;
    try {
        auto rs = _db.query("select m.title, sum(o.total_amount) as box_office from orders o join screening s on o.screening_id = s.screening_id join movie m on s.movie_id = m.movie_id where o.order_status = 1 group by m.movie_id order by box_office DESC");

        if (rs) {
            while (rs->next()) {
                std::string movieTitle = rs->getString("title");
                double boxOffice = rs->getDouble("box_office");
                results.push_back(std::make_pair(movieTitle, boxOffice));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Movie Box Office Error: " << e.what() << std::endl;
    }
    return results;
}