#include "Tool.h"

Tool::Tool(const std::string& name, ToolType type)
    : name_(name), type_(type), operational_(true) {}

std::string Tool::getName() const { return name_; }
ToolType Tool::getType() const { return type_; }
bool Tool::isOperational() const { return operational_; }
void Tool::breakdown() { operational_ = false; }

void Tool::adapt(const std::map<std::string, std::string>& parameters) {
    std::cout << "[Tool::adapt] Adapting tool " << name_ << "...\n";
    for (const auto& pair : parameters) {
        std::cout << "    Setting parameter: " << pair.first
                  << " to value: " << pair.second << "\n";
    }
    std::cout << "[Tool::adapt] Tool " << name_ << " adapted.\n";
}
