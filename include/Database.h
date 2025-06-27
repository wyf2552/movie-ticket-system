class Datebase {
private:
    std::string host;
    std::string user;
    std::string password;
    std::string database;

    // 数据库连接对象
    sql::Driver *driver;
    std::unique_ptr<sql::Connection> conn;
public:
    Database(const std::string& host, const std::string& user,
             const std::string& password, const std::string& database);

    ~Datebase();

    bool connect();

    void diaconnect();

    bool execute(const std::string& query);

    std::unique_ptr<sql::ResultSet> query(const std::string& query);

    std::unique_ptr<sql::PreparedStatement>prepareStatement(const std::string& query);

    void beginTransaction();

    void commit();

    void rollback();

    int getLastInsertId();
};
