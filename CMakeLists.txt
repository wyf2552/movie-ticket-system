cmake_minimum_required(VERSION 3.10)
project(movie-ticket CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable target
add_executable(${PROJECT_NAME}
    main.cpp
    src/Database.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    include
    /usr/local/mysql-connector-c++-8.4.0/include/jdbc
)

# 设置MySQL连接器路径
set(MYSQL_CONNECTOR_PATH "/usr/local/mysql-connector-c++-8.4.0")

# 添加库文件目录
target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${MYSQL_CONNECTOR_PATH}/lib64/
)


target_link_libraries(${PROJECT_NAME}
   mysqlcppconn
)

target_link_options(${PROJECT_NAME} PRIVATE
    "-Wl,-rpath,${MYSQL_CONNECTOR_PATH}/lib64/"
)

# Set output directory for the executable to source root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})