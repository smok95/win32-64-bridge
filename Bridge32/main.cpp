#include "IPCServer.h"
#include <iostream>
#include <csignal>
#include <windows.h>

std::unique_ptr<IPCServer> g_server;

void SignalHandler(int signal) {
    if (g_server) {
        g_server->Stop();
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Signal handling for graceful shutdown
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    g_server = std::make_unique<IPCServer>();

    if (!g_server->Start()) {
        return 1;
    }

    // Run the server
    g_server->Run();

    return 0;
}