# Analysis Skill (Revision)

## Purpose

Use this skill when analyzing the problem domain and system interactions based on the approved Requirements (Use Cases and SRS).

**CRITICAL FOCUS: Establishing the System Boundary**
A core objective of this stage is to precisely recognize and maintain the boundary of the system under development. For this revision project, **the system boundary defined in the legacy project must be reused exactly as-is without any modifications.** Your analysis must focus on integrating the altered hardware logic (the missing right sensor) strictly within this established boundary, ensuring that the system's scope of responsibility remains identical to the previous project. You must reuse the existing Analysis artifacts (Domain Model, System Sequence Diagrams) and adapt them to the new requirements while adhering to this fixed boundary.

---

## Input Prerequisites

Before beginning this stage, ensure you have reviewed the approved outputs from the Requirements stage:
- Revised Software Requirements Specification (SRS)
- Updated Use Case Descriptions
- Updated Use Case Diagram

---

## Responsibilities

During this stage:

- **Enforce Legacy System Boundary:** Strictly adhere to the system boundary established in the previous project. Do not expand, shrink, or alter the system's core responsibilities or its interfaces with external actors.
- **Review Legacy Artifacts:** Analyze the legacy Domain Model and System Sequence Diagrams (SSDs) to understand how the system boundary was previously modeled.
- **Conduct Impact Analysis:** Identify exactly which domain concepts (e.g., RightSensor entity) and system events (e.g., `detectRightObstacle()`) inside or crossing the system boundary are invalidated by the new requirements.
- **Update Domain Model:** Modify the conceptual model to remove the right sensor and introduce workaround concepts (e.g., rotation maneuvers), keeping all changes within the existing system boundary.
- **Update System Sequence Diagrams (SSD):** Redraw the SSDs for the modified Use Cases. Accurately capture the system events crossing the inherited system boundary between the Actor (Environment/Hardware) and the System (acting as a Black Box).
- **Maintain Traceability:** Ensure all conceptual updates directly trace back to the revised requirement IDs.

Implementation or structural software design (Class Diagrams, Object-oriented interfaces) must not begin during this stage.

---

## Analysis Rules

- **The system boundary is immutable:** It must remain identical to the legacy project. No new external actors or out-of-scope system responsibilities can be introduced.
- The Domain Model must represent real-world concepts and their relationships within the context of the established boundary, not software classes.
- SSDs must treat the System as a "Black Box" and clearly show input/output events that cross the fixed system boundary.
- Do not add new system operations that are not justified by the revised Use Cases and the fixed system boundary.

---

## Expected Outputs

The following artifacts must be produced before completing this stage:

1. **Impact Analysis & Boundary Confirmation:** A brief report confirming that the legacy system boundary has been preserved, and detailing which domain concepts and system operations within that boundary were affected.
2. **Revised Domain Model:** A PlantUML class diagram representing the updated domain concepts and relationships within the fixed boundary.
3. **Revised System Sequence Diagrams (SSDs):** PlantUML sequence diagrams for any affected Use Cases, illustrating the updated system events crossing the boundary.

---

## Verification Checklist

Before completing this stage and asking for user approval:

- **Is the system boundary strictly maintained exactly as defined in the legacy project, with no unauthorized scope changes?**
- Did I reuse the legacy Domain Model and SSDs, modifying only what was necessary?
- Is the "Right Sensor" concept completely removed from the Domain Model?
- Do the revised SSDs accurately reflect the system events crossing the boundary without changing the system's identity as a black box?
- Are the PlantUML diagrams syntactically correct and readable?

---

## Communication Rules

When reporting results:

- Explicitly state how the legacy system boundary was preserved and utilized during the analysis.
- Clearly outline which existing Domain concepts and SSDs were preserved, modified, or deleted within that boundary.
- Provide the PlantUML code for both the Domain Model and SSDs in standard code blocks.
- Request explicit user approval of the Domain Model and SSDs before moving to the Design stage.