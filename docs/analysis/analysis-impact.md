# RVC Analysis Impact and Boundary Confirmation

## 현재 단계

현재 단계는 Analysis (Revision)이다. 본 문서는 오른쪽 센서 제거 요구사항이 legacy 분석 산출물에 미치는 영향을 정리하고, legacy system boundary가 유지되었음을 확인한다.

## Boundary Confirmation

| 항목 | 확인 내용 |
|---|---|
| 시스템 바운더리 | legacy와 동일하게 `rvc-controller`이다. |
| 외부 actor 유지 | 사용자, 전방 센서 하드웨어, 측면 센서 하드웨어, 먼지 센서 하드웨어, Movement Motor, Cleaning Motor라는 legacy 외부 actor 구성을 유지한다. 측면 센서 하드웨어의 제공 값만 좌측 장애물 polling으로 축소된다. |
| 책임 범위 유지 | 자동 진공 청소 제어 로직만 포함한다. 실제 센서/actuator 내부 동작, simulator redesign, 물리 제어 세부 구현은 포함하지 않는다. |
| 변경 방식 | 우측 센서 제거로 invalidated 된 판단을 `90도 우회전 완료 후 전방 센싱`이라는 controller behavior로 대체한다. |
| simulator 정보 | simulator의 우측 장애물 정보는 검증 관찰 데이터로만 보존하며 controller 판단 입력 또는 신규 actor로 모델링하지 않는다. |

## Impact Analysis

| 영향 대상 | 영향 유형 | 분석 결과 | 관련 요구사항 |
|---|---|---|---|
| SideSensorHardware | 수정 영향 | 좌측 및 우측 polling actor에서 좌측 polling actor로 의미가 축소된다. | RVC-FR-007, RVC-FR-032, RVC-FR-033 |
| RightSensor / rightDetected | 제거 영향 | controller 판단 입력으로 사용되는 우측 센서 개념은 제거된다. | RVC-FR-010, RVC-FR-035 |
| RightObstacle | 수정 영향 | 우측 센서로 직접 감지되는 값이 아니라, 오른쪽에 존재하는 환경 조건으로 남는다. | RVC-FR-011, RVC-FR-037 |
| RightPathCheck | 신규 영향 | 90도 우회전 완료 후 전방 센싱으로 우측 경로를 확인하는 분석 개념을 추가한다. | RVC-FR-009, RVC-FR-036, RVC-FR-037 |
| ThreeDirectionObstacle | 수정 영향 | 기존 `전방+좌측+우측 직접 감지` 조건을 `전방+좌측 감지 후 우측 경로 확인 실패` 조건으로 대체한다. | RVC-FR-011 |
| MovementState | 유지 및 수정 영향 | 전진, 정지, 후진, 좌회전, 우회전은 유지하되, 우회전은 우측 경로 확인 maneuver에도 사용된다. | RVC-FR-030 |
| TimingRule | 수정 영향 | 회전 완료 시간은 10초에서 4초로 변경되고, 좌측 polling 주기는 1 tick으로 확정된다. | RVC-FR-015, RVC-CON-003, RVC-CON-009 |
| Dust/Suction concepts | 수정 영향 | 먼지 센서는 1 tick polling 방식으로 모델링하며, 직진 주행 중 POWER_UP 개념은 유지하되 정지/회전/후진 중 흡입 상태는 NORMAL에서 OFF로 변경된다. 정지/회전/후진 중 먼지 값은 POWER_UP 진입 조건으로 저장하지 않는다. | RVC-FR-017 ~ RVC-FR-029, RVC-FR-038, RVC-FR-039 |
| Start/Stop use cases | 유지 및 수정 영향 | 시작/종료 개념은 유지하되 센서 actor 명칭은 좌측 센서 기준으로 갱신한다. | RVC-FR-001, RVC-FR-002, RVC-FR-032, RVC-FR-033 |

## Affected System Events

| Legacy event | 영향 유형 | Revised event |
|---|---|---|
| 좌우 장애물 값 요청 | 수정 영향 | 좌측 장애물 값 요청 |
| 좌우 장애물 값 반환 | 수정 영향 | 좌측 장애물 값 반환 |
| 우측 장애물 직접 감지 | 제거 영향 | 90도 우회전 완료 후 전방 장애물 값 확인 |
| 전방+좌측+우측 장애물 직접 감지 | 수정 영향 | 전방+좌측 감지 후 우측 경로 확인 결과 막힘 |
| 우측 장애물 해제 시 우회전 | 수정 영향 | 좌측 장애물이 계속 감지될 때 우측 경로 확인 후 전방 미감지이면 우회전 방향으로 전진 |
| movement motor 정지 명령 | 신규 영향 | cleaning motor OFF 명령이 함께 발생 |
| movement motor 전진 명령 | 신규 영향 | 직진 주행 시작 전 cleaning motor NORMAL 명령이 발생 |
| 먼지 감지 | 수정 영향 | 먼지 감지 이벤트 수신에서 1 tick마다 먼지 감지 값 polling으로 변경 |

## Preserved Concepts and Interactions

| 대상 | 유지 이유 |
|---|---|
| 사용자 시작/종료 입력 | 오른쪽 센서 제거와 무관하다. |
| 전방 장애물 interrupt | 회피 판단의 우선 입력으로 유지된다. |
| 좌회전 우선 정책 | 전방 단독 장애물 및 후진 후 좌측 경로가 열린 경우 유지된다. |
| 후진 이동 | 삼방향 장애물 처리에서 1 tick 후진으로 유지된다. |
| Movement Motor / Cleaning Motor actor | actuator 명령 수신 actor로 기존 바운더리 밖에 유지된다. |
| 먼지 감지 및 POWER_UP | 먼지 값은 1 tick마다 polling으로 확인한다. 직진 주행 중 POWER_UP 판단은 유지한다. 정지/회전/후진 중 흡입 상태는 OFF이며 먼지 값을 POWER_UP 진입 조건으로 저장하지 않는다. |
