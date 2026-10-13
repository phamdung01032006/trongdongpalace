#pragma once

#include "storage/json_storage.hpp"

#include <optional>
#include <string>
#include <vector>

using namespace std;

namespace tdp {

enum class Action {
    ManageUsers,
    ManageCustomers,
    ManageRequests,
    ManageVenues,
    LookupHalls,
    ManageBookings,
    ManagePayments,
    ManageOperations
};

class PalaceService {
public:
    explicit PalaceService(JsonStorage& storage);

    OpResult login(const string& username, const string& password);
    // Đăng ký tài khoản mới từ màn hình đăng nhập: không cần đăng nhập trước.
    OpResult registerUser(const string& username, const string& password, const string& confirmPassword,
                          const string& fullName, Role role);
    void logout();
    bool isLoggedIn() const { return !currentUserId_.empty(); }
    const User* currentUser() const;
    bool can(Action action) const;
    OpResult require(Action action) const;

    const Database& db() const { return storage_.db(); }

    OpResult createUser(const string& username, const string& password, const string& fullName, Role role);
    OpResult setUserActive(const string& userId, bool active);
    OpResult changePassword(const string& userId, const string& newPassword);

    OpResult addCustomer(const string& name, const string& phone, const string& email,
                         const string& address, const string& notes);
    OpResult updateCustomer(const string& id, const string& name, const string& phone,
                            const string& email, const string& address, const string& notes);
    vector<Customer> searchCustomers(const string& keyword) const;
    const Customer* findCustomer(const string& id) const;

    OpResult addRequest(const string& customerId, EventType type, const string& title, DateTime start,
                        DateTime end, int guestCount, const string& special);
    OpResult updateRequestStatus(const string& id, RequestStatus status);
    const EventRequest* findRequest(const string& id) const;

    OpResult addHall(const string& name, int capacity);
    OpResult updateHall(const string& id, const string& name, int capacity, bool active);
    OpResult addService(const string& name, const string& unit, int64_t price, const string& category);
    OpResult updateService(const string& id, const string& name, const string& unit, int64_t price,
                           const string& category, bool active);
    const Hall* findHall(const string& id) const;
    const ServiceItem* findService(const string& id) const;
    vector<Hall> findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                         const string& ignoreBookingId) const;
    bool hallAvailable(const string& hallId, DateTime start, DateTime end, const string& ignoreBookingId,
                       string& reason) const;

    // Tao Booking truc tiep tu Yeu cau (theo dac ta 3.1): sao chep thong tin
    // khach/sanh/ngay/ca tu request, request chuyen sang DaChuyenBooking.
    OpResult createBooking(const string& requestId, const string& hallId, DateTime start, DateTime end,
                           int guestCount);
    OpResult confirmBooking(const string& bookingId);
    OpResult cancelBooking(const string& bookingId, const string& reason);
    const Booking* findBooking(const string& id) const;

    OpResult recordPayment(const string& bookingId, int64_t amount, PaymentMethod method, const string& note);
    int64_t paidAmount(const string& bookingId) const;
    vector<Payment> paymentsOf(const string& bookingId) const;

    OpResult updateEventStatus(const string& eventId, EventStatus status);
    OpResult completeEvent(const string& eventId);
    OpResult setEventNotes(const string& eventId, const string& notes);
    const Event* findEvent(const string& id) const;
    const Event* findEventByBooking(const string& bookingId) const;

    vector<string> userIds() const;
    vector<string> customerIds() const;

private:
    JsonStorage& storage_;
    string currentUserId_;

    Database& mut() { return storage_.db(); }
    const Database& data() const { return storage_.db(); }

    template <typename T>
    T* findById(vector<T>& items, const string& id);
    template <typename T>
    const T* findById(const vector<T>& items, const string& id) const;

    OpResult persist(const string& successMessage, const string& id);
    bool uniqueUsername(const string& username, const string& exceptId) const;
    bool uniquePhone(const string& phone, const string& exceptId) const;
};

}  // namespace tdp
