#pragma once

// PalaceService = lop xu ly nghiep vu (business logic).
// Giao dien console chi goi cac ham o day, KHONG tu sua du lieu.
// Nho vay, tat ca quy tac (phan quyen, trung lich, han giu cho, chiet khau...)
// chi nam o mot noi duy nhat, de kiem tra va giai thich khi bao ve.

#include "Store.h"
#include "Utils.h"

#include <optional>
#include <string>
#include <vector>

using namespace std;

// Cac nhom quyen trong he thong.
enum class Action {
    ManageUsers,
    ManageCustomers,
    ManageRequests,
    ManageVenues,
    LookupHalls,
    ManageBookings,
    ManagePayments,
    ManageEvents
};

// Du lieu giao dien nhap vao cho mot dong dich vu.
struct ServiceLineInput {
    string serviceId;
    int quantity = 0;  // 0 = tu dong tinh theo don vi.
};

class PalaceService {
public:
    // Du lieu dung chung cho moi doi tuong PalaceService: khai bao static.
    static constexpr int kHoldHours = 72;                // Giu sanh 72 gio cho khach dat coc.
    static constexpr int kDiscountApprovalPercent = 10;  // Chiet khau > 10% can Quan ly duyet.

    explicit PalaceService(Store& store);

    // ---------- Dang nhap / phan quyen ----------
    OpResult login(const string& username, const string& password);
    void logout();
    bool isLoggedIn() const { return !currentUserId_.empty(); }
    const User* currentUser() const;
    bool can(Action action) const;
    void require(Action action) const;
    OpResult registerCustomer(const string& username, const string& password,
                              const string& confirmPassword, const string& fullName,
                              const string& phone, const string& email, const string& address);

    // ---------- Quan ly tai khoan ----------
    OpResult createUser(const string& username, const string& password,
                        const string& fullName, Role role);
    OpResult updateUserRole(const string& userId, Role role);
    OpResult deleteUser(const string& userId);
    OpResult setUserActive(const string& userId, bool active);
    OpResult changePassword(const string& userId, const string& newPassword);

    // ---------- Khach hang ----------
    OpResult addCustomer(const string& name, const string& phone, const string& email,
                         const string& address, const string& notes);
    OpResult updateCustomer(const string& id, const string& name, const string& phone,
                            const string& email, const string& address, const string& notes);
    OpResult deleteCustomer(const string& id);
    vector<Customer> searchCustomers(const string& keyword) const;
    const Customer* findCustomer(const string& id) const;

    // ---------- Yeu cau su kien ----------
    OpResult addRequest(const string& customerId, EventType type, const string& title,
                        const string& customerName, const string& customerPhone,
                        const string& hallId, DateTime start, DateTime end, int guestCount,
                        const string& notes, const vector<ServiceLineInput>& services);
    OpResult updateRequest(const string& id, EventType type, const string& title,
                           const string& customerName, const string& customerPhone,
                           const string& hallId, DateTime start, DateTime end, int guestCount,
                           const string& notes, const vector<ServiceLineInput>& services);
    OpResult updateRequestStatus(const string& id, RequestStatus status);
    OpResult cancelOwnRequest(const string& id);
    OpResult deleteRequest(const string& id);
    const EventRequest* findRequest(const string& id) const;
    int64_t estimateRequest(const EventRequest& request) const;
    vector<EventRequest> requestHistory(size_t limit = 10) const;
    size_t countNewRequests() const;
    vector<EventRequest> requestsForCustomer(const string& customerId) const;

    // ---------- Sanh & dich vu ----------
    OpResult addHall(const string& name, int capacity, int64_t rentalPrice);
    OpResult updateHall(const string& id, const string& name, int capacity,
                        int64_t rentalPrice, HallStatus status);
    OpResult deleteHall(const string& id);
    OpResult addService(const string& code, const string& name, const string& unit,
                        int64_t price, const string& category, bool isFood);
    OpResult updateService(const string& id, const string& code, const string& name,
                           const string& unit, int64_t price, const string& category,
                           bool active);
    OpResult deleteService(const string& id);
    OpResult seedCatalog();

    const Hall* findHall(const string& id) const;
    // Tim theo id (SVC-0001) hoac theo ma dich vu (FOD01-P).
    const Service* findService(const string& idOrCode) const;

    vector<Hall> findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                    const string& ignoreBookingId = "") const;
    vector<Hall> filterHalls(DateTime start, DateTime end, int guestCount, int64_t maxPrice,
                             const string& ignoreBookingId = "") const;
    bool hallAvailable(const string& hallId, DateTime start, DateTime end,
                       const string& ignoreBookingId, string& reason) const;
    int maxHallCapacity() const;

    // ---------- Booking ----------
    OpResult createBooking(const string& requestId, const string& hallId, DateTime start,
                           DateTime end, int guestCount, int discountPercent,
                           int64_t discountAmount);
    OpResult cancelBooking(const string& bookingId, const string& reason);
    OpResult deleteBooking(const string& bookingId);
    OpResult updateBookingDiscount(const string& bookingId, int discountPercent,
                                   int64_t discountAmount);
    OpResult approveDiscount(const string& bookingId);
    const Booking* findBooking(const string& id) const;
    vector<Booking> bookingsForCustomer(const string& customerId) const;

    // ---------- Thanh toan ----------
    OpResult recordPayment(const string& bookingId, PaymentType type, int64_t amount,
                           PaymentMethod method, DateTime paidAt, const string& note);
    OpResult updatePayment(const string& id, PaymentType type, int64_t amount,
                           PaymentMethod method, DateTime paidAt, const string& note);
    OpResult deletePayment(const string& id);
    int64_t paidAmount(const string& bookingId) const;
    vector<Payment> paymentsOf(const string& bookingId) const;
    string paymentReceipt(const string& paymentId) const;

    // ---------- Su kien ----------
    vector<Event> allEvents() const;
    const Event* findEventByBooking(const string& bookingId) const;

    // ---------- Danh sach phuc vu giao dien ----------
    vector<User> allUsers() const;
    vector<EventRequest> allRequests() const;
    vector<Booking> allBookings() const;
    vector<Payment> allPayments() const;
    vector<Hall> allHalls() const;
    vector<const Service*> allServices() const;

    // Quet booking qua han + cap nhat trang thai su kien, luu file neu co thay doi.
    void refreshSystem();

private:
    Store& store_;
    string currentUserId_;

    Database& get_db() { return store_.get_db(); }
    const Database& get_db() const { return store_.get_db(); }

    OpResult persist(const string& successMessage, const string& id);
    bool uniqueUsername(const string& username, const string& exceptId) const;
    bool uniquePhone(const string& phone, const string& exceptId) const;

    vector<string> userIds() const;
    vector<string> customerIds() const;
    vector<string> requestIds() const;
    vector<string> bookingIds() const;
    vector<string> paymentIds() const;
    vector<string> eventIds() const;
    Service* findServiceMutable(const string& idOrCode);

    void buildLines(int guestCount, const vector<ServiceLineInput>& input,
                    vector<RequestLine>& output) const;
    OpResult applyBookingFromRequest(const string& requestId, const string& hallId,
                                     DateTime start, DateTime end, int guestCount,
                                     int discountPercent, int64_t discountAmount,
                                     string& bookingId);
    void applyPaymentCompleted(const string& bookingId, PaymentType type);

    int sweepExpiredHolds(DateTime now);
    int refreshEventStatuses(DateTime now);
};
