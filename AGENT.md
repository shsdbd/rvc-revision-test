# AGENTS.md

## Project Overview

**CRITICAL NOTE: THIS IS A REVISION PROJECT.**
This project is **not** a greenfield (built from scratch) project. It is a revision and refactoring of an existing RVC (Robot Vacuum Cleaner) Controller codebase. 

The primary goal is to modify the existing architecture, code, and documentation to satisfy newly changed requirements while preserving the integrity of untouched features. The project emphasizes the active application of software engineering processes, impact analysis, and safe object-oriented refactoring methodologies.

The controller focuses only on automatic cleaning behavior. Detailed hardware control implementation is outside the scope of this project unless explicitly required.

**Hardware Constraint Update:** The RVC hardware configuration has been modified. **The right sensor has been removed**, leaving only the front and left sensors. The existing controller logic and design must be revised to compensate for the lack of right-side environmental detection by utilizing new rotation and movement strategies.

---

## Development Process

Because this is a revision project, the focus must be on understanding the legacy system, identifying the impact of new changes, and updating existing artifacts. 

The project must proceed through the following stages:

1. Requirements (Revision)
2. Analysis (Revision)
3. Design (Refactoring & Update)
4. Implementation (Modification & Extension)
5. Verification (Regression & New Testing)

Each stage has a corresponding skill file. Refer to the appropriate skill file before performing work related to that stage.

Do not skip stages. Do not modify the existing codebase before the Design stage is approved.

The user determines whether the current stage is complete. Do not proceed to the next stage unless the user explicitly approves it.

---

## Stage Descriptions

### 1. Requirements (Revision)

Focus on understanding the new requirements and identifying which existing requirements are affected.

Activities must include:

- Reviewing existing Use Cases and Use Case Diagrams.
- Identifying Use Cases that need to be added, modified, or deprecated due to the missing right sensor.
- Updating the existing Software Requirements Specification (SRS).
- Conducting Requirement Impact Analysis.
- All existing requirements from the original project remain valid and mandatory unless explicitly contradicted by the new 'missing right sensor' constraint.

Implementation must not begin during this stage.

---

### 2. Analysis (Revision)

Focus on analyzing how the requirement changes affect the existing problem domain and system interactions.

Activities must include:

- Reviewing and updating existing System Sequence Diagrams (SSD).
- Updating the Domain Model to reflect new concepts (e.g., specific rotation maneuvers for sensing).
- Clarifying new constraints and assumptions imposed by the legacy system.

---

### 3. Design (Refactoring & Update)

Focus on adapting the existing object-oriented design to accommodate the changes without degrading system architecture.

Activities may include:

- Identifying which existing classes/modules require modification.
- Updating interface designs and class responsibilities.
- Revising the existing state-machine modeling (specifically integrating the new rotation logic for the missing right sensor into the current state flow).
- Dependency analysis to ensure modifications do not introduce circular dependencies or break existing abstractions.
- Producing updated Class Diagrams and State Diagrams.

Implementation must not begin before the revised design artifacts are produced and approved.

---

### 4. Implementation (Modification & Extension)

Modify the existing C++ codebase based strictly on the approved design changes.

Implementation guidelines:

- **Understand before modifying:** Thoroughly analyze the existing code context before making changes.
- Ensure backward compatibility for features that are not meant to change.
- Refactor legacy code safely; prefer simple, maintainable, readable C++ code.
- Implement the designed rotation/movement logic to account for the missing right sensor.
- Do not introduce feature creep or rewrite entire modules unless explicitly instructed to do so.
- Preserve traceability between updated requirements, revised design, modified implementation, and tests.

---

### 5. Verification (Regression & New Testing)

Verification is critical in a revision project to ensure new changes do not break existing functionality.

Verification includes the following activities:

#### Static Analysis
Use existing tools (clang-format, clang-tidy, cppcheck) to ensure new modifications match the project's coding style and do not introduce new vulnerabilities.

#### Dynamic Analysis
Analyze runtime behavior to ensure the newly injected rotation logic integrates smoothly with the existing controller loop.

#### Unit Testing
- **Regression Testing:** Run all existing unit tests. They must pass, or be intentionally updated if the original expected behavior has changed.
- **New Tests:** Write new unit tests for any new or modified public methods (e.g., testing the logic that triggers rotation when the right boundary needs to be checked).

#### System Testing
System tests must validate the modified requirement scenarios against the updated system.

---

## Development Environment

Use the following development environment and tools:

- Git
- CMake
- Ninja
- Clang/LLVM
- clang-format
- clang-tidy
- cppcheck
- VSCode

---

## General Rules for Revision

- **Do no harm:** Never break existing, unrelated functionality.
- Traceability is key: Clearly link modifications back to the new "missing right sensor" requirement.
- Explain what existing components you are modifying, deleting, or keeping intact.
- Understand the current project structure before suggesting code changes.
- Clearly state assumptions and limitations regarding the legacy code.
- Do not hide failures or errors, especially regression test failures.

---

## Skill Usage

Each development stage has a corresponding skill file located under the skills directory.

Examples:

- skills/requirements/SKILL.md
- skills/analysis/SKILL.md
- skills/design/SKILL.md
- skills/implementation/SKILL.md
- skills/verification/SKILL.md

---

## Verification Commands

Use the following commands when appropriate:

- ./commands/configure.sh
- ./commands/build.sh
- ./commands/verify-all.sh
- ./commands/run-tests.sh

---

## Expected Workflow

Typical revision workflow:

1. Analyze existing Requirements & integrate changes (Update Use Case, SRS).
2. Perform Impact Analysis on Domain & Interactions (Update SSD, Domain Model).
3. Refactor Design artifacts (Update Class Diagrams, State Machines).
4. Safely modify and extend the existing C++ codebase.
5. Perform Regression & Verification testing.
6. Request user approval before proceeding to the next stage.

---

## Communication Rules

When responding:

- State the current development stage.
- Explicitly identify whether you are discussing existing legacy components or new additions.
- Describe what was changed in the legacy system and why.
- Describe what was NOT changed and why.
- Request user approval before advancing stages.

---

## Simulator Reuse Rule

This project reuses the existing simulator and verification infrastructure from:

https://github.com/kucse-oosd-team10/rvc-controller

The simulator, scenario files, Python integration layer, and test harness should be treated as external verification infrastructure.
**Do not modify the simulator to solve the missing right sensor problem.** The controller logic itself must adapt to the existing simulator environment.

---

## Code Quality and Coverage

Use SonarQube and coverage tooling as part of verification activities whenever available.

- Ensure code coverage does not drop below the existing baseline after modifications.
- ./commands/coverage.sh
- ./commands/sonarqube.sh