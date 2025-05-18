#include "Tool.h"
#include "Material.h"
#include "Blueprint.h"
#include "Project.h"
#include "ProjectManager.h"
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    auto welder = std::make_shared<Tool>("Welder-01", ToolType::WELDER);
    auto assembler = std::make_shared<Tool>("Assembler-02", ToolType::ASSEMBLER);
    auto painter = std::make_shared<Tool>("Painter-03", ToolType::PAINTER);
    auto printer3D = std::make_shared<Tool>("Printer3D-04", ToolType::PRINTER_3D);

    auto metal = std::make_shared<Material>("Metal", MaterialType::METAL, 100.0);
    auto plastic = std::make_shared<Material>("Plastic", MaterialType::PLASTIC, 50.0);
    auto paint = std::make_shared<Material>("Paint", MaterialType::LIQUID, 20.0);

    auto projectLogic = [&](Project& project) {
        std::cout << "[ExecutionLogic] " << project.getName() << " in progress...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    Welding and assembling...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    Painting final product...\n";
        if (project.getName() == "Project-Failable") {
            std::cerr << "[ExecutionLogic] Error: Unexpected glitch encountered.\n";
            project.setStatus(ProjectStatus::FAILED);
            return;
        }
    };

    auto printingLogic = [&](Project& project) {
        std::cout << "[ExecutionLogic] " << project.getName() << " in progress...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "    Configuring 3D Printer...\n";
        if (plastic->getQuantity() < 10.0) {
            std::cerr << "[ExecutionLogic] Error: Insufficient plastic for 3D printing.\n";
            project.setStatus(ProjectStatus::FAILED);
            return;
        }
        std::cout << "    Printing in progress...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "    3D printing completed.\n";
    };

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

    Project project1("Project-01", blueprint1, 1);
    Project projectFailable("Project-Failable", blueprintFailable, 2);
    Project project2("Project-02", blueprint2, 5);

    ProjectManager manager;

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

    manager.addProject(project1);
    manager.addProject(projectFailable);
    manager.addProject(project2);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
