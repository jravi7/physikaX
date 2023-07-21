#include "utility/camera.h"

#include <algorithm>

namespace physika::utility {

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera()
    : mFovY(0.0f),
      mNear(0.0f),
      mFar(0.0f),
      mAspectRatio(0.0f),
      mRotationX(0.0f),
      mRotationY(0.0f),
      mPosition(Vector3()),
      mTarget(Vector3()),
      mUp(Vector3()),
      mProjection(Matrix()),
      mView(Matrix()),
      mWorldMatrix(Matrix())
{
}

void Camera::SetPosition(DirectX::SimpleMath::Vector3 const& position)
{
    mPosition = position;
    mWorldMatrix.Translation(mPosition);
    WorldMatrixChanged();
}
void Camera::SetXRotation(float radians)
{
    mRotationX = std::clamp(radians, -XM_PIDIV2, XM_PIDIV2);
    Quaternion orientation =
        Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(mRotationX, mRotationY, 0));
    mWorldMatrix = Matrix::CreateFromQuaternion(orientation);
    mWorldMatrix.Translation(mPosition);
    WorldMatrixChanged();
}
void Camera::SetYRotation(float radians)
{
    mRotationY = DirectX::XMScalarModAngle(radians);
    Quaternion orientation =
        Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(mRotationX, mRotationY, 0));

    mWorldMatrix = Matrix::CreateFromQuaternion(orientation);
    mWorldMatrix.Translation(mPosition);
    WorldMatrixChanged();
}

float Camera::XRotation()
{
    return mRotationX;
}
float Camera::YRotation()
{
    return mRotationY;
}

void Camera::WorldMatrixChanged()
{
    mView = mWorldMatrix.Invert();
}

void Camera::SetLookAt(DirectX::SimpleMath::Vector3 const& target,
                       DirectX::SimpleMath::Vector3 const& up,
                       DirectX::SimpleMath::Vector3 const& position)
{
    mPosition = position;
    mTarget   = target;
    mUp       = up;

    mFront = mTarget - mPosition;  // z
    mFront.Normalize();
    mRight = mUp.Cross(mFront);     // x
    mUp    = mFront.Cross(mRight);  // y

    DirectX::XMVECTOR targetSIMD   = XMLoadFloat3(&mTarget);
    DirectX::XMVECTOR positionSIMD = XMLoadFloat3(&mPosition);
    DirectX::XMVECTOR upSIMD       = XMLoadFloat3(&mUp);

    DirectX::XMMATRIX viewSIMD = DirectX::XMMatrixLookAtLH(positionSIMD, targetSIMD, upSIMD);
    XMStoreFloat4x4(&mView, viewSIMD);
    mWorldMatrix = mView.Invert();

    Vector3    scale;
    Quaternion orientation;
    Vector3    translation;
    assert(mWorldMatrix.Decompose(scale, orientation, translation) &&
           "Malformed Camera WorldMatrix");

    Vector3 euler = orientation.ToEuler();
    mRotationX    = euler.x;
    mRotationY    = euler.y;

    // Leaving this as a reference.
    // // Row Major LHS
    // // Row 1
    // mView(0, 0) = mRight.x;
    // mView(0, 1) = mRight.y;
    // mView(0, 2) = mRight.z;

    // // Row 2
    // mView(1, 0) = mUp.x;
    // mView(1, 1) = mUp.y;
    // mView(1, 2) = mUp.z;

    // // Row 3
    // mView(2, 0) = mFront.x;
    // mView(2, 1) = mFront.y;
    // mView(2, 2) = mFront.z;

    // // Translation
    // mView(3, 0) = -mPosition.x;
    // mView(3, 1) = -mPosition.y;
    // mView(3, 2) = -mPosition.z;
}

void Camera::SetCameraProperties(float n, float f, float fovY, float aspectRatio)
{
    mNear             = n;
    mFar              = f;
    mFovY             = fovY;
    mAspectRatio      = aspectRatio;
    float radiansFovY = XMConvertToRadians(fovY);
    XMStoreFloat4x4(&mProjection, XMMatrixPerspectiveFovLH(radiansFovY, mAspectRatio, mNear, mFar));
}
Matrix Camera::View()
{
    return mView;
}
Matrix Camera::Projection()
{
    return mProjection;
}
Matrix Camera::ViewProjection()
{
    return mView * mProjection;
}

Vector3 Camera::Position()
{
    return mPosition;
}

Vector3 Camera::Up()
{
    return mWorldMatrix.Up();
}
Vector3 Camera::Forward()
{
    return mWorldMatrix.Forward();
}
Vector3 Camera::Right()
{
    return mWorldMatrix.Right();
}

}  // namespace physika::utility
