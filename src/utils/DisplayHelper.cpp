#pragma once
#include <string>
#include "DisplayHelper.h"
#include <iostream>
using namespace std;

void DisplayHelper::showHeader(const string& title) {
    cout<<string(50, '=') <<"\n";
    cout<<title<<"\n";
    cout<<string(50, '=')<<"\n";
}

void DisplayHelper::showError(const string& message) {
    cout<<"[ERROR] "<<message<<"\n";
}

void DisplayHelper::pause() {
    cout << "\n Press Enter to continue...";
    cin.get();
}

void DisplayHelper::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
