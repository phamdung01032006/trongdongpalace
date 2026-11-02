# Kiểm thử Trống Đồng Palace

Dự án hiện **chưa có bộ test tự động**. Tài liệu này là checklist kiểm thử thủ công, bám
theo nghiệp vụ và các menu thực tế của phiên bản console + lưu file TXT.

---

## 1. Chuẩn bị

### 1.1. Môi trường

- Build thành công theo [SETUP.md](SETUP.md).
- Dùng dữ liệu demo sạch:

  ```powershell
  git restore data
  ```

- Chạy từ thư mục gốc:

  ```powershell
  .\build\trongdongpalace.exe
  ```

### 1.2. Quy ước nhập liệu

| Mục | Cách nhập |
| :--- | :--- |
| Ngày giờ | `dd/mm/yyyy hh:mm` — ví dụ `25/12/2026 18:00` |
| Tiền | Số nguyên, có thể nhập `5000000` hoặc `5.000.000` |
| Chọn menu | Nhập số rồi Enter |
| Quay lại | Gõ `b` tại màn hình nhập liệu |
| Về menu chính | Gõ `m` tại màn hình nhập liệu |
| Xác nhận | `c` hoặc `có` / `k` hoặc `không` |
| Bỏ qua | Để trống rồi Enter (nếu màn hình cho phép) |

### 1.3. Tài khoản

| Tài khoản | Mật khẩu | Vai trò |
| :--- | :--- | :--- |
| `admin` | `123456` | Quản lý |
| `kinhdoanh` | `123456` | Nhân viên kinh doanh |
| `vanhanh` | `123456` | Vận hành |
| `khachhang` | `123456` | Khách hàng (gắn `CUS-0002`) |

---

## 2. Smoke test

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| SM-01 | Chạy exe | Hiện banner và menu đăng nhập, không lỗi |
| SM-02 | Chọn `0` | Thoát, in `Tạm biệt!` |
| SM-03 | Đăng nhập `admin / 123456` | Vào **MENU QUẢN LÝ** với 10 mục |
| SM-04 | Vào **6. Danh mục sảnh & dịch vụ** | Thấy 6 sảnh và 100 dịch vụ |
| SM-05 | Đăng xuất, đăng nhập `vanhanh` | Chỉ thấy **1. Danh sách sự kiện** |
| SM-06 | Thoát và chạy lại | Dữ liệu vừa xem vẫn còn |

---

## 3. Đăng nhập, đăng ký, phân quyền

### 3.1. Đăng nhập

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| AU-01 | `admin` / `123456` | Đăng nhập thành công, menu Quản lý |
| AU-02 | `admin` / `sai` | Báo lỗi, vẫn ở màn hình đăng nhập |
| AU-03 | Tài khoản không tồn tại | Báo lỗi, không vào hệ thống |
| AU-04 | Đăng nhập rồi chọn `0` | Đăng xuất về menu chính |

### 3.2. Đăng ký khách hàng

Vào menu chính → **2. Đăng ký tài khoản khách hàng**.

| ID | Dữ liệu | Kết quả mong đợi |
| :--- | :--- | :--- |
| RG-01 | Username mới, mật khẩu ≥ 4, nhập lại khớp, tên/SĐT/email hợp lệ | Tạo tài khoản + hồ sơ `CUS-...`; đăng nhập được |
| RG-02 | Mật khẩu 3 ký tự | Lỗi “Mật khẩu tối thiểu 4 ký tự.” |
| RG-03 | Nhập lại mật khẩu khác | Lỗi “Mật khẩu nhập lại không khớp.” |
| RG-04 | Username đã tồn tại (`admin`) | Lỗi trùng tên đăng nhập |
| RG-05 | SĐT 8 chữ số / chứa chữ | Lỗi số điện thoại không hợp lệ |
| RG-06 | SĐT đã dùng (`0912345678`) | Lỗi số điện thoại đã được dùng |
| RG-07 | Email `abc` | Lỗi email không đúng định dạng |
| RG-08 | Đăng nhập tài khoản mới | Vào **MENU KHÁCH HÀNG**, tạo được yêu cầu cho chính mình |

