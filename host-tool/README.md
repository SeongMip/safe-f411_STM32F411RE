# host-tool

`SafeF411.HostTool`은 safe-f411 보드의 UART 로그를 Windows에서 관찰하기 위한 WinForms 기반 MVP입니다.

## MVP scope
- COM 포트 선택
- Connect / Disconnect
- UART 로그 수신
- test state, PASS / FAIL 표시

## Build
```bash
dotnet build SafeF411.HostTool/SafeF411.HostTool.csproj
```

## Role
- firmware: 테스트 실행과 실제 PASS / FAIL 판정 수행
- host tool: UART 로그 수신, test state 표시, PASS / FAIL 결과 가시화

## Run
1. 보드 firmware를 flash 한다.
2. host tool을 실행한다.
3. COM 포트를 선택하고 Connect 한다.
4. UART 로그와 결과 상태를 확인한다.
