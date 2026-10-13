// Kiểm thử lõi nghiệp vụ TrongDongPalace (không cần Qt Widgets).
#include "services/palace_service.hpp"
#include "storage/json_storage.hpp"
#include "utils/helpers.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace fs = filesystem;
using namespace tdp;

static int g_failed = 0, g_total = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_total;                                                               \
        if (cond) {                                                              \
            cout << "  OK  " << #cond << "\n";                              \
        } else {                                                                 \
            ++g_failed;                                                          \
            cout << "  FAIL " << #cond << " (dòng " << __LINE__ << ")\n";   \
        }                                                                        \
    } while (0)

static fs::path makeTempDir(const string& tag) {
    auto dir = fs::temp_directory_path() / ("tdp_test_" + tag + to_string(time(nullptr)));
    fs::remove_all(dir);
    fs::create_directories(dir);
    return dir;
}

static DateTime dt(const string& s) { return *DateTime::parse(s); }

// Quy trình đầy đủ: khách hàng -> yêu cầu -> báo giá -> booking -> hợp đồng ->
// thanh toán -> vận hành -> hoàn tất. Trả về các id để test tái sử dụng.
struct Flow {
    string customerId, requestId, quotationId, bookingId, contractId, eventId;
};

static Flow runFullFlow(PalaceService& svc, const string& tag, const string& hallId,
                        DateTime s = dt("2027-03-20 18:00"), DateTime e = dt("2027-03-20 22:00")) {
    static int phoneSeq = 10000000;
    Flow f;
    auto r1 = svc.login("kinhdoanh", "123456");
    assert(r1.ok);
    f.customerId = svc.addCustomer("Khách " + tag, "09" + to_string(++phoneSeq),
                                   "kh" + tag + "@test.com", "Hà Nội", "").id;
    CHECK(!f.customerId.empty());
    f.requestId = svc.addRequest(f.customerId, EventType::TiecCuoi, "Tiệc " + tag, s, e, 200, "").id;
    CHECK(!f.requestId.empty());
    f.quotationId = svc.createQuotation(f.requestId, "").id;
    CHECK(!f.quotationId.empty());
    CHECK(svc.addQuotationLine(f.quotationId, "SVC-0002", 200).ok);  // 200 * 450000 = 90,000,000
    CHECK(svc.setQuotationStatus(f.quotationId, QuotationStatus::DaGui).ok);
    CHECK(svc.setQuotationStatus(f.quotationId, QuotationStatus::DaChapNhan).ok);
    f.bookingId = svc.createBooking(f.quotationId, hallId, s, e, 200).id;
    CHECK(!f.bookingId.empty());
    CHECK(svc.confirmBooking(f.bookingId).ok);
    f.contractId = svc.createContract(f.bookingId).id;
    CHECK(!f.contractId.empty());
    return f;
}