### 3.3. Phân quyền

| ID | Vai trò | Kỳ vọng |
| :--- | :--- | :--- |
| PM-01 | Khách hàng | Menu 1–5; không có quản lý sảnh/dịch vụ/tài khoản |
| PM-02 | Kinh doanh | Menu 1–6; không có quản lý sảnh/dịch vụ/tài khoản, không xem sự kiện |
| PM-03 | Quản lý | Menu 1–10, gồm quản lý sảnh/dịch vụ/tài khoản và sự kiện |
| PM-04 | Vận hành | Chỉ có danh sách sự kiện |
| PM-05 | Khách A sửa/xóa yêu cầu của khách B | Bị từ chối (nếu thao tác qua service) |

---

## 4. Yêu cầu sự kiện

Thực hiện bằng `kinhdoanh` hoặc `admin`.

### 4.1. Tạo yêu cầu

Vào **1. Quản lý yêu cầu sự kiện → 2. Tạo yêu cầu**.

Khi tới bước chọn dịch vụ, bảng dịch vụ đang cung cấp phải hiện trước lời nhắc nhập mã;
gõ `?` để in lại bảng.

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| RQ-01 | Chọn khách `CUS-0001`, loại Tiệc cưới, sảnh `HAL-0004`, thời gian tương lai, 200 khách, chọn `FOD01-P` số lượng 0 | Tạo `REQ-...`, tính tiền sảnh + dịch vụ; dịch vụ tự tính theo số khách |
| RQ-02 | Bỏ trống tên sự kiện | Lỗi “Tên sự kiện là bắt buộc.” |
| RQ-03 | SĐT `123` | Lỗi số điện thoại không hợp lệ |
| RQ-04 | Số khách `5` | Lỗi số khách phải từ 10 đến … |
| RQ-05 | Số khách lớn hơn sức chứa sảnh lớn nhất | Lỗi số khách vượt giới hạn |
| RQ-06 | Kết thúc trước bắt đầu | Màn hình nhập lại thời gian kết thúc |
| RQ-07 | Bắt đầu trong quá khứ hơn 1 giờ | Lỗi “Ngày diễn ra phải ở tương lai.” |
| RQ-08 | Chọn sảnh không tồn tại (`HAL-9999`) | Lỗi mã sảnh không tồn tại |
| RQ-09 | Chọn dịch vụ đã tắt | Lỗi dịch vụ ngừng cung cấp |
| RQ-10 | Không chọn dịch vụ | Yêu cầu vẫn tạo được, chỉ tính tiền sảnh |

### 4.2. Sửa, đổi trạng thái, xóa

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| RQ-11 | Sửa yêu cầu đang `Chờ` | Lưu thay đổi, tính lại tiền |
| RQ-12 | Đổi `Chờ → Đang tư vấn` | Trạng thái cập nhật |
| RQ-13 | Đổi sang `Hoàn thành` | Tự tạo booking, yêu cầu thành `Đã chốt` |
| RQ-14 | Đổi trạng thái yêu cầu `Đã chốt` | Bị chặn |
| RQ-15 | Hủy yêu cầu `Chờ` | Thành `Bị hủy` |
| RQ-16 | Xóa yêu cầu chưa có booking | Xóa thành công |
| RQ-17 | Xóa yêu cầu đã có booking | Bị chặn |
| RQ-18 | Khách hủy yêu cầu của mình | Thành `Bị hủy` |
| RQ-19 | Khách hủy yêu cầu người khác | Bị từ chối |

### 4.3. Tự động chọn sảnh

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| RQ-20 | Tạo yêu cầu không chọn sảnh, sau đó chuyển `Hoàn thành` | Hệ thống chọn sảnh trống đầu tiên đủ sức chứa |
| RQ-21 | Yêu cầu chọn sảnh đã bị trùng lịch, còn sảnh khác trống | Báo lỗi, **không** tự đổi sảnh, yêu cầu giữ nguyên trạng thái |
| RQ-22 | Yêu cầu chưa chọn sảnh và không còn sảnh nào trống | Báo lỗi, yêu cầu **không** đổi trạng thái |
| RQ-23 | Yêu cầu có `hallId` không còn tồn tại, còn sảnh khác trống | Tự chọn sảnh trống đầu tiên và cập nhật `hallId` |

