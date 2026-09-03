#pragma once
#include <string>
using namespace std;

enum class BookingStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Booking {
private:
    int id;
    string bookingCode;
    int eventId;
    int hallId;
    string startDateTime;
    string endDateTime;
    BookingStatus status;

public:
    Booking(int id,
            const string& bookingCode,
            int eventId,
            int hallId,
            const string& startDateTime,
            const string& endDateTime,
            BookingStatus status)
            : id(id),
            bookingCode(bookingCode),
            eventId(eventId),
            hallId(hallId),
            startDateTime(startDateTime),
            endDateTime(endDateTime),
            status(status) {}
    
    // Getters
    int getId() const { return id; }
    const string& getBookingCode() const { return bookingCode; }
    int getEventId() const { return eventId; }
    int getHallId() const { return hallId; }
    const string& getStartDateTime() const { return startDateTime; }
    const string& getEndDateTime() const { return endDateTime; }
    BookingStatus getStatus() const { return status; }

    // Setters
    void setEventId(int newEventId) { eventId = newEventId; }
    void setHallId(int newHallId) { hallId = newHallId; }
    void setStartDateTime(const string& newStartDateTime) { startDateTime = newStartDateTime; }
    void setEndDateTime(const string& newEndDateTime) { endDateTime = newEndDateTime; }
    void setStatus(BookingStatus newStatus) { status = newStatus; }
};