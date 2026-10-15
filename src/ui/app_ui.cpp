#include "ui/app_ui.hpp"

#include "models/Database.hpp"
#include "services/palace_service.hpp"
#include "utils/helpers.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
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

// Dropdown (Form 1/2/3: loai su kien, sanh, trang thai, loai/hinh thuc thanh toan).
optional<QString> promptCombo(QWidget* parent, const QString& title, const QString& label,
                              const QStringList& items, const QString& current = {})
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(400);
    auto* layout = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout;
    auto* combo = new QComboBox(&dialog);
    combo->addItems(items);
    if (!current.isEmpty()) {
        const int idx = combo->findText(current);
        if (idx >= 0) combo->setCurrentIndex(idx);
    }
    form->addRow(label, combo);
    layout->addLayout(form);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (dialog.exec() != QDialog::Accepted) return nullopt;
    return combo->currentText();
}

// Date-picker (Form 1: ngay dien ra chi tuong lai; Form 3: mac dinh hom nay).
optional<DateTime> promptDateTime(QWidget* parent, const QString& title, const QString& label,
                                  DateTime initial, bool calendarPopup = true)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setMinimumWidth(400);
    auto* layout = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout;
    auto* edit = new QDateTimeEdit(&dialog);
    edit->setDisplayFormat("yyyy-MM-dd HH:mm");
    edit->setCalendarPopup(calendarPopup);
    edit->setDateTime(QDateTime(QDate(initial.year, initial.month, initial.day),
                                QTime(initial.hour, initial.minute)));
    form->addRow(label, edit);
    layout->addLayout(form);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (dialog.exec() != QDialog::Accepted) return nullopt;
    const QDateTime qdt = edit->dateTime();
    DateTime d;
    d.year = qdt.date().year();
    d.month = qdt.date().month();
    d.day = qdt.date().day();
    d.hour = qdt.time().hour();
    d.minute = qdt.time().minute();
    return d;
}

DateTime parseDateTimeText(const QString& text, bool& ok)
{
    auto parsed = DateTime::parse(text.toStdString());
    ok = parsed.has_value();
    return ok ? *parsed : DateTime{};
}

