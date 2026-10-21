#pragma once

// Store = lop quan ly du lieu (tuong tu "database" nho).
// No giu tat ca cac vector doi tuong va chiu trach nhiem DOC / GHI file text.
//
// Cau truc thu muc:
//   data/users.txt          - tai khoan
//   data/customers.txt      - ho so khach hang
//   data/halls.txt          - sanh
//   data/services.txt       - dich vu
//   data/requests.txt       - yeu cau su kien
//   data/request_lines.txt  - cac dich vu cua tung yeu cau
//   data/bookings.txt       - booking
//   data/payments.txt       - thanh toan
//   data/events.txt         - su kien
//
// Moi dong la mot ban ghi, cac truong cach nhau boi ky tu '|'.

#include "Booking.h"
#include "Customer.h"
#include "Event.h"
#include "EventRequest.h"
#include "Hall.h"
#include "Payment.h"
#include "Service.h"
#include "User.h"

#include <string>
#include <vector>

using namespace std;

class Database {
public:
    // Cac getter tra ve reference de Store/PalaceService doc/ghi du lieu.
    // Thuoc tinh ben duoi van la private, dung tinh dong goi cua lop.
    vector<User>& get_users() { return users_; }
    const vector<User>& get_users() const { return users_; }

    vector<Customer>& get_customers() { return customers_; }
    const vector<Customer>& get_customers() const { return customers_; }

    vector<EventRequest>& get_requests() { return requests_; }
    const vector<EventRequest>& get_requests() const { return requests_; }

    vector<Hall>& get_halls() { return halls_; }
    const vector<Hall>& get_halls() const { return halls_; }

    vector<Service*>& get_services() { return services_; }
    const vector<Service*>& get_services() const { return services_; }

    vector<Booking>& get_bookings() { return bookings_; }
    const vector<Booking>& get_bookings() const { return bookings_; }

    vector<Payment>& get_payments() { return payments_; }
    const vector<Payment>& get_payments() const { return payments_; }

    vector<Event>& get_events() { return events_; }
    const vector<Event>& get_events() const { return events_; }

private:
    vector<User> users_;
    vector<Customer> customers_;
    vector<EventRequest> requests_;
    vector<Hall> halls_;
    vector<Service*> services_;  // Da hinh: tro toi FoodService hoac PackageService.
    vector<Booking> bookings_;
    vector<Payment> payments_;
    vector<Event> events_;
};

class Store {
public:
    explicit Store(string dataDir = "data");
    ~Store();

    // Khong cho phep copy Store vi ben trong co con tro Service*.
    Store(const Store&) = delete;
    Store& operator=(const Store&) = delete;

    // Doc tat ca file. Neu data/users.txt chua co thi tao du lieu mau.
    void load();

    // Ghi tat ca file. Nem FileException neu khong ghi duoc.
    void save();

    // Them cac sanh/dich vu mau con thieu (khong xoa du lieu da co).
    void seedCatalog();

    Database& get_db() { return db_; }
    const Database& get_db() const { return db_; }

private:
    string dataDir_;
    Database db_;

    string pathOf(const string& fileName) const;
    void clear();

    void loadUsers();
    void loadCustomers();
    void loadHalls();
    void loadServices();
    void loadRequests();
    void loadBookings();
    void loadPayments();
    void loadEvents();

    void saveUsers() const;
    void saveCustomers() const;
    void saveHalls() const;
    void saveServices() const;
    void saveRequests() const;
    void saveBookings() const;
    void savePayments() const;
    void saveEvents() const;

    void seedDefaults();
};
