# 32비트 to 64비트 브릿지 시스템

이 프로젝트는 64비트 애플리케이션에서 32비트 DLL을 사용하기 위한 브릿지 시스템입니다.

## 아키텍처

```
64비트 앱 -> Bridge64.dll -> IPC -> Bridge32.exe -> Calculator32.dll (32비트)
```

## 구성 요소

1. **Calculator32.dll**: 32비트 계산기 DLL (실제 계산 수행)
2. **Bridge32.exe**: 32비트 브릿지 실행파일 (IPC 서버, Calculator32.dll 호출)
3. **Bridge64.dll**: 64비트 브릿지 DLL (Calculator32와 동일한 인터페이스, IPC 클라이언트)
4. **TestApp64.exe**: 64비트 테스트 애플리케이션

## 빌드 방법

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 실행 방법

1. 먼저 32비트 브릿지 서버를 시작합니다:
   ```
   cd build/bin32
   Bridge32.exe
   ```

2. 다른 터미널에서 64비트 테스트 앱을 실행합니다:
   ```
   cd build/bin
   TestApp64.exe
   ```

## 인터페이스

Calculator32와 Bridge64는 동일한 인터페이스를 제공합니다:

```cpp
extern "C" {
    double Add(double a, double b);
    double Subtract(double a, double b);
    double Multiply(double a, double b);
    double Divide(double a, double b);
}
```

## IPC 통신

- **프로토콜**: Named Pipe (`\\.\pipe\Calculator32Bridge`)
- **메시지 타입**: ADD, SUBTRACT, MULTIPLY, DIVIDE
- **타임아웃**: 5초

## 주의사항

1. Bridge32.exe가 먼저 실행되어야 합니다.
2. Calculator32.dll은 Bridge32.exe와 같은 디렉토리에 있어야 합니다.
3. Bridge64.dll은 64비트 애플리케이션과 같은 디렉토리에 있어야 합니다.

## 확장 방법

새로운 함수를 추가하려면:

1. Calculator32에 함수 추가
2. IPCProtocol.h에 새 메시지 타입 추가
3. Bridge32의 IPCServer에 처리 로직 추가
4. Bridge64의 IPCClient에 클라이언트 함수 추가