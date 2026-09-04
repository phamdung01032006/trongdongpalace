#pragma once
#include <string>
using namespace std;

class DisplayHelper{
public:
    static void showHeader(const string& title);
    static void showError(const string& message);
    static void pause();
    static void clearScreen();
};