# Delivery Package

## 1. 목적
본 문서는 safe-f411을 시험/납품 관점으로 정리할 때 어떤 파일을 전달 단위로 묶을지 정의한다.

## 2. 패키지 구조
```text
delivery/
├─ firmware/
│  ├─ elf/
│  ├─ map/
│  └─ release-note/
├─ source/
│  └─ safe-f411/
├─ docs/
│  ├─ architecture
│  ├─ test-procedure
│  └─ test-report
│  ├─ user-guide
│  └─ developer-guide
├─ test-results/
│  ├─ logs/
│  ├─ screenshots/
│  └─ summary/
└─ checksum/
```

## 3. 포함 대상
- source code
- build/release 산출물
- 시험 절차서
- 시험 결과서
- release note
- user/developer guide
- 대표 로그 및 캡처

## 4. 제외 대상
- 개인 PC 의존 설정
- 임시 로그
- 의미 없는 build cache
- TODO 메모

## 5. 확인 항목
- 받은 사람이 UART 연결과 기본 실행을 재현할 수 있는가
- 시험 절차와 결과를 문서만으로 추적할 수 있는가
- 버전 이력과 전달 파일 구성이 일치하는가
