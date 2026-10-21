#pragma once

// Giao dien console: chi lo viec hien thi menu va doc ban phim.
// Moi thao tac that su deu goi xuong PalaceService.

#include "PalaceService.h"

using namespace std;

class ConsoleUI {
public:
    explicit ConsoleUI(PalaceService& service);

    // Vong lap chinh cua chuong trinh. Tra ve ma thoat (0 = binh thuong).
    int run();

private:
    PalaceService& service_;
    mutable bool navigationEnabled_ = false;

    // ---------- Man hinh chung ----------
    // showNavigation = true: hien thi goi y [b] quay lai / [m] ve menu chinh
    // va bat cac lenh dieu huong nay o cac ham nhap lieu.
    void showBanner(bool showNavigation = true) const;
    void showBusinessError(const BusinessException& error) const;
    bool loginScreen();
    void registerScreen();
    void mainMenu();

    // ---------- Menu theo vai tro ----------
    void customerMenu();
    void salesMenu();
    void managerMenu();
    void operationMenu();
    void requestMenu();
    void bookingMenu();
    void paymentMenu();

    // ---------- Nghiep vu khach hang ----------
    void createRequestScreen();
    void myRequestsScreen();
    void myBookingsScreen();
    void customerCancelRequestScreen();

    // ---------- Nghiep vu kinh doanh ----------
    void allRequestsScreen();
    void editRequestScreen();
    void changeRequestStatusScreen();
    void deleteRequestScreen();
    void bookingsScreen();
    void createBookingScreen();
    void updateDiscountScreen();
    void cancelBookingScreen();
    void deleteBookingScreen();
    void paymentsScreen();
    void bookingsForPaymentScreen();
    void recordPaymentScreen();
    void updatePaymentScreen();
    void paymentReceiptScreen();
    void lookupHallsScreen();
    void customersScreen();
    void catalogScreen();

    // ---------- Nghiep vu quan ly ----------
    void manageHallsScreen();
    void manageServicesScreen();
    void manageUsersScreen();
    void approveDiscountScreen();

    // ---------- Nghiep vu van hanh ----------
    void eventsScreen();

    // ---------- Ham nhap lieu ----------
    string askText(const string& prompt, const string& defaultValue = "") const;
    int askInt(const string& prompt, int defaultValue, int minValue, int maxValue) const;
    int64_t askMoney(const string& prompt, int64_t defaultValue) const;
    DateTime askDateTime(const string& prompt, const DateTime& defaultValue, bool allowEmpty) const;
    DateTime askDateTime(const string& prompt) const;
    int chooseEventType(int defaultIndex = 0) const;
    int choosePaymentType(int defaultIndex = 0) const;
    PaymentMethod choosePaymentMethod(PaymentMethod defaultValue = PaymentMethod::ChuyenKhoan) const;
    int chooseRole(Role defaultValue = Role::Sales) const;
    int chooseStaffRole() const;
    int chooseHallStatus(HallStatus defaultValue = HallStatus::KhaDung) const;
    void chooseServices(vector<ServiceLineInput>& services) const;
    string chooseCustomer() const;

    // ---------- Ham hien thi ----------
    void printRequests(const vector<EventRequest>& requests, bool customerView) const;
    void printBookings(const vector<Booking>& bookings) const;
    void printPayments(const vector<Payment>& payments) const;
    void printEvents(const vector<Event>& events) const;
    void printHalls(const vector<Hall>& halls) const;
    void printServices(const string& keyword, bool onlyActive = false) const;
    void printCustomers(const vector<Customer>& customers) const;
    void printUsers() const;
    void requestDetail(const EventRequest& request) const;
    void bookingDetail(const Booking& booking) const;
    void hallDetail(const Hall& hall) const;

    void pause() const;
    void clearScreen() const;
};
