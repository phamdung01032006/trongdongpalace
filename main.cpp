// ==================== DIEM KHOI DAU CHUONG TRINH ====================
// main.cpp nam o thu muc goc de de tim.
// Nhiem vu:
//   1. Bat che do UTF-8 cho console Windows (de hien thi tieng Viet).
//   2. Tao Store (doc file data/*.txt, neu chua co thi tao du lieu mau).
//   3. Tao PalaceService (nghiep vu) va ConsoleUI (giao dien).
//   4. Chay vong lap giao dien.

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "ConsoleUI.h"
#include "PalaceService.h"
#include "Store.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        // Thu muc data nam canh main.cpp. Neu chay exe tu thu muc build,
        // chuong trinh tu tim ../data de du lieu van nam o goc du an.
        filesystem::path dataDir = "data";
        if (!filesystem::exists(dataDir) && filesystem::exists("../data")) {
            dataDir = "../data";
        }

        Store store(dataDir.string());
        store.load();

        PalaceService service(store);
        ConsoleUI ui(service);
        return ui.run();
    } catch (const exception& ex) {
        cerr << "\n[LỖI KHỞI ĐỘNG] " << ex.what() << '\n';
        cerr << "Kiểm tra thư mục data/ và quyền ghi file.\n";
        return 1;
    } catch (...) {
        cerr << "\n[LỖI KHỞI ĐỘNG] Lỗi không xác định.\n";
        return 1;
    }
}
