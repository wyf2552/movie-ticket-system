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

    CinemaUptr getCinemaById(int cinemaId);

    std::vector<CinemaUptr> getAllCinema();

    bool updateCinema(const Cinema& cinema);

    bool deleteCinema(int cinemaId);

    bool addHall(Hall& hall);

    HallUptr getHallById(int hallId);

    std::vector<HallUptr> getHallsByCinemaId(int cinemaId);

    bool updateHall(const Hall& hall);

    bool deleteHall(int hallId);

    bool addSeats(int hallId, int rows, int columns);

    std:: vector<SeatUptr> getSeatsByHallId(int hallId);

    bool updateSeat(const Seat& seat);
};

export using CinemaServiceSptr = std::shared_ptr<CinemaService>;

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

CinemaUptr CinemaService::getCinemaById(int cinemaId) {
    try {
        auto pstmt = _db.prepareStatement("select * from cinema where cinema_id = ?");
            if (!pstmt) {
                return nullptr;
            }
            pstmt->setInt(1, cinemaId);
            auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

            if (rs && rs->next()) {
                auto cinema = std::make_unique<Cinema>();
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

std::vector<CinemaUptr> CinemaService::getAllCinema() {
    std::vector<CinemaUptr> cinemas;
    try {
        auto rs = _db.query("select * from cinema order by cinema_id");

        if (rs) {
            while (rs->next()) {
                auto cinema = std::make_unique<Cinema>();
                cinema->cinemaId = rs->getInt("cinema_id");
                cinema->cinemaName = rs->getString("cinema_name");
                cinema->address = rs->getString("address");
                cinema->phone = rs->getString("phone");
                cinema->introduction = rs->getString("introduction");
                cinema->status = statusCast<int, Cinema::Status>(rs->getInt("status"));
                cinemas.push_back(std::move(cinema));
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

HallUptr CinemaService::getHallById(int hallId) {
    try {
        auto pstmt = _db.prepareStatement("select * from hall where hall_id = ?");
        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, hallId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            auto hall = std::make_unique<Hall>();
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

std::vector<HallUptr> CinemaService::getHallsByCinemaId(int cinemaId) {
    std::vector<HallUptr> halls;
    try {
        auto pstmt = _db.prepareStatement("select * from hall where cinema_id = ? order by hall_id");
        if (!pstmt) {
            return halls;
        }

        pstmt->setInt(1, cinemaId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto hall = std::make_unique<Hall>();
                hall->hallId = rs->getInt("hall_id");
                hall->cinemaId = rs->getInt("cinema_id");
                hall->hallName = rs->getString("hall_name");
                hall->seatCount = rs->getInt("seat_string");
                hall->hallType = rs->getString("hall_type");
                hall->status = statusCast<int, Hall::Status>(rs->getInt("status"));
                halls.push_back(std::move(hall));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Halls By Cinema Error: " << e.what() << std::endl;
    }
    return halls;
}

bool CinemaService::updateHall(const Hall& hall) {
    try {
        auto pstmt = _db.prepareStatement("update hall set cinema_id = ?, hall_name = ?, seat_count = ?, hall_type = ?, status = ? where hall_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, hall.cinemaId);
        pstmt->setString(2, hall.hallName);
        pstmt->setInt(3, hall.seatCount);
        pstmt->setString(4, hall.hallType);
        pstmt->setInt(5, statusCast<Hall::Status, int>(hall.status));
        pstmt->setInt(6, hall.hallId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Hall Error: " << e.what() << std::endl;
        return false;
    }
}

bool CinemaService::deleteHall(int hallId) {
    try {
        auto pstmt = _db.prepareStatement("select count(*) from seat where hall_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, hallId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "影厅存在关联座位,无法删除!" << std::endl;
            return false;
        }

        pstmt = _db.prepareStatement("delete from hall where hall_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, hallId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Delete Hall Error: " << e.what() << std::endl;
        return false;
    }
}

bool CinemaService::addSeats(int hallId, int rows, int columns) {
    try {
        _db.beginTransaction();

        for (int row = 1; row <= rows; row++) {
            for (int col = 1; col <= columns; col++) {
                auto pstmt = _db.prepareStatement("insert into seat(hall_id, row_num, column_num, seat_type, status) values (?, ?, ?, 0, 1)");

                if (!pstmt) {
                    _db.rollback();
                    return false;
                }

                pstmt->setInt(1, hallId);
                pstmt->setInt(2, row);
                pstmt->setInt(3, col);
                pstmt->executeUpdate();
            }
        }

        auto pstmt = _db.prepareStatement("update hall set seat_count = ? where hall_id = ?");

        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, rows * columns);
        pstmt->setInt(2, hallId);
        pstmt->executeUpdate();

        _db.commit();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Add Seats Error: " << e.what() << std::endl;
        _db.rollback();
        return false;
    }
}

std::vector<SeatUptr> CinemaService::getSeatsByHallId(int hallId) {
    std::vector<SeatUptr> seats;
    try {
        auto pstmt = _db.prepareStatement("select * from seat where hall_id = ? order by row_num, column_num");

        if (!pstmt) {
            return seats;
        }

        pstmt->setInt(1, hallId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto seat = std::make_unique<Seat>();
                seat->seatId = rs->getInt("seat_id");
                seat->hallId = rs->getInt("hall_id");
                seat->rowNum = rs->getInt("row_num");
                seat->columnNum = rs->getInt("column_num");
                seat->seatType = statusCast<int, Seat::SeatType>(rs->getInt("seat_type"));
                seat->status = statusCast<int, Seat::Status>(rs->getInt("status"));
                seats.push_back(std::move(seat));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Seats By Hall Error: " << e.what() << std::endl;
    }
    return seats;
}

bool CinemaService::updateSeat(const Seat& seat) {
    try {
        auto pstmt = _db.prepareStatement ("update seat set seat_type = ?, status = ? where seat_id = ?");
        if (!pstmt) {
            return false;
        }
        pstmt->setInt(1, statusCast<Seat::SeatType, int>(seat.seatType));
        pstmt->setInt(2, statusCast<Seat::Status, int>(seat.status));
        pstmt->setInt(3, seat.seatId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Seat Error: " << e.what() << std::endl;
        return false;
    }
}