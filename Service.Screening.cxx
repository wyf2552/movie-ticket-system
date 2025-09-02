module;

#include <ctime>
#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

import database;
import entities;
import service.user;
import service.movie;
import service.cinema;

export module service.screening;

export class ScreeningService {
private:
    Database& _db;
public:
    ScreeningService(Database& database);

    bool addScreening(Screening& screening);

    ScreeningUptr getScreeningById(int screeningId);

    std::vector<ScreeningUptr> getScreeningsByMovieId(int movieId);

    std::vector<ScreeningUptr> getScreeningsByCinemaId(int cinemaId);

    std::vector<ScreeningSeatUptr> getScreeningSeats(int screeningId);

    bool updateScreening(const Screening& screening);

    bool deleteScreening(int screeningId);

    bool lockSeat(int screeningSeatId, int userId);

    bool unlockSeat(int screeningSeatId, int userId);

    void releaseTimeoutSeats();
};

export using ScreeningServiceSptr = std::shared_ptr<ScreeningService>;

ScreeningService::ScreeningService(Database& database) : _db(database) {}

bool ScreeningService::addScreening(Screening& screening) {
    try {
        auto pstmt = _db.prepareStatement("insert into screening(movie_id, cinema_id, hall_id, start_time, end_time, price, language_version, status) values (?, ?, ?, ?, ?, ?, ?, ?)");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, screening.movieId);
        pstmt->setInt(2, screening.cinemaId);
        pstmt->setInt(3, screening.hallId);
        pstmt->setString(4, screening.startTime);
        pstmt->setString(5, screening.endTime);
        pstmt->setDouble(6, screening.price);
        pstmt->setString(7, screening.languageVersion);
        pstmt->setInt(8, statusCast<Screening::Status, int>(screening.status));

        pstmt->executeUpdate();

        int screeningId = _db.getLastInsertId();
        screening.screeningId = screeningId;

        auto pstmt2 = _db.prepareStatement("insert into screeningseat(screening_id, seat_id, status) select ?, seat_id, 0 from seat where hall_id = ?");
        if (!pstmt2) {
            return false;
        }

        pstmt2->setInt(1, screening.screeningId);
        pstmt2->setInt(2, screening.hallId);
        pstmt2->executeUpdate();

        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Add Screening Error: " << e.what() << std::endl;
        return false;
    }
}

