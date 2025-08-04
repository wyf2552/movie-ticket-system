module;

#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <concepts>
#include <type_traits>

export module entities;

export class User {
public:
    int userId;
    std::string username;
    std::string password;
    std::string realName;
    std::string gender;
    std::string phone;
    std::string email;
    std::string regTime;
    std::string lastLogin;
    int userStatus;
    int userType;

    User() : userId(0), userStatus(1), userType(0) {}

    User(int userId, const std::string& username, const std::string& password,
         const std::string& realName, const std::string& gender,
         const std::string& phone, const std::string& email,
         int userType = 0)
        : userId(userId), username(username), password(password),
          realName(realName), gender(gender), phone(phone),
          email(email), userStatus(1), userType(userType) {

        // 设置注册时间为当前时间
        time_t now = time(0);
        struct tm *ltm = localtime(&now);
        std::stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon << "-"
           << std::setw(2) << std::setfill('0') << ltm->tm_mday << " "
           << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
           << std::setw(2) << std::setfill('0') << ltm->tm_min << ":"
           << std::setw(2) << std::setfill('0') << ltm->tm_sec;
        regTime = ss.str();
        lastLogin = regTime;
    }

    // 判断是否为管理员
    bool isAdmin() const { return userType == 1; }
};

export using UserUptr = std::unique_ptr<User>;
export using UserSptr = std::shared_ptr<User>;

// 电影类
export class Movie {
public:
    int movieId = 0;
    std::string title;
    std::string director;
    std::string actors;
    std::string movieType;
    int duration = 0;
    std::string releaseDate;
    std::string language;
    std::string country;
    std::string synopsis;
    std::string poster;
    double rating = 0.0;
    enum class Status {
        onShow,
        offShow,
        upComing
    } status;

    Movie() = default;

    Movie(int movieId, const std::string& title, const std::string& director,
          const std::string& actors, const std::string& movieType, int duration,
          const std::string& releaseDate, const std::string& language,
          const std::string& country, const std::string& synopsis,
          const std::string& poster, double rating, Status status)
        : movieId(movieId), title(title), director(director), actors(actors),
          movieType(movieType), duration(duration), releaseDate(releaseDate),
          language(language), country(country), synopsis(synopsis),
          poster(poster), rating(rating), status(status) {}

    std::string getStatus() const {
        switch (status) {
            case Status::onShow: return "在映";
            case Status::offShow: return "下架";
            case Status::upComing: return "即将上映";
            default: return "未知";
        }
    }
};

export using MovieUptr = std::unique_ptr<Movie>;
export using MovieSptr = std::shared_ptr<Movie>;
// 影院类
export class Cinema {
public:
    int cinemaId = 0;
    std::string cinemaName;
    std::string address;
    std::string phone;
    std::string introduction;
    enum class Status{
        open,
        close
    } status;

    Cinema() = default;

    Cinema(int cinemaId, const std::string& cinemaName, const std::string& address,
           const std::string& phone, const std::string& introduction, Status status)
        : cinemaId(cinemaId), cinemaName(cinemaName), address(address),
          phone(phone), introduction(introduction), status(status) {}

    std::string getStatus() const {
        switch(status) {
            case Status::open: return "营业";
            case Status::close: return "停业";
            default: return "未知";
        }
    }


};

export using CinemaUptr = std::unique_ptr<Cinema>;
export using CinemaSptr = std::shared_ptr<Cinema>;

//影厅类
export class Hall {
public:
    int hallId = 0;
    int cinemaId = 0;
    std::string hallName;
    int seatCount = 0;
    std::string hallType; // 普通, IMAX, 3D等
    enum class Status {
        normal,
        maintenance
    } status;

    Hall() =default;

    Hall(int hallId, int cinemaId, const std::string& hallName,
         int seatCount, const std::string& hallType, Status status)
        : hallId(hallId), cinemaId(cinemaId), hallName(hallName),
          seatCount(seatCount), hallType(hallType), status(status) {}

    std::string getStatus() const {
        switch(status) {
            case Status::normal: return "正常";
            case Status::maintenance: return "维护";
            default: return "未知";
        }
    }
};

export using HallUptr = std::unique_ptr<Hall>;
export using HallSptr = std::shared_ptr<Hall>;

// 排片类
export class Screening {
public:
    int screeningId = 0;
    int movieId = 0;
    int cinemaId = 0;
    int hallId = 0;
    std::string startTime;
    std::string endTime;
    double price = 0.0;
    std::string languageVersion;
    enum class Status {
        normal,
        cancle
    } status;

    // 关联信息
    std::string movieTitle;
    std::string cinemaName;
    std::string hallName;

    Screening() = default;

