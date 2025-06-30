import database;

#include <iostream>
#include <cppconn/resultset.h>

int main() {
    // 创建数据库连接
    Database db("tcp://localhost:3306", "root", "123456wyf", "");

    // 连接数据库
    if (db.connect()) {
        std::cout << "Connected to the database successfully." << std::endl;
        db.execute("CREATE DATABASE IF NOT EXISTS movie");
        // 执行一些数据库操作
        db.setSchema("movie");
        db.execute("CREATE TABLE IF NOT EXISTS test (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255))");
        db.execute("INSERT INTO test (name) VALUES ('John Doe')");
        db.execute("INSERT INTO test (name) VALUES ('Jane Doe')");

        // 查询并打印结果
        auto rs = db.query("SELECT * FROM test");
        if (rs && rs->next()) {
            std::cout << "ID: " << rs->getInt("id") << ", Name: " << rs->getString("name") << std::endl;
        }
        db.execute("drop database movie");
        // 断开连接
        db.disconnect();
    } else {
        std::cerr << "Failed to connect to the database." << std::endl;
    }

    return 0;
}