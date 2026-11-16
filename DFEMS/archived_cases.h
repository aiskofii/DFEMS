#pragma once

// Forward declarations
void clearScreen();
void showCursor();
void mainMenu();

void archivedCasesMenu();
void viewArchivedCases();
void restoreCase();

extern sql::Driver* driver;
extern sql::Connection* con;

void archivedCasesMenu() {
    std::string options = R"(
                =====================================================
                        1. View Archived Cases
                        2. Restore a Case
                        3. Back to Main Menu
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
        case 1:
            clearScreen();
            viewArchivedCases();
            break;
        case 2:
            clearScreen();
            restoreCase();
            break;
        case 3:
            clearScreen();
            mainMenu();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void viewArchivedCases() {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT case_id, case_number, case_name, status, assigned_to, archived_at FROM archived_cases"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Case No.", "Case Name", "Status", "Assigned To", "Archived" },
            { "case_id", "case_number", "case_name", "status", "assigned_to", "archived_at" });
        std::cout << "\nPress any key to go back..." << std::endl;

        if (_getch != NULL) {
            _getch();
            clearScreen();
            mainMenu();
        }
        
    } catch (const sql::SQLException& e) {
        std::cout << "Error viewing archived cases: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void restoreCase() {
    try {
        // Display archived cases
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT case_id, case_number, case_name, status, assigned_to, archived_at FROM archived_cases"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Case No.", "Case Name", "Status", "Assigned To", "Archived" },
            { "case_id", "case_number", "case_name", "status", "assigned_to", "archived_at" });

        showCursor();

        std::cout << "\n>> Enter the Case ID to restore (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int caseId = std::stoi(idInput);

        if (caseId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if archived case exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT case_name FROM archived_cases WHERE case_id = ?"));
        checkStmt->setInt(1, caseId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nCase ID not found in archive. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::string caseName = checkRes->getString("case_name");

        // Confirm restore
        std::cout << "\nAre you sure you want to restore \"" << caseName << "\"? (y/n): ";
        char confirm = _getch();
        std::cout << confirm << std::endl;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nRestore cancelled." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Move case back to cases table
        std::unique_ptr<sql::PreparedStatement> restoreStmt(con->prepareStatement(
            "INSERT INTO cases (case_id, case_number, case_name, description, status, "
            "assigned_to, backup_investigator, main_analyst, backup_analyst, created_by, created_at, updated_at) "
            "SELECT case_id, case_number, case_name, description, status, "
            "assigned_to, backup_investigator, main_analyst, backup_analyst, created_by, created_at, updated_at "
            "FROM archived_cases WHERE case_id = ?"));
        restoreStmt->setInt(1, caseId);
        restoreStmt->executeUpdate();

        // Remove from archived_cases
        std::unique_ptr<sql::PreparedStatement> delStmt(con->prepareStatement(
            "DELETE FROM archived_cases WHERE case_id = ?"));
        delStmt->setInt(1, caseId);
        delStmt->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCase has been restored successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Case ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error restoring case: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}