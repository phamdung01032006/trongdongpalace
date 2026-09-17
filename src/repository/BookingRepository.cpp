#include "BookingRepository.h"
#include "Exceptions.h"

using namespace std;

BookingRepository::BookingRepository(JsonStorage& storage)
    : storage(storage) {
    load();
}

Booking BookingRepository::fromJson(const json& item) {
    try {
        return Booking(
            item.at("id").get<string>(),
            item.at("eventId").get<string>(),
            item.at("packageId").get<string>(),
            item.value("status", "PENDING")
        );
    } catch (const json::exception& e) {
        throw FileException("Invalid booking data: " + string(e.what()));
    }
}

json BookingRepository::toJson(const Booking& booking) {
    return {
        {"id", booking.getId()},
        {"eventId", booking.getEventId()},
        {"packageId", booking.getPackageId()},
        {"status", booking.getStatus()}
    };
}

int BookingRepository::findIndex(const string& id) const {
    for (size_t i = 0; i < bookings.size(); ++i) {
        if (bookings[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

void BookingRepository::load() {
    bookings.clear();

    const json data = storage.load();

    for (const auto& item : data.at("bookings"))
        bookings.push_back(fromJson(item));
}

void BookingRepository::save() {
    json data = storage.load();

    data["bookings"] = json::array();

    for (const Booking& booking : bookings)
        data["bookings"].push_back(toJson(booking));

    storage.save(data);
}

void BookingRepository::add(const Booking& booking) {
    if (findIndex(booking.getId()) >= 0)
        throw DuplicateException(
            "Booking ID already exists: " + booking.getId()
        );

    bookings.push_back(booking);
    save();
}

void BookingRepository::update(const Booking& booking) {
    const int index = findIndex(booking.getId());

    if (index < 0)
        throw NotFoundException(
            "Booking not found: " + booking.getId()
        );

    bookings[index] = booking;
    save();
}

void BookingRepository::remove(const string& id) {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Booking not found: " + id
        );

    bookings.erase(bookings.begin() + index);
    save();
}

Booking BookingRepository::findById(const string& id) const {
    const int index = findIndex(id);

    if (index < 0)
        throw NotFoundException(
            "Booking not found: " + id
        );

    return bookings[index];
}

vector<Booking> BookingRepository::getAll() const {
    return bookings;
}
