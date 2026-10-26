# Hướng dẫn cài đặt và chạy dự án

Tài liệu này hướng dẫn từng bước cài môi trường, build và chạy **Trống Đồng Palace**
trên Windows, Linux và macOS.

---

## 1. Yêu cầu hệ thống

| Thành phần | Phiên bản tối thiểu | Ghi chú |
| :--- | :--- | :--- |
| Trình biên dịch C++ | hỗ trợ C++17 | g++ ≥ 9, clang++ ≥ 10, MSVC 2019+ |
| CMake | 3.16 | khuyến nghị bản mới nhất |
| Make / Ninja / MSBuild | bất kỳ | đi kèm toolchain |
| RAM | 256 MB | dự án rất nhẹ |
| Dung lượng | ~50 MB | chủ yếu là toolchain/build |

**Không cần:** database, Docker, Node.js, Python, .NET, server, kết nối mạng.

**Không dùng biến môi trường / `.env`:** cấu hình duy nhất là thư mục `data/` nằm cạnh
chương trình (xem mục 5).

---

## 2. Windows — MSYS2 UCRT64 (khuyến nghị)

### 2.1. Cài MSYS2

1. Tải MSYS2 tại <https://www.msys2.org/>.
2. Chạy installer, giữ đường dẫn mặc định `C:\msys64`.
3. Mở **MSYS2 UCRT64** từ Start Menu (không dùng MSYS2 MSYS).

### 2.2. Cài toolchain

Trong cửa sổ MSYS2 UCRT64:

```bash
pacman -Syu
```

Nếu cửa sổ yêu cầu đóng/mở lại, làm theo hướng dẫn rồi chạy tiếp:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
                   mingw-w64-ucrt-x86_64-cmake \
                   mingw-w64-ucrt-x86_64-make
```

### 2.3. Kiểm tra

```bash
g++ --version
cmake --version
mingw32-make --version
```

Kỳ vọng: `g++` là bản UCRT64, `cmake` ≥ 3.16, `mingw32-make` tồn tại.

### 2.4. Build dự án

Di chuyển đến thư mục dự án (thay đường dẫn cho đúng máy bạn):

```bash
cd /c/Users/<TenBan>/trongdongpalace
```

Cấu hình và build:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Nếu CMake không tự nhận trình biên dịch, chỉ định rõ:

```bash
cmake -S . -B build -G "MinGW Makefiles" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe \
      -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/mingw32-make.exe \
      -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
cmake --build build
```

### 2.5. Chạy

Từ MSYS2 UCRT64, ở thư mục gốc dự án:

```bash
./build/trongdongpalace.exe
```

Hoặc từ PowerShell:

```powershell
.\build\trongdongpalace.exe
```

Có thể chạy trực tiếp trong thư mục `build`:

```powershell
cd build
.\trongdongpalace.exe
```

Chương trình tự tìm `data/`:
- Nếu chạy ở thư mục gốc: dùng `./data`.
- Nếu chạy trong `build/`: tự dùng `../data`.

---

## 3. Windows — Visual Studio / MSVC

### 3.1. Cài đặt

Cài **Visual Studio 2019/2022** với workload:

- Desktop development with C++
- MSVC v143 (hoặc v142)
- Windows 10/11 SDK
- CMake tools for Windows (tùy chọn)

### 3.2. Build

Mở **Developer PowerShell for VS**:

```powershell
cd C:\Users\<TenBan>\trongdongpalace
cmake -S . -B build-msvc
cmake --build build-msvc --config Release
.\build-msvc\Release\trongdongpalace.exe
```

`CMakeLists.txt` tự thêm `/utf-8` để MSVC đọc đúng tiếng Việt trong source.

---

## 4. Linux / macOS

### 4.1. Cài toolchain

**Ubuntu / Debian:**

```bash
sudo apt update
sudo apt install -y build-essential cmake
```

**Fedora:**

```bash
sudo dnf install -y gcc-c++ make cmake
```

**macOS** (cần Xcode Command Line Tools):

```bash
xcode-select --install
brew install cmake
```

### 4.2. Build và chạy

```bash
cd trongdongpalace
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/trongdongpalace
```

---

## 5. Vị trí dữ liệu và reset

### 5.1. Vị trí

Dữ liệu nằm trong `data/` với 9 file text:

```text
data/
├── users.txt
├── customers.txt
├── halls.txt
├── services.txt
├── requests.txt
├── request_lines.txt
├── bookings.txt
├── payments.txt
└── events.txt
```

Repo đã kèm dữ liệu demo: **6 sảnh** và **100 dịch vụ**, cùng 4 tài khoản và 3 khách hàng.

### 5.2. Reset dữ liệu

1. Đóng chương trình (Windows có thể khóa file/exe).
2. Sao lưu `data/` nếu cần.
3. Xóa các file trong `data/`.
4. Chạy lại chương trình.

Lần chạy đầu, chương trình tạo tài khoản/khách mẫu. **Sảnh và dịch vụ không tự sinh**;
hãy khôi phục `data/` từ git hoặc thêm mới bằng tài khoản `admin`.

### 5.3. Khôi phục dữ liệu demo

```powershell
git checkout -- data
```

hoặc:

```bash
git restore data
```

> Không sửa file trong `data/` khi chương trình đang chạy: lần lưu tiếp theo sẽ ghi đè.

---

## 6. VS Code (tùy chọn)

### 6.1. Extension

- **C/C++** (`ms-vscode.cpptools`)
- **CMake Tools** (`ms-vscode.cmake-tools`) — tùy chọn
- **Vietnamese Language Pack** — tùy chọn

### 6.2. Cấu hình IntelliSense

Tạo `.vscode/c_cpp_properties.json` (file này đã nằm trong `.gitignore`):

```json
{
  "configurations": [
    {
      "name": "Win32",
      "includePath": ["${workspaceFolder}/include"],
      "compilerPath": "C:/msys64/ucrt64/bin/g++.exe",
      "cStandard": "c17",
      "cppStandard": "c++17",
      "intelliSenseMode": "windows-gcc-x64"
    }
  ],
  "version": 4
}
```

### 6.3. Task build

Tạo `.vscode/tasks.json`:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build Trong Dong Palace",
      "type": "shell",
      "command": "cmake --build build",
      "group": { "kind": "build", "isDefault": true },
      "problemMatcher": ["$gcc"]
    }
  ]
}
```

