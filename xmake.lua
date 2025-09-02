-- Define common configuration for both targets
local mysql_path = "/usr/local/mysql-connector-c++-8.4.0/"
local mysql_include = path.join(mysql_path, "include/jdbc")
local mysql_libdir = path.join(mysql_path, "lib64")

add_rules("mode.debug", "mode.release")

set_warnings("all")
set_languages("c23", "c++26")
set_policy("build.c++.modules", true)
set_toolchains("llvm")

set_config("runtimes", "c++_shared") -- Set C++ runtime to shared
set_config("cxflags", "-fuse-ld=mold")
set_config("cxxflags", "-stdlib=libc++")
set_config("includedirs", mysql_include)
set_config("linkdirs", mysql_libdir)
set_config("links", "mysqlcppconn")
set_config("rpathdirs", mysql_libdir)

-- Target: entities
target("entities")
    set_kind("binary")
    add_files(
        "Entities.cxx",
        "test/Entities.cxx"
    )

-- Target: database
target("database")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "test/Database.cxx"
    )

-- Target: UserService
target("service.user")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "Service.User.cxx",
        "test/Service.User.cxx"
    )

-- Target: MovieService
target("service.movie")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "Service.User.cxx",
        "Service.Movie.cxx",
        "test/Service.Movie.cxx"
    )

-- Target: CinemaService
target("service.cinema")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "Service.User.cxx",
        "Service.Movie.cxx",
        "Service.Cinema.cxx",
        "test/Service.Cinema.cxx"
    )

-- Target: ScreeningService
target("service.screening")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "Service.User.cxx",
        "Service.Movie.cxx",
        "Service.Cinema.cxx",
        "Service.Screening.cxx",
        "test/Service.Screening.cxx"
    )

-- Target: OrderService
target("service.order")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "Service.User.cxx",
        "Service.Movie.cxx",
        "Service.Cinema.cxx",
        "Service.Screening.cxx",
        "Service.Order.cxx",
        "test/Service.Order.cxx"
    )

target("view.helper")
    set_kind("binary")
    add_files(
        "view/View.Helper.cxx",
        "test/View.Helper.cxx"
    )

target("view.auth")
    set_kind("binary")
    add_files(
        "Service.User.cxx",
        "Database.cxx",
        "Entities.cxx",
        "view/View.Helper.cxx",
        "view/View.Auth.cxx",
        "test/View.Auth.cxx"
    )

target("view.movie")
    set_kind("binary")
    add_files(
        "Service.User.cxx",
        "Database.cxx",
        "Entities.cxx",
        "Service.Movie.cxx",
        "Service.Screening.cxx",
        "Service.Cinema.cxx",
        "view/View.Helper.cxx",
        "view/View.Auth.cxx",
        "view/View.Movie.cxx",
        "test/View.Movie.cxx"
    )

target("view.ticket")
    set_kind("binary")
    add_files(
        "Service.User.cxx",
        "Database.cxx",
        "Entities.cxx",
        "Service.Movie.cxx",
        "Service.Screening.cxx",
        "Service.Cinema.cxx",
        "Service.Order.cxx",
        "view/View.Helper.cxx",
        "view/View.Auth.cxx",
        "view/View.Movie.cxx",
        "view/View.Ticket.cxx",
        "test/View.Ticket.cxx"
    )

target("view.order")
    set_kind("binary")
    add_files(
        "Service.User.cxx",
        "Database.cxx",
        "Entities.cxx",
        "Service.Movie.cxx",
        "Service.Screening.cxx",
        "Service.Cinema.cxx",
        "Service.Order.cxx",
        "view/View.Helper.cxx",
        "view/View.Auth.cxx",
        "view/View.Movie.cxx",
        "view/View.Ticket.cxx",
        "view/View.Order.cxx",
        "test/View.Order.cxx"
    )

target("maincontroller")
    set_kind("binary")
    add_files(
        "Service.User.cxx",
        "Database.cxx",
        "Entities.cxx",
        "Service.Movie.cxx",
        "Service.Screening.cxx",
        "Service.Cinema.cxx",
        "Service.Order.cxx",
        "view/View.Helper.cxx",
        "view/View.Auth.cxx",
        "view/View.Movie.cxx",
        "view/View.Order.cxx",
        "view/View.Ticket.cxx",
        "view/MainController.cxx",
        "test/MainController.cxx"
    )