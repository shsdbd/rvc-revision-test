# Design Skill (Revision & Refactoring)

## Purpose

Use this skill when designing and updating the software structure of the RVC controller based on the approved Analysis artifacts.

As this is a **Revision Project**, your objective is to adapt the existing object-oriented software design to satisfy the new requirements (e.g., the missing right sensor) **while strictly respecting the system boundary** confirmed in the Analysis phase. You must use the previous project's architecture and design artifacts as your baseline, applying modifications only where necessary.

---

## Input Prerequisites

Before beginning this stage, ensure you have reviewed:
- Legacy Design Artifacts (Original Class Diagrams, State Diagrams, Interface definitions)
- Revised Analysis Artifacts (Updated Domain Model, Updated SSDs, Boundary Confirmation)

---

## Responsibilities

During this stage:

- **Enforce Architectural Consistency:** Maintain the core structural baseline of the legacy project. Do not rewrite the entire architecture.
- **Identify Refactoring Targets:** Determine exactly which legacy classes, interfaces, and state machines are affected by the removal of the right sensor.
- **Update State-Machine Behavior:** Redesign the controller's state machine to integrate the new workaround logic (e.g., rotating the RVC to check the right side). This is the most critical design change.
- **Update Class & Interface Design:** Remove dependencies on the non-existent right sensor from hardware abstraction interfaces and control logic classes.
- **Preserve the Boundary:** Ensure that your internal structural changes do not leak outside the confirmed system boundary or alter how the system fundamentally interacts with external actors.

Implementation must not begin before sufficient revised design artifacts exist and are approved.

---

## Design Principles

Apply object-oriented and maintainable design principles, focusing on safe legacy code refactoring:

- Open/Closed Principle (Prefer extending behavior over heavily modifying working legacy code where possible)
- Single Responsibility Principle
- Low coupling & High cohesion
- Explicit state transitions for the new sensing maneuvers
- Readability and maintainability

---

## State Machine Revision Guidance

The RVC controller behavior heavily relies on state machine modeling. 

When updating the legacy state machine, you must explicitly design how the system transitions between states to compensate for the missing right sensor. 
Consider how legacy states (e.g., `Idle`, `Cleaning`, `ObstacleAvoidance`, `BackwardMovement`) need to be modified or if new sub-states (e.g., `RotatingToScanRight`) are required.

All new state transitions must be clearly documented and logically sound.

---

## Expected Outputs

The following artifacts must be produced before completing this stage:

1. **Design Modification Rationale:** A document explaining why specific legacy classes or states were modified and how the legacy structure was preserved.
2. **Revised Class Diagrams:** PlantUML diagrams showing the updated structural relationships, focusing on the abstraction of sensor inputs and the behavior controller.
3. **Revised State Machine Diagrams:** PlantUML state diagrams explicitly detailing the new rotation/obstacle detection flow.
4. **Updated Interface Definitions:** Modified API or method signatures for internal communication.

---

## Verification Checklist

Before completing this stage and asking for user approval:

- Does the revised design strictly adhere to the system boundary confirmed in the Analysis stage?
- Was the legacy architecture used as the baseline without unnecessary structural rewrites?
- Is the new state machine logic for right-side detection completely and safely integrated?
- Are the "Right Sensor" references completely removed from internal interfaces?
- Is the revised design implementable and traceable to the updated Domain Model and SSDs?

---

## Communication Rules

When reporting results:

- Clearly separate the legacy design components that remain unchanged from the newly refactored components.
- Explain your refactoring decisions and rationale (e.g., "I modified the `ObstacleAvoidance` state because...").
- Provide PlantUML code for Class and State diagrams in standard code blocks.
- Request explicit user approval of the Design artifacts before moving to the Implementation stage.