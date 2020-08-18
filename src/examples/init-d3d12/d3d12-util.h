#pragma once

#include <exception>
#include <iostream>
namespace directx {

char const* HRErrorDescription(HRESULT hr);
/*
 * Based on
 * https://github.com/Microsoft/DirectXTK/wiki/ThrowIfFailed#enhancements
 */
class ComException : public std::exception
{
public:
    ComException(HRESULT hr);
    const char* what() const override;

private:
    HRESULT mResult;
    char    mErrorString[256];
};

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        std::cout << HRErrorDescription(hr) << std::endl;
        throw ComException(hr);
    }
}

}  // namespace directx