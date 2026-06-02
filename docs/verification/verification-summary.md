# RVC Verification Summary

> [추가] 본 파일은 revision 시작 전 Verification 원본문서에 오른쪽 센서 삭제 revision의 검증 변경사항을 직접 추가한 제출용 문서이다.
> 기존 원문은 삭제하지 않고 유지하며, 변경된 검증 기준/결과는 각 항목에 `[수정]`, `[추가]`, `[삭제]` 표식을 붙여 기록한다.
> 상세 변경사항은 문서 하단의 `8. 오른쪽 센서 삭제 Revision 검증 반영`에 추가한다.

## 1. 현재 단계

현재 단계는 Verification이다. 검증 기준은 다음과 같이 정정되었다.

- Unit Test: GoogleTest 기반으로 수행한다.
- System Test: 직접 개발되어 GitHub에 존재하는 simulator만 사용한다.
- Mock/fake 기반 controller scenario test는 system test로 간주하지 않는다.

## 2. 현재 검증 상태

| 항목 | 상태 | 비고 |
|---|---|---|
| GoogleTest unit test 코드 | 작성됨 | `tests/rvc_unit_tests.cpp` |
| GoogleTest CMake 연동 | 작성됨 | `find_package(GTest REQUIRED)` 사용 |
| GoogleTest 로컬 의존성 | 미충족 | 현재 환경에서 `GTest`를 찾지 못해 configure 실패 |
| Simulator 기반 system test | 미구현 | simulator GitHub 위치와 실행 방식 필요 |
| Mock/fake 기반 system test | 제거됨 | system test로 오해되지 않도록 제거 |

## 3. GoogleTest Unit Test

Unit test 파일:

- `tests/rvc_unit_tests.cpp`

포함된 테스트:

| 테스트 그룹 | 검증 내용 | 관련 요구사항 |
|---|---|---|
| `LeftPriorityAvoidanceStrategyTest` | 좌회전 우선 회피 전략, 후진 필요 여부, 후진 중 해제 방향 판단 | RVC-FR-008 ~ RVC-FR-014 |
| `CleaningManagerTest` | POWER_UP 3초 유지, 먼지 재감지 시 타이머 재시작, 먼지 미감지 시 NORMAL 복귀 | RVC-FR-023 ~ RVC-FR-029 |
| `CleaningManagerTest` | 정지/회전/후진 중 먼지 감지 시 POWER_UP 지연 | RVC-FR-017 ~ RVC-FR-019 |
| `MovementManagerTest` | 회전 명령 후 timer 기반 회전 완료 판단 | RVC-FR-015 |
| `RvcControllerTest` | 청소 시작/종료 시 센서 초기화/종료와 motor 명령 | RVC-FR-001, RVC-FR-002, RVC-FR-020 ~ RVC-FR-022, RVC-FR-030 ~ RVC-FR-033 |
| `RvcControllerTest` | 전방 장애물만 감지된 경우 좌회전 후 전진 복귀 | RVC-FR-008, RVC-FR-016 |
| `RvcControllerTest` | mock 기반 장애물 탈출 흐름 | unit-level integration check only |

## 4. System Test

System test는 현재 구현하지 않았다. 기준은 기존 GitHub simulator만 사용하는 것이다.

`commands/run-system-tests.sh`는 simulator 명령이 명시되지 않으면 실패하도록 변경했다.

필요한 설정:

```bash
RVC_SIMULATOR_TEST_COMMAND="<simulator 실행 명령>" ./commands/run-system-tests.sh
```

또는 simulator repository와 실행 명령이 확정되면 `commands/run-system-tests.sh`에 고정 명령으로 반영할 수 있다.

## 5. 현재 실행 결과

현재 환경에서 다음 명령은 실패한다.

```bash
./commands/configure.sh
```

실패 원인:

```text
Could NOT find GTest
```

즉, GoogleTest 설치 또는 CMake에서 찾을 수 있는 경로 설정이 필요하다.

## 6. 남은 작업

