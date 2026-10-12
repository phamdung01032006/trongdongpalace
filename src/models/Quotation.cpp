#include "models/Quotation.hpp"
#include "utils/helpers.hpp"

namespace tdp {

QuotationLine::QuotationLine(std::string serviceId, std::string nameSnapshot, int quantity, int64_t unitPrice)
    : serviceId_(std::move(serviceId)), nameSnapshot_(std::move(nameSnapshot)), quantity_(quantity), unitPrice_(unitPrice) {}

nlohmann::json QuotationLine::toJson() const {
    return {{"serviceId", serviceId_},
            {"nameSnapshot", nameSnapshot_},
            {"quantity", quantity_},
            {"unitPrice", unitPrice_},
            {"amount", amount()}};
}

QuotationLine QuotationLine::fromJson(const nlohmann::json& j) {
    return QuotationLine(j.at("serviceId").get<std::string>(), j.value("nameSnapshot", ""), j.value("quantity", 1),
                         j.value("unitPrice", 0));
}

Quotation::Quotation(std::string id, std::string requestId, std::string customerId, std::string note)
    : id_(std::move(id)), requestId_(std::move(requestId)), customerId_(std::move(customerId)), note_(std::move(note)) {
    recalc();
}

void Quotation::recalc() {
    subtotal_ = 0;
    for (const auto& line : lines_) subtotal_ += line.amount();
    discountPercent_ = discountPercentForSubtotal(subtotal_);
    discountAmount_ = subtotal_ * discountPercent_ / 100;
    total_ = subtotal_ - discountAmount_;
}

nlohmann::json Quotation::toJson() const {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& line : lines_) arr.push_back(line.toJson());
    return {{"id", id_},
            {"requestId", requestId_},
            {"customerId", customerId_},
            {"lines", arr},
            {"discountPercent", discountPercent_},
            {"discountAmount", discountAmount_},
            {"subtotal", subtotal_},
            {"total", total_},
            {"status", toString(status_)},
            {"note", note_}};
}

Quotation Quotation::fromJson(const nlohmann::json& j) {
    Quotation q(j.at("id").get<std::string>(), j.at("requestId").get<std::string>(),
                j.at("customerId").get<std::string>(), j.value("note", ""));
    if (j.contains("lines")) {
        for (const auto& line : j["lines"]) q.lines_.push_back(QuotationLine::fromJson(line));
    }
    q.status_ = quotationStatusFromString(j.value("status", "Nhat"));
    q.recalc();
    return q;
}

}  // namespace tdp
