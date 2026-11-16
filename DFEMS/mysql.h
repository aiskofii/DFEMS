#pragma once

#include <iostream>
#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

void mysqlConnect();

// CREATE TABLE IF NOT EXISTS never changes a table that already exists,
// so new columns are added here, only when they are missing.
void addColumnIfMissing(const std::string& table, const std::string& column, const std::string& definition) {
    std::unique_ptr<sql::PreparedStatement> check(con->prepareStatement(
        "SELECT COUNT(*) AS total FROM information_schema.COLUMNS "
        "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = ? AND COLUMN_NAME = ?"));
    check->setString(1, table);
    check->setString(2, column);
    std::unique_ptr<sql::ResultSet> res(check->executeQuery());

    if (res->next() && res->getInt("total") == 0) {
        std::unique_ptr<sql::Statement> alter(con->createStatement());
        alter->executeUpdate("ALTER TABLE " + table + " ADD COLUMN " + column + " " + definition);
    }
}

void mysqlConnect() {
    const std::string server   = "tcp://127.0.0.1:3306";
    const std::string username = "root";
    const std::string password = "";

    try {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    } catch (const sql::SQLException& e) {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        system("pause");
        exit(1);
    }

    con->setSchema("dfems");

    std::unique_ptr<sql::Statement> startupStmt(con->createStatement());
    startupStmt->executeUpdate(
        "CREATE TABLE IF NOT EXISTS users ("
        "  user_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
        "  full_name VARCHAR(255) NOT NULL,"
        "  email VARCHAR(255) NOT NULL,"
        "  phone VARCHAR(50),"
        "  username VARCHAR(100) NOT NULL UNIQUE KEY,"
        "  password VARCHAR(255) NOT NULL,"
        "  roles VARCHAR(50) NOT NULL,"
        "  roles_id INT NOT NULL"
        ");"
    );

    startupStmt->executeUpdate(
        "CREATE TABLE IF NOT EXISTS cases ("
        "  case_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
        "  case_number VARCHAR(50) NOT NULL UNIQUE KEY,"
        "  case_name VARCHAR(255) NOT NULL,"
        "  description TEXT,"
        "  status VARCHAR(50) NOT NULL,"    // Open, Under Investigation
        "  assigned_to INT,"
        "  created_by INT,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (assigned_to) REFERENCES users(user_id),"
        "  FOREIGN KEY (created_by) REFERENCES users(user_id)"
        ");"
    );

    startupStmt->executeUpdate(
        "CREATE TABLE IF NOT EXISTS archived_cases ("
        "  case_id INT NOT NULL PRIMARY KEY,"
        "  case_number VARCHAR(50) NOT NULL,"
        "  case_name VARCHAR(255) NOT NULL,"
        "  description TEXT,"
        "  status VARCHAR(50) NOT NULL,"
        "  assigned_to INT,"
        "  created_by INT,"
        "  created_at DATETIME,"
        "  updated_at DATETIME,"
        "  archived_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (created_by) REFERENCES users(user_id)"
        ");"
    );

    startupStmt->executeUpdate(
        "CREATE TABLE IF NOT EXISTS evidences ("
        "  evidence_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
        "  case_id INT NOT NULL,"
        "  evidence_name VARCHAR(255) NOT NULL,"
        "  type VARCHAR(100) NOT NULL,"
        "  description TEXT,"
        "  location VARCHAR(255),"
        "  collected_by INT,"
        "  collected_at DATETIME,"
        "  status VARCHAR(50) NOT NULL,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (case_id) REFERENCES cases(case_id),"
        "  FOREIGN KEY (collected_by) REFERENCES users(user_id)"
        ");"
    );

    startupStmt->executeUpdate(
        "CREATE TABLE IF NOT EXISTS chain_of_custody ("
        "  custody_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
        "  evidence_id INT NOT NULL,"
        "  action VARCHAR(100) NOT NULL,"
        "  from_user INT,"
        "  to_user INT NOT NULL,"
        "  location VARCHAR(255),"
        "  notes TEXT,"
        "  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (evidence_id) REFERENCES evidences(evidence_id),"
        "  FOREIGN KEY (from_user) REFERENCES users(user_id),"
        "  FOREIGN KEY (to_user) REFERENCES users(user_id)"
        ");"
    );

    // Case team: cases.assigned_to is the main investigator, these are the other three slots
    try {
        addColumnIfMissing("cases", "backup_investigator",
            "INT, ADD FOREIGN KEY (backup_investigator) REFERENCES users(user_id)");
        addColumnIfMissing("cases", "main_analyst",
            "INT, ADD FOREIGN KEY (main_analyst) REFERENCES users(user_id)");
        addColumnIfMissing("cases", "backup_analyst",
            "INT, ADD FOREIGN KEY (backup_analyst) REFERENCES users(user_id)");

        addColumnIfMissing("archived_cases", "backup_investigator", "INT");
        addColumnIfMissing("archived_cases", "main_analyst", "INT");
        addColumnIfMissing("archived_cases", "backup_analyst", "INT");
    } catch (const sql::SQLException& e) {
        std::cout << "Could not update the cases tables. Error message: " << e.what() << std::endl;
        system("pause");
        exit(1);
    }

	// Report management table
}
