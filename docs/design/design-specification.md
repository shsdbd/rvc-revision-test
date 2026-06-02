# RVC Controller Design Specification

> [추가] 본 파일은 revision 시작 전 Design 원본문서에 오른쪽 센서 삭제 revision의 설계 변경사항을 직접 추가한 제출용 문서이다.
> 기존 원문은 삭제하지 않고 유지하며, 변경된 설계는 각 항목에 `[수정]`, `[추가]`, `[삭제]` 표식을 붙여 기록한다.
> 상세 변경사항은 문서 하단의 `14. 오른쪽 센서 삭제 Revision 설계 반영`에 추가한다.

## 1. 현재 단계

현재 단계는 Design이다. 본 문서는 요구사항 분석 산출물을 바탕으로 `rvc-controller`의 객체지향 설계를 정의한다.

## 2. 설계 범위

설계 대상은 `rvc-controller` 내부 소프트웨어 구조이다. 실제 센서 하드웨어, movement motor, cleaning motor는 외부 actor이며, controller는 추상 인터페이스를 통해서만 상호작용한다.

## 3. 설계 목표

- 센서 입력, 제어 판단, actuator 명령을 분리한다.
- 장애물 회피 동작을 명시적인 상태 머신으로 표현한다.
- 이동 명령 관리와 청소 흡입 관리를 균형 있게 분리한다.
- 실제 하드웨어 세부사항에 의존하지 않는 인터페이스를 정의한다.
- 요구사항 ID와 설계 요소 사이의 추적성을 유지한다.

## 4. 주요 설계 결정

| ID | 결정 | 근거 |
|---|---|---|
| DD-001 | `RvcController`를 최상위 조정 객체로 둔다. | 청소 시작/종료, 센서 초기화/종료, 이동/흡입 제어를 하나의 시스템 바운더리에서 조정해야 한다. |
| DD-002 | 이동 상태와 청소 흡입 상태를 별도 manager가 관리한다. | 이동 상태 전이와 cleaning motor 제어는 함께 조정되어야 하지만 책임은 분리되어야 하기 때문이다. |
| DD-003 | 장애물 판단은 Strategy 패턴으로 분리한다. | 후진 필요 여부, 회전 방향 결정, 후진 중 해제 방향 선택 규칙을 교체 가능한 전략으로 분리하기 위함이다. |
| DD-004 | 청소 흡입 관리는 `CleaningManager`로 분리한다. | 3초 POWER_UP 유지, 재감지 시 타이머 재시작, 회피 중 먼지 감지 지연 처리를 cleaning motor 관점에서 다루기 위함이다. |
| DD-005 | 시간 의존 동작은 `Timer` 인터페이스로 추상화한다. | 회전 10초, POWER_UP 3초 규칙을 테스트 가능하게 만들기 위함이다. |
| DD-006 | 이동 동작은 State 패턴으로 표현한다. | RVC가 현재 상태 객체 하나를 보유하고, 같은 이벤트 인터페이스를 통해 상태별 동작을 다르게 수행하도록 하기 위함이다. |
| DD-007 | 이동 motor 명령은 `MovementManager`로 분리한다. | 상태 객체가 실제 movement motor 인터페이스에 직접 의존하지 않고 이동 의도를 전달하도록 하기 위함이다. |

## 5. 모듈 구성

| 모듈 | 책임 |
|---|---|
| Controller Core | 전체 상태 전이 조정, 사용자 시작/종료 이벤트 처리 |
| Movement States | 이동 상태별 이벤트 처리와 상태 전이 캡슐화 |
| Sensor Interfaces | 전방/측면/먼지 센서 하드웨어와의 추상 입력 경계 |
| Motor Interfaces | movement motor, cleaning motor로 나가는 추상 명령 경계 |
| Obstacle Avoidance Strategy | 장애물 조합에 따른 후진 필요 여부와 회전 방향 결정 |
| Movement Management | movement motor 명령과 회전 완료 시간 관리 |
| Cleaning Management | OFF/NORMAL/POWER_UP 청소 흡입 상태 결정 |
| Timing | 회전 완료 시간과 POWER_UP 유지 시간 판단 |

## 6. 클래스 책임

| 클래스/인터페이스 | 책임 |
|---|---|
| `RvcController` | 청소 시작/종료, 센서 초기화/종료, 현재 이동 상태 객체 보유, 하위 객체 조정 |
| `IRvcState` | 이동 상태별 공통 이벤트 인터페이스 정의 |
| `OffState` | OFF 상태에서 청소 시작 이벤트 처리 |
| `ForwardState` | 전진 중 전방 장애물, 먼지 감지, 종료 이벤트 처리 |
| `StoppedForObstacleState` | 장애물 감지 후 정지 상태에서 회피 전략 결과에 따른 상태 전이 처리 |
| `TurningState` | 좌회전/우회전 중 10초 완료 이벤트 처리 |
| `BackwardState` | 후진 중 측면 센서 polling 결과에 따른 회전 전이 처리 |
| `IObstacleAvoidanceStrategy` | 장애물 조합에 따른 후진 필요 여부와 회전 방향 결정 인터페이스 |
| `LeftPriorityAvoidanceStrategy` | 좌회전 우선 요구사항을 반영한 기본 회피 전략 |
| `MovementManager` | movement motor 명령, 회전 10초 타이머, 이동 완료 판단 관리 |
| `CleaningManager` | 흡입 상태 전이, POWER_UP 타이머, 회피 중 먼지 감지 지연 처리 |
| `IFrontObstacleSensor` | 전방 장애물 interrupt 입력을 controller에 전달 |
| `ISideObstacleSensor` | 좌측/우측 장애물 값을 polling 방식으로 제공 |
| `IDustSensor` | 먼지 감지 값을 제공 |
| `IMovementMotor` | 전진, 정지, 후진, 좌회전, 우회전 명령 수신 |
| `ICleaningMotor` | OFF, NORMAL, POWER_UP 흡입 명령 수신 |
| `ITimer` | 3초/10초 경과 여부를 추상화 |

