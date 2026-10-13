#pragma once

void clearScreen() {
	system("cls");
}

void hideCursor() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(console, &cursorInfo);
	cursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(console, &cursorInfo);
}

void showCursor() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(console, &cursorInfo);
	cursorInfo.bVisible = TRUE;

	SetConsoleCursorInfo(console, &cursorInfo);
}

void passwordInput(std::string& outPassword) {
	outPassword.clear();
	char ch;
	while (true) {
		ch = _getch();
		if (ch == 13) { // Enter key
			break;
		}
		else if (ch == 8) { // Backspace key
			if (!outPassword.empty()) {
				outPassword.pop_back();
				std::cout << "\b \b"; // Move back, print space, move back again
			}
		}
		else {
			outPassword.push_back(ch);
			std::cout << '*'; // Print asterisk for each character
		}
	}
	std::cout << std::endl; // Move to the next line after password input
}
