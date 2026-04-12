# Module Responsibility

## 1. 공통 모듈
| 모듈 | 책임 | 비고 |
|---|---|---|
| `app/app.c` | 사용자 정의 상위 실행 흐름 연결 | generated 코드와 경계 |
| `common/button_fsm.c` | raw 버튼 입력을 debounce/click/long-press 이벤트로 변환 | polling 기반 |
| `common/log.c` | 공통 로그 레벨과 출력 형식 제공 | bare-metal/RTOS 공통 |
| `common/platform_gpio.c` | LED/버튼 GPIO 접근 캡슐화 | PC13, PA5 해석 집중 |
| `common/platform_port.c` | 시간, delay, UART 포팅 인터페이스 | HAL 의존 분리 |

## 2. RTOS 모듈
| 모듈 | 책임 | 비고 |
|---|---|---|
| `rtos/rtos_app.c` | queue/task 생성과 초기화 | 구조 조립 |
| `rtos/rtos_button.c` | 버튼 polling, FSM 갱신, queue 적재 | button task |
| `rtos/rtos_log_service.c` | queue 수신, 로그 출력, probe tick 갱신 | log service task |
| `rtos/rtos_monitor.c` | overflow, 이상 징후 감시 | 관찰 중심 |
| `rtos/rtos_probe.c` | watchdog/task/queue 관련 통계 저장 | 검증용 probe |

## 3. 테스트 런타임
| 모듈 | 책임 |
|---|---|
| `test/runtime/test_baremetal_runner.c` | 선택된 bare-metal TC 실행 |
| `test/runtime/test_rtos_runner.c` | 선택된 RTOS TC 실행 |
| `test/runtime/test_rtos_runtime.c` | RTOS 테스트 공통 상태 관리 |
| `test/runtime/test_timer.c` | timeout, 주기 판정 공통화 |
| `test/verify/toggle_verifier.c` | 예상 이벤트와 실제 LED 토글 비교 |

## 4. TC 그룹 책임
| 그룹 | 책임 |
|---|---|
| `tc_00` | bring-up / boot / clock sanity |
| `tc_01` | 기본 입력/출력 기능 |
| `tc_02` | UART와 로그 형식 |
| `tc_03` | timing / debounce / load 영향 |
| `tc_04` | 결함 재현 |
| `tc_05` | 수정 검증 |
| `tc_06` | 회귀 / 안정성 / 복구 |
| `tc_rtos` | RTOS 구조 검증 |
