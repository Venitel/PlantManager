#include "Setting.h"
#include "Database.h"
#include "Utils.h"

std::string Setting::getTabName() const
{
    return Database::getTableName(Field::DataType::Setting);
}

std::vector<Field> Setting::getFields()
{
    const std::string value = std::to_string(value_);
    const std::string orderNum = std::to_string(orderNum_);

    return 
    {//   ColNam        Label         Var           Length  InputType                    DataType                  Setter                                     onEdit
        { "name",       "Name    : ", name_,        40,     Field::InputType::ReadOnly,  Field::DataType::Text,    [this](std::string v){setName(v);},        {} },
        { "notes",      "Notes   : ", notes_,       120,    Field::InputType::ReadOnly,  Field::DataType::Text,    [this](std::string v){setNotes(v);},       {} },
        { "value",      "Value   : ", value,        3,      Field::InputType::Mandatory, Field::DataType::Number,  [this](std::string v){setValue(v);},       [this](){updateRecord(); updateCache();} },
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoDisplay, Field::DataType::Number,  [this](std::string v){setOrderNum(v);},    {} }
    };
}

void Setting::setNotes(const std::string& notes)
{
    notes_ = notes;
}

void Setting::setValue(const int value)
{
    value_ = value;
}

void Setting::setValue(const std::string& value)
{
    if(Utils::isNumberLog(value))
    {
        setValue(stoi(value));
    }
}

int Setting::getValue() const
{
    return value_;
}

void Setting::initializeSettings()
{
    insertDbSetting("Action required", "Indicate next watering / feeding when less than X days", 1);
    insertDbSetting("Postpone", "How many days does a single postpone (watering / feeding) delay", 1);
}

void Setting::insertDbSetting(const std::string& name, const std::string& description, const int defaultValue)
{
    std::string query = 
        "INSERT INTO settings (name, notes, value, orderNum) "
        "SELECT " + Database::sqlString(name) + ", " + Database::sqlString(description) + ", " +std::to_string(defaultValue) + ", (SELECT MIN(IFNULL(MAX(orderNum), 0)+1, 999999999) FROM settings) "
        "WHERE NOT EXISTS ( "
            "SELECT 1 FROM settings WHERE name = " + Database::sqlString(name) +
        ")";
    Database::getInstance().exec(query);
}

void Setting::updateCache()
{
    CommonCache::getSettings()[getName()] = getValue();
}
