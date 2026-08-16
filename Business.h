#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Manager.h"
#include "Upgrade.h"

class Business {
protected:
    std::string name;
    double profitPerCycle;
    double initialProfit;
    double upgradeCost;
    double initialUpgradeCost;
    double purchaseCost;
    double managerBaseCost;
    double initialProductionTime;
    int level;
    bool owned;
    std::unique_ptr<Manager> manager;
    std::vector<Upgrade> upgrades;

    double productionTime;
    double currentTimer;
    bool isProducing;
    double sessionMultiplier;

    virtual void print(std::ostream& os) const;
    [[nodiscard]] double baseRevenue(double bonusMultiplier) const;

private:
    void initMilestones();
    [[nodiscard]] const Upgrade* nextMilestone() const;

public:
    Business(std::string name, double profit, double upgrade, double cost, double time, double mngCost);

    Business(const Business& other);
    Business& operator=(const Business& other);
    Business(Business&&) noexcept = default;
    Business& operator=(Business&&) noexcept = default;

    virtual ~Business() = default;

    [[nodiscard]] virtual std::unique_ptr<Business> clone() const = 0;
    [[nodiscard]] virtual double calculateRevenue(double bonusMultiplier) const = 0;
    [[nodiscard]] virtual std::string getStatusLabel() const;

    double update(double deltaTime, double bonusMultiplier);
    void startProduction();

    void display(std::ostream& os) const;

    void levelUp();
    void unlock();
    void hireManager();
    void upgradeManager();
    void reset();
    void restoreState(int savedLevel, bool savedOwned, bool savedManager, int managerLevel,
                      double savedProfit, double savedUpgradeCost);

    void setManagerHired(bool hired);
    void setSessionMultiplier(double multiplier);

    [[nodiscard]] bool isOwned() const;
    [[nodiscard]] bool hasManagerHired() const;
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] int getLevel() const;
    [[nodiscard]] double getProfitPerCycle() const;
    [[nodiscard]] double getSessionMultiplier() const;
    [[nodiscard]] double getUpgradeCost() const;
    [[nodiscard]] double getPurchaseCost() const;
    [[nodiscard]] double getManagerCost() const;
    [[nodiscard]] double getManagerUpgradeCost() const;

    [[nodiscard]] bool nextUpgradeIsMilestone() const;

    [[nodiscard]] double getProgress() const;
    [[nodiscard]] double getProductionTime() const;
    [[nodiscard]] bool isActive() const;

    [[nodiscard]] int getManagerLevel() const;

    friend std::ostream& operator<<(std::ostream& os, const Business& b);
};
