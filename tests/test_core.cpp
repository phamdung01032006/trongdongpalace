// Kiểm thử lõi nghiệp vụ TrongDongPalace (không cần Qt Widgets).
// Luồng chuẩn theo đặc tả: Yêu cầu -> Booking -> Thanh toán -> Sự kiện.
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

// Quy trình đầy đủ: khách hàng -> yêu cầu -> booking -> xác nhận ->
// thanh toán -> sự kiện tự tạo -> hoàn tất. Trả về các id để test tái sử dụng.
struct Flow {
    string customerId, requestId, bookingId, eventId;
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
    f.bookingId = svc.createBooking(f.requestId, hallId, s, e, 200).id;
    CHECK(!f.bookingId.empty());
    CHECK(svc.confirmBooking(f.bookingId).ok);
    const Event* ev = svc.findEventByBooking(f.bookingId);
    CHECK(ev != nullptr);
    if (ev) f.eventId = ev->id();
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
        CHECK(svc2.findBooking(f.bookingId) != nullptr);
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
        auto bk = svc.createBooking(r.id, "HAL-0001", s, e, 50);
        CHECK(!bk.ok);
        // Sảnh khác thì được.
        auto bk2 = svc.createBooking(r.id, "HAL-0002", s, e, 50);
        CHECK(bk2.ok);
        // Hủy booking 1 thì sảnh 1 lại trống.
        CHECK(svc.cancelBooking(b1.bookingId, "test").ok);
        auto bk3 = svc.createBooking(r.id, "HAL-0001", s, e, 50);
        CHECK(bk3.ok);
    }

    cout << "== 5. Ràng buộc sảnh và yêu cầu khi tạo booking ==\n";
    {
        auto dir = makeTempDir("validate");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("admin", "123456");
        CHECK(!svc.addHall("", 100).ok);        // tên sảnh bắt buộc
        CHECK(!svc.addHall("Sảnh X", 0).ok);    // sức chứa > 0
        string hall = svc.addHall("Sảnh Test", 100).id;
        CHECK(!hall.empty());
        svc.logout();
        svc.login("kinhdoanh", "123456");
        string cus = svc.addCustomer("C", "0933111222", "", "", "").id;
        string req = svc.addRequest(cus, EventType::TiecCuoi, "T", dt("2027-08-01 18:00"),
                                    dt("2027-08-01 22:00"), 80, "").id;
        CHECK(!svc.createBooking("REQ-KHONG-TON-TAI", hall,
                                 dt("2027-08-01 18:00"), dt("2027-08-01 22:00"), 80).ok);
        CHECK(!svc.createBooking(req, hall,
                                 dt("2027-08-01 18:00"), dt("2027-08-01 22:00"), 150).ok);  // vượt sức chứa
        CHECK(!svc.createBooking(req, "HAL-KHONG-TON-TAI",
                                 dt("2027-08-01 18:00"), dt("2027-08-01 22:00"), 80).ok);
        // Yêu cầu đã hủy thì không tạo được booking.
        CHECK(svc.updateRequestStatus(req, RequestStatus::Huy).ok);
        CHECK(!svc.createBooking(req, hall,
                                 dt("2027-08-01 18:00"), dt("2027-08-01 22:00"), 80).ok);
    }

    cout << "== 6. Ghi nhận thanh toán trên booking ==\n";
    {
        auto dir = makeTempDir("pay");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "PAY", "HAL-0001");
        // Ghi chú là bắt buộc.
        CHECK(!svc.recordPayment(f.bookingId, 1000000, PaymentMethod::ChuyenKhoan, "").ok);
        CHECK(!svc.recordPayment(f.bookingId, 0, PaymentMethod::TienMat, "Cọc").ok);
        CHECK(!svc.recordPayment(f.bookingId, -5000, PaymentMethod::TienMat, "Cọc").ok);
        CHECK(svc.recordPayment(f.bookingId, 10000000, PaymentMethod::ChuyenKhoan, "Đặt cọc lần 1").ok);
        CHECK(svc.recordPayment(f.bookingId, 5000000, PaymentMethod::TienMat, "Đặt cọc lần 2").ok);
        CHECK(svc.paidAmount(f.bookingId) == 15000000);
        CHECK(svc.paymentsOf(f.bookingId).size() == 2);
        // Booking đã hủy thì không thu tiền.
        CHECK(svc.cancelBooking(f.bookingId, "test").ok);
        CHECK(!svc.recordPayment(f.bookingId, 1000000, PaymentMethod::TienMat, "Thu thêm").ok);
    }

    cout << "== 7. Chuyển trạng thái nghiệp vụ ==\n";
    {
        auto dir = makeTempDir("state");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "ST", "HAL-0001");
        // Xác nhận 2 lần phải thất bại; hủy rồi xác nhận cũng thất bại.
        CHECK(!svc.confirmBooking(f.bookingId).ok);
        CHECK(svc.cancelBooking(f.bookingId, "khách đổi ý").ok);
        CHECK(!svc.confirmBooking(f.bookingId).ok);
        // Hủy booking thì sự kiện vận hành cũng bị hủy theo.
        const Event* ev = svc.findEventByBooking(f.bookingId);
        CHECK(ev != nullptr && ev->status() == EventStatus::Huy);
        // Booking nháp (chưa xác nhận) vẫn hủy được.
        string req2 = svc.addRequest(f.customerId, EventType::Khac, "T2", dt("2027-09-01 18:00"),
                                     dt("2027-09-01 22:00"), 100, "").id;
        auto bk2 = svc.createBooking(req2, "HAL-0002", dt("2027-09-01 18:00"), dt("2027-09-01 22:00"), 100);
        CHECK(bk2.ok);
        CHECK(svc.cancelBooking(bk2.id, "khách đổi ý").ok);
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
        CHECK(svc.can(Action::ManagePayments));  // sales ghi nhận thanh toán (đặc tả B.3)
        CHECK(!svc.can(Action::ManageUsers));
        CHECK(svc.addCustomer("X", "0988123456", "", "", "").ok);  // kinhdoanh được phép
        svc.logout();
        CHECK(svc.login("ketoan", "123456").ok);
        CHECK(!svc.can(Action::ManageCustomers));
        CHECK(svc.can(Action::ManagePayments));
        CHECK(!svc.addCustomer("Y", "0988123457", "", "", "").ok);  // kế toán không được thêm khách
        CHECK(svc.login("dieuphoi", "123456").ok);
        CHECK(svc.can(Action::ManageOperations));
        CHECK(!svc.can(Action::ManageBookings));
        svc.logout();
        CHECK(!svc.can(Action::ManagePayments));
        CHECK(!svc.login("dieuphoi", "saimatkhau").ok);
        // Chỉ kiểm tra quyền ở tầng service, kể cả khi UI cố gọi trực tiếp.
        CHECK(svc.require(Action::ManagePayments).ok == false);
    }

    cout << "== 9. Vận hành sự kiện ==\n";
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
        CHECK(svc.updateEventStatus(ev->id(), EventStatus::DangDienRa).ok);
        CHECK(svc.setEventNotes(ev->id(), "Âm thanh ánh sáng OK").ok);
        CHECK(svc.completeEvent(ev->id()).ok);
        CHECK(svc.findEvent(ev->id())->status() == EventStatus::HoanTat);
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
