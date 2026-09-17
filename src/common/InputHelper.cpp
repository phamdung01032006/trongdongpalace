#include "InputHelper.h"

#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

string InputHelper::readString(const string& prompt, bool allowEmpty) {
    while (true) {
        cout << prompt;
        string value;
        if (!getline(cin, value)) {
            return "";
        }
        if (allowEmpty || !value.empty()) {
            return value;
        }
        cout << "[ERROR] Input cannot be empty.\n";
    }
}

int InputHelper::readInt(const string& prompt, optional<int> minimum, optional<int> maximum) {
    while (true) {
        const string text = readString(prompt, false);
        stringstream stream(text);
        int value;
        char remaining;
        if (!(stream >> value) || (stream >> remaining)) {
            cout << "[ERROR] Please enter a whole number.\n";
            continue;
        }
        if ((minimum && value < *minimum) || (maximum && value > *maximum)) {
            cout << "[ERROR] Number is outside the allowed range.\n";
            continue;
        }
        return value;
    }
}

double InputHelper::readDouble(const string& prompt, optional<double> minimum, optional<double> maximum) {
    while (true) {
        const string text = readString(prompt, false);
        stringstream stream(text);
        double value;
        char remaining;
        if (!(stream >> value) || (stream >> remaining)) {
            cout << "[ERROR] Please enter a number.\n";
            continue;
        }
        if ((minimum && value < *minimum) || (maximum && value > *maximum)) {
            cout << "[ERROR] Number is outside the allowed range.\n";
            continue;
        }
        return value;
    }
}

bool InputHelper::readYesNo(const string& prompt) {
    while (true) {
        const string answer = readString(prompt + " (Y/N): ", false);
        if (answer == "Y" || answer == "y") return true;
        if (answer == "N" || answer == "n") return false;
        cout << "[ERROR] Please enter Y or N.\n";
    }
}