1. GoogleTest 설치 또는 CMake 경로 설정
2. `./commands/configure.sh`
3. `./commands/build.sh`
4. `./commands/run-unit-tests.sh`
5. simulator GitHub repository 확인
6. simulator 기반 system test command 확정
7. `./commands/run-system-tests.sh`
8. `./commands/verify-all.sh`

## 8. 오른쪽 센서 삭제 Revision 검증 반영 [추가]

### 8.1 현재 단계 및 검증 기준 변경 [수정]

현재 단계는 Verification (Regression & New Testing)이다. 본 문서는 오른쪽 센서 제거와 새로운 우측 경로 확인 전략이 적용된 구현 결과를 검증한 내용을 정리한다.

- Unit Test: [유지] GoogleTest 기반으로 수행한다.
- System Test: [수정] 기존 custom simulator를 사용한다. 원문에서는 simulator GitHub 위치와 실행 방식 확정이 필요하다고 기록했으나, revision 검증에서는 repository 내 simulator 기반 pytest를 사용한다.
- Legacy test: [추가] 새 요구사항과 충돌하지 않는 경우 유지한다.
- Legacy 기대값: [수정] 새 요구사항과 충돌하는 경우 테스트 의도와 assertion을 새 요구사항 기준으로 갱신한다.

### 8.2 검증 대상 변경 요약 [추가]

| 변경 영역 | 검증 관점 |
|---|---|
| 우측 센서 제거 | [추가] controller 내부 인터페이스와 회피 전략이 `rightDetected`에 의존하지 않는지 확인 |
| 우측 경로 확인 | [추가] `90도 우회전 -> 전방 센싱 -> 막힘 시 90도 좌회전 복귀 -> 1 tick 후진` 흐름 확인 |
| 3방향 장애물 처리 | [수정] legacy의 즉시 후진이 아니라 revised sequence가 수행되는지 확인 |
| 회전 완료 시간 | [수정] 기본 회전 시간이 4초로 변경되었는지 확인 |
| Cleaning invariant | [수정] non-forward 상태에서 cleaning OFF, forward 시작 시 최소 NORMAL 확인 |
| Dust polling | [수정][삭제] forward tick에서 dust polling으로 POWER_UP 진입, pending POWER_UP 제거 확인 |

### 8.3 Test Evaluation Report [수정]

| 테스트 종류 | 조치 | 이유 |
|---|---|---|
| 기존 GoogleTest unit tests | [수정] 대부분 유지, 변경 요구사항과 충돌하는 기대값은 갱신 | legacy 기능 regression을 유지하면서 새 요구사항을 반영하기 위함 |
| `LeftPriorityAvoidanceStrategyTest` | [수정] 좌우 snapshot 기반 테스트를 좌측 polling 기반 테스트로 수정 | 내부 회피 전략이 더 이상 우측 센서 값을 받지 않음 |
| `CleaningManagerTest` | [수정][삭제] pending POWER_UP 관련 테스트를 non-forward OFF 및 no-pending 테스트로 수정 | 비전진 중 dust 값을 POWER_UP 조건으로 저장하지 않음 |
| `LegacyAdaptersTest` | [추가] side adapter가 우측 값을 읽지 않는지 검증하도록 수정 | simulator 우측 값은 controller 입력이 아니라 검증 관찰값이어야 함 |
| `RvcControllerTest` | [수정][추가] 우측 경로 확인, 원래 방향 복귀, 1 tick 후진 흐름 추가 | 새 회피 전략 핵심 상태 전이를 검증하기 위함 |
| `simulator/scenarios/06_backward_avoidance.yaml` | [수정] 즉시 `BACKWARD` 기대값을 `RIGHT -> LEFT -> BACKWARD` sequence로 수정 | legacy truth table 기대값이 새 요구사항과 충돌함 |
| slow simulator scenario | [추가] 기본 실행에서는 skip 유지 | `30_nfr07_long_run_60min.yaml`은 60분 장기 실행 시나리오로 `slow: true` 지정 |

