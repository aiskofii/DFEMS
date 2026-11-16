#pragma once

#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

sql::Driver* driver = nullptr;
sql::Connection* con = nullptr;

// Only these roles have a menu to log in to
bool isValidRole(const std::string& roles) {
    return roles == "Admin" || roles == "FA" || roles == "Investigator";
}

void addUser() {
    showCursor();

    std::cout << "=======================================================" << std::endl;

    std::cout << ">> Enter full name: ";
    std::string fullName;
    std::getline(std::cin, fullName);

    std::cout << ">> Enter email address: ";
    std::string emailAddress;
    std::getline(std::cin, emailAddress);

    std::cout << ">> Enter phone number: ";
    std::string phoneNumber;
    std::getline(std::cin, phoneNumber);

    std::cout << ">> Enter username: ";
    std::string username;
    std::getline(std::cin, username);

    std::cout << ">> Enter password: ";
    std::string password;
    passwordInput(password);

    std::cout << ">> Enter roles (Admin | FA | Investigator): ";
    std::string roles;
    std::getline(std::cin, roles);

    std::cout << "=======================================================" << std::endl;

    // Validate the input before touching the database
    fullName = trim(fullName);
    emailAddress = trim(emailAddress);
    phoneNumber = trim(phoneNumber);
    username = trim(username);
    roles = trim(roles);

    std::string error;
    if (fullName.empty())          error = "Full name cannot be blank.";
    else if (emailAddress.empty()) error = "Email address cannot be blank.";
    else if (username.empty())     error = "Username cannot be blank.";
    else if (password.empty())     error = "Password cannot be blank.";
    else if (!isValidRole(roles))  error = "Roles must be Admin, FA or Investigator.";

    if (!error.empty()) {
        std::cout << "\n" << error << " User was not added." << std::endl;
        system("pause");
        clearScreen();
        adminMenu();
        return;
    }

    // Hash the password before storing
    password = sha256(password);

    try {
        std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
            "INSERT INTO users (full_name, email, phone, username, password, roles) "
            "VALUES (?, ?, ?, ?, ?, ?)"));
        insert->setString(1, fullName);
        insert->setString(2, emailAddress);
        insert->setString(3, phoneNumber);
        insert->setString(4, username);
        insert->setString(5, password);
        insert->setString(6, roles);
        insert->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nUser has been added successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        adminMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error adding user:\n" << e.what() << std::endl;
        adminMenu();
    }
}

