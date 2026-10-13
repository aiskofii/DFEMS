#pragma once
#include <iostream>
#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

void mysqlConnect();

void mysqlConnect() {
    const std::string server = "tcp://127.0.0.1:3306";
    const std::string username = "root";
    const std::string password = "";

    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        system("pause");
        exit(1);
    }

    con->setSchema("dfems");
    std::unique_ptr<sql::Statement> startupStmt(con->createStatement());
    startupStmt->executeUpdate("CREATE TABLE IF NOT EXISTS users ( user_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, full_name VARCHAR(255) NOT NULL, email VARCHAR(255) NOT NULL, phone VARCHAR(50), username VARCHAR(100) NOT NULL UNIQUE KEY, password VARCHAR(255) NOT NULL, roles VARCHAR(50) NOT NULL );");
    
}