# Test Procedure

## 1. 시험 환경
- Board: NUCLEO-F411RE
- Power/Debug: ST-LINK
- UART Terminal: Tera Term 또는 동등 도구
- UART Setting: 115200 / 8-N-1 / no flow control
- Firmware Build: 현재 선택된 `EXEC_ROLE`, `EXEC_MODEL`, `TEST_*_TARGET` 기준

## 2. 공통 절차
1. 보드를 PC에 연결한다.
2. 프로젝트를 build 하고 firmware를 download 한다.
3. UART terminal을 열고 포트를 연결한다.
4. 선택된 TC의 START 로그를 확인한다.
5. TC별 관찰 포인트를 확인한다.
6. 최종 PASS/FAIL 로그를 기록한다.
7. 필요 시 LED 상태와 terminal 로그를 캡처한다.

## 3. 대표 TC

### TC-000 Firmware download & execution verification
- 목적: 펌웨어 다운로드 후 기본 실행이 시작되는지 확인
- 관찰 포인트: heartbeat LED, START log, 지속 실행 여부
- 기대 결과: heartbeat가 지속되고 시스템이 멈추지 않음
- PASS 기준: download 성공 후 heartbeat와 기본 로그가 정상 확인됨
- FAIL 기준: download 실패, boot 미진입, LED 무응답

### TC-012 Single click toggle
- 목적: 1 click 입력당 LED가 정확히 1회만 토글되는지 검증
- 사전 조건: 버튼 입력이 active-low로 정상 읽힘
- 절차:
  1. 보드를 실행한다.
  2. 버튼을 짧게 1회 입력한다.
  3. UART 로그와 LED 상태를 확인한다.
- 관찰 포인트:
  - click event
  - actual LED state
  - expected count 대비 verifier 결과
- PASS 기준: 정해진 click 횟수 동안 1 click = 1 toggle 유지
- FAIL 기준: click 미검출, duplicate toggle, long event 발생

### TC-042 UART event omission defect
- 목적: 이벤트 처리와 UART 표현 사이의 누락 여부를 관찰
- 절차:
  1. 결함 재현용 빌드 또는 시나리오를 사용한다.
  2. 버튼 입력 또는 이벤트를 반복 발생시킨다.
  3. LED 반응과 UART 로그를 비교한다.
- 관찰 포인트:
  - LED는 반응했지만 UART는 누락되는지
  - backlog 또는 blocking 영향 징후
- PASS 기준: 결함 재현 TC에서는 의도한 취약 조건이 관찰 가능
- FAIL 기준: 재현 조건이 전혀 드러나지 않거나 로그 근거가 부족함

### TC-063_RTOS Watchdog servicing validation
- 목적: RTOS 환경에서 watchdog servicing 간격이 허용 범위 내인지 검증
- 절차:
  1. `TEST_RTOS_TARGET`을 `RUN_TC_063_RTOS`로 설정한다.
  2. RTOS build 후 실행한다.
  3. 관찰 시간 동안 watchdog probe 관련 로그를 확인한다.
- 관찰 포인트:
  - watchdog feed 존재 여부
  - feed 간격
  - timeout 또는 gap 초과 여부
- PASS 기준: 관찰 시간 내 feed가 존재하고 허용 gap 이내 유지
- FAIL 기준: feed 미관찰, 허용 gap 초과, timeout

## 4. 시험 결과 기록 규칙
- 날짜/시간
- firmware 설정값
- 수행한 TC
- 결과: PASS / FAIL
- 캡처 또는 로그 파일 경로
- 비고: 재현 조건, 이상 징후, 수동 확인 사항
