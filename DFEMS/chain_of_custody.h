#pragma once

// Forward declarations
void clearScreen();
void showCursor();
void mainMenu();

// Chain of Custody Functions
void custodyMenu();
void addCustodyRecord();
void viewCustodyChain();

extern sql::Driver* driver;
extern sql::Connection* con;

/* Chain of Custody */

void custodyMenu() {
    std::string options = R"(
                =====================================================
                        1. View Chain of Custody
                        2. Add Custody Record
                        3. Back to Main Menu
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
        case 1:
            clearScreen();
            viewCustodyChain();
            break;
        case 2:
            clearScreen();
            addCustodyRecord();
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

void addCustodyRecord() {
    showCursor();

    std::cout << "=======================================================" << std::endl;

    std::cout << ">> Enter Evidence ID: ";
    std::string evidenceIdInput;
    std::getline(std::cin, evidenceIdInput);

    try {
        int evidenceId = std::stoi(evidenceIdInput);

        // Verify evidence exists and show current info
        std::unique_ptr<sql::PreparedStatement> checkStmt(con->prepareStatement(
            "SELECT evidence_name, location FROM evidences WHERE evidence_id = ?"));
        checkStmt->setInt(1, evidenceId);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());

        if (!checkRes->next()) {
            std::cout << "\nEvidence ID not found. Please try again." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::cout << "\nEvidence: " << checkRes->getString("evidence_name") << std::endl;
        std::cout << "Current location: " << checkRes->getString("location") << std::endl;
        std::cout << std::endl;

        std::cout << ">> Enter action (Transferred | Checked Out | Checked In | Analyzed | Returned | Disposed): ";
        std::string action;
        std::getline(std::cin, action);

        std::cout << ">> Enter from User ID (leave blank if N/A): ";
        std::string fromUserInput;
        std::getline(std::cin, fromUserInput);

        std::cout << ">> Enter to User ID: ";
        std::string toUserInput;
        std::getline(std::cin, toUserInput);

        std::cout << ">> Enter new location: ";
        std::string location;
        std::getline(std::cin, location);

        std::cout << ">> Enter notes: ";
        std::string notes;
        std::getline(std::cin, notes);

        std::cout << "=======================================================" << std::endl;

        int toUser = std::stoi(toUserInput);

        if (!fromUserInput.empty()) {
            int fromUser = std::stoi(fromUserInput);
            std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
                "INSERT INTO chain_of_custody (evidence_id, action, from_user, to_user, location, notes) "
                "VALUES (?, ?, ?, ?, ?, ?)"));
            insert->setInt(1, evidenceId);
            insert->setString(2, action);
            insert->setInt(3, fromUser);
            insert->setInt(4, toUser);
            insert->setString(5, location);
            insert->setString(6, notes);
            insert->executeUpdate();
        } else {
            std::unique_ptr<sql::PreparedStatement> insert(con->prepareStatement(
                "INSERT INTO chain_of_custody (evidence_id, action, to_user, location, notes) "
                "VALUES (?, ?, ?, ?, ?)"));
            insert->setInt(1, evidenceId);
            insert->setString(2, action);
            insert->setInt(3, toUser);
            insert->setString(4, location);
            insert->setString(5, notes);
            insert->executeUpdate();
        }

        // Update the evidence's current location
        std::unique_ptr<sql::PreparedStatement> updateLoc(con->prepareStatement(
            "UPDATE evidences SET location = ? WHERE evidence_id = ?"));
        updateLoc->setString(1, location);
        updateLoc->setInt(2, evidenceId);
        updateLoc->executeUpdate();

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nCustody record added successfully!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clearScreen();
        mainMenu();
    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter valid IDs." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error adding custody record: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}

void viewCustodyChain() {
    showCursor();

    std::cout << ">> Enter Evidence ID to view its chain of custody (0 to cancel): ";
    std::string idInput;
    std::getline(std::cin, idInput);

    try {
        int evidenceId = std::stoi(idInput);

        if (evidenceId == 0) {
            clearScreen();
            mainMenu();
            return;
        }

        // Show evidence info
        std::unique_ptr<sql::PreparedStatement> evidenceStmt(con->prepareStatement(
            "SELECT evidence_name, case_id, status, location FROM evidences WHERE evidence_id = ?"));
        evidenceStmt->setInt(1, evidenceId);
        std::unique_ptr<sql::ResultSet> evidenceRes(evidenceStmt->executeQuery());

        if (!evidenceRes->next()) {
            std::cout << "\nEvidence ID not found." << std::endl;
            system("pause");
            clearScreen();
            mainMenu();
            return;
        }

        std::cout << "\n=======================================================" << std::endl;
        std::cout << "Evidence: " << evidenceRes->getString("evidence_name") << std::endl;
        std::cout << "Case ID: " << evidenceRes->getInt("case_id") << std::endl;
        std::cout << "Current Status: " << evidenceRes->getString("status") << std::endl;
        std::cout << "Current Location: " << evidenceRes->getString("location") << std::endl;
        std::cout << "=======================================================" << std::endl;

        // Show full chain of custody
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT c.custody_id, c.action, c.from_user, c.to_user, c.location, c.notes, c.timestamp "
            "FROM chain_of_custody c WHERE c.evidence_id = ? ORDER BY c.timestamp ASC"));
        stmt->setInt(1, evidenceId);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        std::cout << "\nCHAIN OF CUSTODY LOG:" << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;

        int recordNum = 1;
        while (res->next()) {
            std::cout << "\n  #" << recordNum++ << std::endl;
            std::cout << "  Action:    " << res->getString("action") << std::endl;

            if (!res->isNull("from_user")) {
                std::cout << "  From User: " << res->getInt("from_user") << std::endl;
            }

            std::cout << "  To User:   " << res->getInt("to_user") << std::endl;
            std::cout << "  Location:  " << res->getString("location") << std::endl;
            std::cout << "  Notes:     " << res->getString("notes") << std::endl;
            std::cout << "  Timestamp: " << res->getString("timestamp") << std::endl;
            std::cout << "  -------------------------------------------------------" << std::endl;
        }

        if (recordNum == 1) {
            std::cout << "\n  No custody records found for this evidence." << std::endl;
        }

        std::cout << "\nPress any key to go back..." << std::endl;

		if (_getch != NULL) {
			_getch();
			clearScreen();
			mainMenu();
		}

    } catch (const std::invalid_argument&) {
        std::cout << "\nInvalid input. Please enter a valid Evidence ID." << std::endl;
        system("pause");
        clearScreen();
        mainMenu();
    } catch (const sql::SQLException& e) {
        std::cout << "Error viewing custody chain: " << e.what() << std::endl;
        system("pause");
        mainMenu();
    }
}