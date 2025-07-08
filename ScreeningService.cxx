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

export module screeningservice;

export class ScreeningService {
private:
    Database& _db;
public:
    ScreeningService(Database& database);

    bool addScreening(Screening& screening);

    Screening* getScreeningById(int screeningId);

    std::vector<Screening*> getScreeningsByMovieId(int movieId);

    std::vector<Screening*> getScreeningsByCinemaId(int cinemaId);

    std::vector<ScreeningSeat*> getScreeningSeats(int screeningId);

    bool updateScreening(const Screening& screening);

    bool deleteScreening(int screeningId);

    bool lockSeat(int screeningSeatId, int userId);

    bool unlockSeat(int screeningSeatId, int userId);

    void releaseTimeoutSeats();
};

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

Screening* ScreeningService::getScreeningById(int screeningId) {
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s.screening_id = ?");

        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, screeningId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            Screening* screening = new Screening();
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

std::vector<Screening*> ScreeningService::getScreeningsByMovieId(int movieId) {
    std::vector<Screening*> screenings;
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name form screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s.movie_id = ? and s.start_time >= now() order by s.start_time");

        if (!pstmt) {
            return screenings;
        }

        pstmt->setInt(1, movieId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                Screening* screening = new Screening();
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

                screenings.push_back(screening);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screenings By Movie Error: " << e.what() << std::endl;
    }
    return screenings;
}

std::vector<Screening*> ScreeningService::getScreeningsByCinemaId(int cinemaId) {
    std::vector<Screening*> screenings;
    try {
        auto pstmt = _db.prepareStatement("select s.*, m.title as movie_title, c.cinema_name, h.hall_name from screening s join movie m on s.movie_id = m.movie_id join cinema c on s.cinema_id = c.cinema_id join hall h on s.hall_id = h.hall_id where s cinema_id = ? and s.start_time >= now() order by s.start_time");

        if (!pstmt) {
            return screenings;
        }

        pstmt->setInt(1, cinemaId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                Screening* screening = new Screening();
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

                screenings.push_back(screening);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Screenings By Cinema Error: " << e.what() << std::endl;
    }
    return screenings;
}