## 7. 클래스 다이어그램

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
skinparam classAttributeIconSize 0

class RvcController {
  -currentState: IRvcState
  -frontSensor: IFrontObstacleSensor
  -sideSensor: ISideObstacleSensor
  -dustSensor: IDustSensor
  -movementManager: MovementManager
  -cleaningManager: CleaningManager
  -avoidanceStrategy: IObstacleAvoidanceStrategy
  +startCleaning()
  +stopCleaning()
  +onFrontObstacleDetected()
  +onDustDetected()
  +changeState(state)
  +tick()
}

interface IRvcState {
  +onEnter(context)
  +startCleaning(context)
  +stopCleaning(context)
  +onFrontObstacleDetected(context)
  +onDustDetected(context)
  +tick(context)
}

class OffState {
  +startCleaning(context)
}

class ForwardState {
  -pendingFrontObstacle: bool
  +stopCleaning(context)
  +onFrontObstacleDetected(context)
  +onDustDetected(context)
}

class StoppedForObstacleState {
  -sideSnapshot
  +onEnter(context)
  +tick(context)
}

class TurningState {
  -direction
  -turnStarted: bool
  +onEnter(context)
  +tick(context)
}

class BackwardState {
  -previousSideSnapshot
  +onEnter(context)
  +tick(context)
}

interface IObstacleAvoidanceStrategy {
  +decideOnFrontObstacle(sideSnapshot)
  +decideWhileBackward(previousSideSnapshot, currentSideSnapshot)
}

class LeftPriorityAvoidanceStrategy {
  +decideOnFrontObstacle(sideSnapshot)
  +decideWhileBackward(previousSideSnapshot, currentSideSnapshot)
}

class MovementManager {
  -motor: IMovementMotor
  -turnTimer: ITimer
  -currentCommand
  -turnDurationSeconds: int
  +stop()
  +moveForward()
  +moveBackward()
  +turnLeft()
  +turnRight()
  +isTurnComplete()
}

class CleaningManager {
  -cleaningMotor: ICleaningMotor
  -dustSensor: IDustSensor
  -powerUpTimer: ITimer
  -currentState
  -pendingPowerUp: bool
  -powerUpDurationSeconds: int
  +start()
  +stop()
  +onDustDetected(movementState)
  +onMovementStateChanged(movementState)
  +tick()
}

interface IFrontObstacleSensor {
  +initialize()
  +shutdown()
  +registerInterruptHandler(handler)
}

interface ISideObstacleSensor {
  +initialize()
  +shutdown()
  +read()
}

interface IDustSensor {
  +initialize()
  +shutdown()
  +isDustDetected()
}

interface IMovementMotor {
  +stop()
  +moveForward()
  +moveBackward()
  +turnLeft()
  +turnRight()
}

interface ICleaningMotor {
  +off()
  +normal()
  +powerUp()
}

interface ITimer {
  +start(duration)
  +expired()
  +reset()
}

RvcController --> IRvcState : current
IRvcState <|.. OffState
IRvcState <|.. ForwardState
IRvcState <|.. StoppedForObstacleState
IRvcState <|.. TurningState
IRvcState <|.. BackwardState

