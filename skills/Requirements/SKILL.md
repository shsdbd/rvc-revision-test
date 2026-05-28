# Requirements Skill (Revision)

## Purpose

Use this skill when analyzing, refining, and updating project requirements from the user's perspective.

As this is a **Revision Project**, you are NOT writing requirements from scratch. Your primary objective is to **reuse the existing requirements artifacts (Use Cases, SRS) produced in the previous project** and apply necessary modifications to reflect the new hardware constraint (the removal of the right sensor). 

You must preserve existing requirements that are unaffected by this change.

---

## Preliminary Requirements (Original Baseline)

The legacy RVC system was originally built upon the following preliminary requirements. **Read these carefully to understand the baseline, and identify which legacy requirements must be modified due to the missing right sensor:**

* An RVC automatically cleans household surface.
* It goes straight forward while cleaning.
* If its sensors found an obstacle, it stops cleaning, turns aside left or right, and goes forward with cleaning.
* If there are obstacles in both front, left and right, it move backward and turn aside left or right, and goes forward.
* If it detects dust, power up the cleaning for a while.
* We do not consider the detail design and implementation on HW controls.
* We only focus on the automatic cleaning function.

---

## Responsibilities

During this stage:

- Review the existing requirements (Use Cases, SRS) from the previous project.
- **Identify and modify** only the specific Use Cases and requirements affected by the removal of the right sensor.
- **Actively detect ambiguity or inconsistency** in modifying the legacy behavior (e.g., How should the RVC handle the legacy "turn right" or "detect right obstacle" logic without a right sensor?) and ask the user for clarification.
- Reuse and preserve existing requirements that are unaffected by the hardware change.
- Update the Use Case Descriptions and Use Case Diagram to reflect the changes.
- Produce the updated Software Requirements Specification (SRS) as a revision of the legacy SRS.
- Update requirement traceability.

Implementation or structural design must not begin during this stage.

---

## Requirement Rules

- Every functional requirement must maintain its original requirement ID (e.g., REQ-01). If a new requirement is added, assign a new, unique ID.
- Legacy requirements that are no longer valid must be explicitly marked as **[Modified]** or **[Deprecated]** with an explanation.
- Requirements must be testable and describe behavior, not implementation details.
- Out-of-scope items (like detailed HW control) must remain documented explicitly.

---

## Expected Outputs

The following artifacts must be produced before completing this stage:

1. **Clarification Q&A:** A list of questions directed to the user regarding how to resolve conflicts between legacy requirements and the missing right sensor.
2. **Updated Use Case Descriptions:** Reused from the legacy project, with explicit updates for affected cases.
3. **Updated Use Case Diagram:** A revised PlantUML script visualizing the Use Cases and Actors.
4. **Revised Software Requirements Specification (SRS):** The final updated document containing all preserved, modified, and new requirements.

---

## Verification Checklist

Before completing this stage and asking for user approval:

- Did I reuse the legacy requirements instead of rewriting everything from scratch?
- Did I explicitly ask the user to clarify how to handle the legacy "turn right / right obstacle" logic without a right sensor?
- Are the modifications clearly marked (e.g., [Modified], [Deprecated])?
- Are the Use Case Diagram (PlantUML) and Descriptions accurately updated?
- Is the revised SRS drafted and ready for the next phase?

---

## Communication Rules

When reporting results:

- **Do not assume the answers to ambiguous requirements.** If you do not know how to adapt a legacy requirement to the new constraint, you MUST ask the user.
- Clearly identify which existing requirements were preserved, modified, or deprecated.
- Provide the PlantUML code in a standard code block so the user can render it easily.
- Request explicit user approval of the revised SRS and Use Cases before moving to the Analysis stage.