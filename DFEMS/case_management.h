#pragma once

#include <cppconn/datatype.h>

// Forward declarations
void clearScreen();
void hideCursor();
void showCursor();
void mainMenu();
void casesMenu();

extern sql::Driver* driver;
extern sql::Connection* con;

// The four people assigned to a case. 0 means nobody is assigned to that slot.
struct CaseTeam {
    int mainInvestigator = 0;
    int backupInvestigator = 0;
    int mainAnalyst = 0;
    int backupAnalyst = 0;
};

bool isValidStatus(const std::string& status) {
    return status == "Open" || status == "Under Investigation" || status == "Closed";
}

// Binds a user ID to a query, or NULL when nobody is assigned (0)
void bindUser(sql::PreparedStatement* stmt, int index, int userId) {
    if (userId == 0) {
        stmt->setNull(index, sql::DataType::INTEGER);
    } else {
        stmt->setInt(index, userId);
    }
}

// Lists every user with the given role and how many active cases they are on
void showStaff(const std::string& heading, const std::string& role) {
    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
        "SELECT u.user_id, u.full_name, u.username, "
        "  (SELECT COUNT(*) FROM cases c "
        "   WHERE c.assigned_to = u.user_id "
        "      OR c.backup_investigator = u.user_id "
        "      OR c.main_analyst = u.user_id "
        "      OR c.backup_analyst = u.user_id) AS active_cases "
        "FROM users u WHERE u.roles = ? ORDER BY u.user_id"));
    stmt->setString(1, role);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

    std::cout << "\n" << heading << std::endl;
    printTable(res.get(),
        { "ID", "Full Name", "Username", "Active Cases" },
        { "user_id", "full_name", "username", "active_cases" });
}

// Asks for one team member and keeps asking until the answer is usable.
//   blank -> returns blankValue (0 = nobody, or the person currently assigned)
//   0     -> returns 0 (nobody)
//   ID    -> must belong to a user with the given role, and must not equal `taken`
int pickStaff(const std::string& label, const std::string& hint, const std::string& role, int blankValue, int taken) {
    while (true) {
        std::cout << ">> Enter " << label << " User ID (" << hint << "): ";
        std::string input;
        std::getline(std::cin, input);
        input = trim(input);

        int userId = blankValue;

        if (!input.empty()) {
            try {
                userId = std::stoi(input);
            } catch (const std::exception&) {
                std::cout << "   Please enter an ID from the list above." << std::endl;
                continue;
            }
        }

        if (userId == 0) {
            return 0;
        }

        if (userId == taken) {
            std::cout << "   Main and backup cannot be the same person. Enter another ID, or 0 for none." << std::endl;
            continue;
        }

        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT user_id FROM users WHERE user_id = ? AND roles = ?"));
        checkStmt->setInt(1, userId);
        checkStmt->setString(2, role);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (checkRes->next()) {
            return userId;
        }

        std::cout << "   That ID is not in the list above." << std::endl;
    }
}

// Shows the available staff and asks for the main and backup of each role.
// keepOnBlank = true  -> blank keeps whoever is in `current` (used when reassigning)
// keepOnBlank = false -> blank leaves the slot empty (used when creating a case)
CaseTeam promptCaseTeam(const CaseTeam& current, bool keepOnBlank) {
    std::string hint = keepOnBlank ? "blank to keep current, 0 for none" : "blank for none";
    CaseTeam team;

    showStaff("Available investigators:", "Investigator");
    team.mainInvestigator = pickStaff("main investigator", hint, "Investigator",
        keepOnBlank ? current.mainInvestigator : 0, 0);
    team.backupInvestigator = pickStaff("backup investigator", hint, "Investigator",
        keepOnBlank ? current.backupInvestigator : 0, team.mainInvestigator);

    showStaff("Available forensic analysts:", "FA");
    team.mainAnalyst = pickStaff("main forensic analyst", hint, "FA",
        keepOnBlank ? current.mainAnalyst : 0, 0);
    team.backupAnalyst = pickStaff("backup forensic analyst", hint, "FA",
        keepOnBlank ? current.backupAnalyst : 0, team.mainAnalyst);

    return team;
}

