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
}

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
                cinema->address = rs->getString("adress");
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
