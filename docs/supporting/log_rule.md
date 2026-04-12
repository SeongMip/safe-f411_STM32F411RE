# Log Rule

## 1. 목적
safe-f411의 시험 로그 형식과 출력 규칙을 통일한다.

## 2. 공통 형식
```text
[LEVEL] [TC-ID] message
```

## 3. LEVEL
- INFO
- WARN
- ERROR
- PASS
- FAIL

## 4. 작성 원칙
- 모든 테스트는 `START` 로그로 시작한다.
- 관찰값은 `OBSERVE` 또는 의미 있는 상태 메시지로 남긴다.
- 최종 결과는 `PASS` 또는 `FAIL`이 포함되게 출력한다.
- 실패 메시지는 원인이 보이게 작성한다.
- 단순 코드 번역형 로그는 피한다.

## 5. 예시
```text
[INFO] [TC-012] START single click toggle validation
[INFO] [TC-012] OBSERVE expected=1 actual=1
[PASS] [TC-012] single click toggled exactly once
[FAIL] [TC-012] duplicate toggle detected
TC-012 example
[INFO] [TC-012] START single click toggle validation
[INFO] [TC-012] OBSERVE expected=1 actual=1
[PASS] [TC-012] single click toggled exactly once

TC-042_RTOS example
[INFO] [TC-042_RTOS] START logger omission validation
[WARN] [TC-042_RTOS] backlog increased during burst
[FAIL] [TC-042_RTOS] dropped count increased

TC-063_RTOS example
[INFO] [TC-063_RTOS] START watchdog servicing validation
[INFO] [TC-063_RTOS] OBSERVE feed_seen=1 max_gap_ms=220
[PASS] [TC-063_RTOS] watchdog feed stayed within allowed gap
```
