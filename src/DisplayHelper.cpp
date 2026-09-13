#include "DisplayHelper.h"

#include <iostream>

using namespace std;

void DisplayHelper::showHeader(const string& title) {
    cout << "================================\n";
    cout << "     " << title << "\n";
    cout << "================================\n";
}

void DisplayHelper::showError(const string& message) {
    cout << "[ERROR] " << message << "\n";
}

void DisplayHelper::showSuccess(const string& message) {
    cout << "[SUCCESS] " << message << "\n";
}

void DisplayHelper::pause() {
    cout << "Press Enter to continue...";
    string ignored;
    getline(cin, ignored);
}
