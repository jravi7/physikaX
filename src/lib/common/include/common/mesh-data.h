#pragma once

#include <DirectXMath.h>

#include <vector>

namespace physika::common {

struct VertexData
{
    VertexData() = default;
    VertexData(DirectX::XMFLOAT3 const& p, DirectX::XMFLOAT3 const& n, DirectX::XMFLOAT3 const& t, DirectX::XMFLOAT2 const& uv,
               DirectX::XMFLOAT4 c)
        : position(p), normal(n), tangent(t), texcoord(uv), color(c)
    {
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT2 texcoord;
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