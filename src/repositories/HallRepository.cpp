#include "HallRepository.h"
#include "../exceptions/AppException.h"

namespace {
string statusText(HallStatus status) { return status == HallStatus::ACTIVE ? "ACTIVE" : "INACTIVE"; }
HallStatus hallStatus(const string& value) { return value == "ACTIVE" ? HallStatus::ACTIVE : HallStatus::INACTIVE; }
void prepareOrThrow(DatabaseManager& db, const char* sql, sqlite3_stmt** statement) {
    if (sqlite3_prepare_v2(db.getConnection(), sql, -1, statement, nullptr) != SQLITE_OK) throw DatabaseException("Database error: " + string(sqlite3_errmsg(db.getConnection())));
}
Hall readHall(sqlite3_stmt* statement) {
    return Hall(sqlite3_column_int(statement, 0), sqlite3_column_int(statement, 1), reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)), reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)), sqlite3_column_int(statement, 4), hallStatus(reinterpret_cast<const char*>(sqlite3_column_text(statement, 5))));
}
}

HallRepository::HallRepository(DatabaseManager& db) : db(db) {}
void HallRepository::save(const Hall& hall) {
    sqlite3_stmt* statement = nullptr;
    prepareOrThrow(db, "INSERT INTO halls (center_id, hall_code, name, capacity, status) VALUES (?, ?, ?, ?, ?);", &statement);
    sqlite3_bind_int(statement, 1, hall.getCenterId()); sqlite3_bind_text(statement, 2, hall.getHallCode().c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_text(statement, 3, hall.getName().c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_int(statement, 4, hall.getCapacity()); string status = statusText(hall.getStatus()); sqlite3_bind_text(statement, 5, status.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(statement) != SQLITE_DONE) { string error = sqlite3_errmsg(db.getConnection()); sqlite3_finalize(statement); throw DatabaseException("Unable to save hall: " + error); }
    sqlite3_finalize(statement);
}
optional<Hall> HallRepository::findByCode(const string& hallCode) const {
    sqlite3_stmt* statement = nullptr;
    prepareOrThrow(const_cast<DatabaseManager&>(db), "SELECT id, center_id, hall_code, name, capacity, status FROM halls WHERE hall_code = ?;", &statement); sqlite3_bind_text(statement, 1, hallCode.c_str(), -1, SQLITE_TRANSIENT);
    int result = sqlite3_step(statement); if (result == SQLITE_ROW) { Hall hall = readHall(statement); sqlite3_finalize(statement); return hall; } sqlite3_finalize(statement); return nullopt;
}
vector<Hall> HallRepository::findAll() const {
    vector<Hall> halls; sqlite3_stmt* statement = nullptr; prepareOrThrow(const_cast<DatabaseManager&>(db), "SELECT id, center_id, hall_code, name, capacity, status FROM halls ORDER BY hall_code;", &statement); while (sqlite3_step(statement) == SQLITE_ROW) halls.push_back(readHall(statement)); sqlite3_finalize(statement); return halls;
}
void HallRepository::update(const Hall& hall) {
    sqlite3_stmt* statement = nullptr; prepareOrThrow(db, "UPDATE halls SET center_id = ?, name = ?, capacity = ?, status = ? WHERE hall_code = ?;", &statement); string status = statusText(hall.getStatus()); sqlite3_bind_int(statement, 1, hall.getCenterId()); sqlite3_bind_text(statement, 2, hall.getName().c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_int(statement, 3, hall.getCapacity()); sqlite3_bind_text(statement, 4, status.c_str(), -1, SQLITE_TRANSIENT); sqlite3_bind_text(statement, 5, hall.getHallCode().c_str(), -1, SQLITE_TRANSIENT); if (sqlite3_step(statement) != SQLITE_DONE) { sqlite3_finalize(statement); throw DatabaseException("Unable to update hall."); } sqlite3_finalize(statement);
}
