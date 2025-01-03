# Adaptive-Resource-Allocation-System-ARAS-
ARAS is a ** is a sophisticated, concurrent project management system designed to simulate and manage complex projects with intricate resource allocation, task dependencies, and potential failure points
## Overview

**ARAS** is a sophisticated, concurrent project management system designed to simulate and manage complex projects with intricate resource allocation, task dependencies, and potential failure points. Built using modern C++, **ARAS** leverages advanced features like multithreading, synchronization primitives, and an event-driven architecture to provide a robust and insightful model of real-world project execution.

## Features

*   **Concurrent Project Execution:** Utilizes multithreading to run multiple projects simultaneously, maximizing resource utilization and simulating real-world scenarios.
*   **Priority-Based Scheduling:** Implements a priority queue to ensure that high-priority projects are executed first.
*   **Resource Management:**  Manages tools and materials efficiently, allocating them to projects based on blueprint requirements.
*   **Adaptable Tools:** Tools can be adapted with specific parameters, providing flexibility in project execution.
*   **Detailed Blueprints:** Blueprints define project requirements, including tools, materials, and execution logic.
*   **Error Handling:** Robust error handling mechanisms are in place to catch and manage potential issues like resource shortages, tool malfunctions, and unexpected project failures.
*   **Event-Driven System:** A rudimentary event system allows listeners to react to project events like enqueueing, starting, completing, or failing.
*   **Detailed Logging:** Comprehensive logging provides insights into project execution, resource allocation, and error occurrences.
*   **Customizable Execution Logic:** Projects can have unique execution logic defined through lambda functions, allowing for a wide range of simulated scenarios.

## Getting Started

### Prerequisites

*   A modern C++ compiler (supporting C++11 or later)
*   Basic understanding of C++ concepts like classes, objects, templates, and concurrency

### Compilation

1.  Clone the repository:
    ```bash
    git clone [repository URL]
    ```
2.  Navigate to the project directory:
    ```bash
    cd [project directory]
    ```
3.  Compile the code:
    ```bash
    g++ -std=c++17 -pthread main.cpp -o projects
    ```

### Running the Simulation

Execute the compiled program:

```bash
./projects
