#pragma once
#include "Types.h"
#include <string>
#include <map>
#include <iostream>

class Tool : public IResource, public IAdaptable {
public:
    Tool(const std::string& name, ToolType type);
    std::string getName() const override;
    ToolType getType() const;
    bool isOperational() const;
    void breakdown();
    void adapt(const std::map<std::string, std::string>& parameters) override;
private:
    std::string name_;
    ToolType type_;
    bool operational_;
};
