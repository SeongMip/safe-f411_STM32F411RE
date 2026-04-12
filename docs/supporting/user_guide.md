# User Guide

## 1. 목적
본 문서는 safe-f411을 사용하는 사용자가 보드 실행, UART 연결, 기본 결과 확인을 할 수 있도록 돕기 위한 문서이다.

## 2. 준비물
- NUCLEO-F411RE 보드
- USB 케이블
- ST-LINK 사용 가능한 개발 PC
- UART terminal 프로그램

## 3. 보드 연결
1. 보드를 USB로 PC에 연결한다.
2. ST-LINK debug 연결이 인식되는지 확인한다.
3. terminal에서 해당 COM 포트를 선택한다.

## 4. UART 설정
- Port: 보드에 할당된 COM 포트
- BaudRate: 115200
- Data: 8 bit
- Parity: None
- Stop bit: 1
- Flow control: None

## 5. 실행 방법
1. 프로젝트를 build 한다.
2. firmware를 download 한다.
3. terminal을 열고 UART를 연결한다.
4. 보드가 boot 되면 START 로그를 확인한다.
5. 선택된 TC의 결과를 확인한다.

## 6. 결과 해석
- `START`: 시험 시작
- `OBSERVE`: 관찰값 출력
- `PASS`: 기대 결과 충족
- `FAIL`: 판정 기준 불만족
- `END`: 시험 종료

## 7. 주의 사항
- PC13 버튼은 active-low로 해석된다.
- debug attach 환경에서는 boot 관련 로그가 다르게 보일 수 있다.
- RTOS와 bare-metal은 로그 경로와 실행 흐름이 다를 수 있다.
