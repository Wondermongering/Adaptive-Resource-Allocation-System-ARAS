#include "ProjectManager.h"
#include <iostream>

ProjectManager::ProjectManager() : stop_(false) {
    worker_ = std::thread(&ProjectManager::processProjects, this);
}

ProjectManager::~ProjectManager() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_all();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void ProjectManager::addProject(const Project& project) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        projectQueue_.push(project);
    }
    notifyListeners(ManagerEvent::PROJECT_ENQUEUED, &project);
    cv_.notify_one();
}

void ProjectManager::addListener(std::function<void(ManagerEvent, const Project&)> listener) {
    listeners_.push_back(listener);
}

void ProjectManager::processProjects() {
    while (true) {
        Project currentProject("", nullptr);
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] {
                return stop_ || !projectQueue_.empty();
            });

            if (stop_) {
                break;
            }

            currentProject = projectQueue_.top();
            projectQueue_.pop();
        }

        notifyListeners(ManagerEvent::PROJECT_STARTED, &currentProject);

        currentProject.execute();

        if (currentProject.getStatus() == ProjectStatus::COMPLETED) {
            notifyListeners(ManagerEvent::PROJECT_COMPLETED, &currentProject);
        } else if (currentProject.getStatus() == ProjectStatus::FAILED) {
            notifyListeners(ManagerEvent::PROJECT_FAILED, &currentProject);
        }
    }
}

void ProjectManager::notifyListeners(ManagerEvent event, const Project* p) {
    if (!p) {
        return;
    }
    for (auto& listener : listeners_) {
        listener(event, *p);
    }
}
