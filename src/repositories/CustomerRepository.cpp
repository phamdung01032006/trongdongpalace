#include "CustomerRepository.h"
#include "../exceptions/AppException.h"

namespace {
void checkResult(int result, sqlite3* connection, sqlite3_stmt* statement) {
    if (result != SQLITE_OK) {
        string message = sqlite3_errmsg(connection);
        if (statement != nullptr) sqlite3_finalize(statement);
        throw DatabaseException("Database error: " + message);
    }
}

Customer readCustomer(sqlite3_stmt* statement) {
    return Customer(
        sqlite3_column_int(statement, 0),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 3)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 4)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 5))
    );
}
}

CustomerRepository::CustomerRepository(DatabaseManager& db) : db(db) {}

void CustomerRepository::save(const Customer& customer) {
    const char* sql = "INSERT INTO customers (customer_code, name, phone, email, address) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    sqlite3_bind_text(statement, 1, customer.getCustomerCode().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, customer.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, customer.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, customer.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, customer.getAddress().c_str(), -1, SQLITE_TRANSIENT);
    int result = sqlite3_step(statement);
    if (result != SQLITE_DONE) {
        string message = sqlite3_errmsg(db.getConnection());
        sqlite3_finalize(statement);
        throw DatabaseException("Unable to save customer: " + message);
    }
    sqlite3_finalize(statement);
}

optional<Customer> CustomerRepository::findByCode(const string& customerCode) const {
    const char* sql = "SELECT id, customer_code, name, phone, email, address FROM customers WHERE customer_code = ?;";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    sqlite3_bind_text(statement, 1, customerCode.c_str(), -1, SQLITE_TRANSIENT);
    int result = sqlite3_step(statement);
    if (result == SQLITE_ROW) {
        Customer customer = readCustomer(statement);
        sqlite3_finalize(statement);
        return customer;
    }
    sqlite3_finalize(statement);
    if (result != SQLITE_DONE) throw DatabaseException("Unable to read customer.");
    return nullopt;
}

vector<Customer> CustomerRepository::findAll() const {
    vector<Customer> customers;
    const char* sql = "SELECT id, customer_code, name, phone, email, address FROM customers ORDER BY customer_code;";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    while (sqlite3_step(statement) == SQLITE_ROW) customers.push_back(readCustomer(statement));
    sqlite3_finalize(statement);
    return customers;
}

void CustomerRepository::update(const Customer& customer) {
    const char* sql = "UPDATE customers SET name = ?, phone = ?, email = ?, address = ? WHERE customer_code = ?;";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    sqlite3_bind_text(statement, 1, customer.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, customer.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, customer.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, customer.getAddress().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, customer.getCustomerCode().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(statement) != SQLITE_DONE) {
        sqlite3_finalize(statement);
        throw DatabaseException("Unable to update customer.");
    }
    sqlite3_finalize(statement);
}

void CustomerRepository::remove(const string& customerCode) {
    const char* sql = "DELETE FROM customers WHERE customer_code = ?;";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    sqlite3_bind_text(statement, 1, customerCode.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(statement) != SQLITE_DONE) {
        sqlite3_finalize(statement);
        throw DatabaseException("Unable to delete customer.");
    }
    sqlite3_finalize(statement);
}

bool CustomerRepository::isUsedByEvent(const string& customerCode) const {
    const char* sql = "SELECT COUNT(*) FROM events e JOIN customers c ON e.customer_id = c.id WHERE c.customer_code = ?;";
    sqlite3_stmt* statement = nullptr;
    checkResult(sqlite3_prepare_v2(db.getConnection(), sql, -1, &statement, nullptr), db.getConnection(), statement);
    sqlite3_bind_text(statement, 1, customerCode.c_str(), -1, SQLITE_TRANSIENT);
    bool used = sqlite3_step(statement) == SQLITE_ROW && sqlite3_column_int(statement, 0) > 0;
    sqlite3_finalize(statement);
    return used;
}
