#include "ui/app_ui.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include <algorithm>
#include <sstream>

namespace tdp {

using namespace ftxui;

namespace {

// ---------- Helpers dùng chung cho UI ----------

Element StatusLine(const std::string& status, const std::string& kind) {
    if (status.empty()) return text("") ;
    const Color c = kind == "err" ? Color::Red : Color::Green;
    return hbox({text(kind == "err" ? "[LỖI] " : "[OK] "), text(status)}) | color(c) | yflex_grow;
}


// Bảng chỉ hiển thị (read-only) từ các dòng chuỗi.
Component TableBox(const std::vector<std::vector<std::string>>& rows) {
    auto copy = std::make_shared<std::vector<std::vector<std::string>>>(rows);
    return Renderer([copy] {
        if (copy->empty() || (*copy)[0].empty()) return text("(Không có dữ liệu)") | dim | border;
        Table t(*copy);
        t.SelectAll().Border(LIGHT);
        t.SelectRow(0).Decorate(bold);
        t.SelectRow(0).SeparatorVertical(EMPTY);
        return t.Render();
    });
}

[[maybe_unused]] std::vector<std::string> customerLabels(const Database& db) {
    std::vector<std::string> out;
    for (const auto& c : db.customers)
        out.push_back(c.id() + " | " + c.name() + " | " + c.phone());
    return out;
}

std::vector<std::string> requestLabels(const Database& db) {
    std::vector<std::string> out;
    for (const auto& r : db.requests)
        out.push_back(r.id() + " | " + toString(r.type()) + " | " + r.title() + " | " +
                      r.start().toString() + " | " + toString(r.status()));
    return out;
}

std::vector<std::string> quotationLabels(const Database& db) {
    std::vector<std::string> out;
    for (const auto& q : db.quotations)
        out.push_back(q.id() + " | YC " + q.requestId() + " | " + formatMoney(q.total()) + " | " +
                      toString(q.status()) + " | " + std::to_string(q.lines().size()) + " dòng");
    return out;
}

std::vector<std::string> bookingLabels(const Database& db) {
    std::vector<std::string> out;
    for (const auto& b : db.bookings)
        out.push_back(b.id() + " | Sảnh " + b.hallId() + " | " + b.start().toString() + " -> " +
                      b.end().toString() + " | " + toString(b.status()));
    return out;
}

[[maybe_unused]] std::vector<std::string> eventLabels(const Database& db) {
    std::vector<std::string> out;
    for (const auto& e : db.events)
        out.push_back(e.id() + " | BK " + e.bookingId() + " | Sảnh " + e.hallId() + " | " +
                      e.start().toString() + " | " + toString(e.status()));
    return out;
}

// Form nhập liệu nhiều trường: trả về Component hiển thị label + ô nhập.
Component LabeledInput(const std::string& label, std::string* value, const std::string& placeholder) {
    auto inp = Input(value, placeholder);
    return Renderer(inp, [label, inp] {
        return hbox({text(label) | bold | size(WIDTH, EQUAL, 24), inp->Render() | flex});
    });
}

struct UiState {
    PalaceService& svc;
    std::string screen = "login";
    std::string status;
    std::string statusKind = "ok";

