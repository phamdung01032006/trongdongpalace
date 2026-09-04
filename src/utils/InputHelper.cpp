#include "InputHelper.h"
#include "DisplayHelper.h"
#include <iostream>
#include <limits>
using namespace std;

int InputHelper::readInt(const string& prompt) {
    int value;

    while (true) {
        cout<<prompt;
        cin>>value;

        if(cin.fail()) {
            // Người dùng nhập ký tự không phải số
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            DisplayHelper::showError("Invalid input. Please enter a number.");
            continue;
        }

        // Xóa ký tự Enter còn sót lại trên buffer, tránh lỗi cho lần đọc tiếp theo
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}