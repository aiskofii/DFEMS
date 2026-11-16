#pragma once

// Functionalities
void clearScreen();
void hideCursor();
void showCursor();
void passwordInput(std::string& outPassword);
std::string sha256(const std::string& input);

void adminMenu();
void ivMenu();
void faMenu();
void mainMenu();
void loginPage();

extern sql::Driver* driver;
extern sql::Connection* con;

// Role of the logged-in user (Admin | FA | Investigator), set by loginPage()
std::string currentRole;

// user_id of the logged-in user, set by loginPage()
int currentUserId = 0;

// Opens the main menu that belongs to the logged-in user's role
void mainMenu() {
    if (currentRole == "Admin") {
        adminMenu();
    } else if (currentRole == "Investigator") {
        ivMenu();
    } else if (currentRole == "FA") {
        faMenu();
    } else {
        loginPage();
    }
}

void loginPage() {
    clearScreen();

    std::string username;
    std::string password;

    std::cout << ">> Enter username: ";
    std::getline(std::cin, username);

    std::cout << ">> Enter password: ";
    passwordInput(password);

    // Hash the input so it matches the stored hash
    password = sha256(password);

    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT user_id, roles FROM users WHERE username = ? AND password = ?"));
        stmt->setString(1, username);
        stmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if (res->next()) {
            std::string role = res->getString("roles");

            if (role == "Admin" || role == "Investigator" || role == "FA") {
                currentRole = role;
                currentUserId = res->getInt("user_id");
                std::cout << "\nWelcome, " << role << "." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                clearScreen();
                mainMenu();
            } else {
                std::cout << "\nThis account has an unknown role: " << role << std::endl;
                system("pause");
                loginPage();
            }
        } else {
            std::cout << "\nInvalid username or password. Please try again." << std::endl;
            system("pause");
            loginPage();
        }
    } catch (const sql::SQLException& e) {
        std::cout << "Error during login. Error message: " << e.what() << std::endl;
        system("pause");
        exit(1);
    }
}
