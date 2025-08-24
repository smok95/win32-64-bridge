#pragma once

#ifdef CALCULATOR32_EXPORTS
#define CALCULATOR32_API __declspec(dllexport)
#else
#define CALCULATOR32_API __declspec(dllimport)
#endif

extern "C" {
    // 계산기 4칙 연산 함수들
    CALCULATOR32_API double Add(double a, double b);
    CALCULATOR32_API double Subtract(double a, double b);
    CALCULATOR32_API double Multiply(double a, double b);
    CALCULATOR32_API double Divide(double a, double b);
}