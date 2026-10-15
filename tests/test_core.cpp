// Kiểm thử lõi nghiệp vụ TrongDongPalace (không cần Qt Widgets).
// Luồng chuẩn theo đặc tả: Yêu cầu -> Booking (giữ chỗ 72h) ->
// Thanh toán (đặt cọc) -> Sự kiện (trạng thái theo thời gian).
#include "services/palace_service.cpp"
#include "storage/text_storage.cpp"
#include "utils/helpers.cpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <type_traits>

using namespace std;

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

static filesystem::path makeTempDir(const string& tag) {
    auto dir = filesystem::temp_directory_path() / ("tdp_test_" + tag +
        to_string(chrono::steady_clock::now().time_since_epoch().count()));
    filesystem::create_directories(dir);
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
    cout << "== 1. Lưu và đọc lại TXT ==\n";
    {
        auto dir = makeTempDir("persist");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        Flow f;
        {
            PalaceService svc(storage);
            f = runFullFlow(svc, "P1", "HAL-0004");
        }
        TextStorage storage2(dir / "data", dir / "backup");
        storage2.load();
        PalaceService svc2(storage2);
        CHECK(svc2.findCustomer(f.customerId) != nullptr);
        CHECK(svc2.findRequest(f.requestId) != nullptr);
        CHECK(svc2.findBooking(f.bookingId) != nullptr);
        CHECK(svc2.findEventByBooking(f.bookingId) != nullptr);
        CHECK(svc2.paymentsOf(f.bookingId).size() == 1);
        CHECK(filesystem::exists(dir / "data" / "users.txt"));
        CHECK(filesystem::exists(dir / "data" / "customers.txt"));
        CHECK(filesystem::exists(dir / "data" / "halls.txt"));
        CHECK(filesystem::exists(dir / "data" / "services.txt"));
        CHECK(filesystem::exists(dir / "data" / "requests.txt"));
        CHECK(filesystem::exists(dir / "data" / "events.txt"));
        CHECK(filesystem::exists(dir / "data" / "bookings.txt"));
        CHECK(filesystem::exists(dir / "data" / "payments.txt"));
    }

    cout << "== 2. Seed danh mục đặc tả ==\n";
    {
        auto dir = makeTempDir("seed");
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        DateTime s = dt("2027-06-01 18:00"), e = dt("2027-06-01 22:00");
        auto b1 = runFullFlow(svc, "O1", "HAL-0004", s, e);
        // Cùng sảnh, cùng giờ -> phải thất bại (đang giữ chỗ/đã cọc).
        auto r = svc.addRequest(b1.customerId, EventType::HoiNghiKhachHang, "Trùng", "K", "0912000000",
                                "", s, e, 50, "", {});
        CHECK(r.ok);
        CHECK(!svc.createBooking(r.id, "HAL-0004", s, e, 50, 0, 0).ok);
        // Hai khoang giap nhau khong overlap.
        auto adjacent = svc.addRequest(b1.customerId, EventType::SinhNhat, "Giap gio", "K", "0912000000",
                                       "", e, e.plusHours(2), 50, "", {});
        CHECK(adjacent.ok);
        auto adjacentBooking = svc.createBooking(adjacent.id, "HAL-0004", e, e.plusHours(2), 50, 0, 0);
        CHECK(adjacentBooking.ok);
        CHECK(svc.cancelBooking(adjacentBooking.id, "Ket thuc test giap gio").ok);
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
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
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
        CHECK(svc.findBooking(f.bookingId)->status() == BookingStatus::DaCoc);
        // Chon nhan TatToan nhung tong thu chua du thi van chi la DaCoc.
        int64_t remaining = svc.findBooking(f.bookingId)->finalTotal() - svc.paidAmount(f.bookingId);
        CHECK(remaining > 0);
        CHECK(svc.recordPayment(f.bookingId, PaymentType::TatToan, remaining, PaymentMethod::TienMat,
                                DateTime::now(), "Thu đủ phần còn lại").ok);
        CHECK(svc.findBooking(f.bookingId)->status() == BookingStatus::DaThanhToan);
        string receipt = svc.paymentReceipt(svc.paymentsOf(f.bookingId).back().id());
        CHECK(receipt.find("PHIẾU THU") != string::npos);
        CHECK(receipt.find("Tất toán") != string::npos);
    }

    cout << "== 7. Yêu cầu Hoàn thành -> Booking, lịch sử 10 ==\n";
    {
        auto dir = makeTempDir("state");
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
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
        TextStorage storage(dir / "data", dir / "backup");
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

    cout << "== 11. TXT sai định dạng không làm mất dữ liệu ==\n";
    {
        auto dir = makeTempDir("corrupt");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        {
            PalaceService svc(storage);
            runFullFlow(svc, "CORRUPT", "HAL-0004");
        }
        { ofstream out(dir / "data" / "customers.txt"); out << "dong|txt|hong"; }
        TextStorage storage2(dir / "data", dir / "backup");
        bool threw = false;
        try {
            storage2.load();
        } catch (const StorageError&) {
            threw = true;
        }
        CHECK(threw);
        ifstream in(dir / "data" / "customers.txt");
        stringstream ss;
        ss << in.rdbuf();
        CHECK(ss.str() == "dong|txt|hong");
    }

    cout << "== 12. Nạp danh mục mẫu không trùng, giữ dữ liệu cũ ==" << endl;
    {
        auto dir = makeTempDir("catalog");
        TextStorage storage(dir / "data", dir / "backup");
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

    cout << "== 13. Xóa an toàn + cập nhật qua ô trạng thái ==" << endl;
    {
        auto dir = makeTempDir("delete");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(svc.login("admin", "123456").ok);
        // Xoa user: khong tu xoa, khong xoa quan ly cuoi.
        string u2 = svc.createUser("nv2", "123456", "NV Hai", Role::Sales).id;
        CHECK(!svc.deleteUser("USR-0001").ok);  // dang dung admin
        CHECK(svc.deleteUser(u2).ok);
        CHECK(!svc.updateUserRole("USR-0001", Role::Sales).ok);  // quan ly cuoi
        // Xoa dich vu dang dung -> tu choi; xoa dich vu thua -> duoc.
        string svcTmp = svc.addService("TMP01", "DV tạm", "VNĐ/gói", 1000, "Ẩm thực").id;
        CHECK(svc.deleteService(svcTmp).ok);
        string cus = svc.addCustomer("E", "0936111222", "", "", "").id;
        string req = svc.addRequest(cus, EventType::TiecCuoi, "T", "E", "0936111222", "", dt("2027-08-03 18:00"),
                                    dt("2027-08-03 22:00"), 50, "", {}).id;
        CHECK(!svc.deleteCustomer(cus).ok);  // da co yeu cau
        CHECK(!svc.deleteRequest("REQ-KHONG-CO").ok);
        string bk = svc.createBooking(req, "HAL-0004", dt("2027-08-03 18:00"), dt("2027-08-03 22:00"),
                                      200, 0, 0)
                        .id;
        CHECK(!svc.deleteRequest(req).ok);  // da chot booking
        // Sua giam gia qua o cap nhat + xoa booking nhap (keo theo khoan tu tao).
        CHECK(svc.updateBookingDiscount(bk, 5, 0).ok);
        CHECK(svc.deleteBooking(bk).ok);
        CHECK(svc.paymentsOf(bk).empty() && svc.findEventByBooking(bk) == nullptr);
        // Xoa khoan chua thu duoc, da thu thi khong.
        string req2 = svc.addRequest(cus, EventType::TiecCuoi, "T2", "E", "0936111222", "",
                                     dt("2027-08-04 18:00"), dt("2027-08-04 22:00"), 50, "", {}).id;
        string bk2 = svc.createBooking(req2, "HAL-0004", dt("2027-08-04 18:00"), dt("2027-08-04 22:00"),
                                       100, 0, 0)
                         .id;
        string unpaid = svc.paymentsOf(bk2).front().id();
        CHECK(svc.deletePayment(unpaid).ok);
        CHECK(svc.recordPayment(bk2, PaymentType::DatCoc1, 3000000, PaymentMethod::TienMat,
                                DateTime::now(), "Cọc")
                  .ok);
        string paid = svc.paymentsOf(bk2).front().id();
        CHECK(!svc.deletePayment(paid).ok);
        CHECK(!svc.deleteBooking(bk2).ok);  // da coc -> phai Huy
        // Xoa sanh dang co booking -> tu choi.
        CHECK(!svc.deleteHall("HAL-0004").ok);
    }

    cout << "== 14. Khach vang lai: nhap ho so, phan quyen, luu lai ==\n";
    {
        auto dir = makeTempDir("guest");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        const auto start = dt("2028-04-01 18:00"), end = dt("2028-04-01 22:00");
        CHECK(!svc.isLoggedIn());
        CHECK(svc.can(Action::SubmitRequest) && svc.can(Action::LookupHalls));
        CHECK(!svc.can(Action::ManageRequests) && !svc.can(Action::ManageCustomers));
        CHECK(!svc.can(Action::ManageBookings) && !svc.can(Action::ManagePayments));
        const auto customers = svc.db().customers.size(), requests = svc.db().requests.size();
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "", "0900123000", "", start, end, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "sai", "", start, end, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "", start, end, 50, "", {}, 0).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "", start, end, 50, "", {}, 121).ok);
        CHECK(!svc.addRequest("CUS-0001", EventType::TiecCuoi, "T", "An", "0900123000", "", start, end, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "missing", start, end, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "", end, start, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "", start, start, 50, "", {}, 25).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "An", "0900123000", "", start, end, 50, "", {{"missing",1}}, 25).ok);
        CHECK(svc.db().customers.size() == customers && svc.db().requests.size() == requests);
        string name = "Nguyễn | An, Bình";
        string notes = "Dong 1|ghi chu, co dau\\\nDong 2\r\nKet thuc";
        auto r = svc.addRequest("", EventType::SinhNhat, "Sinh nhat | An", name, "0900123000", "HAL-0002",
                                start, end, 50, notes, {{"FOD01-P",2}}, 25);
        CHECK(r.ok);
        string cid = svc.findRequest(r.id)->customerId();
        CHECK(svc.findCustomer(cid)->age() == 25);
        CHECK(svc.guestRequests().size() == 1);
        CHECK(!svc.updateRequestStatus(r.id, RequestStatus::HoanThanh).ok);
        CHECK(!svc.deleteRequest(r.id).ok);
        CHECK(!svc.createBooking(r.id, "HAL-0002", start, end, 50, 0, 0).ok);
        CHECK(!svc.addRequest("", EventType::TiecCuoi, "T", "Ten khac", "0900123000", "", start, end, 50, "", {}, 25).ok);
        CHECK(svc.login("kinhdoanh", "123456").ok);
        auto b = svc.createBooking(r.id, "HAL-0002", start, end, 50, 0, 0);
        CHECK(b.ok);
        CHECK(svc.findEventByBooking(b.id)->getEventType() == EventType::SinhNhat);
        CHECK(!svc.recordPayment(b.id, PaymentType::DatCoc1, -1, PaymentMethod::TienMat, DateTime::now(), "Sai").ok);
        TextStorage reloaded(dir / "data", dir / "backup");
        reloaded.load();
        PalaceService next(reloaded);
        CHECK(next.findCustomer(cid)->name() == name && next.findCustomer(cid)->age() == 25);
        CHECK(next.findRequest(r.id)->notes() == notes);
        CHECK(next.findRequest(r.id)->lines().size() == 1 && next.findRequest(r.id)->lines()[0].quantity() == 2);
        CHECK(next.findEventByBooking(b.id)->getEventType() == EventType::SinhNhat);
        CHECK(next.guestRequests().empty());
        CHECK(next.login("kinhdoanh", "123456").ok && next.findRequest(r.id) != nullptr);
    }

    cout << "== 15. Da hinh qua Event abstract va khoi phuc loai tu TXT ==\n";
    {
        static_assert(is_abstract<Event>::value, "Event phai la abstract class");
        vector<shared_ptr<Event>> events;
        for (auto type : {EventType::TiecCuoi, EventType::SinhNhat, EventType::HoiThao})
            events.push_back(makeEvent(type, "E", "B", "C", "H", dt("2028-01-01 10:00"),
                                      dt("2028-01-01 11:00"), EventStatus::SapDienRa, ""));
        CHECK(events[0]->preparationGuide() != events[1]->preparationGuide());
        CHECK(events[1]->preparationGuide() != events[2]->preparationGuide());
        for (const auto& e : events) {
            auto copy = Event::fromFields(splitTextFields(joinTextFields(e->toFields())));
            CHECK(copy->getEventType() == e->getEventType());
            CHECK(copy->preparationGuide() == e->preparationGuide());
        }
    }

    cout << "== 16. TXT trung ID, thieu file, sai ngay/so/escape khong bi ghi de ==\n";
    {
        auto dir = makeTempDir("badtext");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        auto path = dir / "data" / "users.txt";
        ifstream input(path, ios::binary);
        string original((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
        input.close();
        { ofstream out(path, ios::binary); out << original << original; }
        bool duplicate = false;
        try { storage.load(); } catch (const StorageError&) { duplicate = true; }
        CHECK(duplicate);
        CHECK(filesystem::file_size(path) == original.size() * 2);
        { ofstream out(path, ios::binary); out << original; }
        filesystem::rename(path, dir / "data" / "users.keep");
        bool missing = false;
        try { storage.load(); } catch (const StorageError&) { missing = true; }
        CHECK(missing && !filesystem::exists(path));
        filesystem::rename(dir / "data" / "users.keep", path);
        int invalid = 0;
        try { textDate("31/02/2028 10:00"); } catch (const exception&) { ++invalid; }
        try { textMoney("12abc"); } catch (const exception&) { ++invalid; }
        try { textMoney("-1"); } catch (const exception&) { ++invalid; }
        try { textBool("yes"); } catch (const exception&) { ++invalid; }
        try { splitTextFields("a|b\\"); } catch (const exception&) { ++invalid; }
        CHECK(invalid == 5);
        auto userFields = storage.db().users.front().toFields();
        userFields[7] = "sai vai tro";
        bool badRole = false;
        try { User::fromFields(userFields); } catch (const exception&) { badRole = true; }
        CHECK(badRole);
        // Bo du lieu rong hop le khong tu dong tao lai tai khoan mau.
        storage.db() = Database{};
        storage.save();
        storage.load();
        CHECK(storage.db().users.empty() && storage.db().customers.empty() && storage.db().halls.empty());
    }

    cout << "== 17. Ngoai le co kieu va chuyen loi an toan cho Qt ==\n";
    {
        auto dir = makeTempDir("exceptions");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(!svc.login("kinhdoanh", "sai mat khau").ok);
        CHECK(!svc.isLoggedIn());
        Flow f = runFullFlow(svc, "EX", "HAL-0004");
        bool missing = false, invalid = false, conflict = false;
        try {
            svc.validateBookingSlot("missing", dt("2027-03-20 18:00"), dt("2027-03-20 22:00"));
        } catch (const NotFoundException&) { missing = true; }
        try {
            svc.validateBookingSlot("HAL-0004", dt("2027-03-20 22:00"), dt("2027-03-20 18:00"));
        } catch (const ValidationException&) { invalid = true; }
        try {
            svc.validateBookingSlot("HAL-0004", dt("2027-03-20 19:00"), dt("2027-03-20 23:00"));
        } catch (const BookingConflictException&) { conflict = true; }
        CHECK(missing && invalid && conflict);
        // Hai ca lien tiep cham moc 22:00 van hop le.
        string reason = "thong bao cu";
        CHECK(svc.hallAvailable("HAL-0004", dt("2027-03-20 22:00"), dt("2027-03-21 01:00"), "", reason));
        CHECK(reason.empty());
        CHECK(svc.hallAvailable("HAL-0004", dt("2027-03-20 18:00"), dt("2027-03-20 22:00"), f.bookingId, reason));
        CHECK(!svc.hallAvailable("HAL-0004", dt("2027-03-20 19:00"), dt("2027-03-20 23:00"), "", reason));
        CHECK(!reason.empty());
        int validationErrors = 0;
        for (const string& value : {string("abc"), string("-1"), string("999999999999999999999999999")}) {
            try { textMoney(value); }
            catch (const ValidationException&) { ++validationErrors; }
        }
        CHECK(validationErrors == 3);
        storage.db().customers.push_back(storage.db().customers.front());
        bool duplicate = false;
        try { storage.save(); }
        catch (const DuplicateException&) { duplicate = true; }
        CHECK(duplicate);
        // save that bai truoc khi ghi: du lieu tren dia van doc lai duoc.
        TextStorage reloaded(dir / "data", dir / "backup");
        reloaded.load();
        CHECK(reloaded.db().customers.size() + 1 == storage.db().customers.size());
        CHECK(reloaded.db().events.front()->preparationGuide() == storage.db().events.front()->preparationGuide());
    }

    cout << "== 18. Trang thai booking theo tong tien thuc thu ==\n";
    {
        auto dir = makeTempDir("payment_status");
        TextStorage storage(dir / "data", dir / "backup");
        storage.load();
        PalaceService svc(storage);
        CHECK(svc.login("admin", "123456").ok);
        auto customer = svc.addCustomer("Khach thanh toan", "0900999123", "", "", "");
        CHECK(customer.ok);
        auto create = [&](int day, int64_t discount) {
            DateTime start = dt("2028-06-01 18:00").plusHours(day * 24);
            DateTime end = start.plusHours(4);
            auto request = svc.addRequest(customer.id, EventType::TiecCuoi, "Test thanh toan",
                "Khach thanh toan", "0900999123", "HAL-0004", start, end, 100, "", {});
            CHECK(request.ok);
            auto booking = svc.createBooking(request.id, "HAL-0004", start, end, 100, 0, discount);
            CHECK(booking.ok);
            return booking.id;
        };
        string partial = create(0, 1000);
        int64_t total = svc.findBooking(partial)->finalTotal();
        CHECK(total > 1);
        CHECK(svc.findBooking(partial)->status() == BookingStatus::ChoCoc);
        CHECK(svc.paidAmount(partial) == 0); // Khoan cho thu khong tinh vao tong.
        CHECK(!svc.recordPayment(partial, PaymentType::TatToan, -1, PaymentMethod::TienMat,
                                DateTime::now(), "Sai").ok);
        CHECK(svc.findBooking(partial)->status() == BookingStatus::ChoCoc);
        CHECK(svc.recordPayment(partial, PaymentType::TatToan, 1, PaymentMethod::TienMat,
                               DateTime::now(), "Nhan tat toan nhung chua du tien").ok);
        CHECK(svc.findBooking(partial)->status() == BookingStatus::DaCoc);
        CHECK(svc.recordPayment(partial, PaymentType::DatCoc2, total - 1, PaymentMethod::TienMat,
                               DateTime::now(), "Du tien duoi nhan dat coc").ok);
        CHECK(svc.paidAmount(partial) == total);
        CHECK(svc.findBooking(partial)->status() == BookingStatus::DaThanhToan);
        CHECK(svc.findBooking(partial)->occupiesHall());
        string reason;
        CHECK(!svc.hallAvailable("HAL-0004", svc.findBooking(partial)->start(),
                                svc.findBooking(partial)->end(), "", reason));
        CHECK(svc.sweepExpiredHolds(DateTime::now().plusHours(100)) == 0);
        CHECK(!svc.deleteBooking(partial).ok);
        CHECK(!svc.setBookingStatus(partial, BookingStatus::DaThanhToan).ok);

        string upfront = create(1, 0);
        CHECK(svc.recordPayment(upfront, PaymentType::TatToan, svc.findBooking(upfront)->finalTotal(),
                               PaymentMethod::TienMat, DateTime::now(), "Tra du ngay lan dau").ok);
        CHECK(svc.findBooking(upfront)->status() == BookingStatus::DaThanhToan);
        string overpaid = create(2, 0);
        CHECK(svc.recordPayment(overpaid, PaymentType::DatCoc1, svc.findBooking(overpaid)->finalTotal() + 1,
                               PaymentMethod::TienMat, DateTime::now(), "Thu vuot tong").ok);
        CHECK(svc.findBooking(overpaid)->status() == BookingStatus::DaThanhToan);

        TextStorage reloaded(dir / "data", dir / "backup");
        reloaded.load();
        PalaceService next(reloaded);
        for (const string& id : {partial, upfront, overpaid}) {
            CHECK(next.findBooking(id)->status() == BookingStatus::DaThanhToan);
            CHECK(next.findBooking(id)->occupiesHall());
            CHECK(next.paidAmount(id) >= next.findBooking(id)->finalTotal());
        }
        CHECK(svc.setBookingStatus(upfront, BookingStatus::HoanTat).ok);
        CHECK(svc.recordPayment(upfront, PaymentType::TatToan, 1, PaymentMethod::TienMat,
                               DateTime::now(), "Thu bo sung sau hoan tat").ok);
        CHECK(svc.findBooking(upfront)->status() == BookingStatus::HoanTat);
        CHECK(svc.cancelBooking(overpaid, "Khach huy").ok);
        CHECK(!svc.recordPayment(overpaid, PaymentType::TatToan, 1, PaymentMethod::TienMat,
                                DateTime::now(), "Khong thu cho booking huy").ok);
        CHECK(svc.findBooking(overpaid)->status() == BookingStatus::DaHuy);
    }

    cout << "\n========================================\n";
    cout << "Tổng: " << g_total << " | Đạt: " << (g_total - g_failed) << " | Lỗi: " << g_failed << "\n";
    cout << "========================================\n";
    return g_failed == 0 ? 0 : 1;
}
