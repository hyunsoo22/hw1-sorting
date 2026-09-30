# 삽입 병합 힙 정렬 비교

C17 표준 라이브러리만 사용하는 정렬 비교 과제입니다.

## 파일

- `src/insertionSort.c`: 샘플 삽입 정렬을 활용
- `src/mergeSort.c`: 안정적인 상향식 병합 정렬
- `src/heapSort.c`: 반복문으로 구현한 최대 힙 정렬
- `src/sort.c`, `src/sort.h`, `src/sortctx.h`: 공통 인터페이스와 측정 도구
- `src/bench.c`, `src/bench.h`: 동일한 입력으로 11회 측정, CPU 시간의 중앙값·평균·범위
- `src/main.c`: 입력 모양 4종 및 입력 크기 4종의 실험
- `tests/test_sort.c`: 42개 집계 검사, 정렬 결과와 원소 보존 검증
- `report/results.csv`: 보고서에 사용한 실제 측정값
- `report/REPORT.md`: 보고서 원문
- `report/ai-learning.md`: AI의 힙 정렬 학습 설명

## 실행

C 컴파일러와 make가 있는 환경에서 프로젝트 폴더로 이동합니다.

```sh
make test
make run
make results
```

macOS에서는 설치된 Apple Clang을 사용하려면 다음처럼 지정할 수 있습니다.

```sh
make CC=clang test
make CC=clang run
```

`make results`는 `report/results.csv`를 새 측정값으로 덮어씁니다. 기존 보고서의 시간은 자동 갱신되지 않습니다. 보고서에 실린 값은 생성 당시 Linux x86_64와 GCC 13.3.0에서 실행한 결과이며 Mac에서 실행하면 시간이 달라질 수 있습니다. `rand()` 구현에 따라 비교·이동 횟수도 플랫폼 간에는 달라질 수 있습니다.

메모리 접근과 정의되지 않은 동작 검사는 `make sanitize`로 실행합니다. 원래 측정 환경에서는 LeakSanitizer의 프로세스 접근이 제한되어 아래처럼 누수 검사만 비활성화했습니다.

```sh
ASAN_OPTIONS=detect_leaks=0 make sanitize
```

## 측정 규약

- 입력: `Record { int key; int tag; }`, 원소 8바이트인 측정 환경
- 비교 기준은 key만 사용, tag는 원소 보존과 안정성 확인에 사용
- seed: 20260930, 무작위 키는 `rand()`, 중복 많은 키는 `rand() % 8`
- 매회 입력을 복사한 후 `clock()`으로 정렬 호출만 측정
- 정렬 내부의 메모리 할당·해제와 통계 계수 비용은 시간에 포함
- `millis`: 11회 CPU 시간의 중앙값, `meanMillis`와 `minMillis`·`maxMillis`도 CSV에 포함
- moves: 원소 한 개의 복사 1회, swap 1회는 3회
- extraBytes: 정렬 함수가 동적으로 요청한 작업 버퍼의 바이트 수. 입력 및 측정 도구 메모리, 할당기 관리 정보, 스택은 제외
- 세 정렬 모두 반복문을 사용하며 재귀 없음

## 출처와 AI 활용

교수님 샘플 https://github.com/lec-algorithm/hw1-sample-2026 의 공통 인터페이스, 도구, 삽입 정렬 및 벤치마크를 바탕으로 변경했습니다. 해당 출처에서 활용한 파일은 sort.h, sort.c, sortctx.h, insertionSort.c, bench.h, bench.c입니다. 병합·힙 정렬과 실험 main 및 테스트는 이 과제를 위해 새로 작성했습니다.

ChatGPT Codex를 사용하여 힙 정렬의 설명, 코드 작성, 테스트, 성능 측정과 보고서 작성을 지원받았습니다. AI 학습 내용은 `report/ai-learning.md`에 첨부합니다.

## 제출 준비

1. 코드 파일과 report 폴더를 본인의 GitHub 저장소에 업로드합니다. `*.out`은 포함하지 않습니다.
2. 보고서의 학번 및 코드 저장소 URL 입력란을 본인의 값으로 채운 뒤 PDF를 다시 저장합니다.
3. PDF 보고서와 본인 GitHub 저장소 URL을 제출합니다.
4. 과제 안내대로 본인 저장소의 Code → Download ZIP으로 받은 코드 ZIP도 제출합니다.

교수님 샘플 저장소 주소는 본인 코드 저장소 주소를 대신하지 않습니다.
