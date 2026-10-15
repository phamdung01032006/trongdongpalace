#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "services/palace_service.cpp"
#include "storage/text_storage.cpp"
#include "ui/app_ui.cpp"

#include <filesystem>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
#ifdef TDP_PROJECT_DIR
        filesystem::path root = TDP_PROJECT_DIR;
#else
        filesystem::path root = filesystem::absolute(argv[0]).parent_path();
        while (!root.empty() && !filesystem::exists(root / "CMakeLists.txt") && root != root.parent_path())
            root = root.parent_path();
        if (!filesystem::exists(root / "CMakeLists.txt"))
            throw runtime_error("Khong tim thay thu muc du an. Hay build bang CMake.");
#endif
        TextStorage storage(root / "data", root / "backups");
        storage.load();
        if (argc > 1 && string(argv[1]) == "--check-data") {
            cout << "Data: " << storage.dataDir().string() << '\n';
            cout << "Users: " << storage.db().users.size() << ", customers: "
                 << storage.db().customers.size() << ", requests: " << storage.db().requests.size()
                 << ", bookings: " << storage.db().bookings.size() << ", events: "
                 << storage.db().events.size() << '\n';
            return 0;
        }
        PalaceService service(storage);
        return runQtUi(service, argc, argv);
    } catch (const exception& ex) {
        cerr << "Loi khoi dong: " << ex.what() << "\n";
        cerr << "File TXT loi khong bi ghi de. Kiem tra thu muc data/ va backups/.\n";
        return 1;
    }
}