// Prints all active cases together with the usernames of the assigned team
void printCases() {
    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
        "SELECT c.case_id, c.case_number, c.case_name, c.status, "
        "  COALESCE(mi.username, '-') AS main_investigator, "
        "  COALESCE(bi.username, '-') AS backup_investigator, "
        "  COALESCE(ma.username, '-') AS main_analyst, "
        "  COALESCE(ba.username, '-') AS backup_analyst "
        "FROM cases c "
        "LEFT JOIN users mi ON c.assigned_to = mi.user_id "
        "LEFT JOIN users bi ON c.backup_investigator = bi.user_id "
        "LEFT JOIN users ma ON c.main_analyst = ma.user_id "
        "LEFT JOIN users ba ON c.backup_analyst = ba.user_id "
        "ORDER BY c.case_id"));
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

    printTable(res.get(),
        { "ID", "Case No.", "Case Name", "Status", "Main Inv.", "Backup Inv.", "Main FA", "Backup FA" },
        { "case_id", "case_number", "case_name", "status", "main_investigator", "backup_investigator", "main_analyst", "backup_analyst" });
}

// Admin only: create a case and assign its team
void addCase() {
    showCursor();

    std::cout << "=======================================================" << std::endl;

    std::cout << ">> Enter case number (e.g. CASE-2026-0001): ";
    std::string caseNumber;
    std::getline(std::cin, caseNumber);

    std::cout << ">> Enter case name: ";
    std::string caseName;
    std::getline(std::cin, caseName);

    std::cout << ">> Enter description: ";
    std::string description;
    std::getline(std::cin, description);

    std::cout << ">> Enter status (Open | Under Investigation | Closed, blank for Open): ";
    std::string status;
    std::getline(std::cin, status);

    // Validate the input before touching the database
    caseNumber = trim(caseNumber);
    caseName = trim(caseName);
    description = trim(description);
    status = trim(status);

    if (status.empty()) status = "Open";

    std::string error;
    if (caseNumber.empty())           error = "Case number cannot be blank.";
    else if (caseName.empty())        error = "Case name cannot be blank.";
    else if (!isValidStatus(status))  error = "Status must be Open, Under Investigation or Closed.";

    if (!error.empty()) {
        std::cout << "\n" << error << " Case was not added." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
        return;
    }

    try {
        CaseTeam team = promptCaseTeam(CaseTeam(), false);

        std::cout << "=======================================================" << std::endl;

        std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
            "INSERT INTO cases (case_number, case_name, description, status, "
            "assigned_to, backup_investigator, main_analyst, backup_analyst, created_by) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"));
        insert->setString(1, caseNumber);
        insert->setString(2, caseName);
        insert->setString(3, description);
        insert->setString(4, status);
        bindUser(insert.get(), 5, team.mainInvestigator);
        bindUser(insert.get(), 6, team.backupInvestigator);
        bindUser(insert.get(), 7, team.mainAnalyst);
        bindUser(insert.get(), 8, team.backupAnalyst);
        bindUser(insert.get(), 9, currentUserId);
        insert->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCase has been added successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error adding case: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

