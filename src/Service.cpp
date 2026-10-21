#include "Service.h"

#include "Utils.h"

#include <stdexcept>

using namespace std;

Service::Service(string id, string code, string name, string category, string unit,
                 int64_t unitPrice, bool active)
    : id_(id),
      code_(code),
      name_(name),
      category_(category),
      unit_(unit),
      unitPrice_(unitPrice),
      active_(active) {}

string Service::toText() const {
    // type | id | ma | ten | nhom | don vi | don gia | dang hoat dong
    vector<string> fields = {
        typeCode(),
        cleanField(id_),
        cleanField(code_),
        cleanField(name_),
        cleanField(category_),
        cleanField(unit_),
        to_string(unitPrice_),
        boolText(active_)
    };
    return joinText(fields, '|');
}

Service* Service::fromText(const string& line) {
    vector<string> fields = splitText(line, '|');
    if (fields.size() < 8) {
        throw FileException("Dong services.txt khong du truong: " + line);
    }

    string type = fieldAt(fields, 0);
    string id = fieldAt(fields, 1);
    string code = fieldAt(fields, 2);
    string name = fieldAt(fields, 3);
    string category = fieldAt(fields, 4);
    string unit = fieldAt(fields, 5);
    int64_t price = toInt64(fieldAt(fields, 6));
    bool active = toBool(fieldAt(fields, 7), true);

    if (type == "FOOD") {
        return new FoodService(id, code, name, category, unit, price, active);
    }
    return new PackageService(id, code, name, category, unit, price, active);
}

FoodService::FoodService(string id, string code, string name, string category, string unit,
                         int64_t unitPrice, bool active)
    : Service(id, code, name, category, unit, unitPrice, active) {}

int FoodService::quantityFor(int guestCount, int enteredQty) const {
    if (enteredQty > 0) return enteredQty;

    // Don vi "/bàn": 10 khach / ban.
    if (unit_.find("bàn") != string::npos || unit_.find("ban") != string::npos) {
        return (guestCount + 9) / 10;
    }
    // Don vi "/khách": moi khach mot phan.
    if (unit_.find("khách") != string::npos || unit_.find("khach") != string::npos) {
        return guestCount;
    }
    return 1;
}

PackageService::PackageService(string id, string code, string name, string category,
                               string unit, int64_t unitPrice, bool active)
    : Service(id, code, name, category, unit, unitPrice, active) {}

int PackageService::quantityFor(int guestCount, int enteredQty) const {
    if (enteredQty > 0) return enteredQty;

    // Mot so goi tinh theo nguoi (MC, le tan...).
    if (unit_.find("người") != string::npos || unit_.find("nguoi") != string::npos ||
        unit_.find("khách") != string::npos || unit_.find("khach") != string::npos) {
        return guestCount;
    }
    return 1;
}
