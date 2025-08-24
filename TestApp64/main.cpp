#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

// Bridge64 functions (same interface as Calculator32)
extern "C" {
    __declspec(dllimport) double Add(double a, double b);
    __declspec(dllimport) double Subtract(double a, double b);
    __declspec(dllimport) double Multiply(double a, double b);
    __declspec(dllimport) double Divide(double a, double b);
}

int main() {
    std::wcout << L"=== 64-bit Application Test ===" << std::endl;
    std::wcout << L"Testing Bridge64.dll -> Bridge32.exe -> Calculator32.dll" << std::endl;
    std::wcout << std::endl;

    // Test values
    double a = 10.5;
    double b = 3.2;

    int remain = 50000;
    while (remain >= 0) {
		
        std::wcout << std::fixed << std::setprecision(2);
        std::wcout << L"Testing with a = " << a << L", b = " << b << std::endl;
        std::wcout << std::endl;

        // Test all operations
        double result = Add(a, b);
        std::wcout << L"Add(" << a << L", " << b << L") = " << result << std::endl;

        result = Subtract(a, b);
        std::wcout << L"Subtract(" << a << L", " << b << L") = " << result << std::endl;

        result = Multiply(a, b);
        std::wcout << L"Multiply(" << a << L", " << b << L") = " << result << std::endl;

        result = Divide(a, b);
        std::wcout << L"Divide(" << a << L", " << b << L") = " << result << std::endl;

        // Test division by zero
        std::wcout << std::endl;
        result = Divide(a, 0.0);
        std::wcout << L"Divide(" << a << L", 0.0) = " << result << L" (division by zero test)" << std::endl;

        a += 1.0;
        b += 0.5;

		// sleep 100 ms
        std::wcout << std::endl;
        std::wcout << L"Waiting 1ms before next test..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::wcout << std::endl;

		remain--;
    }
        
    std::wcout << std::endl;
    std::wcout << L"Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}