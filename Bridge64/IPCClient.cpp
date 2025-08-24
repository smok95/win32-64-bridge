#include "IPCClient.h"
#include <iostream>
#include <thread>
#include <chrono>

IPCClient::IPCClient() : hPipe(INVALID_HANDLE_VALUE), connected(false) {
}

IPCClient::~IPCClient() {
    DisconnectFromPipe();
}

bool IPCClient::ConnectToPipe() {
    if (connected) {
        return true;
    }

    const int MAX_RETRIES = 5;
    const int RETRY_DELAY_MS = 100;
    
    for (int retry = 0; retry < MAX_RETRIES; retry++) {
        hPipe = CreateFile(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }

        DWORD error = GetLastError();
        if (error == ERROR_PIPE_BUSY) {
            // 파이프가 바쁘면 잠시 대기 후 재시도
            if (WaitNamedPipe(PIPE_NAME, IPC_TIMEOUT)) {
                hPipe = CreateFile(
                    PIPE_NAME,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    nullptr,
                    OPEN_EXISTING,
                    0,
                    nullptr
                );
                
                if (hPipe != INVALID_HANDLE_VALUE) {
                    break;
                }
            }
        }
        
        // 재시도 전 잠시 대기
        if (retry < MAX_RETRIES - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
        }
    }

    if (hPipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr)) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
        return false;
    }

    connected = true;
    return true;
}

void IPCClient::DisconnectFromPipe() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
    connected = false;
}

IPCResponse IPCClient::SendRequest(const IPCRequest& request) {
    IPCResponse response = {};
    response.success = false;

    if (!ConnectToPipe()) {
        return response;
    }

    DWORD bytesWritten;
    if (!WriteFile(hPipe, &request, sizeof(request), &bytesWritten, nullptr)) {
        DisconnectFromPipe();
        return response;
    }

    DWORD bytesRead;
    if (!ReadFile(hPipe, &response, sizeof(response), &bytesRead, nullptr)) {
        DisconnectFromPipe();
        return response;
    }

    DisconnectFromPipe();
    return response;
}

double IPCClient::Add(double a, double b) {
    IPCRequest request = { IPCMessageType::ADD, a, b };
    IPCResponse response = SendRequest(request);
    return response.success ? response.result : 0.0;
}

double IPCClient::Subtract(double a, double b) {
    IPCRequest request = { IPCMessageType::SUBTRACT, a, b };
    IPCResponse response = SendRequest(request);
    return response.success ? response.result : 0.0;
}

double IPCClient::Multiply(double a, double b) {
    IPCRequest request = { IPCMessageType::MULTIPLY, a, b };
    IPCResponse response = SendRequest(request);
    return response.success ? response.result : 0.0;
}

double IPCClient::Divide(double a, double b) {
    IPCRequest request = { IPCMessageType::DIVIDE, a, b };
    IPCResponse response = SendRequest(request);
    return response.success ? response.result : 0.0;
}