---

## 5. Tra cứu sảnh và trùng lịch

Vào **4. Tra cứu sảnh trống** (khách hoặc kinh doanh).

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| HL-01 | Nhập thời gian trống, 100 khách, giá 0 | Hiện các sảnh đủ sức chứa và đang khả dụng |
| HL-02 | Giá tối đa thấp | Chỉ hiện sảnh có giá thuê ≤ mức nhập |
| HL-03 | Khoảng thời gian trùng booking `Chờ cọc` | Sảnh đó không xuất hiện |
| HL-04 | Khoảng thời gian trùng booking `Đã cọc` | Sảnh đó không xuất hiện |
| HL-05 | Khoảng thời gian trùng booking `Bị hủy` | Sảnh xuất hiện lại |
| HL-06 | Sảnh đang `Bảo trì` | Không xuất hiện |
| HL-07 | Kết thúc trước bắt đầu | Bắt nhập lại |

### 5.1. Kiểm tra biên giao thời gian

Tạo hai booking cùng sảnh `HAL-0001` (sức chứa 50):

- Booking A: `10/12/2026 10:00` → `12:00`.
- Booking B: `10/12/2026 12:00` → `14:00`.

| ID | Khoảng kiểm tra | Kết quả mong đợi |
| :--- | :--- | :--- |
| HL-08 | `10:00–12:00` và `12:00–14:00` | **Không** trùng, cả hai hợp lệ |
| HL-09 | `11:00–13:00` | Trùng cả hai, bị chặn |
| HL-10 | `09:00–10:01` | Trùng A 1 phút, bị chặn |
| HL-11 | `13:59–15:00` | Trùng B 1 phút, bị chặn |

---

## 6. Booking và hạn giữ chỗ 72 giờ

### 6.1. Tạo và xóa

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| BK-01 | Chuyển yêu cầu sang `Hoàn thành` | Tạo `BKG-...`, trạng thái `Đang chờ đặt cọc`, hạn giữ chỗ = lúc tạo + 72 giờ, tự tạo khoản `Đặt cọc lần 1` chưa thu |
| BK-02 | Xem chi tiết booking | Tổng tiền = giá sảnh + dịch vụ; có hạn giữ chỗ |
| BK-03 | Xóa booking `Chờ cọc`, chưa thu tiền | Xóa được; yêu cầu trở về `Hoàn thành`; khoản chưa thu bị xóa |
| BK-04 | Xóa booking `Đã cọc` | Bị chặn |
| BK-05 | Xóa booking `Hoàn tất` | Bị chặn |
| BK-06 | Hủy booking `Chờ cọc` | Thành `Bị hủy`, sảnh được giải phóng |
| BK-07 | Hủy booking `Đã cọc` | Thành `Bị hủy`, giữ lịch sử; sự kiện liên quan thành `Đã hủy` |
| BK-08 | Hủy booking đã hủy | Báo lỗi |

### 6.2. Hết hạn 72 giờ

1. Tạo một booking `Chờ cọc`, chưa thu tiền.
2. Thoát chương trình.
3. Mở `data/bookings.txt`, sửa trường `holdUntil` (trường 11) của booking thành một thời điểm trong quá khứ.
4. Chạy lại chương trình, đăng nhập.

| ID | Kết quả mong đợi |
| :--- | :--- |
| BK-09 | Booking tự chuyển `Bị hủy`, lý do “Quá hạn giữ chỗ 72 giờ chưa đặt cọc.” |
| BK-10 | Yêu cầu liên quan trở về `Hoàn thành` để có thể tạo booking lại |
| BK-11 | Sự kiện liên quan (nếu có) chuyển `Đã hủy` |
| BK-12 | Booking đã có cọc **không** bị quét hủy dù quá hạn |