RvcController --> IFrontObstacleSensor
RvcController --> ISideObstacleSensor
RvcController --> IDustSensor
RvcController --> MovementManager
RvcController --> CleaningManager
RvcController --> IObstacleAvoidanceStrategy
IObstacleAvoidanceStrategy <|.. LeftPriorityAvoidanceStrategy
StoppedForObstacleState --> IObstacleAvoidanceStrategy
BackwardState --> IObstacleAvoidanceStrategy
MovementManager --> IMovementMotor
MovementManager --> ITimer
CleaningManager --> IDustSensor
CleaningManager --> ICleaningMotor
CleaningManager --> ITimer
@enduml
```

`RvcController`는 외부 actor로부터 들어온 이벤트를 현재 `IRvcState` 객체에 위임한다. 각 concrete state는 동일한 인터페이스를 구현하지만, 현재 상태에 맞는 동작과 다음 상태 전이를 다르게 수행한다.

## 8. 인터페이스 설계

### 8.1 센서 인터페이스

| 인터페이스 | 주요 동작 | 관련 요구사항 |
|---|---|---|
| `IFrontObstacleSensor` | 초기화, 종료, 전방 장애물 interrupt handler 등록 | RVC-FR-004, RVC-FR-006, RVC-FR-032, RVC-FR-033 |
| `ISideObstacleSensor` | 초기화, 종료, 좌우 장애물 값 polling | RVC-FR-005, RVC-FR-007, RVC-FR-011 ~ RVC-FR-014, RVC-FR-032, RVC-FR-033 |
| `IDustSensor` | 초기화, 종료, 먼지 감지 값 제공 | RVC-FR-025 ~ RVC-FR-029, RVC-FR-032, RVC-FR-033 |

### 8.2 Actuator 인터페이스

| 인터페이스 | 주요 동작 | 관련 요구사항 |
|---|---|---|
| `IMovementMotor` | 정지, 전진, 후진, 좌회전, 우회전 명령 | RVC-FR-003, RVC-FR-008 ~ RVC-FR-016, RVC-FR-030 |
| `ICleaningMotor` | OFF, NORMAL, POWER_UP 흡입 명령 | RVC-FR-017 ~ RVC-FR-029, RVC-FR-031 |

### 8.3 State 패턴 인터페이스

`RvcController`는 현재 이동 상태를 나타내는 `IRvcState` 객체 하나를 보유한다. 사용자 입력, 전방 장애물 interrupt, 먼지 감지, 주기적 tick 이벤트는 모두 현재 상태 객체의 동일한 인터페이스로 위임된다.

| 상태 클래스 | 주요 책임 | 관련 요구사항 |
|---|---|---|
| `OffState` | 청소 시작 시 센서 초기화, NORMAL 흡입, 전진 상태 전이 시작 | RVC-FR-001, RVC-FR-021, RVC-FR-022, RVC-FR-032 |
| `ForwardState` | 전방 장애물 interrupt와 전진 중 먼지 감지 처리 | RVC-FR-003, RVC-FR-004, RVC-FR-025 |
| `StoppedForObstacleState` | 측면 센서 값을 읽고 회피 전략에 따라 회전 또는 후진으로 전이 | RVC-FR-008 ~ RVC-FR-011 |
| `TurningState` | 좌회전/우회전 명령 후 10초 경과 시 전진으로 전이 | RVC-FR-015, RVC-FR-016 |
| `BackwardState` | 후진 중 측면 센서를 polling하고 장애물이 해제된 방향으로 전이 | RVC-FR-011 ~ RVC-FR-014 |

### 8.4 Strategy 패턴 인터페이스

`IObstacleAvoidanceStrategy`는 장애물 발견 후 회피가 필요할 때 후진 필요 여부와 회전 방향을 결정한다. 기본 전략은 `LeftPriorityAvoidanceStrategy`이며, 요구사항의 좌회전 우선 규칙을 반영한다.

| 전략 메서드 | 결정 내용 | 관련 요구사항 |
|---|---|---|
| `decideOnFrontObstacle(sideSnapshot)` | 전방 장애물 감지 후 좌우 센서 조합에 따라 좌회전, 우회전, 후진 중 하나를 결정 | RVC-FR-008 ~ RVC-FR-011 |
| `decideWhileBackward(previousSideSnapshot, currentSideSnapshot)` | 후진 중 좌우 장애물 해제 상태에 따라 좌회전, 우회전, 후진 유지 중 하나를 결정 | RVC-FR-012 ~ RVC-FR-014 |

## 9. 이동 상태 머신

이동 상태 머신은 State 패턴으로 구현 가능한 형태로 설계한다. 상태 다이어그램의 각 상태는 `IRvcState`의 구체 상태 클래스에 대응한다.

### 9.1 이동 상태

| 상태 | 의미 |
|---|---|
| `Off` | 시스템 OFF 상태 |
| `Forward` | 전진 청소 상태 |
| `StoppedForObstacle` | 전방 장애물 감지 후 정지한 상태 |
| `TurningLeft` | 좌회전 중 상태 |
| `TurningRight` | 우회전 중 상태 |
| `Backward` | 삼방향 장애물 감지 후 후진 중 상태 |

### 9.2 이동 상태 다이어그램

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    [*] --> Off
    Off --> Forward : startCleaning / initialize sensors, NORMAL, moveForward
    Forward --> Off : stopCleaning / stop, OFF, shutdown sensors

    Forward --> StoppedForObstacle : front obstacle interrupt / stop, NORMAL
    StoppedForObstacle --> TurningLeft : front only OR front+right / turnLeft
    StoppedForObstacle --> TurningRight : front+left / turnRight
    StoppedForObstacle --> Backward : front+left+right / moveBackward

    Backward --> TurningLeft : left cleared OR both cleared / turnLeft
    Backward --> TurningRight : right cleared / turnRight

    TurningLeft --> Forward : 10 seconds elapsed / moveForward
    TurningRight --> Forward : 10 seconds elapsed / moveForward

    StoppedForObstacle --> Off : stopCleaning
    TurningLeft --> Off : stopCleaning
    TurningRight --> Off : stopCleaning
    Backward --> Off : stopCleaning
@enduml
```

### 9.3 종료 입력 설계 보류

`RVC-TBD-004`에 따라 회전, 후진, POWER_UP 중 청소 종료 버튼 입력이 발생했을 때 즉시 OFF로 전환하는지 여부는 요구사항에서 아직 확정되지 않았다. 본 설계 다이어그램은 안전한 기본 후보로 모든 이동 상태에서 `stopCleaning` 이벤트를 받을 수 있게 표현했지만, 최종 구현 전 요구사항 확정이 필요하다.

## 10. 청소 흡입 상태 머신

### 10.1 청소 흡입 상태

| 상태 | 의미 |
|---|---|
| `CleaningOff` | 전체 시스템 OFF로 인해 흡입 정지 |
| `Normal` | 기본 흡입 상태 |
| `PowerUp` | 먼지 감지 후 3초 유지되는 강화 흡입 상태 |

### 10.2 청소 흡입 상태 다이어그램

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    [*] --> CleaningOff
    CleaningOff --> Normal : startCleaning / cleaningMotor.normal
    Normal --> CleaningOff : stopCleaning / cleaningMotor.off
    PowerUp --> CleaningOff : stopCleaning / cleaningMotor.off

    Normal --> PowerUp : dust detected while Forward / cleaningMotor.powerUp, start 3s timer
    PowerUp --> PowerUp : 3s elapsed and dust detected / restart 3s timer
    PowerUp --> Normal : 3s elapsed and no dust / cleaningMotor.normal

    Normal --> Normal : dust detected while stopped/turning/backward / mark pendingPowerUp
    Normal --> PowerUp : movement returns to Forward and pendingPowerUp / cleaningMotor.powerUp, start 3s timer

    PowerUp --> Normal : movement leaves Forward / cleaningMotor.normal
@enduml
```

## 11. 주요 상호작용 설계

### 11.1 SD-001 청소 시작

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    actor "사용자" as User
    participant "RvcController" as Controller
    participant "IFrontObstacleSensor" as Front
    participant "ISideObstacleSensor" as Side
    participant "IDustSensor" as Dust
    participant "CleaningManager" as Cleaning
    participant "MovementManager" as Move

    User -> Controller : startCleaning()
    Controller -> Front : initialize()
    Controller -> Side : initialize()
    Controller -> Dust : initialize()
    Controller -> Front : registerInterruptHandler(...)
    Controller -> Cleaning : start()
    Controller -> Move : moveForward()
@enduml
```

[수정] 오른쪽 센서 삭제 revision 반영 후 SD-001 청소 시작

