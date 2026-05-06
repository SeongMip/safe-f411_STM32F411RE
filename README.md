# safe-f411

STM32 NUCLEO-F411RE 기반 embedded verification project입니다.
버튼, LED, UART, RTOS 동작을 단순 기능 확인이 아니라 문제 재현, 관찰 기준, PASS/FAIL 판정, 수정 검증, 회귀 확인 관점에서 정리했습니다.

이 저장소는 보드 펌웨어와 Windows host tool을 하나의 시험 체계로 관리하도록 재구성했습니다.
보드에서 시험 로그를 생성하고, PC에서는 UART 로그를 test state와 PASS/FAIL을 표시하도록 구성했습니다.

## Overview

- Board: STM32 NUCLEO-F411RE
- MCU: STM32F411RETx
- Firmware Language: C
- Host Tool Language: C#
- Environment: VS Code + STM32Cube extension / Windows WinForms
- Interface: GPIO, UART, TIM, FreeRTOS
- Focus: embedded test firmware, defect reproduction, validation, documentation

## Repository Structure

```text
safe-f411/
├─ firmware/
│  ├─ Core/
│  ├─ Drivers/
│  ├─ Middlewares/
│  ├─ cmake/
│  ├─ CMakeLists.txt
│  └─ CMakePresets.json
├─ host-tool/
│  └─ SafeF411.HostTool/
├─ docs/
│  ├─ core/
│  └─ supporting/
├─ .vscode/
└─ README.md
```

## What this project shows

동일한 버튼 입력이라도 내부 처리 경로에 따라 LED 반응과 UART 로그 시점이 달라질 수 있다는 점에 주목했고, 이를 설명 가능한 테스트 구조로 정리했습니다.
bare-metal에서는 빠른 bring-up과 기본 기능 검증에 집중하고, RTOS에서는 button, logger, monitor, probe 역할을 분리해 queue, watchdog, overflow, blocking UART 영향을 더 구조적으로 관찰할 수 있도록 확장했습니다.

- STM32 기반 임베디드 C 개발
- GPIO, UART, timing, RTOS 기반 구조 이해
- 문제 재현과 원인 분리 중심의 테스트 설계
- PASS/FAIL 판정 기준 정리
- 수정 검증과 회귀 테스트 수행
- Windows host tool을 통해 UART 시험 로그, test state, PASS / FAIL 결과 확인
- 로그 규칙, 판정 기준, 사용/개발 가이드 문서화

## Build / Run

### Firmware build

```bash
cd firmware
cmake --preset default
cmake --build --preset default
```

### Firmware flash

VS Code task 기준으로 아래 흐름을 사용합니다.

- firmware-configure-debug
- firmware-build-debug
- firmware-flash-run
- firmware-build-and-run

### Host tool build

```bash
dotnet build host-tool/SafeF411.HostTool/SafeF411.HostTool.csproj
```

### Host tool run

```bash
dotnet run --project host-tool/SafeF411.HostTool/SafeF411.HostTool.csproj
```

Execution flow
firmware를 build 및 flash 한다.
host tool을 실행한다.
COM 포트를 선택하고 Connect 한다.
UART 로그, test state, PASS / FAIL 결과를 확인한다.

## Documentation

문서 목록은 `docs/README.md`에 정리했습니다.

## Conclusion

safe-f411은 STM32F411 보드에서 버튼, LED, UART, RTOS 동작을 테스트 관점으로 구조화한 프로젝트입니다.

문제 재현, 관찰 기준, 판정 규칙, 수정 검증, 회귀 확인을 중심으로 정리한 프로젝트입니다.
