#pragma once
#include <exception>
#include <string>
#include <utility>

class GameException : public std::exception {
private:
    std::string message;

public:
    explicit GameException(std::string msg) : message(std::move(msg)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

class InsufficientFundsException : public GameException {
public:
    InsufficientFundsException(double required, double available)
        : GameException("Fonduri insuficiente! Necesar: " + std::to_string(static_cast<long long>(required)) +
                        "$, Disponibil: " + std::to_string(static_cast<long long>(available)) + "$") {}
};

class InsufficientGoldException : public GameException {
public:
    InsufficientGoldException(int required, int available)
        : GameException("Gold insuficient! Necesar: " + std::to_string(required) +
                        ", Disponibil: " + std::to_string(available)) {}
};

class InvalidBusinessIndexException : public GameException {
public:
    explicit InvalidBusinessIndexException(int index)
        : GameException("Index business invalid: " + std::to_string(index)) {}
};

class UnknownUpgradeException : public GameException {
public:
    explicit UnknownUpgradeException(int id)
        : GameException("Upgrade-ul cu id-ul " + std::to_string(id) + " nu exista in catalog!") {}
};

class UpgradeAlreadyOwnedException : public GameException {
public:
    explicit UpgradeAlreadyOwnedException(const std::string& label)
        : GameException("Upgrade-ul '" + label + "' este deja cumparat!") {}
};

class BusinessAlreadyOwnedException : public GameException {
public:
    explicit BusinessAlreadyOwnedException(const std::string& name)
        : GameException("Business-ul '" + name + "' este deja detinut!") {}
};

class BusinessNotOwnedException : public GameException {
public:
    explicit BusinessNotOwnedException(const std::string& name)
        : GameException("Trebuie sa detii business-ul '" + name + "' pentru a efectua aceasta actiune!") {}
};

class PrestigeLockedException : public GameException {
public:
    PrestigeLockedException(double requiredMoney, int requiredLevel)
        : GameException("Prestige blocat: ai nevoie de " +
                        std::to_string(static_cast<long long>(requiredMoney)) +
                        "$ si ultima afacere la nivelul " + std::to_string(requiredLevel) + ".") {}
};

class InvalidBusinessDataException : public GameException {
public:
    InvalidBusinessDataException(const std::string& name, const std::string& reason)
        : GameException("Date invalide pentru afacerea '" + name + "': " + reason) {}
};

class DataFileException : public GameException {
public:
    DataFileException(const std::string& filename, const std::string& reason)
        : GameException("Problema cu fisierul '" + filename + "': " + reason) {}

    DataFileException(const std::string& filename, size_t lineNumber, const std::string& reason)
        : GameException("Problema cu fisierul '" + filename + "', linia " +
                        std::to_string(lineNumber) + ": " + reason) {}
};