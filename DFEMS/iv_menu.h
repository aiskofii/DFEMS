#pragma once

void ivMenu();

// Case Management Functions
void casesMenu();
void addCase();
void editCase();
void archiveCase();
void viewCases();

// Archived Cases Functions
void archivedCasesMenu();
void viewArchivedCases();
void restoreCase();

// Evidence Management Functions
void evidencesMenu();
void addEvidence();
void editEvidence();
void deleteEvidence();
void viewEvidences();

// Chain of Custody Functions
void custodyMenu();

void ivMenu() {
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
                        1. Manage Cases
                        2. Manage Evidences
                        3. Transfer Evidence Custody
                        4. View Report Logs
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
            casesMenu();
            break;
        case 2:
            clearScreen();
            std::cout << title << std::endl;
            evidencesMenu();
            break;
        case 3:
            clearScreen();
            std::cout << title << std::endl;
            custodyMenu();
            break;
		case 4:
			clearScreen();
			std::cout << title << std::endl;
            reportLogs();
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

/* Case Management */

// Investigators can view and edit cases. Creating, assigning and archiving are admin only.
void casesMenu() {
    std::string options = R"(
                =====================================================
                        1. View Cases
                        2. Edit Case
                        3. Back to Main Menu
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
            editCase();
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

/* Evidence Management */

void evidencesMenu() {
    std::string options = R"(
                =====================================================
                        1. View Evidences
                        2. Add Evidence
                        3. Edit Evidence
                        4. Delete Evidence
                        5. Back to Main Menu
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
        case 1:
            clearScreen();
            viewEvidences();
            break;
        case 2:
            clearScreen();
            addEvidence();
            break;
        case 3:
            clearScreen();
            editEvidence();
            break;
        case 4:
            clearScreen();
            deleteEvidence();
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