// Kiem tra luong khach vang lai tren giao dien Qt, khong mo cua so that.
#include "ui/app_ui.cpp"
#include <QTimer>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    auto dir = filesystem::temp_directory_path() / ("tdp_ui_" +
        to_string(chrono::steady_clock::now().time_since_epoch().count()));
    TextStorage storage(dir / "data", dir / "backup");
    storage.load();
    PalaceService service(storage);
    MainWindow window(service);
    window.show();
    auto* guest = window.findChild<QPushButton*>("guestButton");
    auto* navigation = window.findChild<QComboBox*>("navigation");
    if (!guest || !navigation) return 1;
    guest->click();
    app.processEvents();
    if (service.isLoggedIn() || navigation->count() != 4) return 2;
    for (int i = 0; i < navigation->count(); ++i) {
        auto screen = navigation->itemData(i).toString();
        if (screen != "requests" && screen != "halls" && screen != "services" && screen != "lookup")
            return 3;
    }
    QPushButton* create = nullptr;
    for (auto* button : window.findChildren<QPushButton*>())
        if (button->text() == "Tạo mới" && button->isVisible()) create = button;
    if (!create) return 4;
    bool formFilled = false;
    QTimer::singleShot(0, [&]() {
        auto* dialog = qobject_cast<QDialog*>(QApplication::activeModalWidget());
        if (!dialog) return;
        auto* title = dialog->findChild<QLineEdit*>("requestTitle");
        auto* name = dialog->findChild<QLineEdit*>("customerName");
        auto* phone = dialog->findChild<QLineEdit*>("customerPhone");
        auto* age = dialog->findChild<QSpinBox*>("customerAge");
        if (!title || !name || !phone || !age) { dialog->reject(); return; }
        // Khach vang lai phai nhap, form khong dien san ho so khach khac.
        if (!name->text().isEmpty() || !phone->text().isEmpty() || age->value() != 0) {
            dialog->reject(); return;
        }
        title->setText("Tiec tu giao dien"); name->setText("Khach UI");
        phone->setText("0900555777"); age->setValue(26);
        formFilled = true;
        dialog->accept();
    });
    // Neu loi xuat hien trong QMessageBox, dong de test ket thuc voi ma loi.
    QTimer dismissErrors;
    QObject::connect(&dismissErrors, &QTimer::timeout, [&]() {
        if (auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) box->reject();
    });
    dismissErrors.start(100);
    create->click();
    if (!formFilled || service.guestRequests().size() != 1) return 5;
    TextStorage next(dir / "data", dir / "backup");
    next.load();
    if (next.db().requests.size() != 1 || next.db().customers.back().age() != 26) return 6;
    cout << "Qt guest flow: OK (navigation, manual profile, submit, reload)\n";
    return 0;
}
