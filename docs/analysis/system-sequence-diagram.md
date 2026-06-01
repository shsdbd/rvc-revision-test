# RVC System Sequence Diagrams

## 현재 단계

현재 단계는 Analysis (Revision)이다. 본 문서는 외부 actor와 `rvc-controller` 사이의 시스템 수준 이벤트 순서를 표현한다. `rvc-controller`는 black box로 취급하며, 내부 클래스나 내부 메서드 호출은 표현하지 않는다.

## SSD-001 자동 진공 청소 시작

```plantuml
@startuml
actor "사용자" as User
actor "전방 센서" as Front
actor "측면 센서\n(좌측 polling)" as Side
actor "먼지 센서" as Dust
participant "rvc-controller" as RVC
actor "Movement Motor" as Move
actor "Cleaning Motor" as Clean

User -> RVC : 청소 시작 버튼 입력
RVC -> RVC : OFF에서 전원 ON 상태로 전환
RVC -> Front : 센서 인터페이스 초기화
RVC -> Side : 센서 인터페이스 초기화
RVC -> Dust : 센서 인터페이스 초기화
RVC -> Clean : NORMAL 흡입 명령
RVC -> Move : 전진 명령
@enduml
```

## SSD-002 자동 진공 청소 종료

```plantuml
@startuml
actor "사용자" as User
actor "전방 센서" as Front
actor "측면 센서\n(좌측 polling)" as Side
actor "먼지 센서" as Dust
participant "rvc-controller" as RVC
actor "Movement Motor" as Move
actor "Cleaning Motor" as Clean

User -> RVC : 청소 종료 버튼 입력
RVC -> RVC : [수정] 현재 RVC state에 정의된 종료 절차 시작\n수정 전: 청소 동작 종료
RVC -> Move : 현재 RVC state에 정의된 movement motor 종료 명령
RVC -> Clean : 현재 RVC state에 정의된 cleaning motor 종료 명령
RVC -> Front : 센서 인터페이스 종료
RVC -> Side : 센서 인터페이스 종료
RVC -> Dust : 센서 인터페이스 종료
RVC -> RVC : OFF 상태로 전환
@enduml
```

## SSD-003 전방 장애물 회피

```plantuml
@startuml
actor "전방 센서" as Front
actor "측면 센서\n(좌측 polling)" as Side
participant "rvc-controller" as RVC
actor "Movement Motor" as Move
actor "Cleaning Motor" as Clean

Front -> RVC : 전방 장애물 interrupt
RVC -> Move : 정지 명령
RVC -> Clean : [수정] OFF 흡입 명령\n수정 전: NORMAL 흡입 명령
RVC -> Side : [수정] 좌측 장애물 값 요청\n수정 전: 좌우 장애물 값 요청
Side --> RVC : [수정] 좌측 장애물 값 반환\n수정 전: 좌우 장애물 값 반환

alt 전방 장애물만 감지
    RVC -> Move : 좌회전 명령
    RVC -> RVC : [수정] 4초 후 회전 완료 간주\n수정 전: 10초
    RVC -> Clean : [추가] NORMAL 흡입 명령
    RVC -> Move : 전진 명령
else 전방 + 좌측 장애물 감지
    RVC -> Move : [수정] 우측 경로 확인을 위한 90도 우회전 명령\n수정 전: 우회전 명령 후 전진
    RVC -> RVC : 4초 후 회전 완료 간주
    RVC -> Front : [추가] 전방 장애물 값 요청
    Front --> RVC : [추가] 우회전 완료 후 전방 장애물 값 반환
    alt 우회전 후 전방 장애물 미감지
        RVC -> Clean : [추가] NORMAL 흡입 명령
        RVC -> Move : 우회전한 방향으로 전진 명령
    else 우회전 후 전방 장애물 감지
        RVC -> Move : 90도 좌회전 명령
        RVC -> RVC : 4초 후 원래 방향 복귀 완료 간주
        RVC -> RVC : UC-004 삼방향 장애물 처리 조건 성립
    end
else [삭제] 전방 + 우측 장애물 직접 감지
    RVC -> RVC : 직접적인 우측 센서 판단은 수행하지 않음
end
@enduml
```

## SSD-004 삼방향 장애물 처리

```plantuml
@startuml
actor "전방 센서" as Front
actor "측면 센서\n(좌측 polling)" as Side
participant "rvc-controller" as RVC
actor "Movement Motor" as Move
actor "Cleaning Motor" as Clean

RVC -> RVC : [수정] 우측 경로 확인 결과 막힘 + 원래 방향 복귀 상태 확인\n수정 전: 전방/좌측/우측 장애물 직접 감지
RVC -> Move : 정지 명령
RVC -> Clean : [수정] OFF 흡입 명령\n수정 전: NORMAL 흡입 명령

loop 삼방향 장애물 처리 반복
    RVC -> Move : [수정] 1 tick 후진 명령\n수정 전: 후진 명령 지속
    RVC -> Side : [수정] 좌측 장애물 값 요청\n수정 전: 좌우 장애물 값 요청
    Side --> RVC : [수정] 좌측 장애물 값 반환\n수정 전: 좌우 장애물 값 반환

    alt 좌측 장애물 미감지
        RVC -> Move : 90도 좌회전 명령
        RVC -> RVC : 4초 후 회전 완료 간주
        RVC -> Clean : [추가] NORMAL 흡입 명령
        RVC -> Move : 전진 명령
    else 좌측 장애물 계속 감지
        RVC -> Move : [수정] 우측 경로 확인을 위한 90도 우회전 명령\n수정 전: 우측 장애물 해제 시 우회전
        RVC -> RVC : 4초 후 회전 완료 간주
        RVC -> Front : [추가] 전방 장애물 값 요청
        Front --> RVC : [추가] 우회전 완료 후 전방 장애물 값 반환

        alt 우회전 후 전방 장애물 미감지
            RVC -> Clean : [추가] NORMAL 흡입 명령
            RVC -> Move : 우회전한 방향으로 전진 명령
        else 우회전 후 전방 장애물 감지
            RVC -> Move : 90도 좌회전 명령
            RVC -> RVC : 4초 후 원래 방향 복귀 완료 간주
        end
    end
end

note over RVC
후진 상태가 장시간 지속되는 경우의 예외 처리는
현재 요구사항 범위에서 고려하지 않는다.
end note
@enduml
```

## SSD-005 먼지 감지에 따른 흡입 강화

```plantuml
@startuml
actor "먼지 센서" as Dust
participant "rvc-controller" as RVC
actor "Cleaning Motor" as Clean

RVC -> Dust : [수정] 먼지 감지 값 polling\n수정 전: 먼지 감지 이벤트 수신
Dust --> RVC : [추가] 먼지 감지 값 반환
alt RVC 전진 중
    RVC -> Clean : POWER_UP 흡입 명령
    RVC -> RVC : 3초 유지
    RVC -> Dust : 1 tick마다 먼지 값 재확인 polling
    Dust --> RVC : 먼지 감지 값 반환
    alt 먼지 재감지
        RVC -> Clean : POWER_UP 흡입 명령 유지
        RVC -> RVC : 3초 타이머 재시작
    else 먼지 미감지
        RVC -> Clean : NORMAL 흡입 명령
    end
else RVC 정지/회전/후진 중
    RVC -> Clean : [수정] OFF 흡입 명령 유지\n수정 전: NORMAL 흡입 명령 유지
    RVC -> RVC : 먼지 값을 POWER_UP 진입 조건으로 저장하지 않음
end
@enduml
```
