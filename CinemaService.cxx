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

export module cinemaservice;

export class CinemaService {
private:
    Database& _db;
public:
    CinemaService(Database& database);

    bool addCinema(Cinema& cinema);

    Cinema* getCinemaById(int cinemaId);

    std::vector<Cinema*> getAllCinema();

    bool updateCinema(const Cinema& cinema);

    bool deleteCinema(int cinemaId);

    bool addHall(Hall& hall);

    Hall* getHallById(int hallId);

    std::vector<Hall*> getHallsByCinemaId(int cinemaId);

    bool updateHall(const Hall& hall);

    bool deleteHall(int hallId);

    bool addSeats(int hallId, int rows, int columns);

    std:: vector<Seat*> getSeatsByHallId(int hallId);

    bool updateSeat(const Seat& seat);
};

CinemaService::CinemaService(Database& database) : _db(database) {}

bool CinemaService::addCinema(Cinema& cinema) {
    try {
        auto pstmt = _db.prepareStatement(
            "insert into cinema (cinema_name, address, phone, introduction, status) values (?, ?, ?, ?, ?)"
        );

        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, cinema.cinemaName);
        pstmt->setString(2, cinema.address);
        pstmt->setString(3, cinema.phone);
        pstmt->setString(4, cinema.introduction);
        pstmt->setInt(5, statusCast<Cinema::Status, int>(cinema.status));

        pstmt->executeUpdate();

        cinema.cinemaId = _db.getLastInsertId();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Add Cinema Error: " << e.what() << std::endl;
        return false;
    }
}

Cinema* CinemaService::getCinemaById(int cinemaId) {
    try {
        auto pstmt = _db.prepareStatement("select * from cinema where cinema_id = ?");
            if (!pstmt) {
                return nullptr;
            }
            pstmt->setInt(1, cinemaId);
            auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

            if (rs && rs->next()) {
                Cinema* cinema = new Cinema();
                cinema->cinemaId = rs->getInt("cinema_id");
                cinema->cinemaName = rs->getString("cinema_name");
                cinema->address = rs->getString("address");
                cinema->phone = rs->getString("phone");
                cinema->introduction = rs->getString("introduction");
                cinema->status = statusCast<int, Cinema::Status>(rs->getInt("status"));
                return cinema;
            }
            return nullptr;
        } catch (sql::SQLException &e) {
            std::cerr << "Get Cinema Error: " << e.what() << std::endl;
            return nullptr;
    }
}

std::vector<Cinema*> CinemaService::getAllCinema() {
    std::vector<Cinema*> cinemas;
    try {
        auto rs = _db.query("select * from cinema order by cinema_id");

        if (rs) {
            while (rs->next()) {
                Cinema* cinema = new Cinema();
                cinema->cinemaId = rs->getInt("cinema_id");
                cinema->cinemaName = rs->getString("cinema_name");
                cinema->address = rs->getString("address");
                cinema->phone = rs->getString("phone");
                cinema->introduction = rs->getString("introduction");
                cinema->status = statusCast<int, Cinema::Status>(rs->getInt("status"));
                cinemas.push_back(cinema);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get All Cinemas Error: " << e.what() << std::endl;
    }
    return cinemas;
}

bool CinemaService::updateCinema(const Cinema& cinema) {
    try {
        auto pstmt = _db.prepareStatement("update Cinema set cinema_name = ?, address = ?, phone = ?, introduction = ?, status = ? where cinema_id = ?");

        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, cinema.cinemaName);
        pstmt->setString(2, cinema.address);
        pstmt->setString(3, cinema.phone);
        pstmt->setString(4, cinema.introduction);
        pstmt->setInt(5, statusCast<Cinema::Status, int>(cinema.status));
        pstmt->setInt(6, cinema.cinemaId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Cinema Error: " << e.what() << std::endl;
        return false;
    }
}

bool CinemaService::deleteCinema(int cinemaId) {
    try {
        auto pstmt = _db.prepareStatement("select count(*) from hall where cinema_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, cinemaId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "影院存在关联影厅，无法删除!" << std::endl;
            return false;
        }

        pstmt = _db.prepareStatement("delete from cinema where cinema_id = ?");
        if (!pstmt) {
            return false;
        }
        pstmt->setInt(1, cinemaId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Delete Cinema Error: " << e.what() << std::endl;
        return false;
    }
}

bool CinemaService::addHall(Hall& hall) {
    try {
        auto pstmt = _db.prepareStatement("insert into hall (cinema_id, hall_name, seat_count, hall_type, status) values (?, ?, ?, ?, ?)");

        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, hall.cinemaId);
        pstmt->setString(2, hall.hallName);
        pstmt->setInt(3, hall.seatCount);
        pstmt->setString(4, hall.hallType);
        pstmt->setInt(5, statusCast<Hall::Status, int>(hall.status));

        pstmt->executeUpdate();

        hall.hallId = _db.getLastInsertId();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Add Hall Error: " << e.what() << std::endl;
        return false;
    }
}

Hall* CinemaService::getHallById(int hallId) {
    try {
        auto pstmt = _db.prepareStatement("select * from hall where hall_id = ?");
        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, hallId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            Hall* hall = new Hall();
            hall->hallId = rs->getInt("hall_id");
            hall->cinemaId = rs->getInt("cinema_id");
            hall->hallName = rs->getString("hall_name");
            hall->seatCount = rs->getInt("seat_count");
            hall->hallType = rs->getString("hall_type");
            hall->status = statusCast<int, Hall::Status>(rs->getInt("status"));
            return hall;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "Get Hall Error: " << e.what() << std::endl;
        return nullptr;
    }
}