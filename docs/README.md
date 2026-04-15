# safe-f411

STM32 NUCLEO-F411RE 기반 embedded verification project입니다.
버튼, LED, UART, RTOS 동작을 단순 기능 확인이 아니라 문제 재현, 관찰 기준, PASS/FAIL 판정, 수정 검증, 회귀 확인 관점에서 정리했습니다.
bare-metal에서 시작해 RTOS로 확장하며 event 처리와 로그 출력 경로를 분리했고, 시험 및 문서화까지 함께 구성했습니다.

## Overview

- Board: STM32 NUCLEO-F411RE
- MCU: STM32F411RETx
- Language: C
- Environment: VS Code + STM32Cube extension
- Interface: GPIO, UART, TIM, FreeRTOS
- Focus: embedded test firmware, defect reproduction, validation, documentation

## What this project shows

동일한 버튼 입력이라도 내부 처리 경로에 따라 LED 반응과 UART 로그 시점이 달라질 수 있다는 점에 주목했고, 이를 설명 가능한 테스트 구조로 정리했습니다.
bare-metal에서는 빠른 bring-up과 기본 기능 검증에 집중하고, RTOS에서는 button, logger, monitor, probe 역할을 분리해 queue, watchdog, overflow, blocking UART 영향을 더 구조적으로 관찰할 수 있도록 확장했습니다.

- STM32 기반 임베디드 C 개발
- GPIO, UART, timing, RTOS 기반 구조 이해
- 문제 재현과 원인 분리 중심의 테스트 설계
- PASS/FAIL 판정 기준 정리
- 수정 검증과 회귀 테스트 수행
- 시험 절차서, 결과서, Release Note 등 문서화

## Key Design Points

### Test-oriented structure
APP 실행과 TEST 실행을 분리해 기능 구현보다 검증 흐름이 먼저 보이도록 구성했습니다.

### Reproducible input handling
raw 버튼 입력을 바로 사용하지 않고 debounce, click, long-press 기준으로 판정해 재현 가능한 테스트 입력 구조를 만들었습니다.

### Log and judgement rules
로그를 START, OBSERVE, PASS, FAIL 흐름으로 정리해 결과를 해석 가능한 형태로 남기도록 했습니다.

### RTOS extension
RTOS에서는 button, logger, monitor, probe 역할을 분리해 event 처리와 로그 출력 경로를 나누고, 운영 상태를 더 구조적으로 관찰할 수 있도록 했습니다.

## Representative Test Cases

### TC-012 Single Click Toggle
1 click 입력당 LED가 정확히 1회만 토글되는지 검증합니다.

### TC-042_RTOS Logger Omission Validation
RTOS 환경에서 로그 처리 경로의 지연, 누락, backlog 가능성을 관찰합니다.

### TC-060 Regression Validation
버튼 처리 수정 이후 click, long-press, LED 반응이 다시 깨지지 않았는지 회귀 확인합니다.

### TC-063_RTOS Watchdog Servicing Validation
RTOS 환경에서 watchdog servicing 간격이 허용 범위 내인지 검증합니다.

## Hardware / Environment

### Hardware
- LED: PA5 (LD2)
- Button: PC13, active-low
- UART: USART2
- Time base: TIM2 1ms tick

### Development / Test Environment
- VS Code + STM32Cube
- CMake
- Arm GNU Toolchain
- ST-LINK
- Tera Term

### UART Setting
- 115200 / 8-N-1 / No Flow Control

## Repository Structure

    safe-f411/
    ├─ Core/
    │  ├─ Inc/
    │  └─ Src/
    │     ├─ app/
    │     ├─ common/
    │     ├─ rtos/
    │     └─ test/
    ├─ Drivers/
    ├─ Middlewares/
    ├─ .vscode/
    ├─ build/
    └─ docs/
       ├─ core/
       └─ supporting/

## Build / Run

### Build
    cmake --build --preset default

### Flash
VS Code task 기준으로 아래 흐름을 사용합니다.

- build-debug
- flash-run
- build-and-run

### UART Monitor
- Tera Term 실행
- 보드 COM 포트 연결
- 115200 / 8-N-1 / No Flow Control 설정
- START / OBSERVE / PASS / FAIL 로그 확인

## Documentation

이 프로젝트는 코드뿐 아니라 시험과 문서도 함께 정리했습니다.

### Core documents
- project_overview
- architecture
- module_responsibility
- test_strategy
- test_procedure
- test_report
- release_note

### Supporting documents
- user_guide
- developer_guide
- log_rule
- test_judgement_rule
- traceability
- delivery_package

## Conclusion

safe-f411은 STM32F411 보드에서 버튼, LED, UART, RTOS 동작을 테스트 관점으로 구조화한 프로젝트입니다.

문제 재현, 관찰 기준, 판정 규칙, 수정 검증, 회귀 확인, 문서화까지 연결되는 프로젝트입니다
