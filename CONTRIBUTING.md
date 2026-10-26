# Đóng góp cho Trống Đồng Palace

Cảm ơn bạn đã quan tâm đến dự án. Tài liệu này mô tả cách cài đặt, quy ước code và
quy trình gửi thay đổi để bản build luôn ổn định.

---

## 1. Trước khi bắt đầu

1. Đọc [README.md](README.md) để hiểu nghiệp vụ và cách chạy.
2. Cài môi trường theo [docs/SETUP.md](docs/SETUP.md).
3. Build thành công và chạy được smoke test:

   ```powershell
   cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   .\build\trongdongpalace.exe
   ```

4. Đọc [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) để biết code nào đặt ở đâu.

---

## 2. Quy trình đóng góp

### 2.1. Tạo nhánh

Tạo nhánh từ `master` (hoặc nhánh được giao):

```bash
git switch -c feat/ten-ngan-gon
```

Tiền tố gợi ý:

| Tiền tố | Dùng cho |
| :--- | :--- |
| `feat/` | Tính năng mới |
| `fix/` | Sửa lỗi |
| `docs/` | Tài liệu |
| `refactor/` | Tái cấu trúc, không đổi hành vi |
| `test/` | Bổ sung kiểm thử |
| `chore/` | Build, cấu hình, dọn dẹp |

### 2.2. Commit

Viết commit ngắn gọn, nêu **việc đã làm**, ví dụ:

```text
feat: them trang thai dat coc lan 2
fix: chan xoa booking da thu tien
docs: cap nhat so do kien truc
```

Không commit các file sinh ra khi build:

- `build/`, `build-*/`
- `*.exe`, `*.o`, `*.obj`
- `.vscode/`, `.idea/`
- `data/*.tmp`

Các file trong `data/` là dữ liệu demo có chủ đích. Nếu thay đổi dữ liệu khi test,
hãy khôi phục trước khi commit:

```powershell
git restore data
```

### 2.3. Trước khi gửi

- [ ] Build sạch, không có warning mới.
- [ ] Chạy smoke test và các test liên quan trong [docs/TESTING.md](docs/TESTING.md).
- [ ] Không đưa thông tin bí mật, mật khẩu thật hay dữ liệu cá nhân vào repo.
- [ ] Cập nhật tài liệu nếu thay đổi nghiệp vụ, menu hoặc định dạng file.
- [ ] Commit message mô tả đúng nội dung.

---

## 3. Quy ước code

### 3.1. Ngôn ngữ và encoding

- Chuẩn C++17, không dùng thư viện ngoài.
- Tất cả file source là **UTF-8** (không BOM).
- Chuỗi hiển thị cho người dùng dùng tiếng Việt có dấu.
- Comment trong code giữ phong cách hiện tại: tiếng Việt không dấu, ngắn gọn, giải thích
  **vì sao** hơn là kể lại code.

### 3.2. Định dạng

- Thụt lề **4 dấu cách**, không dùng tab.
- Dấu `{` mở đầu hàm/lớp đặt cùng dòng khai báo.
- Tên lớp: `PascalCase` (`PalaceService`, `EventRequest`).
- Tên hàm: `camelCase` (`recordPayment`, `findAvailableHalls`).
- Tên biến/thành viên: `snake_case` cho getter/setter theo style hiện có
  (`get_fullName`, `set_holdUntil`); biến cục bộ `camelCase`.
- Hằng số: `k` + `PascalCase` (`kHoldHours`).
- Không dùng `using namespace std;` trong header mới nếu có thể tránh; các header hiện tại
  đang dùng để thống nhất.

### 3.3. Phân lớp trách nhiệm

Đây là quy tắc quan trọng nhất:

| Lớp | Được làm | Không được làm |
| :--- | :--- | :--- |
| `ConsoleUI` | Nhập/xuất, menu, gọi service | Sửa trực tiếp `Database`, tự tính nghiệp vụ |
| `PalaceService` | Kiểm tra quyền, quy tắc nghiệp vụ, gọi `Store::save()` | In menu, đọc bàn phím |
| `Store` | Đọc/ghi file, giữ `Database` | Quyết định nghiệp vụ |
| Entity (`Booking`, `Payment`...) | Dữ liệu, `toText`/`fromText`, hàm hỗ trợ nhỏ | Gọi UI hoặc service |

