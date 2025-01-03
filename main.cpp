#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

// Forward declarations
class Tool;
class Material;
class Blueprint;
class Project;
class ProjectManager;

// --- Enums ---

// Enum for different tool types
enum class ToolType {
    NONE,
    WELDER,
    ASSEMBLER,
    PAINTER,
    SCANNER,
    CALIBRATOR,
    CUTTER,
    PRINTER_3D,
    // ... Add more as needed
};

// Enum for different material types
enum class MaterialType {
    NONE,
    METAL,
    PLASTIC,
    COMPOSITE,
    LIQUID,
    GAS,
    ELECTRONIC,
    // ... Add more as needed
};

// Enum for project status
enum class ProjectStatus {
    IN_QUEUE,
    ACQUIRING_RESOURCES,
    IN_PROGRESS,
    PAUSED,
    COMPLETED,
    FAILED,
};

// --- Interfaces ---

// Interface for adaptable tools
class IAdaptable {
public:
    virtual void adapt(const std::map<std::string, std::string>& parameters) = 0;
    virtual ~IAdaptable() = default;
};

// Interface for a generic resource
class IResource {
public:
    virtual std::string getName() const = 0;
    virtual ~IResource() = default;
};

// --- Classes ---

// Class representing a generic tool
class Tool : public IResource, public IAdaptable {
public:
    Tool(const std::string& name, ToolType type) 
        : name_(name), type_(type), operational_(true) {}

    std::string getName() const override { return name_; }
    ToolType getType() const { return type_; }
    bool isOperational() const { return operational_; }
    void breakdown() { operational_ = false; }
    
    void adapt(const std::map<std::string, std::string>& parameters) override {
        std::cout << "[Tool::adapt] Adapting tool " << name_ << "...\n";
        for (const auto& pair : parameters) {
             std::cout << "    Setting parameter: " << pair.first 
                       << " to value: " << pair.second << "\n";
        }   
        std::cout << "[Tool::adapt] Tool " << name_ << " adapted.\n";
    }

private:
    std::string name_;
    ToolType type_;
    bool operational_;
};

// Class representing a generic material
class Material : public IResource {
public:
    Material(const std::string& name, MaterialType type, double quantity) 
        : name_(name), type_(type), quantity_(quantity) {}

    std::string getName() const override { return name_; }
    MaterialType getType() const { return type_; }
    double getQuantity() const { return quantity_; }
    
    void consume(double amount) {
        if (quantity_ >= amount) {
            quantity_ -= amount;
        } else {
            throw std::runtime_error("Not enough material available.");
        }
    }

    void replenish(double amount) {
        quantity_ += amount;
    }

private:
    std::string name_;
    MaterialType type_;
    double quantity_;
};

// Class representing a blueprint
class Blueprint : public IResource {
public:
    Blueprint(const std::string& name,
              const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& requiredTools,
              const std::vector<std::pair<std::shared_ptr<Material>, double>>& requiredMaterials,
              const std::function<void(Project&)>& executionLogic)
        : name_(name),
          requiredTools_(requiredTools),
          requiredMaterials_(requiredMaterials),
          executionLogic_(executionLogic) {}

    std::string getName() const override { return name_; }
    const std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>& getRequiredTools() const {
        return requiredTools_;
    }
    const std::vector<std::pair<std::shared_ptr<Material>, double>>& getRequiredMaterials() const {
        return requiredMaterials_;
    }
    const std::function<void(Project&)>& getExecutionLogic() const {
        return executionLogic_;
    }

private:
    std::string name_;
    std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>> requiredTools_;
    std::vector<std::pair<std::shared_ptr<Material>, double>> requiredMaterials_;
    std::function<void(Project&)> executionLogic_;
};

// Class representing a project
class Project {
public:
    Project(const std::string& name, 
            const std::shared_ptr<Blueprint>& blueprint,
            int priority = 0)
        : name_(name),
          blueprint_(blueprint),
          status_(ProjectStatus::IN_QUEUE),
          priority_(priority) {}

