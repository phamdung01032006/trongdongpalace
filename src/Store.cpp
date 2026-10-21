#include "Store.h"

#include "Utils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace {


void writeTextFile(const string& path, const string& header, const vector<string>& lines) {
    ofstream output;
    output.open(path, ios::trunc);
    if (!output.is_open()) throw FileException("Không mở được file để ghi: " + path);

    output << header << '\n';
    for (const string& line : lines) output << line << '\n';

    output.flush();
    if (!output) {
        output.close();
        throw FileException("Ghi file thất bại: " + path);
    }
    output.close();
    if (output.fail()) throw FileException("Đóng file sau khi ghi thất bại: " + path);
}

vector<string> readTextFile(const string& path) {
    ifstream input;
    input.open(path);
    if (!input.is_open()) throw FileException("Không mở được file để đọc: " + path);

    vector<string> lines;
    string line;
    while (getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        lines.push_back(line);
    }
    input.close();
    if (input.fail() && !input.eof()) throw FileException("Đọc file thất bại: " + path);
    return lines;
}

}  // namespace

Store::Store(string dataDir) : dataDir_(dataDir) {}

Store::~Store() { clear(); }

void Store::clear() {
    for (Service* service : db_.get_services()) delete service;
    db_ = Database{};
}

string Store::pathOf(const string& fileName) const {
    return (filesystem::path(dataDir_) / fileName).string();
}

void Store::load() {
    clear();
    filesystem::create_directories(dataDir_);

    if (!filesystem::exists(pathOf("users.txt"))) {
        seedDefaults();
        save();
        return;
    }

    loadUsers();
    loadCustomers();
    loadHalls();
    loadServices();
    loadRequests();
    loadBookings();
    loadPayments();
    loadEvents();
}

void Store::save() {
    filesystem::create_directories(dataDir_);
    saveUsers();
    saveCustomers();
    saveHalls();
    saveServices();
    saveRequests();
    saveBookings();
    savePayments();
    saveEvents();
}

void Store::loadUsers() {
    if (!filesystem::exists(pathOf("users.txt"))) return;
    for (const string& line : readTextFile(pathOf("users.txt"))) {
        db_.get_users().push_back(User::fromText(line));
    }
}

void Store::loadCustomers() {
    if (!filesystem::exists(pathOf("customers.txt"))) return;
    for (const string& line : readTextFile(pathOf("customers.txt"))) {
        db_.get_customers().push_back(Customer::fromText(line));
    }
}

void Store::loadHalls() {
    if (!filesystem::exists(pathOf("halls.txt"))) return;
    for (const string& line : readTextFile(pathOf("halls.txt"))) {
        db_.get_halls().push_back(Hall::fromText(line));
    }
}

void Store::loadServices() {
    if (!filesystem::exists(pathOf("services.txt"))) return;
    for (const string& line : readTextFile(pathOf("services.txt"))) {
        db_.get_services().push_back(Service::fromText(line));
    }
}

void Store::loadRequests() {
    if (!filesystem::exists(pathOf("requests.txt"))) return;

    for (const string& line : readTextFile(pathOf("requests.txt"))) {
        db_.get_requests().push_back(EventRequest::fromText(line));
    }

    if (!filesystem::exists(pathOf("request_lines.txt"))) return;

    for (const string& line : readTextFile(pathOf("request_lines.txt"))) {
        vector<string> fields = splitText(line, '|');
        if (fields.size() < 6) continue;

        string requestId = fields[0];
        // Noi 5 truong con lai thanh mot dong cua RequestLine.
        vector<string> lineFields(fields.begin() + 1, fields.end());
        for (EventRequest& request : db_.get_requests()) {
            if (request.get_id() == requestId) {
                request.get_lines().push_back(RequestLine::fromText(joinText(lineFields, '|')));
                break;
            }
        }
    }
}

void Store::loadBookings() {
    if (!filesystem::exists(pathOf("bookings.txt"))) return;
    for (const string& line : readTextFile(pathOf("bookings.txt"))) {
        db_.get_bookings().push_back(Booking::fromText(line));
    }
}

void Store::loadPayments() {
    if (!filesystem::exists(pathOf("payments.txt"))) return;
    for (const string& line : readTextFile(pathOf("payments.txt"))) {
        db_.get_payments().push_back(Payment::fromText(line));
    }
}

void Store::loadEvents() {
    if (!filesystem::exists(pathOf("events.txt"))) return;
    for (const string& line : readTextFile(pathOf("events.txt"))) {
        db_.get_events().push_back(Event::fromText(line));
    }
}