> Thư mục `.vscode/` đã bị gitignore để tránh xung đột cấu hình giữa các máy.

---

## 7. Build sạch từ đầu

Khi đổi compiler, đổi CMake hoặc gặp lỗi cache:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Linux/macOS:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

---

## 8. Kiểm tra sau khi build

### 8.1. File exe

```powershell
Get-Item .\build\trongdongpalace.exe
```

### 8.2. Chạy smoke test

```powershell
.\build\trongdongpalace.exe
```

Kỳ vọng:

```text
==============================================================
     TRỐNG ĐỒNG PALACE - HỆ THỐNG ĐẶT LỊCH SỰ KIỆN
==============================================================

1. Đăng nhập
2. Đăng ký tài khoản khách hàng
0. Thoát chương trình
```

Đăng nhập `admin / 123456`, vào **6. Danh mục sảnh & dịch vụ** để kiểm tra dữ liệu.

### 8.3. Kiểm thử đầy đủ

Xem [TESTING.md](TESTING.md).

---

## 9. Lỗi thường gặp

### 9.1. `Permission denied` khi build trên Windows

Nguyên nhân: `trongdongpalace.exe` vẫn đang chạy.

```powershell
Stop-Process -Name trongdongpalace -Force -ErrorAction SilentlyContinue
cmake --build build
```

### 9.2. `cmake` hoặc `g++` không được nhận diện

- Mở đúng **MSYS2 UCRT64** (không phải Command Prompt).
- Kiểm tra `C:\msys64\ucrt64\bin` có trong `PATH` nếu dùng terminal ngoài.
- Chỉ định tuyệt đối `-DCMAKE_CXX_COMPILER` như mục 2.4.

### 9.3. CMake báo `Could not find CMAKE_CXX_COMPILER`

Chưa cài g++ hoặc chọn sai toolchain. Cài lại gói:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

### 9.4. Tiếng Việt bị lỗi font / dấu hỏi

- Chạy `trongdongpalace.exe`, không chạy trực tiếp file `.cpp`.
- Dùng **Windows Terminal** hoặc console hỗ trợ UTF-8.
- Không sửa source bằng editor lưu ANSI; luôn lưu **UTF-8**.

### 9.5. `Dong bookings.txt khong du truong`

File dữ liệu bị sửa tay hoặc thiếu trường. Khôi phục:

```powershell
git checkout -- data/bookings.txt
```

Nếu file có dữ liệu quan trọng, sửa để mỗi dòng có đủ **13 trường** (xem
[ARCHITECTURE.md](ARCHITECTURE.md#8-lưu-trữ-dữ-liệu)).

### 9.6. Không thấy sảnh/dịch vụ sau khi reset

Như mục 5.2: dữ liệu mẫu sảnh/dịch vụ không tự sinh. Khôi phục `data/` từ git hoặc
đăng nhập `admin` → **7. Quản lý sảnh** / **8. Quản lý dịch vụ** để thêm.

### 9.7. `[LỖI KHỞI ĐỘNG] Không mở được file...`

- Kiểm tra quyền ghi thư mục dự án.
- Chắc chắn thư mục `data/` tồn tại hoặc có thể tạo.
- Trên Windows, tránh chạy exe từ thư mục `Program Files`.

---

## 10. Gỡ / dọn dẹp

- Xóa thư mục `build/` để giải phóng dung lượng.
- Dữ liệu demo: xóa `data/` (sau khi sao lưu nếu cần).
- Không cần gỡ cài đặt gì vì chương trình không ghi registry, không tạo service.

---

## 11. Tóm tắt lệnh

```powershell
# Cấu hình
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Chạy
.\build\trongdongpalace.exe

# Build sạch
Remove-Item -Recurse -Force build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
