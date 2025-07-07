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