```plantuml
@startuml
actor "사용자" as User
actor "전방 센서" as FrontHw
actor "측면 센서\n(좌측 polling)" as SideHw
actor "먼지 센서" as DustHw
participant "RvcController" as RVC
participant "OffState" as Off
participant "IFrontObstacleSensor" as Front
participant "ISideObstacleSensor" as Side
participant "IDustSensor" as Dust
participant "CleaningManager" as CleanMgr
participant "MovementManager" as MoveMgr
actor "Cleaning Motor" as CleanHw
actor "Movement Motor" as MoveHw

User -> RVC : startCleaning()
RVC -> Off : startCleaning(context)
Off -> Front : initialize()
Front -> FrontHw : init
Off -> Side : initialize()
Side -> SideHw : init
Off -> Dust : initialize()
Dust -> DustHw : init
Off -> Front : registerInterruptHandler(handler)
Off -> RVC : startForwardCleaning()
RVC -> CleanMgr : onMovementStateChanged(Forward)
CleanMgr -> CleanHw : clean
RVC -> MoveMgr : moveForward()
MoveMgr -> MoveHw : on
Off -> RVC : changeState(ForwardState)
@enduml
```

### 11.2 SD-002 청소 종료

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    actor "사용자" as User
    participant "RvcController" as Controller
    participant "MovementManager" as Move
    participant "CleaningManager" as Cleaning
    participant "IFrontObstacleSensor" as Front
    participant "ISideObstacleSensor" as Side
    participant "IDustSensor" as Dust

    User -> Controller : stopCleaning()
    Controller -> Move : stop()
    Controller -> Cleaning : stop()
    Controller -> Front : shutdown()
    Controller -> Side : shutdown()
    Controller -> Dust : shutdown()
    Controller --> Controller : movementState = Off
@enduml
```

[수정] 오른쪽 센서 삭제 revision 반영 후 SD-002 청소 종료

```plantuml
@startuml
actor "사용자" as User
actor "전방 센서" as FrontHw
actor "측면 센서\n(좌측 polling)" as SideHw
actor "먼지 센서" as DustHw
participant "RvcController" as RVC
participant "Current IRvcState" as State
participant "MovementManager" as MoveMgr
participant "CleaningManager" as CleanMgr
participant "IFrontObstacleSensor" as Front
participant "ISideObstacleSensor" as Side
participant "IDustSensor" as Dust
actor "Movement Motor" as MoveHw
actor "Cleaning Motor" as CleanHw

User -> RVC : stopCleaning()
RVC -> RVC : performStop()
RVC -> RVC : stopMovementAndCleaning()
RVC -> MoveMgr : stop()
MoveMgr -> MoveHw : off
RVC -> CleanMgr : stop()
CleanMgr -> CleanHw : stop
RVC -> Front : shutdown()
Front -> FrontHw : off
RVC -> Side : shutdown()
Side -> SideHw : off
RVC -> Dust : shutdown()
Dust -> DustHw : off
RVC -> RVC : currentState = OffState
@enduml
```

### 11.3 SD-003 전방 장애물 회피

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    participant "IFrontObstacleSensor" as Front
    participant "RvcController" as Controller
    participant "ISideObstacleSensor" as Side
    participant "IObstacleAvoidanceStrategy" as Strategy
    participant "CleaningManager" as Cleaning
    participant "MovementManager" as Move

    Front -> Controller : onFrontObstacleDetected()
    Controller -> Move : stop()
    Controller -> Cleaning : onMovementStateChanged(StoppedForObstacle)
    Controller -> Side : read()
    Side --> Controller : sideSnapshot
    Controller -> Strategy : decideOnFrontObstacle(sideSnapshot)
    Strategy --> Controller : turnLeft or turnRight or backward
    Controller -> Move : selected movement command
@enduml
```

[수정] 오른쪽 센서 삭제 revision 반영 후 SD-003 전방 장애물 회피

```plantuml
@startuml
actor "전방 센서" as FrontHw
actor "측면 센서\n(좌측 polling)" as SideHw
participant "IFrontObstacleSensor" as Front
participant "RvcController" as RVC
participant "ForwardState" as Forward
participant "StoppedForObstacleState" as Stopped
participant "TurningState" as Turning
participant "ISideObstacleSensor" as Side
participant "IObstacleAvoidanceStrategy" as Strategy
participant "RightPathCheckState" as RightCheck
participant "ReturnFromRightCheckState" as Return
participant "CleaningManager" as CleanMgr
participant "MovementManager" as MoveMgr
actor "Cleaning Motor" as CleanHw
actor "Movement Motor" as MoveHw

FrontHw -> Front : front obstacle interrupt
Front -> RVC : interrupt handler()
RVC -> Forward : onFrontObstacleDetected(context)
Forward -> RVC : stopMovementAndCleaning()
RVC -> MoveMgr : stop()
MoveMgr -> MoveHw : off
RVC -> CleanMgr : stop()
CleanMgr -> CleanHw : stop
Forward -> RVC : changeState(StoppedForObstacleState)

RVC -> Stopped : onEnter(context)
Stopped -> Side : readLeft()
Side -> SideHw : poll left
SideHw --> Side : leftDetected
Side --> Stopped : leftDetected
Stopped -> Strategy : decideAfterFrontObstacle(leftDetected)
Strategy --> Stopped : action

alt action == TurnLeft
    Stopped -> RVC : changeState(TurningState)
    RVC -> Turning : onEnter(context)
    Turning -> CleanMgr : onMovementStateChanged(TurningLeft)
    CleanMgr -> CleanHw : stop
    Turning -> MoveMgr : turnLeft()
    MoveMgr -> MoveHw : left
    RVC -> Turning : tick(context)
    Turning -> MoveMgr : isTurnComplete()
    MoveMgr --> Turning : true after 4s
    Turning -> RVC : startForwardCleaning()
    RVC -> CleanMgr : onMovementStateChanged(Forward)
    CleanMgr -> CleanHw : clean
    RVC -> MoveMgr : moveForward()
    MoveMgr -> MoveHw : on
else action == CheckRightPath
    Stopped -> RVC : changeState(RightPathCheckState)
    RVC -> RightCheck : onEnter(context)
    RightCheck -> CleanMgr : onMovementStateChanged(TurningRight)
    CleanMgr -> CleanHw : stop
    RightCheck -> MoveMgr : turnRight()
    MoveMgr -> MoveHw : right
    RightCheck -> MoveMgr : isTurnComplete()
    MoveMgr --> RightCheck : true after 4s
    RightCheck -> Front : isObstacleDetected()
    Front -> FrontHw : poll front
    FrontHw --> Front : frontBlocked
    Front --> RightCheck : frontBlocked
    alt frontBlocked == false
        RightCheck -> RVC : startForwardCleaning()
        RVC -> CleanMgr : onMovementStateChanged(Forward)
        CleanMgr -> CleanHw : clean
        RVC -> MoveMgr : moveForward()
        MoveMgr -> MoveHw : on
    else frontBlocked == true
        RightCheck -> RVC : changeState(ReturnFromRightCheckState)
        RVC -> Return : onEnter(context)
        Return -> CleanMgr : onMovementStateChanged(TurningLeft)
        CleanMgr -> CleanHw : stop
        Return -> MoveMgr : turnLeft()
        MoveMgr -> MoveHw : left
        Return -> MoveMgr : isTurnComplete()
        MoveMgr --> Return : true after 4s
        Return -> RVC : changeState(BackwardState)
    end
end
@enduml
```

