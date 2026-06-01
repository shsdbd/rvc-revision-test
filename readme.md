# Team Project #6.  Vibe-Coded ver. of Modified Reqirement : Delete Right Obs Sensor


#5 레포를 그대로 가져와서 작업을 진행함.

#5에서 단계를 잘못 나눴어서 requirement랑 analysis가 같이 있던 것을 분리하였음. 

원래 레포의 docs/requirements analysis 에 있던 파일 중 domain-model, ssd 등 analysis단계의 산출물들의 refined 버전은 docs/analysis 에 저장, 원래 잘못들어있던 파일들은 비교를 위해 그대로 둠.

## 단계별 주요 변경점. 수정한 4개 단계의 모든 md파일(harness 제외)은 docs/ 에 위치. 
### Environments
- 기존 agent.md와 skills 폴더명 뒤에 _old를 붙이고 새로 쓸 agent, skill을 새로 생성.

### Requirements
- srs, usecase-description, usecase-diagram (.md) 를 수정. 
- codex가 제안한 내용-회전 횟수가 증가하므로 회전 완료 기준을 기존 10초에서 4초로 변경. 
- 장애물 회피 로직 변경
    - 조교 설명처럼 오른쪽 센서 polling을 90도 우회전 후 전방 감지로 수정.
    - 후진 시에도 1 tick마다 좌측polling-90도 우회전 루틴 구현.

### Analysis
- analysis-impact.md : 요구사항 변경의 영향을 별도로 정리
- domain-model, ssd (.md) 를 수정(한 새 파일을 새 경로에 생성).

### Design
- design-specification.md 를 수정. 
- sequence-diagram.md 를 생성해 다이어그램을 분리.

### Implementation
- PR#3의 코드들과 PR#5의 system test case 1 개 수정. PR#5의 내용은 6번 시나리오에 벽을 연장한 것인데, 시나리오를 기존 플젝과 똑같이 가져가려 한다면 되돌려도 무방함.

### Verification
- verification-summary.md 수정.

## 코드 레벨 변경점
### 센서 인터페이스

- ISideObstacleSensor가 우측 값을 제공하지 않도록 축소됨.
    - 현재는 readLeft()만 있음: include/rvc/Interfaces.hpp:22
    - legacy IObstacleSensor::isRightDetected()는 하위 호환/adapter 쪽에는 남아 있지만, 새 controller 판단 입력으로는
        쓰지 않음.

- IFrontObstacleSensor에 isObstacleDetected()가 추가/사용됨.
      - 전방 interrupt뿐 아니라, 90도 우회전 후 “오른쪽 경로가 막혔는지”를 전방 센서로 확인하는 용도: include/rvc/
        Interfaces.hpp:10

  - legacy adapter도 이에 맞춰 분리됨.
      - CombinedFrontObstacleSensorAdapter::isObstacleDetected()는 legacy front 값을 읽음.
      - CombinedSideObstacleSensorAdapter::readLeft()는 left 값만 읽음: src/LegacyAdapters.cpp:22

### 장애물 회피 Strategy

  - AvoidanceAction이 기존 좌/우/후진 직접 결정 방식이 아니라 TurnLeft, CheckRightPath 중심으로 바뀜: include/rvc/
    Types.hpp:31

  - IObstacleAvoidanceStrategy는 좌측 센서 값만 받아 판단함.
      - decideAfterFrontObstacle(bool leftDetected)
      - decideAfterBackwardTick(bool leftDetected): include/rvc/IObstacleAvoidanceStrategy.hpp:7

  - 구현체 LeftPriorityAvoidanceStrategy는 좌측이 비어 있으면 좌회전, 좌측이 막혀 있으면 우측 경로 확인으로 보냄: src/
    LeftPriorityAvoidanceStrategy.cpp:5

### State Machine

  - StoppedForObstacleState
      - 전방 장애물로 멈춘 뒤 readLeft()만 수행.
      - 좌측 막힘이면 RightPathCheckState, 아니면 좌회전: src/RvcStates.cpp:40

  - RightPathCheckState 추가
      - 90도 우회전.
      - 회전 완료 후 frontSensor().isObstacleDetected()로 우측 경로 확인.
      - 막혀 있으면 ReturnFromRightCheckState, 안 막혀 있으면 전진 복귀: src/RvcStates.cpp:81

  - ReturnFromRightCheckState 추가
      - 우측 경로가 막힌 경우 90도 좌회전해서 원래 방향으로 복귀.
      - 완료 후 BackwardState 진입: src/RvcStates.cpp:107

  - BackwardState 수정
      - 진입 시 후진.
      - 다음 tick에서 movement/cleaning stop 후 좌측 polling.
      - 좌측이 비면 좌회전, 계속 막혀 있으면 다시 RightPathCheckState: src/RvcStates.cpp:127

### Movement / Cleaning 연동

  - controller에 stopMovementAndCleaning()이 생겨 movement stop 시 cleaning도 같이 OFF 처리함: src/RvcController.cpp:104
  - startForwardCleaning()은 cleaning을 forward 상태로 바꾸고 motor forward를 시작함: src/RvcController.cpp:109
  - CleaningManager는 비전진 상태면 즉시 OFF, 전진 상태에서만 dust polling으로 NORMAL/POWER_UP 판단: src/
    CleaningManager.cpp:74

### Timing

  - 회전 완료 기준이 10초에서 4초로 변경됨.
      - kDefaultTurnDuration{4000}: include/rvc/Types.hpp:50