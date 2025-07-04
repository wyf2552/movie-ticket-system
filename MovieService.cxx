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

export module movieservice;

export class MovieService {
private:
    Database& _db;
public:
    MovieService(Database& database);

    bool addMovie(Movie& movie);

    Movie* getMovieById(int moveiId);

    std::vector<Movie*> getAllMovies();

    std::vector<Movie*> getNowPlayingMovies();

    std::vector<Movie*> getComingSoonMovies();

    bool updateMovie(const Movie& movie);

    bool deleteMovie(int movieId);

    std::vector<Movie*> searchMovies(const std::string& keyword);
};

MovieService::MovieService(Database& database) : _db(database) {}

bool MovieService::addMovie(Movie& movie) {
    try {
        auto pstmt = _db.prepareStatement("insert into Movie (title, director, actors, movie_type, duration, release_date, language, country, synopsis, poster, rating, status) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, movie.title);
        pstmt->setString(2, movie.director);
        pstmt->setString(3, movie.actors);
        pstmt->setString(4, movie.movieType);
        pstmt->setInt(5, movie.duration);
        pstmt->setString(6, movie.releaseDate);
        pstmt->setString(7, movie.language);
        pstmt->setString(8, movie.country);
        pstmt->setString(9, movie.synopsis);
        pstmt->setString(10, movie.poster);
        pstmt->setDouble(11, movie. rating);
        pstmt->setInt(12, statusCast<Movie::Status, int>(movie.status));

        pstmt->executeUpdate();

        movie.movieId = _db.getLastInsertId();
        return true;
    } catch (sql::SQLException &e) {
        std::cerr << "Add Movie Error: " << e.what() << std::endl;
        return false;
    }
}

Movie* MovieService::getMovieById(int movieId) {
    try {
        auto pstmt = _db.prepareStatement("select * from Movie where movie_id = ?");
        if (!pstmt) {
            return nullptr;
        }

        pstmt->setInt(1, movieId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next()) {
            Movie* movie = new Movie();
            movie->movieId = rs->getInt("movie_id");
            movie->title = rs->getString("title");
            movie->director = rs->getString("director");
            movie->actors = rs->getString("actors");
            movie->movieType = rs->getString("movie_type");
            movie->duration = rs->getInt("duration");
            movie->releaseDate = rs->getString("release_date");
            movie->language = rs->getString("language");
            movie->country = rs->getString("country");
            movie->synopsis = rs->getString("synopsis");
            movie->poster = rs->getString("poster");
            movie->rating = rs->getDouble("rating");
            // movie->status = static_cast<Movie::Status>(rs->getInt("status"));
            movie->status = statusCast<int, Movie::Status>(rs->getInt("status"));
            return movie;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "Get Movie Error: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<Movie*> MovieService::getAllMovies() {
    std::vector<Movie*> movies;
    try {
        auto rs = _db.query("select * from Movie order by release_date DESC");

        if (rs) {
            while (rs->next()) {
                while (rs->next()) {
                    Movie* movie = new Movie();
                    movie->movieId = rs->getInt("movie_id");
                    movie->title = rs->getString("title");
                    movie->director = rs->getString("director");
                    movie->actors = rs->getString("actors");
                    movie->movieType = rs->getString("movie_type");
                    movie->duration = rs->getInt("duration");
                    movie->releaseDate = rs->getString("release_date");
                    movie->language = rs->getString("language");
                    movie->country = rs->getString("country");
                    movie->synopsis = rs->getString("synopsis");
                    movie->poster = rs->getString("poster");
                    movie->rating = rs->getDouble("rating");
                    movie->status = statusCast<int, Movie::Status>(rs->getInt("status"));
                    movies.push_back(movie);
                }
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get All Movies Error: " << e.what() << std::endl;
    }
    return movies;
}

std::vector<Movie*> MovieService::getNowPlayingMovies() {
    std::vector<Movie*> movies;
    try {
        auto rs = _db.query("select * from Movie where status = 1 order by release_date DESC");

        if (rs) {
            while (rs->next()) {
                Movie* movie = new Movie();
                movie->movieId = rs->getInt("movie_id");
                movie->title = rs->getString("title");
                movie->director = rs->getString("director");
                movie->actors = rs->getString("actors");
                movie->movieType = rs->getString("movie_type");
                movie->duration = rs->getInt("duration");
                movie->releaseDate = rs->getString("release_date");
                movie->language = rs->getString("language");
                movie->country = rs->getString("country");
                movie->synopsis = rs->getString("synopsis");
                movie->poster = rs->getString("poster");
                movie->rating = rs->getDouble("rating");
                movie->status = statusCast<int, Movie::Status>(rs->getInt("status"));
                movies.push_back(movie);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Now Playing Movies Error: " << e.what() << std::endl;
    }
    return movies;
}

std::vector<Movie*> MovieService::getComingSoonMovies() {
    std::vector<Movie*> movies;
    try {
        auto rs = _db.query("select * from Movie where status = 2 order by release_date ASC");

        if (rs) {
            while (rs->next()) {
                Movie* movie = new Movie();
                movie->movieId = rs->getInt("movie_id");
                movie->title = rs->getString("title");
                movie->director = rs->getString("director");
                movie->actors = rs->getString("actors");
                movie->movieType = rs->getString("movie_type");
                movie->duration = rs->getInt("duration");
                movie->releaseDate = rs->getString("release_date");
                movie->language = rs->getString("language");
                movie->country = rs->getString("country");
                movie->synopsis = rs->getString("synopsis");
                movie->poster = rs->getString("poster");
                movie->rating = rs->getDouble("rating");
                movie->status = statusCast<int, Movie::Status>(rs->getInt("status"));
                movies.push_back(movie);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Get Coming Soon Movies Error: " << e.what() << std::endl;
    }
    return movies;
}

bool MovieService::updateMovie(const Movie& movie) {
    try {
        auto pstmt = _db.prepareStatement("update Movie set title = ?, director = ?, actors = ?, movie_type = ?, duration = ?, release_date = ?, language = ?, country = ?, synopsis = ?, poster = ?, rating = ?, status = ? where movie_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setString(1, movie.title);
        pstmt->setString(2, movie.director);
        pstmt->setString(3, movie.actors);
        pstmt->setString(4, movie.movieType);
        pstmt->setInt(5, movie.duration);
        pstmt->setString(6, movie.releaseDate);
        pstmt->setString(7, movie.language);
        pstmt->setString(8, movie.country);
        pstmt->setString(9, movie.synopsis);
        pstmt->setString(10, movie.poster);
        pstmt->setDouble(11, movie.rating);
        pstmt->setInt(12, statusCast<Movie::Status, int>(movie.status));
        pstmt->setInt(13, movie.movieId);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Update Movie Error: " << e.what() << std::endl;
        return false;
    }
}

bool MovieService::deleteMovie(int movieId) {
    try {
        auto pstmt = _db.prepareStatement("select count(*) from screening where movie_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, movieId);
        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs && rs->next() && rs->getInt(1) > 0) {
            std::cout << "电影已关联排片,无法删除!" << std::endl;
            return false;
        }

        pstmt = _db.prepareStatement("delete from Movie where movie_id = ?");
        if (!pstmt) {
            return false;
        }

        pstmt->setInt(1, movieId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "Delete Movie Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Movie*> MovieService::searchMovies(const std::string& keyword) {
    std::vector<Movie*> movies;
    try {
        auto pstmt = _db.prepareStatement("select * from movie where title like ? or director like ? or actors like ? order by release_date DESC"
        );

        if (!pstmt) {
            return movies;
        }

        std::string likeKeyword = "%" + keyword + "%";
        pstmt->setString(1, likeKeyword);
        pstmt->setString(2, likeKeyword);
        pstmt->setString(3, likeKeyword);

        auto rs = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

        if (rs) {
            while (rs->next()) {
                Movie* movie = new Movie();
                movie->movieId = rs->getInt("movie_id");
                movie->title = rs->getString("title");
                movie->director = rs->getString("director");
                movie->actors = rs->getString("actors");
                movie->movieType = rs->getString("movie_type");
                movie->duration = rs->getInt("duration");
                movie->releaseDate = rs->getString("release_date");
                movie->language = rs->getString("language");
                movie->country = rs->getString("country");
                movie->synopsis = rs->getString("synopsis");
                movie->poster = rs->getString("poster");
                movie->rating = rs->getDouble("rating");
                movie->status = statusCast<int, Movie::Status>(rs->getInt("status"));
                movies.push_back(movie);
            }
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Search Movies Error: " << e.what() << std::endl;
    }
    return movies;
}