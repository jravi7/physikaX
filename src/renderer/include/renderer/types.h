#pragma once

#include <DirectXMath.h>
#include <inttypes.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "graphics/types.h"

namespace physika::renderer {

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

struct Submesh
{
    uint32_t indexCount          = 0;
    uint32_t vertexStartLocation = 0;
    uint32_t indexStartLocation  = 0;
};

struct Mesh
{
    std::string                              name;
    std::unordered_map<std::string, Submesh> submeshes;

    graphics::ID3DBlobPtr vertexBufferCPU = nullptr;
    graphics::ID3DBlobPtr indexBufferCPU  = nullptr;

    graphics::ID3D12ResourcePtr vertexBufferGPU = nullptr;
    graphics::ID3D12ResourcePtr indexBufferGPU  = nullptr;

    graphics::ID3D12ResourcePtr vertexBufferUploadHeap = nullptr;
    graphics::ID3D12ResourcePtr indexBufferUploadHeap  = nullptr;

    DXGI_FORMAT indexFormat          = DXGI_FORMAT_UNKNOWN;
    UINT        indexBufferByteSize  = 0;
    uint32_t    vertexByteStride     = 0;
    uint32_t    vertexBufferByteSize = 0;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes  = vertexByteStride;
        vbv.SizeInBytes    = vertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
        ibv.Format         = indexFormat;
        ibv.SizeInBytes    = indexBufferByteSize;

        return ibv;
    }
    void DisposeUploaders()
    {
        vertexBufferUploadHeap = nullptr;
        indexBufferUploadHeap  = nullptr;
    }
};

struct Material
{
    std::string       name;
    int               cbHeapIndex         = -1;
    int               diffuseSrvHeapIndex = -1;
    int               normalHeapIndex     = -1;
    DirectX::XMFLOAT4 diffuseAlbedo       = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 fresnel             = { 0.01f, 0.01f, 0.01f };
    float             roughness           = 0.25f;  // 0 being shiny and 1.0 for maximum roughness
    int               numFramesDirty      = -1;     // Not initialized
};

struct Light
{
    DirectX::XMFLOAT3 position;
    float             fallOffStart;  // 16
    DirectX::XMFLOAT3 direction;
    float             fallOffend;  // 32
    DirectX::XMFLOAT3 strength;
    float             innerCutOffAngle;  // 48 cosine of angle in degrees
    DirectX::XMFLOAT3 ambientStrength;
    float             outerCutOffAngle;  // 64
};
}  // namespace physika::renderer
