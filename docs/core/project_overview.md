# safe-f411 Project Overview

## 1. 프로젝트 한 줄 소개
safe-f411은 STM32 NUCLEO-F411RE 보드에서 버튼, LED, UART, RTOS 동작을 시험 관점으로 검증하는 embedded test project이다.

## 2. 프로젝트 목적
- GPIO, UART, timing, debounce 같은 기본 동작을 검증
- 결함 재현과 수정 검증을 TC 기반으로 관리
- bare-metal 구조의 한계를 RTOS 구조와 비교 관찰
- 시험 절차서, 결과서, 릴리즈 문서까지 포함한 전달 가능한 형태를 정리

## 3. 대상 환경
- Board: NUCLEO-F411RE
- MCU: STM32F411RETx
- IDE/Config: STM32CubeIDE / CubeMX 기반 설정
- Build: CMake + VS Code 환경 병행
- UART: USART2, 115200, 8-N-1, no flow control
- Time base: TIM2 1ms tick
- LED: PA5
- Button: PC13, active-low, pull-up 입력

## 4. 현재 코드 기준 실행 설정
- `EXEC_ROLE`: TEST
- `EXEC_MODEL`: BAREMETAL
- `TEST_BAREMETAL_TARGET`: `RUN_TC_060`
- `TEST_RTOS_TARGET`: `RUN_TC_063_RTOS`

## 5. 시험 관점 핵심 포인트
- 단순 동작 확인보다 관찰 가능성과 판정 기준을 중시
- 버튼 입력은 polling 기반 FSM으로 click/long-press를 판정
- UART 출력과 이벤트 처리 경로 차이를 시험 포인트로 활용
- RTOS에서는 button task와 log service task를 분리해 queue 기반으로 관찰

## 6. 대표 TC 그룹
- TC-000 ~ TC-022: bring-up / 기능 기본 검증
- TC-030 ~ TC-032: timing / debounce / 부하 영향
- TC-040 ~ TC-042: 결함 재현
- TC-050 ~ TC-052: 수정 검증
- TC-060 ~ TC-066: 회귀 / 장시간 / 복구 / safe-state 관찰
- TC-030_RTOS ~ TC-066_RTOS: RTOS 구조 검증

## 7. 산출물 방향
이 프로젝트는 코드 자체보다 아래 산출물을 함께 보여줄 때 실무 적합도가 높아진다.
- 아키텍처 문서
- 시험 절차서
- 시험 결과서
- Release Note
- User Guide
- Developer Guide
- Delivery Package 구조
