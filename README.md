# 32��Ʈ to 64��Ʈ �긴�� �ý���

�� ������Ʈ�� 64��Ʈ ���ø����̼ǿ��� 32��Ʈ DLL�� ����ϱ� ���� �긴�� �ý����Դϴ�.

## ��Ű��ó

```
64��Ʈ �� -> Bridge64.dll -> IPC -> Bridge32.exe -> Calculator32.dll (32��Ʈ)
```

## ���� ���

1. **Calculator32.dll**: 32��Ʈ ���� DLL (���� ��� ����)
2. **Bridge32.exe**: 32��Ʈ �긴�� �������� (IPC ����, Calculator32.dll ȣ��)
3. **Bridge64.dll**: 64��Ʈ �긴�� DLL (Calculator32�� ������ �������̽�, IPC Ŭ���̾�Ʈ)
4. **TestApp64.exe**: 64��Ʈ �׽�Ʈ ���ø����̼�

## ���� ���

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## ���� ���

1. ���� 32��Ʈ �긴�� ������ �����մϴ�:
   ```
   cd build/bin32
   Bridge32.exe
   ```

2. �ٸ� �͹̳ο��� 64��Ʈ �׽�Ʈ ���� �����մϴ�:
   ```
   cd build/bin
   TestApp64.exe
   ```

## �������̽�

Calculator32�� Bridge64�� ������ �������̽��� �����մϴ�:

```cpp
extern "C" {
    double Add(double a, double b);
    double Subtract(double a, double b);
    double Multiply(double a, double b);
    double Divide(double a, double b);
}
```

## IPC ���

- **��������**: Named Pipe (`\\.\pipe\Calculator32Bridge`)
- **�޽��� Ÿ��**: ADD, SUBTRACT, MULTIPLY, DIVIDE
- **Ÿ�Ӿƿ�**: 5��

## ���ǻ���

1. Bridge32.exe�� ���� ����Ǿ�� �մϴ�.
2. Calculator32.dll�� Bridge32.exe�� ���� ���丮�� �־�� �մϴ�.
3. Bridge64.dll�� 64��Ʈ ���ø����̼ǰ� ���� ���丮�� �־�� �մϴ�.

## Ȯ�� ���

���ο� �Լ��� �߰��Ϸ���:

1. Calculator32�� �Լ� �߰�
2. IPCProtocol.h�� �� �޽��� Ÿ�� �߰�
3. Bridge32�� IPCServer�� ó�� ���� �߰�
4. Bridge64�� IPCClient�� Ŭ���̾�Ʈ �Լ� �߰