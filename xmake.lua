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
target("userservice")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "UserService.cxx",
        "test/UserService.cxx"
    )

-- Target: MovieService
target("movieservice")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "UserService.cxx",
        "MovieService.cxx",
        "test/MovieService.cxx"
    )

-- Target: CinemaService
target("cinemaservice")
    set_kind("binary")
    add_files(
        "Database.cxx",
        "Entities.cxx",
        "UserService.cxx",
        "MovieService.cxx",
        "CinemaService.cxx",
        "test/CinemaService.cxx"
    )