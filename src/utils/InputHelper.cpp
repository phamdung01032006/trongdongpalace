#include "InputHelper.h"
#include "DisplayHelper.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include "../exceptions/AppException.h"
using namespace std;

int InputHelper::readInt(const string& prompt) {
    while (true) {
        cout<<prompt;
        string line;
        if (!getline(cin, line)) throw runtime_error("Input stream closed.");
        stringstream parser(line);
        int value;
        char extra;
        if (!(parser >> value) || (parser >> extra)) {
            DisplayHelper::showError("Invalid input. Please enter a number.");
            continue;
        }
        return value;
    }
}

string InputHelper::readString(const string& prompt) {
    cout << prompt;
    string value;
    if (!getline(cin, value)) throw runtime_error("Input stream closed.");
    return value;
}