void Store::saveUsers() const {
    vector<string> lines;
    for (const User& user : db_.get_users()) lines.push_back(user.toText());
    writeTextFile(pathOf("users.txt"),
                  "# id|username|password|hoTen|sdt|email|diaChi|vaiTro|customerId|active",
                  lines);
}

void Store::saveCustomers() const {
    vector<string> lines;
    for (const Customer& customer : db_.get_customers()) lines.push_back(customer.toText());
    writeTextFile(pathOf("customers.txt"),
                  "# id|hoTen|sdt|email|diaChi|ghiChu|createdAt", lines);
}

void Store::saveHalls() const {
    vector<string> lines;
    for (const Hall& hall : db_.get_halls()) lines.push_back(hall.toText());
    writeTextFile(pathOf("halls.txt"), "# id|ten|sucChua|giaThue|trangThai", lines);
}

void Store::saveServices() const {
    vector<string> lines;
    for (const Service* service : db_.get_services()) lines.push_back(service->toText());
    writeTextFile(pathOf("services.txt"),
                  "# type|id|ma|ten|nhom|donVi|donGia|active", lines);
}

void Store::saveRequests() const {
    vector<string> requestLines;
    for (const EventRequest& request : db_.get_requests()) requestLines.push_back(request.toText());
    writeTextFile(pathOf("requests.txt"),
                  "# id|customerId|loai|ten|tenKhach|sdt|hallId|batDau|ketThuc|soKhach|ghiChu|trangThai|createdAt",
                  requestLines);

    vector<string> serviceLines;
    for (const EventRequest& request : db_.get_requests()) {
        for (const RequestLine& line : request.get_lines()) {
            serviceLines.push_back(request.get_id() + "|" + line.toText());
        }
    }
    writeTextFile(pathOf("request_lines.txt"),
                  "# requestId|serviceId|ten|donVi|soLuong|donGia", serviceLines);
}

void Store::saveBookings() const {
    vector<string> lines;
    for (const Booking& booking : db_.get_bookings()) lines.push_back(booking.toText());
    writeTextFile(pathOf("bookings.txt"),
                  "# id|requestId|customerId|hallId|batDau|ketThuc|soKhach|tongDuKien|chietKhauPhanTram|chietKhauTien|canDuyet|daDuyet|trangThai|createdAt|holdUntil|sales|cancelReason",
                  lines);
}

void Store::savePayments() const {
    vector<string> lines;
    for (const Payment& payment : db_.get_payments()) lines.push_back(payment.toText());
    writeTextFile(pathOf("payments.txt"),
                  "# id|bookingId|loai|soTien|ngayThu|hinhThuc|trangThai|ghiChu|nguoiThu",
                  lines);
}

void Store::saveEvents() const {
    vector<string> lines;
    for (const Event& event : db_.get_events()) lines.push_back(event.toText());
    writeTextFile(pathOf("events.txt"),
                  "# id|bookingId|customerId|hallId|batDau|ketThuc|trangThai|ghiChu", lines);
}

void Store::seedDefaults() {
    // Tai khoan mau. Mat khau chung: 123456.
    db_.get_users().push_back(User("USR-0001", "admin", "123456", "Quản trị viên hệ thống",
                             "", "", "", Role::Manager, "", true));
    db_.get_users().push_back(User("USR-0002", "kinhdoanh", "123456", "Nguyễn Văn Kinh Doanh",
                             "0901111111", "kinhdoanh@example.com", "Hà Nội", Role::Sales, "", true));
    db_.get_users().push_back(User("USR-0003", "vanhanh", "123456", "Lê Văn Vận Hành",
                             "0902222222", "vanhanh@example.com", "Hà Nội", Role::Operation, "", true));

    db_.get_customers().push_back(Customer("CUS-0001", "Công ty Ánh Dương", "0912345678",
                                     "anhduong@example.com", "Cầu Giấy, Hà Nội",
                                     "Khách doanh nghiệp"));
    db_.get_customers().push_back(Customer("CUS-0002", "Nguyễn Văn An", "0987654321",
                                     "nguyenvanan@example.com", "Hoàn Kiếm, Hà Nội",
                                     "Khách hàng cá nhân"));

    db_.get_users().push_back(User("USR-0004", "khachhang", "123456", "Nguyễn Văn An",
                             "0987654321", "nguyenvanan@example.com", "Hoàn Kiếm, Hà Nội",
                             Role::Customer, "CUS-0002", true));

}

