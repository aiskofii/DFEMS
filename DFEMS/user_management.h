#pragma once

#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>


sql::Driver* driver = nullptr;
sql::Connection* con = nullptr;


void addUser() {
	showCursor();
	// Discard any leftover input (e.g. a newline from previous >> reads)
	std::cout << "=======================================================" << std::endl;
	std::cout << ">> Enter full name: ";
	std::string FullName;
	std::getline(std::cin, FullName);

	std::cout << ">> Enter email address: ";
	std::string EmailAddress;
	std::getline(std::cin, EmailAddress);

	std::cout << ">> Enter phone number: ";
	std::string PhoneNumber;
	std::getline(std::cin, PhoneNumber);

	std::cout << ">> Enter username: ";
	std::string Username;
	std::getline(std::cin, Username);

	std::cout << ">> Enter password: ";
	std::string password;
	passwordInput(password);

	std::cout << ">> Enter roles (1. Admin | 2. FA | 3. Investigator): ";
	std::string Roles;
	std::getline(std::cin, Roles);
	std::cout << "=======================================================" << std::endl;

	try {
		std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
			"INSERT INTO users (full_name, email, phone, username, password, roles) VALUES (?, ?, ?, ?, ?, ?)"));
		insert->setString(1, FullName);
		insert->setString(2, EmailAddress);
		insert->setString(3, PhoneNumber);
		insert->setString(4, Username);
		insert->setString(5, password);
		insert->setString(6, Roles);
		insert->executeUpdate();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "\nUser has been added successfully!" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		clearScreen();
		mainMenu();
	
	}
	catch (const sql::SQLException& e) {
		std::cout << "Error adding user: " << e.what() << std::endl;
		mainMenu();
	}

}

void editUser() {
	std::cout << "Hi, this is the editUser function." << std::endl;
}

void deleteUser() {
	std::cout << "Hi, this is the deleteUser function." << std::endl;
}

void viewUsers() {
	std::cout << "Hi, this is the viewUsers function." << std::endl;
}