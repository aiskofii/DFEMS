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

// Admin Menu
#include "admin_menu.h"
#include "user_management.h"

// Others
#include "mysql.h"
#include "functionalities.h"

// MySQL Connection Variables
extern sql::Driver* driver;
extern sql::Connection* con;

// Functions Definitions
void mainMenu();
void mysqlConnect();

// Main Function
int main() {
	mysqlConnect();
	loginPage();

	return 0;
}