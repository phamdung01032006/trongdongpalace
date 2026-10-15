# Kiểm thử nghiệp vụ, giao diện Qt và refactor phong cách OOP

## Mục tiêu và file thay đổi

1. Đưa constructor, initializer list và hành vi ngắn vào trong các class model;
   giữ định dạng dữ liệu và hành vi đã có. File sửa: `src/models/Customer.cpp`,
   `Hall.cpp`, `User.cpp`, `ServiceItem.cpp`, `EventRequest.cpp`, `Booking.cpp`,
   `Payment.cpp`, `Event.cpp`.
2. Làm rõ constructor lớp con và giữ đa hình qua `shared_ptr<Event>`.
   File sửa: `src/models/Event.cpp`.
3. Phân loại ngoại lệ: thêm `src/models/Exceptions.cpp`; sửa các hàm parse
   trong `src/utils/text_fields.cpp`, kiểm tra ID ở `src/storage/text_storage.cpp`,
   xác thực và kiểm tra lịch trong `src/services/palace_service.cpp`.
4. Bổ sung test trong `tests/test_core.cpp`; viết báo cáo tại `README.md`
   và đặc tả UML tại `docs/CLASS_DIAGRAM.md`.

Giữ ứng dụng Qt và tám file TXT hiện tại. Không thay `CMakeLists.txt`,
`src/main.cpp`, `src/ui/app_ui.cpp` hoặc dữ liệu thực trong lần refactor này.
Các thay đổi ở những file này đã có trong workspace trước khi bắt đầu.

Code cài đặt nằm trực tiếp trong các file trên. UI gọi service để kiểm tra
quyền/nghiệp vụ; storage chịu trách nhiệm tách cột, đọc và ghi file.

## Chạy kiểm thử

```powershell
cmake --preset qt-ucrt64
cmake --build --preset qt-ucrt64 -j 2
ctest --test-dir build/qt-ucrt64 --output-on-failure
```

`tests/test_core.cpp` dùng thư mục tạm riêng, không sửa `data/` của dự án.
`tests/test_ui.cpp` chạy Qt ở chế độ `offscreen`, bấm nút vào vai khách,
điền form thật, gửi yêu cầu và đọc lại TXT. CTest giới hạn test giao diện 20 giây.

Kết quả kiểm tra sau refactor ngày 15/10/2026:

- Build thành công ứng dụng Qt và hai executable kiểm thử.
- CTest: `core` và `guest_ui` đều đạt, 2/2 bộ test.
- `tdp_tests`: 211/211 kiểm tra đạt, không có lỗi.

## Test cases và expected result