### 11.4 SD-004 삼방향 장애물 탈출

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    participant "RvcController" as Controller
    participant "ISideObstacleSensor" as Side
    participant "IObstacleAvoidanceStrategy" as Strategy
    participant "CleaningManager" as Cleaning
    participant "MovementManager" as Move

    Controller -> Move : stop()
    Controller -> Cleaning : onMovementStateChanged(StoppedForObstacle)
    Controller -> Side : read()
    Side --> Controller : front + left + right blocked
    Controller -> Strategy : decideOnFrontObstacle(sideSnapshot)
    Strategy --> Controller : backward
    Controller -> Move : moveBackward()
    Controller -> Cleaning : onMovementStateChanged(Backward)
    loop backward state
        Controller -> Side : read()
        Side --> Controller : currentSideSnapshot
        Controller -> Strategy : decideWhileBackward(previousSideSnapshot, currentSideSnapshot)
        alt left cleared or both cleared
            Strategy --> Controller : turnLeft
            Controller -> Move : turnLeft()
        else right cleared
            Strategy --> Controller : turnRight
            Controller -> Move : turnRight()
        else neither side cleared
            Strategy --> Controller : keepBackward
        end
    end
    Controller -> Move : isTurnComplete()
    Move --> Controller : true
    Controller -> Move : moveForward()
    Controller -> Cleaning : onMovementStateChanged(Forward)
@enduml
```

[수정] 오른쪽 센서 삭제 revision 반영 후 SD-004 삼방향 장애물 처리

```plantuml
@startuml
actor "전방 센서" as FrontHw
actor "측면 센서\n(좌측 polling)" as SideHw
participant "RvcController" as RVC
participant "BackwardState" as Backward
participant "TurningState" as Turning
participant "ISideObstacleSensor" as Side
participant "IObstacleAvoidanceStrategy" as Strategy
participant "RightPathCheckState" as RightCheck
participant "ReturnFromRightCheckState" as Return
participant "IFrontObstacleSensor" as Front
participant "CleaningManager" as CleanMgr
participant "MovementManager" as MoveMgr
actor "Cleaning Motor" as CleanHw
actor "Movement Motor" as MoveHw

RVC -> Backward : onEnter(context)
Backward -> CleanMgr : onMovementStateChanged(Backward)
CleanMgr -> CleanHw : stop
Backward -> MoveMgr : moveBackward()
MoveMgr -> MoveHw : backward

loop until left clear or right path clear
    RVC -> Backward : tick(context)
    Backward -> RVC : stopMovementAndCleaning()
    RVC -> MoveMgr : stop()
    MoveMgr -> MoveHw : off
    RVC -> CleanMgr : stop()
    CleanMgr -> CleanHw : stop
    Backward -> Side : readLeft()
    Side -> SideHw : poll left
    SideHw --> Side : leftDetected
    Side --> Backward : leftDetected
    Backward -> Strategy : decideAfterBackwardTick(leftDetected)
    Strategy --> Backward : action

    alt action == TurnLeft
        Backward -> RVC : changeState(TurningState)
        RVC -> Turning : onEnter(context)
        Turning -> CleanMgr : onMovementStateChanged(TurningLeft)
        CleanMgr -> CleanHw : stop
        Turning -> MoveMgr : turnLeft()
        MoveMgr -> MoveHw : left
        RVC -> Turning : tick(context)
        Turning -> MoveMgr : isTurnComplete()
        MoveMgr --> Turning : true after 4s
        Turning -> RVC : startForwardCleaning()
        RVC -> CleanMgr : onMovementStateChanged(Forward)
        CleanMgr -> CleanHw : clean
        RVC -> MoveMgr : moveForward()
        MoveMgr -> MoveHw : on
    else action == CheckRightPath
        Backward -> RVC : changeState(RightPathCheckState)
        RVC -> RightCheck : onEnter(context)
        RightCheck -> CleanMgr : onMovementStateChanged(TurningRight)
        CleanMgr -> CleanHw : stop
        RightCheck -> MoveMgr : turnRight()
        MoveMgr -> MoveHw : right
        RightCheck -> MoveMgr : isTurnComplete()
        MoveMgr --> RightCheck : true after 4s
        RightCheck -> Front : isObstacleDetected()
        Front -> FrontHw : poll front
        FrontHw --> Front : frontBlocked
        Front --> RightCheck : frontBlocked
        alt frontBlocked == false
            RightCheck -> RVC : startForwardCleaning()
            RVC -> CleanMgr : onMovementStateChanged(Forward)
            CleanMgr -> CleanHw : clean
            RVC -> MoveMgr : moveForward()
            MoveMgr -> MoveHw : on
        else frontBlocked == true
            RightCheck -> RVC : changeState(ReturnFromRightCheckState)
            RVC -> Return : onEnter(context)
            Return -> CleanMgr : onMovementStateChanged(TurningLeft)
            CleanMgr -> CleanHw : stop
            Return -> MoveMgr : turnLeft()
            MoveMgr -> MoveHw : left
            Return -> MoveMgr : isTurnComplete()
            MoveMgr --> Return : true after 4s
            Return -> RVC : changeState(BackwardState)
        end
    end
