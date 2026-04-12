# Test Judgement Rule

## 1. 공통 원칙
- PASS: 기대 동작과 관찰 결과가 일치
- FAIL: 기대 결과 불일치, timeout, overflow, duplicate event, 미검출
- TEST_IN_REVIEW: 시간 경과 후 재평가가 필요한 상태

## 2. 대표 TC 기준
### TC-012
- PASS: 1 click당 LED 1회만 토글
- FAIL: click 미검출, duplicate toggle, long event 발생

### TC-042_RTOS
- PASS: 지정 시간 내 처리 완료 및 누락 없음
- FAIL: timeout, dropped 증가, expected 수량 미달

### TC-063_RTOS
- PASS: watchdog feed 존재, gap 허용 범위 이내
- FAIL: feed 미관찰, max gap 초과

## 3. 문서 반영 원칙
- 코드, 시험 절차서, 결과서의 판정 기준 표현을 맞춘다.
- 절차성 서술보다 판정 문장을 우선한다.
