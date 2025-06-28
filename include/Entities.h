#include <string>
#include <iomanip>
#include <sstream>

// 用户类
class User {
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

    // 构造函数
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

enum class Status {
    onShow,
    offShow,
    upComing
};
// 电影类
class Movie {
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
    Status status = Status::onShow;

    // 构造函数
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


    // 获取状态描述
    std::string getStatusDescription() const {
        switch (status) {
            case Status::onShow: return "在映";
            case Status::offShow: return "下架";
            case Status::upComing: return "即将上映";
            default: return "未知";
        }
    }
};