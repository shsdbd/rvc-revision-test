# Verification Skill (Revision & Regression)

## Purpose

Use this skill when validating software quality, behavior, correctness, and requirement coverage after the implementation is complete.

As this is a **Revision Project**, verification has two equally important goals:
1. **Regression Testing:** Ensure that the legacy functionalities that were not supposed to change still work perfectly.
2. **New Feature Testing:** Validate that the new workaround logic (handling the missing right sensor) works correctly and satisfies the revised requirements.

---

## Input Prerequisites

Before beginning this stage, ensure you have:
- The modified C++ implementation code.
- The revised Requirements (SRS, Use Cases) and Design artifacts.
- The existing legacy test suite (Unit tests and Simulator scenarios).

---

## Responsibilities

During this stage:

- **Evaluate Existing Tests:** Review the legacy unit and system tests. If they are still applicable to the new requirements, preserve them exactly as they are. If legacy behavior has intentionally changed (e.g., right-side obstacle detection logic), modify the tests accordingly.
- **Perform Unit Testing:** Validate component-level behavior using Google Test.
- **Perform System Testing:** Validate requirement-level scenarios using the custom-developed simulator.
- **Execute Static & Dynamic Analysis:** Ensure code quality and safe runtime behavior.
- **Report Coverage:** Check if the modified or newly added code is adequately covered by tests.

---

## Unit Testing Rules (Google Test)

- All unit tests must be written and executed using the **Google Test (gtest)** framework.
- Run all legacy unit tests first. If a legacy test fails, analyze whether it is a true regression bug or if the test itself needs updating due to the revised logic.
- Write new Google Test cases for any newly introduced classes, methods, or state transitions related to the missing right sensor.
- Test normal cases, boundary conditions, and error situations.

---

## System Testing Rules (Custom Simulator)

- System tests must validate requirement-level behavior using the **existing custom-developed simulator**.
- **Scenario Reuse & Modification:** You can likely reuse most of the existing simulator test scenarios (e.g., room layouts, obstacle placements). However, you must critically review the **validation conditions (pass/fail criteria)**. 
    - *Example:* Because the RVC now needs to rotate to check its right side, the overall cleaning time or the exact movement sequence might differ from the legacy version. Update the test assertions to accommodate these new behavioral patterns.
- Every modified or new requirement (from the revised SRS) must be traceable to at least one successful simulator test.

---

## Static & Dynamic Analysis Responsibilities

- **Static Analysis:** Use `clang-format`, `clang-tidy`, and `cppcheck` to review the modified codebase for coding style consistency, complexity, and potential defects.
- **Dynamic Analysis:** Monitor runtime logs during simulator execution to verify correct state transitions (especially verifying that the new rotation/sensing states trigger correctly without crashing).

---

## Expected Outputs

The following artifacts and results must be produced before completing this stage:

1. **Test Evaluation Report:** A brief summary of which existing tests were kept, modified, or removed.
2. **Google Test Results:** Logs showing the successful execution of all unit tests.
3. **Simulator Test Results:** Logs showing the successful execution of system scenarios using the custom simulator.
4. **Static Analysis & Coverage Reports:** Results from cppcheck, clang-tidy, and coverage tools (e.g., lcov, SonarQube).

---

## Verification Checklist

Before completing verification and asking for final user approval:

- Did all legacy tests pass (either as-is or after valid modification)?
- Are unit tests correctly implemented using Google Test?
- Were the custom simulator scenarios executed, and were their pass/fail conditions correctly updated to reflect the new right-sensor logic?
- Are verification failures or unexpected behaviors reported honestly without hiding them?
- Is there a clear traceability link between the revised requirements and the passed tests?

---

## Communication Rules

When reporting results:

- Clearly distinguish between **Regression Test results** (legacy code) and **New Test results** (modified code).
- Report any warnings, failing tests, or limitations honestly. Do not hide or suppress failing test outputs.
- Explain the likely causes of any test failures and propose fixes if necessary.
- If simulator conditions had to be modified, explain exactly what was changed and why.