    std::string getName() const { return name_; }
    ProjectStatus getStatus() const { return status_; }
    void setStatus(ProjectStatus status) { status_ = status; }

    int getPriority() const { return priority_; }
    const std::shared_ptr<Blueprint>& getBlueprint() const { return blueprint_; }
    
    void execute() {
        if (!blueprint_) {
            std::cerr << "[Project::execute] Error: Project " 
                      << name_ << " does not have a valid blueprint.\n";
            status_ = ProjectStatus::FAILED;
            return;
        }

        std::cout << "[Project::execute] Executing project: " << name_ << "\n";
        status_ = ProjectStatus::ACQUIRING_RESOURCES;

        // Acquire necessary tools and adapt them
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

        // Acquire necessary materials
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

        // Execute project logic
        blueprint_->getExecutionLogic()(*this);

        if (status_ != ProjectStatus::FAILED) {
            status_ = ProjectStatus::COMPLETED;
            std::cout << "[Project::execute] Project " << name_ 
                      << " completed successfully.\n";
        }
    }

private:
    std::string name_;
    std::shared_ptr<Blueprint> blueprint_;
    ProjectStatus status_;
    int priority_;
    std::vector<std::shared_ptr<Tool>> acquiredTools_;
    std::vector<std::shared_ptr<Material>> acquiredMaterials_;
};

// --- Project Manager with concurrency and event system ---

// Simple event system for demonstration
enum class ManagerEvent {
    PROJECT_ENQUEUED,
    PROJECT_STARTED,
    PROJECT_COMPLETED,
    PROJECT_FAILED
};

class ProjectManager {
public:
    ProjectManager() : stop_(false) {
        // The worker thread continuously checks for available projects
        worker_ = std::thread(&ProjectManager::processProjects, this);
    }

    ~ProjectManager() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void addProject(const Project& project) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            projectQueue_.push(project);
        }
        notifyListeners(ManagerEvent::PROJECT_ENQUEUED);
        cv_.notify_one();
    }

    void addListener(std::function<void(ManagerEvent, const Project&)> listener) {
        listeners_.push_back(listener);
    }

private:
    // A priority-comparator to ensure higher priority is processed first
    struct ProjectCompare {
        bool operator()(const Project& a, const Project& b) {
            return a.getPriority() < b.getPriority();
        }
    };

    void processProjects() {
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

            // Notify that we're starting a project
            notifyListeners(ManagerEvent::PROJECT_STARTED, currentProject);

            currentProject.execute();

            // Check the final status of the project for notifications
            if (currentProject.getStatus() == ProjectStatus::COMPLETED) {
                notifyListeners(ManagerEvent::PROJECT_COMPLETED, currentProject);
            } else if (currentProject.getStatus() == ProjectStatus::FAILED) {
                notifyListeners(ManagerEvent::PROJECT_FAILED, currentProject);
            }
        }
    }

    void notifyListeners(ManagerEvent event, const Project& p = Project("None", nullptr)) {
        for (auto& listener : listeners_) {
            listener(event, p);
        }
    }

    std::priority_queue<Project, std::vector<Project>, ProjectCompare> projectQueue_;
    std::vector<std::function<void(ManagerEvent, const Project&)>> listeners_;

    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};

// --- Main Function ---