> Dòng booking có **13 trường** theo thứ tự:
> `id|requestId|customerId|hallId|batDau|ketThuc|soKhach|tongDuKien|trangThai|createdAt|holdUntil|sales|cancelReason`.

---

## 7. Thanh toán

Vào **3. Quản lý thanh toán**.

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| PY-01 | Ghi nhận `Đặt cọc lần 1`, số tiền > 0 | Khoản thu `Đã thanh toán`; booking `Đã cọc`; tự tạo sự kiện |
| PY-02 | Ghi nhận `Đặt cọc lần 2` | Ghi nhận bình thường; booking vẫn `Đã cọc` |
| PY-03 | Ghi nhận `Tất toán` đúng số còn lại | Booking `Hoàn tất` |
| PY-04 | `Tất toán` ít hơn số còn lại | Bị chặn |
| PY-05 | Số tiền vượt số còn lại | Bị chặn, báo số còn lại |
| PY-06 | Số tiền = 0 | Bị chặn |
| PY-07 | Ghi chú để trống | Bị chặn |
| PY-08 | Ngày thu ở tương lai | Bị chặn |
| PY-09 | Thu tiền booking đã hủy | Bị chặn |
| PY-10 | Booking đã thu đủ, thu thêm | Bị chặn “Booking đã thu đủ tiền.” |
| PY-11 | Sửa khoản thu | Cập nhật số tiền/hình thức/ngày/ghi chú; trạng thái vẫn `Đã thanh toán` |
| PY-12 | Xóa khoản `Chưa thanh toán` | Xóa được |
| PY-13 | Xóa khoản `Đã thanh toán` | Bị chặn |
| PY-14 | In phiếu thu | Hiện mã phiếu, booking, khách, sảnh, số tiền, hình thức, ngày, người thu |

### 7.1. Kiểm tra tự tạo khoản cọc

| ID | Kết quả mong đợi |
| :--- | :--- |
| PY-15 | Khi tạo booking, `data/payments.txt` có dòng `Đặt cọc lần 1` với số tiền `0`, trạng thái `Chưa thanh toán` |
| PY-16 | Khi ghi nhận đúng loại `Đặt cọc lần 1`, hệ thống điền vào dòng có sẵn thay vì tạo dòng trùng |

---

## 8. Sự kiện

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| EV-01 | Thu đặt cọc lần đầu | Sinh đúng **một** sự kiện cho booking |
| EV-02 | Thu thêm/tất toán | Không sinh sự kiện trùng |
| EV-03 | `admin` xem **10. Danh sách sự kiện** | Thấy sự kiện, trạng thái `Sắp diễn ra` nếu tương lai |
| EV-04 | `vanhanh` đăng nhập | Thấy menu sự kiện và xem được |
| EV-05 | `kinhdoanh` / `khachhang` | Không có màn hình danh sách sự kiện |
| EV-06 | Hủy booking đã có sự kiện | Sự kiện chuyển `Đã hủy` |
| EV-07 | Sự kiện có `start` = hiện tại | Trạng thái `Đang diễn ra` |
| EV-08 | Sự kiện có `end` < hiện tại | Trạng thái `Đã kết thúc` |

> Có thể kiểm tra EV-07/EV-08 bằng cách tạo yêu cầu có thời gian gần hiện tại (trong
> tương lai 1–2 phút), đặt cọc, rồi xem lại sau khi thời gian trôi qua. Không cần chỉnh
> đồng hồ hệ thống.

---

## 9. Quản lý danh mục và tài khoản

### 9.1. Sảnh

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| HV-01 | Thêm sảnh mới | Có mã `HAL-...`, hiện trong danh mục |
| HV-02 | Sửa tên/sức chứa/giá | Cập nhật thành công |
| HV-03 | Đổi trạng thái `Bảo trì` | Sảnh không còn trong kết quả tra cứu |
| HV-04 | Xóa sảnh đã có booking | Bị chặn |
| HV-05 | Xóa sảnh chưa dùng | Xóa thành công |

