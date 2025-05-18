#pragma once
#include <string>
#include <map>

enum class ToolType {
    NONE,
    WELDER,
    ASSEMBLER,
    PAINTER,
    SCANNER,
    CALIBRATOR,
    CUTTER,
    PRINTER_3D,
};

enum class MaterialType {
    NONE,
    METAL,
    PLASTIC,
    COMPOSITE,
    LIQUID,
    GAS,
    ELECTRONIC,
};

enum class ProjectStatus {
    IN_QUEUE,
    ACQUIRING_RESOURCES,
    IN_PROGRESS,
    PAUSED,
    COMPLETED,
    FAILED,
};

enum class ManagerEvent {
    PROJECT_ENQUEUED,
    PROJECT_STARTED,
    PROJECT_COMPLETED,
    PROJECT_FAILED
};

class IAdaptable {
public:
    virtual void adapt(const std::map<std::string, std::string>& parameters) = 0;
    virtual ~IAdaptable() = default;
};

class IResource {
public:
    virtual std::string getName() const = 0;
    virtual ~IResource() = default;
};
