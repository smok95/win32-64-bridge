#pragma once
#include "../Common/IPCProtocol.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

class IPCServer {
private:
    static constexpr int MAX_PIPE_INSTANCES = 10; // 최대 파이프 인스턴스 수
    
    std::vector<HANDLE> hPipes;
    std::vector<std::unique_ptr<std::thread>> workerThreads;
    bool isRunning;
    std::mutex serverMutex;
    
    // Calculator32 DLL 함수 포인터들
    typedef double(*AddFunc)(double, double);
    typedef double(*SubtractFunc)(double, double);
    typedef double(*MultiplyFunc)(double, double);
    typedef double(*DivideFunc)(double, double);
    
    HMODULE calculatorDll;
    AddFunc addFunc;
    SubtractFunc subtractFunc;
    MultiplyFunc multiplyFunc;
    DivideFunc divideFunc;
    
    bool LoadCalculatorDLL();
    void UnloadCalculatorDLL();
    IPCResponse ProcessRequest(const IPCRequest& request);
    
    // 개별 파이프 인스턴스를 처리하는 워커 스레드
    void HandlePipeInstance(HANDLE hPipe, int instanceId);
    HANDLE CreatePipeInstance();
    
public:
    IPCServer();
    ~IPCServer();
    
    bool Start();
    void Stop();
    void Run();
};