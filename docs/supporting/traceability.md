# Traceability

## 1. 목적
요구 사항, 구현 모듈, TC, 결과 사이의 연결 관계를 간단히 추적하기 위한 표이다.

## 2. Traceability Matrix
| 요구/관찰 항목 | 관련 모듈 | 관련 TC | 결과 문서 |
|---|---|---|---|
| firmware boot 후 실행 지속 | app, platform_port | TC-000 | `05_test_report.md` |
| button raw 입력 감지 | platform_gpio, button_fsm | TC-011 | `05_test_report.md` |
| 1 click = 1 toggle | button_fsm, toggle_verifier | TC-012 | `05_test_report.md` |
| UART log 기본 경로 | log, usart | TC-020, TC-021 | `05_test_report.md` |
| button event ↔ UART mapping | runtime, tc_022 | TC-022, TC-052 | `05_test_report.md` |
| debounce 안정성 | button_fsm | TC-031, TC-050 | `05_test_report.md` |
| 부하 영향 | tc_032, rtos_probe | TC-032, TC-032_RTOS | `05_test_report.md` |
| UART 누락 결함 재현 | log path, runtime | TC-042, TC-042_RTOS | `05_test_report.md` |
| watchdog servicing | rtos_log_service, rtos_probe | TC-063_RTOS | `05_test_report.md` |
| safe-state/fault log 관찰 | monitor, tc_066 | TC-066, TC-066_RTOS | `05_test_report.md` |

## 3. 운영 원칙
- 새 TC 추가 시 관련 요구/관찰 항목을 같이 연결한다.
- Fix validation TC는 defect reproduction TC와 대응 관계를 남긴다.
- 문서와 실제 코드 selection 값이 달라지면 함께 갱신한다.
