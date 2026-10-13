#include "storage/json_storage.hpp"

#include "utils/sha256.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace tdp {
namespace fs = filesystem;

namespace {

template <typename T>
vector<T> loadArray(const nlohmann::json& root, const string& key) {
    vector<T> out;
    if (!root.contains(key) || !root.at(key).is_array()) return out;
    for (const auto& item : root.at(key)) out.push_back(T::fromJson(item));
    return out;
}

template <typename T>
nlohmann::json dumpArray(const vector<T>& items) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& item : items) arr.push_back(item.toJson());
    return arr;
}

void addUser(Database& db, const string& id, const string& username, const string& fullName, Role role,
             const string& customerId = "") {
    const string salt = randomSaltHex();
    User u(id, username, salt, hashPassword("123456", salt), fullName, role, true);
    u.setDemoPassword("123456");
    u.setCustomerId(customerId);
    db.users.push_back(move(u));
}

struct SeedService {
    const char* code;
    const char* name;
    const char* unit;
    int64_t price;
    const char* category;
};

// Danh muc dich vu theo dac ta muc 2 (gia gia lap tham khao).
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
    {"EXT10", "In ảnh lấy liền (300 ảnh)", "VNĐ/gói", 6500000, "Nhân sự & dịch vụ bổ sung"},
};

struct SeedHall {
    const char* name;
    int capacity;
    int64_t rentalPrice;
};

// Sanh theo dac ta 2.6 (suc chua lay muc toi da tham khao).
const SeedHall kSeedHalls[] = {
    {"Sảnh Mini", 50, 5000000},
    {"Sảnh Small", 100, 8000000},
    {"Sảnh Standard", 200, 12000000},
    {"Sảnh Large", 300, 18000000},
    {"Sảnh Grand", 500, 30000000},
    {"Sảnh Ballroom", 800, 50000000},
};

}  // namespace

JsonStorage::JsonStorage(fs::path dataDir, fs::path backupDir)
    : dataDir_(move(dataDir)), backupDir_(move(backupDir)) {}

fs::path JsonStorage::filePath(const string& name) const { return dataDir_ / name; }

nlohmann::json JsonStorage::readJsonFile(const fs::path& path) const {
    ifstream in(path);
    if (!in) throw StorageError("Không đọc được file: " + path.string());
    try {
        nlohmann::json j;
        in >> j;
        return j;
    } catch (const exception& ex) {
        throw StorageError("JSON sai định dạng ở " + path.string() + ": " + ex.what());
    }
}

void JsonStorage::writeJsonAtomic(const fs::path& path, const nlohmann::json& j) const {
    fs::path tmp = path;
    tmp += ".tmp";
    {
        ofstream out(tmp, ios::binary | ios::trunc);
        if (!out) throw StorageError("Không ghi được file tạm: " + tmp.string());
        out << setw(2) << j << '\n';
        out.flush();
        if (!out) throw StorageError("Ghi file tạm thất bại: " + tmp.string());
    }
    error_code ec;
    fs::rename(tmp, path, ec);
    if (ec) {
        fs::remove(path, ec);
        fs::rename(tmp, path, ec);
        if (ec) throw StorageError("Không thay thế được file chính: " + path.string());
    }
}

void JsonStorage::backupCurrentFiles() const {
    if (!fs::exists(dataDir_)) return;
    bool any = false;
    for (auto& p : fs::directory_iterator(dataDir_)) {
        if (p.path().extension() == ".json") {
            any = true;
            break;
        }
    }
    if (!any) return;

    auto now = DateTime::now();
    ostringstream folder;
    folder << setfill('0') << now.year << setw(2) << now.month << setw(2) << now.day << '_'
           << setw(2) << now.hour << setw(2) << now.minute << '_' << time(nullptr);
    fs::path dest = backupDir_ / folder.str();
    fs::create_directories(dest);
    for (auto& p : fs::directory_iterator(dataDir_)) {
        if (p.path().extension() == ".json") {
            fs::copy_file(p.path(), dest / p.path().filename(), fs::copy_options::overwrite_existing);
        }
    }
}

