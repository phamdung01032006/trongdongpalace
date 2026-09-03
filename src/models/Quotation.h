#pragma once
#include <string>
using namespace std;

enum class QuotationStatus {
    DRAFT,
    CONFIRMED,
    CONVERTED,
    CANCELLED
};

class Quotation {
private:
    int id;
    string quotationCode;
    int bookingId;
    double subtotal;
    double discount;
    double tax;
    double total;
    QuotationStatus status;

public:
    Quotation(int id,
            const string& quotationCode,
            int bookingId,
            double subtotal,
            double discount,
            double tax,
            double total,
            QuotationStatus status)
            : id(id),
            quotationCode(quotationCode),
            bookingId(bookingId),
            subtotal(subtotal),
            discount(discount),
            tax(tax),
            total(total),
            status(status) {}
    
    // Getters
    int getId() const { return id; }
    const string& getQuotationCode() const { return quotationCode; }
    int getBookingId() const { return bookingId; }
    double getSubtotal() const { return subtotal; }
    double getDiscount() const { return discount; }
    double getTax() const { return tax; }
    double getTotal() const { return total; }
    QuotationStatus getStatus() const { return status; }

    // Setters
    void setBookingId(int newBookingId) { bookingId = newBookingId; }
    void setSubtotal(double newSubtotal) { subtotal = newSubtotal; }
    void setDiscount(double newDiscount) { discount = newDiscount; }
    void setTax(double newTax) { tax = newTax; }
    void setTotal(double newTotal) { total = newTotal; }
    void setStatus(QuotationStatus newStatus) { status = newStatus; }
};