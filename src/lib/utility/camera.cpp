#include "utility/camera.h"

namespace {

double const kPI = 3.14159265358979323846;

float RadiansToDeg(float radians)
{
    return radians * 180 / (float)kPI;
}

}  // namespace

namespace physika::utility {

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera()
    : mPosition(), mTarget(), mUp(), mFovY(0.0f), mNear(0.0f), mFar(0.0f), mAspectRatio(0.0f)
{
    mProjection  = Matrix();
    mView        = Matrix();
    mWorldMatrix = Matrix();
}

void Camera::SetPosition(DirectX::SimpleMath::Vector3 const& position)
{
    mPosition = position;
    mWorldMatrix.Translation(mPosition);
    WorldMatrixChanged();
}
void Camera::SetXRotation(float angle)
{
    mRotX = angle;
}
void Camera::SetYRotation(float angle)
{
    mRotY = angle;
}

void Camera::WorldMatrixChanged()
{
    mView = mWorldMatrix.Invert();
    Vector3    scale;
    Quaternion orientation;
    Vector3    translation;
    assert(mWorldMatrix.Decompose(scale, orientation, translation) &&
           "Malformed Camera WorldMatrix");

    Vector3 euler;
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

void Camera::OffsetPosition(Vector3 const& offset)
{
    mPosition += offset;
    mWorldMatrix.Translation(mPosition);
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

void Camera::OnMouseUp()
{
    mIsMouseDown = false;
}
void Camera::OnMouseDown()
{
    mIsMouseDown = true;
}
void Camera::OnMouseMove(float x, float y)
{
    float dx = (x - mX);
    float dy = (y - mY);
    mX       = x;
    mY       = y;
    (void)dx;
    (void)dy;
}

}  // namespace physika::utility