    Screening(int screeningId, int movieId, int cinemaId, int hallId,
              const std::string& startTime, const std::string& endTime,
              double price, const std::string& languageVersion, Status status)
        : screeningId(screeningId), movieId(movieId), cinemaId(cinemaId), hallId(hallId),
          startTime(startTime), endTime(endTime), price(price),
          languageVersion(languageVersion), status(status) {}

    std::string getStatus() const {
        switch(status) {
            case Status::normal: return "正常";
            case Status::cancle: return "取消";
            default: return "未知";
        }
    }
};

export using ScreeningUptr = std::unique_ptr<Screening>;
export using ScreeningSptr = std::shared_ptr<Screening>;

// 座位类
export class Seat {
public:
    int seatId = 0;
    int hallId = 0;
    int rowNum = 0;
    int columnNum = 0;
    enum class SeatType {
        normal,
        couple,
        VIP
    } seatType;
    enum class Status {
        normal,
        maintenance
    } status;

    Seat() = default;

    Seat(int seatId, int hallId, int rowNum, int columnNum, SeatType seatType, Status status)
        : seatId(seatId), hallId(hallId), rowNum(rowNum), columnNum(columnNum),
          seatType(seatType), status(status) {}

    // 获取座位位置描述
    std::string getPositionStr() const {
        return std::to_string(rowNum) + "排" + std::to_string(columnNum) + "座";
    }

    std::string getTypeStr() const {
        switch (seatType) {
            case SeatType::normal: return "普通座位";
            case SeatType::couple: return "情侣座位";
            case SeatType::VIP: return "VIP座位";
            default: return "未知";
        }
    }

    std::string getStatus() const {
        switch (status) {
            case Status::normal: return "正常";
            case Status::maintenance: return "维护";
            default: return "未知";
        }
    }
};

export using SeatUptr = std::unique_ptr<Seat>;
export using SeatSptr = std::shared_ptr<Seat>;

// 排片座位类
export class ScreeningSeat {
public:
    int screeningSeatId = 0;
    int screeningId = 0;
    int seatId = 0;
    enum class Status {
        avilable,
        sold,
        lock
    } status;

    std::string lockTime;
    int lockUserId = 0;

    // 关联信息
    int rowNum = 0;
    int columnNum = 0;

    ScreeningSeat()  = default;

    ScreeningSeat(int screeningSeatId, int screeningId, int seatId,
        Status status,const std::string& lockTime, int lockUserId)
        : screeningSeatId(screeningSeatId), screeningId(screeningId),
            seatId(seatId),status(status), lockTime(lockTime),
            lockUserId(lockUserId) {}

    std::string getStatus() const {
        switch (status) {
            case Status::avilable: return "可用";
            case Status::sold: return "已售";
            case Status::lock: return "锁定";
            default: return "未知";
        }
    }

    std::string getPositionStr() const {
        return std::to_string(rowNum) + "排" + std::to_string(columnNum) + "座";
    }
};

export using ScreeningSeatUptr = std::unique_ptr<ScreeningSeat>;
export using ScreeningSeatSptr = std::shared_ptr<ScreeningSeat>;

export class Order {
public:
    int orderId = 0;
    std::string orderNo;
    int userId = 0;
    int screeningId = 0;
    double totalAmount = 0.0;
    std::string createTime;
    std::string payTime;
    enum class PayMethod {
        alipay,
        wechat,
        bank
    } payMethod;
    enum class Status {
        nopaid,
        paid,
        cancle,
        complete
    } status;

    std::string username;
    std::string movieTitle;
    std::string cinemaName;
    std::string hallName;
    std::string startTime;
    std::vector<std::string> seatPositions;

    Order() = default;

    Order(int orderId, const std::string& orderNo, int userId, int screeningId,
          double totalAmount, const std::string& createTime, const std::string& payTime,
          PayMethod PayMethod, Status status)
        : orderId(orderId), orderNo(orderNo), userId(userId), screeningId(screeningId),
          totalAmount(totalAmount), createTime(createTime), payTime(payTime),
          payMethod(PayMethod), status(status) {}

    std::string getPayMethodStr() const {
        switch (payMethod) {
            case PayMethod::alipay: return "支付宝";
            case PayMethod::wechat: return "微信";
            case PayMethod::bank: return "银行卡";
            default: return "未支付";
        }
    }

    std::string getOrderStatusStr() const {
        switch (status) {
            case Status::nopaid: return "待支付";
            case Status::paid: return "已支付";
            case Status::cancle: return "已取消";
            case Status::complete: return "已完成";
            default: return "未知";
        }
    }
};

export using OrderUptr = std::unique_ptr<Order>;
export using OrderSptr = std::shared_ptr<Order>;

export template <typename From, typename To>
requires ((std::is_enum_v<From> && std::same_as<To, int>) ||
          (std::is_enum_v<To> && std::same_as<From, int> ))
constexpr To statusCast(From value) {
    return static_cast<To>(value);
}
