#include "Material.h"
#include <stdexcept>
#include <iostream>

Material::Material(const std::string& name, MaterialType type, double quantity)
    : name_(name), type_(type), quantity_(quantity) {}

std::string Material::getName() const { return name_; }
MaterialType Material::getType() const { return type_; }
double Material::getQuantity() const { return quantity_; }

void Material::consume(double amount) {
    if (quantity_ >= amount) {
        quantity_ -= amount;
    } else {
        throw std::runtime_error("Not enough material available.");
    }
}

void Material::replenish(double amount) {
    quantity_ += amount;
}

void Material::release(double amount) {
    std::cout << "[Material::release] Replenishing " << amount
              << " units of " << name_ << "...\n";
    replenish(amount);
}