| Nhóm | Test case | Kết quả mong đợi |
| --- | --- | --- |
| Lưu TXT | Tạo khách → yêu cầu → booking → thanh toán, tạo storage khác và load | Giữ nguyên ID, liên kết, giá trị và các file TXT |
| Ký tự đặc biệt | Tên/ghi chú chứa dấu phẩy, `|`, `\`, xuống dòng và tiếng Việt | Đọc lại đúng nội dung sau kiểm tra/trim đầu vào của service |
| Khách vãng lai | Không đăng nhập, nhập tên, tuổi 25, SĐT, thời gian hợp lệ | Tạo hồ sơ và yêu cầu, không tạo tài khoản |
| Đầu vào sai | Thiếu tên, SĐT sai, tuổi 0/121, số khách sai | Từ chối, không thêm hồ sơ/yêu cầu rác |
| ID không tồn tại | Sảnh hoặc dịch vụ không tồn tại | Từ chối yêu cầu |
| ID trùng | Nhân đôi bản ghi trong `users.txt` | Load throw `StorageError`, file giữ nguyên |
| Trùng tên đăng nhập/SĐT | Đăng ký trùng; khách nhập cùng SĐT khác tên | Từ chối, không ghi đè hồ sơ |
| Thời gian sai | Bắt đầu bằng/sau kết thúc, ngày quá khứ | Từ chối |
| Overlap | Hai booking cùng sảnh có thời gian giao nhau | Booking thứ hai bị từ chối |
| Giáp giờ | Booking sau bắt đầu đúng giờ booking trước kết thúc | Cho phép nếu các điều kiện khác hợp lệ |
| Phân quyền khách | Gọi sửa trạng thái, xóa yêu cầu, tạo booking/thanh toán khi chưa đăng nhập | Từ chối, chỉ cho gửi yêu cầu và tra cứu công khai |
| Đăng nhập | Nhân viên đăng nhập, khách có tài khoản đăng nhập | Quyền đúng vai trò, hồ sơ khách được tự điền |
| File lỗi | Sai số cột, sai enum, số/ngày/bool sai, escape thiếu | Báo lỗi, không ghi đè hoặc tự reset |
| File thiếu | Đổi tên `users.txt` trong bộ dữ liệu đang có | Báo thiếu file, không seed lại |
| Bộ dữ liệu rỗng | Lưu bộ dữ liệu có đủ file nhưng không có bản ghi rồi load | Vẫn rỗng, không tự tạo mẫu |
| Đa hình | Tạo cưới, sinh nhật, hội thảo qua `shared_ptr<Event>` | Hướng dẫn chuẩn bị khác nhau |
| Đa hình sau load | Serialize/deserialize các loại sự kiện | Giữ nguyên loại và hành vi virtual |
| Thanh toán | Số tiền âm, thiếu ghi chú, ngày tương lai | Từ chối, luồng thanh toán hợp lệ vẫn chạy |
| Qt | Bấm “Tiếp tục không đăng nhập”, điền form, gửi | Menu công khai, tên/SĐT trống ban đầu, tuổi được lưu |
| Ngoại lệ lịch | Gọi `validateBookingSlot()` với ID thiếu, lịch đảo ngược, lịch giao nhau | Lần lượt throw `NotFoundException`, `ValidationException`, `BookingConflictException` |
| Chuyển lỗi cho Qt | Gọi `hallAvailable()` khi trùng lịch | Trả false và lý do, không để exception vượt ra callback |
| Lý do đã hết lỗi | Gọi `hallAvailable()` hợp lệ với chuỗi reason cũ | Trả true và xóa reason cũ |
| Bỏ qua booking đang kiểm tra | Truyền đúng `ignoreBookingId` | Không tự xung đột với booking đó |
| Ngoại lệ số | Chuỗi abc, số âm, số vượt long long | Đều throw `ValidationException` |
| ID trùng trong RAM | Thêm bản ghi cùng ID rồi save | Throw `DuplicateException` trước khi ghi; bản trên đĩa vẫn nạp lại được |
| Xác thực lỗi | Đăng nhập mật khẩu sai | Trả `OpResult` lỗi, không tạo phiên đăng nhập |

## Nghiệm thu thủ công

1. Mở ứng dụng, vào không đăng nhập, gửi yêu cầu với tên, tuổi và SĐT mới.
2. Đăng xuất/về màn hình đầu, đăng nhập `kinhdoanh` / `123456`.
3. Xác nhận yêu cầu mới xuất hiện, tạo booking cho một sảnh.
4. Tạo yêu cầu khác và thử booking cùng sảnh, cùng giờ: phải bị từ chối.
5. Thoát ứng dụng hoàn toàn, mở lại, đăng nhập nhân viên: dữ liệu còn nguyên.
6. Đăng nhập `khachhang`: khi tạo yêu cầu, họ tên và SĐT đã được điền sẵn.
7. Đăng nhập `vanhanh`: bảng sự kiện có cột Chuẩn bị theo loại sự kiện.

## Acceptance criteria

- CMake build được ứng dụng và các test; CTest không có test thất bại.
- Không còn `.h/.hpp` trong mã nguồn của dự án, không còn namespace tự định nghĩa.
- Không có include/link `nlohmann/json`; chương trình ghi 8 file TXT trong `data/`.
- Mở lại vẫn đọc được dữ liệu đã lưu, kể cả khi khởi chạy từ thư mục khác.
- Khách không cần tài khoản để gửi yêu cầu; chức năng nội bộ vẫn yêu cầu đăng nhập.
- `Event` là abstract class; lớp con override và hành vi virtual xuất hiện ở UI.
- File lỗi/thiếu hoặc ID trùng được báo lỗi và giữ nguyên dữ liệu trên đĩa.
- Các constructor sau refactor vẫn nạp/ghi được dữ liệu theo định dạng TXT cũ.
- Ca trùng lịch throw đúng kiểu khi gọi hàm kiểm tra trực tiếp; luồng Qt nhận thông báo lỗi.
- README đủ năm nội dung báo cáo; đặc tả draw.io khớp tên lớp và quan hệ hiện có.
