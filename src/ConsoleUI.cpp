#include "ConsoleUI.h"

#include "Utils.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

// Dem so ky tu hien thi cua chuoi UTF-8 (khong dem byte phu).
int textWidth(const string& text) {
    int width = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) ++width;
    }
    return width;
}

string padRight(const string& text, int width) {
    string result = text;
    int current = textWidth(result);
    if (current < width) result.append(static_cast<size_t>(width - current), ' ');
    return result;
}

// Cat bot chuoi dai va them "..." de bang khong bi vo.
string cutText(const string& text, int maxWidth) {
    if (textWidth(text) <= maxWidth) return text;
    if (maxWidth <= 3) return string(static_cast<size_t>(maxWidth), '.');

    string result;
    int count = 0;
    for (size_t i = 0; i < text.size();) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        size_t length = 1;
        if ((c & 0xE0) == 0xC0) length = 2;
        else if ((c & 0xF0) == 0xE0) length = 3;
        else if ((c & 0xF8) == 0xF0) length = 4;

        if (count + 1 > maxWidth - 3) break;
        result.append(text, i, length);
        i += length;
        ++count;
    }
    return result + "...";
}

void printTable(const vector<string>& headers, const vector<int>& widths,
                const vector<vector<string>>& rows) {
    int totalWidth = 0;
    for (size_t i = 0; i < widths.size(); ++i) {
        totalWidth += widths[i];
        if (i + 1 < widths.size()) totalWidth += 3;
    }

    string separator(static_cast<size_t>(totalWidth), '-');
    cout << separator << '\n';

    for (size_t i = 0; i < headers.size(); ++i) {
        cout << padRight(cutText(headers[i], widths[i]), widths[i]);
        if (i + 1 < headers.size()) cout << " | ";
    }
    cout << '\n' << separator << '\n';

    for (const vector<string>& row : rows) {
        for (size_t i = 0; i < widths.size() && i < row.size(); ++i) {
            cout << padRight(cutText(row[i], widths[i]), widths[i]);
            if (i + 1 < widths.size()) cout << " | ";
        }
        cout << '\n';
    }
    cout << separator << '\n';
}

bool isYes(const string& answer) {
    string text = toLower(trim(answer));
    return text == "c" || text == "co" || text == "có" || text == "y" || text == "yes";
}

// Lenh dieu huong dung chung cho moi man hinh.
struct Navigation {
    enum class Target { Back, Main };
    Target target;
};

// Neu nguoi dung go 'b' hoac 'm' thi nem loi de thoat khoi man hinh hien tai.
// Menu se bat loi nay: Back = ve man hinh truoc, Main = ve menu chinh.
void throwIfNavigation(const string& text, bool enabled) {
    if (!enabled) return;

    string value = toLower(trim(text));
    if (value == "b" || value == "back" || value == "quaylai")
        throw Navigation{Navigation::Target::Back};
    if (value == "m" || value == "menu" || value == "main" || value == "mainmenu")
        throw Navigation{Navigation::Target::Main};
}

}  // namespace

ConsoleUI::ConsoleUI(PalaceService& service) : service_(service) {}

// ==================== MAN HINH CHUNG ====================

void ConsoleUI::showBanner(bool showNavigation) const {
    navigationEnabled_ = showNavigation;
    cout << "==============================================================\n";
    cout << "     TRỐNG ĐỒNG PALACE - HỆ THỐNG ĐẶT LỊCH SỰ KIỆN\n";
    cout << "==============================================================\n";
    if (showNavigation) {
        cout << "[b] Quay lại màn hình trước   |   [m] Về menu chính\n";
    }
}

void ConsoleUI::showBusinessError(const BusinessException& error) const {
    cout << "\n[LỖI] " << error.what() << '\n';
    try {
        pause();
    } catch (const Navigation&) {
        // Nguoi dung bam b/m tai dong loi: bo qua, menu hien tai se hien lai.
    }
}

void ConsoleUI::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleUI::pause() const {
    cout << "\nNhấn Enter để tiếp tục";
    if (navigationEnabled_) cout << " (b: quay lại, m: về menu chính)";
    cout << "...";

    string line;
    if (!getline(cin, line)) return;
    throwIfNavigation(line, navigationEnabled_);
}

int ConsoleUI::run() {
    showBanner(false);
    while (true) {
        navigationEnabled_ = false;
        if (service_.isLoggedIn()) {
            mainMenu();
            continue;
        }

        cout << "\n1. Đăng nhập\n";
        cout << "2. Đăng ký tài khoản khách hàng\n";
        cout << "0. Thoát chương trình\n";

        int choice = askInt("Chọn", 0, 0, 2);
        if (choice == 0) {
            cout << "\nTạm biệt!\n";
            return 0;
        }
        if (choice == 1) {
            if (loginScreen()) mainMenu();
        } else {
            registerScreen();
        }
    }
}

bool ConsoleUI::loginScreen() {
    clearScreen();
    showBanner(false);
    cout << "--- ĐĂNG NHẬP ---\n";
    cout << "Tài khoản mẫu: admin / kinhdoanh / vanhanh / khachhang (mật khẩu 123456)\n\n";

    string username = askText("Tên đăng nhập", "");
    string password = askText("Mật khẩu", "");

    try {
        OpResult result = service_.login(username, password);
        cout << "\n[OK] " << result.message << '\n';
        pause();
        return true;
    } catch (const BusinessException& error) {
        showBusinessError(error);
        return false;
    }
}

void ConsoleUI::registerScreen() {
    clearScreen();
    showBanner();
    cout << "--- ĐĂNG KÝ TÀI KHOẢN KHÁCH HÀNG ---\n";

    try {
        string username = askText("Tên đăng nhập", "");
        string password = askText("Mật khẩu (tối thiểu 4 ký tự)", "");
        string confirm = askText("Nhập lại mật khẩu", "");
        string fullName = askText("Họ và tên", "");
        string phone = askText("Số điện thoại", "");
        string email = askText("Email", "");
        string address = askText("Địa chỉ", "");

        OpResult result = service_.registerCustomer(username, password, confirm, fullName, phone,
                                                     email, address);
        cout << '\n' << "[OK] " << result.message << '\n';
        pause();
    } catch (const BusinessException& error) {
        showBusinessError(error);
    } catch (const Navigation&) {
        // Quay lai man hinh dang nhap / menu chinh.
    }
}

void ConsoleUI::mainMenu() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner(false);

        const User* user = service_.currentUser();
        if (user == nullptr) {
            service_.logout();
            return;
        }

        cout << "Xin chào: " << user->get_fullName() << " (" << user->roleName() << ")\n";
        cout << "--------------------------------------------------------------\n";

        switch (user->get_role()) {
            case Role::Customer: customerMenu(); break;
            case Role::Sales: salesMenu(); break;
            case Role::Manager: managerMenu(); break;
            case Role::Operation: operationMenu(); break;
        }
    }
}

// ==================== MENU THEO VAI TRO ====================

