#pragma once

void faMenu() {
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
                        1. Record Examination Activities
                        2. Verify Evidence Integrity
                        3. View Report Logs
                        4. Exit
                =====================================================
    )";

    std::cout << options << std::endl;

    while (true) {
        int choice = _getch() - '0';

        switch (choice) {
        case 1:
            clearScreen();
            std::cout << title << std::endl;
            // TODO: record examination activities
            std::cout << "This feature is not implemented yet." << std::endl;
            system("pause");
            clearScreen();
            faMenu();
            break;
        case 2:
            clearScreen();
            std::cout << title << std::endl;
            // TODO: verify evidence integrity
            std::cout << "This feature is not implemented yet." << std::endl;
            system("pause");
            clearScreen();
            faMenu();
            break;
        case 3:
            clearScreen();
            std::cout << title << std::endl;
            reportLogs();
            break;
        case 4:
            clearScreen();
            std::cout << title << std::endl;
            exitMenu();
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}