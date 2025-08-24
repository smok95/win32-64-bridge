#pragma once
#include "../Common/IPCProtocol.h"

class IPCClient {
private:
    HANDLE hPipe;
    bool connected;
    
    bool ConnectToPipe();
    void DisconnectFromPipe();
    IPCResponse SendRequest(const IPCRequest& request);
    
public:
    IPCClient();
    ~IPCClient();
    
    double Add(double a, double b);
    double Subtract(double a, double b);
    double Multiply(double a, double b);
    double Divide(double a, double b);
};