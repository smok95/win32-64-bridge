#pragma once
#include <windows.h>
#include <string>

// IPC �޽��� Ÿ��
enum class IPCMessageType {
    ADD = 1,
    SUBTRACT = 2,
    MULTIPLY = 3,
    DIVIDE = 4
};

// IPC ��û ����ü
struct IPCRequest {
    IPCMessageType type;
    double operand1;
    double operand2;
};

// IPC ���� ����ü
struct IPCResponse {
    double result;
    bool success;
};

// Named pipe �̸�
constexpr const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\Calculator32Bridge";

// IPC Ÿ�Ӿƿ� (�и���)
constexpr DWORD IPC_TIMEOUT = 5000;