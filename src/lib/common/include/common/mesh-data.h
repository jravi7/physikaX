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

    constexpr static size_t PerVertexDataSize()
    {
        return sizeof(VertexData);
    }

    constexpr static size_t IndexDataSize()
    {
        return sizeof(uint32_t);
    }

    size_t VertexBufferSize() const
    {
        return vertices.size() * PerVertexDataSize();
    }

    size_t IndexBufferSize() const
    {
        return indices.size() * IndexDataSize();
    }
};

}  // namespace physika::common