int main() {
    cout << "== 1. Lưu và đọc lại JSON ==\n";
    {
        auto dir = makeTempDir("persist");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        Flow f;
        {
            PalaceService svc(storage);
            f = runFullFlow(svc, "P1", "HAL-0001");
        }
        // Mở lại "chương trình" mới: dữ liệu phải còn nguyên.
        JsonStorage storage2(dir / "data", dir / "backup");
        storage2.load();
        PalaceService svc2(storage2);
        CHECK(svc2.findCustomer(f.customerId) != nullptr);
        CHECK(svc2.findRequest(f.requestId) != nullptr);
        CHECK(svc2.findQuotation(f.quotationId) != nullptr);
        CHECK(svc2.findBooking(f.bookingId) != nullptr);
        CHECK(svc2.findContract(f.contractId) != nullptr);
        CHECK(svc2.findEventByBooking(f.bookingId) != nullptr);
        CHECK(fs::exists(dir / "data" / "users.json"));
        CHECK(fs::exists(dir / "data" / "customers.json"));
        CHECK(fs::exists(dir / "data" / "venues.json"));
        CHECK(fs::exists(dir / "data" / "events.json"));
        CHECK(fs::exists(dir / "data" / "transactions.json"));
    }

    cout << "== 2. Sửa dữ liệu được ghi xuống file ==\n";
    {
        auto dir = makeTempDir("update");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("kinhdoanh", "123456");
        string id = svc.addCustomer("Tên cũ", "0912111222", "", "", "").id;
        CHECK(svc.updateCustomer(id, "Tên mới", "0912111222", "x@y.com", "", "").ok);
        ifstream in(dir / "data" / "customers.json");
        stringstream ss; ss << in.rdbuf();
        CHECK(ss.str().find("Tên mới") != string::npos);
    }

    cout << "== 3. ID trùng bị từ chối ==\n";
    {
        auto dir = makeTempDir("dup");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto id1 = svc.addCustomer("A", "0999000001", "", "", "");
        auto id2 = svc.addCustomer("B", "0999000001", "", "", "");
        CHECK(!id2.ok);  // trùng số điện thoại
    }

    cout << "== 4. Booking trùng sảnh chồng lấn thời gian ==\n";
    {
        auto dir = makeTempDir("overlap");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        DateTime s = dt("2027-06-01 18:00"), e = dt("2027-06-01 22:00");
        auto b1 = runFullFlow(svc, "O1", "HAL-0001", s, e);
        // Cùng sảnh, cùng giờ -> phải thất bại.
        auto r = svc.addRequest(b1.customerId, EventType::HoiNghi, "Trùng", s, e, 50, "");
        CHECK(r.ok);
        auto q = svc.createQuotation(r.id, "");
        CHECK(q.ok);
        CHECK(svc.addQuotationLine(q.id, "SVC-0003", 50).ok);
        auto bk = svc.createBooking(q.id, "HAL-0001", s, e, 50);
        CHECK(!bk.ok);
        // Sảnh khác thì được.
        auto bk2 = svc.createBooking(q.id, "HAL-0002", s, e, 50);
        CHECK(bk2.ok);
        // Hủy booking 1 thì sảnh 1 lại trống.
        CHECK(svc.cancelBooking(b1.bookingId, "test").ok);
        auto bk3 = svc.createBooking(q.id, "HAL-0001", s, e, 50);
        CHECK(bk3.ok);
    }

    cout << "== 5. Tính toán báo giá và chiết khấu ==\n";
    {
        auto dir = makeTempDir("quote");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("kinhdoanh", "123456");
        string cus = svc.addCustomer("C", "0933111222", "", "", "").id;
        string req = svc.addRequest(cus, EventType::TiecCuoi, "T", dt("2027-08-01 18:00"),
                                         dt("2027-08-01 22:00"), 250, "").id;
        string q = svc.createQuotation(req, "").id;
        CHECK(svc.addQuotationLine(q, "SVC-0001", 1).ok);   // 25,000,000
        CHECK(svc.addQuotationLine(q, "SVC-0002", 250).ok); // 112,500,000
        const Quotation* quo = svc.findQuotation(q);
        CHECK(quo != nullptr);
        CHECK(quo->subtotal() == 137500000);
        int expectDisc = discountPercentForSubtotal(137500000);
        CHECK(quo->discountPercent() == expectDisc);
        int64_t expectTotal = quo->subtotal() - quo->discountAmount();
        CHECK(quo->total() == expectTotal);
        // Cập nhật số lượng phải tính lại tổng.
        CHECK(svc.updateQuotationLine(q, 1, 200).ok);
        quo = svc.findQuotation(q);
        CHECK(quo->subtotal() == 25'000'000 + 90'000'000);
    }

    cout << "== 6. Thanh toán vượt công nợ bị từ chối ==\n";
    {
        auto dir = makeTempDir("pay");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "PAY", "HAL-0001");
        svc.logout();
        CHECK(svc.login("ketoan", "123456").ok);
        const Contract* c = svc.findContract(f.contractId);
        CHECK(c != nullptr);
        CHECK(svc.recordPayment(f.contractId, c->totalAmount(), PaymentMethod::ChuyenKhoan, "").ok);
        CHECK(!svc.recordPayment(f.contractId, 1, PaymentMethod::TienMat, "").ok);  // vượt số dư
        CHECK(svc.paidAmount(f.contractId) == c->totalAmount());
        CHECK(svc.outstanding(f.contractId) == 0);
        // Số tiền <= 0 bị từ chối.
        string c2 = runFullFlow(svc, "PAY2", "HAL-0002").contractId;
        CHECK(!svc.recordPayment(c2, 0, PaymentMethod::TienMat, "").ok);
        CHECK(!svc.recordPayment(c2, -5000, PaymentMethod::TienMat, "").ok);
    }

    cout << "== 7. Chuyển trạng thái nghiệp vụ ==\n";
    {
        auto dir = makeTempDir("state");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "ST", "HAL-0001");
        // Hợp đồng từ booking đã hủy phải thất bại.
        string req2 = svc.addRequest(f.customerId, EventType::Khac, "T2", dt("2027-09-01 18:00"),
                                          dt("2027-09-01 22:00"), 100, "").id;
        string q2 = svc.createQuotation(req2, "").id;
        CHECK(svc.addQuotationLine(q2, "SVC-0001", 1).ok);
        auto bk2 = svc.createBooking(q2, "HAL-0002", dt("2027-09-01 18:00"), dt("2027-09-01 22:00"), 100);
        CHECK(bk2.ok);
        CHECK(svc.cancelBooking(bk2.id, "khách đổi ý").ok);
        CHECK(!svc.createContract(bk2.id).ok);
        // Booking chưa xác nhận thì không lập được hợp đồng.
        auto bk3 = svc.createBooking(q2, "HAL-0003", dt("2027-09-02 18:00"), dt("2027-09-02 22:00"), 100);
        if (bk3.ok) {
            CHECK(!svc.createContract(bk3.id).ok);  // chưa xác nhận
            CHECK(svc.confirmBooking(bk3.id).ok);
            CHECK(svc.createContract(bk3.id).ok);
        }
    }

    cout << "== 8. Phân quyền ==\n";
    {
        auto dir = makeTempDir("perm");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(!svc.can(Action::ManageCustomers));  // chưa đăng nhập
        CHECK(svc.login("kinhdoanh", "123456").ok);
        CHECK(svc.can(Action::ManageCustomers));
        CHECK(!svc.can(Action::ManagePayments));
        CHECK(!svc.can(Action::ManageUsers));
        CHECK(svc.addCustomer("X", "0988123456", "", "", "").ok);  // kinhdoanh được phép
        svc.logout();
        CHECK(svc.login("ketoan", "123456").ok);
        CHECK(!svc.can(Action::ManageCustomers));
        CHECK(svc.can(Action::ManagePayments));
        CHECK(!svc.addCustomer("Y", "0988123457", "", "", "").ok);  // kế toán không được thêm khách
        CHECK(svc.login("dieuphoi", "123456").ok);
        svc.logout();
        CHECK(!svc.can(Action::ManagePayments));
        CHECK(!svc.login("dieuphoi", "saimatkhau").ok);
        // Chỉ kiểm tra quyền ở tầng service, kể cả khi UI cố gọi trực tiếp.
        CHECK(svc.require(Action::ManagePayments).ok == false);
    }

    cout << "== 9. Hoàn tất sự kiện khi còn sự cố ==\n";
    {
        auto dir = makeTempDir("ops");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "OPS", "HAL-0001");
        svc.logout();
        CHECK(svc.login("dieuphoi", "123456").ok);
        const Event* ev = svc.findEventByBooking(f.bookingId);
        CHECK(ev != nullptr);
        string taskId = svc.addTask(ev->id(), "Chuẩn bị sảnh", "Anh Bình", dt("2027-03-19 17:00")).id;
        CHECK(!taskId.empty());
        string inc = svc.addIncident(ev->id(), "Mất điện thử nghiệm").id;
        CHECK(!inc.empty());
        CHECK(!svc.completeEvent(ev->id()).ok);  // còn sự cố mở
        CHECK(svc.resolveIncident(inc, "Đã chuyển điện dự phòng", IncidentStatus::DaXuLy).ok);
        CHECK(svc.completeEvent(ev->id()).ok);
    }

    cout << "== 10. JSON sai định dạng không làm mất dữ liệu ==\n";
    {
        auto dir = makeTempDir("corrupt");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        {
            PalaceService svc(storage);
            runFullFlow(svc, "CORRUPT", "HAL-0001");
        }
        // Phá file customers.json.
        { ofstream out(dir / "data" / "customers.json"); out << "{ json hỏng !!!"; }
        JsonStorage storage2(dir / "data", dir / "backup");
        bool threw = false;
        try {
            storage2.load();
        } catch (const StorageError&) {
            threw = true;
        }
        CHECK(threw);  // phải ném lỗi thay vì ghi đè/xóa
        // File hỏng vẫn còn nguyên để phục hồi thủ công.
        ifstream in(dir / "data" / "customers.json");
        stringstream ss; ss << in.rdbuf();
        CHECK(ss.str().find("json hỏng") != string::npos);
    }

    cout << "\n========================================\n";
    cout << "Tổng: " << g_total << " | Đạt: " << (g_total - g_failed) << " | Lỗi: " << g_failed << "\n";
    cout << "========================================\n";
    return g_failed == 0 ? 0 : 1;
}
