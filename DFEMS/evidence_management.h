#pragma once

// Forward declarations
void clearScreen();
void showCursor();
void mainMenu();
void evidencesMenu();

extern sql::Driver* driver;
extern sql::Connection* con;

void addEvidence() {
    showCursor();

    std::cout << "=======================================================" << std::endl;

    std::cout << ">> Enter Case ID linked to the evidence: ";
    std::string caseIdInput;
    std::getline(std::cin, caseIdInput);

    std::cout << ">> Enter evidence name: ";
    std::string evidenceName;
    std::getline(std::cin, evidenceName);

    std::cout << ">> Enter type: ";
    std::string type;
    std::getline(std::cin, type);

    std::cout << ">> Enter description: ";
    std::string description;
    std::getline(std::cin, description);

    std::cout << ">> Enter storage location: ";
    std::string location;
    std::getline(std::cin, location);

    std::cout << ">> Enter collected by User ID: ";
    std::string collectedByInput;
    std::getline(std::cin, collectedByInput);

    std::cout << ">> Enter collection date and time (YYYY-MM-DD HH:MM:SS): ";
    std::string collectedAt;
    std::getline(std::cin, collectedAt);

    std::cout << ">> Enter status (Collected | In Analysis | Analyzed | Archived | Disposed): ";
    std::string status;
    std::getline(std::cin, status);

    std::cout << "=======================================================" << std::endl;

    try {
        int caseId = std::stoi(caseIdInput);
        int collectedBy = std::stoi(collectedByInput);

        // Verify the case exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT case_id FROM cases WHERE case_id = ?"));
        checkStmt->setInt(1, caseId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nCase ID not found. Please create the case first." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
            "INSERT INTO evidences (case_id, evidence_name, type, description, location, collected_by, collected_at, status) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"));
        insert->setInt(1, caseId);
        insert->setString(2, evidenceName);
        insert->setString(3, type);
        insert->setString(4, description);
        insert->setString(5, location);
        insert->setInt(6, collectedBy);
        insert->setString(7, collectedAt);
        insert->setString(8, status);
        insert->executeUpdate();

        // Automatically create the first chain of custody record
        std::unique_ptr<sql::PreparedStatement> custodyStmt(con->prepareStatement(
            "INSERT INTO chain_of_custody (evidence_id, action, to_user, location, notes) "
            "VALUES (LAST_INSERT_ID(), 'Collected', ?, ?, 'Initial collection of evidence')"));
        custodyStmt->setInt(1, collectedBy);
        custodyStmt->setString(2, location);
        custodyStmt->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nEvidence has been added successfully!" << std::endl;
        std::cout << "Chain of custody record created automatically." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter valid IDs." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error adding evidence: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void editEvidence() {
    try {
        // Display all evidences
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT evidence_id, case_id, evidence_name, type, status, location FROM evidences"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Case ID", "Name", "Type", "Status", "Location" },
            { "evidence_id", "case_id", "evidence_name", "type", "status", "location" });

        showCursor();

        std::cout << "\n>> Enter the Evidence ID to edit (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int evidenceId = std::stoi(idInput);

        if (evidenceId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if evidence exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT evidence_id FROM evidences WHERE evidence_id = ?"));
        checkStmt->setInt(1, evidenceId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nEvidence ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Prompt for new values
        std::cout << "\nLeave blank to keep the current value.\n" << std::endl;

        std::cout << ">> Enter new evidence name: ";
        std::string evidenceName;
        std::getline(std::cin, evidenceName);

        std::cout << ">> Enter new type (Physical | Digital | Document | Image): ";
        std::string type;
        std::getline(std::cin, type);

        std::cout << ">> Enter new description: ";
        std::string description;
        std::getline(std::cin, description);

        std::cout << ">> Enter new storage location: ";
        std::string location;
        std::getline(std::cin, location);

        std::cout << ">> Enter new status (Collected | In Analysis | Analyzed | Archived | Disposed): ";
        std::string status;
        std::getline(std::cin, status);

        // Build dynamic UPDATE query
        std::string query = "UPDATE evidences SET ";
        bool hasField = false;

        if (!evidenceName.empty()) { query += "evidence_name = ?, "; hasField = true; }
        if (!type.empty())         { query += "type = ?, ";          hasField = true; }
        if (!description.empty())  { query += "description = ?, ";  hasField = true; }
        if (!location.empty())     { query += "location = ?, ";     hasField = true; }
        if (!status.empty())       { query += "status = ?, ";       hasField = true; }

        if (!hasField) {
            std::cout << "\nNo fields were changed." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Remove trailing ", "
        query = query.substr(0, query.size() - 2);
        query += " WHERE evidence_id = ?";

        std::unique_ptr<sql::PreparedStatement> update(con->prepareStatement(query));

        int paramIndex = 1;
        if (!evidenceName.empty()) update->setString(paramIndex++, evidenceName);
        if (!type.empty())         update->setString(paramIndex++, type);
        if (!description.empty())  update->setString(paramIndex++, description);
        if (!location.empty())     update->setString(paramIndex++, location);
        if (!status.empty())       update->setString(paramIndex++, status);
        update->setInt(paramIndex, evidenceId);

        update->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nEvidence has been updated successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Evidence ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error editing evidence: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void deleteEvidence() {
    try {
        // Display all evidences
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT evidence_id, case_id, evidence_name, type, status FROM evidences"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Case ID", "Name", "Type", "Status" },
            { "evidence_id", "case_id", "evidence_name", "type", "status" });

        showCursor();

        std::cout << "\n>> Enter the Evidence ID to delete (0 to cancel): ";
        std::string idInput;
        std::getline(std::cin, idInput);
        int evidenceId = std::stoi(idInput);

        if (evidenceId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Check if evidence exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT evidence_name FROM evidences WHERE evidence_id = ?"));
        checkStmt->setInt(1, evidenceId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nEvidence ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::string evidenceName = checkRes->getString("evidence_name");

        // Confirm deletion
        std::cout << "\nAre you sure you want to delete \"" << evidenceName << "\"? (y/n): ";
        char confirm = _getch();
        std::cout << confirm << std::endl;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nDeletion cancelled." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        // Delete chain of custody records first (foreign key dependency)
        std::unique_ptr<sql::PreparedStatement> delCustody(con->prepareStatement(
            "DELETE FROM chain_of_custody WHERE evidence_id = ?"));
        delCustody->setInt(1, evidenceId);
        delCustody->executeUpdate();

        // Delete the evidence
        std::unique_ptr<sql::PreparedStatement> delStmt(con->prepareStatement(
            "DELETE FROM evidences WHERE evidence_id = ?"));
        delStmt->setInt(1, evidenceId);
        delStmt->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nEvidence and its custody records have been deleted successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Evidence ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error deleting evidence: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void viewEvidences() {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT e.evidence_id, e.case_id, c.case_number, e.evidence_name, e.type, "
            "e.description, e.location, e.collected_by, e.collected_at, e.status "
            "FROM evidences e JOIN cases c ON e.case_id = c.case_id"));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        printTable(res.get(),
            { "ID", "Case No.", "Name", "Type", "Location", "Status" },
            { "evidence_id", "case_number", "evidence_name", "type", "location", "status" });
        std::cout << "\nPress any key to go back..." << std::endl;
        _getch();
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error viewing evidences: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}