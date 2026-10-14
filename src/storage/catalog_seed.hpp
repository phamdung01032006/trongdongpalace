#pragma once

// Bang danh muc mau theo dac ta muc 2 (EVT/FOD/DEC/MED/AV/VEN/HR/EXT).
// Dung chung cho seed lan dau va nut "Nap danh muc mau" cua Quan ly.

#include "models/entities.hpp"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace tdp {

struct SeedService {
    const char* code;
    const char* name;
    const char* unit;
    int64_t price;
    const char* category;
};

// Danh muc dich vu theo dac ta muc 2 (gia gia lap tham khao).
const SeedService kSeedServices[] = {
    {"FOD01-S", "Ẩm thực Việt Nam (Standard)", "VNĐ/bàn", 3000000, "Ẩm thực"},
    {"FOD01-P", "Ẩm thực Việt Nam (Premium)", "VNĐ/bàn", 4500000, "Ẩm thực"},
    {"FOD01-L", "Ẩm thực Việt Nam (Luxury)", "VNĐ/bàn", 6500000, "Ẩm thực"},
    {"FOD02-S", "Ẩm thực Phương Tây (Standard)", "VNĐ/bàn", 4000000, "Ẩm thực"},
    {"FOD02-P", "Ẩm thực Phương Tây (Premium)", "VNĐ/bàn", 6000000, "Ẩm thực"},
    {"FOD02-L", "Ẩm thực Phương Tây (Luxury)", "VNĐ/bàn", 9000000, "Ẩm thực"},
    {"FOD03-S", "Ẩm thực Nhật Bản (Standard)", "VNĐ/bàn", 4500000, "Ẩm thực"},
    {"FOD03-P", "Ẩm thực Nhật Bản (Premium)", "VNĐ/bàn", 7000000, "Ẩm thực"},
    {"FOD03-L", "Ẩm thực Nhật Bản (Luxury)", "VNĐ/bàn", 12000000, "Ẩm thực"},
    {"FOD04-S", "Ẩm thực Hàn Quốc (Standard)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD04-P", "Ẩm thực Hàn Quốc (Premium)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD04-L", "Ẩm thực Hàn Quốc (Luxury)", "VNĐ/bàn", 7500000, "Ẩm thực"},
    {"FOD05-S", "Ẩm thực Trung Hoa (Standard)", "VNĐ/bàn", 4000000, "Ẩm thực"},
    {"FOD05-P", "Ẩm thực Trung Hoa (Premium)", "VNĐ/bàn", 6000000, "Ẩm thực"},
    {"FOD05-L", "Ẩm thực Trung Hoa (Luxury)", "VNĐ/bàn", 9000000, "Ẩm thực"},
    {"FOD06-S", "Ẩm thực Thái Lan (Standard)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD06-P", "Ẩm thực Thái Lan (Premium)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD06-L", "Ẩm thực Thái Lan (Luxury)", "VNĐ/bàn", 7000000, "Ẩm thực"},
    {"FOD07-S", "Ẩm thực Fusion (Standard)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"FOD07-P", "Ẩm thực Fusion (Premium)", "VNĐ/bàn", 7500000, "Ẩm thực"},
    {"FOD07-L", "Ẩm thực Fusion (Luxury)", "VNĐ/bàn", 12000000, "Ẩm thực"},
    {"FOD08-S", "Buffet Á - Âu (Standard)", "VNĐ/khách", 350000, "Ẩm thực"},
    {"FOD08-P", "Buffet Á - Âu (Premium)", "VNĐ/khách", 600000, "Ẩm thực"},
    {"FOD08-L", "Buffet Á - Âu (Luxury)", "VNĐ/khách", 1000000, "Ẩm thực"},
    {"FOD09-S", "Finger Food (Standard)", "VNĐ/khách", 150000, "Ẩm thực"},
    {"FOD09-P", "Finger Food (Premium)", "VNĐ/khách", 250000, "Ẩm thực"},
    {"FOD09-L", "Finger Food (Luxury)", "VNĐ/khách", 400000, "Ẩm thực"},
    {"FOD10-S", "Thực đơn chay (Standard)", "VNĐ/bàn", 2500000, "Ẩm thực"},
    {"FOD10-P", "Thực đơn chay (Premium)", "VNĐ/bàn", 3500000, "Ẩm thực"},
    {"FOD10-L", "Thực đơn chay (Luxury)", "VNĐ/bàn", 5000000, "Ẩm thực"},
    {"DEC01-S", "Trang trí Classic (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC01-P", "Trang trí Classic (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC01-L", "Trang trí Classic (Luxury)", "VNĐ/gói", 40000000, "Trang trí"},
    {"DEC02-S", "Trang trí Luxury (Basic)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC02-P", "Trang trí Luxury (Premium)", "VNĐ/gói", 45000000, "Trang trí"},
    {"DEC02-L", "Trang trí Luxury (Luxury)", "VNĐ/gói", 100000000, "Trang trí"},
    {"DEC03-S", "Trang trí Minimalist (Basic)", "VNĐ/gói", 5000000, "Trang trí"},
    {"DEC03-P", "Trang trí Minimalist (Premium)", "VNĐ/gói", 12000000, "Trang trí"},
    {"DEC03-L", "Trang trí Minimalist (Luxury)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC04-S", "Trang trí Romantic (Basic)", "VNĐ/gói", 10000000, "Trang trí"},
    {"DEC04-P", "Trang trí Romantic (Premium)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC04-L", "Trang trí Romantic (Luxury)", "VNĐ/gói", 50000000, "Trang trí"},
    {"DEC05-S", "Trang trí Rustic (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC05-P", "Trang trí Rustic (Premium)", "VNĐ/gói", 18000000, "Trang trí"},
    {"DEC05-L", "Trang trí Rustic (Luxury)", "VNĐ/gói", 35000000, "Trang trí"},
    {"DEC06-S", "Trang trí Floral (Basic)", "VNĐ/gói", 12000000, "Trang trí"},
    {"DEC06-P", "Trang trí Floral (Premium)", "VNĐ/gói", 30000000, "Trang trí"},
    {"DEC06-L", "Trang trí Floral (Luxury)", "VNĐ/gói", 70000000, "Trang trí"},
    {"DEC07-S", "Trang trí Modern (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC07-P", "Trang trí Modern (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC07-L", "Trang trí Modern (Luxury)", "VNĐ/gói", 45000000, "Trang trí"},
    {"DEC08-S", "Trang trí Themed (Basic)", "VNĐ/gói", 10000000, "Trang trí"},
    {"DEC08-P", "Trang trí Themed (Premium)", "VNĐ/gói", 30000000, "Trang trí"},
    {"DEC08-L", "Trang trí Themed (Luxury)", "VNĐ/gói", 80000000, "Trang trí"},
    {"DEC09-S", "Trang trí Royal (Basic)", "VNĐ/gói", 25000000, "Trang trí"},
    {"DEC09-P", "Trang trí Royal (Premium)", "VNĐ/gói", 60000000, "Trang trí"},
    {"DEC09-L", "Trang trí Royal (Luxury)", "VNĐ/gói", 150000000, "Trang trí"},
    {"DEC10-S", "Trang trí Festive (Basic)", "VNĐ/gói", 8000000, "Trang trí"},
    {"DEC10-P", "Trang trí Festive (Premium)", "VNĐ/gói", 20000000, "Trang trí"},
    {"DEC10-L", "Trang trí Festive (Luxury)", "VNĐ/gói", 50000000, "Trang trí"},
    {"MED01", "Chụp ảnh Basic Photography", "VNĐ/gói", 2000000, "Chụp ảnh & quay phim"},
    {"MED02", "Chụp ảnh Standard Photography", "VNĐ/gói", 3500000, "Chụp ảnh & quay phim"},
    {"MED03", "Chụp ảnh Premium Photography", "VNĐ/gói", 6000000, "Chụp ảnh & quay phim"},
    {"MED04", "Chụp ảnh Luxury Photography", "VNĐ/gói", 10000000, "Chụp ảnh & quay phim"},
    {"MED05", "Quay phim Basic Videography", "VNĐ/gói", 2500000, "Chụp ảnh & quay phim"},
    {"MED06", "Quay Highlight Film", "VNĐ/gói", 4500000, "Chụp ảnh & quay phim"},
    {"MED07", "Gói Full Package chụp + quay", "VNĐ/gói", 8300000, "Chụp ảnh & quay phim"},
    {"MED08", "Quay Cinematic Package", "VNĐ/gói", 12000000, "Chụp ảnh & quay phim"},
    {"MED09", "Quay Multi-Camera Package", "VNĐ/gói", 15000000, "Chụp ảnh & quay phim"},
    {"MED10", "Gói Premium Full chụp + quay", "VNĐ/gói", 20000000, "Chụp ảnh & quay phim"},
    {"AV01", "Âm thanh Basic Audio", "VNĐ/gói", 3000000, "Âm thanh & ánh sáng"},
    {"AV02", "Âm thanh Standard Audio", "VNĐ/gói", 5000000, "Âm thanh & ánh sáng"},
    {"AV03", "Âm thanh Premium Audio", "VNĐ/gói", 10000000, "Âm thanh & ánh sáng"},
    {"AV04", "Ánh sáng Basic Lighting", "VNĐ/gói", 3000000, "Âm thanh & ánh sáng"},
    {"AV05", "Ánh sáng Dynamic Lighting", "VNĐ/gói", 7000000, "Âm thanh & ánh sáng"},
    {"AV06", "Ánh sáng Wedding Lighting", "VNĐ/gói", 8000000, "Âm thanh & ánh sáng"},
    {"AV07", "Gala Standard AV", "VNĐ/gói", 12000000, "Âm thanh & ánh sáng"},
    {"AV08", "Gala Premium AV", "VNĐ/gói", 25000000, "Âm thanh & ánh sáng"},
    {"AV09", "Concert Package AV", "VNĐ/gói", 40000000, "Âm thanh & ánh sáng"},
    {"AV10", "Full AV Package", "VNĐ/gói", 30000000, "Âm thanh & ánh sáng"},
    {"HR01", "MC cơ bản", "VNĐ/buổi", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR02", "MC chuyên nghiệp", "VNĐ/buổi", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR03", "MC song ngữ", "VNĐ/buổi", 8000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR04", "Lễ tân sự kiện", "VNĐ/người/buổi", 500000, "Nhân sự & dịch vụ bổ sung"},
    {"HR05", "Điều phối viên", "VNĐ/người/buổi", 1500000, "Nhân sự & dịch vụ bổ sung"},
    {"HR06", "Ca sĩ biểu diễn", "VNĐ/người/buổi", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR07", "Ban nhạc", "VNĐ/buổi", 10000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR08", "Vũ đoàn", "VNĐ/buổi", 8000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR09", "Phiên dịch viên", "VNĐ/người/buổi", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"HR10", "Kỹ thuật viên sân khấu", "VNĐ/người/buổi", 1000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT01", "Bánh cưới / bánh sự kiện", "VNĐ/chiếc", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT02", "Bánh sinh nhật cao cấp", "VNĐ/chiếc", 800000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT03", "Thiệp mời", "VNĐ/thiệp", 15000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT04", "Quà tặng khách mời", "VNĐ/phần", 100000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT05", "Backdrop chụp ảnh", "VNĐ/gói", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT06", "Màn hình LED bổ sung", "VNĐ/gói", 10000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT07", "Livestream sự kiện", "VNĐ/gói", 5000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT08", "Pháo giấy sân khấu", "VNĐ/lần", 2000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT09", "Xe đưa đón", "VNĐ/xe/chuyến", 3000000, "Nhân sự & dịch vụ bổ sung"},
    {"EXT10", "In ảnh lấy liền (300 ảnh)", "VNĐ/gói", 6500000, "Nhân sự & dịch vụ bổ sung"},
};

struct SeedHall {
    const char* name;
    int capacity;
    int64_t rentalPrice;
};

// Sanh theo dac ta 2.6 (suc chua lay muc toi da tham khao).
const SeedHall kSeedHalls[] = {
    {"Sảnh Mini", 50, 5000000},
    {"Sảnh Small", 100, 8000000},
    {"Sảnh Standard", 200, 12000000},
    {"Sảnh Large", 300, 18000000},
    {"Sảnh Grand", 500, 30000000},
    {"Sảnh Ballroom", 800, 50000000},
};

// Nap bo sung cac muc con thieu (theo ten sanh / ma dich vu), khong xoa du lieu hien co.
inline void seedCatalog(Database& db) {
    {
        int maxNo = 0;
        for (const auto& x : db.halls) {
            if (x.id().rfind("HAL-", 0) == 0) {
                try {
                    maxNo = std::max(maxNo, std::stoi(x.id().substr(4)));
                } catch (...) {
                }
            }
        }
        for (const auto& h : kSeedHalls) {
            bool exists = false;
            for (const auto& x : db.halls) {
                if (x.name() == h.name) { exists = true; break; }
            }
            if (exists) continue;
            std::ostringstream id;
            id << "HAL-" << std::setfill('0') << std::setw(4) << (++maxNo);
            db.halls.push_back(Hall(id.str(), h.name, h.capacity, h.rentalPrice, HallStatus::KhaDung));
        }
    }
    {
        int maxNo = 0;
        for (const auto& x : db.services) {
            if (x.id().rfind("SVC-", 0) == 0) {
                try {
                    maxNo = std::max(maxNo, std::stoi(x.id().substr(4)));
                } catch (...) {
                }
            }
        }
        for (const auto& s : kSeedServices) {
            bool exists = false;
            for (const auto& x : db.services) {
                if (x.code() == s.code) { exists = true; break; }
            }
            if (exists) continue;
            std::ostringstream id;
            id << "SVC-" << std::setfill('0') << std::setw(4) << (++maxNo);
            db.services.push_back(ServiceItem(id.str(), s.code, s.name, s.unit, s.price, s.category, true));
        }
    }
}

}  // namespace tdp