### 9.2. Dịch vụ

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| SV-01 | Thêm dịch vụ `FOOD` | Lưu với type `FOOD`, tự tính số lượng theo đơn vị |
| SV-02 | Thêm dịch vụ `PACKAGE` | Lưu với type `PACKAGE` |
| SV-03 | Tắt dịch vụ | Không chọn được trong yêu cầu mới |
| SV-04 | Xóa dịch vụ đã dùng trong yêu cầu | Bị chặn; snapshot cũ vẫn giữ nguyên |
| SV-05 | Sửa giá dịch vụ | Yêu cầu cũ giữ đơn giá snapshot, yêu cầu mới dùng giá mới |

### 9.3. Tài khoản

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| UV-01 | Tạo tài khoản nhân viên | Đăng nhập được với vai trò đã chọn |
| UV-02 | Đổi vai trò | Menu sau lần đăng nhập sau thay đổi theo vai trò |
| UV-03 | Khóa tài khoản | Tài khoản không đăng nhập được |
| UV-04 | Mở khóa | Đăng nhập lại được |
| UV-05 | Đổi mật khẩu | Mật khẩu cũ không dùng được, mật khẩu mới dùng được |
| UV-06 | Xóa tài khoản | Không còn trong danh sách |
| UV-07 | Hạ quyền quản lý cuối cùng | Bị chặn |
| UV-08 | Tự xóa tài khoản đang đăng nhập | Bị chặn |

---

## 10. Lưu trữ và phục hồi

| ID | Thao tác | Kết quả mong đợi |
| :--- | :--- | :--- |
| DT-01 | Tạo yêu cầu, thoát, mở lại | Yêu cầu còn nguyên, đúng mã |
| DT-02 | Tạo booking + thanh toán, thoát, mở lại | Booking, khoản thu, sự kiện còn nguyên |
| DT-03 | Kiểm tra `data/bookings.txt` | Mỗi dòng có 13 trường, đúng thứ tự |
| DT-04 | Kiểm tra file UTF-8 | Tiếng Việt hiển thị đúng khi mở lại trong app |
| DT-05 | Xóa một dòng trong `requests.txt` | Chương trình vẫn tải các yêu cầu còn lại |
| DT-06 | Sửa tay một dòng booking thiếu trường | Chương trình báo `Dong bookings.txt khong du truong` |
| DT-07 | Khôi phục `data/` từ git | Dữ liệu demo trở về trạng thái ban đầu |
| DT-08 | Xóa toàn bộ `data/` rồi chạy | Tạo tài khoản/khách mẫu; sảnh/dịch vụ trống (phải thêm hoặc khôi phục) |

### 10.1. Tương thích định dạng cũ (tùy chọn)

`Booking::fromText()` vẫn đọc được dòng 17 trường của phiên bản trước khi bỏ chiết khấu.
Nếu có dữ liệu cũ, sao lưu file rồi kiểm tra:

- Ứng dụng mở được, không báo thiếu trường.
- Trạng thái booking đọc đúng.
- Sau lần lưu tiếp theo, file được ghi lại theo 13 trường hiện tại.

---

## 11. Tiêu chí pass

Một bản build được coi là đạt khi:

- [ ] Tất cả mục **Smoke test** pass.
- [ ] Ít nhất một luồng end-to-end pass: đăng ký → yêu cầu → booking → đặt cọc → sự kiện → tất toán.
- [ ] Các test phân quyền pass.
- [ ] Không có lỗi build mới.
- [ ] Dữ liệu đọc lại sau khi khởi động lại.
- [ ] Không có thay đổi ngoài phạm vi trong `data/` khi kết thúc kiểm thử.

---

## 12. Ghi nhận lỗi

Khi phát hiện lỗi, ghi lại:

1. Mã test (ví dụ `PY-05`).
2. Các bước tái hiện.
3. Mã yêu cầu/booking/khoản thu liên quan.
4. Thông báo lỗi nguyên văn.
5. Ảnh chụp màn hình (nếu có).
6. File `data/` liên quan (đã ẩn thông tin cá nhân).

Xem thêm quy trình báo lỗi trong [CONTRIBUTING.md](../CONTRIBUTING.md).
