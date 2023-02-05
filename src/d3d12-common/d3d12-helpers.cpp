#include "d3d12-common/d3d12-helpers.h"

#include <comdef.h>  // _com_error
#include <d3dx12.h>

#include "logger/logger.h"

namespace physika {
namespace d3d12_util {

char const* HRErrorDescription(HRESULT hr)
{
    _com_error err(hr);
    return err.ErrorMessage();
}
void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        physika::logger::LOG_ERROR("%s", HRErrorDescription(hr));
        throw std::exception();
    }
}

}  // namespace d3d12_util
}  // namespace physika