#ifndef TDP_MODELS_SERVICEITEM_CPP
#define TDP_MODELS_SERVICEITEM_CPP

#include <cstdint>
#include "utils/text_fields.cpp"
#include <string>

using namespace std;

class ServiceItem {
public:
    ServiceItem() = default;

    ServiceItem(const string& id, const string& code, const string& name, const string& unit, int64_t unitPrice,
             const string& category, bool active)
        : id_(id),
          code_(code),
          name_(name),
          unit_(unit),
          category_(category),
          unitPrice_(unitPrice),
          active_(active) {}
    const string& id() const { return id_; }
    const string& code() const { return code_; }
    const string& name() const { return name_; }
    const string& unit() const { return unit_; }
    int64_t unitPrice() const { return unitPrice_; }
    const string& category() const { return category_; }
    bool active() const { return active_; }
    void setName(const string& value) { name_ = value; }
    void setUnit(const string& value) { unit_ = value; }
    void setUnitPrice(int64_t v) { unitPrice_ = v; }
    void setCategory(const string& value) { category_ = value; }
    void setActive(bool v) { active_ = v; }
    void setCode(const string& value) { code_ = value; }
    vector<string> toFields() const;
    static ServiceItem fromFields(const vector<string>& f);

private:
    string id_, code_, name_, unit_, category_;
    int64_t unitPrice_ = 0;
    bool active_ = true;
};

vector<string> ServiceItem::toFields() const {
    vector<string> f = {id_, code_, name_, unit_, to_string(unitPrice_), category_, (active_ ? "1" : "0")};
    return f;
}

ServiceItem ServiceItem::fromFields(const vector<string>& f) {
    requireFieldCount(f, 7);
    ServiceItem item;
    item.id_ = f.at(0);
    item.code_ = f.at(1);
    item.name_ = f.at(2);
    item.unit_ = f.at(3);
    item.unitPrice_ = textMoney(f.at(4));
    item.category_ = f.at(5);
    item.active_ = textBool(f.at(6));
    return item;
}

#endif
