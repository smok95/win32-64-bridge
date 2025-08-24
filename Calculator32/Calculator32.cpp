#define CALCULATOR32_EXPORTS
#include "Calculator32.h"
#include <stdexcept>

extern "C" {
    CALCULATOR32_API double Add(double a, double b) {
        return a + b;
    }

    CALCULATOR32_API double Subtract(double a, double b) {
        return a - b;
    }

    CALCULATOR32_API double Multiply(double a, double b) {
        return a * b;
    }

    CALCULATOR32_API double Divide(double a, double b) {
        if (b == 0.0) {
            return 0.0; // Division by zero handling
        }
        return a / b;
    }
}