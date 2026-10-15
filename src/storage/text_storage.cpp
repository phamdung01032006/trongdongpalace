#ifndef TDP_TEXT_STORAGE_CPP
#define TDP_TEXT_STORAGE_CPP

#include "models/Database.cpp"
#include "storage/catalog_seed.cpp"
#include "utils/sha256.cpp"
#include <filesystem>
#include <fstream>
#include <set>

using namespace std;

class StorageError : public FileException {
public:
    explicit StorageError(const string& message) : FileException(message) {}
};

class TextStorage {
public:
    explicit TextStorage(filesystem::path dataDir, filesystem::path backupDir)
        : dataDir_(move(dataDir)), backupDir_(move(backupDir)) {}
    Database& db() { return db_; }
    const Database& db() const { return db_; }
    const filesystem::path& dataDir() const { return dataDir_; }
    void load();
    void save();
private:
    filesystem::path dataDir_, backupDir_;
    Database db_;
    const vector<string> names_ = {"users", "customers", "requests", "halls", "services",
                                   "bookings", "payments", "events"};
    void seed();
    vector<vector<string>> readRows(const string& name) const;
    void writeRows(const string& name, const vector<vector<string>>& rows) const;
    void validate(const Database& data) const;
};

vector<vector<string>> TextStorage::readRows(const string& name) const {
    auto path = dataDir_ / (name + ".txt");
    ifstream in(path, ios::binary);
    if (!in) throw StorageError("Khong doc duoc file: " + path.string());
    vector<vector<string>> rows;
    string line;
    size_t number = 0;
    while (getline(in, line)) {
        ++number;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        try { rows.push_back(splitTextFields(line)); }
        catch (const exception& ex) {
            throw StorageError(path.string() + ": dong " + to_string(number) + ": " + ex.what());
        }
    }
    if (in.bad()) throw StorageError("Loi doc file: " + path.string());
    return rows;
}

void TextStorage::writeRows(const string& name, const vector<vector<string>>& rows) const {
    auto path = dataDir_ / (name + ".txt.tmp");
    ofstream out(path, ios::binary | ios::trunc);
    if (!out) throw StorageError("Khong ghi duoc file: " + path.string());
    for (const auto& row : rows) out << joinTextFields(row) << '\n';
    out.flush();
    if (!out) throw StorageError("Ghi file that bai: " + path.string());
    out.close();
    if (!out) throw StorageError("Dong file that bai: " + path.string());
}

template <typename T>
void checkTextIds(const vector<T>& records) {
    set<string> ids;
    for (const auto& item : records) {
        if (item.id().empty()) throw StorageError("ID rong");
        if (!ids.insert(item.id()).second) throw DuplicateException("ID trung: " + item.id());
    }
}

template <typename T>
bool hasTextId(const vector<T>& records, const string& id) {
    for (const auto& item : records) if (item.id() == id) return true;
    return false;
}

void TextStorage::validate(const Database& d) const {
    checkTextIds(d.users); checkTextIds(d.customers); checkTextIds(d.requests);
    checkTextIds(d.halls); checkTextIds(d.services); checkTextIds(d.bookings); checkTextIds(d.payments);
    set<string> usernames, phones, codes, eventIds;
    for (const auto& u : d.users) {
        if (trim(u.username()).empty() || !usernames.insert(toLower(u.username())).second)
            throw StorageError("Ten dang nhap rong/trung");
        if (!u.customerId().empty() && !hasTextId(d.customers, u.customerId()))
            throw StorageError("User tham chieu khach hang khong ton tai");
    }
    for (const auto& c : d.customers)
        if (c.age() < 0 || c.age() > 120 || trim(c.name()).empty() || !looksLikePhone(c.phone()) ||
            !phones.insert(c.phone()).second) throw StorageError("Ho so khach hang khong hop le/trung SDT");
    for (const auto& s : d.services)
        if (s.code().empty() || !codes.insert(s.code()).second) throw StorageError("Ma dich vu rong/trung");
    for (const auto& r : d.requests) {
        if (!hasTextId(d.customers, r.customerId()) ||
            (!r.hallId().empty() && !hasTextId(d.halls, r.hallId())) || !(r.start() < r.end()))
            throw StorageError("Yeu cau co ID lien ket/thoi gian khong hop le");
        for (const auto& l : r.lines())
            if (!hasTextId(d.services, l.serviceId()) || l.quantity() <= 0)
                throw StorageError("Dong dich vu khong hop le");
    }
    for (size_t i = 0; i < d.bookings.size(); ++i) {
        const auto& b = d.bookings[i];
        if (!hasTextId(d.requests, b.requestId()) || !hasTextId(d.customers, b.customerId()) ||
            !hasTextId(d.halls, b.hallId()) || !(b.start() < b.end()) ||
            b.discountAmount() > b.estimatedTotal() || b.discountPercent() > 100)
            throw StorageError("Booking co ID lien ket/thoi gian/giam gia khong hop le");
        for (size_t j = 0; j < i; ++j) {
            const auto& old = d.bookings[j];
            if (b.occupiesHall() && old.occupiesHall() && b.hallId() == old.hallId() &&
                timeOverlaps(b.start(), b.end(), old.start(), old.end()))
                throw StorageError("Booking trung lich sanh");
        }
    }
    for (const auto& p : d.payments)
        if (!hasTextId(d.bookings, p.bookingId())) throw StorageError("Payment khong co booking");
    for (const auto& e : d.events) {
        if (!e || e->id().empty() || !eventIds.insert(e->id()).second)
            throw StorageError("ID su kien rong/trung");
        if (!hasTextId(d.bookings, e->bookingId()) || !hasTextId(d.customers, e->customerId()) ||
            !hasTextId(d.halls, e->hallId()) || !(e->start() < e->end()))
            throw StorageError("Su kien co ID lien ket/thoi gian khong hop le");
    }
}