int main() {
    // Create some tools
    auto welder = std::make_shared<Tool>("Welder-01", ToolType::WELDER);
    auto assembler = std::make_shared<Tool>("Assembler-02", ToolType::ASSEMBLER);
    auto painter = std::make_shared<Tool>("Painter-03", ToolType::PAINTER);
    auto printer3D = std::make_shared<Tool>("3D-Printer-04", ToolType::PRINTER_3D);

    // Create some materials
    auto metal = std::make_shared<Material>("Metal Sheet", MaterialType::METAL, 100.0);
    auto plastic = std::make_shared<Material>("Plastic Roll", MaterialType::PLASTIC, 50.0);
    auto paint = std::make_shared<Material>("Blue Paint", MaterialType::LIQUID, 20.0);

    // Define the execution logic for a welding/assembly project
    auto projectLogic = [&](Project& project) {
        std::cout << "[ExecutionLogic] " << project.getName() << " in progress...\n";

        // Simulate some steps
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    Welding and assembling...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    Painting final product...\n";

        // Random check: simulate a potential error
        if (project.getName() == "Project-Failable") {
            std::cerr << "[ExecutionLogic] Error: Unexpected glitch encountered.\n";
            project.setStatus(ProjectStatus::FAILED);
            return;
        }
    };

    // Define the execution logic for a 3D printing project
    auto printingLogic = [&](Project& project) {
        std::cout << "[ExecutionLogic] " << project.getName() << " in progress...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "    Configuring 3D Printer...\n";

        // Simulate checking materials
        if (plastic->getQuantity() < 10.0) {
            std::cerr << "[ExecutionLogic] Error: Insufficient plastic for 3D printing.\n";
            project.setStatus(ProjectStatus::FAILED);
            return;
        }

        std::cout << "    Printing in progress...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    3D printing completed.\n";
    };

    // Create blueprints
    // Blueprint 1: needs welder, assembler, painter, plus some metal, plastic, paint
    auto blueprint1 = std::make_shared<Blueprint>(
        "Blueprint-01",
        std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>{
            { welder,    {{"power", "high"}, {"mode", "spot"}} },
            { assembler, {{"speed", "medium"}, {"precision", "high"}} },
            { painter,   {{"color", "blue"}, {"finish", "gloss"}} }
        },
        std::vector<std::pair<std::shared_ptr<Material>, double>>{
            { metal,   20.0 },
            { plastic, 10.0 },
            { paint,   5.0 }
        },
        projectLogic
    );

    // Blueprint that can fail
    auto blueprintFailable = std::make_shared<Blueprint>(
        "Blueprint-Failable",
        std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>{
            { welder,    {{"power", "high"}, {"mode", "spot"}} },
            { assembler, {{"speed", "medium"}, {"precision", "high"}} },
            { painter,   {{"color", "blue"}, {"finish", "gloss"}} }
        },
        std::vector<std::pair<std::shared_ptr<Material>, double>>{
            { metal,   20.0 },
            { plastic, 10.0 },
            { paint,   5.0 }
        },
        projectLogic
    );

    // Blueprint 2: needs only the 3D printer, and some plastic
    auto blueprint2 = std::make_shared<Blueprint>(
        "Blueprint-02",
        std::vector<std::pair<std::shared_ptr<Tool>, std::map<std::string, std::string>>>{
            { printer3D, {{"resolution", "high"}, {"material", "PLA"}} }
        },
        std::vector<std::pair<std::shared_ptr<Material>, double>>{
            { plastic, 15.0 }
        },
        printingLogic
    );

    // Create projects with various priorities (higher means processed first)
    Project project1("Project-01", blueprint1, 1);
    Project projectFailable("Project-Failable", blueprintFailable, 2);
    Project project2("Project-02", blueprint2, 5);

    // Create a project manager
    ProjectManager manager;

    // Add a basic event listener
    manager.addListener(
        [&](ManagerEvent event, const Project& p) {
            switch (event) {
                case ManagerEvent::PROJECT_ENQUEUED:
                    std::cout << "[Listener] Project " << p.getName() << " was enqueued.\n";
                    break;
                case ManagerEvent::PROJECT_STARTED:
                    std::cout << "[Listener] Project " << p.getName() << " started.\n";
                    break;
                case ManagerEvent::PROJECT_COMPLETED:
                    std::cout << "[Listener] Project " << p.getName() << " completed.\n";
                    break;
                case ManagerEvent::PROJECT_FAILED:
                    std::cout << "[Listener] Project " << p.getName() << " failed.\n";
                    break;
            }
        }
    );

    // Enqueue projects
    manager.addProject(project1);
    manager.addProject(projectFailable);
    manager.addProject(project2);

    // Let the program run a bit so the threads can process the projects
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // End of main, manager destructor will wait for worker thread to finish
    return 0;
}
