// Based on TheRealMJP's Camera
// https://github.com/TheRealMJP/DXRPathTracer/blob/master/SampleFramework12/v1.02/Graphics/Camera.cpp

#pragma once

#include <DirectXMath.h>
#include <SimpleMath.h>

namespace physika::utility {
class Camera
{
public:
    Camera();
    void SetCameraProperties(float n, float f, float fovY, float aspectRatio);
    void SetPosition(DirectX::SimpleMath::Vector3 const& position);
    void SetLookAt(DirectX::SimpleMath::Vector3 const& target, DirectX::SimpleMath::Vector3 const& up,
                   DirectX::SimpleMath::Vector3 const& position);

    void  SetXRotation(float radians);
    void  SetYRotation(float radians);
    float XRotation();
    float YRotation();

    DirectX::SimpleMath::Vector3 Position();

    DirectX::SimpleMath::Vector3 Up();
    DirectX::SimpleMath::Vector3 Forward();
    DirectX::SimpleMath::Vector3 Right();

    DirectX::SimpleMath::Matrix View();
    DirectX::SimpleMath::Matrix Projection();
    DirectX::SimpleMath::Matrix ViewProjection();

private:
    void WorldMatrixChanged();

    float mRotationX;
    float mRotationY;

    // Camera properties
    float mNear;
    float mFar;
    float mAspectRatio;
    float mFovY;
    float mSpeed;

    // Matrices
    DirectX::SimpleMath::Matrix mProjection;
    DirectX::SimpleMath::Matrix mView;
    DirectX::SimpleMath::Matrix mWorldMatrix;

    // Positions
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mTarget;

    // View vectors
    DirectX::SimpleMath::Vector3 mUp;
    DirectX::SimpleMath::Vector3 mFront;
    DirectX::SimpleMath::Vector3 mRight;
};

}  // namespace physika::utility