void ConsoleUI::customerMenu() {
    while (service_.isLoggedIn()) {
        try {
            service_.refreshSystem();
        } catch (const BusinessException& error) {
            showBusinessError(error);
        }
        clearScreen();
        showBanner(false);
        cout << "--- MENU KHÁCH HÀNG ---\n";
        cout << "1. Tạo yêu cầu sự kiện\n";
        cout << "2. Yêu cầu của tôi\n";
        cout << "3. Đặt lịch của tôi (booking & thanh toán)\n";
        cout << "4. Tra cứu sảnh trống\n";
        cout << "5. Hủy yêu cầu của tôi\n";
        cout << "0. Đăng xuất\n";

        int choice = askInt("Chọn", 0, 0, 5);
        try {
            switch (choice) {
                case 0:
                    service_.logout();
                    cout << "Đã đăng xuất.\n";
                    pause();
                    return;
                case 1: createRequestScreen(); break;
                case 2: myRequestsScreen(); break;
                case 3: myBookingsScreen(); break;
                case 4: lookupHallsScreen(); break;
                case 5: customerCancelRequestScreen(); break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
        } catch (const Navigation&) {
            // b/m: menu nay da la menu chinh, tiep tuc hien thi.
        }
    }
}

void ConsoleUI::salesMenu() {
    while (service_.isLoggedIn()) {
        try {
            service_.refreshSystem();
        } catch (const BusinessException& error) {
            showBusinessError(error);
        }
        clearScreen();
        showBanner(false);
        cout << "--- MENU NHÂN VIÊN KINH DOANH ---\n";
        cout << "1. Quản lý yêu cầu sự kiện\n";
        cout << "2. Quản lý booking\n";
        cout << "3. Quản lý thanh toán\n";
        cout << "4. Tra cứu sảnh trống\n";
        cout << "5. Danh sách khách hàng\n";
        cout << "6. Danh mục sảnh & dịch vụ\n";
        cout << "0. Đăng xuất\n";

        int choice = askInt("Chọn", 0, 0, 6);
        try {
            switch (choice) {
                case 0:
                    service_.logout();
                    cout << "Đã đăng xuất.\n";
                    pause();
                    return;
                case 1: requestMenu(); break;
                case 2: bookingMenu(); break;
                case 3: paymentMenu(); break;
                case 4: lookupHallsScreen(); break;
                case 5: customersScreen(); break;
                case 6: catalogScreen(); break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
        } catch (const Navigation&) {
            // b/m: menu nay da la menu chinh, tiep tuc hien thi.
        }
    }
}

void ConsoleUI::managerMenu() {
    while (service_.isLoggedIn()) {
        try {
            service_.refreshSystem();
        } catch (const BusinessException& error) {
            showBusinessError(error);
        }
        clearScreen();
        showBanner(false);
        cout << "--- MENU QUẢN LÝ ---\n";
        cout << "1. Quản lý yêu cầu sự kiện\n";
        cout << "2. Quản lý booking\n";
        cout << "3. Quản lý thanh toán\n";
        cout << "4. Tra cứu sảnh trống\n";
        cout << "5. Danh sách khách hàng\n";
        cout << "6. Danh mục sảnh & dịch vụ\n";
        cout << "7. Quản lý sảnh\n";
        cout << "8. Quản lý dịch vụ\n";
        cout << "9. Quản lý tài khoản\n";
        cout << "10. Danh sách sự kiện\n";
        cout << "0. Đăng xuất\n";

        int choice = askInt("Chọn", 0, 0, 10);
        try {
            switch (choice) {
                case 0:
                    service_.logout();
                    cout << "Đã đăng xuất.\n";
                    pause();
                    return;
                case 1: requestMenu(); break;
                case 2: bookingMenu(); break;
                case 3: paymentMenu(); break;
                case 4: lookupHallsScreen(); break;
                case 5: customersScreen(); break;
                case 6: catalogScreen(); break;
                case 7: manageHallsScreen(); break;
                case 8: manageServicesScreen(); break;
                case 9: manageUsersScreen(); break;
                case 10: eventsScreen(); break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
        } catch (const Navigation&) {
            // b/m: menu nay da la menu chinh, tiep tuc hien thi.
        }
    }
}

void ConsoleUI::operationMenu() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner(false);
        cout << "--- MENU VẬN HÀNH ---\n";
        cout << "1. Danh sách sự kiện\n";
        cout << "0. Đăng xuất\n";

        int choice = askInt("Chọn", 0, 0, 1);
        if (choice == 0) {
            service_.logout();
            cout << "Đã đăng xuất.\n";
            pause();
            return;
        }
        try {
            eventsScreen();
        } catch (const BusinessException& error) {
            showBusinessError(error);
        } catch (const Navigation&) {
            // b/m: menu nay da la menu chinh, tiep tuc hien thi.
        }
    }
}

void ConsoleUI::requestMenu() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ YÊU CẦU ---\n";
        cout << "1. Xem tất cả yêu cầu\n";
        cout << "2. Tạo yêu cầu\n";
        cout << "3. Sửa yêu cầu\n";
        cout << "4. Đổi trạng thái yêu cầu\n";
        cout << "5. Xóa yêu cầu\n";
        cout << "6. Lịch sử 10 yêu cầu gần nhất\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }

        try {
            switch (choice) {
                case 0: return;
                case 9: return;
                case 1: allRequestsScreen(); break;
                case 2: createRequestScreen(); break;
                case 3: editRequestScreen(); break;
                case 4: changeRequestStatusScreen(); break;
                case 5: deleteRequestScreen(); break;
                case 6:
                    printRequests(service_.requestHistory(10), false);
                    pause();
                    break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::bookingMenu() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ BOOKING ---\n";
        cout << "1. Danh sách booking\n";
        cout << "2. Tạo booking từ yêu cầu\n";
        cout << "3. Hủy booking\n";
        cout << "4. Xóa booking chưa cọc\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 4);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }

        try {
            switch (choice) {
                case 0: return;
                case 9: return;
                case 1: bookingsScreen(); break;
                case 2: createBookingScreen(); break;
                case 3: cancelBookingScreen(); break;
                case 4: deleteBookingScreen(); break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::paymentMenu() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ THANH TOÁN ---\n";
        cout << "1. Xem khoản thu theo booking\n";
        cout << "2. Ghi nhận thanh toán\n";
        cout << "3. Sửa khoản thu\n";
        cout << "4. Xóa khoản chưa thu\n";
        cout << "5. In phiếu thu\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }

        try {
            switch (choice) {
                case 0: return;
                case 9: return;
                case 1: bookingsForPaymentScreen(); break;
                case 2: recordPaymentScreen(); break;
                case 3: updatePaymentScreen(); break;
                case 4: {
                    printPayments(service_.allPayments());
                    string id = askText("Mã khoản thu cần xóa", "");
                    if (id.empty()) break;
                    if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) break;
                    OpResult result = service_.deletePayment(id);
                    cout << '\n' << "[OK] " << result.message << '\n';
                    pause();
                    break;
                }
                case 5: paymentReceiptScreen(); break;
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

// ==================== NGHIEP VU KHACH HANG ====================

void ConsoleUI::createRequestScreen() {
    const User* actor = service_.currentUser();
    if (actor == nullptr) return;

    clearScreen();
    showBanner();
    cout << "--- TẠO YÊU CẦU SỰ KIỆN ---\n";

    string customerId;
    if (actor->get_role() == Role::Customer) {
        customerId = actor->get_customerId();
        if (customerId.empty()) {
            cout << "[LỖI] Tài khoản chưa gắn hồ sơ khách hàng.\n";
            pause();
            return;
        }
    } else {
        customerId = chooseCustomer();
        if (customerId.empty()) return;
    }

    const Customer* customer = service_.findCustomer(customerId);
    string defaultName = customer != nullptr ? customer->get_fullName() : "";
    string defaultPhone = customer != nullptr ? customer->get_phone() : "";

    int typeIndex = chooseEventType(0);
    EventType type = static_cast<EventType>(typeIndex);
    EventTypeInfo info = eventTypeInfo(type);

    string title = askText("Tên sự kiện", string("Sự kiện ") + info.name);
    string name = askText("Họ tên khách hàng", defaultName);
    string phone = askText("Số điện thoại", defaultPhone);

    cout << "\nDanh sách sảnh:\n";
    printHalls(service_.allHalls());
    string hallId = askText("Mã sảnh mong muốn (Enter = chưa xác định)", "");
    if (!hallId.empty() && service_.findHall(hallId) == nullptr) {
        cout << "[LỖI] Mã sảnh không tồn tại.\n";
        pause();
        return;
    }

    DateTime start = askDateTime("Thời gian bắt đầu (dd/mm/yyyy hh:mm)");
    DateTime end = askDateTime("Thời gian kết thúc (dd/mm/yyyy hh:mm)");
    while (!(start < end)) {
        cout << "[LỖI] Kết thúc phải sau bắt đầu.\n";
        end = askDateTime("Thời gian kết thúc (dd/mm/yyyy hh:mm)");
    }

    int guestCount = askInt("Số khách", 100, 10, service_.maxHallCapacity());
    string notes = askText("Ghi chú / yêu cầu đặc biệt", "");

    vector<ServiceLineInput> services;
    chooseServices(services);

    OpResult result = service_.addRequest(customerId, type, title, name, phone, hallId, start, end,
                                          guestCount, notes, services);
    cout << '\n' << "[OK] " << result.message << '\n';

    const EventRequest* created = service_.findRequest(result.id);
    if (created != nullptr) {
        cout << '\n';
        requestDetail(*created);
    }
    pause();
}

void ConsoleUI::myRequestsScreen() {
    const User* actor = service_.currentUser();
    if (actor == nullptr) return;

    vector<EventRequest> requests = service_.requestsForCustomer(actor->get_customerId());
    clearScreen();
    showBanner();
    cout << "--- YÊU CẦU CỦA TÔI ---\n";

    if (requests.empty()) {
        cout << "Bạn chưa gửi yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, true);
    string id = askText("Nhập mã yêu cầu để xem chi tiết (Enter bỏ qua)", "");
    if (!id.empty()) {
        const EventRequest* request = service_.findRequest(id);
        if (request == nullptr || request->get_customerId() != actor->get_customerId()) {
            cout << "[LỖI] Không tìm thấy yêu cầu của bạn.\n";
        } else {
            requestDetail(*request);
        }
    }
    pause();
}

void ConsoleUI::myBookingsScreen() {
    const User* actor = service_.currentUser();
    if (actor == nullptr) return;

    vector<Booking> bookings = service_.bookingsForCustomer(actor->get_customerId());
    clearScreen();
    showBanner();
    cout << "--- ĐẶT LỊCH CỦA TÔI ---\n";

    if (bookings.empty()) {
        cout << "Bạn chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string id = askText("Nhập mã booking để xem chi tiết (Enter bỏ qua)", "");
    if (!id.empty()) {
        const Booking* booking = service_.findBooking(id);
        if (booking == nullptr || booking->get_customerId() != actor->get_customerId()) {
            cout << "[LỖI] Không tìm thấy booking của bạn.\n";
        } else {
            bookingDetail(*booking);
            cout << "\nCác khoản thanh toán:\n";
            printPayments(service_.paymentsOf(id));
        }
    }
    pause();
}

void ConsoleUI::customerCancelRequestScreen() {
    const User* actor = service_.currentUser();
    if (actor == nullptr) return;

    vector<EventRequest> requests = service_.requestsForCustomer(actor->get_customerId());
    clearScreen();
    showBanner();
    cout << "--- HỦY YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Bạn chưa gửi yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, true);
    string id = askText("Mã yêu cầu cần hủy", "");
    if (id.empty()) return;

    const EventRequest* request = service_.findRequest(id);
    if (request == nullptr || request->get_customerId() != actor->get_customerId()) {
        cout << "[LỖI] Không tìm thấy yêu cầu của bạn.\n";
        pause();
        return;
    }
    if (!isYes(askText("Xác nhận hủy yêu cầu (c/k)", "k"))) return;

    OpResult result = service_.cancelOwnRequest(id);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

// ==================== NGHIEP VU KINH DOANH ====================

void ConsoleUI::allRequestsScreen() {
    vector<EventRequest> requests = service_.allRequests();
    clearScreen();
    showBanner();
    cout << "--- TẤT CẢ YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Chưa có yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, false);
    string id = askText("Nhập mã yêu cầu để xem chi tiết (Enter bỏ qua)", "");
    if (!id.empty()) {
        const EventRequest* request = service_.findRequest(id);
        if (request == nullptr) {
            cout << "[LỖI] Không tìm thấy yêu cầu.\n";
        } else {
            requestDetail(*request);
        }
    }
    pause();
}

void ConsoleUI::editRequestScreen() {
    vector<EventRequest> requests = service_.allRequests();
    clearScreen();
    showBanner();
    cout << "--- SỬA YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Chưa có yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, false);
    string id = askText("Mã yêu cầu cần sửa", "");
    if (id.empty()) return;

    const EventRequest* request = service_.findRequest(id);
    if (request == nullptr) {
        cout << "[LỖI] Không tìm thấy yêu cầu.\n";
        pause();
        return;
    }
    if (request->get_status() == RequestStatus::DaChot || request->get_status() == RequestStatus::BiHuy) {
        cout << "[LỖI] Yêu cầu đã chốt/hủy, không thể sửa.\n";
        pause();
        return;
    }

    cout << "\n(Enter để giữ giá trị cũ)\n";
    int defaultType = static_cast<int>(request->get_type());
    int typeIndex = chooseEventType(defaultType);
    EventType type = static_cast<EventType>(typeIndex);

    string title = askText("Tên sự kiện", request->get_title());
    string name = askText("Họ tên khách hàng", request->get_customerName());
    string phone = askText("Số điện thoại", request->get_customerPhone());

    cout << "\nDanh sách sảnh:\n";
    printHalls(service_.allHalls());
    string hallId = askText("Mã sảnh mong muốn (Enter = chưa xác định)", request->get_hallId());
    if (!hallId.empty() && service_.findHall(hallId) == nullptr) {
        cout << "[LỖI] Mã sảnh không tồn tại.\n";
        pause();
        return;
    }

    DateTime start = askDateTime("Thời gian bắt đầu", request->get_start(), false);
    DateTime end = askDateTime("Thời gian kết thúc", request->get_end(), false);
    while (!(start < end)) {
        cout << "[LỖI] Kết thúc phải sau bắt đầu.\n";
        end = askDateTime("Thời gian kết thúc", request->get_end(), false);
    }

    int guestCount = askInt("Số khách", request->get_guestCount(), 10, service_.maxHallCapacity());
    string notes = askText("Ghi chú", request->get_notes());

    vector<ServiceLineInput> services;
    if (isYes(askText("Sửa danh sách dịch vụ? (c/k)", "k"))) {
        chooseServices(services);
    } else {
        for (const RequestLine& line : request->get_lines()) {
            ServiceLineInput input;
            input.serviceId = line.get_serviceId();
            input.quantity = line.get_quantity();
            services.push_back(input);
        }
    }

    OpResult result = service_.updateRequest(id, type, title, name, phone, hallId, start, end,
                                              guestCount, notes, services);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::changeRequestStatusScreen() {
    vector<EventRequest> requests = service_.allRequests();
    clearScreen();
    showBanner();
    cout << "--- ĐỔI TRẠNG THÁI YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Chưa có yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, false);
    string id = askText("Mã yêu cầu", "");
    if (id.empty()) return;

    const EventRequest* request = service_.findRequest(id);
    if (request == nullptr) {
        cout << "[LỖI] Không tìm thấy yêu cầu.\n";
        pause();
        return;
    }

    cout << "Trạng thái hiện tại: " << toString(request->get_status()) << '\n';
    cout << "1. Chờ\n";
    cout << "2. Đang tư vấn\n";
    cout << "3. Hoàn thành (tự động tạo booking)\n";
    cout << "4. Hủy yêu cầu\n";
    int choice = askInt("Chọn trạng thái", 1, 1, 4);

    RequestStatus status = RequestStatus::Cho;
    if (choice == 2) status = RequestStatus::DangTuVan;
    else if (choice == 3) status = RequestStatus::HoanThanh;
    else if (choice == 4) status = RequestStatus::BiHuy;

    OpResult result = service_.updateRequestStatus(id, status);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::deleteRequestScreen() {
    vector<EventRequest> requests = service_.allRequests();
    clearScreen();
    showBanner();
    cout << "--- XÓA YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Chưa có yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, false);
    string id = askText("Mã yêu cầu cần xóa", "");
    if (id.empty()) return;
    if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) return;

    OpResult result = service_.deleteRequest(id);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::bookingsScreen() {
    vector<Booking> bookings = service_.allBookings();
    clearScreen();
    showBanner();
    cout << "--- DANH SÁCH BOOKING ---\n";

    if (bookings.empty()) {
        cout << "Chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string id = askText("Nhập mã booking để xem chi tiết (Enter bỏ qua)", "");
    if (!id.empty()) {
        const Booking* booking = service_.findBooking(id);
        if (booking == nullptr) {
            cout << "[LỖI] Không tìm thấy booking.\n";
        } else {
            bookingDetail(*booking);
            cout << "\nCác khoản thanh toán:\n";
            printPayments(service_.paymentsOf(id));
        }
    }
    pause();
}

void ConsoleUI::createBookingScreen() {
    vector<EventRequest> requests = service_.allRequests();
    clearScreen();
    showBanner();
    cout << "--- TẠO BOOKING TỪ YÊU CẦU ---\n";

    if (requests.empty()) {
        cout << "Chưa có yêu cầu nào.\n";
        pause();
        return;
    }

    printRequests(requests, false);
    string requestId = askText("Mã yêu cầu", "");
    if (requestId.empty()) return;

    const EventRequest* request = service_.findRequest(requestId);
    if (request == nullptr) {
        cout << "[LỖI] Không tìm thấy yêu cầu.\n";
        pause();
        return;
    }
    if (request->get_status() == RequestStatus::DaChot || request->get_status() == RequestStatus::BiHuy) {
        cout << "[LỖI] Yêu cầu đã chốt/hủy, không tạo booking.\n";
        pause();
        return;
    }

    cout << "\nDanh sách sảnh:\n";
    printHalls(service_.allHalls());
    string hallId = askText("Mã sảnh chốt", request->get_hallId());
    if (hallId.empty()) {
        cout << "[LỖI] Phải chọn sảnh.\n";
        pause();
        return;
    }

    DateTime start = askDateTime("Thời gian bắt đầu", request->get_start(), false);
    DateTime end = askDateTime("Thời gian kết thúc", request->get_end(), false);
    while (!(start < end)) {
        cout << "[LỖI] Kết thúc phải sau bắt đầu.\n";
        end = askDateTime("Thời gian kết thúc", request->get_end(), false);
    }

    int guestCount = askInt("Số khách", request->get_guestCount(), 1, service_.maxHallCapacity());

    OpResult result = service_.createBooking(requestId, hallId, start, end, guestCount);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::cancelBookingScreen() {
    vector<Booking> bookings = service_.allBookings();
    clearScreen();
    showBanner();
    cout << "--- HỦY BOOKING ---\n";

    if (bookings.empty()) {
        cout << "Chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string id = askText("Mã booking cần hủy", "");
    if (id.empty()) return;
    string reason = askText("Lý do hủy", "Khách yêu cầu hủy");
    if (!isYes(askText("Xác nhận hủy (c/k)", "k"))) return;

    OpResult result = service_.cancelBooking(id, reason);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::deleteBookingScreen() {
    vector<Booking> bookings = service_.allBookings();
    clearScreen();
    showBanner();
    cout << "--- XÓA BOOKING CHƯA CỌC ---\n";

    if (bookings.empty()) {
        cout << "Chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string id = askText("Mã booking cần xóa", "");
    if (id.empty()) return;
    if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) return;

    OpResult result = service_.deleteBooking(id);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::bookingsForPaymentScreen() {
    vector<Booking> bookings = service_.allBookings();
    clearScreen();
    showBanner();
    cout << "--- KHOẢN THU THEO BOOKING ---\n";

    if (bookings.empty()) {
        cout << "Chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string id = askText("Mã booking", "");
    if (id.empty()) return;

    const Booking* booking = service_.findBooking(id);
    if (booking == nullptr) {
        cout << "[LỖI] Không tìm thấy booking.\n";
        pause();
        return;
    }

    bookingDetail(*booking);
    cout << "\nCác khoản thanh toán:\n";
    printPayments(service_.paymentsOf(id));
    pause();
}

void ConsoleUI::recordPaymentScreen() {
    vector<Booking> bookings = service_.allBookings();
    clearScreen();
    showBanner();
    cout << "--- GHI NHẬN THANH TOÁN ---\n";

    if (bookings.empty()) {
        cout << "Chưa có booking nào.\n";
        pause();
        return;
    }

    printBookings(bookings);
    string bookingId = askText("Mã booking", "");
    if (bookingId.empty()) return;

    const Booking* booking = service_.findBooking(bookingId);
    if (booking == nullptr) {
        cout << "[LỖI] Không tìm thấy booking.\n";
        pause();
        return;
    }

    cout << "\nCác khoản thu hiện có:\n";
    printPayments(service_.paymentsOf(bookingId));

    int typeIndex = choosePaymentType(0);
    PaymentType type = static_cast<PaymentType>(typeIndex);
    int64_t remaining = booking->get_finalTotal() - service_.paidAmount(bookingId);
    if (remaining < 0) remaining = 0;

    int64_t amount = askMoney("Số tiền thu", remaining);
    PaymentMethod method = choosePaymentMethod(PaymentMethod::ChuyenKhoan);
    DateTime paidAt = askDateTime("Ngày thu", DateTime::now(), false);
    string note = askText("Ghi chú", "Thu tiền");

    OpResult result = service_.recordPayment(bookingId, type, amount, method, paidAt, note);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::updatePaymentScreen() {
    vector<Payment> payments = service_.allPayments();
    clearScreen();
    showBanner();
    cout << "--- SỬA KHOẢN THU ---\n";

    if (payments.empty()) {
        cout << "Chưa có khoản thu nào.\n";
        pause();
        return;
    }

    printPayments(payments);
    string id = askText("Mã khoản thu", "");
    if (id.empty()) return;

    const Payment* payment = nullptr;
    for (const Payment& item : payments) {
        if (item.get_id() == id) {
            payment = &item;
            break;
        }
    }
    if (payment == nullptr) {
        cout << "[LỖI] Không tìm thấy khoản thu.\n";
        pause();
        return;
    }

    int typeIndex = choosePaymentType(static_cast<int>(payment->get_type()));
    PaymentType type = static_cast<PaymentType>(typeIndex);
    int64_t amount = askMoney("Số tiền", payment->get_amount());
    PaymentMethod method = choosePaymentMethod(payment->get_method());
    DateTime paidAt = askDateTime("Ngày thu", payment->get_paidAt(), false);
    string note = askText("Ghi chú", payment->get_note());

    OpResult result = service_.updatePayment(id, type, amount, method, paidAt, note);
    cout << '\n' << "[OK] " << result.message << '\n';
    pause();
}

void ConsoleUI::paymentReceiptScreen() {
    vector<Payment> payments = service_.allPayments();
    clearScreen();
    showBanner();
    cout << "--- IN PHIẾU THU ---\n";

    if (payments.empty()) {
        cout << "Chưa có khoản thu nào.\n";
        pause();
        return;
    }

    printPayments(payments);
    string id = askText("Mã khoản thu", "");
    if (id.empty()) return;

    cout << '\n' << service_.paymentReceipt(id) << '\n';
    pause();
}

void ConsoleUI::lookupHallsScreen() {
    clearScreen();
    showBanner();
    cout << "--- TRA CỨU SẢNH TRỐNG ---\n";

    DateTime start = askDateTime("Thời gian bắt đầu (dd/mm/yyyy hh:mm)");
    DateTime end = askDateTime("Thời gian kết thúc (dd/mm/yyyy hh:mm)");
    while (!(start < end)) {
        cout << "[LỖI] Kết thúc phải sau bắt đầu.\n";
        end = askDateTime("Thời gian kết thúc (dd/mm/yyyy hh:mm)");
    }

    int guestCount = askInt("Số khách", 100, 10, service_.maxHallCapacity());
    int64_t maxPrice = askMoney("Giá thuê tối đa (0 = không giới hạn)", 0);

    vector<Hall> halls = service_.filterHalls(start, end, guestCount, maxPrice);
    cout << '\n';
    if (halls.empty()) {
        cout << "Không còn sảnh trống phù hợp.\n";
    } else {
        cout << "Các sảnh còn trống:\n";
        printHalls(halls);
    }
    pause();
}

void ConsoleUI::customersScreen() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- KHÁCH HÀNG ---\n";
        cout << "1. Tìm / xem danh sách\n";
        cout << "2. Thêm khách hàng\n";
        cout << "3. Sửa khách hàng\n";
        cout << "4. Xóa khách hàng\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }
        if (choice == 0 || choice == 9) return;

        try {
            if (choice == 1) {
                string keyword = askText("Từ khóa (mã / tên / SĐT, Enter = tất cả)", "");
                printCustomers(service_.searchCustomers(keyword));
                pause();
            } else if (choice == 2) {
                string name = askText("Họ tên", "");
                string phone = askText("Số điện thoại", "");
                string email = askText("Email", "");
                string address = askText("Địa chỉ", "");
                string notes = askText("Ghi chú", "");
                OpResult result = service_.addCustomer(name, phone, email, address, notes);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 3) {
                printCustomers(service_.searchCustomers(""));
                string id = askText("Mã khách hàng cần sửa", "");
                if (id.empty()) continue;
                const Customer* customer = service_.findCustomer(id);
                if (customer == nullptr) {
                    cout << "[LỖI] Không tìm thấy khách hàng.\n";
                    pause();
                    continue;
                }
                string name = askText("Họ tên", customer->get_fullName());
                string phone = askText("Số điện thoại", customer->get_phone());
                string email = askText("Email", customer->get_email());
                string address = askText("Địa chỉ", customer->get_address());
                string notes = askText("Ghi chú", customer->get_notes());
                OpResult result = service_.updateCustomer(id, name, phone, email, address, notes);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 4) {
                printCustomers(service_.searchCustomers(""));
                string id = askText("Mã khách hàng cần xóa", "");
                if (id.empty()) continue;
                if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) continue;
                OpResult result = service_.deleteCustomer(id);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::catalogScreen() {
    clearScreen();
    showBanner();
    cout << "--- DANH MỤC SẢNH & DỊCH VỤ ---\n";
    cout << "\nSẢNH:\n";
    printHalls(service_.allHalls());

    string keyword = askText("Tìm dịch vụ theo mã / tên / nhóm (Enter = tất cả)", "");
    cout << "\nDỊCH VỤ:\n";
    printServices(keyword);
    pause();
}

// ==================== NGHIEP VU QUAN LY ====================

void ConsoleUI::manageHallsScreen() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ SẢNH ---\n";
        cout << "1. Danh sách sảnh\n";
        cout << "2. Thêm sảnh\n";
        cout << "3. Sửa sảnh\n";
        cout << "4. Xóa sảnh\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }
        if (choice == 0 || choice == 9) return;

        try {
            if (choice == 1) {
                printHalls(service_.allHalls());
                pause();
            } else if (choice == 2) {
                string name = askText("Tên sảnh", "");
                int capacity = askInt("Sức chứa", 200, 1, 5000);
                int64_t price = askMoney("Giá thuê", 10000000);
                OpResult result = service_.addHall(name, capacity, price);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 3) {
                printHalls(service_.allHalls());
                string id = askText("Mã sảnh cần sửa", "");
                if (id.empty()) continue;
                const Hall* hall = service_.findHall(id);
                if (hall == nullptr) {
                    cout << "[LỖI] Không tìm thấy sảnh.\n";
                    pause();
                    continue;
                }
                string name = askText("Tên sảnh", hall->get_name());
                int capacity = askInt("Sức chứa", hall->get_capacity(), 1, 5000);
                int64_t price = askMoney("Giá thuê", hall->get_rentalPrice());
                int statusIndex = chooseHallStatus(hall->get_status());
                HallStatus status = static_cast<HallStatus>(statusIndex);
                OpResult result = service_.updateHall(id, name, capacity, price, status);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 4) {
                printHalls(service_.allHalls());
                string id = askText("Mã sảnh cần xóa", "");
                if (id.empty()) continue;
                if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) continue;
                OpResult result = service_.deleteHall(id);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::manageServicesScreen() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ DỊCH VỤ ---\n";
        cout << "1. Danh sách / tìm dịch vụ\n";
        cout << "2. Thêm dịch vụ\n";
        cout << "3. Sửa dịch vụ\n";
        cout << "4. Xóa dịch vụ\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }
        if (choice == 0 || choice == 9) return;

        try {
            if (choice == 1) {
                string keyword = askText("Từ khóa (Enter = tất cả)", "");
                printServices(keyword);
                pause();
            } else if (choice == 2) {
                string code = askText("Mã dịch vụ (ví dụ FOD11-P)", "");
                string name = askText("Tên dịch vụ", "");
                string unit = askText("Đơn vị", "VNĐ/gói");
                int64_t price = askMoney("Đơn giá", 0);
                string category = askText("Nhóm dịch vụ", "Khác");
                int kind = askInt("Loại (1 = Ẩm thực, 2 = Trọn gói)", 2, 1, 2);
                OpResult result = service_.addService(code, name, unit, price, category, kind == 1);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 3) {
                printServices("");
                string id = askText("Mã dịch vụ cần sửa (id hoặc code)", "");
                if (id.empty()) continue;
                const Service* service = service_.findService(id);
                if (service == nullptr) {
                    cout << "[LỖI] Không tìm thấy dịch vụ.\n";
                    pause();
                    continue;
                }
                string code = askText("Mã dịch vụ", service->get_code());
                string name = askText("Tên dịch vụ", service->get_name());
                string unit = askText("Đơn vị", service->get_unit());
                int64_t price = askMoney("Đơn giá", service->get_unitPrice());
                string category = askText("Nhóm dịch vụ", service->get_category());
                int active = askInt("Đang cung cấp? (1 = có, 0 = ngừng)", service->is_active() ? 1 : 0, 0, 1);
                OpResult result = service_.updateService(service->get_id(), code, name, unit, price,
                                                         category, active == 1);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 4) {
                printServices("");
                string id = askText("Mã dịch vụ cần xóa (id hoặc code)", "");
                if (id.empty()) continue;
                if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) continue;
                OpResult result = service_.deleteService(id);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::manageUsersScreen() {
    while (service_.isLoggedIn()) {
        clearScreen();
        showBanner();
        cout << "--- QUẢN LÝ TÀI KHOẢN ---\n";
        cout << "1. Danh sách tài khoản\n";
        cout << "2. Tạo tài khoản\n";
        cout << "3. Đổi vai trò\n";
        cout << "4. Khóa / mở khóa\n";
        cout << "5. Đổi mật khẩu\n";
        cout << "6. Xóa tài khoản\n";
        cout << "0. Quay lại\n";
        cout << "9. Về menu chính\n";

        int choice;
        try {
            choice = askInt("Chọn", 0, 0, 9);
        } catch (const Navigation&) {
            return;  // b/m o menu con: quay lai menu vai tro
        }
        if (choice == 0 || choice == 9) return;

        try {
            if (choice == 1) {
                printUsers();
                pause();
            } else if (choice == 2) {
                string username = askText("Tên đăng nhập", "");
                string password = askText("Mật khẩu", "");
                string fullName = askText("Họ tên", "");
                int roleIndex = chooseStaffRole();
                OpResult result = service_.createUser(username, password, fullName,
                                                      static_cast<Role>(roleIndex));
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 3) {
                printUsers();
                string id = askText("Mã tài khoản cần đổi vai trò", "");
                if (id.empty()) continue;
                Role current = Role::Sales;
                bool found = false;
                for (const User& user : service_.allUsers()) {
                    if (user.get_id() == id) {
                        current = user.get_role();
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "[LỖI] Không tìm thấy tài khoản.\n";
                    pause();
                    continue;
                }
                int roleIndex = chooseRole(current);
                OpResult result = service_.updateUserRole(id, static_cast<Role>(roleIndex));
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 4) {
                printUsers();
                string id = askText("Mã tài khoản", "");
                if (id.empty()) continue;
                int active = askInt("1 = mở khóa, 0 = khóa", 1, 0, 1);
                OpResult result = service_.setUserActive(id, active == 1);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 5) {
                printUsers();
                string id = askText("Mã tài khoản", "");
                if (id.empty()) continue;
                string password = askText("Mật khẩu mới", "");
                OpResult result = service_.changePassword(id, password);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            } else if (choice == 6) {
                printUsers();
                string id = askText("Mã tài khoản cần xóa", "");
                if (id.empty()) continue;
                if (!isYes(askText("Xác nhận xóa (c/k)", "k"))) continue;
                OpResult result = service_.deleteUser(id);
                cout << '\n' << "[OK] " << result.message << '\n';
                pause();
            }
        } catch (const BusinessException& error) {
            showBusinessError(error);
            continue;
        } catch (const Navigation& nav) {
            if (nav.target == Navigation::Target::Main) throw;
            // b trong man hinh con: quay lai menu nay.
            continue;
        }
    }
}

void ConsoleUI::eventsScreen() {
    service_.refreshSystem();
    vector<Event> events = service_.allEvents();

    clearScreen();
    showBanner();
    cout << "--- DANH SÁCH SỰ KIỆN ---\n";

    if (events.empty()) {
        cout << "Chưa có sự kiện nào (sự kiện chỉ sinh ra sau khi thu tiền cọc).\n";
        pause();
        return;
    }

    printEvents(events);
    string id = askText("Nhập mã sự kiện để xem chi tiết (Enter bỏ qua)", "");
    if (!id.empty()) {
        const Event* target = nullptr;
        for (const Event& event : events) {
            if (event.get_id() == id) {
                target = &event;
                break;
            }
        }
        if (target == nullptr) {
            cout << "[LỖI] Không tìm thấy sự kiện.\n";
        } else {
            const Customer* customer = service_.findCustomer(target->get_customerId());
            const Hall* hall = service_.findHall(target->get_hallId());
            cout << "\nMã sự kiện : " << target->get_id() << '\n';
            cout << "Booking    : " << target->get_bookingId() << '\n';
            cout << "Khách hàng : " << (customer != nullptr ? customer->get_fullName() : "-") << '\n';
            cout << "Sảnh       : " << (hall != nullptr ? hall->get_name() : "-") << '\n';
            cout << "Bắt đầu    : " << target->get_start() << '\n';
            cout << "Kết thúc   : " << target->get_end() << '\n';
            cout << "Trạng thái : " << toString(target->get_status()) << '\n';
            cout << "Ghi chú    : " << target->get_specialNotes() << '\n';
        }
    }
    pause();
}

// ==================== HAM NHAP LIEU ====================

string ConsoleUI::askText(const string& prompt, const string& defaultValue) const {
    cout << prompt;
    if (!defaultValue.empty()) cout << " [" << defaultValue << "]";
    cout << ": ";

    string line;
    if (!getline(cin, line)) return defaultValue;
    line = trim(line);
    throwIfNavigation(line, navigationEnabled_);
    return line.empty() ? defaultValue : line;
}

int ConsoleUI::askInt(const string& prompt, int defaultValue, int minValue, int maxValue) const {
    while (true) {
        cout << prompt << " [" << defaultValue << "]: ";
        string line;
        if (!getline(cin, line)) return defaultValue;

        line = trim(line);
        if (line.empty()) return defaultValue;
        throwIfNavigation(line, navigationEnabled_);

        optional<int> value = parseInt(line);
        if (!value.has_value()) {
            cout << "  -> Vui lòng nhập số.\n";
            continue;
        }
        if (*value < minValue || *value > maxValue) {
            cout << "  -> Giá trị phải từ " << minValue << " đến " << maxValue << ".\n";
            continue;
        }
        return *value;
    }
}

int64_t ConsoleUI::askMoney(const string& prompt, int64_t defaultValue) const {
    while (true) {
        cout << prompt << " [" << formatMoney(defaultValue) << "]: ";
        string line;
        if (!getline(cin, line)) return defaultValue;

        line = trim(line);
        if (line.empty()) return defaultValue;
        throwIfNavigation(line, navigationEnabled_);

        optional<int64_t> value = parseMoney(line);
        if (!value.has_value() || *value < 0) {
            cout << "  -> Số tiền không hợp lệ.\n";
            continue;
        }
        return *value;
    }
}

DateTime ConsoleUI::askDateTime(const string& prompt, const DateTime& defaultValue,
                                bool allowEmpty) const {
    while (true) {
        cout << prompt;
        if (allowEmpty) cout << " [Enter = bỏ qua]";
        else if (defaultValue.is_valid()) cout << " [" << defaultValue << "]";
        cout << ": ";

        string line;
        if (!getline(cin, line)) return defaultValue;

        line = trim(line);
        throwIfNavigation(line, navigationEnabled_);
        if (line.empty()) {
            if (allowEmpty) return defaultValue;
            if (defaultValue.is_valid()) return defaultValue;
            cout << "  -> Bắt buộc nhập thời gian.\n";
            continue;
        }

        optional<DateTime> value = DateTime::parse(line);
        if (!value.has_value()) {
            cout << "  -> Sai định dạng. Ví dụ: 25/12/2026 18:30\n";
            continue;
        }
        return *value;
    }
}

DateTime ConsoleUI::askDateTime(const string& prompt) const {
    return askDateTime(prompt, DateTime{}, false);
}

int ConsoleUI::chooseEventType(int defaultIndex) const {
    cout << "\nCác loại sự kiện:\n";
    for (int i = 0; i < 10; ++i) {
        EventType type = static_cast<EventType>(i);
        EventTypeInfo info = eventTypeInfo(type);
        cout << "  " << (i + 1) << ". " << info.name << " (" << info.scale << ")\n";
    }
    return askInt("Chọn loại sự kiện", defaultIndex + 1, 1, 10) - 1;
}

int ConsoleUI::choosePaymentType(int defaultIndex) const {
    cout << "\nLoại thanh toán:\n";
    cout << "  1. Đặt cọc lần 1\n";
    cout << "  2. Đặt cọc lần 2\n";
    cout << "  3. Tất toán\n";
    return askInt("Chọn loại", defaultIndex + 1, 1, 3) - 1;
}

PaymentMethod ConsoleUI::choosePaymentMethod(PaymentMethod defaultValue) const {
    cout << "\nHình thức thanh toán:\n";
    cout << "  1. Tiền mặt\n";
    cout << "  2. Chuyển khoản\n";
    cout << "  3. Thẻ tín dụng\n";
    int defaultIndex = static_cast<int>(defaultValue);
    int choice = askInt("Chọn hình thức", defaultIndex + 1, 1, 3);
    return static_cast<PaymentMethod>(choice - 1);
}

int ConsoleUI::chooseRole(Role defaultValue) const {
    cout << "\nVai trò:\n";
    cout << "  1. Nhân viên kinh doanh\n";
    cout << "  2. Quản lý\n";
    cout << "  3. Vận hành\n";
    cout << "  4. Khách hàng\n";
    int defaultIndex = static_cast<int>(defaultValue);
    return askInt("Chọn vai trò", defaultIndex + 1, 1, 4) - 1;
}

int ConsoleUI::chooseStaffRole() const {
    cout << "\nVai trò nhân viên:\n";
    cout << "  1. Nhân viên kinh doanh\n";
    cout << "  2. Quản lý\n";
    cout << "  3. Vận hành\n";
    return askInt("Chọn vai trò", 1, 1, 3) - 1;
}

int ConsoleUI::chooseHallStatus(HallStatus defaultValue) const {
    cout << "\nTrạng thái sảnh:\n";
    cout << "  1. Khả dụng\n";
    cout << "  2. Bảo trì\n";
    cout << "  3. Không khả dụng\n";
    int defaultIndex = static_cast<int>(defaultValue);
    return askInt("Chọn trạng thái", defaultIndex + 1, 1, 3) - 1;
}

void ConsoleUI::chooseServices(vector<ServiceLineInput>& services) const {
    cout << "\nChọn dịch vụ (ví dụ: FOD01-P, DEC01-P, MED07, AV07, HR02, EXT01).\n";
    cout << "Nhập mã dịch vụ, Enter để kết thúc.\n";

    while (true) {
        string code = askText("Mã dịch vụ", "");
        if (code.empty()) break;

        const Service* service = service_.findService(code);
        if (service == nullptr) {
            cout << "  -> Không tìm thấy dịch vụ.\n";
            continue;
        }
        if (!service->is_active()) {
            cout << "  -> Dịch vụ đã ngừng cung cấp.\n";
            continue;
        }

        cout << "  " << service->get_name() << " - " << formatMoney(service->get_unitPrice())
             << " / " << service->get_unit() << '\n';
        int quantity = askInt("Số lượng (0 = tự động theo số khách)", 0, 0, 100000);

        ServiceLineInput input;
        input.serviceId = service->get_id();
        input.quantity = quantity;
        services.push_back(input);

        cout << "  Đã thêm " << service->get_code() << ".\n";
    }

    if (services.empty()) cout << "  (Không chọn dịch vụ nào)\n";
}

string ConsoleUI::chooseCustomer() const {
    vector<Customer> customers = service_.searchCustomers("");
    if (customers.empty()) {
        cout << "Chưa có khách hàng nào. Hãy thêm khách hàng trước.\n";
        pause();
        return "";
    }

    cout << "\nDanh sách khách hàng:\n";
    printCustomers(customers);
    int index = askInt("Chọn số thứ tự khách hàng (1-" + to_string(customers.size()) + ")",
                       1, 1, static_cast<int>(customers.size()));
    return customers[static_cast<size_t>(index - 1)].get_id();
}

// ==================== HAM HIEN THI ====================

void ConsoleUI::printRequests(const vector<EventRequest>& requests, bool customerView) const {
    vector<string> headers = {"Mã", "Loại", "Tên sự kiện", "Bắt đầu", "Khách", "Trạng thái", "Dự kiến"};
    vector<int> widths = {8, 17, 28, 16, 6, 15, 14};

    vector<vector<string>> rows;
    for (const EventRequest& request : requests) {
        string status = customerView ? customerViewOfRequest(request.get_status())
                                     : toString(request.get_status());
        rows.push_back({request.get_id(),
                        toString(request.get_type()),
                        request.get_title(),
                        request.get_start().toString(),
                        to_string(request.get_guestCount()),
                        status,
                        formatMoney(service_.estimateRequest(request))});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printBookings(const vector<Booking>& bookings) const {
    vector<string> headers = {"Mã", "Sảnh", "Bắt đầu", "Khách", "Tổng", "Giảm", "Trạng thái", "Hạn giữ chỗ"};
    vector<int> widths = {8, 10, 16, 6, 14, 13, 16, 16};

    vector<vector<string>> rows;
    for (const Booking& booking : bookings) {
        rows.push_back({booking.get_id(),
                        booking.get_hallId(),
                        booking.get_start().toString(),
                        to_string(booking.get_guestCount()),
                        formatMoney(booking.get_finalTotal()),
                        toString(booking.get_status()),
                        booking.get_holdUntil().toString()});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printPayments(const vector<Payment>& payments) const {
    if (payments.empty()) {
        cout << "(Chưa có khoản thu nào)\n";
        return;
    }

    vector<string> headers = {"Mã", "Booking", "Loại", "Số tiền", "Ngày thu", "Hình thức", "Trạng thái"};
    vector<int> widths = {8, 9, 14, 14, 16, 13, 16};

    vector<vector<string>> rows;
    for (const Payment& payment : payments) {
        rows.push_back({payment.get_id(),
                        payment.get_bookingId(),
                        toString(payment.get_type()),
                        formatMoney(payment.get_amount()),
                        payment.get_paidAt().toString(),
                        toString(payment.get_method()),
                        toString(payment.get_status())});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printEvents(const vector<Event>& events) const {
    vector<string> headers = {"Mã", "Booking", "Sảnh", "Bắt đầu", "Kết thúc", "Trạng thái", "Ghi chú"};
    vector<int> widths = {8, 9, 10, 16, 16, 14, 24};

    vector<vector<string>> rows;
    for (const Event& event : events) {
        rows.push_back({event.get_id(),
                        event.get_bookingId(),
                        event.get_hallId(),
                        event.get_start().toString(),
                        event.get_end().toString(),
                        toString(event.get_status()),
                        event.get_specialNotes()});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printHalls(const vector<Hall>& halls) const {
    if (halls.empty()) {
        cout << "(Không có sảnh)\n";
        return;
    }

    vector<string> headers = {"Mã", "Tên sảnh", "Sức chứa", "Giá thuê", "Trạng thái"};
    vector<int> widths = {8, 22, 9, 15, 15};

    vector<vector<string>> rows;
    for (const Hall& hall : halls) {
        rows.push_back({hall.get_id(), hall.get_name(), to_string(hall.get_capacity()),
                        formatMoney(hall.get_rentalPrice()), toString(hall.get_status())});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printServices(const string& keyword) const {
    vector<const Service*> services = service_.allServices();
    vector<string> headers = {"Mã", "Code", "Tên dịch vụ", "Đơn vị", "Đơn giá", "Loại"};
    vector<int> widths = {8, 12, 38, 15, 14, 10};

    vector<vector<string>> rows;
    for (const Service* service : services) {
        if (!keyword.empty() && !containsIgnoreCase(service->get_code(), keyword) &&
            !containsIgnoreCase(service->get_name(), keyword) &&
            !containsIgnoreCase(service->get_category(), keyword)) {
            continue;
        }
        rows.push_back({service->get_id(), service->get_code(), service->get_name(), service->get_unit(),
                        formatMoney(service->get_unitPrice()), service->typeName()});
    }

    if (rows.empty()) {
        cout << "(Không có dịch vụ phù hợp)\n";
        return;
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printCustomers(const vector<Customer>& customers) const {
    if (customers.empty()) {
        cout << "(Không có khách hàng)\n";
        return;
    }

    vector<string> headers = {"Mã", "Họ tên", "SĐT", "Email", "Địa chỉ"};
    vector<int> widths = {8, 24, 13, 24, 24};

    vector<vector<string>> rows;
    for (const Customer& customer : customers) {
        rows.push_back({customer.get_id(), customer.get_fullName(), customer.get_phone(),
                        customer.get_email(), customer.get_address()});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::printUsers() const {
    vector<User> users = service_.allUsers();
    vector<string> headers = {"Mã", "Tên đăng nhập", "Họ tên", "Vai trò", "Trạng thái"};
    vector<int> widths = {8, 16, 24, 20, 12};

    vector<vector<string>> rows;
    for (const User& user : users) {
        rows.push_back({user.get_id(), user.get_username(), user.get_fullName(), user.roleName(),
                        user.is_active() ? "Hoạt động" : "Đã khóa"});
    }
    printTable(headers, widths, rows);
}

void ConsoleUI::requestDetail(const EventRequest& request) const {
    const User* actor = service_.currentUser();
    bool customerView = actor != nullptr && actor->get_role() == Role::Customer;

    const Customer* customer = service_.findCustomer(request.get_customerId());
    const Hall* hall = service_.findHall(request.get_hallId());

    cout << "\n---------- CHI TIẾT YÊU CẦU ----------\n";
    cout << "Mã yêu cầu   : " << request.get_id() << '\n';
    cout << "Loại sự kiện : " << toString(request.get_type()) << '\n';
    cout << "Tên sự kiện  : " << request.get_title() << '\n';
    cout << "Khách hàng   : " << request.get_customerName() << " - " << request.get_customerPhone() << '\n';
    if (customer != nullptr) cout << "Hồ sơ khách  : " << customer->get_id() << '\n';
    cout << "Sảnh mong muốn: " << (hall != nullptr ? hall->get_name() : "Chưa xác định") << '\n';
    cout << "Bắt đầu      : " << request.get_start() << '\n';
    cout << "Kết thúc     : " << request.get_end() << '\n';
    cout << "Số khách     : " << request.get_guestCount() << '\n';
    cout << "Trạng thái   : "
         << (customerView ? customerViewOfRequest(request.get_status()) : toString(request.get_status()))
         << '\n';
    cout << "Ghi chú      : " << request.get_notes() << '\n';

    cout << "\nDịch vụ đã chọn:\n";
    if (request.get_lines().empty()) {
        cout << "  (Chưa chọn dịch vụ)\n";
    } else {
        for (const RequestLine& line : request.get_lines()) {
            cout << "  - " << line.get_nameSnapshot() << ": " << line.get_quantity() << " x "
                 << formatMoney(line.get_unitPrice()) << " = " << formatMoney(line.get_amount()) << '\n';
        }
    }

    int64_t hallPrice = hall != nullptr ? hall->get_rentalPrice() : 0;
    cout << "\nTiền dịch vụ : " << formatMoney(request.get_servicesTotal()) << '\n';
    cout << "Tiền sảnh    : " << formatMoney(hallPrice) << '\n';
    cout << "Dự kiến      : " << formatMoney(request.get_servicesTotal() + hallPrice) << '\n';
    cout << "--------------------------------------\n";
}

void ConsoleUI::bookingDetail(const Booking& booking) const {
    const Customer* customer = service_.findCustomer(booking.get_customerId());
    const Hall* hall = service_.findHall(booking.get_hallId());

    int64_t paid = service_.paidAmount(booking.get_id());
    int64_t remaining = booking.get_finalTotal() - paid;

    cout << "\n---------- CHI TIẾT BOOKING ----------\n";
    cout << "Mã booking   : " << booking.get_id() << '\n';
    cout << "Từ yêu cầu   : " << booking.get_requestId() << '\n';
    cout << "Khách hàng   : " << (customer != nullptr ? customer->get_fullName() : booking.get_customerId())
         << '\n';
    cout << "Sảnh         : " << (hall != nullptr ? hall->get_name() : booking.get_hallId()) << '\n';
    cout << "Bắt đầu      : " << booking.get_start() << '\n';
    cout << "Kết thúc     : " << booking.get_end() << '\n';
    cout << "Số khách     : " << booking.get_guestCount() << '\n';
    cout << "Tổng dự kiến : " << formatMoney(booking.get_estimatedTotal()) << '\n';
    cout << "Thành tiền   : " << formatMoney(booking.get_finalTotal()) << '\n';
    cout << "Đã thu       : " << formatMoney(paid) << '\n';
    cout << "Còn lại      : " << formatMoney(remaining) << '\n';
    cout << "Trạng thái   : " << toString(booking.get_status()) << '\n';
    cout << "Ngày tạo     : " << booking.get_createdAt() << '\n';
    cout << "Hạn giữ chỗ  : " << booking.get_holdUntil() << '\n';
    cout << "Nhân viên    : " << booking.get_salesUsername() << '\n';
    if (!booking.get_cancelReason().empty())
        cout << "Lý do hủy    : " << booking.get_cancelReason() << '\n';
    cout << "--------------------------------------\n";
}

void ConsoleUI::hallDetail(const Hall& hall) const {
    cout << "\nMã sảnh     : " << hall.get_id() << '\n';
    cout << "Tên sảnh    : " << hall.get_name() << '\n';
    cout << "Sức chứa    : " << hall.get_capacity() << " khách\n";
    cout << "Giá thuê    : " << formatMoney(hall.get_rentalPrice()) << '\n';
    cout << "Trạng thái  : " << toString(hall.get_status()) << '\n';
}