[수정] 삭제한 테스트는 없다. 다만 요구사항상 더 이상 유효하지 않은 assertion은 새 동작 기준으로 변경했다.

### 8.4 GoogleTest Unit Test Results [수정]

원문에서는 현재 환경에서 `GTest`를 찾지 못해 configure 실패한다고 기록했다. Revision 검증에서는 의존성 설정 후 다음 명령으로 unit test를 실행했다.

```bash
./commands/run-unit-tests.sh
```

결과:

```text
100% tests passed, 0 tests failed out of 47
```

주요 검증 항목:

| 테스트 그룹 | 검증 내용 | 관련 요구사항/설계 |
|---|---|---|
| `LeftPriorityAvoidanceStrategyTest` | [수정] 좌측 clear 시 좌회전, 좌측 blocked 시 우측 경로 확인 선택 | RVC-FR-008, RVC-FR-009, RVC-FR-013 |
| `CleaningManagerTest` | [수정][삭제] POWER_UP 유지, dust polling, non-forward OFF, pending POWER_UP 제거 | RVC-FR-017 ~ RVC-FR-029, RVC-FR-038, RVC-FR-039 |
| `MovementManagerTest` | [수정] 회전 타이머와 movement motor 명령 | RVC-FR-015, RVC-FR-030 |
| `LegacyAdaptersTest` | [추가] legacy simulator adapter가 core에 좌측 값만 전달 | RVC-FR-035, RVC-CON-007 |
| `SensorSubjectsTest` | [수정] obstacle/dust polling 및 observer 동작 | RVC-FR-004, RVC-FR-039 |
| `RvcControllerTest` | [수정][추가] 시작/종료, 전방 단독 좌회전, front+left 우측 경로 확인, blocked right path 복귀 후 후진 | RVC-FR-001, RVC-FR-002, RVC-FR-008 ~ RVC-FR-016 |
| `SimulatorRVCControllerAdapterTest` | [추가] simulator adapter와 core controller 통합 동작 | simulator reuse rule |

### 8.5 Simulator System Test Results [수정]

원문에서는 system test를 구현하지 않았고, `RVC_SIMULATOR_TEST_COMMAND="<simulator 실행 명령>" ./commands/run-system-tests.sh` 형태의 실행 명령 확정이 필요하다고 기록했다.

Revision 검증 실행 명령:

```bash
PYTHONPATH="build/bindings:simulator" .venv/bin/python -m pytest simulator/tests -q -rs -p no:cacheprovider
```

결과:

```text
29 passed, 1 skipped
SKIPPED [1] simulator/tests/test_scenarios.py:31: slow test; run with -m slow to enable
```

skip된 시나리오:

| 파일 | 이유 |
|---|---|
| `simulator/scenarios/30_nfr07_long_run_60min.yaml` | [추가] 60분 장기 실행 시나리오이며 `slow: true`로 표시되어 기본 pytest 실행에서 제외됨 |

수정한 system scenario:

| 파일 | Legacy 기대값 | Revised 기대값 |
|---|---|---|
| `simulator/scenarios/06_backward_avoidance.yaml` | `front+left+right` 조건에서 즉시 `BACKWARD` | [수정] `RIGHT -> LEFT -> BACKWARD` sequence. longer dead-end corridor fixture로 강화 |

[수정] 오른쪽 센서가 제거되었으므로 controller는 더 이상 `front+left+right`를 직접 감지해 즉시 후진하지 않는다. 새 요구사항에 따라 `front+left` 감지 후 90도 우회전하여 전방 센싱으로 우측 경로를 확인하고, 막혀 있으면 90도 좌회전으로 원래 방향에 복귀한 뒤 1 tick 후진한다. 따라서 scenario 06의 pass/fail 기준을 새 회피 전략의 observable motor sequence로 갱신했다.

### 8.6 Static Analysis [추가]

원문에는 static analysis 실행 결과가 없었다. Revision 검증에서는 다음 명령을 실행했다.

```bash
./commands/static-analysis.sh
```

결과:

