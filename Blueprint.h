#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

class Tool;
class Material;
class Project;

class Blueprint : public IResource {
public:
    Blueprint(const std::string& name,
              const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& requiredTools,
              const std::vector<std::pair<std::shared_ptr<Material>, double>>& requiredMaterials,
              const std::function<void(Project&)>& executionLogic);

    std::string getName() const override;
    const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& getRequiredTools() const;
    const std::vector<std::pair<std::shared_ptr<Material>, double>>& getRequiredMaterials() const;
    const std::function<void(Project&)>& getExecutionLogic() const;

private:
    std::string name_;
    std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>> requiredTools_;
    std::vector<std::pair<std::shared_ptr<Material>, double>> requiredMaterials_;
    std::function<void(Project&)> executionLogic_;
};
