#pragma once

#include "Booking.h"
#include "JsonStorage.h"
#include "Repository.h"

class BookingRepository : public Repository<Booking> {
private:
    JsonStorage& storage;
    vector<Booking> bookings;

    static Booking fromJson(const json& item);
    static json toJson(const Booking& booking);
    int findIndex(const string& id) const;

public:
    explicit BookingRepository(JsonStorage& storage);

    void load() override;
    void save() override;
    void add(const Booking& booking) override;
    void update(const Booking& booking) override;
    void remove(const string& id) override;
    Booking findById(const string& id) const override;
    vector<Booking> getAll() const override;
};
