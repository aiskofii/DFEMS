#pragma once

#include <bcrypt.h>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <cppconn/resultset.h>
#pragma comment(lib, "bcrypt.lib")

// SHA-256 hash function using Windows CryptoAPI
std::string sha256(const std::string& input) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE hash[32];  // SHA-256 produces 32 bytes
    DWORD hashSize = 32;

    BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
    BCryptHashData(hHash, (BYTE*)input.c_str(), (ULONG)input.size(), 0);
    BCryptFinishHash(hHash, hash, hashSize, 0);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    // Convert raw bytes to a 64-character hex string
    std::string result;
    char hex[3];
    for (int i = 0; i < 32; i++) {
        sprintf_s(hex, "%02x", hash[i]);
        result += hex;
    }
    return result;
}

// Removes spaces and tabs from both ends, so "   " counts as blank
std::string trim(const std::string& text) {
    size_t first = text.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    size_t last = text.find_last_not_of(" \t");
    return text.substr(first, last - first + 1);
}

// Prints a query result as an aligned table. Every column is as wide as its
// longest value, so the " | " separators line up.
//   headers -> the titles shown on screen
//   columns -> the matching column names in the query
void printTable(sql::ResultSet* res, const std::vector<std::string>& headers, const std::vector<std::string>& columns) {
    std::vector<std::vector<std::string>> rows;
    std::vector<size_t> widths;

    for (const std::string& header : headers) {
        widths.push_back(header.size());
    }

    // Read every row first, because the widths depend on all of the values
    while (res->next()) {
        std::vector<std::string> row;

        for (size_t i = 0; i < columns.size(); i++) {
            std::string value = res->getString(columns[i]);
            if (value.empty()) value = "-";

            widths[i] = (std::max)(widths[i], value.size());
            row.push_back(value);
        }

        rows.push_back(row);
    }

    size_t totalWidth = 3 * (widths.size() - 1);
    for (size_t width : widths) {
        totalWidth += width;
    }

    std::string line(totalWidth, '=');

    // Prints one row, padding every cell except the last to its column width
    auto printRow = [&widths](const std::vector<std::string>& row) {
        for (size_t i = 0; i < row.size(); i++) {
            if (i + 1 < row.size()) {
                std::cout << std::left << std::setw(widths[i]) << row[i] << " | ";
            } else {
                std::cout << row[i] << std::endl;
            }
        }
    };

    std::cout << line << std::endl;
    printRow(headers);
    std::cout << line << std::endl;

    for (const std::vector<std::string>& row : rows) {
        printRow(row);
    }

    if (rows.empty()) {
        std::cout << "(no records)" << std::endl;
    }

    std::cout << line << std::endl;
}

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

        if (ch == 13) {
            // Enter key
            break;
        } else if (ch == 8) {
            // Backspace key
            if (!outPassword.empty()) {
                outPassword.pop_back();
                std::cout << "\b \b";
            }
        } else {
            outPassword.push_back(ch);
            std::cout << '*';
        }
    }

    std::cout << std::endl;
}
