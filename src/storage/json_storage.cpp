#include "storage/json_storage.hpp"

#include "utils/sha256.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace tdp {
namespace fs = std::filesystem;

namespace {

template <typename T>
std::vector<T> loadArray(const nlohmann::json& root, const std::string& key) {
    std::vector<T> out;
    if (!root.contains(key) || !root.at(key).is_array()) return out;
    for (const auto& item : root.at(key)) out.push_back(T::fromJson(item));
    return out;
}

template <typename T>
nlohmann::json dumpArray(const std::vector<T>& items) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& item : items) arr.push_back(item.toJson());
    return arr;
}

void addUser(Database& db, const std::string& id, const std::string& username, const std::string& fullName, Role role) {
    const std::string salt = randomSaltHex();
    User u(id, username, salt, hashPassword("123456", salt), fullName, role, true);
    u.setDemoPassword("123456");
    db.users.push_back(std::move(u));
}

}  // namespace

JsonStorage::JsonStorage(fs::path dataDir, fs::path backupDir)
    : dataDir_(std::move(dataDir)), backupDir_(std::move(backupDir)) {}

fs::path JsonStorage::filePath(const std::string& name) const { return dataDir_ / name; }

nlohmann::json JsonStorage::readJsonFile(const fs::path& path) const {
    std::ifstream in(path);
    if (!in) throw StorageError("Không đọc được file: " + path.string());
    try {
        nlohmann::json j;
        in >> j;
        return j;
    } catch (const std::exception& ex) {
        throw StorageError("JSON sai định dạng ở " + path.string() + ": " + ex.what());
    }
}

void JsonStorage::writeJsonAtomic(const fs::path& path, const nlohmann::json& j) const {
    fs::path tmp = path;
    tmp += ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) throw StorageError("Không ghi được file tạm: " + tmp.string());
        out << std::setw(2) << j << '\n';
        out.flush();
        if (!out) throw StorageError("Ghi file tạm thất bại: " + tmp.string());
    }
    std::error_code ec;
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
    std::ostringstream folder;
    folder << std::setfill('0') << now.year << std::setw(2) << now.month << std::setw(2) << now.day << '_'
           << std::setw(2) << now.hour << std::setw(2) << now.minute << '_' << std::time(nullptr);
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
    nlohmann::json users = {{"users", dumpArray(data.users)}, {"logs", dumpArray(data.logs)}};
    nlohmann::json customers = {{"customers", dumpArray(data.customers)}, {"requests", dumpArray(data.requests)}};
    nlohmann::json venues = {{"centers", dumpArray(data.centers)},
                             {"halls", dumpArray(data.halls)},
                             {"services", dumpArray(data.services)}};
    nlohmann::json transactions = {{"quotations", dumpArray(data.quotations)},
                                   {"bookings", dumpArray(data.bookings)},
                                   {"contracts", dumpArray(data.contracts)},
                                   {"payments", dumpArray(data.payments)}};
    nlohmann::json events = {{"events", dumpArray(data.events)},
                             {"tasks", dumpArray(data.tasks)},
                             {"incidents", dumpArray(data.incidents)}};

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
        db_ = std::move(snapshot);
        throw;
    }
}

void JsonStorage::seedIfEmpty() {
    if (!db_.users.empty()) return;

    addUser(db_, "USR-0001", "admin", "Quản trị viên hệ thống", Role::Admin);
    addUser(db_, "USR-0002", "kinhdoanh", "Nguyễn Văn Kinh Doanh", Role::Sales);
    addUser(db_, "USR-0003", "ketoan", "Trần Thị Kế Toán", Role::Accountant);
    addUser(db_, "USR-0004", "dieuphoi", "Lê Văn Điều Phối", Role::Coordinator);

    db_.centers.push_back(Center("CTR-0001", "Trống Đồng Palace Trần Đăng Ninh", "72 Trần Đăng Ninh, Cầu Giấy, Hà Nội", true));
    db_.centers.push_back(Center("CTR-0002", "Trống Đồng Palace Hàng Cót", "40 Hàng Cót, Hoàn Kiếm, Hà Nội", true));
    db_.centers.push_back(Center("CTR-0003", "Trống Đồng Palace Cảnh Hồ", "Hà Nội", true));

    db_.halls.push_back(Hall("HAL-0001", "CTR-0001", "Sảnh Ruby", 250, true));
    db_.halls.push_back(Hall("HAL-0002", "CTR-0001", "Sảnh Sapphire", 400, true));
    db_.halls.push_back(Hall("HAL-0003", "CTR-0002", "Sảnh Jade", 180, true));
    db_.halls.push_back(Hall("HAL-0004", "CTR-0003", "King 1+2", 800, true));
    db_.halls.push_back(Hall("HAL-0005", "CTR-0003", "Queen 1", 280, true));

    db_.services.push_back(ServiceItem("SVC-0001", "Thuê sảnh buổi tối", "buổi", 25000000, "Sảnh", true));
    db_.services.push_back(ServiceItem("SVC-0002", "Thực đơn tiệc cưới", "khách", 450000, "Ẩm thực", true));
    db_.services.push_back(ServiceItem("SVC-0003", "Thực đơn hội nghị", "khách", 320000, "Ẩm thực", true));
    db_.services.push_back(ServiceItem("SVC-0004", "Âm thanh ánh sáng", "gói", 8000000, "Kỹ thuật", true));
    db_.services.push_back(ServiceItem("SVC-0005", "Màn hình LED", "gói", 12000000, "Kỹ thuật", true));
    db_.services.push_back(ServiceItem("SVC-0006", "Trang trí không gian", "gói", 15000000, "Trang trí", true));
    db_.services.push_back(ServiceItem("SVC-0007", "MC sự kiện", "buổi", 5000000, "Nhân sự", true));

    db_.customers.push_back(Customer("CUS-0001", "Công ty Ánh Dương", "0912345678", "anhduong@example.com",
                                     "Cầu Giấy, Hà Nội", "Khách doanh nghiệp", DateTime::now()));
    db_.customers.push_back(Customer("CUS-0002", "Nguyễn Văn An", "0987654321", "nguyenvanan@example.com",
                                     "Hoàn Kiếm, Hà Nội", "Tiệc cưới", DateTime::now()));
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
        loaded.centers = loadArray<Center>(venues, "centers");
        loaded.halls = loadArray<Hall>(venues, "halls");
        loaded.services = loadArray<ServiceItem>(venues, "services");
        loaded.quotations = loadArray<Quotation>(transactions, "quotations");
        loaded.bookings = loadArray<Booking>(transactions, "bookings");
        loaded.contracts = loadArray<Contract>(transactions, "contracts");
        loaded.payments = loadArray<Payment>(transactions, "payments");
        loaded.events = loadArray<Event>(events, "events");
        loaded.tasks = loadArray<OperationTask>(events, "tasks");
        loaded.incidents = loadArray<Incident>(events, "incidents");
        loaded.logs = loadArray<AuditLog>(users, "logs");
        db_ = std::move(loaded);
        lastError_.clear();
    } catch (const StorageError&) {
        throw;
    } catch (const std::exception& ex) {
        throw StorageError(std::string("Không nạp được dữ liệu (giữ nguyên file cũ): ") + ex.what());
    }
}

}  // namespace tdp
