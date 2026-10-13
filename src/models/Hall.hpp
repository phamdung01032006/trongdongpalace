#pragma once

#include "models/Enums.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace tdp {

// Sanh co dinh (dac ta 2.6 + C): gia thue rieng + 3 trang thai.
class Hall {
public:
    Hall() = default;
    Hall(string id, string name, int capacity, int64_t rentalPrice, HallStatus status);
    const string& id() const { return id_; }
    const string& name() const { return name_; }
    int capacity() const { return capacity_; }
    int64_t rentalPrice() const { return rentalPrice_; }
    HallStatus status() const { return status_; }
    bool active() const { return status_ == HallStatus::KhaDung; }
    void setName(string v) { name_ = move(v); }
    void setCapacity(int v) { capacity_ = v; }
    void setRentalPrice(int64_t v) { rentalPrice_ = v; }
    void setStatus(HallStatus v) { status_ = v; }
    void setActive(bool v) { status_ = v ? HallStatus::KhaDung : HallStatus::KhongKhaDung; }
    nlohmann::json toJson() const;
    static Hall fromJson(const nlohmann::json& j);

private:
    string id_, name_;
    int capacity_ = 0;
    int64_t rentalPrice_ = 0;
    HallStatus status_ = HallStatus::KhaDung;
};

}  // namespace tdp