void editUser() {
    try {
        // Display all users first so the admin can pick one
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT user_id, full_name, email, phone, username, roles FROM users"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Full Name", "Email", "Phone", "Username", "Roles" },
            { "user_id", "full_name", "email", "phone", "username", "roles" });

        showCursor();

        std::cout << "\n>> Enter the User ID to edit (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int userId = std::stoi(idInput);

        if (userId == 0) {
            clearScreen();
            adminMenu();
            return; //
        }

        // Check if user exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT user_id FROM users WHERE user_id = ?"));
        checkStmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nUser ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            adminMenu();
            return;
        }

        // Prompt for new values (leave blank to keep current)
        std::cout << "\nLeave blank to keep the current value.\n" << std::endl;

        std::cout << ">> Enter new full name: ";
        std::string fullName;
        std::getline(std::cin, fullName);

        std::cout << ">> Enter new email address: ";
        std::string emailAddress;
        std::getline(std::cin, emailAddress);

        std::cout << ">> Enter new phone number: ";
        std::string phoneNumber;
        std::getline(std::cin, phoneNumber);

        std::cout << ">> Enter new username: ";
        std::string username;
        std::getline(std::cin, username);

        std::cout << ">> Enter new password: ";
        std::string password;
        passwordInput(password);

        std::cout << ">> Enter new roles (Admin | FA | Investigator): ";
        std::string roles;
        std::getline(std::cin, roles);

        // Spaces only count as blank (keep the current value)
        fullName = trim(fullName);
        emailAddress = trim(emailAddress);
        phoneNumber = trim(phoneNumber);
        username = trim(username);
        roles = trim(roles);

        if (!roles.empty() && !isValidRole(roles)) {
            std::cout << "\nRoles must be Admin, FA or Investigator. User was not updated." << std::endl;
            system("pause");
            clearScreen();
            adminMenu();
            return;
        }

        // Hash the password if a new one was provided
        if (!password.empty()) password = sha256(password);

        // Build the UPDATE query dynamically based on which fields were provided
        std::string query = "UPDATE users SET ";
        bool hasField = false;

        if (!fullName.empty())    { query += "full_name = ?, "; hasField = true; }
        if (!emailAddress.empty()){ query += "email = ?, ";     hasField = true; }
        if (!phoneNumber.empty()) { query += "phone = ?, ";     hasField = true; }
        if (!username.empty())    { query += "username = ?, ";  hasField = true; }
        if (!password.empty())    { query += "password = ?, ";  hasField = true; }
        if (!roles.empty())       { query += "roles = ?, ";     hasField = true; }

        if (!hasField) {
            std::cout << "\nNo fields were changed." << std::endl;
            system("pause");
            clearScreen();
            adminMenu();
            return;
        }

        // Remove trailing ", "
        query = query.substr(0, query.size() - 2);
        query += " WHERE user_id = ?";

        std::unique_ptr<sql::PreparedStatement> update(con->prepareStatement(query));

        // Bind parameters in order
        int paramIndex = 1;
        if (!fullName.empty())     update->setString(paramIndex++, fullName);
        if (!emailAddress.empty()) update->setString(paramIndex++, emailAddress);
        if (!phoneNumber.empty())  update->setString(paramIndex++, phoneNumber);
        if (!username.empty())     update->setString(paramIndex++, username);
        if (!password.empty())     update->setString(paramIndex++, password);
        if (!roles.empty())        update->setString(paramIndex++, roles);
        update->setInt(paramIndex, userId);

        update->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nUser has been updated successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        adminMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid User ID." << std::endl;
        system("pause");
        clearScreen();
        adminMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error editing user: " << e.what() << std::endl;
        system("pause");
        adminMenu();
    }
}

void deleteUser() {
    try {
        // Display all users first
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT user_id, full_name, email, phone, username, roles FROM users"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Full Name", "Email", "Phone", "Username", "Roles" },
            { "user_id", "full_name", "email", "phone", "username", "roles" });

        showCursor();

        std::cout << "\n>> Enter the User ID to delete (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int userId = std::stoi(idInput);

        if (userId == 0) {
            clearScreen();
            adminMenu();
            return;
        }

        // Check if user exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT full_name FROM users WHERE user_id = ?"));
        checkStmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nUser ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            adminMenu();
            return;
        }

        std::string userName = checkRes->getString("full_name");

        // Confirm deletion
        std::cout << "\nAre you sure you want to delete \"" << userName << "\"? (y/n): ";
        char confirm = _getch();
        std::cout << confirm << std::endl;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nDeletion cancelled." << std::endl;
            system("pause");
            clearScreen();
            adminMenu();
            return;
        }

        // Delete the user
        std::unique_ptr<sql::PreparedStatement> delStmt(con->prepareStatement(
            "DELETE FROM users WHERE user_id = ?"));
        delStmt->setInt(1, userId);
        delStmt->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nUser has been deleted successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        adminMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid User ID." << std::endl;
        system("pause");
        clearScreen();
        adminMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error deleting user: " << e.what() << std::endl;
        system("pause");
        adminMenu();
    }
}

void viewUsers() {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT user_id, full_name, email, phone, username, roles FROM users"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Full Name", "Email", "Phone", "Username", "Roles" },
            { "user_id", "full_name", "email", "phone", "username", "roles" });
        std::cout << "\nPress any key to go back..." << std::endl;
        _getch();
        clearScreen();
        adminMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error viewing users: " << e.what() << std::endl;
        system("pause");
        adminMenu();
    }
}
