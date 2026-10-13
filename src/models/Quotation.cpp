#include "models/Quotation.hpp"
#include "utils/helpers.hpp"

using namespace std;

namespace tdp {

QuotationLine::QuotationLine(string serviceId, string nameSnapshot, int quantity, int64_t unitPrice)
    : serviceId_(move(serviceId)), nameSnapshot_(move(nameSnapshot)), quantity_(quantity), unitPrice_(unitPrice) {}

nlohmann::json QuotationLine::toJson() const {
    return {{"serviceId", serviceId_},
            {"nameSnapshot", nameSnapshot_},
            {"quantity", quantity_},
            {"unitPrice", unitPrice_},
            {"amount", amount()}};
}

QuotationLine QuotationLine::fromJson(const nlohmann::json& j) {
    return QuotationLine(j.at("serviceId").get<string>(), j.value("nameSnapshot", ""), j.value("quantity", 1),
                         j.value("unitPrice", 0));
}

Quotation::Quotation(string id, string requestId, string customerId, string note)
    : id_(move(id)), requestId_(move(requestId)), customerId_(move(customerId)), note_(move(note)) {
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
    Quotation q(j.at("id").get<string>(), j.at("requestId").get<string>(),
                j.at("customerId").get<string>(), j.value("note", ""));
    if (j.contains("lines")) {
        for (const auto& line : j["lines"]) q.lines_.push_back(QuotationLine::fromJson(line));
    }
    q.status_ = quotationStatusFromString(j.value("status", "Nhat"));
    q.recalc();
    return q;
}

}  // namespace tdp
