#include "IPCServer.h"
#include "../Calculator32/Calculator32.h"
#include <iostream>
#include <thread>
#include <chrono>

IPCServer::IPCServer() : isRunning(false), 
    calculatorDll(nullptr), addFunc(nullptr), subtractFunc(nullptr), 
    multiplyFunc(nullptr), divideFunc(nullptr) {
}

IPCServer::~IPCServer() {
    Stop();
    UnloadCalculatorDLL();
}

bool IPCServer::LoadCalculatorDLL() {
    calculatorDll = LoadLibrary(L"Calculator32.dll");
    if (!calculatorDll) {
        std::wcout << L"Failed to load Calculator32.dll" << std::endl;
        return false;
    }

    addFunc = (AddFunc)GetProcAddress(calculatorDll, "Add");
    subtractFunc = (SubtractFunc)GetProcAddress(calculatorDll, "Subtract");
    multiplyFunc = (MultiplyFunc)GetProcAddress(calculatorDll, "Multiply");
    divideFunc = (DivideFunc)GetProcAddress(calculatorDll, "Divide");

    if (!addFunc || !subtractFunc || !multiplyFunc || !divideFunc) {
        std::wcout << L"Failed to get function addresses from Calculator32.dll" << std::endl;
        UnloadCalculatorDLL();
        return false;
    }

    return true;
}

void IPCServer::UnloadCalculatorDLL() {
    if (calculatorDll) {
        FreeLibrary(calculatorDll);
        calculatorDll = nullptr;
    }
    addFunc = nullptr;
    subtractFunc = nullptr;
    multiplyFunc = nullptr;
    divideFunc = nullptr;
}

HANDLE IPCServer::CreatePipeInstance() {
    return CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        MAX_PIPE_INSTANCES,  // 여러 인스턴스 허용
        sizeof(IPCResponse),
        sizeof(IPCRequest),
        IPC_TIMEOUT,
        nullptr
    );
}

bool IPCServer::Start() {
    if (!LoadCalculatorDLL()) {
        return false;
    }

    // 여러 파이프 인스턴스 생성
    for (int i = 0; i < MAX_PIPE_INSTANCES; i++) {
        HANDLE hPipe = CreatePipeInstance();
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::wcout << L"Failed to create named pipe instance " << i << L". Error: " << GetLastError() << std::endl;
            // 기존에 생성된 파이프들 정리
            for (HANDLE pipe : hPipes) {
                CloseHandle(pipe);
            }
            hPipes.clear();
            return false;
        }
        hPipes.push_back(hPipe);
    }

    isRunning = true;
    std::wcout << L"Bridge32 server started successfully with " << MAX_PIPE_INSTANCES << L" pipe instances" << std::endl;
    return true;
}

void IPCServer::Stop() {
    {
        std::lock_guard<std::mutex> lock(serverMutex);
        isRunning = false;
    }
    
    // 모든 파이프 핸들 닫기
    for (HANDLE hPipe : hPipes) {
        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
        }
    }
    hPipes.clear();
    
    // 모든 워커 스레드 종료 대기
    for (auto& thread : workerThreads) {
        if (thread && thread->joinable()) {
            thread->join();
        }
    }
    workerThreads.clear();
}

IPCResponse IPCServer::ProcessRequest(const IPCRequest& request) {
    IPCResponse response = {};
    response.success = true;

    try {
        switch (request.type) {
            case IPCMessageType::ADD:
                response.result = addFunc(request.operand1, request.operand2);
                break;
            case IPCMessageType::SUBTRACT:
                response.result = subtractFunc(request.operand1, request.operand2);
                break;
            case IPCMessageType::MULTIPLY:
                response.result = multiplyFunc(request.operand1, request.operand2);
                break;
            case IPCMessageType::DIVIDE:
                response.result = divideFunc(request.operand1, request.operand2);
                break;
            default:
                response.success = false;
                break;
        }
    }
    catch (...) {
        response.success = false;
    }

    return response;
}

void IPCServer::HandlePipeInstance(HANDLE hPipe, int instanceId) {
    std::wcout << L"Worker thread " << instanceId << L" started" << std::endl;
    
    size_t connectCount = 0;
    
    while (true) {
        {
            std::lock_guard<std::mutex> lock(serverMutex);
            if (!isRunning) {
                break;
            }
        }
        
        // 클라이언트 연결 대기
        BOOL connected = ConnectNamedPipe(hPipe, nullptr);
        if (!connected && GetLastError() != ERROR_PIPE_CONNECTED) {
            // 연결 실패 시 잠시 대기
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        connectCount++;
        std::wcout << L"Instance " << instanceId << L": Client connected (" << connectCount << L")" << std::endl;

        // 요청 읽기
        IPCRequest request;
        DWORD bytesRead;
        
        if (ReadFile(hPipe, &request, sizeof(request), &bytesRead, nullptr)) {
            // 요청 처리
            IPCResponse response = ProcessRequest(request);

            // 응답 전송
            DWORD bytesWritten;
            WriteFile(hPipe, &response, sizeof(response), &bytesWritten, nullptr);
        }

        // 클라이언트 연결 해제
        DisconnectNamedPipe(hPipe);
        std::wcout << L"Instance " << instanceId << L": Client disconnected (" << connectCount << L")" << std::endl;
    }
    
    std::wcout << L"Worker thread " << instanceId << L" stopped" << std::endl;
}

void IPCServer::Run() {
    std::wcout << L"Starting worker threads for pipe instances..." << std::endl;

    // 각 파이프 인스턴스에 대해 워커 스레드 생성
    for (int i = 0; i < static_cast<int>(hPipes.size()); i++) {
        auto thread = std::make_unique<std::thread>(&IPCServer::HandlePipeInstance, this, hPipes[i], i);
        workerThreads.push_back(std::move(thread));
    }
    
    std::wcout << L"All worker threads started. Waiting for connections..." << std::endl;
    
    // 메인 스레드는 종료 신호를 기다림
    while (true) {
        {
            std::lock_guard<std::mutex> lock(serverMutex);
            if (!isRunning) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}