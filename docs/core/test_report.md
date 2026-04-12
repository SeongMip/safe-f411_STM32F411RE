# Test Report

## 1. 문서 목적
본 문서는 대표 TC 실행 결과를 정리하기 위한 결과서 초안이다.

## 2. 시험 개요
- Project: safe-f411
- Board: NUCLEO-F411RE
- Test Scope: bring-up, button/LED/UART, defect reproduction, RTOS validation
- Result Format: PASS / FAIL / observation note

## 3. 결과 요약
| TC | 제목 | 결과 | 근거 |
|---|---|---|---|
| TC-000 | Firmware execution verification | PASS | boot 후 heartbeat와 기본 실행 흐름 확인 |
| TC-012 | Single click toggle | TBD | 실제 실행 로그/캡처 반영 필요 |
| TC-042 | UART event omission defect | TBD | 결함 재현 로그 반영 필요 |
| TC-063_RTOS | Watchdog servicing validation | TBD | probe 관찰 로그 반영 필요 |

## 4. 상세 결과 예시
### TC-000
- 환경: bare-metal
- 결과: PASS
- 관찰:
  - firmware download 성공
  - boot 후 heartbeat 유지
- 비고:
  - bring-up 기준 최소 실행 가능 여부 확인 완료

### TC-012
- 환경: bare-metal
- 결과: TBD
- 관찰:
  - UART 로그 첨부 필요
  - LED toggle 캡처 필요
- 판정:
  - 1 click당 1 toggle이면 PASS
  - duplicate toggle 또는 미검출이면 FAIL

### TC-063_RTOS
- 환경: RTOS
- 결과: TBD
- 관찰:
  - watchdog feed count
  - max gap
  - timeout 여부
- 판정:
  - feed 존재 및 허용 간격 이내면 PASS
  - feed 없음 또는 gap 초과 시 FAIL

## 5. 첨부 위치
- 로그: `docs/assets/logs/`
- 캡처: `docs/assets/screenshots/`
- 결과 파일: `docs/assets/test-results/`
