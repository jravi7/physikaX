#pragma once

#include <vector>
#include <DirectXMath.h>

namespace physika::common {

struct VertexData
{

    VertexData () = default; 
    VertexData (DirectX::XMFLOAT3 const& p, DirectX::XMFLOAT4 const& c): position(p), color(c){}

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

struct MeshData
{
    
std::vector<VertexData> vertices; 
std::vector<uint32_t> indices; 

};

}  // namespace physika::common