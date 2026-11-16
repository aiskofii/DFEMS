// Libraries
#include <iostream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <limits>
#include <memory>
#include <thread>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

#include "login_page.h"

// Others
#include "mysql.h"
#include "functionalities.h"

// Module Headers
#include "user_management.h"
#include "case_management.h"
#include "archived_cases.h"
#include "chain_of_custody.h"
#include "evidence_management.h"
#include "verify_integrity.h"
#include "report_management.h"

// Admin Menu (must be last — calls functions from all modules above)
#include "admin_menu.h"
#include "iv_menu.h"
#include "fa_menu.h"

// MySQL Connection Variables
extern sql::Driver* driver;
extern sql::Connection* con;

// Function Declarations
void adminMenu();
void mysqlConnect();

// Main Function
int main() {
    mysqlConnect();
    loginPage();

    return 0;
}
