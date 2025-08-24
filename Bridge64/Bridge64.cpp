#define BRIDGE64_EXPORTS
#include "IPCClient.h"
#include <memory>

// Redefine the API for Bridge64 exports
#ifdef BRIDGE64_EXPORTS
#define BRIDGE64_API __declspec(dllexport)
#else
#define BRIDGE64_API __declspec(dllimport)
#endif

// Global IPC client instance
std::unique_ptr<IPCClient> g_ipcClient;

// Initialize the bridge on DLL load
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            g_ipcClient = std::make_unique<IPCClient>();
            break;
        case DLL_PROCESS_DETACH:
            g_ipcClient.reset();
            break;
    }
    return TRUE;
}

// Export the same interface as Calculator32, but redirect to IPC
extern "C" {
    BRIDGE64_API double Add(double a, double b) {
        return g_ipcClient ? g_ipcClient->Add(a, b) : 0.0;
    }

    BRIDGE64_API double Subtract(double a, double b) {
        return g_ipcClient ? g_ipcClient->Subtract(a, b) : 0.0;
    }

    BRIDGE64_API double Multiply(double a, double b) {
        return g_ipcClient ? g_ipcClient->Multiply(a, b) : 0.0;
    }
    
    BRIDGE64_API double Divide(double a, double b) {
        return g_ipcClient ? g_ipcClient->Divide(a, b) : 0.0;
    }
}