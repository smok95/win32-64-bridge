#include "IPCServer.h"
#include <iostream>
#include <csignal>

std::unique_ptr<IPCServer> g_server;

void SignalHandler(int signal) {
    if (g_server) {
        std::wcout << L"\nShutting down server..." << std::endl;
        g_server->Stop();
    }
}

int main() {
    std::wcout << L"Bridge32 - 32-bit IPC Server starting..." << std::endl;

    // Signal handling for graceful shutdown
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    g_server = std::make_unique<IPCServer>();

    if (!g_server->Start()) {
        std::wcout << L"Failed to start server" << std::endl;
        return 1;
    }

    // Run the server
    g_server->Run();

    std::wcout << L"Server stopped." << std::endl;
    return 0;
}