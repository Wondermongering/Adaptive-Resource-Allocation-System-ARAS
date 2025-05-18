#pragma once
#include "Types.h"
#include <string>
#include <memory>
#include <vector>

class Blueprint;
class Tool;
class Material;

class Project {
public:
    Project(const std::string& name,
            const std::shared_ptr<Blueprint>& blueprint,
            int priority = 0);

    std::string getName() const;
    ProjectStatus getStatus() const;
    void setStatus(ProjectStatus status);
    int getPriority() const;
    const std::shared_ptr<Blueprint>& getBlueprint() const;
    void execute();

private:
    std::string name_;
    std::shared_ptr<Blueprint> blueprint_;
    ProjectStatus status_;
    int priority_;
    std::vector<std::shared_ptr<Tool>> acquiredTools_;
    std::vector<std::pair<std::shared_ptr<Material>, double>> acquiredMaterials_;

    void releaseResources(bool success);
};
