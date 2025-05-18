#include "Blueprint.h"
#include "Project.h"
#include "Tool.h"
#include "Material.h"

Blueprint::Blueprint(const std::string& name,
                     const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& requiredTools,
                     const std::vector<std::pair<std::shared_ptr<Material>, double>>& requiredMaterials,
                     const std::function<void(Project&)>& executionLogic)
    : name_(name),
      requiredTools_(requiredTools),
      requiredMaterials_(requiredMaterials),
      executionLogic_(executionLogic) {}

std::string Blueprint::getName() const { return name_; }
const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& Blueprint::getRequiredTools() const {
    return requiredTools_;
}
const std::vector<std::pair<std::shared_ptr<Material>, double>>& Blueprint::getRequiredMaterials() const {
    return requiredMaterials_;
}
const std::function<void(Project&)>& Blueprint::getExecutionLogic() const {
    return executionLogic_;
}
