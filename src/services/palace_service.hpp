#pragma once

#include "storage/json_storage.hpp"

#include <optional>
#include <string>
#include <vector>

namespace tdp {

enum class Action {
    ManageUsers,
    ManageCustomers,
    ManageRequests,
    ManageVenues,
    LookupHalls,
    ManageQuotations,
    ManageBookings,
    ManageContracts,
    ManagePayments,
    ManageOperations,
    ViewReports,
    ViewAudit
};

struct ReportRow {
    std::vector<std::string> cells;
};

class PalaceService {
public:
    explicit PalaceService(JsonStorage& storage);

    OpResult login(const std::string& username, const std::string& password);
    // Đăng ký tài khoản mới từ màn hình đăng nhập: không cần đăng nhập trước.
    OpResult registerUser(const std::string& username, const std::string& password, const std::string& confirmPassword,
                          const std::string& fullName, Role role);
    void logout();
    bool isLoggedIn() const { return !currentUserId_.empty(); }
    const User* currentUser() const;
    bool can(Action action) const;
    OpResult require(Action action) const;

    const Database& db() const { return storage_.db(); }

    OpResult createUser(const std::string& username, const std::string& password, const std::string& fullName, Role role);
    OpResult setUserActive(const std::string& userId, bool active);
    OpResult changePassword(const std::string& userId, const std::string& newPassword);

    OpResult addCustomer(const std::string& name, const std::string& phone, const std::string& email,
                         const std::string& address, const std::string& notes);
    OpResult updateCustomer(const std::string& id, const std::string& name, const std::string& phone,
                            const std::string& email, const std::string& address, const std::string& notes);
    std::vector<Customer> searchCustomers(const std::string& keyword) const;
    const Customer* findCustomer(const std::string& id) const;

    OpResult addRequest(const std::string& customerId, EventType type, const std::string& title, DateTime start,
                        DateTime end, int guestCount, const std::string& special);
    OpResult updateRequestStatus(const std::string& id, RequestStatus status);
    const EventRequest* findRequest(const std::string& id) const;

    OpResult addCenter(const std::string& name, const std::string& address);
    OpResult updateCenter(const std::string& id, const std::string& name, const std::string& address, bool active);
    OpResult addHall(const std::string& centerId, const std::string& name, int capacity);
    OpResult updateHall(const std::string& id, const std::string& name, int capacity, bool active);
    OpResult addService(const std::string& name, const std::string& unit, int64_t price, const std::string& category);
    OpResult updateService(const std::string& id, const std::string& name, const std::string& unit, int64_t price,
                           const std::string& category, bool active);
    const Hall* findHall(const std::string& id) const;
    const ServiceItem* findService(const std::string& id) const;
    std::vector<Hall> findAvailableHalls(DateTime start, DateTime end, int guestCount,
                                         const std::string& ignoreBookingId) const;
    bool hallAvailable(const std::string& hallId, DateTime start, DateTime end, const std::string& ignoreBookingId,
                       std::string& reason) const;

    OpResult createQuotation(const std::string& requestId, const std::string& note);
    OpResult addQuotationLine(const std::string& quotationId, const std::string& serviceId, int quantity);
    OpResult updateQuotationLine(const std::string& quotationId, int index, int quantity);
    OpResult removeQuotationLine(const std::string& quotationId, int index);
    OpResult setQuotationStatus(const std::string& quotationId, QuotationStatus status);
    const Quotation* findQuotation(const std::string& id) const;

    OpResult createBooking(const std::string& quotationId, const std::string& hallId, DateTime start, DateTime end,
                           int guestCount);
    OpResult confirmBooking(const std::string& bookingId);
    OpResult cancelBooking(const std::string& bookingId, const std::string& reason);
    const Booking* findBooking(const std::string& id) const;

    OpResult createContract(const std::string& bookingId);
    const Contract* findContract(const std::string& id) const;
    const Contract* findContractByBooking(const std::string& bookingId) const;

    OpResult recordPayment(const std::string& contractId, int64_t amount, PaymentMethod method, const std::string& note);
    int64_t paidAmount(const std::string& contractId) const;
    int64_t outstanding(const std::string& contractId) const;
    std::vector<Payment> paymentsOf(const std::string& contractId) const;

    OpResult addTask(const std::string& eventId, const std::string& title, const std::string& assignee, DateTime due);
    OpResult updateTaskStatus(const std::string& taskId, TaskStatus status, const std::string& note);
    OpResult addIncident(const std::string& eventId, const std::string& description);
    OpResult resolveIncident(const std::string& incidentId, const std::string& resolution, IncidentStatus status);
    OpResult updateEventStatus(const std::string& eventId, EventStatus status);
    OpResult completeEvent(const std::string& eventId);
    OpResult setEventNotes(const std::string& eventId, const std::string& notes);
    const Event* findEvent(const std::string& id) const;
    const Event* findEventByBooking(const std::string& bookingId) const;

    std::vector<ReportRow> reportEventsByDateStatus(const std::string& fromText, const std::string& toText,
                                                    const std::string& statusFilter) const;
    std::vector<ReportRow> reportUpcoming() const;
    std::vector<ReportRow> reportHallUsage() const;
    std::vector<ReportRow> reportFinance() const;
    std::vector<ReportRow> reportDebt() const;
    std::vector<ReportRow> reportOps() const;

    std::vector<std::string> userIds() const;
    std::vector<std::string> customerIds() const;

private:
    JsonStorage& storage_;
    std::string currentUserId_;

    Database& mut() { return storage_.db(); }
    const Database& data() const { return storage_.db(); }

    template <typename T>
    T* findById(std::vector<T>& items, const std::string& id);
    template <typename T>
    const T* findById(const std::vector<T>& items, const std::string& id) const;

    OpResult persist(const std::string& successMessage, const std::string& id, const std::string& auditAction,
                     const std::string& auditDetail);
    void audit(const std::string& action, const std::string& detail);
    bool uniqueUsername(const std::string& username, const std::string& exceptId) const;
    bool uniquePhone(const std::string& phone, const std::string& exceptId) const;
};

}  // namespace tdp
