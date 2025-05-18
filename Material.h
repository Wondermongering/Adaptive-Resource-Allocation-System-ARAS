#pragma once
#include "Types.h"
#include <string>

class Material : public IResource {
public:
    Material(const std::string& name, MaterialType type, double quantity);
    std::string getName() const override;
    MaterialType getType() const;
    double getQuantity() const;
    void consume(double amount);
    void replenish(double amount);
    void release(double amount);
private:
    std::string name_;
    MaterialType type_;
    double quantity_;
};
