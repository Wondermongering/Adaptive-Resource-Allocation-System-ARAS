#include "Project.h"
#include "Blueprint.h"
#include "Tool.h"
#include "Material.h"
#include <iostream>
#include <stdexcept>

Project::Project(const std::string& name,
                 const std::shared_ptr<Blueprint>& blueprint,
                 int priority)
    : name_(name),
      blueprint_(blueprint),
      status_(ProjectStatus::IN_QUEUE),
      priority_(priority) {}

std::string Project::getName() const { return name_; }
ProjectStatus Project::getStatus() const { return status_; }
void Project::setStatus(ProjectStatus status) { status_ = status; }
int Project::getPriority() const { return priority_; }
const std::shared_ptr<Blueprint>& Project::getBlueprint() const { return blueprint_; }

void Project::execute() {
    if (!blueprint_) {
        std::cerr << "[Project::execute] Error: Project "
                  << name_ << " does not have a valid blueprint.\n";
        status_ = ProjectStatus::FAILED;
        return;
    }

    std::cout << "[Project::execute] Executing project: " << name_ << "\n";
    status_ = ProjectStatus::ACQUIRING_RESOURCES;

    for (const auto& toolPair : blueprint_->getRequiredTools()) {
        if (!toolPair.first) {
            std::cerr << "[Project::execute] Error: Invalid tool in blueprint for project "
                      << name_ << "\n";
            status_ = ProjectStatus::FAILED;
            return;
        }

        std::cout << "    Acquiring tool: " << toolPair.first->getName() << "\n";
        if (!toolPair.first->isOperational()) {
            std::cerr << "[Project::execute] Error: Tool "
                      << toolPair.first->getName() << " is not operational.\n";
            status_ = ProjectStatus::FAILED;
            return;
        }

        toolPair.first->adapt(toolPair.second);
        acquiredTools_.push_back(toolPair.first);
    }

    for (const auto& materialPair : blueprint_->getRequiredMaterials()) {
        if (!materialPair.first) {
            std::cerr << "[Project::execute] Error: Invalid material in blueprint for project "
                      << name_ << "\n";
            status_ = ProjectStatus::FAILED;
            return;
        }

        std::cout << "    Acquiring material: " << materialPair.first->getName()
                  << " (Quantity: " << materialPair.second << ")\n";
        try {
            materialPair.first->consume(materialPair.second);
            acquiredMaterials_.push_back(materialPair.first);
        } catch (const std::runtime_error& error) {
            std::cerr << "[Project::execute] Error: "
                      << error.what()
                      << " -> Project " << name_ << " failed.\n";
            status_ = ProjectStatus::FAILED;
            return;
        }
    }

    status_ = ProjectStatus::IN_PROGRESS;
    blueprint_->getExecutionLogic()(*this);

    if (status_ != ProjectStatus::FAILED) {
        status_ = ProjectStatus::COMPLETED;
        std::cout << "[Project::execute] Project " << name_
                  << " completed successfully.\n";
    }
}
