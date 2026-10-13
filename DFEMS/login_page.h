#pragma once

// Functionalities
void clearScreen();
void hideCursor();
void showCursor();
void passwordInput(std::string& outPassword);

void mainMenu();
void loginPage();

extern sql::Driver* driver;
extern sql::Connection* con;

void loginPage() {
	clearScreen();
	std::string username;
	std::string password;
	std::cout << ">> Enter username: ";
	std::getline(std::cin, username);
	std::cout << ">> Enter password: ";
	passwordInput(password);
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
			"SELECT roles FROM users WHERE username = ? AND password = ?"));
		stmt->setString(1, username);
		stmt->setString(2, password);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
		if (res->next()) {
			std::string role = res->getString("roles");
			if (role == "Admin") {
				std::cout << "\nWelcome, Admin." << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				clearScreen();
				mainMenu();
			}
		}
		else {
			std::cout << "\nInvalid username or password. Please try again." << std::endl;
			system("pause");
			loginPage(); // Retry login
		}
	}
	catch (const sql::SQLException& e) {
		std::cout << "Error during login. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}
}