# Developer Guide

## 1. 목적
본 문서는 safe-f411 소스 구조를 빠르게 이해하고, 새 TC 추가나 문서 갱신을 할 수 있도록 돕는다.

## 2. 루트 구조
```text
safe-f411/
├─ Core/
├─ Drivers/
├─ Middlewares/
├─ cmake/
├─ build/
└─ docs/
```

## 3. 핵심 설정
- 실행 역할: `Core/Inc/app/config.h`
- 테스트 선택: `Core/Inc/test/test_config.h`
- UART: `Core/Src/usart.c`
- GPIO: `Core/Src/gpio.c`
- RTOS object/task 생성: `Core/Src/rtos/rtos_app.c`

## 4. 새 TC 추가 절차
1. `Core/Inc/test/...` 와 `Core/Src/test/...`에 header/source를 추가한다.
2. runtime runner의 테스트 테이블에 등록한다.
3. START / PASS / FAIL 로그 형식을 공통 규칙에 맞춘다.
4. 관련 시험 절차 문서와 traceability 문서를 함께 갱신한다.

## 5. 로그 규칙
- 공통 형식: `[LEVEL] [TC-ID] message`
- 판정 메시지는 관찰 근거가 드러나야 한다.
- 코드 해석형 로그보다 판정형 로그를 우선한다.

## 6. 주석 규칙
- `docs/CODE_COMMENT_GUIDE.md`를 기준으로 유지
- generated 영역에는 프로젝트 고유 주석을 최소화
- TC 함수 주석은 PASS/FAIL 기준 중심으로 작성

## 7. 리뷰 우선순위
1. 실행 설정값이 실제 의도와 맞는지 확인
2. runner selection과 target 값 확인
3. button/LED/UART 흐름 확인
4. RTOS queue/task/probe 연동 확인
5. TC 문서와 실제 코드의 판정 기준이 일치하는지 확인