ScreeningUptr ScreeningService::getScreeningById(int screeningId) {
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s.screening_id = ?");

        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, screeningId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            auto screening = std::make_unique<Screening>();
            screening->screeningId = rs->getInt("screening_id");
            screening->movieId = rs->getInt("movie_id");
            screening->cinemaId = rs->getInt("cinema_id");
            screening->hallId = rs->getInt("hall_id");
            screening->startTime = rs->getString("start_time");
            screening->endTime = rs->getString("end_time");
            screening->price = rs->getDouble("price");
            screening->languageVersion = rs->getString("language_version");
            screening->status = statusCast<int, Screening::Status>(rs->getInt("status"));

            screening->movieTitle = rs->getString("movie_title");
            screening->cinemaName = rs->getString("cinema_name");
            screening->hallName = rs->getString("hall_name");

            return screening;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screening Error: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<ScreeningUptr> ScreeningService::getScreeningsByMovieId(int movieId) {
    std::vector<ScreeningUptr> screenings;
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s.movie_id = ? and s.start_time >= now() order by s.start_time");

        if (!pstmt) {
            return screenings;
        }

        pstmt->setInt(1, movieId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto screening = std::make_unique<Screening>();
                screening->screeningId = rs->getInt("screening_id");
                screening->movieId = rs->getInt("movie_id");
                screening->cinemaId = rs->getInt("cinema_id");
                screening->hallId = rs->getInt("hall_id");
                screening->startTime = rs->getString("start_time");
                screening->endTime = rs->getString("end_time");
                screening->price = rs->getDouble("price");
                screening->languageVersion = rs->getString("language_version");
                screening->status = statusCast<int, Screening::Status>(rs->getInt("status"));

                screening->movieTitle = rs->getString("movie_title");
                screening->cinemaName = rs->getString("cinema_name");
                screening->hallName = rs->getString("hall_name");

                screenings.push_back(std::move(screening));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screenings By Movie Error: " << e.what() << std::endl;
    }
    return screenings;
}

std::vector<ScreeningUptr> ScreeningService::getScreeningsByCinemaId(int cinemaId) {
    std::vector<ScreeningUptr> screenings;
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s.cinema_id = ? and s.start_time >= now() order by s.start_time");

        if (!pstmt) {
            return screenings;
        }

        pstmt->setInt(1, cinemaId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto screening = std::make_unique<Screening>();
                screening->screeningId = rs->getInt("screening_id");
                screening->movieId = rs->getInt("movie_id");
                screening->cinemaId = rs->getInt("cinema_id");
                screening->hallId = rs->getInt("hall_id");
                screening->startTime = rs->getString("start_time");
                screening->endTime = rs->getString("end_time");
                screening->price = rs->getDouble("price");
                screening->languageVersion = rs->getString("language_version");
                screening->status = statusCast<int, Screening::Status>(rs->getInt("status"));

                screening->movieTitle = rs->getString("movie_title");
                screening->cinemaName = rs->getString("cinema_name");
                screening->hallName = rs->getString("hall_name");

                screenings.push_back(std::move(screening));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screenings By Cinema Error: " << e.what() << std::endl;
    }
    return screenings;
}

std::vector<ScreeningSeatUptr> ScreeningService::getScreeningSeats(int screeningId) {
    std::vector<ScreeningSeatUptr> seats;
    try {
        auto pstmt = _db.prepareStatement("select ss.*, s.row_num, s.column_num from screeningseat ss join seat s on ss.seat_id = s.seat_id where ss.screening_id = ? order by s.row_num, s.column_num");

        if (!pstmt) {
            return seats;
        }

        pstmt->setInt(1, screeningId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                auto seat = std::make_unique<ScreeningSeat>();
                seat->screeningSeatId = rs->getInt("screening_seat_id");
                seat->screeningId = rs->getInt("screening_id");
                seat->seatId = rs->getInt("seat_id");
                seat->status = statusCast<int, ScreeningSeat::Status>(rs->getInt("status"));
                seat->lockTime = rs->getString("lock_time");
                seat->lockUserId = rs->getInt("lock_user_id");
                seat->rowNum = rs->getInt("row_num");
                seat->columnNum = rs->getInt("column_num");
                seats.push_back(std::move(seat));
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screening Seats Error: " << e.what() << std::endl;
    }
    return seats;
}

bool ScreeningService::updateScreening(const Screening& screening) {
    try {
        auto pstmt = _db.prepareStatement("update screening set movie_id = ?, cinema_id = ?, hall_id = ?, start_time = ?, end_time = ?, price = ?, language_version = ?, status = ? where screening_id = ?");

        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, screening.movieId);
        pstmt->setInt(2, screening.cinemaId);
        pstmt->setInt(3, screening.hallId);
        pstmt->setString(4, screening.startTime);
        pstmt->setString(5, screening.endTime);
        pstmt->setDouble(6, screening.price);
        pstmt->setString(7, screening.languageVersion);
        pstmt->setInt(8, statusCast<Screening::Status, int>(screening.status));
        pstmt->setInt(9, screening.screeningId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Screening Error: " << e.what() << std::endl;
        return false;
    }
}

bool ScreeningService::deleteScreening(int screeningId) {
    try {
        auto pstmt = _db.prepareStatement("select count(*) from orders where screening_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, screeningId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "排片存在关联订单，无法删除!" << std::endl;
            return false;
        }

        _db.beginTransaction();

        pstmt = _db.prepareStatement("delete from screeningseat where screening_id = ?");
        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, screeningId);
        pstmt->executeUpdate();

        pstmt = _db.prepareStatement("delete from screening where screening_id = ?");
        if (!pstmt) {
            _db.rollback();
            return false;
        }

        pstmt->setInt(1, screeningId);
        bool result = pstmt->executeUpdate() > 0;

        if (result) {
            _db.commit();
        } else {
            _db.rollback();
        }
        return result;
    } catch (sql::SQLException &e) {
        std::cerr << "Delete Screening Error: " << e.what() << std::endl;
        _db.rollback();
        return false;
    }
}

bool ScreeningService::lockSeat(int screeningSeatId, int userId) {
    try {
        auto pstmt = _db.prepareStatement("update screeningseat set status = 2, lock_time = now(), lock_user_id = ? where screening_seat_id = ? and status = 0");

        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, userId);
        pstmt->setInt(2, screeningSeatId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Lock Seat Error: " << e.what() << std::endl;
        return false;
    }
}

bool ScreeningService::unlockSeat(int screeningSeatId, int userId) {
    try {
        auto pstmt = _db.prepareStatement("update screeningseat set status = 0, lock_time = NULL, lock_user_id = NULL where screening_seat_id = ? and status = 2 and lock_user_id = ?");

        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, screeningSeatId);
        pstmt->setInt(2, userId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Unlock Seat Error: " << e.what() << std::endl;
        return false;
    }
}

void ScreeningService::releaseTimeoutSeats() {
    try {
        _db.execute("update screeningseat set status = 0, lock_time = NULL, lock_user_id = NULL where status = 2 and lock_time < date_sub(now(), interval 15 minute)");
    }  catch (sql::SQLException &e) {
        std::cerr << "Unlock Seat Error: " << e.what() << std::endl;
    }
}