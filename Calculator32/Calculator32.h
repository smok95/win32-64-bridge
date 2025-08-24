#pragma once

#ifdef CALCULATOR32_EXPORTS
#define CALCULATOR32_API __declspec(dllexport)
#else
#define CALCULATOR32_API __declspec(dllimport)
#endif

extern "C" {
    // ���� 4Ģ ���� �Լ���
    CALCULATOR32_API double Add(double a, double b);
    CALCULATOR32_API double Subtract(double a, double b);
    CALCULATOR32_API double Multiply(double a, double b);
    CALCULATOR32_API double Divide(double a, double b);
}