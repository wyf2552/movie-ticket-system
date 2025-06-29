-- Define common configuration for both targets
local mysql_path = "/usr/local/mysql-connector-c++-9.0.0"
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

-- Target: database
target("database")
    set_kind("binary")
    add_files("Entities.cpp", "main.cpp")

-- Target: entities
target("entities")
    set_kind("binary")
    add_files("Database.cpp", "main_database.cpp")
