#include "services/palace_service.hpp"

#include <algorithm>
#include <sstream>

using namespace std;

namespace tdp {

int PalaceService::refreshEventStatuses(DateTime now) {
    int n = 0;
    for (auto& e : mut().events) {
        if (e.status() == EventStatus::Huy) continue;
        EventStatus want = eventTimeStatus(e.start(), e.end(), now);
        if (want != e.status()) {
            e.setStatus(want);
            ++n;
        }
    }
    return n;
}

OpResult PalaceService::recordPayment(const string& bookingId, PaymentType type, int64_t amount,
                                      PaymentMethod method, DateTime paidAt, const string& note) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    const Booking* b = findBooking(bookingId);
    if (!b) return OpResult::fail("Booking không tồn tại.");
    if (b->status() == BookingStatus::DaHuy) return OpResult::fail("Không thu tiền cho booking đã hủy.");
    if (amount <= 0) return OpResult::fail("Nhân viên phải nhập giá trị khoản thanh toán (> 0).");
    if (trim(note).empty()) return OpResult::fail("Ghi chú thanh toán là bắt buộc.");
    if (!paidAt.valid()) return OpResult::fail("Ngày thanh toán không hợp lệ.");
    if (DateTime::now() < paidAt)
        return OpResult::fail("Ngày thanh toán không được trong tương lai.");
    const User* actor = currentUser();
    auto* slot = (Payment*)nullptr;
    for (auto& p : mut().payments) {
        if (p.bookingId() == bookingId && p.type() == type && p.status() == PaymentStatus::ChuaThanhToan) {
            slot = &p;
            break;
        }
    }
    string id;
    if (slot) {
        slot->setAmount(amount);
        slot->setMethod(method);
        slot->setPaidAt(paidAt);
        slot->setNote(trim(note));
        slot->setStatus(PaymentStatus::DaThanhToan);
        id = slot->id();
    } else {
        vector<string> ids;
        for (const auto& x : data().payments) ids.push_back(x.id());
        id = nextId("PAY-", ids);
        mut().payments.push_back(Payment(id, bookingId, type, amount, paidAt, method,
                                         PaymentStatus::DaThanhToan, trim(note),
                                         actor ? actor->username() : ""));
    }
    applyPaymentCompleted(bookingId, type);
    return persist("Đã ghi nhận " + toString(type) + " " + formatMoney(amount) + " cho booking " + bookingId +
                       ".",
                   id);
}

OpResult PalaceService::updatePayment(const string& id, PaymentType type, int64_t amount,
                                      PaymentMethod method, DateTime paidAt, const string& note) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    auto* p = findById(mut().payments, id);
    if (!p) return OpResult::fail("Không tìm thấy khoản thanh toán.");
    if (amount <= 0) return OpResult::fail("Nhân viên phải nhập giá trị khoản thanh toán (> 0).");
    if (trim(note).empty()) return OpResult::fail("Ghi chú thanh toán là bắt buộc.");
    if (!paidAt.valid()) return OpResult::fail("Ngày thanh toán không hợp lệ.");
    if (DateTime::now() < paidAt)
        return OpResult::fail("Ngày thanh toán không được trong tương lai.");
    p->setType(type);
    p->setAmount(amount);
    p->setMethod(method);
    p->setPaidAt(paidAt);
    p->setNote(trim(note));
    p->setStatus(PaymentStatus::DaThanhToan);
    applyPaymentCompleted(p->bookingId(), type);
    return persist("Đã cập nhật khoản thanh toán " + id + ".", id);
}

void PalaceService::applyPaymentCompleted(const string& bookingId, PaymentType type) {
    auto* bb = findById(mut().bookings, bookingId);
    if (!bb || bb->status() == BookingStatus::DaHuy) return;
    if ((type == PaymentType::DatCoc1 || type == PaymentType::DatCoc2) &&
        bb->status() == BookingStatus::ChoCoc) {
        bb->setStatus(BookingStatus::DaCoc);
    }
    if (type == PaymentType::TatToan && bb->status() != BookingStatus::HoanTat) {
        bb->setStatus(BookingStatus::HoanTat);
    }
    // Thu coc/tat toan xong -> tu dong tao su kien tuong ung.
    if (findEventByBooking(bookingId)) return;
    vector<string> eids;
    for (const auto& x : data().events) eids.push_back(x.id());
    string eid = nextId("EVT-", eids);
    const EventRequest* req = findRequest(bb->requestId());
    mut().events.push_back(Event(eid, bookingId, bb->customerId(), bb->hallId(), bb->start(), bb->end(),
                                 EventStatus::SapDienRa, req ? req->notes() : ""));
}

OpResult PalaceService::deletePayment(const string& id) {
    auto r = require(Action::ManagePayments);
    if (!r.ok) return r;
    auto* p = findById(mut().payments, id);
    if (!p) return OpResult::fail("Không tìm thấy khoản thanh toán.");
    if (p->status() == PaymentStatus::DaThanhToan)
        return OpResult::fail("Khoản đã thanh toán không được xóa.");
    auto& items = mut().payments;
    items.erase(remove_if(items.begin(), items.end(), [&](const Payment& x) { return x.id() == id; }),
                items.end());
    return persist("Đã xóa khoản thanh toán.", id);
}

int64_t PalaceService::paidAmount(const string& bookingId) const {
    int64_t sum = 0;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId && p.status() == PaymentStatus::DaThanhToan && p.amount() > 0)
            sum += p.amount();
    return sum;
}

vector<Payment> PalaceService::paymentsOf(const string& bookingId) const {
    vector<Payment> out;
    for (const auto& p : data().payments)
        if (p.bookingId() == bookingId) out.push_back(p);
    return out;
}

string PalaceService::paymentReceipt(const string& paymentId) const {
    const Payment* p = findPayment(paymentId);
    if (!p) return "Không tìm thấy phiếu thu.";
    const Booking* b = findBooking(p->bookingId());
    const Customer* c = b ? findCustomer(b->customerId()) : nullptr;
    const Hall* h = b ? findHall(b->hallId()) : nullptr;
    ostringstream ss;
    ss << "========== PHIẾU THU ==========\n";
    ss << "Số phiếu: " << p->id() << "\n";
    ss << "Booking: " << p->bookingId() << "\n";
    if (c) ss << "Khách hàng: " << c->name() << " - " << c->phone() << "\n";
    if (h) ss << "Sảnh: " << h->name() << "\n";
    if (b) ss << "Tổng booking: " << formatMoney(b->finalTotal()) << "\n";
    ss << "Khoản thu: " << toString(p->type()) << "\n";
    ss << "Số tiền: " << formatMoney(p->amount()) << "\n";
    ss << "Hình thức: " << toString(p->method()) << "\n";
    ss << "Ngày thu: " << p->paidAt().toString() << "\n";
    ss << "Trạng thái: " << toString(p->status()) << "\n";
    ss << "Ghi chú: " << p->note() << "\n";
    ss << "Người thu: " << p->recordedBy() << "\n";
    ss << "==============================";
    return ss.str();
}

vector<Event> PalaceService::allEvents() const { return data().events; }

}  // namespace tdp