struct NavigationEntry {
    Action action;
    QString label;
    QString screen;
};

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
    QWidget* content_ = nullptr;
    QTabWidget* serviceTabs_ = nullptr;
    int serviceTab_ = 0;
    // Khung loc tra cuu sanh (hien + loc chung 1 khung).
    QWidget* lookupControls_ = nullptr;
    QDateTimeEdit* fStart_ = nullptr;
    QDateTimeEdit* fEnd_ = nullptr;
    QSpinBox* fGuests_ = nullptr;
    QLineEdit* fMaxPrice_ = nullptr;
    bool lookupActive_ = false;
    DateTime lStart_, lEnd_;
    int lGuests_ = 0;
    int64_t lMaxPrice_ = 0;
    QHBoxLayout* actionLayout_ = nullptr;
    QString screen_;

    void notify(const OpResult& result)
    {
        if (!result.ok) {
            QMessageBox::warning(this, "Không lưu được", qstr(result.message));
            return;
        }
        statusBar()->showMessage(qstr(result.message), 5000);
        loadNavigation();
        refreshTable();
    }

    void showError(const QString& message)
    {
        QMessageBox::warning(this, "Dữ liệu chưa hợp lệ", message);
    }

    void showInfo(const QString& title, const QString& message)
    {
        QMessageBox::information(this, title, message);
    }

    QString selectedValue(int column = 0) const
    {
        if (!table_ || !table_->currentItem()) return {};
        const int row = table_->currentRow();
        QTableWidgetItem* item = table_->item(row, column);
        return item ? item->text() : QString{};
    }

    bool isCustomer() const
    {
        const User* u = service_.currentUser();
        return u && u->role() == Role::Customer;
    }

    bool isManager() const
    {
        const User* u = service_.currentUser();
        return u && u->role() == Role::Manager;
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
        auto* registerButton = new QPushButton("Đăng ký tài khoản khách hàng", card);
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
                this, "Đăng ký tài khoản khách hàng",
                {"Tên đăng nhập", "Họ và tên", "Số điện thoại", "Email", "Địa chỉ",
                 "Mật khẩu (ít nhất 4 ký tự)", "Nhập lại mật khẩu"},
                {"", "", "", "", "", "", ""}, {5, 6});
            if (!fields) return;
            const OpResult result = service_.registerUser(
                (*fields)[0].toStdString(), (*fields)[5].toStdString(),
                (*fields)[6].toStdString(), (*fields)[1].toStdString(),
                (*fields)[2].toStdString(), (*fields)[3].toStdString(),
                (*fields)[4].toStdString());
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

        // Khung loc tra cuu: suc chua + thoi gian + gia tien (chi hien o tab tra cuu).
        lookupControls_ = new QWidget(page);
        auto* filter = new QHBoxLayout(lookupControls_);
        filter->setContentsMargins(0, 0, 0, 0);
        DateTime defStart = DateTime::now().plusHours(24);
        fStart_ = new QDateTimeEdit(page);
        fStart_->setDisplayFormat("yyyy-MM-dd HH:mm");
        fStart_->setCalendarPopup(true);
        fStart_->setDateTime(QDateTime(QDate(defStart.year, defStart.month, defStart.day),
                                       QTime(defStart.hour, defStart.minute)));
        fEnd_ = new QDateTimeEdit(page);
        fEnd_->setDisplayFormat("yyyy-MM-dd HH:mm");
        fEnd_->setCalendarPopup(true);
        DateTime defEnd = defStart.plusHours(4);
        fEnd_->setDateTime(QDateTime(QDate(defEnd.year, defEnd.month, defEnd.day),
                                     QTime(defEnd.hour, defEnd.minute)));
        fGuests_ = new QSpinBox(page);
        fGuests_->setRange(10, 800);
        fGuests_->setValue(100);
        fGuests_->setPrefix("Khách: ");
        fMaxPrice_ = new QLineEdit(page);
        fMaxPrice_->setPlaceholderText("Giá thuê tối đa (trống = không giới hạn)");
        auto* doLookup = new QPushButton("Tra cứu", page);
        auto* showAll = new QPushButton("Hiện tất cả", page);
        filter->addWidget(new QLabel("Từ:", page));
        filter->addWidget(fStart_);
        filter->addWidget(new QLabel("Đến:", page));
        filter->addWidget(fEnd_);
        filter->addWidget(fGuests_);
        filter->addWidget(fMaxPrice_, 1);
        filter->addWidget(doLookup);
        filter->addWidget(showAll);
        lookupControls_->setVisible(false);
        layout->addWidget(lookupControls_);
        connect(doLookup, &QPushButton::clicked, this, [this]() { lookupHalls(); });
        connect(showAll, &QPushButton::clicked, this, [this]() {
            lookupActive_ = false;
            refreshTable();
        });

        table_ = makeTable({}, {}, page);
        content_ = table_;
        layout->addWidget(content_, 1);
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

    // Dieu huong theo Actor (dac ta muc 1).
    vector<NavigationEntry> navigationEntries() const
    {
        const User* u = service_.currentUser();
        if (!u) return {};
        if (u->role() == Role::Customer) {
            return {{Action::ManageRequests, "Yêu cầu đã gửi", "requests"},
                    {Action::ManageRequests, "Đặt lịch của tôi", "mybookings"},
                    {Action::LookupHalls, "Sảnh & dịch vụ", "halls"}};
        }
        if (u->role() == Role::Operation) {
            return {{Action::ManageEvents, "Danh sách sự kiện", "events"}};
        }
        vector<NavigationEntry> entries{
            {Action::ManageRequests, "Yêu cầu sự kiện", "requests"},
            {Action::ManageBookings, "Booking", "bookings"},
            {Action::ManagePayments, "Thanh toán", "payments"},
            {Action::ManageVenues, "Sảnh & địa điểm", "halls"},
            {Action::ManageVenues, "Danh mục dịch vụ", "services"},
            {Action::LookupHalls, "Tra cứu sảnh trống", "lookup"},
            {Action::ManageCustomers, "Khách hàng", "customers"},
            {Action::ManageEvents, "Sự kiện", "events"},
        };
        if (u->role() == Role::Manager) {
            entries.push_back({Action::ManageUsers, "Người dùng", "users"});
        }
        return entries;
    }

    void loadNavigation()
    {
        const QString keepScreen = screen_;
        navigation_->blockSignals(true);
        navigation_->clear();
        for (const NavigationEntry& entry : navigationEntries()) {
            if (service_.can(entry.action)) {
                QString label = entry.label;
                // Thong bao yeu cau moi cho sales (dac ta B.1).
                if (entry.screen == "requests" && !isCustomer()) {
                    const size_t n = service_.countNewRequests();
                    if (n > 0) label += QString(" (%1 mới)").arg(n);
                }
                navigation_->addItem(label, entry.screen);
            }
        }
        const User* user = service_.currentUser();
        userLabel_->setText(user ? qstr(user->fullName() + " · " + toString(user->role())) : QString{});
        navigation_->blockSignals(false);
        // Giu dung man hinh dang xem sau khi luu (khong bat ve tab dau).
        for (int i = 0; i < navigation_->count(); ++i) {
            if (navigation_->itemData(i).toString() == keepScreen) {
                if (navigation_->currentIndex() != i) navigation_->setCurrentIndex(i);
                break;
            }
        }
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
        lookupControls_->setVisible(screen_ == "lookup");
        refreshTable();
        addActions();
    }

    QStringList tableHeaders() const
    {
        if (screen_ == "customers") return {"Mã", "Tên", "Điện thoại", "Email", "Địa chỉ"};
        if (screen_ == "requests") {
            if (isCustomer())
                return {"Mã", "Loại sự kiện", "Tên sự kiện", "Bắt đầu", "Số khách", "Chi phí dự kiến",
                        "Trạng thái"};
            return {"Mã", "Khách hàng", "Loại", "Tên sự kiện", "Sảnh", "Bắt đầu", "Số khách",
                    "Chi phí dự kiến", "Trạng thái"};
        }
        if (screen_ == "mybookings")
            return {"Mã", "Sảnh", "Bắt đầu", "Kết thúc", "Tổng booking", "Đã thu", "Trạng thái"};
        if (screen_ == "halls") return {"Mã", "Tên sảnh", "Sức chứa", "Giá thuê", "Trạng thái"};
        if (screen_ == "services") return serviceHeaders(serviceTab_);
        if (screen_ == "lookup")
            return {"Mã sảnh", "Tên sảnh", "Sức chứa", "Giá thuê", "Trạng thái sảnh", "Hiện tại"};
        if (screen_ == "bookings")
            return {"Mã", "Yêu cầu", "Sảnh", "Bắt đầu", "Tổng", "Giảm giá", "Trạng thái", "NV phụ trách"};
        if (screen_ == "payments")
            return {"Mã", "Booking", "Loại", "Số tiền", "Ngày thu", "Hình thức", "Trạng thái", "Ghi chú"};
        if (screen_ == "events")
            return {"Mã sự kiện", "Booking", "Sảnh", "Khách hàng", "Bắt đầu", "Kết thúc", "Trạng thái"};
        if (screen_ == "users") return {"Mã", "Tài khoản", "Họ tên", "Vai trò", "Hoạt động"};
        return {};
    }

    vector<EventRequest> visibleRequests() const
    {
        const User* u = service_.currentUser();
        if (u && u->role() == Role::Customer) return service_.requestsForCustomer(u->customerId());
        return vector<EventRequest>(service_.db().requests.begin(), service_.db().requests.end());
    }

    Rows getRows()
    {
        Rows rows;
        const Database& db = service_.db();
        if (screen_ == "customers") {
            for (const Customer& customer : service_.searchCustomers(search_->text().toStdString())) {
                appendRow(rows, {customer.id(), customer.name(), customer.phone(), customer.email(),
                                 customer.address()});
            }
        } else if (screen_ == "requests") {
            for (const EventRequest& request : visibleRequests()) {
                const Customer* customer = service_.findCustomer(request.customerId());
                const Hall* hall = request.hallId().empty() ? nullptr : service_.findHall(request.hallId());
                const string cost = formatMoney(service_.estimateRequest(request));
                const string status = isCustomer() ? customerViewOfRequest(request.status())
                                                   : toString(request.status());
                if (isCustomer()) {
                    appendRow(rows, {request.id(), toString(request.type()), request.title(),
                                     request.start().toString(), to_string(request.guestCount()), cost,
                                     status});
                } else {
                    appendRow(rows, {request.id(), customer ? customer->name() : request.customerId(),
                                     toString(request.type()), request.title(),
                                     hall ? hall->name() : "Chưa xác định", request.start().toString(),
                                     to_string(request.guestCount()), cost, status});
                }
            }
        } else if (screen_ == "mybookings") {
            const User* u = service_.currentUser();
            if (u) {
                for (const Booking& booking : service_.bookingsForCustomer(u->customerId())) {
                    const Hall* hall = service_.findHall(booking.hallId());
                    appendRow(rows, {booking.id(), hall ? hall->name() : booking.hallId(),
                                     booking.start().toString(), booking.end().toString(),
                                     formatMoney(booking.finalTotal()),
                                     formatMoney(service_.paidAmount(booking.id())),
                                     toString(booking.status())});
                }
            }
        } else if (screen_ == "halls") {
            for (const Hall& hall : db.halls) {
                appendRow(rows, {hall.id(), hall.name(), to_string(hall.capacity()),
                                 formatMoney(hall.rentalPrice()), toString(hall.status())});
            }
        } else if (screen_ == "services") {
            rows = serviceRows(serviceTab_);
        } else if (screen_ == "lookup") {
            const DateTime now = DateTime::now();
            if (!lookupActive_) {
                // Mac dinh: hien tat ca sanh kem trang thai o thoi diem hien tai.
                for (const Hall& hall : db.halls) {
                    string present = "—";
                    if (hall.status() == HallStatus::KhaDung) {
                        bool busy = false;
                        for (const Booking& b : db.bookings) {
                            if (b.hallId() != hall.id() || !b.occupiesHall()) continue;
                            if (!(now < b.start()) && !(b.end() < now)) {
                                busy = true;
                                break;
                            }
                        }
                        present = busy ? "Đang có tiệc" : "Trống";
                    }
                    appendRow(rows, {hall.id(), hall.name(), to_string(hall.capacity()),
                                     formatMoney(hall.rentalPrice()), toString(hall.status()), present});
                }
            } else {
                for (const Hall& hall :
                     service_.filterHalls(lStart_, lEnd_, lGuests_, lMaxPrice_, "")) {
                    appendRow(rows, {hall.id(), hall.name(), to_string(hall.capacity()),
                                     formatMoney(hall.rentalPrice()), toString(hall.status()),
                                     "Trống (khung đã chọn)"});
                }
            }
        } else if (screen_ == "bookings") {
            for (const Booking& booking : db.bookings) {
                string discount = "-";
                if (booking.discountPercent() > 0 || booking.discountAmount() > 0) {
                    discount = to_string(booking.discountPercent()) + "% (-" +
                               formatMoney(booking.discountAmount()) + ")" +
                               (booking.discountNeedsApproval() && !booking.discountApproved() ? " [chờ duyệt]"
                                                                                               : "");
                }
                appendRow(rows, {booking.id(), booking.requestId(), booking.hallId(),
                                 booking.start().toString(), formatMoney(booking.finalTotal()), discount,
                                 toString(booking.status()), booking.salesUsername()});
            }
        } else if (screen_ == "payments") {
            for (const Payment& payment : db.payments) {
                appendRow(rows, {payment.id(), payment.bookingId(), toString(payment.type()),
                                 payment.amount() > 0 ? formatMoney(payment.amount()) : "(chưa nhập)",
                                 payment.paidAt().toString(), toString(payment.method()),
                                 toString(payment.status()), payment.note()});
            }
        } else if (screen_ == "events") {
            for (const Event& event : service_.allEvents()) {
                const Customer* customer = service_.findCustomer(event.customerId());
                appendRow(rows, {event.id(), event.bookingId(), event.hallId(),
                                 customer ? customer->name() : event.customerId(), event.start().toString(),
                                 event.end().toString(), toString(event.status())});
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
        if (screen_.isEmpty()) return;
        auto* layout = qobject_cast<QVBoxLayout*>(workspacePage_->layout());
        if (!layout) return;
        if (screen_ == "services") {
            refreshServiceTabs(layout);
            return;
        }
        if (!table_) return;
        const Rows rows = getRows();
        QTableWidget* replacement = makeTable(tableHeaders(), rows, workspacePage_);
        layout->replaceWidget(content_, replacement);
        content_->deleteLater();
        table_ = replacement;
        content_ = table_;
        connectTableSignals();
    }

    void connectTableSignals()
    {
        if (!table_) return;
        // Bam truc tiep vao o trang thai de doi (dropdown tai cho).
        connect(table_, &QTableWidget::clicked, this, [this](const QModelIndex& idx) {
            statusCellClicked(idx.row(), idx.column());
        });
        // Double-click vao dong thanh toan / dat lich de xem phieu thu.
        connect(table_, &QTableWidget::doubleClicked, this, [this](const QModelIndex& idx) {
            if (screen_ == "payments" || screen_ == "mybookings") viewReceipts();
            (void)idx;
        });
    }

    // 7 tab danh muc theo dac ta 2.9 (tab ngang).
    static QString serviceTabName(int index)
    {
        switch (index) {
            case 0: return "Loại sự kiện";
            case 1: return "Ẩm thực";
            case 2: return "Trang trí";
            case 3: return "Chụp ảnh & quay phim";
            case 4: return "Âm thanh & ánh sáng";
            case 5: return "Sảnh & địa điểm";
            default: return "Nhân sự & dịch vụ bổ sung";
        }
    }

    static string serviceTabCategory(int index)
    {
        switch (index) {
            case 1: return "Ẩm thực";
            case 2: return "Trang trí";
            case 3: return "Chụp ảnh & quay phim";
            case 4: return "Âm thanh & ánh sáng";
            case 6: return "Nhân sự & dịch vụ bổ sung";
            default: return "";
        }
    }

    QStringList serviceHeaders(int index) const
    {
        if (index == 0) return {"Mã", "Loại sự kiện", "Quy mô tham khảo", "Ngân sách tổ chức"};
        if (index == 5) return {"Mã", "Tên sảnh", "Sức chứa", "Giá thuê", "Trạng thái"};
        return {"Mã", "Tên dịch vụ", "Đơn vị", "Đơn giá", "Cung cấp"};
    }

    Rows serviceRows(int index) const
    {
        Rows rows;
        const Database& db = service_.db();
        if (index == 0) {
            for (int i = 0; i <= static_cast<int>(EventType::VanHoa); ++i) {
                const EventTypeMeta meta = eventTypeMeta(static_cast<EventType>(i));
                appendRow(rows, {meta.code, meta.name, meta.scale, meta.budget});
            }
        } else if (index == 5) {
            for (const Hall& hall : db.halls) {
                appendRow(rows, {hall.id(), hall.name(), to_string(hall.capacity()),
                                 formatMoney(hall.rentalPrice()), toString(hall.status())});
            }
        } else {
            const string category = serviceTabCategory(index);
            for (const ServiceItem& item : db.services) {
                if (item.category() != category) continue;
                if (!item.active() && !isManager()) continue;
                appendRow(rows, {item.code(), item.name(), item.unit(), formatMoney(item.unitPrice()),
                                 item.active() ? "Có" : "Không"});
            }
        }
        return rows;
    }

    void refreshServiceTabs(QVBoxLayout* layout)
    {
        auto* tabs = new QTabWidget(workspacePage_);
        for (int i = 0; i < 7; ++i) {
            QTableWidget* page = makeTable(serviceHeaders(i), serviceRows(i), tabs);
            tabs->addTab(page, serviceTabName(i));
        }
        if (serviceTab_ < 0 || serviceTab_ >= tabs->count()) serviceTab_ = 0;
        tabs->setCurrentIndex(serviceTab_);
        table_ = qobject_cast<QTableWidget*>(tabs->currentWidget());
        connect(tabs, &QTabWidget::currentChanged, this, [this](int index) {
            serviceTab_ = index;
            if (serviceTabs_) table_ = qobject_cast<QTableWidget*>(serviceTabs_->widget(index));
            addActions();
        });
        layout->replaceWidget(content_, tabs);
        content_->deleteLater();
        content_ = tabs;
        serviceTabs_ = tabs;
        connectTableSignals();
    }

    // Bam vao o trang thai trong bang de cap nhat truc tiep.
    void statusCellClicked(int row, int column)
    {
        if (!table_) return;
        QTableWidgetItem* cell0 = table_->item(row, 0);
        if (!cell0) return;
        const QString id = cell0->text();
        if (screen_ == "requests") {
            if (isCustomer()) {
                if (column == 6) customerRequestStatusCell(id);
            } else if (column == 8) {
                salesRequestStatusCell(id);
            }
        } else if (screen_ == "bookings") {
            if (column == 6)
                bookingStatusCell(id);
            else if (column == 5)
                discountCell(id);
        } else if (screen_ == "halls") {
            if (column == 4 && isManager()) hallStatusCell(id);
        } else if (screen_ == "services") {
            if (column == 4 && isManager() && !serviceTabCategory(serviceTab_).empty())
                serviceActiveCell(id);
        } else if (screen_ == "users") {
            if (!isManager()) return;
            if (column == 3)
                userRoleCell(id);
            else if (column == 4)
                userActiveCell(id);
        }
    }

    void salesRequestStatusCell(const QString& id)
    {
        const EventRequest* req = service_.findRequest(id.toStdString());
        if (!req) return;
        const auto picked = promptCombo(this, "Cập nhật trạng thái yêu cầu", "Trạng thái",
                                        {"Chờ", "Đang tư vấn", "Hoàn thành", "Bị hủy"},
                                        qstr(toString(req->status())));
        if (!picked) return;
        notify(service_.updateRequestStatus(id.toStdString(),
                                            requestStatusFromString(picked->toStdString())));
    }

    void customerRequestStatusCell(const QString& id)
    {
        const EventRequest* req = service_.findRequest(id.toStdString());
        if (!req) return;
        const QString current = qstr(customerViewOfRequest(req->status()));
        if (current == "Bị hủy" || current == "Hoàn thành") {
            showInfo("Trạng thái", "Yêu cầu đã kết thúc, không đổi được nữa.");
            return;
        }
        const auto picked =
            promptCombo(this, "Hủy yêu cầu", "Chọn Bị hủy để tự hủy yêu cầu này",
                        {current, "Bị hủy"}, current);
        if (!picked || *picked != "Bị hủy") return;
        notify(service_.cancelOwnRequest(id.toStdString()));
    }

    void bookingStatusCell(const QString& id)
    {
        const Booking* b = service_.findBooking(id.toStdString());
        if (!b) return;
        const QString current = qstr(toString(b->status()));
        if (current == "Bị hủy" || current == "Hoàn tất") {
            showInfo("Trạng thái", "Booking đã kết thúc, không đổi được nữa.");
            return;
        }
        const auto picked = promptCombo(this, "Cập nhật trạng thái booking", "Trạng thái",
                                        {current, "Hoàn tất", "Bị hủy"}, current);
        if (!picked || *picked == current) return;
        if (*picked == "Bị hủy") {
            const auto reason =
                promptFields(this, "Hủy booking", {"Lý do"}, {"Hủy từ màn hình booking"});
            if (!reason) return;
            notify(service_.cancelBooking(id.toStdString(), (*reason)[0].toStdString()));
        } else {
            notify(service_.setBookingStatus(id.toStdString(), BookingStatus::HoanTat));
        }
    }

    void discountCell(const QString& id)
    {
        const Booking* b = service_.findBooking(id.toStdString());
        if (!b) return;
        if (b->discountNeedsApproval() && !b->discountApproved()) {
            if (!isManager()) {
                showInfo("Chiết khấu", "Mức giảm vượt 10% đang chờ Quản lý duyệt.");
                return;
            }
            const auto picked = promptCombo(this, "Duyệt chiết khấu",
                                            "Duyệt mức giảm cho booking " + id, {"Duyệt", "Để sau"},
                                            "Để sau");
            if (picked && *picked == "Duyệt") notify(service_.approveDiscount(id.toStdString()));
            return;
        }
        showInfo("Chiết khấu",
                 qstr("Giảm " + to_string(b->discountPercent()) + "% (-" +
                      formatMoney(b->discountAmount()) + "). Bấm Cập nhật giảm giá để sửa."));
    }

    void hallStatusCell(const QString& id)
    {
        const Hall* hall = service_.findHall(id.toStdString());
        if (!hall) return;
        const auto picked =
            promptCombo(this, "Trạng thái sảnh", "Trạng thái", {"Khả dụng", "Bảo trì", "Không khả dụng"},
                        qstr(toString(hall->status())));
        if (!picked) return;
        notify(service_.updateHall(hall->id(), hall->name(), hall->capacity(), hall->rentalPrice(),
                                   hallStatusFromString(picked->toStdString())));
    }

    void serviceActiveCell(const QString& code)
    {
        const ServiceItem* found = nullptr;
        for (const ServiceItem& s : service_.db().services) {
            if (s.code() == code.toStdString() || s.id() == code.toStdString()) {
                found = &s;
                break;
            }
        }
        if (!found) return;
        const ServiceItem item = *found;
        const auto picked =
            promptCombo(this, "Cung cấp dịch vụ", "Đang cung cấp", {"Có", "Không"},
                        item.active() ? "Có" : "Không");
        if (!picked) return;
        notify(service_.updateService(item.id(), item.code(), item.name(), item.unit(),
                                      item.unitPrice(), item.category(), *picked == "Có"));
    }

    void userRoleCell(const QString& id)
    {
        const User* target = nullptr;
        for (const User& x : service_.db().users) {
            if (x.id() == id.toStdString()) {
                target = &x;
                break;
            }
        }
        if (!target) return;
        const auto picked =
            promptCombo(this, "Vai trò người dùng", "Vai trò",
                        {"Khách hàng", "Nhân viên kinh doanh", "Quản lý", "Vận hành"},
                        qstr(toString(target->role())));
        if (!picked) return;
        notify(service_.updateUserRole(id.toStdString(), roleFromString(picked->toStdString())));
    }

    void userActiveCell(const QString& id)
    {
        const User* target = nullptr;
        for (const User& x : service_.db().users) {
            if (x.id() == id.toStdString()) {
                target = &x;
                break;
            }
        }
        if (!target) return;
        const auto picked =
            promptCombo(this, "Khóa / mở tài khoản", "Hoạt động", {"Có", "Không"},
                        target->active() ? "Có" : "Không");
        if (!picked) return;
        notify(service_.setUserActive(id.toStdString(), *picked == "Có"));
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
            addActionButton(layout, "Tạo mới", [this]() { addCustomer(); });
            addActionButton(layout, "Cập nhật", [this]() { editCustomer(); });
            addActionButton(layout, "Xóa", [this]() { deleteSelected(); });
        } else if (screen_ == "requests") {
            addActionButton(layout, "Tạo mới", [this]() { addRequest(); });
            addActionButton(layout, "Cập nhật", [this]() { editRequest(); });
            if (!isCustomer()) {
                addActionButton(layout, "Tạo Booking", [this]() { createBooking(); });
                addActionButton(layout, "Xóa", [this]() { deleteSelected(); });
            }
        } else if (screen_ == "mybookings") {
            // Chi xem + double-click de xem phieu thu.
        } else if (screen_ == "halls") {
            if (isManager()) {
                addActionButton(layout, "Tạo mới", [this]() { addHall(); });
                addActionButton(layout, "Cập nhật", [this]() { editHall(); });
                addActionButton(layout, "Xóa", [this]() { deleteSelected(); });
            }
        } else if (screen_ == "services") {
            if (isManager() && !serviceTabCategory(serviceTab_).empty()) {
                addActionButton(layout, "Tạo mới", [this]() { addService(); });
                addActionButton(layout, "Cập nhật", [this]() { editService(); });
                addActionButton(layout, "Xóa", [this]() { deleteSelected(); });
            }
            if (isManager()) {
                addActionButton(layout, "Nạp danh mục mẫu", [this]() { importCatalog(); });
            }
        } else if (screen_ == "lookup") {
            // Nut Tra cuu / Hien tat ca nam ngay tren khung loc.
        } else if (screen_ == "bookings") {
            addActionButton(layout, "Tạo mới", [this]() { createBooking(); });
            addActionButton(layout, "Cập nhật giảm giá", [this]() { editBookingDiscount(); });
        } else if (screen_ == "payments") {
            addActionButton(layout, "Tạo mới", [this]() { recordPayment(); });
            addActionButton(layout, "Cập nhật", [this]() { editPayment(); });
            addActionButton(layout, "Xóa khoản chưa thu", [this]() { deleteSelected(); });
        } else if (screen_ == "users") {
            addActionButton(layout, "Tạo mới", [this]() { createUser(); });
            addActionButton(layout, "Cập nhật (đổi mật khẩu)", [this]() { changePassword(); });
            addActionButton(layout, "Xóa", [this]() { deleteSelected(); });
        }
        layout->addStretch();
    }

    // Xoa dong dang chon (tuy man hinh).
    void deleteSelected()
    {
        const QString id = selectedValue();
        if (id.isEmpty()) {
            showError("Hãy chọn một dòng trong bảng.");
            return;
        }
        OpResult result = OpResult::fail("Màn hình này không xóa được.");
        if (screen_ == "customers")
            result = service_.deleteCustomer(id.toStdString());
        else if (screen_ == "requests")
            result = service_.deleteRequest(id.toStdString());
        else if (screen_ == "halls")
            result = service_.deleteHall(id.toStdString());
        else if (screen_ == "services")
            result = service_.deleteService(id.toStdString());
        else if (screen_ == "payments")
            result = service_.deletePayment(id.toStdString());
        else if (screen_ == "users")
            result = service_.deleteUser(id.toStdString());
        notify(result);
    }

    // ---- Form 1: tao yeu cau (dropdown loai/sanh, date-picker, dich vu tich chon) ----

    struct RequestFormResult {
        string customerId;
        EventType type = EventType::TiecCuoi;
        string title, customerName, customerPhone, hallId, notes;
        DateTime start, end;
        int guestCount = 100;
        vector<string> serviceIds;
    };

    optional<RequestFormResult> promptRequestForm(const QString& title, const RequestFormResult& preset,
                                                 bool pickCustomer)
    {
        QDialog dialog(this);
        dialog.setWindowTitle(title);
        dialog.setMinimumWidth(560);
        auto* layout = new QVBoxLayout(&dialog);
        auto* form = new QFormLayout;
        form->setSpacing(8);

        QComboBox* customerCombo = nullptr;
        if (pickCustomer) {
            customerCombo = new QComboBox(&dialog);
            for (const Customer& c : service_.db().customers) {
                customerCombo->addItem(qstr(c.id() + " - " + c.name()), qstr(c.id()));
            }
            const int idx = customerCombo->findData(qstr(preset.customerId));
            if (idx >= 0) customerCombo->setCurrentIndex(idx);
            form->addRow("Khách hàng", customerCombo);
        }
        auto* typeCombo = new QComboBox(&dialog);
        for (int i = 0; i <= static_cast<int>(EventType::VanHoa); ++i) {
            const EventType t = static_cast<EventType>(i);
            typeCombo->addItem(qstr(eventTypeMeta(t).code + string(" - ") + toString(t)));
        }
        typeCombo->setCurrentIndex(static_cast<int>(preset.type));
        form->addRow("Loại sự kiện", typeCombo);

        auto* titleEdit = new QLineEdit(qstr(preset.title), &dialog);
        titleEdit->setPlaceholderText("Tên sự kiện");
        form->addRow("Tên sự kiện", titleEdit);
        auto* nameEdit = new QLineEdit(qstr(preset.customerName), &dialog);
        nameEdit->setPlaceholderText("Họ tên khách hàng");
        form->addRow("Họ tên khách hàng", nameEdit);
        auto* phoneEdit = new QLineEdit(qstr(preset.customerPhone), &dialog);
        phoneEdit->setPlaceholderText("Số điện thoại");
        form->addRow("Số điện thoại", phoneEdit);

        auto* hallCombo = new QComboBox(&dialog);
        hallCombo->addItem("Chưa xác định - Cần tư vấn", "");
        for (const Hall& h : service_.db().halls) {
            if (h.status() != HallStatus::KhaDung) continue;
            hallCombo->addItem(qstr(h.name() + " (" + to_string(h.capacity()) + " khách, " +
                                    formatMoney(h.rentalPrice()) + ")"),
                               qstr(h.id()));
        }
        const int hallIdx = hallCombo->findData(qstr(preset.hallId));
        if (hallIdx >= 0) hallCombo->setCurrentIndex(hallIdx);
        form->addRow("Sảnh mong muốn", hallCombo);

        auto* startEdit = new QDateTimeEdit(&dialog);
        startEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
        startEdit->setCalendarPopup(true);
        startEdit->setDateTime(QDateTime(QDate(preset.start.year, preset.start.month, preset.start.day),
                                         QTime(preset.start.hour, preset.start.minute)));
        form->addRow("Bắt đầu", startEdit);
        auto* endEdit = new QDateTimeEdit(&dialog);
        endEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
        endEdit->setCalendarPopup(true);
        endEdit->setDateTime(QDateTime(QDate(preset.end.year, preset.end.month, preset.end.day),
                                       QTime(preset.end.hour, preset.end.minute)));
        form->addRow("Kết thúc", endEdit);

        auto* guestsSpin = new QSpinBox(&dialog);
        guestsSpin->setRange(10, service_.maxHallCapacity());
        guestsSpin->setValue(preset.guestCount);
        form->addRow("Số lượng khách", guestsSpin);

        // Dich vu tich chon theo tung tab, giong thiet ke danh muc (dac ta 2.9).
        auto* serviceTabs = new QTabWidget(&dialog);
        serviceTabs->setMinimumHeight(200);
        for (const QString& category :
             QStringList({"Ẩm thực", "Trang trí", "Chụp ảnh & quay phim", "Âm thanh & ánh sáng",
                          "Nhân sự & dịch vụ bổ sung"})) {
            auto* list = new QListWidget(serviceTabs);
            list->setSelectionMode(QAbstractItemView::NoSelection);
            for (const ServiceItem& s : service_.db().services) {
                if (!s.active()) continue;
                if (qstr(s.category()) != category) continue;
                auto* item = new QListWidgetItem(
                    qstr(s.code() + " - " + s.name() + " (" + formatMoney(s.unitPrice()) + " " +
                         s.unit() + ")"),
                    list);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                const bool checked =
                    std::find(preset.serviceIds.begin(), preset.serviceIds.end(), s.id()) !=
                    preset.serviceIds.end();
                item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
                item->setData(Qt::UserRole, qstr(s.id()));
            }
            serviceTabs->addTab(list, category);
        }
        layout->addLayout(form);
        layout->addWidget(new QLabel("Dịch vụ (tích chọn theo từng tab):", &dialog));
        layout->addWidget(serviceTabs, 1);

        auto* notesEdit = new QTextEdit(qstr(preset.notes), &dialog);
        notesEdit->setPlaceholderText("Ghi chú thêm");
        notesEdit->setMaximumHeight(70);
        layout->addWidget(new QLabel("Ghi chú:", &dialog));
        layout->addWidget(notesEdit);

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
        QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        layout->addWidget(buttons);

        if (dialog.exec() != QDialog::Accepted) return nullopt;
        RequestFormResult out;
        if (customerCombo) out.customerId = customerCombo->currentData().toString().toStdString();
        out.type = static_cast<EventType>(typeCombo->currentIndex());
        out.title = titleEdit->text().trimmed().toStdString();
        out.customerName = nameEdit->text().trimmed().toStdString();
        out.customerPhone = phoneEdit->text().trimmed().toStdString();
        out.hallId = hallCombo->currentData().toString().toStdString();
        const QDateTime qs = startEdit->dateTime(), qe = endEdit->dateTime();
        out.start = {qs.date().year(), qs.date().month(), qs.date().day(), qs.time().hour(),
                     qs.time().minute()};
        out.end = {qe.date().year(), qe.date().month(), qe.date().day(), qe.time().hour(),
                   qe.time().minute()};
        out.guestCount = guestsSpin->value();
        for (int t = 0; t < serviceTabs->count(); ++t) {
            auto* list = qobject_cast<QListWidget*>(serviceTabs->widget(t));
            if (!list) continue;
            for (int i = 0; i < list->count(); ++i) {
                QListWidgetItem* item = list->item(i);
                if (item->checkState() == Qt::Checked) {
                    out.serviceIds.push_back(item->data(Qt::UserRole).toString().toStdString());
                }
            }
        }
        out.notes = notesEdit->toPlainText().toStdString();
        return out;
    }

    void addRequest()
    {
        const User* u = service_.currentUser();
        RequestFormResult preset;
        preset.start = DateTime::now().plusHours(24);
        preset.end = preset.start.plusHours(4);
        preset.guestCount = 100;
        if (u && u->role() == Role::Customer) {
            preset.customerId = u->customerId();
            if (const Customer* c = service_.findCustomer(preset.customerId)) {
                preset.customerName = c->name();
                preset.customerPhone = c->phone();
            }
        }
        const auto form = promptRequestForm("Tạo yêu cầu sự kiện", preset, !isCustomer());
        if (!form) return;
        vector<ServiceLineInput> lines;
        for (const string& sid : form->serviceIds) lines.push_back({sid, 0});
        notify(service_.addRequest(form->customerId, form->type, form->title, form->customerName,
                                   form->customerPhone, form->hallId, form->start, form->end,
                                   form->guestCount, form->notes, lines));
    }

    void editRequest()
    {
        const QString id = selectedValue();
        const EventRequest* req = service_.findRequest(id.toStdString());
        if (!req) {
            showError("Hãy chọn một yêu cầu trong bảng.");
            return;
        }
        const User* u = service_.currentUser();
        const bool customer = u && u->role() == Role::Customer;
        if (customer) {
            if (req->customerId() != u->customerId()) {
                showError("Bạn chỉ sửa yêu cầu của mình.");
                return;
            }
            if (req->status() != RequestStatus::Cho && req->status() != RequestStatus::DangTuVan) {
                showError("Chỉ sửa được yêu cầu đang xét duyệt.");
                return;
            }
        }
        RequestFormResult preset;
        preset.customerId = req->customerId();
        preset.type = req->type();
        preset.title = req->title();
        preset.customerName = req->customerName();
        preset.customerPhone = req->customerPhone();
        preset.hallId = req->hallId();
        preset.start = req->start();
        preset.end = req->end();
        preset.guestCount = req->guestCount();
        preset.notes = req->notes();
        for (const auto& l : req->lines()) preset.serviceIds.push_back(l.serviceId());
        const auto form = promptRequestForm("Sửa yêu cầu " + id, preset, !customer);
        if (!form) return;
        vector<ServiceLineInput> lines;
        for (const string& sid : form->serviceIds) lines.push_back({sid, 0});
        notify(service_.updateRequest(id.toStdString(), form->type, form->title, form->customerName,
                                      form->customerPhone, form->hallId, form->start, form->end,
                                      form->guestCount, form->notes, lines));
    }

    // ---- Form 2: booking ----

    void createBooking()
    {
        QString reqId = selectedValue();
        const EventRequest* presetReq = service_.findRequest(reqId.toStdString());
        if (screen_ != "requests" || !presetReq) {
            const auto picked = promptCombo(this, "Tạo Booking", "Mã yêu cầu (tìm theo mã)",
                                            requestOptions(), reqId);
            if (!picked) return;
            reqId = picked->split(" - ").first();
            presetReq = service_.findRequest(reqId.toStdString());
        }
        if (!presetReq) {
            showError("Không tìm thấy yêu cầu.");
            return;
        }
        const auto hallPicked = promptCombo(this, "Tạo Booking", "Sảnh chốt",
                                            hallOptions(presetReq->start(), presetReq->end(),
                                                        presetReq->guestCount()),
                                            qstr(presetReq->hallId()));
        if (!hallPicked) return;
        const string hallId = hallPicked->split(" - ").first().toStdString();
        const auto fields = promptFields(
            this, "Tạo Booking " + reqId,
            {"Bắt đầu (YYYY-MM-DD HH:MM)", "Kết thúc (YYYY-MM-DD HH:MM)", "Số lượng khách",
             "Chiết khấu % (0 nếu không)", "Giảm giá VNĐ (0 nếu không)"},
            {qstr(presetReq->start().toString()), qstr(presetReq->end().toString()),
             QString::number(presetReq->guestCount()), "0", "0"});
        if (!fields) return;
        bool okStart = false, okEnd = false;
        const DateTime start = parseDateTimeText((*fields)[0], okStart);
        const DateTime end = parseDateTimeText((*fields)[1], okEnd);
        if (!okStart || !okEnd) {
            showError("Thời gian không hợp lệ. Dùng định dạng YYYY-MM-DD HH:MM.");
            return;
        }
        const auto guests = parseInt((*fields)[2].toStdString());
        const auto pct = parseInt((*fields)[3].toStdString());
        const auto amt = parseMoney((*fields)[4].toStdString());
        if (!guests || !pct || !amt) {
            showError("Số lượng khách / chiết khấu không hợp lệ.");
            return;
        }
        notify(service_.createBooking(reqId.toStdString(), hallId, start, end, *guests, *pct, *amt));
    }

    QStringList requestOptions() const
    {
        QStringList out;
        for (const EventRequest& r : service_.db().requests) {
            if (r.status() == RequestStatus::BiHuy || r.status() == RequestStatus::DaChot) continue;
            out.push_back(qstr(r.id() + " - " + r.title()));
        }
        return out;
    }

    QStringList hallOptions(DateTime start, DateTime end, int guests) const
    {
        QStringList out;
        for (const Hall& h : service_.findAvailableHalls(start, end, guests, "")) {
            out.push_back(qstr(h.id() + " - " + h.name()));
        }
        if (out.isEmpty()) {
            for (const Hall& h : service_.db().halls) {
                if (h.status() == HallStatus::KhaDung) out.push_back(qstr(h.id() + " - " + h.name()));
            }
        }
        return out;
    }

    void editBookingDiscount()
    {
        const QString id = selectedValue();
        const Booking* b = service_.findBooking(id.toStdString());
        if (!b) {
            showError("Hãy chọn booking trong bảng.");
            return;
        }
        const auto values = promptFields(this, "Cập nhật giảm giá " + id,
                                         {"Chiết khấu %", "Giảm giá VNĐ"},
                                         {QString::number(b->discountPercent()),
                                          QString::number(b->discountAmount())});
        if (!values) return;
        const auto pct = parseInt((*values)[0].toStdString());
        const auto amt = parseMoney((*values)[1].toStdString());
        if (!pct || !amt) {
            showError("Chiết khấu không hợp lệ.");
            return;
        }
        notify(service_.updateBookingDiscount(id.toStdString(), *pct, *amt));
    }

    // ---- Form 3: thanh toan ----

    void editPayment()
    {
        const QString id = selectedValue();
        const Payment* payment = service_.findPayment(id.toStdString());
        if (!payment) {
            showError("Hãy chọn khoản thanh toán trong bảng.");
            return;
        }
        const Payment item = *payment;
        const auto typePicked =
            promptCombo(this, "Cập nhật thanh toán", "Loại thanh toán",
                        {"Đặt cọc lần 1", "Đặt cọc lần 2", "Tất toán"},
                        qstr(toString(item.type())));
        if (!typePicked) return;
        const auto fields = promptFields(this, "Cập nhật thanh toán " + id,
                                         {"Số tiền (VND)", "Ghi chú (bắt buộc)"},
                                         {QString::number(item.amount()), qstr(item.note())});
        if (!fields) return;
        const auto amount = parseMoney((*fields)[0].toStdString());
        if (!amount) {
            showError("Số tiền không hợp lệ.");
            return;
        }
        const auto methodPicked = promptCombo(this, "Cập nhật thanh toán", "Phương thức",
                                              {"Tiền mặt", "Chuyển khoản", "Thẻ tín dụng"},
                                              qstr(toString(item.method())));
        if (!methodPicked) return;
        const auto paidAt =
            promptDateTime(this, "Cập nhật thanh toán", "Ngày thanh toán", item.paidAt());
        if (!paidAt) return;
        notify(service_.updatePayment(
            id.toStdString(), paymentTypeFromString(typePicked->toStdString()), *amount,
            paymentMethodFromString(methodPicked->toStdString()), *paidAt,
            (*fields)[1].toStdString()));
    }

    void recordPayment()
    {
        QString bookingId;
        if (screen_ == "bookings")
            bookingId = selectedValue(0);
        else if (screen_ == "payments")
            bookingId = selectedValue(1);
        const auto bookingPicked =
            promptCombo(this, "Ghi nhận thanh toán", "Mã Booking", bookingOptions(), bookingId);
        if (!bookingPicked) return;
        bookingId = bookingPicked->split(" - ").first();
        const auto typePicked =
            promptCombo(this, "Ghi nhận thanh toán", "Loại thanh toán",
                        {"Đặt cọc lần 1", "Đặt cọc lần 2", "Tất toán"}, "Đặt cọc lần 1");
        if (!typePicked) return;
        const auto fields = promptFields(this, "Ghi nhận thanh toán " + bookingId,
                                         {"Số tiền (VND)", "Ghi chú (bắt buộc)"}, {"10000000", ""});
        if (!fields) return;
        const auto amount = parseMoney((*fields)[0].toStdString());
        if (!amount) {
            showError("Số tiền không hợp lệ.");
            return;
        }
        const auto methodPicked = promptCombo(this, "Ghi nhận thanh toán", "Phương thức",
                                              {"Tiền mặt", "Chuyển khoản", "Thẻ tín dụng"}, "Chuyển khoản");
        if (!methodPicked) return;
        const auto paidAt = promptDateTime(this, "Ghi nhận thanh toán", "Ngày thanh toán (mặc định hôm nay)",
                                           DateTime::now());
        if (!paidAt) return;
        notify(service_.recordPayment(bookingId.toStdString(),
                                      paymentTypeFromString(typePicked->toStdString()), *amount,
                                      paymentMethodFromString(methodPicked->toStdString()), *paidAt,
                                      (*fields)[1].toStdString()));
    }

    QStringList bookingOptions() const
    {
        QStringList out;
        for (const Booking& b : service_.db().bookings) {
            if (b.status() == BookingStatus::DaHuy) continue;
            out.push_back(qstr(b.id() + " - " + toString(b.status())));
        }
        return out;
    }

    void viewReceipts()
    {
        QString bookingId = selectedValue();
        if (screen_ == "payments") bookingId = selectedValue(1);
        if (screen_ == "mybookings") bookingId = selectedValue();
        const vector<Payment> pays = service_.paymentsOf(bookingId.toStdString());
        if (pays.empty()) {
            showError("Booking này chưa có khoản thanh toán nào.");
            return;
        }
        QStringList options;
        for (const Payment& p : pays) {
            options.push_back(qstr(p.id() + " - " + toString(p.type()) + " - " + toString(p.status())));
        }
        const auto picked = promptCombo(this, "Phiếu thu", "Chọn khoản thu", options, options.front());
        if (!picked) return;
        const string pid = picked->split(" - ").first().toStdString();
        showInfo("Phiếu thu", qstr(service_.paymentReceipt(pid)));
    }

    // ---- Khach hang / sanh / dich vu / nguoi dung ----

    void addCustomer()
    {
        const auto values =
            promptFields(this, "Thêm khách hàng", {"Họ tên / Tên công ty", "Số điện thoại", "Email",
                                                   "Địa chỉ", "Ghi chú"});
        if (!values) return;
        notify(service_.addCustomer((*values)[0].toStdString(), (*values)[1].toStdString(),
                                    (*values)[2].toStdString(), (*values)[3].toStdString(),
                                    (*values)[4].toStdString()));
    }

    void editCustomer()
    {
        const QString id = selectedValue();
        const Customer* customer = service_.findCustomer(id.toStdString());
        if (!customer) {
            showError("Hãy chọn một khách hàng trong bảng.");
            return;
        }
        const auto values = promptFields(this, "Sửa khách hàng",
                                         {"Họ tên / Tên công ty", "Số điện thoại", "Email", "Địa chỉ",
                                          "Ghi chú"},
                                         {qstr(customer->name()), qstr(customer->phone()),
                                          qstr(customer->email()), qstr(customer->address()),
                                          qstr(customer->notes())});
        if (!values) return;
        notify(service_.updateCustomer(id.toStdString(), (*values)[0].toStdString(),
                                       (*values)[1].toStdString(), (*values)[2].toStdString(),
                                       (*values)[3].toStdString(), (*values)[4].toStdString()));
    }

    void addHall()
    {
        const auto values = promptFields(this, "Thêm sảnh (mã tự động)",
                                         {"Tên sảnh", "Sức chứa", "Giá thuê (VNĐ/sự kiện)"},
                                         {"", "200", "12000000"});
        if (!values) return;
        const auto capacity = parseInt((*values)[1].toStdString());
        const auto price = parseMoney((*values)[2].toStdString());
        if (!capacity || !price) {
            showError("Sức chứa / giá thuê không hợp lệ.");
            return;
        }
        notify(service_.addHall((*values)[0].toStdString(), *capacity, *price));
    }

    void editHall()
    {
        const QString id = selectedValue();
        const Hall* hall = service_.findHall(id.toStdString());
        if (!hall) {
            showError("Hãy chọn sảnh trong bảng.");
            return;
        }
        const auto values = promptFields(this, "Sửa sảnh",
                                         {"Tên sảnh", "Sức chứa", "Giá thuê (VNĐ/sự kiện)"},
                                         {qstr(hall->name()), QString::number(hall->capacity()),
                                          QString::number(hall->rentalPrice())});
        if (!values) return;
        const auto capacity = parseInt((*values)[1].toStdString());
        const auto price = parseMoney((*values)[2].toStdString());
        if (!capacity || !price) {
            showError("Sức chứa / giá thuê không hợp lệ.");
            return;
        }
        const auto statusPicked =
            promptCombo(this, "Sửa sảnh", "Trạng thái (checkbox trạng thái sảnh)",
                        {"Khả dụng", "Bảo trì", "Không khả dụng"}, qstr(toString(hall->status())));
        if (!statusPicked) return;
        notify(service_.updateHall(hall->id(), (*values)[0].toStdString(), *capacity, *price,
                                   hallStatusFromString(statusPicked->toStdString())));
    }

    void importCatalog()
    {
        notify(service_.importCatalog());
    }

    void addService()
    {
        const auto values = promptFields(this, "Thêm dịch vụ",
                                         {"Mã gói (VD FOD01-P)", "Tên dịch vụ", "Đơn vị", "Đơn giá (VND)"},
                                         {"", "", "VNĐ/gói", "1000000"});
        if (!values) return;
        const auto price = parseMoney((*values)[3].toStdString());
        if (!price) {
            showError("Đơn giá không hợp lệ.");
            return;
        }
        const auto catPicked = promptCombo(this, "Thêm dịch vụ", "Tab danh mục",
                                           {"Ẩm thực", "Trang trí", "Chụp ảnh & quay phim",
                                            "Âm thanh & ánh sáng", "Nhân sự & dịch vụ bổ sung"},
                                           qstr(serviceTabCategory(serviceTab_).empty()
                                                    ? "Ẩm thực"
                                                    : serviceTabCategory(serviceTab_)));
        if (!catPicked) return;
        notify(service_.addService((*values)[0].toStdString(), (*values)[1].toStdString(),
                                   (*values)[2].toStdString(), *price,
                                   catPicked->toStdString()));
    }

    void editService()
    {
        const QString id = selectedValue();
        const ServiceItem* item = nullptr;
        for (const ServiceItem& s : service_.db().services) {
            if (s.code() == id.toStdString() || s.id() == id.toStdString()) {
                item = &s;
                break;
            }
        }
        if (!item) {
            showError("Hãy chọn dịch vụ trong bảng.");
            return;
        }
        const auto values = promptFields(this, "Sửa dịch vụ",
                                         {"Mã gói", "Tên dịch vụ", "Đơn vị", "Đơn giá (VND)"},
                                         {qstr(item->code()), qstr(item->name()), qstr(item->unit()),
                                          QString::number(item->unitPrice())});
        if (!values) return;
        const auto price = parseMoney((*values)[3].toStdString());
        if (!price) {
            showError("Đơn giá không hợp lệ.");
            return;
        }
        const auto catPicked = promptCombo(this, "Sửa dịch vụ", "Tab danh mục",
                                           {"Ẩm thực", "Trang trí", "Chụp ảnh & quay phim",
                                            "Âm thanh & ánh sáng", "Nhân sự & dịch vụ bổ sung"},
                                           qstr(item->category()));
        if (!catPicked) return;
        const auto activePicked =
            promptCombo(this, "Sửa dịch vụ", "Đang cung cấp", {"Có", "Không"},
                        item->active() ? "Có" : "Không");
        if (!activePicked) return;
        notify(service_.updateService(item->id(), (*values)[0].toStdString(),
                                      (*values)[1].toStdString(), (*values)[2].toStdString(), *price,
                                      catPicked->toStdString(), *activePicked == "Có"));
    }

    void lookupHalls()
    {
        const QDateTime qs = fStart_->dateTime(), qe = fEnd_->dateTime();
        lStart_ = {qs.date().year(), qs.date().month(), qs.date().day(), qs.time().hour(),
                   qs.time().minute()};
        lEnd_ = {qe.date().year(), qe.date().month(), qe.date().day(), qe.time().hour(),
                 qe.time().minute()};
        lGuests_ = fGuests_->value();
        const string maxText = fMaxPrice_->text().trimmed().toStdString();
        if (maxText.empty()) {
            lMaxPrice_ = 0;
        } else {
            const auto parsed = parseMoney(maxText);
            if (!parsed) {
                showError("Giá thuê tối đa không hợp lệ.");
                return;
            }
            lMaxPrice_ = *parsed;
        }
        if (!(lStart_ < lEnd_)) {
            showError("Thời gian bắt đầu phải sớm hơn kết thúc.");
            return;
        }
        lookupActive_ = true;
        refreshTable();
        size_t count = service_.filterHalls(lStart_, lEnd_, lGuests_, lMaxPrice_, "").size();
        statusBar()->showMessage(QString("Tìm thấy %1 sảnh phù hợp").arg(count), 5000);
    }

    void createUser()
    {
        const auto values = promptFields(this, "Tạo người dùng",
                                         {"Tên đăng nhập", "Mật khẩu", "Họ tên"},
                                         {"", "", ""}, {1});
        if (!values) return;
        const auto rolePicked =
            promptCombo(this, "Tạo người dùng", "Vai trò",
                        {"Khách hàng", "Nhân viên kinh doanh", "Quản lý", "Vận hành"}, "Nhân viên kinh doanh");
        if (!rolePicked) return;
        notify(service_.createUser((*values)[0].toStdString(), (*values)[1].toStdString(),
                                   (*values)[2].toStdString(),
                                   roleFromString(rolePicked->toStdString())));
    }

    void changePassword()
    {
        const QString id = selectedValue();
        if (id.isEmpty()) {
            showError("Hãy chọn người dùng trong bảng.");
            return;
        }
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
