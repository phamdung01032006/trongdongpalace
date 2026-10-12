#pragma once

#include "models/Enums.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tdp {

class QuotationLine {
public:
    QuotationLine() = default;
    QuotationLine(std::string serviceId, std::string nameSnapshot, int quantity, int64_t unitPrice);
    const std::string& serviceId() const { return serviceId_; }
    const std::string& nameSnapshot() const { return nameSnapshot_; }
    int quantity() const { return quantity_; }
    int64_t unitPrice() const { return unitPrice_; }
    int64_t amount() const { return static_cast<int64_t>(quantity_) * unitPrice_; }
    void setQuantity(int v) { quantity_ = v; }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    nlohmann::json toJson() const;
    static QuotationLine fromJson(const nlohmann::json& j);

private:
    std::string serviceId_, nameSnapshot_;
    int quantity_ = 1;
    int64_t unitPrice_ = 0;
};

class Quotation {
public:
    Quotation() = default;
    Quotation(std::string id, std::string requestId, std::string customerId, std::string note);

    const std::string& id() const { return id_; }
    const std::string& requestId() const { return requestId_; }
    const std::string& customerId() const { return customerId_; }
    const std::vector<QuotationLine>& lines() const { return lines_; }
    std::vector<QuotationLine>& lines() { return lines_; }
    int discountPercent() const { return discountPercent_; }
    int64_t discountAmount() const { return discountAmount_; }
    int64_t subtotal() const { return subtotal_; }
    int64_t total() const { return total_; }
    QuotationStatus status() const { return status_; }
    const std::string& note() const { return note_; }

    void setStatus(QuotationStatus v) { status_ = v; }
    void setNote(std::string v) { note_ = std::move(v); }
    void recalc();

    nlohmann::json toJson() const;
    static Quotation fromJson(const nlohmann::json& j);

private:
    std::string id_, requestId_, customerId_, note_;
    std::vector<QuotationLine> lines_;
    int discountPercent_ = 0;
    int64_t discountAmount_ = 0;
    int64_t subtotal_ = 0;
    int64_t total_ = 0;
    QuotationStatus status_ = QuotationStatus::Nhat;
};

}  // namespace tdp
