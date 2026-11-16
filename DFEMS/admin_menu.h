#pragma once

// Functionalities
void clearScreen();
void hideCursor();
void showCursor();
void passwordInput(std::string& outPassword);
std::string sha256(const std::string& input);

// Main Menu Functions
void adminMenu();
void exitMenu();

// User Management Functions
void usersMenu();
void addUser();
void editUser();
void deleteUser();
void viewUsers();

// Case Management Functions (admin side)
void adminCasesMenu();


void adminMenu() {
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
                        1. Manage Users Account
                        2. Manage Cases
                        3. Review Report Logs
                        4. Generate Report
                        5. Exit
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
            case 1:
                clearScreen();
                std::cout << title << std::endl;
                usersMenu();
                break;
            case 2:
                clearScreen();
                std::cout << title << std::endl;
                adminCasesMenu();
                break;
            case 3:
                clearScreen();
                std::cout << title << std::endl;
                reportLogs();
                break;
            case 4:
                clearScreen();
                std::cout << title << std::endl;
                reportGenerate();
                break;
            case 5:
                clearScreen();
                std::cout << title << std::endl;
				exitMenu();
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

/* Case Management (admin) */

void adminCasesMenu() {
    std::string options = R"(
                =====================================================
                        1. View Cases
                        2. Create a Case
                        3. Assign Investigators / Forensic Analysts
                        4. Archive a Case
                        5. Archived Cases
                        6. Back to Main Menu
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
            case 1:
                clearScreen();
                viewCases();
                break;
            case 2:
                clearScreen();
                addCase();
                break;
            case 3:
                clearScreen();
                assignCase();
                break;
            case 4:
                clearScreen();
                archiveCase();
                break;
            case 5:
                clearScreen();
                archivedCasesMenu();
                break;
            case 6:
                clearScreen();
                adminMenu();
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
                        1. View current users
                        2. Add a user
                        3. Edit user's information
                        4. Delete a user
                        5. Back to Main Menu
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
            case 1:
                clearScreen();
                viewUsers();
                break;
            case 2:
                clearScreen();
                addUser();
                break;
            case 3:
                clearScreen();
                editUser();
                break;
            case 4:
                clearScreen();
                deleteUser();
                break;
            case 5:
                clearScreen();
                adminMenu();
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

    char response = _getch();

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