// Admin only: assign or change the team of an existing case
void assignCase() {
    try {
        printCases();

        showCursor();

        std::cout << "\n>> Enter the Case ID to assign (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int caseId = std::stoi(idInput);

        if (caseId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if case exists and load its current team
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT case_name, assigned_to, backup_investigator, main_analyst, backup_analyst "
            "FROM cases WHERE case_id = ?"));
        checkStmt->setInt(1, caseId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nCase ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // getInt returns 0 for NULL, which is our "nobody assigned" value
        CaseTeam current;
        current.mainInvestigator = checkRes->getInt("assigned_to");
        current.backupInvestigator = checkRes->getInt("backup_investigator");
        current.mainAnalyst = checkRes->getInt("main_analyst");
        current.backupAnalyst = checkRes->getInt("backup_analyst");

        std::cout << "\nAssigning the team for \"" << checkRes->getString("case_name") << "\"." << std::endl;

        CaseTeam team = promptCaseTeam(current, true);

        std::unique_ptr<sql::PreparedStatement> update(con->prepareStatement(
            "UPDATE cases SET assigned_to = ?, backup_investigator = ?, "
            "main_analyst = ?, backup_analyst = ? WHERE case_id = ?"));
        bindUser(update.get(), 1, team.mainInvestigator);
        bindUser(update.get(), 2, team.backupInvestigator);
        bindUser(update.get(), 3, team.mainAnalyst);
        bindUser(update.get(), 4, team.backupAnalyst);
        update->setInt(5, caseId);
        update->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCase team has been updated successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Case ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error assigning case: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void editCase() {
    try {
        // Display all active cases
        printCases();

        showCursor();

        std::cout << "\n>> Enter the Case ID to edit (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int caseId = std::stoi(idInput);

        if (caseId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if case exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT case_id FROM cases WHERE case_id = ?"));
        checkStmt->setInt(1, caseId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nCase ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Prompt for new values (leave blank to keep current)
        std::cout << "\nLeave blank to keep the current value.\n" << std::endl;

        std::cout << ">> Enter new case number: ";
        std::string caseNumber;
        std::getline(std::cin, caseNumber);

        std::cout << ">> Enter new case name: ";
        std::string caseName;
        std::getline(std::cin, caseName);

        std::cout << ">> Enter new description: ";
        std::string description;
        std::getline(std::cin, description);

        std::cout << ">> Enter new status (Open | Under Investigation | Closed): ";
        std::string status;
        std::getline(std::cin, status);

        // Spaces only count as blank (keep the current value)
        caseNumber = trim(caseNumber);
        caseName = trim(caseName);
        description = trim(description);
        status = trim(status);

        if (!status.empty() && !isValidStatus(status)) {
            std::cout << "\nStatus must be Open, Under Investigation or Closed. Case was not updated." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Build dynamic UPDATE query
        std::string query = "UPDATE cases SET ";
        bool hasField = false;

        if (!caseNumber.empty())  { query += "case_number = ?, ";  hasField = true; }
        if (!caseName.empty())    { query += "case_name = ?, ";    hasField = true; }
        if (!description.empty()) { query += "description = ?, ";  hasField = true; }
        if (!status.empty())      { query += "status = ?, ";       hasField = true; }

        if (!hasField) {
            std::cout << "\nNo fields were changed." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Remove trailing ", "
        query = query.substr(0, query.size() - 2);
        query += " WHERE case_id = ?";

        std::unique_ptr<sql::PreparedStatement> update(con->prepareStatement(query));

        // Bind parameters in order
        int paramIndex = 1;
        if (!caseNumber.empty())    update->setString(paramIndex++, caseNumber);
        if (!caseName.empty())      update->setString(paramIndex++, caseName);
        if (!description.empty())   update->setString(paramIndex++, description);
        if (!status.empty())        update->setString(paramIndex++, status);
        update->setInt(paramIndex, caseId);

        update->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCase has been updated successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error editing case: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

// Admin only: move a case to the archive
void archiveCase() {
    try {
        // Display all active cases
        printCases();

        showCursor();

        std::cout << "\n>> Enter the Case ID to archive (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int caseId = std::stoi(idInput);

        if (caseId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if case exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT case_name FROM cases WHERE case_id = ?"));
        checkStmt->setInt(1, caseId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nCase ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::string caseName = checkRes->getString("case_name");

        // Confirm archival
        std::cout << "\nAre you sure you want to archive \"" << caseName << "\"? (y/n): ";
        char confirm = _getch();
        std::cout << confirm << std::endl;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nArchival cancelled." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Move case to archived_cases table
        std::unique_ptr<sql::PreparedStatement> archiveStmt(con->prepareStatement(
            "INSERT INTO archived_cases (case_id, case_number, case_name, description, status, "
            "assigned_to, backup_investigator, main_analyst, backup_analyst, created_by, created_at, updated_at, archived_at) "
            "SELECT case_id, case_number, case_name, description, status, "
            "assigned_to, backup_investigator, main_analyst, backup_analyst, created_by, created_at, updated_at, NOW() "
            "FROM cases WHERE case_id = ?"));
        archiveStmt->setInt(1, caseId);
        archiveStmt->executeUpdate();

        // Remove from active cases table
        std::unique_ptr<sql::PreparedStatement> delStmt(con->prepareStatement(
            "DELETE FROM cases WHERE case_id = ?"));
        delStmt->setInt(1, caseId);
        delStmt->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCase has been archived successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Case ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error archiving case: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void viewCases() {
    try {
        printCases();

        std::cout << "\nPress any key to go back..." << std::endl;
        _getch();
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error viewing cases: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}