```text
Checking src/AdapterTimer.cpp ...
...
Checking src/SimulatorApi.cpp ...
nofile:0:0: information: Active checkers: 183/186
```

cppcheck에서 수정 코드 관련 defect는 보고되지 않았다.

Formatting 및 diff check:

```bash
./commands/format.sh
git diff --check
```

결과: 통과.

### 8.7 Coverage [추가]

원문에는 coverage 실행 결과가 없었다. Revision 검증에서는 다음 명령을 실행했다.

```bash
./commands/coverage.sh
```

결과:

```text
100% tests passed, 0 tests failed out of 47
(INFO) Writing coverage report...
```

생성 파일:

- `build/coverage/coverage.xml`

Coverage 요약:

| 항목 | 결과 |
|---|---|
| line coverage | 98.58% |
| branch coverage | 80.56% |
| lines covered / valid | 625 / 634 |
| branches covered / valid | 145 / 180 |

### 8.8 Integrated Verification Script [추가]

원문에서는 `commands/run-system-tests.sh` 실행 명령 확정과 `commands/verify-all.sh` 실행이 남은 작업으로 기록되어 있었다.

Revision 검증 실행 명령:

```bash
RVC_PYTEST=.venv/bin/pytest ./commands/verify-all.sh
```

결과:

```text
static-analysis: passed
build: passed
unit/system tests: passed
coverage: passed
./commands/verify-all.sh: line 9: ./commands/sonarqube.sh: No such file or directory
```

[추가] `verify-all.sh`는 마지막에 `./commands/sonarqube.sh`를 호출하지만 현재 repository에는 해당 파일이 없다. 따라서 통합 스크립트 자체는 최종 exit code 1로 종료된다. 다만 그 전 단계인 static analysis, build, unit test, system test, coverage는 모두 통과했다.

### 8.9 Requirement Traceability [수정]

| 요구사항/설계 항목 | 검증 |
|---|---|
| 우측 센서 제거 | [추가] `LeftPriorityAvoidanceStrategyTest`, `LegacyAdaptersTest.SideAdapterReadsOnlyLeftValue` |
| `front+left` 후 우측 경로 확인 | [추가] `RvcControllerTest.FrontAndLeftObstacleTurnsRightThenMovesForward` |
| 우측 경로 막힘 후 원래 방향 복귀 및 후진 | [수정] `RvcControllerTest.BlockedRightPathReturnsToOriginalDirectionThenBacksUp`, scenario 06 |
| 후진 후 좌측 polling | [수정] `LeftPriorityAvoidanceStrategyTest.UsesLeftPollingAfterBackwardTick`, `RvcControllerTest.MockBasedObstacleEscapeScenarioIsCoveredByUnitTestOnly` |
| 회전 시간 4초 | [수정] `MovementManagerTest.CustomTurnDurationStartsTimerWithProvidedDuration`, `kDefaultTurnDuration` implementation |
| movement stop 시 cleaning OFF | [추가] `CleaningManagerTest.AvoidanceMovementForcesOffFromPowerUp`, controller obstacle tests |
| forward 시작 시 cleaning NORMAL | [수정] `RvcControllerTest.StartStopInitializesAndShutsDownExternalInterfaces`, forward re-entry tests |
| dust polling 및 no pending POWER_UP | [수정][삭제] `CleaningManagerTest.TickWhileForwardPollsDustSensor`, `CleaningManagerTest.NonForwardTickKeepsCleaningOffWithoutPendingPowerUp` |
| simulator regression | [추가] `29 passed, 1 skipped` system test result |

### 8.10 Verification Conclusion [수정]

현재 검증 기준에서 구현 변경은 unit test, simulator system test, static analysis, coverage를 통과했다.

남은 제한사항은 다음 하나이다.

- [추가] `commands/verify-all.sh`가 존재하지 않는 `commands/sonarqube.sh`를 호출한다. SonarQube 검증을 통합 완료 상태로 만들려면 해당 command 파일을 추가하거나 `verify-all.sh`에서 현재 repository에 맞는 SonarQube 실행 방식으로 갱신해야 한다.