void JsonStorage::writeAllFiles(const Database& data) const {
    fs::create_directories(dataDir_);
    nlohmann::json users = {{"users", dumpArray(data.users)}};
    nlohmann::json customers = {{"customers", dumpArray(data.customers)}, {"requests", dumpArray(data.requests)}};
    nlohmann::json venues = {{"halls", dumpArray(data.halls)}, {"services", dumpArray(data.services)}};
    nlohmann::json transactions = {{"bookings", dumpArray(data.bookings)},
                                   {"payments", dumpArray(data.payments)}};
    nlohmann::json events = {{"events", dumpArray(data.events)}};

    writeJsonAtomic(filePath("users.json"), users);
    writeJsonAtomic(filePath("customers.json"), customers);
    writeJsonAtomic(filePath("venues.json"), venues);
    writeJsonAtomic(filePath("transactions.json"), transactions);
    writeJsonAtomic(filePath("events.json"), events);
}

void JsonStorage::save() {
    Database snapshot = db_;
    try {
        backupCurrentFiles();
        writeAllFiles(db_);
        lastError_.clear();
    } catch (...) {
        db_ = move(snapshot);
        throw;
    }
}

void JsonStorage::seedIfEmpty() {
    if (!db_.users.empty()) return;

    addUser(db_, "USR-0001", "admin", "Quản trị viên hệ thống", Role::Manager);
    addUser(db_, "USR-0002", "kinhdoanh", "Nguyễn Văn Kinh Doanh", Role::Sales);
    addUser(db_, "USR-0003", "vanhanh", "Lê Văn Vận Hành", Role::Operation);
    db_.customers.push_back(Customer("CUS-0001", "Công ty Ánh Dương", "0912345678",
                                     "anhduong@example.com", "Cầu Giấy, Hà Nội", "Khách doanh nghiệp",
                                     DateTime::now()));
    db_.customers.push_back(Customer("CUS-0002", "Nguyễn Văn An", "0987654321",
                                     "nguyenvanan@example.com", "Hoàn Kiếm, Hà Nội", "Tiệc cưới",
                                     DateTime::now()));
    addUser(db_, "USR-0004", "khachhang", "Nguyễn Văn An", Role::Customer, "CUS-0002");

    int hallNo = 0;
    for (const auto& h : kSeedHalls) {
        ostringstream id;
        id << "HAL-" << setfill('0') << setw(4) << ++hallNo;
        db_.halls.push_back(Hall(id.str(), h.name, h.capacity, h.rentalPrice, HallStatus::KhaDung));
    }

    int svcNo = 0;
    for (const auto& s : kSeedServices) {
        ostringstream id;
        id << "SVC-" << setfill('0') << setw(4) << ++svcNo;
        db_.services.push_back(ServiceItem(id.str(), s.code, s.name, s.unit, s.price, s.category, true));
    }
}

void JsonStorage::load() {
    fs::create_directories(dataDir_);
    fs::create_directories(backupDir_);

    const bool hasUsers = fs::exists(filePath("users.json"));
    if (!hasUsers) {
        db_ = Database{};
        seedIfEmpty();
        save();
        return;
    }

    try {
        auto users = readJsonFile(filePath("users.json"));
        auto customers = fs::exists(filePath("customers.json")) ? readJsonFile(filePath("customers.json"))
                                                                : nlohmann::json::object();
        auto venues = fs::exists(filePath("venues.json")) ? readJsonFile(filePath("venues.json")) : nlohmann::json::object();
        auto transactions = fs::exists(filePath("transactions.json")) ? readJsonFile(filePath("transactions.json"))
                                                                     : nlohmann::json::object();
        auto events = fs::exists(filePath("events.json")) ? readJsonFile(filePath("events.json"))
                                                          : nlohmann::json::object();

        Database loaded;
        loaded.users = loadArray<User>(users, "users");
        loaded.customers = loadArray<Customer>(customers, "customers");
        loaded.requests = loadArray<EventRequest>(customers, "requests");
        loaded.halls = loadArray<Hall>(venues, "halls");
        loaded.services = loadArray<ServiceItem>(venues, "services");
        loaded.bookings = loadArray<Booking>(transactions, "bookings");
        loaded.payments = loadArray<Payment>(transactions, "payments");
        loaded.events = loadArray<Event>(events, "events");
        db_ = move(loaded);
        lastError_.clear();
    } catch (const StorageError&) {
        throw;
    } catch (const exception& ex) {
        throw StorageError(string("Không nạp được dữ liệu (giữ nguyên file cũ): ") + ex.what());
    }
}

}  // namespace tdp
