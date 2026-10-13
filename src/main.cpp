#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "services/palace_service.hpp"
#include "storage/json_storage.hpp"
#include "ui/app_ui.hpp"

#include <filesystem>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        namespace fs = filesystem;
        fs::path root = fs::current_path();
        tdp::JsonStorage storage(root / "data", root / "backups");
        storage.load();
        tdp::PalaceService service(storage);
        return tdp::runQtUi(service, argc, argv);
    } catch (const exception& ex) {
        cerr << "Loi khoi dong: " << ex.what() << "\n";
        cerr << "File JSON loi khong bi ghi de. Kiem tra thu muc data/ va backups/.\n";
        return 1;
    }
}