end
@enduml
```

### 11.5 SD-005 POWER_UP 유지

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    participant "RvcController" as Controller
    participant "CleaningManager" as Cleaning
    participant "IDustSensor" as Dust
    participant "ICleaningMotor" as Clean
    participant "ITimer" as Timer

    Controller -> Cleaning : onDustDetected(Forward)
    Cleaning -> Clean : powerUp()
    Cleaning -> Timer : start(3 seconds)
    Controller -> Cleaning : tick()
    Cleaning -> Timer : expired()
    Timer --> Cleaning : true
    Cleaning -> Dust : isDustDetected()
    alt dust detected
        Cleaning -> Timer : start(3 seconds)
    else no dust
        Cleaning -> Clean : normal()
    end
@enduml
```

[수정] 오른쪽 센서 삭제 revision 반영 후 SD-005 먼지 감지에 따른 흡입 강화

```plantuml
@startuml
actor "먼지 센서" as DustHw
participant "RvcController" as RVC
participant "Current IRvcState" as State
participant "CleaningManager" as CleanMgr
participant "IDustSensor" as Dust
participant "ITimer" as Timer
actor "Cleaning Motor" as CleanHw

RVC -> State : tick(context)
RVC -> State : movementState()
State --> RVC : movementState
RVC -> CleanMgr : tick(movementState)

alt movementState == Forward
    CleanMgr -> Dust : isDustDetected()
    Dust -> DustHw : poll
    DustHw --> Dust : dustDetected
    Dust --> CleanMgr : dustDetected
    alt dustDetected and currentState != PowerUp
        CleanMgr -> Timer : start(3s)
        CleanMgr -> CleanHw : powerup
    else currentState == PowerUp and timer not expired
        CleanMgr -> CleanMgr : keep PowerUp
    else currentState == PowerUp and timer expired and dustDetected
        CleanMgr -> Timer : start(3s)
    else currentState == PowerUp and timer expired and no dust
        CleanMgr -> Timer : reset()
        CleanMgr -> CleanHw : clean
    end
else movementState != Forward
    CleanMgr -> Timer : reset()
    CleanMgr -> CleanHw : stop
end
@enduml
```

### 11.6 State 이벤트 위임 구조

(mermaid -> plantuml로 형식만 수정)

```plantuml
@startuml
    participant "외부 이벤트" as External
    participant "RvcController" as Controller
    participant "IRvcState" as State
    participant "IRvcState" as NextState

    External -> Controller : start/stop/frontObstacle/dust/tick
    Controller -> State : same event method(context)
    State --> Controller : action result and optional next state
    alt state transition required
        Controller -> NextState : onEnter(context)
        Controller --> Controller : currentState = NextState
    else state unchanged
        Controller --> Controller : keep currentState
    end
@enduml
```

## 12. 요구사항-설계 추적표

| 요구사항 | 설계 요소 |
|---|---|
| RVC-FR-001, RVC-FR-021, RVC-FR-022 | `RvcController.startCleaning`, SD-001 |
| RVC-FR-002, RVC-FR-020 | `RvcController.stopCleaning`, 이동/청소 흡입 상태의 `Off` 전이, SD-002 |
| RVC-FR-003 | 이동 상태 `Forward`, `MovementManager.moveForward` |
| RVC-FR-004 ~ RVC-FR-007 | `IFrontObstacleSensor`, `ISideObstacleSensor`, `IObstacleAvoidanceStrategy`, SD-003, SD-004 |
| RVC-FR-008 ~ RVC-FR-016 | 이동 상태 머신, `IRvcState`, `IObstacleAvoidanceStrategy`, `MovementManager`, `IMovementMotor`, SD-003, SD-004 |
| RVC-FR-017 ~ RVC-FR-029 | 청소 흡입 상태 머신, `CleaningManager`, `ICleaningMotor`, `ITimer`, SD-005 |
| RVC-FR-030 | `MovementManager`, `IMovementMotor` |
| RVC-FR-031 | `CleaningManager`, `ICleaningMotor` |
| RVC-FR-032, RVC-FR-033 | 센서 인터페이스 `initialize`, `shutdown`, SD-001, SD-002 |
| RVC-NFR-001 | 모든 외부 하드웨어 의존성은 인터페이스 뒤에 배치 |
| RVC-NFR-003 | Sensor Interfaces, Controller Core, Motor Interfaces, Movement Management, Cleaning Management 분리 |

## 13. 미확정 요구사항의 설계 영향

| 미정 항목 | 설계 영향 |
|---|---|
| RVC-TBD-001 후진 중 좌우 센서 polling 주기 | `Backward` 상태에서 `tick()` 또는 주기 이벤트로 처리 가능하도록 열어둔다. |
| RVC-TBD-002 후진 장시간 지속 예외 처리 | 현재 상태 머신에는 timeout 전이를 넣지 않는다. 향후 `Backward` 상태에 timeout 전이를 추가할 수 있다. |
| RVC-TBD-003 360도 회전 탈출 기능 | 현재 설계에는 포함하지 않는다. 향후 별도 이동 상태로 확장 가능하다. |
| RVC-TBD-004 종료 입력 우선순위 | 설계 다이어그램에는 모든 이동 상태에서 종료 이벤트 후보를 표시했으나, 구현 전 최종 확정이 필요하다. |

## 14. 오른쪽 센서 삭제 Revision 설계 반영 [추가]

### 14.1 현재 단계 및 설계 범위 [수정]

현재 단계는 Design (Refactoring & Update)이다. 본 문서는 승인된 Requirements 및 Analysis 산출물을 바탕으로 legacy `rvc-controller` 설계를 수정한다.