- Mọi thao tác nghiệp vụ thay đổi dữ liệu phải đi qua `PalaceService` và kết thúc bằng
  `persist()`; riêng `refreshSystem()` tự gọi `Store::save()` khi phát hiện thay đổi.
- Kiểm tra phân quyền đặt ở **đầu hàm service** bằng `require(Action::...)`.
- Lỗi nghiệp vụ dùng `BusinessException`; lỗi file dùng `FileException`.

### 3.4. Thêm hoặc sửa entity

Khi thêm trường mới:

1. Cập nhật constructor, getter/setter trong header.
2. Cập nhật `toText()` và `fromText()` cùng thứ tự trường.
3. Cập nhật header tương ứng trong `Store::save...()`.
4. Cập nhật dữ liệu mẫu/tài liệu định dạng file.
5. Chạy test lưu → thoát → mở lại để chắc dữ liệu đọc được.

> Đây là lỗi dễ gặp nhất: sửa `toText()` nhưng quên `fromText()` hoặc quên header file.

### 3.5. Thêm enum hoặc trạng thái

- Cập nhật `Enums.h`.
- Cập nhật `toString()` và hàm `...FromString()` trong `Enums.cpp`.
- Cập nhật state machine, menu hiển thị và tài liệu kiến trúc.
- Đảm bảo dữ liệu cũ vẫn đọc được (giữ giá trị text cũ hoặc có fallback).

### 3.6. Thêm menu

- Khai báo hàm trong `ConsoleUI.h`.
- Thêm mục menu và `case` trong menu vai trò phù hợp.
- Thêm `require(...)` ở service nếu là thao tác nghiệp vụ.
- Cập nhật README và ảnh hưởng đến phân quyền.

---

## 4. Kiểm thử

Dự án hiện kiểm thử thủ công. Trước khi gửi thay đổi:

1. Chạy build sạch.
2. Làm theo checklist [docs/TESTING.md](docs/TESTING.md).
3. Ít nhất kiểm tra các luồng:
   - Đăng nhập 4 vai trò.
   - Tạo yêu cầu → Hoàn thành → tạo booking.
   - Ghi nhận đặt cọc → booking Đã cọc + sự kiện xuất hiện.
   - Tất toán → booking Hoàn tất.
   - Thoát và mở lại → dữ liệu còn nguyên.

Nếu sửa logic ngày giờ hoặc trùng lịch, hãy thử cả trường hợp biên:

- Booking kết thúc đúng lúc booking khác bắt đầu (không trùng).
- Booking giao 1 phút (trùng).
- Hạn giữ chỗ đúng 72 giờ.

---

## 5. Báo lỗi

Khi mở issue/báo lỗi, cung cấp:

- Hệ điều hành và compiler (`g++ --version`, `cmake --version`).
- Các bước tái hiện, dữ liệu đầu vào (mã yêu cầu/booking nếu có).
- Thông báo lỗi đầy đủ.
- Ảnh chụp màn hình nếu lỗi hiển thị.
- File `data/` liên quan **sau khi đã xóa dữ liệu cá nhân**.

Không gửi kèm mật khẩu thật hoặc thông tin cá nhân của người khác.

---

## 6. Câu hỏi thường gặp

**Có được thêm thư viện ngoài không?**

Mục tiêu của đồ án là C++17 thuần, không phụ thuộc. Nếu thật sự cần, hãy thảo luận trước
và cập nhật `CMakeLists.txt` + tài liệu cài đặt.

**Có được đổi sang JSON/SQLite không?**

Có thể, nhưng là thay đổi lớn: cần thay `Store`, cập nhật tài liệu, test migration và
không được làm vỡ dữ liệu cũ. Hãy tách thành nhánh riêng.

**Vì sao mật khẩu lưu plain text?**

Đây là lựa chọn đơn giản hóa cho đồ án. Không dùng cho hệ thống thật. Nếu nâng cấp, thêm
hash + salt và test đăng nhập/đổi mật khẩu.
