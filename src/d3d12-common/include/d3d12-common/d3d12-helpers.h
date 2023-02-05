#include <d3d12.h>
#include <inttypes.h>    // uint64_t
#include <wrl/client.h>  // ComPtr

namespace physika {
namespace d3d12_common {

//! @brief Returns a string representation of a HRESULT param.
char const* HRErrorDescription(HRESULT hr);

//! @brief throw an exception on HRESULT failure
void ThrowIfFailed(HRESULT hr);



}  // namespace d3d12_common
}  // namespace physika