    void ok(const std::string& m) { status = m; statusKind = "ok"; }
    void err(const std::string& m) { status = m; statusKind = "err"; }
    void set(const OpResult& r) {
        if (r.ok) ok(r.message); else err(r.message);
    }
    void back() {
        screen = "main";
        status.clear();
    }
};

}  // namespace

void runTerminalUi(PalaceService& service) {
    auto screen = ScreenInteractive::Fullscreen();
    UiState st{service, "login", "", "ok"};

    // ============================ MENU CHÍNH DEFINITION ============================
    struct MainEntry {
        std::string label;
        std::string screen;
        Action action;
    };
    std::vector<MainEntry> mainEntries = {
        {"Khách hàng", "customers", Action::ManageCustomers},
        {"Yêu cầu sự kiện", "requests", Action::ManageRequests},
        {"Sảnh & Dịch vụ (quản trị)", "venues", Action::ManageVenues},
        {"Tra cứu sảnh trống", "lookup", Action::LookupHalls},
        {"Báo giá", "quotations", Action::ManageQuotations},
        {"Booking", "bookings", Action::ManageBookings},
        {"Hợp đồng", "contracts", Action::ManageContracts},
        {"Thanh toán", "payments", Action::ManagePayments},
        {"Vận hành sự kiện", "operations", Action::ManageOperations},
        {"Báo cáo", "reports", Action::ViewReports},
        {"Nhật ký hệ thống", "audit", Action::ViewAudit},
        {"Người dùng (quản trị)", "users", Action::ManageUsers},
    };
    int main_sel = 0;
    std::vector<std::string> mainLabels;
    auto refreshMain = [&] {
        mainLabels.clear();
        for (auto& e : mainEntries)
            if (service.can(e.action)) mainLabels.push_back(e.label);
        mainLabels.push_back("Đăng xuất");
        mainLabels.push_back("Thoát chương trình");
        if (main_sel >= (int)mainLabels.size()) main_sel = 0;
    };

    // ============================ ĐĂNG NHẬP ============================
    std::string login_user, login_pass;
    std::string reg_user, reg_pass, reg_confirm, reg_name;
    std::vector<std::string> roleLabels{"Nhân viên kinh doanh", "Kế toán", "Điều phối viên", "Quản trị viên"};
    std::vector<Role> roleValues{Role::Sales, Role::Accountant, Role::Coordinator, Role::Admin};
    int reg_role = 0;
    auto doLogin = [&] {
        OpResult r = service.login(login_user, login_pass);
        st.set(r);
        if (r.ok) {
            login_pass.clear();
            refreshMain();
            main_sel = 0;
            st.screen = "main";
        }
    };
    auto loginScreen = Container::Vertical({
        LabeledInput("Tài khoản:", &login_user, "tên đăng nhập"),
        LabeledInput("Mật khẩu:", &login_pass, "mật khẩu"),
        Container::Horizontal({
            Button("Đăng nhập", doLogin),
            Button("Đăng ký tài khoản mới", [&] {
                reg_user.clear(); reg_pass.clear(); reg_confirm.clear(); reg_name.clear();
                st.status.clear();
                st.screen = "register";
            }),
            Button("Thoát", screen.ExitLoopClosure()),
        }),
    });

    // ============================ ĐĂNG KÝ ============================
    auto doRegister = [&] {
        OpResult r = service.registerUser(reg_user, reg_pass, reg_confirm, reg_name, roleValues[reg_role]);
        st.set(r);
        if (r.ok) {
            login_user = reg_user;
            login_pass.clear();
            st.screen = "login";
        }
    };
    auto registerScreen = Container::Vertical({
        LabeledInput("Tên đăng nhập:", &reg_user, "chưa được dùng"),
        LabeledInput("Họ và tên:", &reg_name, "để trống sẽ dùng tên đăng nhập"),
        LabeledInput("Mật khẩu:", &reg_pass, "tối thiểu 4 ký tự"),
        LabeledInput("Nhập lại mật khẩu:", &reg_confirm, "giống mật khẩu ở trên"),
        Radiobox(&roleLabels, &reg_role),
        Container::Horizontal({
            Button("Tạo tài khoản", doRegister),
            Button("Quay lại đăng nhập", [&] { st.screen = "login"; st.status.clear(); }),
        }),
    });

    // ============================ KHÁCH HÀNG ============================
    std::string cus_search, cus_name, cus_phone, cus_email, cus_addr, cus_notes, cus_editId;
    int cus_sel = 0;
    std::vector<std::string> cusEntries;
    auto customerList = Menu(&cusEntries, &cus_sel);
    auto refreshCustomers = [&] {
        auto list = service.searchCustomers(cus_search);
        cusEntries.clear();
        for (auto& c : list) cusEntries.push_back(c.id() + " | " + c.name() + " | " + c.phone() + " | " + c.email());
        if (cus_sel >= (int)cusEntries.size()) cus_sel = 0;
    };
    auto customersScreen = Container::Vertical({
        LabeledInput("Tìm kiếm (tên/điện thoại):", &cus_search, "từ khóa..."),
        Button("Tìm / Làm mới", [&] { refreshCustomers(); st.ok("Đã làm mới danh sách khách hàng."); }),
        Button("Thêm khách hàng mới", [&] { st.screen = "customer_add"; }),
        Button("Sửa khách hàng đang chọn", [&] {
            if (cus_sel < 0 || cus_sel >= (int)cusEntries.size()) { st.err("Chưa chọn khách hàng hợp lệ."); return; }
            std::string id = cusEntries[cus_sel].substr(0, cusEntries[cus_sel].find(' '));
            const Customer* c = service.findCustomer(id);
            if (!c) { st.err("Không tìm thấy khách hàng."); return; }
            cus_editId = c->id();
            cus_name = c->name(); cus_phone = c->phone(); cus_email = c->email();
            cus_addr = c->address(); cus_notes = c->notes();
            st.screen = "customer_edit";
        }),
        customerList,
    });

    auto customerAddForm = Container::Vertical({
        LabeledInput("Họ tên / Tên công ty:", &cus_name, "bắt buộc"),
        LabeledInput("Số điện thoại:", &cus_phone, "vd: 0912345678"),
        LabeledInput("Email:", &cus_email, "vd: a@b.com"),
        LabeledInput("Địa chỉ:", &cus_addr, ""),
        LabeledInput("Ghi chú:", &cus_notes, ""),
        Container::Horizontal({
            Button("Lưu", [&] {
                st.set(service.addCustomer(cus_name, cus_phone, cus_email, cus_addr, cus_notes));
                cus_name = cus_phone = cus_email = cus_addr = cus_notes = "";
            }),
            Button("Quay lại", [&] { cus_name = cus_phone = cus_email = cus_addr = cus_notes = ""; st.back(); }),
        }),
    });

    auto customerEditForm = Container::Vertical({
        LabeledInput("Họ tên / Tên công ty:", &cus_name, "bắt buộc"),
        LabeledInput("Số điện thoại:", &cus_phone, ""),
        LabeledInput("Email:", &cus_email, ""),
        LabeledInput("Địa chỉ:", &cus_addr, ""),
        LabeledInput("Ghi chú:", &cus_notes, ""),
        Container::Horizontal({
            Button("Lưu thay đổi", [&] { st.set(service.updateCustomer(cus_editId, cus_name, cus_phone, cus_email, cus_addr, cus_notes)); }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });

    // ============================ YÊU CẦU SỰ KIỆN ============================
    std::string req_customerId = "CUS-", req_title, req_start = "2026-12-20 18:00", req_end = "2026-12-20 22:00",
                req_guests = "100", req_special, req_type = "TiecCuoi";
    int req_sel = 0;
    std::vector<std::string> reqEntries;
    auto requestList = Menu(&reqEntries, &req_sel);
    auto refreshRequests = [&] { reqEntries = requestLabels(service.db()); };
    auto requestsScreen = Container::Vertical({
        Button("Tiếp nhận yêu cầu mới", [&] { st.screen = "request_add"; }),
        Button("Xem / Làm mới danh sách", [&] { refreshRequests(); st.ok("Đã làm mới."); }),
        Button("Đánh dấu đã báo giá", [&] {
            if (req_sel >= 0 && req_sel < (int)reqEntries.size()) {
                std::string id = reqEntries[req_sel].substr(0, reqEntries[req_sel].find(' '));
                st.set(service.updateRequestStatus(id, RequestStatus::DaBaoGia));
            }
        }),
        Button("Hủy yêu cầu đang chọn", [&] {
            if (req_sel >= 0 && req_sel < (int)reqEntries.size()) {
                std::string id = reqEntries[req_sel].substr(0, reqEntries[req_sel].find(' '));
                st.set(service.updateRequestStatus(id, RequestStatus::Huy));
            }
        }),
        requestList,
    });
    auto requestAddForm = Container::Vertical({
        LabeledInput("Mã khách hàng:", &req_customerId, "CUS-xxxx (xem danh sách bên dưới)"),
        LabeledInput("Loại sự kiện:", &req_type, "TiecCuoi/HoiNghi/SinhNhat/DoanhNghiep/Khac"),
        LabeledInput("Tiêu đề:", &req_title, "vd: Tiệc cưới Nguyễn Văn A"),
        LabeledInput("Bắt đầu (YYYY-MM-DD HH:MM):", &req_start, ""),
        LabeledInput("Kết thúc (YYYY-MM-DD HH:MM):", &req_end, ""),
        LabeledInput("Số lượng khách:", &req_guests, ""),
        LabeledInput("Yêu cầu đặc biệt:", &req_special, ""),
        Container::Horizontal({
            Button("Lưu yêu cầu", [&] {
                auto s = DateTime::parse(req_start), e = DateTime::parse(req_end);
                auto g = parseInt(req_guests);
                if (!s || !e) { st.err("Thời gian không hợp lệ (dùng YYYY-MM-DD HH:MM)."); return; }
                if (!g) { st.err("Số lượng khách không hợp lệ."); return; }
                st.set(service.addRequest(req_customerId, eventTypeFromString(req_type), req_title, *s, *e, *g, req_special));
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });

    // ============================ SẢNH & DỊCH VỤ ============================
    std::string ven_centerName, ven_centerAddr, ven_centerId, ven_hallCenter = "CTR-0001", ven_hallName, ven_hallCap = "200",
                ven_hallId, ven_svcName, ven_svcUnit = "gói", ven_svcPrice = "1000000", ven_svcCat = "Dịch vụ", ven_svcId;
    auto venuesScreen = Container::Vertical({
        Button("Thêm trung tâm", [&] { st.screen = "center_add"; }),
        Button("Thêm sảnh", [&] { st.screen = "hall_add"; }),
        Button("Thêm dịch vụ", [&] { st.screen = "service_add"; }),
        Button("Cập nhật sảnh đang chọn", [&] { st.screen = "hall_edit"; }),
        Button("Cập nhật dịch vụ đang chọn", [&] { st.screen = "service_edit"; }),
    });
    auto centerAddForm = Container::Vertical({
        LabeledInput("Tên trung tâm:", &ven_centerName, "bắt buộc"),
        LabeledInput("Địa chỉ:", &ven_centerAddr, ""),
        Container::Horizontal({
            Button("Lưu", [&] { st.set(service.addCenter(ven_centerName, ven_centerAddr)); ven_centerName = ven_centerAddr = ""; }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });
    auto hallAddForm = Container::Vertical({
        LabeledInput("Mã trung tâm:", &ven_hallCenter, "CTR-xxxx"),
        LabeledInput("Tên sảnh:", &ven_hallName, "bắt buộc"),
        LabeledInput("Sức chứa:", &ven_hallCap, ""),
        Container::Horizontal({
            Button("Lưu", [&] {
                auto cap = parseInt(ven_hallCap);
                if (!cap) { st.err("Sức chứa không hợp lệ."); return; }
                st.set(service.addHall(ven_hallCenter, ven_hallName, *cap));
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });
    auto hallEditForm = Container::Vertical({
        LabeledInput("Mã sảnh:", &ven_hallId, "HAL-xxxx"),
        LabeledInput("Tên mới:", &ven_hallName, ""),
        LabeledInput("Sức chứa mới:", &ven_hallCap, ""),
        Button("Đang hoạt động? (nhập lại để bật/tắt)", [&] {
            auto cap = parseInt(ven_hallCap);
            const Hall* h = service.findHall(ven_hallId);
            if (!h) { st.err("Không tìm thấy sảnh."); return; }
            if (!cap) { st.err("Sức chứa không hợp lệ."); return; }
            st.set(service.updateHall(ven_hallId, ven_hallName.empty() ? h->name() : ven_hallName, *cap, !h->active()));
        }),
        Button("Quay lại", [&] { st.back(); }),
    });
    auto serviceAddForm = Container::Vertical({
        LabeledInput("Tên dịch vụ:", &ven_svcName, "bắt buộc"),
        LabeledInput("Đơn vị:", &ven_svcUnit, "buổi/gói/khách"),
        LabeledInput("Đơn giá (VND):", &ven_svcPrice, ""),
        LabeledInput("Nhóm:", &ven_svcCat, ""),
        Container::Horizontal({
            Button("Lưu", [&] {
                auto p = parseMoney(ven_svcPrice);
                if (!p) { st.err("Đơn giá không hợp lệ."); return; }
                st.set(service.addService(ven_svcName, ven_svcUnit, *p, ven_svcCat));
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });
    auto serviceEditForm = Container::Vertical({
        LabeledInput("Mã dịch vụ:", &ven_svcId, "SVC-xxxx"),
        LabeledInput("Tên mới:", &ven_svcName, "trống = giữ nguyên"),
        LabeledInput("Đơn giá mới:", &ven_svcPrice, "trống = giữ nguyên"),
        Container::Horizontal({
            Button("Lưu (bật/tắt hoạt động)", [&] {
                const ServiceItem* s = service.findService(ven_svcId);
                if (!s) { st.err("Không tìm thấy dịch vụ."); return; }
                int64_t price = s->unitPrice();
                if (!ven_svcPrice.empty()) {
                    auto p = parseMoney(ven_svcPrice);
                    if (!p) { st.err("Đơn giá không hợp lệ."); return; }
                    price = *p;
                }
                st.set(service.updateService(ven_svcId, ven_svcName.empty() ? s->name() : ven_svcName,
                                             s->unit(), price, s->category(), !s->active()));
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });

    // ============================ TRA CỨU SẢNH ============================
    std::string lk_start = "2026-12-20 18:00", lk_end = "2026-12-20 22:00", lk_guests = "200";
    std::vector<std::vector<std::string>> lk_rows{{"Mã sảnh", "Tên sảnh", "Trung tâm", "Sức chứa"}};
    auto lookupScreen = Container::Vertical({
        LabeledInput("Bắt đầu:", &lk_start, "YYYY-MM-DD HH:MM"),
        LabeledInput("Kết thúc:", &lk_end, "YYYY-MM-DD HH:MM"),
        LabeledInput("Số khách:", &lk_guests, ""),
        Container::Horizontal({
            Button("Tra cứu", [&] {
                auto s = DateTime::parse(lk_start), e = DateTime::parse(lk_end);
                auto g = parseInt(lk_guests);
                if (!s || !e || !g) { st.err("Dữ liệu tra cứu không hợp lệ."); return; }
                auto halls = service.findAvailableHalls(*s, *e, *g, "");
                lk_rows.assign({{"Mã sảnh", "Tên sảnh", "Trung tâm", "Sức chứa"}});
                for (auto& h : halls) {
                    const Center* c = nullptr;
                    for (auto& cc : service.db().centers)
                        if (cc.id() == h.centerId()) c = &cc;
                    lk_rows.push_back({h.id(), h.name(), c ? c->name() : h.centerId(), std::to_string(h.capacity())});
                }
                st.ok("Tìm thấy " + std::to_string(halls.size()) + " sảnh phù hợp.");
            }),
        }),
    });

    // ============================ BÁO GIÁ ============================
    std::string q_reqId = "REQ-", q_note, q_selId, q_svcId = "SVC-", q_qty = "1", q_lineIdx = "0";
    int q_sel = 0;
    std::vector<std::string> qEntries;
    auto quotationList = Menu(&qEntries, &q_sel);
    auto quotationsScreen = Container::Vertical({
        Button("Tạo báo giá từ yêu cầu", [&] { st.screen = "quotation_add"; }),
        Button("Chọn báo giá đang chọn để xử lý", [&] {
            if (q_sel >= 0 && q_sel < (int)qEntries.size()) {
                q_selId = qEntries[q_sel].substr(0, qEntries[q_sel].find(' '));
                st.ok("Đang xử lý báo giá " + q_selId);
            }
        }),
        Button("Làm mới danh sách", [&] { qEntries = quotationLabels(service.db()); }),
        quotationList,
    });
    auto quotationAddForm = Container::Vertical({
        LabeledInput("Mã yêu cầu:", &q_reqId, "REQ-xxxx"),
        LabeledInput("Ghi chú:", &q_note, ""),
        Container::Horizontal({
            Button("Tạo", [&] {
                OpResult r = service.createQuotation(q_reqId, q_note);
                st.set(r);
                if (r.ok) q_selId = r.id;
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });
    auto quotationDetail = Container::Vertical({
        LabeledInput("Mã báo giá:", &q_selId, "BQ-xxxx"),
        LabeledInput("Mã dịch vụ:", &q_svcId, "SVC-xxxx"),
        LabeledInput("Số lượng:", &q_qty, ""),
        Container::Horizontal({
            Button("Thêm dòng dịch vụ", [&] {
                auto qty = parseInt(q_qty);
                if (!qty) { st.err("Số lượng không hợp lệ."); return; }
                st.set(service.addQuotationLine(q_selId, q_svcId, *qty));
            }),
            Button("Gửi báo giá", [&] { st.set(service.setQuotationStatus(q_selId, QuotationStatus::DaGui)); }),
            Button("Khách chấp nhận", [&] { st.set(service.setQuotationStatus(q_selId, QuotationStatus::DaChapNhan)); }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });

    // ============================ BOOKING ============================
    std::string bk_quotation = "BQ-", bk_hall = "HAL-", bk_start = "2026-12-20 18:00", bk_end = "2026-12-20 22:00",
                bk_guests = "100", bk_selId, bk_reason;
    int bk_sel = 0;
    std::vector<std::string> bkEntries;
    auto bookingList = Menu(&bkEntries, &bk_sel);
    auto bookingsScreen = Container::Vertical({
        Button("Tạo booking từ báo giá", [&] { st.screen = "booking_add"; }),
        Button("Làm mới danh sách", [&] { bkEntries = bookingLabels(service.db()); }),
        Button("Xác nhận booking đang chọn", [&] {
            if (bk_sel >= 0 && bk_sel < (int)bkEntries.size()) {
                std::string id = bkEntries[bk_sel].substr(0, bkEntries[bk_sel].find(' '));
                st.set(service.confirmBooking(id));
            }
        }),
        Button("Hủy booking đang chọn", [&] {
            if (bk_sel >= 0 && bk_sel < (int)bkEntries.size()) {
                std::string id = bkEntries[bk_sel].substr(0, bkEntries[bk_sel].find(' '));
                st.set(service.cancelBooking(id, bk_reason.empty() ? "Hủy từ màn hình booking" : bk_reason));
            }
        }),
        LabeledInput("Lý do hủy:", &bk_reason, ""),
        bookingList,
    });
    auto bookingAddForm = Container::Vertical({
        LabeledInput("Mã báo giá:", &bk_quotation, "BQ-xxxx"),
        LabeledInput("Mã sảnh:", &bk_hall, "HAL-xxxx"),
        LabeledInput("Bắt đầu:", &bk_start, "YYYY-MM-DD HH:MM"),
        LabeledInput("Kết thúc:", &bk_end, "YYYY-MM-DD HH:MM"),
        LabeledInput("Số khách:", &bk_guests, ""),
        Container::Horizontal({
            Button("Tạo booking", [&] {
                auto s = DateTime::parse(bk_start), e = DateTime::parse(bk_end);
                auto g = parseInt(bk_guests);
                if (!s || !e || !g) { st.err("Dữ liệu không hợp lệ."); return; }
                st.set(service.createBooking(bk_quotation, bk_hall, *s, *e, *g));
            }),
            Button("Quay lại", [&] { st.back(); }),
        }),
    });

    // ============================ HỢP ĐỒNG & THANH TOÁN ============================
    std::string ct_booking = "BK-", ct_selId, pm_contract, pm_amount = "1000000", pm_note, pm_method = "ChuyenKhoan";
    auto contractsScreen = Container::Vertical({
        LabeledInput("Mã booking:", &ct_booking, "BK-xxxx"),
        Container::Horizontal({
            Button("Lập hợp đồng", [&] { st.set(service.createContract(ct_booking)); }),
            Button("Làm mới", [&] { st.ok("Đã làm mới danh sách hợp đồng."); }),
        }),
    });

    auto paymentsScreen = Container::Vertical({
        LabeledInput("Mã hợp đồng:", &pm_contract, "HD-xxxx"),
        LabeledInput("Số tiền (VND):", &pm_amount, ""),
        LabeledInput("Hình thức:", &pm_method, "TienMat/ChuyenKhoan/The"),
        LabeledInput("Ghi chú:", &pm_note, ""),
        Container::Horizontal({
            Button("Ghi nhận thanh toán", [&] {
                auto a = parseMoney(pm_amount);
                if (!a) { st.err("Số tiền không hợp lệ."); return; }
                st.set(service.recordPayment(pm_contract, *a, paymentMethodFromString(pm_method), pm_note));
            }),
            Button("Làm mới", [&] { st.ok("Đã làm mới."); }),
        }),
    });

    // ============================ VẬN HÀNH ============================
    std::string op_eventId = "EV-", op_taskTitle, op_assignee, op_due = "2026-12-19 17:00",
                op_incidentDesc, op_incidentId, op_resolution, op_taskId, op_taskNote, op_eventNotes;
    auto operationsScreen = Container::Vertical({
        LabeledInput("Mã sự kiện:", &op_eventId, "EV-xxxx"),
        Button("Bắt đầu sự kiện (Đang diễn ra)", [&] { st.set(service.updateEventStatus(op_eventId, EventStatus::DangDienRa)); }),
        Button("Xác nhận hoàn tất sự kiện", [&] { st.set(service.completeEvent(op_eventId)); }),
        LabeledInput("Ghi chú đặc biệt:", &op_eventNotes, ""),
        Button("Cập nhật ghi chú", [&] { st.set(service.setEventNotes(op_eventId, op_eventNotes)); }),
        LabeledInput("Nhiệm vụ mới - tiêu đề:", &op_taskTitle, ""),
        LabeledInput("Người phụ trách:", &op_assignee, ""),
        LabeledInput("Hạn hoàn thành:", &op_due, "YYYY-MM-DD HH:MM"),
        Button("Thêm nhiệm vụ", [&] {
            auto d = DateTime::parse(op_due);
            if (!d) { st.err("Hạn hoàn thành không hợp lệ."); return; }
            st.set(service.addTask(op_eventId, op_taskTitle, op_assignee, *d));
        }),
        LabeledInput("Mã nhiệm vụ:", &op_taskId, "TSK-xxxx"),
        LabeledInput("Ghi chú nhiệm vụ:", &op_taskNote, ""),
        Container::Horizontal({
            Button("Bắt đầu làm", [&] { st.set(service.updateTaskStatus(op_taskId, TaskStatus::DangLam, op_taskNote)); }),
            Button("Hoàn thành", [&] { st.set(service.updateTaskStatus(op_taskId, TaskStatus::HoanThanh, op_taskNote)); }),
        }),
        LabeledInput("Sự cố mới - mô tả:", &op_incidentDesc, ""),
        Button("Ghi nhận sự cố", [&] { st.set(service.addIncident(op_eventId, op_incidentDesc)); }),
        LabeledInput("Mã sự cố:", &op_incidentId, "INC-xxxx"),
        LabeledInput("Cách xử lý:", &op_resolution, ""),
        Button("Đánh dấu đã xử lý", [&] { st.set(service.resolveIncident(op_incidentId, op_resolution, IncidentStatus::DaXuLy)); }),
    });

    // ============================ BÁO CÁO ============================
    int rep_sel = 0;
    std::vector<std::string> repEntries{"Sự kiện theo ngày và trạng thái", "Sự kiện sắp diễn ra",
                                        "Tình trạng sử dụng sảnh", "Tổng hợp tài chính",
                                        "Công nợ phải thu", "Nhiệm vụ và sự cố"};
    auto reportMenu = Menu(&repEntries, &rep_sel);
    std::string rep_from = "2026-01-01 00:00", rep_to = "2026-12-31 23:59", rep_status = "TatCa";
    auto reportsScreen = Container::Vertical({
        reportMenu,
        LabeledInput("Từ ngày:", &rep_from, "YYYY-MM-DD HH:MM"),
        LabeledInput("Đến ngày:", &rep_to, "YYYY-MM-DD HH:MM"),
        LabeledInput("Trạng thái sự kiện:", &rep_status, "ChuanBi/DangDienRa/HoanTat/Huy/TatCa"),
    });

    // ============================ NGƯỜI DÙNG & NHẬT KÝ ============================
    std::string us_username, us_password, us_fullname, us_role = "Sales", us_userId, us_newPass;
    auto usersScreen = Container::Vertical({
        LabeledInput("Tên đăng nhập:", &us_username, "không trùng"),
        LabeledInput("Mật khẩu:", &us_password, "tối thiểu 4 ký tự"),
        LabeledInput("Họ tên:", &us_fullname, ""),
        LabeledInput("Vai trò:", &us_role, "Admin/Sales/Accountant/Coordinator"),
        Container::Horizontal({
            Button("Tạo tài khoản", [&] { st.set(service.createUser(us_username, us_password, us_fullname, roleFromString(us_role))); }),
        }),
        LabeledInput("Mã người dùng:", &us_userId, "USR-xxxx"),
        Container::Horizontal({
            Button("Khóa / Mở khóa", [&] {
                const User* u = nullptr;
                for (auto& x : service.db().users) if (x.id() == us_userId) u = &x;
                if (!u) { st.err("Không tìm thấy người dùng."); return; }
                st.set(service.setUserActive(us_userId, !u->active()));
            }),
            Button("Đổi mật khẩu", [&] { st.set(service.changePassword(us_userId, us_newPass)); }),
        }),
        LabeledInput("Mật khẩu mới:", &us_newPass, ""),
    });

    // ============================ MENU CHÍNH ============================
    refreshMain();
    auto mainMenu = Menu(&mainLabels, &main_sel);

    // ============================ BỐ CỤC CHÍNH ============================
    std::vector<std::pair<std::string, Component>> screens = {
        {"login", loginScreen},
        {"register", registerScreen},
        {"main", mainMenu},
        {"customers", customersScreen},
        {"customer_add", customerAddForm},
        {"customer_edit", customerEditForm},
        {"requests", requestsScreen},
        {"request_add", requestAddForm},
        {"venues", venuesScreen},
        {"center_add", centerAddForm},
        {"hall_add", hallAddForm},
        {"hall_edit", hallEditForm},
        {"service_add", serviceAddForm},
        {"service_edit", serviceEditForm},
        {"lookup", lookupScreen},
        {"quotations", quotationsScreen},
        {"quotation_add", quotationAddForm},
        {"quotation_detail", quotationDetail},
        {"bookings", bookingsScreen},
        {"booking_add", bookingAddForm},
        {"contracts", contractsScreen},
        {"payments", paymentsScreen},
        {"operations", operationsScreen},
        {"reports", reportsScreen},
        {"users", usersScreen},
    };
    int activeTab = 0;
    Components comps;
    for (auto& s : screens) comps.push_back(s.second);
    auto shell = Container::Tab(std::move(comps), &activeTab);
    auto syncTab = [&] {
        for (int i = 0; i < (int)screens.size(); ++i)
            if (screens[i].first == st.screen) { activeTab = i; return; }
        activeTab = 0;
    };

    // Lưu trạng thái hiển thị bảng dữ liệu chính.
    auto makeDataTable = [&]() -> Element {
        const Database& db = service.db();
        if (st.screen == "customers") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Tên", "Điện thoại", "Email", "Địa chỉ"}};
            for (auto& c : db.customers) rows.push_back({c.id(), c.name(), c.phone(), c.email(), c.address()});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "requests") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Khách", "Loại", "Tiêu đề", "Bắt đầu", "Khách(m)", "Trạng thái"}};
            for (auto& r : db.requests) {
                const Customer* c = service.findCustomer(r.customerId());
                rows.push_back({r.id(), c ? c->name() : r.customerId(), toString(r.type()), r.title(),
                                          r.start().toString(), std::to_string(r.guestCount()), toString(r.status())});
            }
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "venues") {
            std::vector<std::vector<std::string>> rows{{"Mã sảnh", "Sảnh", "Trung tâm", "Sức chứa", "Hoạt động"},
                                                       {"Mã DV", "Dịch vụ", "Đơn vị", "Đơn giá", "Hoạt động"}};
            for (auto& h : db.halls)
                rows.push_back({h.id(), h.name(), h.centerId(), std::to_string(h.capacity()), h.active() ? "x" : "-"});
            rows.push_back({"---", "---", "---", "---", "---"});
            for (auto& s : db.services)
                rows.push_back({s.id(), s.name(), s.unit(), formatMoney(s.unitPrice()), s.active() ? "x" : "-"});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "lookup") return TableBox(lk_rows)->Render();
        if (st.screen == "quotations" || st.screen == "quotation_detail") {
            std::vector<std::vector<std::string>> rows{{"Mã", "YC", "Tạm tính", "Chiết khấu", "Tổng", "Trạng thái"}};
            for (auto& q : db.quotations)
                rows.push_back({q.id(), q.requestId(), formatMoney(q.subtotal()),
                                std::to_string(q.discountPercent()) + "% (-" + formatMoney(q.discountAmount()) + ")",
                                formatMoney(q.total()), toString(q.status())});
            const Quotation* q = service.findQuotation(q_selId);
            if (q && st.screen == "quotation_detail") {
                rows.push_back({"CHI TIẾT " + q->id(), "Dịch vụ", "SL", "Đơn giá", "Thành tiền", ""});
                for (auto& l : q->lines())
                    rows.push_back({"", l.nameSnapshot(), std::to_string(l.quantity()),
                                    formatMoney(l.unitPrice()), formatMoney(l.amount()), ""});
            }
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "bookings") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Sảnh", "Bắt đầu", "Kết thúc", "Khách", "Trạng thái"}};
            for (auto& b : db.bookings)
                rows.push_back({b.id(), b.hallId(), b.start().toString(), b.end().toString(),
                                std::to_string(b.guestCount()), toString(b.status())});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "contracts") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Số HĐ", "Booking", "Giá trị", "Đã thu", "Còn lại", "Trạng thái"}};
            for (auto& c : db.contracts)
                rows.push_back({c.id(), c.contractNo(), c.bookingId(), formatMoney(c.totalAmount()),
                                formatMoney(service.paidAmount(c.id())), formatMoney(service.outstanding(c.id())),
                                toString(c.status())});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "payments") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Hợp đồng", "Số tiền", "Thời điểm", "Hình thức", "Ghi chú"}};
            for (auto& p : db.payments)
                rows.push_back({p.id(), p.contractId(), formatMoney(p.amount()), p.paidAt().toString(),
                                toString(p.method()), p.note()});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "operations") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Booking", "Sảnh", "Bắt đầu", "Trạng thái"}};
            for (auto& e : db.events)
                rows.push_back({e.id(), e.bookingId(), e.hallId(), e.start().toString(), toString(e.status())});
            rows.push_back({"NHIỆM VỤ", "Sự kiện", "Tiêu đề", "Phụ trách", "Hạn", "Trạng thái"});
            for (auto& tk : db.tasks)
                rows.push_back({tk.id(), tk.eventId(), tk.title(), tk.assignee(), tk.due().toString(), toString(tk.status())});
            rows.push_back({"SỰ CỐ", "Sự kiện", "Mô tả", "Trạng thái", "Xử lý", ""});
            for (auto& i : db.incidents)
                rows.push_back({i.id(), i.eventId(), i.description(), toString(i.status()), i.resolution(), ""});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "reports") {
            std::vector<ReportRow> data;
            switch (rep_sel) {
                case 0: data = service.reportEventsByDateStatus(rep_from, rep_to, rep_status); break;
                case 1: data = service.reportUpcoming(); break;
                case 2: data = service.reportHallUsage(); break;
                case 3: data = service.reportFinance(); break;
                case 4: data = service.reportDebt(); break;
                default: data = service.reportOps(); break;
            }
            std::vector<std::vector<std::string>> rows;
            for (auto& r : data) rows.push_back(r.cells);
            if (rows.empty()) return text("(Không có dữ liệu)") | dim | border;
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "audit") {
            std::vector<std::vector<std::string>> rows{{"Thời điểm", "Người dùng", "Hành động", "Chi tiết"}};
            for (auto& l : db.logs)
                rows.push_back({l.at().toString(), l.username(), l.action(), l.detail()});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        if (st.screen == "users") {
            std::vector<std::vector<std::string>> rows{{"Mã", "Tài khoản", "Họ tên", "Vai trò", "Hoạt động"}};
            for (auto& u : db.users)
                rows.push_back({u.id(), u.username(), u.fullName(), toString(u.role()), u.active() ? "x" : "-"});
            Table t(rows); t.SelectAll().Border(LIGHT); t.SelectRow(0).Decorate(bold);
            return t.Render();
        }
        return emptyElement();
    };

    auto renderer = Renderer(shell, [&] {
        // Đăng nhập
        if (st.screen == "login" || st.screen == "register") {
            const bool isReg = st.screen == "register";
            return vbox({
                hbox({text("TRỐNG ĐỒNG PALACE - QUẢN LÝ SỰ KIỆN") | bold | color(Color::Yellow)}),
                separator(),
                (isReg ? registerScreen : loginScreen)->Render() | border | size(WIDTH, EQUAL, 70),
                StatusLine(st.status, st.statusKind),
                filler(),
            });
        }
        // Menu chính
        const User* user = service.currentUser();
        std::string header = user ? (user->fullName() + " (" + toString(user->role()) + ")") : "";
        if (st.screen == "main") {
            return vbox({
                hbox({text("TRỐNG ĐỒNG PALACE") | bold | color(Color::Yellow) | flex,
                      text("Người dùng: " + header)}),
                separator(),
                hbox({
                    mainMenu->Render() | border | size(WIDTH, EQUAL, 40),
                    filler(),
                }) | flex,
                StatusLine(st.status, st.statusKind),
            });
        }
        // Các màn hình nghiệp vụ: tiêu đề + form trái + bảng phải
        std::string title = st.screen;
        if (title == "customers") title = "KHÁCH HÀNG";
        else if (title == "customer_add" || title == "customer_edit") title = "KHÁCH HÀNG - THÊM/SỬA";
        else if (title == "requests" || title == "request_add") title = "YÊU CẦU SỰ KIỆN";
        else if (title.rfind("ven", 0) == 0 || title.rfind("center", 0) == 0 || title.rfind("hall", 0) == 0 ||
                 title.rfind("service", 0) == 0)
            title = "SẢNH & DỊCH VỤ";
        else if (title == "lookup") title = "TRA CỨU SẢNH TRỐNG";
        else if (title.rfind("quotation", 0) == 0) title = "BÁO GIÁ";
        else if (title.rfind("booking", 0) == 0) title = "BOOKING";
        else if (title == "contracts") title = "HỢP ĐỒNG";
        else if (title == "payments") title = "THANH TOÁN";
        else if (title == "operations") title = "VẬN HÀNH SỰ KIỆN";
        else if (title == "reports") title = "BÁO CÁO";
        else if (title == "audit") title = "NHẬT KÝ HỆ THỐNG";
        else if (title == "users") title = "QUẢN LÝ NGƯỜI DÙNG";

        Component form = loginScreen;
        if (st.screen == "customers") form = customersScreen;
        else if (st.screen == "customer_add") form = customerAddForm;
        else if (st.screen == "customer_edit") form = customerEditForm;
        else if (st.screen == "requests") form = requestsScreen;
        else if (st.screen == "request_add") form = requestAddForm;
        else if (st.screen == "venues") form = venuesScreen;
        else if (st.screen == "center_add") form = centerAddForm;
        else if (st.screen == "hall_add") form = hallAddForm;
        else if (st.screen == "hall_edit") form = hallEditForm;
        else if (st.screen == "service_add") form = serviceAddForm;
        else if (st.screen == "service_edit") form = serviceEditForm;
        else if (st.screen == "lookup") form = lookupScreen;
        else if (st.screen == "quotations") form = quotationsScreen;
        else if (st.screen == "quotation_add") form = quotationAddForm;
        else if (st.screen == "quotation_detail") form = quotationDetail;
        else if (st.screen == "bookings") form = bookingsScreen;
        else if (st.screen == "booking_add") form = bookingAddForm;
        else if (st.screen == "contracts") form = contractsScreen;
        else if (st.screen == "payments") form = paymentsScreen;
        else if (st.screen == "operations") form = operationsScreen;
        else if (st.screen == "reports") form = reportsScreen;
        else if (st.screen == "users") form = usersScreen;

        return vbox({
            hbox({text("TRỐNG ĐỒNG PALACE") | bold | color(Color::Yellow),
                  text(" > " + title) | bold, filler(), text(header), text("  [Esc = quay lại]") | dim}),
            separator(),
            hbox({
                vbox({form->Render() | border, filler()}) | size(WIDTH, EQUAL, 62),
                makeDataTable() | flex | border,
            }) | flex,
            StatusLine(st.status, st.statusKind),
        });
    });

    auto withEvents = CatchEvent(renderer, [&](ftxui::Event ev) {
        syncTab();  // đảm bảo tab đúng trước khi sự kiện đi xuống component
        if (ev == ftxui::Event::Escape) {
            if (st.screen != "login" && st.screen != "main") { st.screen = "main"; st.status.clear(); return true; }
            if (st.screen == "main") return true;
        }
        if (st.screen == "main" && ev == ftxui::Event::Return) {
            // Enter trên menu chính: xử lý chọn mục.
            if (main_sel < (int)mainLabels.size()) {
                const std::string chosen = mainLabels[main_sel];
                if (chosen == "Đăng xuất") { service.logout(); st.screen = "login"; st.status.clear(); return true; }
                if (chosen == "Thoát chương trình") { screen.Exit(); return true; }
                // Khởi tạo dữ liệu màn hình trước khi vào.
                if (chosen == "Khách hàng") refreshCustomers();
                if (chosen == "Yêu cầu sự kiện") reqEntries = requestLabels(service.db());
                if (chosen == "Báo giá") qEntries = quotationLabels(service.db());
                if (chosen == "Booking") bkEntries = bookingLabels(service.db());
                for (auto& e : mainEntries) {
                    if (e.label == chosen) {
                        OpResult perm = service.require(e.action);
                        if (!perm.ok) { st.err(perm.message); return true; }
                        st.screen = e.screen;
                        st.status.clear();
                        return true;
                    }
                }
            }
            return false;
        }
        return false;
    });

    screen.Loop(withEvents);
}

}  // namespace tdp
