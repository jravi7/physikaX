#include "utility/camera.h"

namespace physika::utility {

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up,
               float fov, float near, float far, float aspectRatio)
    : mPostion(position),
      mTarget(target),
      mUp(up),
      mFov(fov),
      mNear(near),
      mFar(far),
      mAspectRatio(aspectRatio)
{
}
DirectX::XMFLOAT4X4 Camera::View() const
{
    XMFLOAT4X4 view4x4;
    XMStoreFloat4x4(&view4x4, XMMatrixLookAtLH(XMLoadFloat3(&mPostion), XMLoadFloat3(&mTarget),
                                               XMLoadFloat3(&mUp)));
    return view4x4;
}
DirectX::XMFLOAT4X4 Camera::Projection() const
{
    XMFLOAT4X4 proj4x4;
    XMStoreFloat4x4(&proj4x4, XMMatrixPerspectiveFovLH(mFov, mAspectRatio, mNear, mFar));
    return proj4x4;
}
DirectX::XMFLOAT4X4 Camera::Matrix() const
{
    XMFLOAT4X4  matrix4x4;
    auto const& viewFloat4x4 = View();
    auto const& projFloat4x4 = Projection();
    XMStoreFloat4x4(&matrix4x4,
                    XMMatrixMultiply(XMLoadFloat4x4(&viewFloat4x4), XMLoadFloat4x4(&projFloat4x4)));
    return matrix4x4;
}

}  // namespace physika::utility
