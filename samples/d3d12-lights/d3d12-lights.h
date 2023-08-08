#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>  // uint32_t

#include <memory>  // unique_ptr
#include <vector>

#include "core/application.h"
#include "core/input.h"
#include "core/timer.h"
#include "frame-resource.h"
#include "graphics/helpers.h"
#include "graphics/types.h"
#include "renderer/camera.h"

namespace sample {

using physika::core::Keycode;
using physika::core::MouseButton;

struct InputStates
{
    bool  isMouseDown                            = {};
    bool  keyState[physika::core::kKeyCodeCount] = {};
    float mouseX                                 = {};
    float mouseY                                 = {};
};

class D3D12Lights : public physika::core::Application
{
public:
    D3D12Lights(TCHAR const* const title, int width, int height);
    ~D3D12Lights();

    bool Initialize();
    bool Shutdown();

    void OnUpdate() override;
    void OnResize(int width, int height) override;
    void OnKeyUp(Keycode key) override;
    void OnKeyDown(Keycode key) override;
    void OnMouseUp(MouseButton button, int x, int y) override;
    void OnMouseDown(MouseButton button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseWheel(int delta) override;

private:
    void InitializeDeviceObjects();
    void InitializeCommandObjects();

    // RTs
    void InitializeSwapChain();
    void CreateDepthStencilBuffer();

    // Resources
    void InitializeSceneCamera();
    void InitializeSceneGeometry();
    void InitializeSceneMaterials();
    void InitializeFrameResources();
    void InitializeRenderItems();
    void CreateDescriptorHeaps();
    void CreateDescriptorViews();
    void CreateConstantBufferViews();
    void CreateRenderTargetViews();
    void ResizeViewportAndScissorRect();
    void CreateRootSignatures();
    void InitializePSOs();

    void FlushCommandQueue();
    void Update();
    void Draw();

    void ProcessKeyStates();

    uint32_t    mSwapChainBufferCount;
    uint32_t    mCurrentBackBuffer;
    DXGI_FORMAT mBackBufferFormat;

    // Device Objects
    physika::graphics::ID3D12DevicePtr              mD3D12Device;
    physika::graphics::IDXGIFactory1Ptr             mDXGIFactory;
    physika::graphics::IDXGIAdapter1Ptr             mDXGIAdapter;
    physika::graphics::ID3D12CommandQueuePtr        mCommandQueue;
    physika::graphics::ID3D12GraphicsCommandListPtr mGraphicsCommandList;
    physika::graphics::ID3D12CommandAllocatorPtr    mCommandAllocator;
    physika::graphics::ID3D12Debug1Ptr              mD3D12DebugController;
    physika::graphics::ID3D12DescriptorHeapPtr      mRtvHeap;
    physika::graphics::ID3D12DescriptorHeapPtr      mDsvHeap;
    physika::graphics::ID3D12DescriptorHeapPtr      mCBVHeap;

    //! Window surface objects
    physika::graphics::IDXGISwapChainPtr              mSwapChain;
    std::vector<physika::graphics::ID3D12ResourcePtr> mSwapChainBackBuffers;
    physika::graphics::ID3D12ResourcePtr              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    physika::core::Timer              mTimer;
    uint64_t                          mCurrentFrameIndex;
    uint64_t                          mFenceValue;
    physika::graphics::ID3D12FencePtr mFence;

    //! Resources
    physika::graphics::ID3D12PipelineStatePtr            mPipelineState;
    physika::graphics::ID3D12RootSignaturePtr            mRootSignature;
    std::vector<std::shared_ptr<physika::FrameResource>> mFrameResources;
    std::shared_ptr<physika::FrameResource>              mCurrentFrameResource;

    //! Scene resources
    uint32_t                                                                      mNumDescriptorsPerFrame;
    uint32_t                                                                      mPerPassDescriptorOffset;
    uint32_t                                                                      mMaterialDescriptorOffset;
    std::unordered_map<std::string, std::shared_ptr<physika::renderer::Mesh>>     mMeshBuffers;
    std::unordered_map<std::string, std::shared_ptr<physika::renderer::Material>> mMaterials;
    std::vector<std::shared_ptr<physika::RenderItem>>                             mSceneObjects;
    std::vector<physika::renderer::Light>                                         mDirectionalLights;
    std::vector<physika::renderer::Light>                                         mPointLights;
    std::vector<physika::renderer::Light>                                         mSpotLights;

    physika::renderer::Camera mCamera;

    //! Input
    InputStates mInputStates;
};

}  // namespace sample
