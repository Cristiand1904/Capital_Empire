#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Achievement.h"
#include "Business.h"
#include "GoldUpgrade.h"

class DataLoader {
private:
    class Record {
    private:
        std::vector<std::string> fields;
        size_t lineNumber;
        std::string source;

    public:
        Record(std::vector<std::string> fields, size_t lineNumber, std::string source);

        [[nodiscard]] size_t size() const;
        [[nodiscard]] size_t line() const;
        [[nodiscard]] const std::string& file() const;
        [[nodiscard]] std::string text(size_t index) const;
        [[nodiscard]] int integer(size_t index) const;
        [[nodiscard]] double number(size_t index) const;
        [[noreturn]] void fail(const std::string& reason) const;
    };

    static std::vector<Record> readRecords(const std::string& filename, size_t minFields);
    static std::vector<std::string> splitLine(const std::string& line);
    static std::string trim(const std::string& text);

    static std::unique_ptr<Business> makeBusiness(const Record& record);

public:
    static std::vector<std::unique_ptr<Business>> loadBusinesses(const std::string& filename);
    static std::vector<GoldUpgrade> loadGoldUpgrades(const std::string& filename);
    static std::vector<Achievement> loadAchievements(const std::string& filename);
};