- [수정] 설계 대상은 여전히 `rvc-controller` 내부 구조이다.
- [유지] 사용자, 전방 센서 하드웨어, 측면 센서 하드웨어, 먼지 센서 하드웨어, Movement Motor, Cleaning Motor는 외부 actor이다.
- [추가] simulator 및 검증 인프라는 controller 설계 대상이 아니며, simulator의 우측 장애물 정보는 검증 관찰 데이터로만 사용할 수 있다.
- [삭제] controller 내부 판단 입력과 내부 인터페이스에서 우측 센서 값을 제거한다.
- [추가] legacy 우측 센서 감지는 `90도 우회전 완료 후 전방 센싱`으로 대체한다.
- [수정] movement가 정지/회전/후진 상태이면 cleaning motor는 OFF이다. 직진 시작 시 cleaning motor의 최소 상태는 NORMAL이다.

### 14.2 주요 설계 결정 변경 [수정]

| ID | 원문 설계 | Revision 설계 |
|---|---|---|
| DD-003 | 장애물 판단은 Strategy 패턴으로 분리한다. | [수정] Strategy는 좌측 polling 결과만으로 좌회전 또는 우측 경로 확인을 선택한다. 우측 경로의 막힘 여부는 우회전 후 전방 센싱으로 state machine이 판단한다. |
| DD-004 | `CleaningManager`는 회피 중 먼지 감지 지연 처리를 담당한다. | [수정][삭제] 회피 중 pending POWER_UP을 저장하지 않는다. `CleaningManager`는 전진 tick에서만 dust polling으로 POWER_UP을 판단한다. |
| DD-005 | 회전 10초, POWER_UP 3초 규칙을 `Timer`로 추상화한다. | [수정] 회전 완료 시간은 4초로 변경한다. POWER_UP 3초 규칙은 유지한다. |
| DD-006 | 이동 동작은 State 패턴으로 표현한다. | [수정] 우측 경로 확인을 위해 `RightPathCheckState`, `ReturnFromRightCheckState`를 추가하는 설계로 변경한다. |

### 14.3 모듈 및 클래스 책임 변경 [수정]

| 클래스/인터페이스 | 원문 책임 | Revision 책임 |
|---|---|---|
| `StoppedForObstacleState` | 측면 센서 값을 읽고 회피 전략 결과에 따라 상태 전이 | [수정] 좌측 센서를 polling한다. 좌측이 비어 있으면 좌회전, 막혀 있으면 우측 경로 확인으로 전이한다. |
| `TurningState` | 좌회전/우회전 중 10초 완료 이벤트 처리 | [수정] 일반 회전 후 전진하는 `TurningToForwardState`와 우측 경로 확인 목적의 우회전 흐름을 구분한다. 회전 시간은 4초이다. |
| `BackwardState` | 후진 중 측면 센서 polling 결과에 따른 회전 전이 | [수정] 원래 방향으로 복귀한 상태에서 1 tick 후진하고, 매 tick 좌측 polling 후 좌회전 또는 우측 경로 확인을 선택한다. |
| `RightPathCheckState` | 원문에 없음 | [추가] 90도 우회전 후 전방 센서를 polling하여 우측 경로가 열렸는지 확인한다. |
| `ReturnFromRightCheckState` | 원문에 없음 | [추가] 우측 경로가 막힌 경우 90도 좌회전으로 원래 방향에 복귀한 뒤 후진 상태로 전이한다. |
| `IObstacleAvoidanceStrategy` | 좌우 snapshot으로 후진 필요 여부와 회전 방향 결정 | [수정] 좌측 값만으로 `TurnLeft` 또는 `CheckRightPath`를 결정한다. |
| `LeftPriorityAvoidanceStrategy` | 좌회전 우선 요구사항을 반영한 기본 회피 전략 | [수정] 좌측이 비어 있으면 좌회전을 우선 선택하고, 좌측이 막힌 경우 우측 경로 확인을 선택한다. |
| `MovementManager` | movement motor 명령, 회전 10초 타이머 관리 | [수정] movement motor 명령과 4초 회전 타이머를 관리한다. |
| `CleaningManager` | POWER_UP 타이머, 회피 중 먼지 감지 지연 처리 | [수정][삭제] pending POWER_UP을 제거한다. 전진 중 dust polling, non-forward OFF, forward 진입 시 NORMAL을 관리한다. |
| `ISideObstacleSensor` | 좌측/우측 장애물 값을 polling 방식으로 제공 | [수정][삭제] controller 내부에는 좌측 장애물 polling만 제공한다. 우측 값은 노출하지 않는다. |
| `IDustSensor` | 먼지 감지 값을 제공 | [수정] 먼지 센서는 controller tick마다 polling되는 입력이다. |

### 14.4 내부 인터페이스 변경 [수정]

```cpp
class IFrontObstacleSensor {
public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void registerInterruptHandler(std::function<void()> handler) = 0;

    // [추가] 우측 경로 확인을 위한 90도 우회전 완료 후 전방 센싱.
    virtual bool isObstacleDetected() = 0;
};

class ISideObstacleSensor {
public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    // [수정][삭제] 좌측 polling만 제공한다. controller 내부에 rightDetected를 전달하지 않는다.
    virtual bool readLeft() = 0;
};

class IDustSensor {
public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    // [수정] movement state가 Forward일 때 controller tick에서 polling된다.
    virtual bool isDustDetected() = 0;
};

class CleaningManager {
public:
    void enterOff();
    void enterNormal();
    void enterPowerUp();

    // [수정] 전진 중에만 dust polling 결과로 POWER_UP을 판단한다.
    void tick(MovementState movementState);
    CleaningState currentState() const;
};

enum class AvoidanceAction {
    TurnLeft,
    CheckRightPath
};

class IObstacleAvoidanceStrategy {
public:
    virtual AvoidanceAction decideAfterFrontObstacle(bool leftDetected) const = 0;
    virtual AvoidanceAction decideAfterBackwardTick(bool leftDetected) const = 0;
};
```

[삭제] `CleaningManager`의 `pendingPowerUp` 필드와 관련 API는 제거한다.

