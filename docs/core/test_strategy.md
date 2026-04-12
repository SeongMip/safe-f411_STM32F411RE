# Test Strategy

## 1. 기본 방향
safe-f411의 테스트는 기능 데모보다 다음 4가지를 우선한다.
- 무엇을 검증하는가
- 무엇을 관찰하는가
- PASS/FAIL을 무엇으로 나누는가
- 수정 후 동일 문제가 다시 생기지 않는가

## 2. 시험 분류
### Bring-up Verification
- 펌웨어 다운로드 후 최소 실행 가능 여부 확인
- boot log, heartbeat, clock sanity 포함

### Basic Functional Verification
- LED, 버튼, UART 기본 동작 확인
- raw 입력과 내부 판정의 일관성 확인

### Defect Reproduction
- bounce, repeated event, UART omission 같은 취약 시나리오 재현
- 문제를 “왜 발생했는지 분리 관찰”하는 단계

### Fix Validation
- 결함 수정 후 기존 FAIL 조건이 제거되었는지 확인
- 수정이 실제 동작으로 연결되었는지 검증

### Regression / Robustness Validation
- 빠른 입력, 장시간 동작, reset/power cycle, safe-state 관련 관찰
- 수정 이후 주변 경로가 깨지지 않았는지 확인

### RTOS Validation
- queue, task, watchdog, backlog 관점에서 구조적 개선 확인
- bare-metal 대비 event path 분리 효과 관찰

## 3. 공통 관찰 항목
- UART 로그
- LED state 변화
- button FSM event
- queue 처리량 / overflow
- probe tick / watchdog feed
- timeout 여부

## 4. 공통 판정 원칙
- PASS: 기대 동작과 관찰 결과가 일치
- FAIL: 기대 동작 불일치, timeout, overflow 증가, 중복 이벤트, 누락 발생
- TEST_IN_REVIEW: 재평가 필요

## 5. 문서화 원칙
- 절차 나열보다 판정 기준을 우선 기록
- 원인 단정형 표현보다 관찰 사실을 우선 기록
- 재현 조건과 수정 후 결과를 대응 관계로 남긴다
