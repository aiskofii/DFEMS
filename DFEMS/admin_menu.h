#pragma once


// Functionalities
void clearScreen();
void hideCursor();
void showCursor();
void passwordInput(std::string& outPassword);

// Main Menu Functions
void mainMenu();
void usersMenu();
void casesMenu();
void exitMenu();

// User Management Functions
void addUser();
void editUser();
void deleteUser();
void viewUsers();

void mainMenu() {
	hideCursor();
	const char* title = R"(
				$$$$$$$\  $$$$$$$$\ $$$$$$$$\ $$\      $$\  $$$$$$\  
				$$  __$$\ $$  _____|$$  _____|$$$\    $$$ |$$  __$$\ 
				$$ |  $$ |$$ |      $$ |      $$$$\  $$$$ |$$ /  \__|
				$$ |  $$ |$$$$$\    $$$$$\    $$\$$\$$ $$ |\$$$$$$\  
				$$ |  $$ |$$  __|   $$  __|   $$ \$$$  $$ | \____$$\ 
				$$ |  $$ |$$ |      $$ |      $$ |\$  /$$ |$$\   $$ |
				$$$$$$$  |$$ |      $$$$$$$$\ $$ | \_/ $$ |\$$$$$$  |
				\_______/ \__|      \________|\__|     \__| \______/    
				    Digital Forensics Evidence Management System
		)";

	std::cout << title << std::endl;

	std::string options = R"(
				=====================================================
						1. User Management
						2. Case Management
						3. Evidence Management
						4. Chain of Custody
						5. Audit Logs
						6. Exit
				=====================================================
	)";

	std::cout << options << std::endl;

	while (true) {
		int choice;
		choice = _getch() - '0';
		switch (choice) {
		case 1:
			clearScreen();
			std::cout << title << std::endl;
			usersMenu();
			break;
		case 2:
			clearScreen();
			std::cout << title << std::endl;
			casesMenu();
			break;
		case 6:
			clearScreen();
			std::cout << title << std::endl;
			exitMenu();
			break;
		default:
			std::cout << "Invalid choice. Please try again." << std::endl;
		}
	}
}

/* User Management */

void usersMenu() {
	std::string options = R"(
				=====================================================
						1. Add User
						2. Edit User
						3. Delete User
						4. View Users
						5. Back to Main Menu
				=====================================================
	)";
	std::cout << options << std::endl;
	while (true) {
		int choice;
		choice = _getch() - '0';
		switch (choice) {
		case 1:
			clearScreen();
			addUser();
			break;
		case 2:
			clearScreen();
			editUser();
			break;
		case 3:
			clearScreen();
			deleteUser();
			break;
		case 4:
			clearScreen();
			viewUsers();
			break;
		case 5:
			clearScreen();
			mainMenu();
			break;
		default:
			std::cout << "Invalid choice. Please try again." << std::endl;
		}
	}
}

/* Case Management */

void casesMenu() {
	std::string options = R"(
				=====================================================
						1. Add Case
						2. Edit Case
						3. Delete Case
						4. View Cases
						5. Back to Main Menu
				=====================================================
	)";
	std::cout << options << std::endl;
	while (true) {
		int choice;
		choice = _getch() - '0';
		switch (choice) {
		case 5:
			clearScreen();
			mainMenu();
			break;
		default:
			std::cout << "Invalid choice. Please try again." << std::endl;
		}
	}
}

/* Exit Menu */

void exitMenu() {
	std::string title = R"(
				$$$$$$$\  $$$$$$$$\ $$$$$$$$\ $$\      $$\  $$$$$$\  
				$$  __$$\ $$  _____|$$  _____|$$$\    $$$ |$$  __$$\ 
				$$ |  $$ |$$ |      $$ |      $$$$\  $$$$ |$$ /  \__|
				$$ |  $$ |$$$$$\    $$$$$\    $$\$$\$$ $$ |\$$$$$$\  
				$$ |  $$ |$$  __|   $$  __|   $$ \$$$  $$ | \____$$\ 
				$$ |  $$ |$$ |      $$ |      $$ |\$  /$$ |$$\   $$ |
				$$$$$$$  |$$ |      $$$$$$$$\ $$ | \_/ $$ |\$$$$$$  |
				\_______/ \__|      \________|\__|     \__| \______/    
				    Digital Forensics Evidence Management System
		)";

	std::string options = R"(
				=====================================================

					Are you sure you want to exit? (y/n)

				=====================================================
	)";

	std::string exitMessage = R"(
				=====================================================

					    Exiting the program. Goodbye!

				=====================================================
		)";

	std::cout << options << std::endl;
	char response;
	response = _getch();
	if (response == 'y' || response == 'Y' || response == 13) {
		clearScreen();
		std::cout << title << std::endl;
		std::cout << exitMessage << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		exit(0);
	} else {
		clearScreen();
		mainMenu();
	}
}