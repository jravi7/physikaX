#pragma once

#include <DirectXMath.h>

#include <vector>

namespace physika::common {

struct VertexData
{
    VertexData() = default;
    VertexData(DirectX::XMFLOAT3 const& p, DirectX::XMFLOAT4 const& c) : position(p), color(c)
    {
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

struct MeshData
{
    std::vector<VertexData> vertices;
    std::vector<uint32_t>   indices;

    size_t PerVertexDataSize() const
    {
        return sizeof(VertexData);
    }

    size_t IndexDataSize() const
    {
        return sizeof(uint32_t);
    }
};

}  // namespace physika::common