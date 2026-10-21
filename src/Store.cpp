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

// ---------- Du lieu mau cho danh muc dich vu (theo dac ta muc 2) ----------
struct SeedService {
    const char* code;
    const char* name;
    const char* unit;
    int64_t price;
    const char* category;
};

const SeedService kSeedServices[] = {
    {"FOD01-S", "Ẩm thực Việt Nam (Standard)", "VNĐ/bàn", 3000000, "Ẩm thực"},
    {"FOD01-P", "Ẩm thực Việt Nam (Premium)", "VNĐ/bàn", 4500000, "Ẩm thực"},
    {"FOD01-L", "Ẩm thực Việt Nam (Luxury)", "VNĐ/bàn", 6500000, "Ẩm thực"},
    {"FOD02-S", "Ẩm thực Phương Tây (Standard)", "VNĐ/bàn", 4000000, "Ẩm thực"},
    {"FOD02-P", "Ẩm thực Phương Tây (Premium)", "VNĐ/bàn", 6000000, "Ẩm thực"},
    {"FOD02-L", "Ẩm thực Phương Tây (Luxury)", "VNĐ/bàn", 9000000, "Ẩm thực"},
    {"FOD03-S", "Ẩm thực Nhật Bản (Standard)", "VNĐ/bàn", 4500000, "Ẩm thực"},
    {"FOD03-P", "Ẩm thực Nhật Bản (Premium)", "VNĐ/bàn", 7000000, "Ẩm thực"},
    {"FOD03-L", "Ẩm thực Nhật Bản (Luxury)", "VNĐ/bàn", 12000000, "Ẩm thực"},
    {"FOD04-S", "Ẩm thực Hàn Quốc (Standard)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD04-P", "Ẩm thực Hàn Quốc (Premium)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD04-L", "Ẩm thực Hàn Quốc (Luxury)", "VNĐ/bàn", 7500000, "Ẩm thực"},
    {"FOD05-S", "Ẩm thực Trung Hoa (Standard)", "VNĐ/bàn", 4000000, "Ẩm thực"},
    {"FOD05-P", "Ẩm thực Trung Hoa (Premium)", "VNĐ/bàn", 6000000, "Ẩm thực"},
    {"FOD05-L", "Ẩm thực Trung Hoa (Luxury)", "VNĐ/bàn", 9000000, "Ẩm thực"},
    {"FOD06-S", "Ẩm thực Thái Lan (Standard)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD06-P", "Ẩm thực Thái Lan (Premium)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD06-L", "Ẩm thực Thái Lan (Luxury)", "VNĐ/bàn", 7000000, "Ẩm thực"},
    {"FOD07-S", "Ẩm thực Fusion (Standard)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD07-P", "Ẩm thực Fusion (Premium)", "VNĐ/bàn", 7500000, "Ẩm thực"},
    {"FOD07-L", "Ẩm thực Fusion (Luxury)", "VNĐ/bàn", 12000000, "Ẩm thực"},
    {"FOD08-S", "Buffet Á - Âu (Standard)", "VNĐ/khách", 350000, "Ẩm thực"},
    {"FOD08-P", "Buffet Á - Âu (Premium)", "VNĐ/khách", 600000, "Ẩm thực"},
    {"FOD08-L", "Buffet Á - Âu (Luxury)", "VNĐ/khách", 1000000, "Ẩm thực"},
    {"FOD09-S", "Finger Food (Standard)", "VNĐ/khách", 150000, "Ẩm thực"},
    {"FOD09-P", "Finger Food (Premium)", "VNĐ/khách", 250000, "Ẩm thực"},
    {"FOD09-L", "Finger Food (Luxury)", "VNĐ/khách", 400000, "Ẩm thực"},
    {"FOD10-S", "Thực đơn chay (Standard)", "VNĐ/bàn", 2500000, "Ẩm thực"},
    {"FOD10-P", "Thực đơn chay (Premium)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD10-L", "Thực đơn chay (Luxury)", "VNĐ/bàn", 5000000, "Ẩm thực"},

    {"DEC01-S", "Trang trí Classic (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC01-P", "Trang trí Classic (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC01-L", "Trang trí Classic (Luxury)", "VNĐ/gói", 40000000, "Trang trí"},
    {"DEC02-S", "Trang trí Luxury (Basic)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC02-P", "Trang trí Luxury (Premium)", "VNĐ/gói", 45000000, "Trang trí"},
    {"DEC02-L", "Trang trí Luxury (Luxury)", "VNĐ/gói", 100000000, "Trang trí"},
    {"DEC03-S", "Trang trí Minimalist (Basic)", "VNĐ/gói", 5000000, "Trang trí"},
    {"DEC03-P", "Trang trí Minimalist (Premium)", "VNĐ/gói", 12000000, "Trang trí"},
    {"DEC03-L", "Trang trí Minimalist (Luxury)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC04-S", "Trang trí Romantic (Basic)", "VNĐ/gói", 10000000, "Trang trí"},
    {"DEC04-P", "Trang trí Romantic (Premium)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC04-L", "Trang trí Romantic (Luxury)", "VNĐ/gói", 50000000, "Trang trí"},
    {"DEC05-S", "Trang trí Rustic (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC05-P", "Trang trí Rustic (Premium)", "VNĐ/gói", 18000000, "Trang trí"},
    {"DEC05-L", "Trang trí Rustic (Luxury)", "VNĐ/gói", 35000000, "Trang trí"},
    {"DEC06-S", "Trang trí Floral (Basic)", "VNĐ/gói", 12000000, "Trang trí"},
    {"DEC06-P", "Trang trí Floral (Premium)", "VNĐ/gói", 30000000, "Trang trí"},
    {"DEC06-L", "Trang trí Floral (Luxury)", "VNĐ/gói", 70000000, "Trang trí"},
    {"DEC07-S", "Trang trí Modern (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC07-P", "Trang trí Modern (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC07-L", "Trang trí Modern (Luxury)", "VNĐ/gói", 45000000, "Trang trí"},
    {"DEC08-S", "Trang trí Themed (Basic)", "VNĐ/gói", 10000000, "Trang trí"},
    {"DEC08-P", "Trang trí Themed (Premium)", "VNĐ/gói", 30000000, "Trang trí"},
    {"DEC08-L", "Trang trí Themed (Luxury)", "VNĐ/gói", 80000000, "Trang trí"},
    {"DEC09-S", "Trang trí Royal (Basic)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC09-P", "Trang trí Royal (Premium)", "VNĐ/gói", 60000000, "Trang trí"},
    {"DEC09-L", "Trang trí Royal (Luxury)", "VNĐ/gói", 150000000, "Trang trí"},
    {"DEC10-S", "Trang trí Festive (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC10-P", "Trang trí Festive (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC10-L", "Trang trí Festive (Luxury)", "VNĐ/gói", 50000000, "Trang trí"},

    {"MED01", "Chụp ảnh Basic Photography", "VNĐ/gói", 2000000, "Chụp ảnh & quay phim"},
    {"MED02", "Chụp ảnh Standard Photography", "VNĐ/gói", 3500000, "Chụp ảnh & quay phim"},
    {"MED03", "Chụp ảnh Premium Photography", "VNĐ/gói", 6000000, "Chụp ảnh & quay phim"},
    {"MED04", "Chụp ảnh Luxury Photography", "VNĐ/gói", 10000000, "Chụp ảnh & quay phim"},
    {"MED05", "Quay phim Basic Videography", "VNĐ/gói", 2500000, "Chụp ảnh & quay phim"},
    {"MED06", "Quay Highlight Film", "VNĐ/gói", 4500000, "Chụp ảnh & quay phim"},
    {"MED07", "Gói Full Package chụp + quay", "VNĐ/gói", 8300000, "Chụp ảnh & quay phim"},
    {"MED08", "Quay Cinematic Package", "VNĐ/gói", 12000000, "Chụp ảnh & quay phim"},
    {"MED09", "Quay Multi-Camera Package", "VNĐ/gói", 15000000, "Chụp ảnh & quay phim"},
    {"MED10", "Gói Premium Full chụp + quay", "VNĐ/gói", 20000000, "Chụp ảnh & quay phim"},

    {"AV01", "Âm thanh Basic Audio", "VNĐ/gói", 3000000, "Âm thanh & ánh sáng"},
    {"AV02", "Âm thanh Standard Audio", "VNĐ/gói", 5000000, "Âm thanh & ánh sáng"},
    {"AV03", "Âm thanh Premium Audio", "VNĐ/gói", 10000000, "Âm thanh & ánh sáng"},
    {"AV04", "Ánh sáng Basic Lighting", "VNĐ/gói", 3000000, "Âm thanh & ánh sáng"},
    {"AV05", "Ánh sáng Dynamic Lighting", "VNĐ/gói", 7000000, "Âm thanh & ánh sáng"},
    {"AV06", "Ánh sáng Wedding Lighting", "VNĐ/gói", 8000000, "Âm thanh & ánh sáng"},
    {"AV07", "Gala Standard AV", "VNĐ/gói", 12000000, "Âm thanh & ánh sáng"},
    {"AV08", "Gala Premium AV", "VNĐ/gói", 25000000, "Âm thanh & ánh sáng"},
    {"AV09", "Concert Package AV", "VNĐ/gói", 40000000, "Âm thanh & ánh sáng"},
    {"AV10", "Full AV Package", "VNĐ/gói", 30000000, "Âm thanh & ánh sáng"},

    {"HR01", "MC cơ bản", "VNĐ/buổi", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR02", "MC chuyên nghiệp", "VNĐ/buổi", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR03", "MC song ngữ", "VNĐ/buổi", 8000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR04", "Lễ tân sự kiện", "VNĐ/người/buổi", 500000, "Nhân sự & dịch vụ bổ sung"},
    {"HR05", "Điều phối viên", "VNĐ/người/buổi", 1500000, "Nhân sự & dịch vụ bổ sung"},
    {"HR06", "Ca sĩ biểu diễn", "VNĐ/người/buổi", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR07", "Ban nhạc", "VNĐ/buổi", 10000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR08", "Vũ đoàn", "VNĐ/buổi", 8000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR09", "Phiên dịch viên", "VNĐ/người/buổi", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR10", "Kỹ thuật viên sân khấu", "VNĐ/người/buổi", 1000000, "Nhân sự & dịch vụ bổ sung"},

    {"EXT01", "Bánh cưới / bánh sự kiện", "VNĐ/chiếc", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT02", "Bánh sinh nhật cao cấp", "VNĐ/chiếc", 800000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT03", "Thiệp mời", "VNĐ/thiệp", 15000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT04", "Quà tặng khách mời", "VNĐ/phần", 100000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT05", "Backdrop chụp ảnh", "VNĐ/gói", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT06", "Màn hình LED bổ sung", "VNĐ/gói", 10000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT07", "Livestream sự kiện", "VNĐ/gói", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT08", "Pháo giấy sân khấu", "VNĐ/lần", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT09", "Xe đưa đón", "VNĐ/xe/chuyến", 3000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT10", "In ảnh lấy liền (300 ảnh)", "VNĐ/gói", 6500000, "Nhân sự & dịch vụ bổ sung"}
};

struct SeedHall {
    const char* name;
    int capacity;
    int64_t rentalPrice;
};

const SeedHall kSeedHalls[] = {
    {"Sảnh Mini", 50, 5000000},
    {"Sảnh Small", 100, 8000000},
    {"Sảnh Standard", 200, 12000000},
    {"Sảnh Large", 300, 18000000},
    {"Sảnh Grand", 500, 30000000},
    {"Sảnh Ballroom", 800, 50000000}
};

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

    seedCatalog();
}

void Store::seedCatalog() {
    // --- Them sanh con thieu (so sanh theo ten) ---
    int maxHallNumber = 0;
    for (const Hall& hall : db_.get_halls()) {
        if (hall.get_id().rfind("HAL-", 0) == 0) {
            maxHallNumber = max(maxHallNumber, toInt(hall.get_id().substr(4)));
        }
    }

    for (const SeedHall& seed : kSeedHalls) {
        bool exists = false;
        for (const Hall& hall : db_.get_halls()) {
            if (hall.get_name() == seed.name) {
                exists = true;
                break;
            }
        }
        if (exists) continue;

        ostringstream id;
        id << "HAL-" << setfill('0') << setw(4) << (++maxHallNumber);
        db_.get_halls().push_back(Hall(id.str(), seed.name, seed.capacity, seed.rentalPrice,
                                 HallStatus::KhaDung));
    }

    // --- Them dich vu con thieu (so sanh theo ma) ---
    int maxServiceNumber = 0;
    for (const Service* service : db_.get_services()) {
        if (service->get_id().rfind("SVC-", 0) == 0) {
            maxServiceNumber = max(maxServiceNumber, toInt(service->get_id().substr(4)));
        }
    }

    for (const SeedService& seed : kSeedServices) {
        bool exists = false;
        for (const Service* service : db_.get_services()) {
            if (service->get_code() == seed.code) {
                exists = true;
                break;
            }
        }
        if (exists) continue;

        ostringstream id;
        id << "SVC-" << setfill('0') << setw(4) << (++maxServiceNumber);

        string code = seed.code;
        if (code.rfind("FOD", 0) == 0) {
            db_.get_services().push_back(new FoodService(id.str(), code, seed.name, seed.category,
                                                    seed.unit, seed.price, true));
        } else {
            db_.get_services().push_back(new PackageService(id.str(), code, seed.name, seed.category,
                                                       seed.unit, seed.price, true));
        }
    }
}
