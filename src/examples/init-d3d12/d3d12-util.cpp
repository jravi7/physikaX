#include <comdef.h>

#include "d3d12-util.h"

namespace directx {

char const* HRErrorDescription(HRESULT hr)
{
    _com_error err(hr);
    return err.ErrorMessage();
}

ComException::ComException(HRESULT hr)
    : mResult(hr)
{
}

const char* ComException::what() const
{
    _com_error comError(mResult);
    snprintf((char*)mErrorString, 256, "HResult error: %s",
             comError.ErrorMessage());
    return mErrorString;
}

}  // namespace directx