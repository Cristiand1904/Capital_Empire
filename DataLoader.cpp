#include "DataLoader.h"
#include "Exceptions.h"
#include "FranchiseBusiness.h"
#include "PremiumBusiness.h"
#include "SeasonalBusiness.h"
#include "StandardBusiness.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

DataLoader::Record::Record(std::vector<std::string> fields, size_t lineNumber, std::string source)
    : fields(std::move(fields)), lineNumber(lineNumber), source(std::move(source)) {}

size_t DataLoader::Record::size() const {
    return fields.size();
}

size_t DataLoader::Record::line() const {
    return lineNumber;
}

const std::string& DataLoader::Record::file() const {
    return source;
}

std::string DataLoader::Record::text(size_t index) const {
    return index < fields.size() ? fields[index] : "";
}

void DataLoader::Record::fail(const std::string& reason) const {
    throw DataFileException(source, lineNumber, reason);
}

int DataLoader::Record::integer(size_t index) const {
    const std::string raw = text(index);
    try {
        size_t consumed = 0;
        const int value = std::stoi(raw, &consumed);
        if (consumed != raw.size()) {
            fail("numar intreg invalid: '" + raw + "'");
        }
        return value;
    } catch (const std::invalid_argument&) {
        fail("numar intreg invalid: '" + raw + "'");
    } catch (const std::out_of_range&) {
        fail("numar intreg prea mare: '" + raw + "'");
    }
}

double DataLoader::Record::number(size_t index) const {
    const std::string raw = text(index);
    try {
        size_t consumed = 0;
        const double value = std::stod(raw, &consumed);
        if (consumed != raw.size()) {
            fail("numar zecimal invalid: '" + raw + "'");
        }
        return value;
    } catch (const std::invalid_argument&) {
        fail("numar zecimal invalid: '" + raw + "'");
    } catch (const std::out_of_range&) {
        fail("numar zecimal prea mare: '" + raw + "'");
    }
}

std::string DataLoader::trim(const std::string& text) {
    const size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::vector<std::string> DataLoader::splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream stream(line);
    std::string cell;
    while (std::getline(stream, cell, ';')) {
        fields.push_back(trim(cell));
    }
    return fields;
}

std::vector<DataLoader::Record> DataLoader::readRecords(const std::string& filename, size_t minFields) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw DataFileException(filename, "nu poate fi deschis pentru citire");
    }

    std::vector<Record> records;
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty()) continue;

        Record record(splitLine(line), lineNumber, filename);
        if (record.size() < minFields) {
            record.fail("sunt necesare cel putin " + std::to_string(minFields) + " campuri");
        }
        records.push_back(std::move(record));
    }

    if (records.empty()) {
        throw DataFileException(filename, "fisierul nu contine nicio inregistrare");
    }
    return records;
}

std::unique_ptr<Business> DataLoader::makeBusiness(const Record& record) {
    const std::string kind = record.text(0);
    std::string name = record.text(1);
    const double profit = record.number(2);
    const double upgradeCost = record.number(3);
    const double purchaseCost = record.number(4);
    const double productionTime = record.number(5);
    const double managerCost = record.number(6);

    if (kind == "STANDARD") {
        const auto type = StandardBusiness::typeFromString(record.text(7));
        if (!type.has_value()) {
            record.fail("tip standard necunoscut: '" + record.text(7) + "'");
        }
        return std::make_unique<StandardBusiness>(std::move(name), *type, profit, upgradeCost,
                                                  purchaseCost, productionTime, managerCost);
    }

    if (kind == "SEASONAL") {
        const auto season = SeasonalBusiness::seasonFromString(record.text(7));
        if (!season.has_value()) {
            record.fail("sezon necunoscut: '" + record.text(7) + "'");
        }
        return std::make_unique<SeasonalBusiness>(std::move(name), profit, upgradeCost, purchaseCost,
                                                  productionTime, managerCost, *season);
    }

    if (kind == "PREMIUM") {
        return std::make_unique<PremiumBusiness>(std::move(name), profit, upgradeCost, purchaseCost,
                                                 productionTime, managerCost, record.number(7));
    }

    if (kind == "FRANCHISE") {
        if (record.size() < 9) {
            record.fail("o afacere de tip franciza are nevoie de 9 campuri");
        }
        return std::make_unique<FranchiseBusiness>(std::move(name), profit, upgradeCost, purchaseCost,
                                                   productionTime, managerCost,
                                                   record.integer(7), record.number(8));
    }

    record.fail("tip de afacere necunoscut: '" + kind + "'");
}

std::vector<std::unique_ptr<Business>> DataLoader::loadBusinesses(const std::string& filename) {
    const std::vector<Record> records = readRecords(filename, 8);

    std::vector<std::unique_ptr<Business>> businesses;
    businesses.reserve(records.size());
    for (const Record& record : records) {
        businesses.push_back(makeBusiness(record));
    }
    return businesses;
}

std::vector<GoldUpgrade> DataLoader::loadGoldUpgrades(const std::string& filename) {
    const std::vector<Record> records = readRecords(filename, 7);

    std::vector<GoldUpgrade> upgrades;
    upgrades.reserve(records.size());
    for (const Record& record : records) {
        const int id = record.integer(0);
        if (id <= 0) {
            record.fail("id-ul unui upgrade trebuie sa fie pozitiv");
        }

        const auto effect = GoldUpgrade::effectFromString(record.text(2));
        if (!effect.has_value()) {
            record.fail("efect necunoscut: '" + record.text(2) + "'");
        }

        upgrades.emplace_back(id, record.integer(1), *effect, record.number(3),
                              record.text(4), record.text(5), record.text(6));
    }
    return upgrades;
}

std::vector<Achievement> DataLoader::loadAchievements(const std::string& filename) {
    const std::vector<Record> records = readRecords(filename, 5);

    std::vector<Achievement> achievements;
    achievements.reserve(records.size());
    for (const Record& record : records) {
        const auto type = Achievement::typeFromString(record.text(0));
        if (!type.has_value()) {
            record.fail("tip de realizare necunoscut: '" + record.text(0) + "'");
        }
        achievements.emplace_back(record.text(3), record.text(4), record.number(2),
                                  *type, record.number(1), record.text(5));
    }
    return achievements;
}
