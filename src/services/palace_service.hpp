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
    ManageEvents
};

// So gio giu cho sanh cho khach dat coc (dac ta B.2: 72 gio).
constexpr int kHoldHours = 72;
// Nguong chiet khau can Quan ly duyet (Form 2).
constexpr int kDiscountApprovalPercent = 10;

struct ServiceLineInput {
    string serviceId;
    int quantity = 0;  // <= 0: tu tinh theo don vi (ban/khach/goi)
};

class PalaceService {
public:
    explicit PalaceService(JsonStorage& storage);

    OpResult login(const string& username, const string& password);
    // Tu dang ky tai khoan Customer: nhap du ho so -> tu tao Customer lien ket.
    OpResult registerUser(const string& username, const string& password, const string& confirmPassword,
                          const string& fullName, const string& phone, const string& email,
                          const string& address);
    void logout();
    bool isLoggedIn() const { return !currentUserId_.empty(); }
    const User* currentUser() const;
    bool can(Action action) const;
    OpResult require(Action action) const;

    const Database& db() const { return storage_.db(); }

    OpResult createUser(const string& username, const string& password, const string& fullName, Role role);
    OpResult updateUserRole(const string& userId, Role role);
    OpResult deleteUser(const string& userId);
    OpResult setUserActive(const string& userId, bool active);
    OpResult changePassword(const string& userId, const string& newPassword);

    OpResult addCustomer(const string& name, const string& phone, const string& email,
                         const string& address, const string& notes);
    OpResult updateCustomer(const string& id, const string& name, const string& phone,
                            const string& email, const string& address, const string& notes);
    OpResult deleteCustomer(const string& id);
    vector<Customer> searchCustomers(const string& keyword) const;
    const Customer* findCustomer(const string& id) const;

    // Form 1: tao yeu cau (khach hang hoac sales nhap ho).
    OpResult addRequest(const string& customerId, EventType type, const string& title,
                        const string& customerName, const string& customerPhone, const string& hallId,
                        DateTime start, DateTime end, int guestCount, const string& notes,
                        const vector<ServiceLineInput>& services);
    // Sales cap nhat toan bo sau trao doi (dac ta B.1).
    OpResult updateRequest(const string& id, EventType type, const string& title,
                           const string& customerName, const string& customerPhone, const string& hallId,
                           DateTime start, DateTime end, int guestCount, const string& notes,
                           const vector<ServiceLineInput>& services);
    OpResult updateRequestStatus(const string& id, RequestStatus status);
    // Khach hang tu huy yeu cau cua minh.
    OpResult cancelOwnRequest(const string& id);
    OpResult deleteRequest(const string& id);
    const EventRequest* findRequest(const string& id) const;
    // Chi phi du kien = tien thue sanh + tong dich vu (dac ta A.2).
    int64_t estimateRequest(const EventRequest& req) const;
    // Lich su 10 yeu cau gan nhat da huy/hoan thanh/chot (dac ta B.1).
    vector<EventRequest> requestHistory(size_t limit = 10) const;
    // So yeu cau moi (Cho) de bao sales (dac ta B.1).
    size_t countNewRequests() const;
    vector<EventRequest> requestsForCustomer(const string& customerId) const;

    OpResult addHall(const string& name, int capacity, int64_t rentalPrice);
    OpResult updateHall(const string& id, const string& name, int capacity, int64_t rentalPrice,
                        HallStatus status);
    OpResult deleteHall(const string& id);
    OpResult addService(const string& code, const string& name, const string& unit, int64_t price,
                        const string& category);
    OpResult updateService(const string& id, const string& code, const string& name, const string& unit,
                           int64_t price, const string& category, bool active);
    OpResult deleteService(const string& id);
    // Quan ly nap bo sung danh muc mau dac ta (khong xoa du lieu hien co).
    OpResult importCatalog();
    const Hall* findHall(const string& id) const;
    const ServiceItem* findService(const string& id) const;
    const ServiceItem* findServiceByCode(const string& code) const;
    vector<string> serviceCategories() const;
    vector<Hall> findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                    const string& ignoreBookingId) const;
    // Loc sanh kha dung theo nhieu tieu chi (tab tra cuu): gia thue toi da 0 = khong gioi han.
    vector<Hall> filterHalls(DateTime start, DateTime end, int guestCount, int64_t maxPrice,
                             const string& ignoreBookingId) const;
    bool hallAvailable(const string& hallId, DateTime start, DateTime end, const string& ignoreBookingId,
                       string& reason) const;
    int maxHallCapacity() const;

    // Dac ta 3.1: nut Tao Booking -> copy thong tin, request DaChot.
    OpResult createBooking(const string& requestId, const string& hallId, DateTime start, DateTime end,
                           int guestCount, int discountPercent, int64_t discountAmount);
    OpResult setBookingStatus(const string& bookingId, BookingStatus status);
    OpResult cancelBooking(const string& bookingId, const string& reason);
    OpResult deleteBooking(const string& bookingId);
    OpResult updateBookingDiscount(const string& bookingId, int discountPercent, int64_t discountAmount);
    OpResult approveDiscount(const string& bookingId);
    const Booking* findBooking(const string& id) const;
    vector<Booking> bookingsForCustomer(const string& customerId) const;
    // Quet het han giu cho (dac ta 3.3): tra ve so booking bi huy.
    int sweepExpiredHolds(DateTime now);
    // Lam moi trang thai su kien theo thoi gian (dac ta D).
    int refreshEventStatuses(DateTime now);

    // Form 3: dien khoan da tu tao (amount 0) hoac tao moi, danh dau DaThanhToan.
    OpResult recordPayment(const string& bookingId, PaymentType type, int64_t amount, PaymentMethod method,
                           DateTime paidAt, const string& note);
    // Sua khoan thu (dien khoan tu tao hoac sua chua). Len DaThanhToan -> keo booking.
    OpResult updatePayment(const string& id, PaymentType type, int64_t amount, PaymentMethod method,
                           DateTime paidAt, const string& note);
    OpResult deletePayment(const string& id);
    int64_t paidAmount(const string& bookingId) const;
    vector<Payment> paymentsOf(const string& bookingId) const;
    const Payment* findPayment(const string& id) const;
    string paymentReceipt(const string& paymentId) const;

    const Event* findEvent(const string& id) const;
    const Event* findEventByBooking(const string& bookingId) const;
    vector<Event> allEvents() const;

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
    // Dung dich vu theo don vi: /ban -> so ban, /khach -> so khach, con lai 1.
    int resolveQuantity(const ServiceItem& svc, int guestCount, int qty) const;
    OpResult buildLines(int guestCount, const vector<ServiceLineInput>& in,
                        vector<RequestServiceLine>& out) const;
    OpResult applyBookingFromRequest(const string& requestId, const string& hallId, DateTime start,
                                     DateTime end, int guestCount, int discountPercent,
                                     int64_t discountAmount, string& bookingId);
    // Booking doi trang thai theo khoan thu + tu tao su kien khi thu coc xong.
    void applyPaymentCompleted(const string& bookingId, PaymentType type);
};

}  // namespace tdp
