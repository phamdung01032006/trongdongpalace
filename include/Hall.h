#pragma once

// Sanh tiec cua trung tam. Moi sanh co suc chua va gia thue rieng.

#include "Enums.h"

#include <cstdint>
#include <string>

using namespace std;

class Hall {
public:
    Hall(string id = "", string name = "", int capacity = 0, int64_t rentalPrice = 0,
         HallStatus status = HallStatus::KhaDung);

    const string& get_id() const { return id_; }
    const string& get_name() const { return name_; }
    int get_capacity() const { return capacity_; }
    int64_t get_rentalPrice() const { return rentalPrice_; }
    HallStatus get_status() const { return status_; }

    void set_name(const string& value) { name_ = value; }
    void set_capacity(int value) { capacity_ = value; }
    void set_rentalPrice(int64_t value) { rentalPrice_ = value; }
    void set_status(HallStatus value) { status_ = value; }

    bool is_usable() const { return status_ == HallStatus::KhaDung; }

    string toText() const;
    static Hall fromText(const string& line);

private:
    string id_;
    string name_;
    int capacity_ = 0;
    int64_t rentalPrice_ = 0;
    HallStatus status_ = HallStatus::KhaDung;
};
