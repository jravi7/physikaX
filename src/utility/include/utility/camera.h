#include <DirectXMath.h>

namespace physika::utility{

namespace dx = DirectX; 

class Camera
{

public: 
    Camera(dx::XMFLOAT3 position, dx::XMFLOAT3 target, dx::XMFLOAT3 up); 

    dx::XMFLOAT4X4 View(); 
    dx::XMFLOAT4X4 Projection(); 
private: 

    dx::XMFLOAT3 mPostion;
    dx::XMFLOAT3 mTarget;
    dx::XMFLOAT3 mUp;
};

}
