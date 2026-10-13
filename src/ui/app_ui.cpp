#include "ui/app_ui.hpp"

#include "models/entities.hpp"
#include "services/palace_service.hpp"
#include "utils/helpers.hpp"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>
#include <initializer_list>
#include <optional>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace tdp {
namespace {

using Rows = vector<vector<string>>;

QString qstr(const string& value)
{
    return QString::fromStdString(value);
}

void appendRow(Rows& rows, initializer_list<string> row)
{
    rows.emplace_back(row);
}

bool isOneOf(const QString& value, initializer_list<const char*> choices)
{
    for (const char* choice : choices) {
        if (value == QLatin1String(choice)) return true;
    }
    return false;
}

QTableWidget* makeTable(const QStringList& headers, const Rows& rows, QWidget* parent)
{
    auto* table = new QTableWidget(static_cast<int>(rows.size()), headers.size(), parent);
    table->setHorizontalHeaderLabels(headers);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(true);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(34);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setStyleSheet(
        "QTableWidget { background: white; alternate-background-color: #f4f7fa; "
        "gridline-color: #dce3ea; border: 1px solid #dce3ea; border-radius: 6px; }"
        "QTableWidget::item { padding: 6px; }"
        "QTableWidget::item:selected { background: #dceaf7; color: #1f2937; }"
        "QHeaderView::section { background: #25364a; color: white; padding: 9px; "
        "border: none; font-weight: bold; }");

    for (int row = 0; row < static_cast<int>(rows.size()); ++row) {
        for (int column = 0; column < headers.size(); ++column) {
            const string value = column < static_cast<int>(rows[row].size())
                                          ? rows[row][column]
                                          : string{};
            auto* item = new QTableWidgetItem(qstr(value));
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            table->setItem(row, column, item);
        }
    }
    return table;
}

optional<QStringList> promptFields(QWidget* parent, const QString& title,
                                        const QStringList& labels,
                                        const QStringList& defaults = {},
                                        const QList<int>& passwordFields = {})
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(440);
    auto* layout = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout;
    form->setSpacing(10);
    QList<QLineEdit*> edits;

    for (int i = 0; i < labels.size(); ++i) {
        auto* edit = new QLineEdit(&dialog);
        if (i < defaults.size()) edit->setText(defaults[i]);
        if (passwordFields.contains(i)) edit->setEchoMode(QLineEdit::Password);
        form->addRow(labels[i], edit);
        edits.push_back(edit);
    }
    layout->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) return nullopt;
    QStringList result;
    for (QLineEdit* edit : edits) result.push_back(edit->text().trimmed());
    return result;
}

struct NavigationEntry {
    Action action;
    QString label;
    QString screen;
};

const vector<NavigationEntry>& navigationEntries()
{
    static const vector<NavigationEntry> entries{
        {Action::ManageCustomers, "Khách hàng", "customers"},
        {Action::ManageRequests, "Yêu cầu sự kiện", "requests"},
        {Action::ManageVenues, "Sảnh", "halls"},
        {Action::ManageVenues, "Dịch vụ", "services"},
        {Action::LookupHalls, "Tra cứu sảnh trống", "lookup"},
        {Action::ManageBookings, "Booking", "bookings"},
        {Action::ManagePayments, "Thanh toán", "payments"},
        {Action::ManageOperations, "Sự kiện vận hành", "events"},
        {Action::ManageUsers, "Người dùng", "users"},
    };
    return entries;
}

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(PalaceService& service)
        : service_(service)
    {
        setWindowTitle("Trống Đồng Palace | Quản lý sự kiện");
        resize(1280, 780);
        setMinimumSize(980, 620);

        root_ = new QStackedWidget(this);
        setCentralWidget(root_);
        loginPage_ = buildLoginPage();
        workspacePage_ = buildWorkspacePage();
        root_->addWidget(loginPage_);
        root_->addWidget(workspacePage_);
        root_->setCurrentWidget(loginPage_);
        statusBar()->showMessage("Chào mừng bạn đến với Trống Đồng Palace");
    }

