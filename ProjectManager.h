#pragma once
#include "Types.h"
#include "Project.h"
#include <queue>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ProjectManager {
public:
    ProjectManager();
    ~ProjectManager();

    void addProject(const Project& project);
    void addListener(std::function<void(ManagerEvent, const Project&)> listener);

private:
    struct ProjectCompare {
        bool operator()(const Project& a, const Project& b) {
            return a.getPriority() < b.getPriority();
        }
    };

    void processProjects();
    void notifyListeners(ManagerEvent event, const Project* p = nullptr);

    std::priority_queue<Project, std::vector<Project>, ProjectCompare> projectQueue_;
    std::vector<std::function<void(ManagerEvent, const Project&)>> listeners_;

    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};
