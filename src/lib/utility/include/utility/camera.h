#pragma once

#include <DirectXMath.h>

namespace physika::utility {

class Camera
{
public:
    Camera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up, float fov,
           float near, float far, float aspectRatio);

    DirectX::XMFLOAT4X4 View() const;
    DirectX::XMFLOAT4X4 Projection() const;
    DirectX::XMFLOAT4X4 Matrix() const;

private:
    float             mNear;
    float             mFar;
    float             mAspectRatio;
    float             mFov;
    DirectX::XMFLOAT3 mPostion;
    DirectX::XMFLOAT3 mTarget;
    DirectX::XMFLOAT3 mUp;
};

}  // namespace physika::utility