### 14.5 이동 상태 머신 변경 [수정]

| 상태 | Revision 의미 |
|---|---|
| `Off` | 청소 세션이 시작되지 않았거나 종료된 상태 |
| `Forward` | 전진 청소 상태. cleaning motor는 최소 NORMAL이다. |
| `StoppedForObstacle` | 전방 장애물 interrupt 후 movement와 cleaning이 모두 멈춘 상태 |
| `TurningToForward` | 좌회전 후 전진하기 위한 일반 회전 상태 |
| `RightPathCheck` | [추가] 우측 경로 확인을 위해 90도 우회전하고, 완료 후 전방 센싱을 수행하는 상태 |
| `ReturnFromRightCheck` | [추가] 우측 경로가 막혀 있을 때 90도 좌회전으로 원래 방향에 복귀하는 상태 |
| `Backward` | [수정] 원래 방향으로 복귀한 상태에서 1 tick 후진하고 좌측 polling을 수행하는 상태 |

```plantuml
@startuml
[*] --> Off

Off --> Forward : startCleaning / initialize sensors,\ncleaning NORMAL, moveForward

Forward --> StoppedForObstacle : front obstacle interrupt /\nmovement stop, cleaning OFF
Forward --> Off : stopCleaning / stop movement,\ncleaning OFF, shutdown sensors

StoppedForObstacle --> TurningToForward : left polling = clear /\nturnLeft
StoppedForObstacle --> RightPathCheck : left polling = blocked /\nturnRight
StoppedForObstacle --> Off : stopCleaning

TurningToForward --> Forward : 4s elapsed /\ncleaning NORMAL, moveForward
TurningToForward --> Off : stopCleaning

RightPathCheck --> Forward : 4s elapsed and front clear /\ncleaning NORMAL, moveForward
RightPathCheck --> ReturnFromRightCheck : 4s elapsed and front blocked /\nturnLeft
RightPathCheck --> Off : stopCleaning

ReturnFromRightCheck --> Backward : 4s elapsed /\noriginal direction restored
ReturnFromRightCheck --> Off : stopCleaning

Backward --> TurningToForward : after 1 tick backward,\nleft polling = clear / turnLeft
Backward --> RightPathCheck : after 1 tick backward,\nleft polling = blocked / turnRight
Backward --> Off : stopCleaning
@enduml
```

### 14.6 RightPathCheck 상세 규칙 [추가]

| 단계 | 동작 |
|---|---|
| 1 | `RightPathCheckState::onEnter()`에서 cleaning OFF를 유지하고 movement motor에 90도 우회전 명령을 보낸다. |
| 2 | `MovementManager::isTurnComplete()`가 4초 경과를 반환할 때까지 tick을 기다린다. |
| 3 | 회전 완료 후 `IFrontObstacleSensor::isObstacleDetected()`로 전방 값을 polling한다. |
| 4 | 전방 장애물이 없으면 현재 우회전한 방향으로 전진한다. 이때 cleaning motor에 NORMAL을 먼저 보장한다. |
| 5 | 전방 장애물이 있으면 `ReturnFromRightCheckState`로 전이하고 90도 좌회전으로 원래 방향에 복귀한다. |
| 6 | 원래 방향 복귀 후 `BackwardState`에서 1 tick 후진한다. |

### 14.7 청소 흡입 상태 머신 변경 [수정]

```plantuml
@startuml
[*] --> CleaningOff

CleaningOff --> Normal : movement enters Forward /\ncleaningMotor.normal
Normal --> PowerUp : tick while Forward and dust detected /\ncleaningMotor.powerUp, start 3s timer
PowerUp --> PowerUp : 3s elapsed and dust detected /\nrestart 3s timer
PowerUp --> Normal : 3s elapsed and no dust /\ncleaningMotor.normal

Normal --> CleaningOff : movement leaves Forward or stopCleaning /\ncleaningMotor.off
PowerUp --> CleaningOff : movement leaves Forward or stopCleaning /\ncleaningMotor.off
CleaningOff --> CleaningOff : tick while non-forward / no pending POWER_UP
@enduml
```

| 규칙 | 설명 |
|---|---|
| Cleaning invariant | [추가] movement가 `Forward`가 아니면 cleaning state는 `CleaningOff`이다. |
| Forward minimum | [수정] movement가 `Forward`로 진입할 때 cleaning state는 최소 `Normal`이다. |
| Dust polling | [수정] `Forward` tick에서 먼지 센서를 polling한다. |
| Pending 제거 | [삭제] 정지/회전/후진 중 먼지 값은 이후 POWER_UP 조건으로 저장하지 않는다. |

### 14.8 요구사항-설계 추적 변경 [수정]

| 요구사항/분석 규칙 | Revision 설계 요소 |
|---|---|
| RVC-FR-035, DR-010 | [추가] `ISideObstacleSensor::readLeft()`, `rightDetected` 제거 |
| RVC-FR-009, RVC-FR-036, RVC-FR-037, DR-011 | [추가] `RightPathCheckState`, `IFrontObstacleSensor::isObstacleDetected()` |
| RVC-FR-011, DR-012 | [수정] `ReturnFromRightCheckState`, `BackwardState` |
| RVC-FR-013, RVC-CON-009, DR-013 | [수정] `BackwardState`의 1 tick 후진 및 매회 좌측 polling |
| RVC-FR-015, RVC-CON-003, DR-005 | [수정] `MovementManager` 4초 회전 타이머 |
| RVC-FR-038, DR-015 | [추가] `RvcController::stopMovementAndCleaning()`, cleaning OFF invariant |
| RVC-FR-024, DR-016 | [수정] `RvcController::startForwardCleaning()`, `CleaningManager::enterNormal()` |
| RVC-FR-039, RVC-CON-011, DR-017 | [수정] `CleaningManager::tick(MovementState)`의 dust polling |
| RVC-FR-018, RVC-FR-019, DR-018 | [삭제] `pendingPowerUp` 제거 |
| RVC-CON-007 | [추가] simulator 우측 값은 검증 관찰 데이터로만 유지 |
