# Architecture

## 1. 구조 요약
safe-f411은 보드 종속 접근, 입력 판정, 시험 실행, RTOS 관찰 기능을 분리한 layered test project로 정리되어 있다.

## 2. 상위 구조
```text
main / freertos
 ├─ app
 ├─ common
 │   ├─ platform_gpio
 │   ├─ platform_port
 │   ├─ log
 │   └─ button_fsm
 ├─ test
 │   ├─ runtime
 │   ├─ verify
 │   └─ tc_00 ~ tc_06 / tc_rtos
 └─ rtos
     ├─ rtos_app
     ├─ rtos_button
     ├─ rtos_log_service
     ├─ rtos_monitor
     └─ rtos_probe
```

## 3. 계층별 의도
### app
- 시스템 초기화 후 프로젝트 고유 실행 흐름 진입점 역할
- CubeMX generated 코드와 사용자 동작 경계 유지

### common
- HAL 세부사항과 시험 로직을 직접 결합하지 않기 위한 계층
- 버튼 raw 입력, LED, UART, 시간 포팅 인터페이스 제공

### test/runtime
- 선택된 TC를 실행하고 `TEST_IN_REVIEW`를 최종 `PASS/FAIL`로 수렴
- bare-metal과 RTOS 실행 모델 차이를 runtime 레벨에서 분리

### test/verify
- 예상 이벤트 수와 실제 반응 차이를 정량 검증
- FAIL 원인을 click 미검출, 중복 toggle, long 오판정 등으로 분리

### rtos
- 버튼 입력 처리와 로그 출력을 task/queue로 분리
- 지연, 누락, overflow, watchdog servicing을 관찰 포인트로 사용

## 4. RTOS 구조 핵심 값
- `buttonTask`: AboveNormal priority
- `logServiceTask`: Normal priority
- `buttonEventQueue`: depth 8
- `rtos_log_service` local queue depth: 16
- button debounce: 30ms
- button release debounce: 30ms
- long-press threshold: 1500ms

## 5. bare-metal과 RTOS를 둘 다 두는 이유
### bare-metal
- 단순한 흐름에서 기능과 결함을 빠르게 재현
- 초기 bring-up, 기본 기능 검증, 결함 재현용으로 유리

### RTOS
- 버튼 처리와 UART 출력 경로 분리
- queue 기반 backlog / overflow / processing latency 관찰
- watchdog feed와 scheduler 영향 검증

## 6. 이벤트 흐름
### bare-metal
```text
raw button read
 -> button_fsm update
 -> event 판정
 -> 테스트 로직에서 LED/UART 검증
```

### RTOS
```text
buttonTask
 -> button_fsm update
 -> buttonEventQueue 적재
 -> logServiceTask 수신
 -> 로그/프로브 갱신
 -> monitor/test 관찰
```

## 7. 설계 원칙
- 보드 의존 접근은 common 계층에 모은다
- 시험 코드는 절차보다 판정 기준을 우선한다
- RTOS에서는 ISR 대신 task 문맥 처리 구조를 우선 유지한다
- queue overflow와 watchdog feed는 기능 제어가 아니라 관찰 데이터로 본다
