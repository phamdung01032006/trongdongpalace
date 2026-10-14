// Kiểm thử lõi nghiệp vụ TrongDongPalace (không cần Qt Widgets).
// Luồng chuẩn theo đặc tả: Yêu cầu -> Booking (giữ chỗ 72h) ->
// Thanh toán (đặt cọc) -> Sự kiện (trạng thái theo thời gian).
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

// Quy trình đầy đủ: khách hàng -> yêu cầu (+dịch vụ) -> booking (giữ chỗ) ->
// đặt cọc -> sự kiện tự tạo. Trả về các id để test tái sử dụng.
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
    f.requestId =
        svc.addRequest(f.customerId, EventType::TiecCuoi, "Tiệc " + tag, "Khách " + tag,
                       "09" + to_string(phoneSeq), hallId, s, e, 200, "",
                       {{"FOD01-P", 0}, {"DEC04-P", 0}})
            .id;
    CHECK(!f.requestId.empty());
    f.bookingId = svc.createBooking(f.requestId, hallId, s, e, 200, 0, 0).id;
    CHECK(!f.bookingId.empty());
    // Khoản đặt cọc tự động tạo, giá trị để trống.
    CHECK(svc.paymentsOf(f.bookingId).size() == 1);
    CHECK(svc.recordPayment(f.bookingId, PaymentType::DatCoc1, 20000000, PaymentMethod::ChuyenKhoan,
                            DateTime::now(), "Đặt cọc giữ chỗ")
              .ok);
    const Booking* b = svc.findBooking(f.bookingId);
    CHECK(b != nullptr && b->status() == BookingStatus::DaCoc);
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
            f = runFullFlow(svc, "P1", "HAL-0004");
        }
        JsonStorage storage2(dir / "data", dir / "backup");
        storage2.load();
        PalaceService svc2(storage2);
        CHECK(svc2.findCustomer(f.customerId) != nullptr);
        CHECK(svc2.findRequest(f.requestId) != nullptr);
        CHECK(svc2.findBooking(f.bookingId) != nullptr);
        CHECK(svc2.findEventByBooking(f.bookingId) != nullptr);
        CHECK(svc2.paymentsOf(f.bookingId).size() == 1);
        CHECK(fs::exists(dir / "data" / "users.json"));
        CHECK(fs::exists(dir / "data" / "customers.json"));
        CHECK(fs::exists(dir / "data" / "venues.json"));
        CHECK(fs::exists(dir / "data" / "events.json"));
        CHECK(fs::exists(dir / "data" / "transactions.json"));
    }

    cout << "== 2. Seed danh mục đặc tả ==\n";
    {
        auto dir = makeTempDir("seed");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(svc.db().services.size() == 100);
        CHECK(svc.db().halls.size() == 6);
        const ServiceItem* fod = svc.findServiceByCode("FOD01-P");
        CHECK(fod != nullptr && fod->unitPrice() == 4500000);
        const Hall* ballroom = svc.findHall("HAL-0006");
        CHECK(ballroom != nullptr && ballroom->rentalPrice() == 50000000);
        CHECK(eventTypeMeta(EventType::TiecCuoi).code == string("EVT01"));
        CHECK(customerViewOfRequest(RequestStatus::Cho) == "Đang xét duyệt");
        CHECK(customerViewOfRequest(RequestStatus::DaChot) == "Hoàn thành");
        CHECK(customerViewOfRequest(RequestStatus::BiHuy) == "Bị hủy");
    }

    cout << "== 3. Chi phí dự kiến = tiền sảnh + dịch vụ ==\n";
    {
        auto dir = makeTempDir("estimate");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("kinhdoanh", "123456");
        string cus = svc.addCustomer("C", "0933111222", "", "", "").id;
        // 200 khách: FOD01-P 20 bàn x 4.5M = 90M + DEC04-P 25M + sảnh Large 18M.
        string req =
            svc.addRequest(cus, EventType::TiecCuoi, "T", "C", "0933111222", "HAL-0004",
                           dt("2027-08-01 18:00"), dt("2027-08-01 22:00"), 200, "",
                           {{"FOD01-P", 0}, {"DEC04-P", 0}})
                .id;
        const EventRequest* r = svc.findRequest(req);
        CHECK(r != nullptr);
        CHECK(r->servicesTotal() == 90000000 + 25000000);
        CHECK(svc.estimateRequest(*r) == 90000000 + 25000000 + 18000000);
        // Số khách < 10 bị từ chối; ngày quá khứ bị từ chối.
        CHECK(!svc.addRequest(cus, EventType::TiecCuoi, "T", "C", "0933111222", "", dt("2027-08-01 18:00"),
                              dt("2027-08-01 22:00"), 5, "", {})
                   .ok);
        CHECK(!svc.addRequest(cus, EventType::TiecCuoi, "T", "C", "0933111222", "", dt("2020-01-01 18:00"),
                              dt("2020-01-01 22:00"), 50, "", {})
                   .ok);
    }

    cout << "== 4. Booking trùng sảnh + hết hạn giữ chỗ 72h ==\n";
    {
        auto dir = makeTempDir("overlap");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        DateTime s = dt("2027-06-01 18:00"), e = dt("2027-06-01 22:00");
        auto b1 = runFullFlow(svc, "O1", "HAL-0004", s, e);
        // Cùng sảnh, cùng giờ -> phải thất bại (đang giữ chỗ/đã cọc).
        auto r = svc.addRequest(b1.customerId, EventType::HoiNghiKhachHang, "Trùng", "K", "0912000000",
                                "", s, e, 50, "", {});
        CHECK(r.ok);
        CHECK(!svc.createBooking(r.id, "HAL-0004", s, e, 50, 0, 0).ok);
        // Sảnh khác thì được.
        auto bk2 = svc.createBooking(r.id, "HAL-0005", s, e, 50, 0, 0);
        CHECK(bk2.ok);
        // Booking chưa cọc: quét sau 73h -> tự hủy, sảnh được giải phóng.
        CHECK(svc.sweepExpiredHolds(s.plusHours(73)) == 1);
        CHECK(svc.findBooking(bk2.id)->status() == BookingStatus::DaHuy);
        auto r2 = svc.addRequest(b1.customerId, EventType::HoiNghiKhachHang, "Moi", "K", "0912000000",
                                 "", s, e, 50, "", {});
        auto bk3 = svc.createBooking(r2.id, "HAL-0005", s, e, 50, 0, 0);
        CHECK(bk3.ok);
        // Booking đã cọc của b1 không bị quét.
        CHECK(svc.findBooking(b1.bookingId)->status() == BookingStatus::DaCoc);
    }

    cout << "== 5. Chiết khấu vượt 10% cần Quản lý duyệt ==\n";
    {
        auto dir = makeTempDir("discount");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("kinhdoanh", "123456");
        string cus = svc.addCustomer("D", "0934111222", "", "", "").id;
        string req = svc.addRequest(cus, EventType::TiecCuoi, "T", "D", "0934111222", "HAL-0004",
                                    dt("2027-08-02 18:00"), dt("2027-08-02 22:00"), 200, "", {}).id;
        string bk = svc.createBooking(req, "HAL-0004", dt("2027-08-02 18:00"), dt("2027-08-02 22:00"),
                                      200, 15, 0)
                        .id;
        const Booking* b = svc.findBooking(bk);
        CHECK(b != nullptr && b->discountNeedsApproval() && !b->discountApproved());
        CHECK(!svc.approveDiscount(bk).ok);  // sales không duyệt được
        svc.logout();
        CHECK(svc.login("admin", "123456").ok);
        CHECK(svc.approveDiscount(bk).ok);
        CHECK(svc.findBooking(bk)->discountApproved());
    }

    cout << "== 6. Thanh toán: bắt buộc ghi chú, phiếu thu, tất toán ==\n";
    {
        auto dir = makeTempDir("pay");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "PAY", "HAL-0004");
        CHECK(!svc.recordPayment(f.bookingId, PaymentType::DatCoc2, 5000000, PaymentMethod::TienMat,
                                 DateTime::now(), "")
                   .ok);  // thiếu ghi chú
        CHECK(!svc.recordPayment(f.bookingId, PaymentType::TatToan, 50000000, PaymentMethod::TienMat,
                                 DateTime::now().plusHours(5), "Tất toán")
                   .ok);  // ngày tương lai
        CHECK(svc.recordPayment(f.bookingId, PaymentType::TatToan, 50000000, PaymentMethod::TienMat,
                                DateTime::now(), "Tất toán đợt cuối")
                  .ok);
        CHECK(svc.findBooking(f.bookingId)->status() == BookingStatus::HoanTat);
        string receipt = svc.paymentReceipt(svc.paymentsOf(f.bookingId).back().id());
        CHECK(receipt.find("PHIẾU THU") != string::npos);
        CHECK(receipt.find("Tất toán") != string::npos);
    }

    cout << "== 7. Yêu cầu Hoàn thành -> Booking, lịch sử 10 ==\n";
    {
        auto dir = makeTempDir("state");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        svc.login("kinhdoanh", "123456");
        string cus = svc.addCustomer("S", "0935111222", "", "", "").id;
        string req = svc.addRequest(cus, EventType::SinhNhat, "SN", "S", "0935111222", "HAL-0002",
                                    dt("2027-09-01 18:00"), dt("2027-09-01 22:00"), 80, "", {}).id;
        CHECK(svc.updateRequestStatus(req, RequestStatus::DangTuVan).ok);
        CHECK(svc.updateRequestStatus(req, RequestStatus::HoanThanh).ok);  // tự sang Booking
        CHECK(svc.findRequest(req)->status() == RequestStatus::DaChot);
        // Lịch sử chỉ giữ 10 yêu cầu gần nhất.
        for (int i = 0; i < 12; ++i) {
            string q = svc.addRequest(cus, EventType::SinhNhat, "SN" + to_string(i), "S", "0935111222",
                                      "", dt("2027-10-01 18:00"), dt("2027-10-01 22:00"), 20, "", {}).id;
            svc.updateRequestStatus(q, RequestStatus::BiHuy);
        }
        CHECK(svc.requestHistory().size() == 10);
        CHECK(svc.countNewRequests() == 0);
    }

    cout << "== 8. Phân quyền 4 actor ==\n";
    {
        auto dir = makeTempDir("perm");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(!svc.can(Action::ManageCustomers));  // chưa đăng nhập
        CHECK(svc.login("khachhang", "123456").ok);
        CHECK(svc.can(Action::ManageRequests));
        CHECK(!svc.can(Action::ManageBookings));
        CHECK(!svc.can(Action::ManageEvents));
        svc.logout();
        CHECK(svc.login("kinhdoanh", "123456").ok);
        CHECK(svc.can(Action::ManageBookings) && svc.can(Action::ManagePayments));
        CHECK(!svc.can(Action::ManageVenues) && !svc.can(Action::ManageUsers));
        svc.logout();
        CHECK(svc.login("admin", "123456").ok);
        CHECK(svc.can(Action::ManageVenues) && svc.can(Action::ManageUsers));
        svc.logout();
        CHECK(svc.login("vanhanh", "123456").ok);
        CHECK(svc.can(Action::ManageEvents));
        CHECK(!svc.can(Action::ManageBookings) && !svc.can(Action::ManageRequests));
        svc.logout();
        CHECK(!svc.login("vanhanh", "saimatkhau").ok);
    }

    cout << "== 9. Khách hàng: tạo + hủy yêu cầu của mình ==\n";
    {
        auto dir = makeTempDir("cust");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(svc.login("khachhang", "123456").ok);
        string req =
            svc.addRequest("", EventType::TiecCuoi, "Đám cưới nhà An", "", "", "HAL-0003",
                           dt("2027-11-01 18:00"), dt("2027-11-01 22:00"), 150, "", {{"MED07", 0}}).id;
        CHECK(!req.empty());
        CHECK(svc.requestsForCustomer("CUS-0002").size() == 1);
        CHECK(svc.cancelOwnRequest(req).ok);
        CHECK(customerViewOfRequest(svc.findRequest(req)->status()) == "Bị hủy");
        // Đăng ký mới: đủ hồ sơ -> tự tạo Customer liên kết; thiếu/SĐT trùng bị từ chối.
        svc.logout();
        CHECK(!svc.registerUser("thieu", "123456", "123456", "", "0900111333", "t@t.com", "").ok);
        CHECK(!svc.registerUser("trung", "123456", "123456", "Trùng", "0987654321", "t@t.com", "").ok);
        CHECK(svc.registerUser("khachmoi", "123456", "123456", "Khách Mới", "0900111333",
                               "moi@test.com", "Hà Nội")
                  .ok);
        CHECK(svc.login("khachmoi", "123456").ok);
        CHECK(svc.currentUser()->role() == Role::Customer);
        const Customer* prof = svc.findCustomer(svc.currentUser()->customerId());
        CHECK(prof != nullptr && prof->phone() == "0900111333" && prof->email() == "moi@test.com");
        // Khách sửa SĐT trên form yêu cầu -> đồng bộ ngược về hồ sơ.
        string req2 =
            svc.addRequest("", EventType::TiecCuoi, "T2", "Khách Mới", "0900111444", "", dt("2027-11-02 18:00"),
                           dt("2027-11-02 22:00"), 50, "", {})
                .id;
        CHECK(!req2.empty());
        CHECK(svc.findCustomer(svc.currentUser()->customerId())->phone() == "0900111444");
    }

    cout << "== 10. Sảnh Bảo trì + trạng thái sự kiện theo thời gian ==\n";
    {
        auto dir = makeTempDir("ops");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        auto f = runFullFlow(svc, "OPS", "HAL-0004");
        svc.logout();
        CHECK(svc.login("admin", "123456").ok);
        CHECK(svc.updateHall("HAL-0004", "Sảnh Large", 300, 18000000, HallStatus::BaoTri).ok);
        string reason;
        CHECK(!svc.hallAvailable("HAL-0004", dt("2028-01-01 18:00"), dt("2028-01-01 22:00"), "", reason));
        CHECK(svc.login("vanhanh", "123456").ok);
        CHECK(svc.allEvents().size() == 1);
        // Sự kiện 2027 chưa tới -> Sắp diễn ra; giả lập sau khi kết thúc -> Đã kết thúc.
        CHECK(svc.findEvent(f.eventId)->status() == EventStatus::SapDienRa);
        CHECK(svc.refreshEventStatuses(dt("2027-03-20 19:00")) == 1);
        CHECK(svc.findEvent(f.eventId)->status() == EventStatus::DangDienRa);
        CHECK(svc.refreshEventStatuses(dt("2027-04-01 00:00")) == 1);
        CHECK(svc.findEvent(f.eventId)->status() == EventStatus::DaKetThuc);
    }

    cout << "== 11. JSON sai định dạng không làm mất dữ liệu ==\n";
    {
        auto dir = makeTempDir("corrupt");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        {
            PalaceService svc(storage);
            runFullFlow(svc, "CORRUPT", "HAL-0004");
        }
        { ofstream out(dir / "data" / "customers.json"); out << "{ json hỏng !!!"; }
        JsonStorage storage2(dir / "data", dir / "backup");
        bool threw = false;
        try {
            storage2.load();
        } catch (const StorageError&) {
            threw = true;
        }
        CHECK(threw);
        ifstream in(dir / "data" / "customers.json");
        stringstream ss;
        ss << in.rdbuf();
        CHECK(ss.str().find("json hỏng") != string::npos);
    }

    cout << "== 12. Nạp danh mục mẫu không trùng, giữ dữ liệu cũ ==" << endl;
    {
        auto dir = makeTempDir("catalog");
        JsonStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(svc.login("admin", "123456").ok);
        OpResult r1 = svc.importCatalog();
        CHECK(r1.ok);
        CHECK(svc.db().halls.size() == 6 && svc.db().services.size() == 100);
        // Sales không được nạp.
        svc.logout();
        CHECK(svc.login("kinhdoanh", "123456").ok);
        CHECK(!svc.importCatalog().ok);
    }

    cout << "\n========================================\n";
    cout << "Tổng: " << g_total << " | Đạt: " << (g_total - g_failed) << " | Lỗi: " << g_failed << "\n";
    cout << "========================================\n";
    return g_failed == 0 ? 0 : 1;
}