void TextStorage::seed() {
    auto addUser = [&](const string& id, const string& name, const string& fullName, Role role,
                       const string& customerId = "") {
        string salt = randomSaltHex();
        User user(id, name, salt, hashPassword("123456", salt), fullName, role, true);
        user.setDemoPassword("123456"); user.setCustomerId(customerId);
        db_.users.push_back(user);
    };
    addUser("USR-0001", "admin", "Quan tri vien", Role::Manager);
    addUser("USR-0002", "kinhdoanh", "Nhan vien kinh doanh", Role::Sales);
    addUser("USR-0003", "vanhanh", "Nhan vien van hanh", Role::Operation);
    db_.customers.emplace_back("CUS-0001", "Công ty Ánh Dương", "0912345678",
                              "anhduong@example.com", "Hà Nội", "Khách doanh nghiệp", DateTime::now());
    db_.customers.emplace_back("CUS-0002", "Nguyễn Văn An", "0987654321",
                              "nguyenvanan@example.com", "Hà Nội", "Tiệc cưới", DateTime::now());
    addUser("USR-0004", "khachhang", "Nguyễn Văn An", Role::Customer, "CUS-0002");
    seedCatalog(db_);
}

void TextStorage::load() {
    filesystem::create_directories(dataDir_);
    bool any = false;
    for (const auto& name : names_)
        if (filesystem::exists(dataDir_ / (name + ".txt"))) any = true;
    if (!any) {
        for (const auto& entry : filesystem::directory_iterator(dataDir_))
            if (entry.path().extension() == ".json")
                throw StorageError("Thu muc chi co JSON cu. Can khoi phuc bo du lieu TXT truoc khi chay.");
        db_ = Database{};
        seed(); save(); return;
    }
    Database loaded;
    string currentFile;
    try {
        currentFile = "users";
        for (const auto& f : readRows(currentFile)) loaded.users.push_back(User::fromFields(f));
        currentFile = "customers";
        for (const auto& f : readRows(currentFile)) loaded.customers.push_back(Customer::fromFields(f));
        currentFile = "requests";
        for (const auto& f : readRows(currentFile)) loaded.requests.push_back(EventRequest::fromFields(f));
        currentFile = "halls";
        for (const auto& f : readRows(currentFile)) loaded.halls.push_back(Hall::fromFields(f));
        currentFile = "services";
        for (const auto& f : readRows(currentFile)) loaded.services.push_back(ServiceItem::fromFields(f));
        currentFile = "bookings";
        for (const auto& f : readRows(currentFile)) loaded.bookings.push_back(Booking::fromFields(f));
        currentFile = "payments";
        for (const auto& f : readRows(currentFile)) loaded.payments.push_back(Payment::fromFields(f));
        currentFile = "events";
        for (const auto& f : readRows(currentFile)) loaded.events.push_back(Event::fromFields(f));
        validate(loaded);
        db_ = move(loaded);
    } catch (const exception& ex) {
        throw StorageError(currentFile + ".txt: " + ex.what() + " (giu nguyen file cu)");
    }
}

template <typename T>
vector<vector<string>> textRows(const vector<T>& items) {
    vector<vector<string>> rows;
    for (const auto& item : items) rows.push_back(item.toFields());
    return rows;
}

void TextStorage::save() {
    validate(db_);
    filesystem::create_directories(dataDir_);
    writeRows("users", textRows(db_.users));
    writeRows("customers", textRows(db_.customers));
    writeRows("requests", textRows(db_.requests));
    writeRows("halls", textRows(db_.halls));
    writeRows("services", textRows(db_.services));
    writeRows("bookings", textRows(db_.bookings));
    writeRows("payments", textRows(db_.payments));
    vector<vector<string>> events;
    for (const auto& e : db_.events) events.push_back(e->toFields());
    writeRows("events", events);
    auto backup = backupDir_ / "previous";
    filesystem::create_directories(backup);
    for (const auto& name : names_) {
        auto path = dataDir_ / (name + ".txt");
        if (filesystem::exists(path))
            filesystem::copy_file(path, backup / path.filename(), filesystem::copy_options::overwrite_existing);
    }
    vector<string> changed;
    try {
        for (const auto& name : names_) {
            auto path = dataDir_ / (name + ".txt");
            changed.push_back(name);
            filesystem::copy_file(dataDir_ / (name + ".txt.tmp"), path,
                                  filesystem::copy_options::overwrite_existing);
        }
    } catch (...) {
        for (const auto& name : changed) {
            auto old = backup / (name + ".txt");
            if (filesystem::exists(old)) {
                error_code ec;
                filesystem::copy_file(old, dataDir_ / (name + ".txt"),
                                      filesystem::copy_options::overwrite_existing, ec);
            }
        }
        throw StorageError("Luu that bai; da thu khoi phuc tu backups/previous");
    }
    for (const auto& name : names_) {
        error_code ec;
        filesystem::remove(dataDir_ / (name + ".txt.tmp"), ec);
    }
}

#endif
