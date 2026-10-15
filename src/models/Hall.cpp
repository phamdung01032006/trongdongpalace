#ifndef TDP_MODELS_HALL_CPP
#define TDP_MODELS_HALL_CPP

#include "models/Enums.cpp"

#include <cstdint>
#include "utils/text_fields.cpp"
#include <string>

using namespace std;

// Sanh co dinh (dac ta 2.6 + C): gia thue rieng + 3 trang thai.
class Hall {
public:
    Hall() = default;

    Hall(const string& id, const string& name, int capacity, int64_t rentalPrice, HallStatus status)
        : id_(id), name_(name), capacity_(capacity), rentalPrice_(rentalPrice), status_(status) {}
    const string& id() const { return id_; }
    const string& name() const { return name_; }
    int capacity() const { return capacity_; }
    int64_t rentalPrice() const { return rentalPrice_; }
    HallStatus status() const { return status_; }
    bool active() const { return status_ == HallStatus::KhaDung; }
    void setName(const string& value) { name_ = value; }
    void setCapacity(int v) { capacity_ = v; }
    void setRentalPrice(int64_t v) { rentalPrice_ = v; }
    void setStatus(HallStatus v) { status_ = v; }
    void setActive(bool v) { status_ = v ? HallStatus::KhaDung : HallStatus::KhongKhaDung; }
    vector<string> toFields() const;
    static Hall fromFields(const vector<string>& f);

private:
    string id_, name_;
    int capacity_ = 0;
    int64_t rentalPrice_ = 0;
    HallStatus status_ = HallStatus::KhaDung;
};

vector<string> Hall::toFields() const {
    vector<string> f = {id_, name_, to_string(capacity_), to_string(rentalPrice_), toString(status_)};
    return f;
}

Hall Hall::fromFields(const vector<string>& f) {
    requireFieldCount(f, 5);
    Hall item;
    item.id_ = f.at(0);
    item.name_ = f.at(1);
    item.capacity_ = textInt(f.at(2));
    item.rentalPrice_ = textMoney(f.at(3));
    item.status_ = hallStatusFromString(f.at(4));
    if (toString(item.status_) != f.at(4)) throw ValidationException("Trang thai/loai khong hop le");
    return item;
}

#endif
