#pragma once
#include <windows.h>
#include <string>

// IPC 메시지 타입
enum class IPCMessageType {
    ADD = 1,
    SUBTRACT = 2,
    MULTIPLY = 3,
    DIVIDE = 4
};

// IPC 요청 구조체
struct IPCRequest {
    IPCMessageType type;
    double operand1;
    double operand2;
};

// IPC 응답 구조체
struct IPCResponse {
    double result;
    bool success;
};

// Named pipe 이름
constexpr const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\Calculator32Bridge";

// IPC 타임아웃 (밀리초)
constexpr DWORD IPC_TIMEOUT = 5000;