private:
    PalaceService& service_;
    QStackedWidget* root_ = nullptr;
    QWidget* loginPage_ = nullptr;
    QWidget* workspacePage_ = nullptr;
    QComboBox* navigation_ = nullptr;
    QLabel* userLabel_ = nullptr;
    QLabel* pageTitle_ = nullptr;
    QLineEdit* search_ = nullptr;
    QTableWidget* table_ = nullptr;
    QHBoxLayout* actionLayout_ = nullptr;
    QString screen_;

    void notify(const OpResult& result)
    {
        statusBar()->showMessage(qstr(result.message), result.ok ? 5000 : 10000);
        if (result.ok) refreshTable();
    }

    void showError(const QString& message)
    {
        QMessageBox::warning(this, "Dữ liệu chưa hợp lệ", message);
    }

    QString selectedValue(int column = 0) const
    {
        if (!table_ || !table_->currentItem()) return {};
        const int row = table_->currentRow();
        QTableWidgetItem* item = table_->item(row, column);
        return item ? item->text() : QString{};
    }

    bool parseDateFields(const QString& startText, const QString& endText,
                         DateTime& start, DateTime& end)
    {
        const auto parsedStart = DateTime::parse(startText.toStdString());
        const auto parsedEnd = DateTime::parse(endText.toStdString());
        if (!parsedStart || !parsedEnd) {
            showError("Thời gian không hợp lệ. Dùng định dạng YYYY-MM-DD HH:MM.");
            return false;
        }
        start = *parsedStart;
        end = *parsedEnd;
        return true;
    }

    QWidget* buildLoginPage()
    {
        auto* page = new QWidget;
        page->setObjectName("loginPage");
        auto* outer = new QVBoxLayout(page);
        outer->setContentsMargins(30, 30, 30, 30);
        outer->addStretch();

        auto* card = new QWidget(page);
        card->setObjectName("loginCard");
        card->setMaximumWidth(470);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setSizeConstraint(QLayout::SetMinimumSize);
        cardLayout->setContentsMargins(30, 28, 30, 28);
        cardLayout->setSpacing(14);

        auto* title = new QLabel("TRỐNG ĐỒNG PALACE", card);
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 22px; font-weight: 700; color: #25364a;");
        auto* subtitle = new QLabel("Hệ thống quản lý sự kiện", card);
        subtitle->setAlignment(Qt::AlignCenter);
        subtitle->setStyleSheet("color: #718096; font-size: 14px;");
        auto* username = new QLineEdit(card);
        username->setPlaceholderText("Tên đăng nhập");
        auto* password = new QLineEdit(card);
        password->setPlaceholderText("Mật khẩu");
        password->setEchoMode(QLineEdit::Password);
        auto* status = new QLabel(card);
        status->setWordWrap(true);
        status->setStyleSheet("color: #b42318;");

        auto* login = new QPushButton("Đăng nhập", card);
        login->setDefault(true);
        auto* registerButton = new QPushButton("Đăng ký tài khoản mới", card);
        auto* quit = new QPushButton("Thoát", card);
        login->setMinimumHeight(40);
        registerButton->setMinimumHeight(40);
        quit->setMinimumHeight(40);

        cardLayout->addWidget(title);
        cardLayout->addWidget(subtitle);
        cardLayout->addSpacing(8);
        cardLayout->addWidget(username);
        cardLayout->addWidget(password);
        auto* loginButtons = new QHBoxLayout;
        loginButtons->addWidget(login);
        loginButtons->addWidget(quit);
        cardLayout->addLayout(loginButtons);
        cardLayout->addWidget(registerButton);
        cardLayout->addWidget(status);
        outer->addWidget(card, 0, Qt::AlignHCenter);
        outer->addStretch();

        const auto doLogin = [this, username, password, status]() {
            const OpResult result = service_.login(username->text().toStdString(),
                                                    password->text().toStdString());
            if (!result.ok) {
                status->setText(qstr(result.message));
                return;
            }
            password->clear();
            status->clear();
            loadNavigation();
            root_->setCurrentWidget(workspacePage_);
            selectInitialScreen();
        };
        connect(login, &QPushButton::clicked, this, doLogin);
        connect(password, &QLineEdit::returnPressed, this, doLogin);
        connect(registerButton, &QPushButton::clicked, this, [this, username, status]() {
            const auto fields = promptFields(
                this, "Đăng ký tài khoản",
                {"Tên đăng nhập", "Họ và tên", "Mật khẩu (ít nhất 4 ký tự)",
                 "Nhập lại mật khẩu", "Vai trò"},
                {"", "", "", "", "Sales"}, {2, 3});
            if (!fields) return;
            if (!isOneOf((*fields)[4], {"Sales", "Accountant", "Coordinator", "Admin"})) {
                status->setText("Vai trò không hợp lệ.");
                return;
            }
            const Role role = roleFromString((*fields)[4].toStdString());
            const OpResult result = service_.registerUser(
                (*fields)[0].toStdString(), (*fields)[2].toStdString(),
                (*fields)[3].toStdString(), (*fields)[1].toStdString(), role);
            status->setText(qstr(result.message));
            if (result.ok) username->setText((*fields)[0]);
        });
        connect(quit, &QPushButton::clicked, qApp, &QApplication::quit);
        page->setStyleSheet(
            "QWidget#loginPage { background: #edf2f7; }"
            "QWidget#loginCard { background: white; border-radius: 10px; }"
            "QLineEdit { padding: 10px; border: 1px solid #d5dde5; border-radius: 5px; background: white; }"
            "QPushButton { padding: 10px; border: 0; border-radius: 5px; background: #25364a; color: white; }"
            "QPushButton:hover { background: #38516c; }");
        return page;
    }

    QWidget* buildWorkspacePage()
    {
        auto* page = new QWidget;
        page->setObjectName("workspacePage");
        auto* layout = new QVBoxLayout(page);
        layout->setContentsMargins(22, 18, 22, 14);
        layout->setSpacing(14);

        auto* top = new QHBoxLayout;
        auto* brand = new QLabel("TRỐNG ĐỒNG PALACE", page);
        brand->setStyleSheet("font-size: 19px; font-weight: 700; color: #25364a;");
        navigation_ = new QComboBox(page);
        navigation_->setMinimumWidth(230);
        userLabel_ = new QLabel(page);
        userLabel_->setStyleSheet("color: #526174;");
        auto* logout = new QPushButton("Đăng xuất", page);
        auto* quit = new QPushButton("Thoát", page);
        top->addWidget(brand);
        top->addStretch();
        top->addWidget(navigation_);
        top->addWidget(userLabel_);
        top->addWidget(logout);
        top->addWidget(quit);
        layout->addLayout(top);

        pageTitle_ = new QLabel(page);
        pageTitle_->setStyleSheet("font-size: 17px; font-weight: 600; color: #25364a;");
        layout->addWidget(pageTitle_);

        auto* controls = new QHBoxLayout;
        search_ = new QLineEdit(page);
        search_->setPlaceholderText("Tìm khách hàng theo tên hoặc số điện thoại");
        auto* refresh = new QPushButton("Làm mới", page);
        controls->addWidget(search_, 1);
        controls->addWidget(refresh);
        layout->addLayout(controls);

        table_ = makeTable({}, {}, page);
        layout->addWidget(table_, 1);
        actionLayout_ = new QHBoxLayout;
        actionLayout_->setSpacing(8);
        layout->addLayout(actionLayout_);

        connect(navigation_, qOverload<int>(&QComboBox::currentIndexChanged),
                this, [this](int index) { setScreen(index); });
        connect(refresh, &QPushButton::clicked, this, [this]() { refreshTable(); });
        connect(search_, &QLineEdit::returnPressed, this, [this]() { refreshTable(); });
        connect(logout, &QPushButton::clicked, this, [this]() {
            service_.logout();
            root_->setCurrentWidget(loginPage_);
            navigation_->clear();
            statusBar()->showMessage("Đã đăng xuất");
        });
        connect(quit, &QPushButton::clicked, qApp, &QApplication::quit);
        setStyleSheet(
            "QWidget#workspacePage { background: #f5f7fa; color: #263445; }"
            "QWidget#workspacePage QLineEdit, QWidget#workspacePage QComboBox { padding: 7px 8px; border: 1px solid #cfd8e3; "
            "border-radius: 5px; background: white; }"
            "QWidget#workspacePage QPushButton { padding: 8px 12px; border: 0; border-radius: 5px; "
            "background: #25364a; color: white; }"
            "QWidget#workspacePage QPushButton:hover { background: #38516c; }"
            "QStatusBar { background: white; border-top: 1px solid #dce3ea; }");
        return page;
    }

    void loadNavigation()
    {
        navigation_->blockSignals(true);
        navigation_->clear();
        for (const NavigationEntry& entry : navigationEntries()) {
            if (service_.can(entry.action)) {
                navigation_->addItem(entry.label, entry.screen);
            }
        }
        const User* user = service_.currentUser();
        userLabel_->setText(user ? qstr(user->fullName() + " · " + toString(user->role())) : QString{});
        navigation_->blockSignals(false);
    }

    void selectInitialScreen()
    {
        if (navigation_->count() > 0) setScreen(0);
    }

    void setScreen(int index)
    {
        if (index < 0 || index >= navigation_->count()) return;
        screen_ = navigation_->itemData(index).toString();
        pageTitle_->setText(navigation_->currentText());
        search_->setVisible(screen_ == "customers");
        refreshTable();
        addActions();
    }

    QStringList tableHeaders() const
    {
        if (screen_ == "customers") return {"Mã", "Tên", "Điện thoại", "Email", "Địa chỉ"};
        if (screen_ == "requests") return {"Mã", "Khách hàng", "Loại", "Tiêu đề", "Bắt đầu", "Số khách", "Trạng thái"};
        if (screen_ == "halls") return {"Mã sảnh", "Tên sảnh", "Sức chứa", "Hoạt động"};
        if (screen_ == "services") return {"Mã dịch vụ", "Tên dịch vụ", "Đơn vị", "Đơn giá", "Nhóm", "Hoạt động"};
        if (screen_ == "lookup") return {"Mã sảnh", "Tên sảnh", "Sức chứa"};
        if (screen_ == "bookings") return {"Mã", "Sảnh", "Bắt đầu", "Kết thúc", "Số khách", "Trạng thái"};
        if (screen_ == "payments") return {"Mã", "Booking", "Số tiền", "Thời điểm", "Hình thức", "Ghi chú"};
        if (screen_ == "events") return {"Mã sự kiện", "Booking", "Sảnh", "Khách hàng", "Bắt đầu", "Kết thúc", "Trạng thái", "Ghi chú"};
        if (screen_ == "users") return {"Mã", "Tài khoản", "Họ tên", "Vai trò", "Hoạt động"};
        return {};
    }

    Rows getRows()
    {
        Rows rows;
        const Database& db = service_.db();
        if (screen_ == "customers") {
            for (const Customer& customer : service_.searchCustomers(search_->text().toStdString())) {
                appendRow(rows, {customer.id(), customer.name(), customer.phone(), customer.email(), customer.address()});
            }
        } else if (screen_ == "requests") {
            for (const EventRequest& request : db.requests) {
                const Customer* customer = service_.findCustomer(request.customerId());
                appendRow(rows, {request.id(), customer ? customer->name() : request.customerId(),
                                 toString(request.type()), request.title(), request.start().toString(),
                                 to_string(request.guestCount()), toString(request.status())});
            }
        } else if (screen_ == "halls") {
            for (const Hall& hall : db.halls) {
                appendRow(rows, {hall.id(), hall.name(),
                                 to_string(hall.capacity()), hall.active() ? "Có" : "Không"});
            }
        } else if (screen_ == "services") {
            for (const ServiceItem& item : db.services) {
                appendRow(rows, {item.id(), item.name(), item.unit(), formatMoney(item.unitPrice()),
                                 item.category(), item.active() ? "Có" : "Không"});
            }
        } else if (screen_ == "lookup") {
            // Results are populated only after an explicit availability search.
        } else if (screen_ == "bookings") {
            for (const Booking& booking : db.bookings) {
                appendRow(rows, {booking.id(), booking.hallId(), booking.start().toString(), booking.end().toString(),
                                 to_string(booking.guestCount()), toString(booking.status())});
            }
        } else if (screen_ == "payments") {
            for (const Payment& payment : db.payments) {
                appendRow(rows, {payment.id(), payment.bookingId(), formatMoney(payment.amount()),
                                 payment.paidAt().toString(), toString(payment.method()), payment.note()});
            }
        } else if (screen_ == "events") {
            for (const Event& event : db.events) {
                const Customer* customer = service_.findCustomer(event.customerId());
                appendRow(rows, {event.id(), event.bookingId(), event.hallId(),
                                 customer ? customer->name() : event.customerId(),
                                 event.start().toString(), event.end().toString(),
                                 toString(event.status()), event.specialNotes()});
            }
        } else if (screen_ == "users") {
            for (const User& user : db.users) {
                appendRow(rows, {user.id(), user.username(), user.fullName(), toString(user.role()),
                                 user.active() ? "Có" : "Không"});
            }
        }
        return rows;
    }

    void refreshTable()
    {
        if (!table_ || screen_.isEmpty()) return;
        const Rows rows = getRows();
        QTableWidget* replacement = makeTable(tableHeaders(), rows, workspacePage_);
        auto* layout = qobject_cast<QVBoxLayout*>(workspacePage_->layout());
        if (!layout) return;
        layout->replaceWidget(table_, replacement);
        table_->deleteLater();
        table_ = replacement;
    }

    void addActionButton(QHBoxLayout* layout, const QString& text, const function<void()>& action)
    {
        auto* button = new QPushButton(text, workspacePage_);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, action);
    }

    void addActions()
    {
        QHBoxLayout* layout = actionLayout_;
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* widget = item->widget()) widget->deleteLater();
            delete item;
        }
        if (screen_ == "customers") {
            addActionButton(layout, "Thêm khách hàng", [this]() { addCustomer(); });
            addActionButton(layout, "Sửa khách hàng", [this]() { editCustomer(); });
        } else if (screen_ == "requests") {
            addActionButton(layout, "Tiếp nhận yêu cầu", [this]() { addRequest(); });
            addActionButton(layout, "Đánh dấu đã báo giá", [this]() { setRequestStatus(RequestStatus::DaBaoGia); });
            addActionButton(layout, "Hủy yêu cầu", [this]() { setRequestStatus(RequestStatus::Huy); });
        } else if (screen_ == "halls") {
            addActionButton(layout, "Thêm sảnh", [this]() { addHall(); });
            addActionButton(layout, "Sửa sảnh", [this]() { editHall(); });
            addActionButton(layout, "Bật / tắt sảnh", [this]() { toggleHall(); });
        } else if (screen_ == "services") {
            addActionButton(layout, "Thêm dịch vụ", [this]() { addService(); });
            addActionButton(layout, "Sửa dịch vụ", [this]() { editService(); });
            addActionButton(layout, "Bật / tắt dịch vụ", [this]() { toggleService(); });
        } else if (screen_ == "lookup") {
            addActionButton(layout, "Tra cứu theo thời gian", [this]() { lookupHalls(); });
        } else if (screen_ == "bookings") {
            addActionButton(layout, "Tạo booking", [this]() { createBooking(); });
            addActionButton(layout, "Xác nhận booking", [this]() { changeBookingStatus(true); });
            addActionButton(layout, "Hủy booking", [this]() { changeBookingStatus(false); });
        } else if (screen_ == "payments") {
            addActionButton(layout, "Ghi nhận thanh toán", [this]() { recordPayment(); });
        } else if (screen_ == "events") {
            addActionButton(layout, "Cập nhật sự kiện", [this]() { operateOnSelection(); });
        } else if (screen_ == "users") {
            addActionButton(layout, "Tạo người dùng", [this]() { createUser(); });
            addActionButton(layout, "Khóa / mở khóa", [this]() { toggleUser(); });
            addActionButton(layout, "Đổi mật khẩu", [this]() { changePassword(); });
        }
        layout->addStretch();
    }

    void addCustomer()
    {
        const auto values = promptFields(this, "Thêm khách hàng",
            {"Họ tên / Tên công ty", "Số điện thoại", "Email", "Địa chỉ", "Ghi chú"});
        if (!values) return;
        notify(service_.addCustomer((*values)[0].toStdString(), (*values)[1].toStdString(),
                                    (*values)[2].toStdString(), (*values)[3].toStdString(),
                                    (*values)[4].toStdString()));
    }

    void editCustomer()
    {
        const QString id = selectedValue();
        const Customer* customer = service_.findCustomer(id.toStdString());
        if (!customer) { showError("Hãy chọn một khách hàng trong bảng."); return; }
        const auto values = promptFields(this, "Sửa khách hàng",
            {"Họ tên / Tên công ty", "Số điện thoại", "Email", "Địa chỉ", "Ghi chú"},
            {qstr(customer->name()), qstr(customer->phone()), qstr(customer->email()),
             qstr(customer->address()), qstr(customer->notes())});
        if (!values) return;
        notify(service_.updateCustomer(id.toStdString(), (*values)[0].toStdString(),
                                       (*values)[1].toStdString(), (*values)[2].toStdString(),
                                       (*values)[3].toStdString(), (*values)[4].toStdString()));
    }

    void addRequest()
    {
        const auto values = promptFields(this, "Tiếp nhận yêu cầu sự kiện",
            {"Mã khách hàng", "Loại (TiecCuoi/HoiNghi/SinhNhat/SuKienDoanhNghiep/Khac)",
             "Tiêu đề", "Bắt đầu (YYYY-MM-DD HH:MM)", "Kết thúc (YYYY-MM-DD HH:MM)",
             "Số lượng khách", "Yêu cầu đặc biệt"},
            {"", "TiecCuoi", "", "2026-12-20 18:00", "2026-12-20 22:00", "100", ""});
        if (!values) return;
        if (!isOneOf((*values)[1], {"TiecCuoi", "HoiNghi", "SinhNhat", "SuKienDoanhNghiep", "Khac"})) {
            showError("Loại sự kiện không hợp lệ.");
            return;
        }
        DateTime start, end;
        if (!parseDateFields((*values)[3], (*values)[4], start, end)) return;
        const auto guests = parseInt((*values)[5].toStdString());
        if (!guests) { showError("Số lượng khách không hợp lệ."); return; }
        notify(service_.addRequest((*values)[0].toStdString(),
            eventTypeFromString((*values)[1].toStdString()), (*values)[2].toStdString(),
            start, end, *guests, (*values)[6].toStdString()));
    }

    void setRequestStatus(RequestStatus status)
    {
        const QString id = selectedValue();
        if (id.isEmpty()) { showError("Hãy chọn một yêu cầu trong bảng."); return; }
        notify(service_.updateRequestStatus(id.toStdString(), status));
    }

    void addHall()
    {
        const auto values = promptFields(this, "Thêm sảnh", {"Tên sảnh", "Sức chứa"},
                                         {"", "200"});
        if (!values) return;
        const auto capacity = parseInt((*values)[1].toStdString());
        if (!capacity) { showError("Sức chứa không hợp lệ."); return; }
        notify(service_.addHall((*values)[0].toStdString(), *capacity));
    }

    void addService()
    {
        const auto values = promptFields(this, "Thêm dịch vụ",
            {"Tên dịch vụ", "Đơn vị", "Đơn giá (VND)", "Nhóm"},
            {"", "gói", "1000000", "Dịch vụ"});
        if (!values) return;
        const auto price = parseMoney((*values)[2].toStdString());
        if (!price) { showError("Đơn giá không hợp lệ."); return; }
        notify(service_.addService((*values)[0].toStdString(), (*values)[1].toStdString(),
                                   *price, (*values)[3].toStdString()));
    }

    void editHall()
    {
        const QString id = selectedValue();
        const Hall* hall = service_.findHall(id.toStdString());
        if (!hall) { showError("Hãy chọn sảnh trong bảng."); return; }
        const auto values = promptFields(this, "Cập nhật sảnh",
            {"Tên sảnh", "Sức chứa"},
            {qstr(hall->name()), QString::number(hall->capacity())});
        if (!values) return;
        const auto capacity = parseInt((*values)[1].toStdString());
        if (!capacity) { showError("Sức chứa không hợp lệ."); return; }
        notify(service_.updateHall(hall->id(), (*values)[0].toStdString(), *capacity, hall->active()));
    }

    void editService()
    {
        const QString id = selectedValue();
        const ServiceItem* item = service_.findService(id.toStdString());
        if (!item) { showError("Hãy chọn dịch vụ trong bảng."); return; }
        const auto values = promptFields(this, "Cập nhật dịch vụ",
            {"Tên dịch vụ", "Đơn vị", "Đơn giá (VND)", "Nhóm"},
            {qstr(item->name()), qstr(item->unit()), QString::number(item->unitPrice()),
             qstr(item->category())});
        if (!values) return;
        const auto price = parseMoney((*values)[2].toStdString());
        if (!price) { showError("Đơn giá không hợp lệ."); return; }
        notify(service_.updateService(item->id(), (*values)[0].toStdString(),
                                      (*values)[1].toStdString(), *price,
                                      (*values)[3].toStdString(), item->active()));
    }

    void toggleHall()
    {
        const QString id = selectedValue();
        const Hall* hall = service_.findHall(id.toStdString());
        if (!hall) { showError("Hãy chọn sảnh trong bảng."); return; }
        notify(service_.updateHall(hall->id(), hall->name(), hall->capacity(), !hall->active()));
    }

    void toggleService()
    {
        const QString id = selectedValue();
        const ServiceItem* item = service_.findService(id.toStdString());
        if (!item) { showError("Hãy chọn dịch vụ trong bảng."); return; }
        notify(service_.updateService(item->id(), item->name(), item->unit(), item->unitPrice(),
                                      item->category(), !item->active()));
    }

    void lookupHalls()
    {
        const auto values = promptFields(this, "Tra cứu sảnh trống",
            {"Bắt đầu (YYYY-MM-DD HH:MM)", "Kết thúc (YYYY-MM-DD HH:MM)", "Số lượng khách"},
            {"2026-12-20 18:00", "2026-12-20 22:00", "200"});
        if (!values) return;
        DateTime start, end;
        if (!parseDateFields((*values)[0], (*values)[1], start, end)) return;
        const auto guests = parseInt((*values)[2].toStdString());
        if (!guests) { showError("Số lượng khách không hợp lệ."); return; }
        Rows rows;
        for (const Hall& hall : service_.findAvailableHalls(start, end, *guests, "")) {
            appendRow(rows, {hall.id(), hall.name(), to_string(hall.capacity())});
        }
        QTableWidget* replacement = makeTable(tableHeaders(), rows, workspacePage_);
        auto* layout = qobject_cast<QVBoxLayout*>(workspacePage_->layout());
        layout->replaceWidget(table_, replacement);
        table_->deleteLater();
        table_ = replacement;
        statusBar()->showMessage(QString("Tìm thấy %1 sảnh phù hợp").arg(rows.size()), 5000);
    }

    void createBooking()
    {
        const auto values = promptFields(this, "Tạo booking từ yêu cầu",
            {"Mã yêu cầu", "Mã sảnh", "Bắt đầu (YYYY-MM-DD HH:MM)",
             "Kết thúc (YYYY-MM-DD HH:MM)", "Số lượng khách"},
            {"", "", "2026-12-20 18:00", "2026-12-20 22:00", "100"});
        if (!values) return;
        DateTime start, end;
        if (!parseDateFields((*values)[2], (*values)[3], start, end)) return;
        const auto guests = parseInt((*values)[4].toStdString());
        if (!guests) { showError("Số lượng khách không hợp lệ."); return; }
        notify(service_.createBooking((*values)[0].toStdString(), (*values)[1].toStdString(),
                                      start, end, *guests));
    }

    void changeBookingStatus(bool confirm)
    {
        const QString id = selectedValue();
        if (id.isEmpty()) { showError("Hãy chọn booking trong bảng."); return; }
        if (confirm) {
            notify(service_.confirmBooking(id.toStdString()));
            return;
        }
        const auto values = promptFields(this, "Hủy booking", {"Lý do"}, {"Hủy từ màn hình booking"});
        if (!values) return;
        notify(service_.cancelBooking(id.toStdString(), (*values)[0].toStdString()));
    }

    void recordPayment()
    {
        const auto values = promptFields(this, "Ghi nhận thanh toán",
            {"Mã booking", "Số tiền (VND)", "Hình thức (TienMat/ChuyenKhoan/The)", "Ghi chú"},
            {"", "1000000", "ChuyenKhoan", ""});
        if (!values) return;
        if (!isOneOf((*values)[2], {"TienMat", "ChuyenKhoan", "The"})) {
            showError("Hình thức thanh toán không hợp lệ.");
            return;
        }
        const auto amount = parseMoney((*values)[1].toStdString());
        if (!amount) { showError("Số tiền không hợp lệ."); return; }
        notify(service_.recordPayment((*values)[0].toStdString(), *amount,
            paymentMethodFromString((*values)[2].toStdString()), (*values)[3].toStdString()));
    }

    void operateOnSelection()
    {
        const QString id = selectedValue();
        if (id.isEmpty()) { showError("Hãy chọn một mục trong bảng."); return; }
        if (screen_ == "events") {
            const auto values = promptFields(this, "Cập nhật sự kiện",
                {"Trạng thái (ChuanBi/DangDienRa/HoanTat/Huy)", "Ghi chú"},
                {"DangDienRa", ""});
            if (!values) return;
            const string status = (*values)[0].toStdString();
            if (!isOneOf((*values)[0], {"ChuanBi", "DangDienRa", "HoanTat", "Huy"})) {
                showError("Trạng thái sự kiện không hợp lệ.");
                return;
            }
            OpResult result = status == "HoanTat"
                                  ? service_.completeEvent(id.toStdString())
                                  : service_.updateEventStatus(id.toStdString(), eventStatusFromString(status));
            if (result.ok && !(*values)[1].isEmpty()) {
                result = service_.setEventNotes(id.toStdString(), (*values)[1].toStdString());
            }
            notify(result);
        }
    }

    void createUser()
    {
        const auto values = promptFields(this, "Tạo người dùng",
            {"Tên đăng nhập", "Mật khẩu", "Họ tên", "Vai trò (Admin/Sales/Accountant/Coordinator)"},
            {"", "", "", "Sales"}, {1});
        if (!values) return;
        if (!isOneOf((*values)[3], {"Admin", "Sales", "Accountant", "Coordinator"})) {
            showError("Vai trò không hợp lệ.");
            return;
        }
        notify(service_.createUser((*values)[0].toStdString(), (*values)[1].toStdString(),
                                   (*values)[2].toStdString(),
                                   roleFromString((*values)[3].toStdString())));
    }

    void toggleUser()
    {
        const QString id = selectedValue();
        if (id.isEmpty()) { showError("Hãy chọn người dùng trong bảng."); return; }
        for (const User& user : service_.db().users) {
            if (user.id() == id.toStdString()) {
                notify(service_.setUserActive(user.id(), !user.active()));
                return;
            }
        }
        showError("Không tìm thấy người dùng.");
    }

    void changePassword()
    {
        const QString id = selectedValue();
        if (id.isEmpty()) { showError("Hãy chọn người dùng trong bảng."); return; }
        const auto values = promptFields(this, "Đổi mật khẩu", {"Mật khẩu mới"}, {}, {0});
        if (!values) return;
        notify(service_.changePassword(id.toStdString(), (*values)[0].toStdString()));
    }
};

}  // namespace

int runQtUi(PalaceService& service, int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("TrongDongPalace");
    app.setOrganizationName("TrongDongPalace");
    MainWindow window(service);
    window.show();
    return app.exec();
}

}  // namespace tdp
