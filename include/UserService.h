class UserService {
private:
    Database& db;
public:
    UserService(Database& database);

    bool registerUser(User& user);

    User* login(const std::string& username,const std::string& password);

    User* getUserById(int userId);

    std::vector<User*> getAllUsers();

    
}