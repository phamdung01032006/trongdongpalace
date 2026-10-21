#include "Hall.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Hall::Hall(string id, string name, int capacity, int64_t rentalPrice, HallStatus status)
    : id_(id), name_(name), capacity_(capacity), rentalPrice_(rentalPrice), status_(status) {}

string Hall::toText() const {
    // id | ten sanh | suc chua | gia thue | trang thai
    vector<string> fields = {
        cleanField(id_),
        cleanField(name_),
        to_string(capacity_),
        to_string(rentalPrice_),
        toString(status_)
    };
    return joinText(fields, '|');
}

Hall Hall::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 5) {
        throw FileException("Dong halls.txt khong du truong: " + line);
    }

    return Hall(fieldAt(fields, 0), fieldAt(fields, 1), toInt(fieldAt(fields, 2)),
                toInt64(fieldAt(fields, 3)), hallStatusFromString(fieldAt(fields, 4)));
}
