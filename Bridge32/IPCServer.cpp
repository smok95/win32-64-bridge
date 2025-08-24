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
        MAX_PIPE_INSTANCES,  // ���� �ν��Ͻ� ���
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

    // ���� ������ �ν��Ͻ� ����
    for (int i = 0; i < MAX_PIPE_INSTANCES; i++) {
        HANDLE hPipe = CreatePipeInstance();
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::wcout << L"Failed to create named pipe instance " << i << L". Error: " << GetLastError() << std::endl;
            // ������ ������ �������� ����
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
    
    // ��� ������ �ڵ� �ݱ�
    for (HANDLE hPipe : hPipes) {
        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
        }
    }
    hPipes.clear();
    
    // ��� ��Ŀ ������ ���� ���
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
        
        // Ŭ���̾�Ʈ ���� ���
        BOOL connected = ConnectNamedPipe(hPipe, nullptr);
        if (!connected && GetLastError() != ERROR_PIPE_CONNECTED) {
            // ���� ���� �� ��� ���
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        connectCount++;
        std::wcout << L"Instance " << instanceId << L": Client connected (" << connectCount << L")" << std::endl;

        // ��û �б�
        IPCRequest request;
        DWORD bytesRead;
        
        if (ReadFile(hPipe, &request, sizeof(request), &bytesRead, nullptr)) {
            // ��û ó��
            IPCResponse response = ProcessRequest(request);

            // ���� ����
            DWORD bytesWritten;
            WriteFile(hPipe, &response, sizeof(response), &bytesWritten, nullptr);
        }

        // Ŭ���̾�Ʈ ���� ����
        DisconnectNamedPipe(hPipe);
        std::wcout << L"Instance " << instanceId << L": Client disconnected (" << connectCount << L")" << std::endl;
    }
    
    std::wcout << L"Worker thread " << instanceId << L" stopped" << std::endl;
}

void IPCServer::Run() {
    std::wcout << L"Starting worker threads for pipe instances..." << std::endl;

    // �� ������ �ν��Ͻ��� ���� ��Ŀ ������ ����
    for (int i = 0; i < static_cast<int>(hPipes.size()); i++) {
        auto thread = std::make_unique<std::thread>(&IPCServer::HandlePipeInstance, this, hPipes[i], i);
        workerThreads.push_back(std::move(thread));
    }
    
    std::wcout << L"All worker threads started. Waiting for connections..." << std::endl;
    
    // ���� ������� ���� ��ȣ�� ��ٸ�
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