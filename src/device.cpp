
#include "device.hpp"

#include <array>
#include <d3d9.h>

#include "glew.h"
#include "wglew.h"
#include "trace.hpp"
#include "d3dgl.hpp"
#include "swapchain.hpp"
#include "rendertarget.hpp"
#include "adapter.hpp"
#include "texture.hpp"
#include "bufferobject.hpp"


namespace
{

template<typename T>
bool fmt_to_glattrs(D3DFORMAT fmt, T inserter)
{
    switch(fmt)
    {
        case D3DFMT_X8R8G8B8:
            *inserter = {WGL_RED_BITS_ARB, 8};
            *inserter = {WGL_GREEN_BITS_ARB, 8};
            *inserter = {WGL_BLUE_BITS_ARB, 8};
            *inserter = {WGL_COLOR_BITS_ARB, 32};
            return true;
        case D3DFMT_D24S8:
            *inserter = {WGL_DEPTH_BITS_ARB, 24};
            *inserter = {WGL_STENCIL_BITS_ARB, 8};
            return true;

        default:
            ERR("Unhandled D3DFORMAT: 0x%x\n", fmt);
            break;
    }
    return false;
}

DWORD float_to_dword(float f)
{
    union {
        float f;
        DWORD d;
    } tmpfloat = { f };
    return tmpfloat.d;
}
std::array<DWORD,210> GenerateDefaultRSValues()
{
    std::array<DWORD,210> ret;
    ret[D3DRS_ZENABLE] = D3DZB_TRUE;
    ret[D3DRS_FILLMODE] = D3DFILL_SOLID;
    ret[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
    ret[D3DRS_ZWRITEENABLE] = TRUE;
    ret[D3DRS_ALPHATESTENABLE] = FALSE;
    ret[D3DRS_LASTPIXEL] = TRUE;
    ret[D3DRS_SRCBLEND] = D3DBLEND_ONE;
    ret[D3DRS_DESTBLEND] = D3DBLEND_ZERO;
    ret[D3DRS_CULLMODE] = D3DCULL_CCW;
    ret[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
    ret[D3DRS_ALPHAFUNC] = D3DCMP_ALWAYS;
    ret[D3DRS_ALPHAREF] = 0;
    ret[D3DRS_DITHERENABLE] = FALSE;
    ret[D3DRS_ALPHABLENDENABLE] = FALSE;
    ret[D3DRS_FOGENABLE] = FALSE;
    ret[D3DRS_SPECULARENABLE] = FALSE;
    ret[D3DRS_FOGCOLOR] = 0;
    ret[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
    ret[D3DRS_FOGSTART] = float_to_dword(0.0f);
    ret[D3DRS_FOGEND] = float_to_dword(1.0f);
    ret[D3DRS_FOGDENSITY] = float_to_dword(1.0f);
    ret[D3DRS_RANGEFOGENABLE] = FALSE;
    ret[D3DRS_STENCILENABLE] = FALSE;
    ret[D3DRS_STENCILFAIL] = D3DSTENCILOP_KEEP;
    ret[D3DRS_STENCILZFAIL] = D3DSTENCILOP_KEEP;
    ret[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;
    ret[D3DRS_STENCILREF] = 0;
    ret[D3DRS_STENCILMASK] = 0xffffffff;
    ret[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
    ret[D3DRS_STENCILWRITEMASK] = 0xffffffff;
    ret[D3DRS_TEXTUREFACTOR] = 0xffffffff;
    ret[D3DRS_WRAP0] = 0;
    ret[D3DRS_WRAP1] = 0;
    ret[D3DRS_WRAP2] = 0;
    ret[D3DRS_WRAP3] = 0;
    ret[D3DRS_WRAP4] = 0;
    ret[D3DRS_WRAP5] = 0;
    ret[D3DRS_WRAP6] = 0;
    ret[D3DRS_WRAP7] = 0;
    ret[D3DRS_CLIPPING] = TRUE;
    ret[D3DRS_LIGHTING] = TRUE;
    ret[D3DRS_AMBIENT] = 0;
    ret[D3DRS_FOGVERTEXMODE] = D3DFOG_NONE;
    ret[D3DRS_COLORVERTEX] = TRUE;
    ret[D3DRS_LOCALVIEWER] = TRUE;
    ret[D3DRS_NORMALIZENORMALS] = FALSE;
    ret[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
    ret[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
    ret[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_MATERIAL;
    ret[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
    ret[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
    ret[D3DRS_CLIPPLANEENABLE] = 0;
    ret[D3DRS_POINTSIZE] = float_to_dword(1.0f);
    ret[D3DRS_POINTSIZE_MIN] = float_to_dword(1.0f);
    ret[D3DRS_POINTSPRITEENABLE] = FALSE;
    ret[D3DRS_POINTSCALEENABLE] = FALSE;
    ret[D3DRS_POINTSCALE_A] = float_to_dword(1.0f);
    ret[D3DRS_POINTSCALE_B] = float_to_dword(0.0f);
    ret[D3DRS_POINTSCALE_C] = float_to_dword(0.0f);
    ret[D3DRS_MULTISAMPLEANTIALIAS] = TRUE;
    ret[D3DRS_MULTISAMPLEMASK] = 0xffffffff;
    ret[D3DRS_PATCHEDGESTYLE] = D3DPATCHEDGE_DISCRETE;
    ret[D3DRS_DEBUGMONITORTOKEN] = 0xbaadcafe;
    ret[D3DRS_POINTSIZE_MAX] = float_to_dword(1.0f);
    ret[D3DRS_INDEXEDVERTEXBLENDENABLE] = FALSE;
    ret[D3DRS_COLORWRITEENABLE] = 0x0000000f;
    ret[D3DRS_TWEENFACTOR] = float_to_dword(0.0f);
    ret[D3DRS_BLENDOP] = D3DBLENDOP_ADD;
    ret[D3DRS_POSITIONDEGREE] = D3DDEGREE_CUBIC;
    ret[D3DRS_NORMALDEGREE] = D3DDEGREE_LINEAR;
    ret[D3DRS_SCISSORTESTENABLE] = FALSE;
    ret[D3DRS_SLOPESCALEDEPTHBIAS] = 0;
    ret[D3DRS_MINTESSELLATIONLEVEL] = float_to_dword(1.0f);
    ret[D3DRS_MAXTESSELLATIONLEVEL] = float_to_dword(1.0f);
    ret[D3DRS_ANTIALIASEDLINEENABLE] = FALSE;
    ret[D3DRS_ADAPTIVETESS_X] = float_to_dword(0.0f);
    ret[D3DRS_ADAPTIVETESS_Y] = float_to_dword(0.0f);
    ret[D3DRS_ADAPTIVETESS_Z] = float_to_dword(1.0f);
    ret[D3DRS_ADAPTIVETESS_W] = float_to_dword(0.0f);
    ret[D3DRS_ENABLEADAPTIVETESSELLATION] = FALSE;
    ret[D3DRS_TWOSIDEDSTENCILMODE] = FALSE;
    ret[D3DRS_CCW_STENCILFAIL] = D3DSTENCILOP_KEEP;
    ret[D3DRS_CCW_STENCILZFAIL] = D3DSTENCILOP_KEEP;
    ret[D3DRS_CCW_STENCILPASS] = D3DSTENCILOP_KEEP;
    ret[D3DRS_CCW_STENCILFUNC] = D3DCMP_ALWAYS;
    ret[D3DRS_COLORWRITEENABLE1] = 0x0000000f;
    ret[D3DRS_COLORWRITEENABLE2] = 0x0000000f;
    ret[D3DRS_COLORWRITEENABLE3] = 0x0000000f;
    ret[D3DRS_BLENDFACTOR] = 0xffffffff;
    ret[D3DRS_SRGBWRITEENABLE] = 0;
    ret[D3DRS_DEPTHBIAS] = 0;
    ret[D3DRS_WRAP8] = 0;
    ret[D3DRS_WRAP9] = 0;
    ret[D3DRS_WRAP10] = 0;
    ret[D3DRS_WRAP11] = 0;
    ret[D3DRS_WRAP12] = 0;
    ret[D3DRS_WRAP13] = 0;
    ret[D3DRS_WRAP14] = 0;
    ret[D3DRS_WRAP15] = 0;
    ret[D3DRS_SEPARATEALPHABLENDENABLE] = FALSE;
    ret[D3DRS_SRCBLENDALPHA] = D3DBLEND_ONE;
    ret[D3DRS_DESTBLENDALPHA] = D3DBLEND_ZERO;
    ret[D3DRS_BLENDOPALPHA] = D3DBLENDOP_ADD;
    return ret;
}
static const std::array<DWORD,210> DefaultRSValues = GenerateDefaultRSValues();


class StateEnable : public Command {
    GLenum mState;
    bool mEnable;

public:
    StateEnable(GLenum state, bool enable) : mState(state), mEnable(enable) { }

    virtual ULONG execute()
    {
        if(mEnable)
            glEnable(mState);
        else
            glDisable(mState);
        return sizeof(*this);
    }
};

class MaterialSet : public Command {
    float mShininess;
    float mDiffuse[4];
    float mAmbient[4];
    float mSpecular[4];
    float mEmission[4];

public:
    MaterialSet(const D3DMATERIAL9 &material)
      : mShininess(material.Power)
      , mDiffuse{material.Diffuse.r, material.Diffuse.g, material.Diffuse.b, material.Diffuse.a}
      , mAmbient{material.Ambient.r, material.Ambient.g, material.Ambient.b, material.Ambient.a}
      , mSpecular{material.Specular.r, material.Specular.g, material.Specular.b, material.Specular.a}
      , mEmission{material.Emissive.r, material.Emissive.g, material.Emissive.b, material.Emissive.a}
    { }

    virtual ULONG execute()
    {
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mEmission);
        return sizeof(*this);
    }
};

} // namespace


D3DGLDevice::D3DGLDevice(Direct3DGL *parent, const D3DAdapter &adapter, HWND window, DWORD flags)
  : mRefCount(0)
  , mParent(parent)
  , mAdapter(adapter)
  , mGLContext(nullptr)
  , mWindow(window)
  , mFlags(flags)
  , mAutoDepthStencil(nullptr)
  , mDepthStencil(nullptr)
  , mInScene(false)
{
    std::copy(DefaultRSValues.begin(), DefaultRSValues.end(), mRenderState.begin());
    mRenderState[D3DRS_POINTSIZE_MAX] = float_to_dword(mAdapter.getLimits().pointsize_max);
}

D3DGLDevice::~D3DGLDevice()
{
    for(auto schain : mSwapchains)
        delete schain;
    mSwapchains.clear();
    delete mAutoDepthStencil;
    mAutoDepthStencil = nullptr;

    mQueue.deinit();
    if(mGLContext)
        wglDeleteContext(mGLContext);
    mGLContext = nullptr;

    mParent = nullptr;
}

bool D3DGLDevice::init(D3DPRESENT_PARAMETERS *params)
{
    if(params->BackBufferCount > 1)
    {
        WARN("Too many backbuffers requested (%u)\n", params->BackBufferCount);
        params->BackBufferCount = 1;
        return false;
    }

    if((params->Flags&D3DPRESENTFLAG_LOCKABLE_BACKBUFFER))
    {
        FIXME("Lockable backbuffer not currently supported\n");
        return false;
    }

    std::vector<std::array<int,2>> glattrs;
    glattrs.reserve(16);
    glattrs.push_back({WGL_DRAW_TO_WINDOW_ARB, GL_TRUE});
    glattrs.push_back({WGL_SUPPORT_OPENGL_ARB, GL_TRUE});
    glattrs.push_back({WGL_DOUBLE_BUFFER_ARB, GL_TRUE});
    glattrs.push_back({WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB});
    if(!fmt_to_glattrs(params->BackBufferFormat, std::back_inserter(glattrs)))
        return false;
    if(params->EnableAutoDepthStencil)
    {
        if(!fmt_to_glattrs(params->AutoDepthStencilFormat, std::back_inserter(glattrs)))
            return false;
    }
    // Got all attrs
    glattrs.push_back({0, 0});

    HWND win = ((params->Windowed && !params->hDeviceWindow) ? mWindow : params->hDeviceWindow);
    HDC hdc = GetDC(win);

    int pixelFormat;
    UINT numFormats;
    if(!wglChoosePixelFormatARB(hdc, &glattrs[0][0], NULL, 1, &pixelFormat, &numFormats))
    {
        ERR("Failed to choose a pixel format\n");
        ReleaseDC(win, hdc);
        return false;
    }
    if(numFormats < 1)
    {
        ERR("No suitable pixel formats found\n");
        ReleaseDC(win, hdc);
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if(SetPixelFormat(hdc, pixelFormat, &pfd) == 0)
    {
        ERR("Failed to set a pixel format, error %lu\n", GetLastError());
        ReleaseDC(win, hdc);
        return false;
    }

    mGLContext = wglCreateContextAttribsARB(hdc, nullptr, nullptr);
    if(!mGLContext)
    {
        ERR("Failed to create OpenGL context, error %lu\n", GetLastError());
        ReleaseDC(win, hdc);
        return false;
    }
    ReleaseDC(win, hdc);

    if(!mQueue.init(win, mGLContext))
        return false;

    D3DGLSwapChain *schain = new D3DGLSwapChain(this);
    if(!schain->init(params, win, true))
    {
        delete schain;
        return false;
    }
    mSwapchains.push_back(schain);

    if(params->EnableAutoDepthStencil)
    {
        D3DSURFACE_DESC desc;
        desc.Format = params->AutoDepthStencilFormat;
        desc.Type = D3DRTYPE_SURFACE;
        desc.Usage = D3DUSAGE_DEPTHSTENCIL;
        desc.Pool = D3DPOOL_DEFAULT;
        desc.MultiSampleType = params->MultiSampleType;
        desc.MultiSampleQuality = params->MultiSampleQuality;
        desc.Width = params->BackBufferWidth;
        desc.Height = params->BackBufferHeight;

        mAutoDepthStencil = new D3DGLRenderTarget(this);
        if(!mAutoDepthStencil->init(&desc, true))
            return false;
        mDepthStencil = mAutoDepthStencil;
    }

    return true;
}


HRESULT D3DGLDevice::QueryInterface(const IID &riid, void **obj)
{
    TRACE("iface %p, riid %s, obj %p.\n", this, debugstr_guid(riid), obj);

    if(riid == IID_IDirect3DDevice9 || riid == IID_IUnknown)
    {
        AddRef();
        *obj = static_cast<IDirect3DDevice9*>(this);
        return S_OK;
    }

    if(riid == IID_IDirect3DDevice9Ex)
    {
        WARN("IDirect3D9 instance wasn't created with CreateDirect3D9Ex, returning E_NOINTERFACE.\n");
        *obj = NULL;
        return E_NOINTERFACE;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(riid));
    *obj = NULL;
    return E_NOINTERFACE;
}

ULONG D3DGLDevice::AddRef(void)
{
    ULONG ret = ++mRefCount;
    TRACE("%p New refcount: %lu\n", this, ret);
    return ret;
}

ULONG D3DGLDevice::Release(void)
{
    ULONG ret = --mRefCount;
    TRACE("%p New refcount: %lu\n", this, ret);
    if(ret == 0) delete this;
    return ret;
}


HRESULT D3DGLDevice::TestCooperativeLevel()
{
    FIXME("iface %p : stub!\n", this);
    return D3D_OK;
}

UINT D3DGLDevice::GetAvailableTextureMem()
{
    FIXME("iface %p : stub!\n", this);
    return 0;
}

HRESULT D3DGLDevice::EvictManagedResources()
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetDirect3D(IDirect3D9 **d3d9)
{
    TRACE("iface %p, d3d9 %p\n", this, d3d9);
    *d3d9 = mParent;
    (*d3d9)->AddRef();
    return D3D_OK;
}

HRESULT D3DGLDevice::GetDeviceCaps(D3DCAPS9 *caps)
{
    TRACE("iface %p, caps %p\n", this, caps);
    *caps = mAdapter.getCaps();
    return D3D_OK;
}

HRESULT D3DGLDevice::GetDisplayMode(UINT swapchain, D3DDISPLAYMODE *mode)
{
    TRACE("iface %p, swapchain %u, mode %p\n", this, swapchain, mode);

    if(swapchain >= mSwapchains.size())
    {
        FIXME("Out of range swapchain (%u >= %u)\n", swapchain, mSwapchains.size());
        return D3DERR_INVALIDCALL;
    }

    return mSwapchains[swapchain]->GetDisplayMode(mode);
}

HRESULT D3DGLDevice::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *params)
{
    TRACE("iface %p, params %p\n", this, params);

    params->AdapterOrdinal = mAdapter.getOrdinal();
    params->DeviceType = D3DDEVTYPE_HAL;
    params->hFocusWindow = mWindow;
    params->BehaviorFlags = mFlags;

    return D3D_OK;
}

HRESULT D3DGLDevice::SetCursorProperties(UINT xoffset, UINT yoffset, IDirect3DSurface9 *image)
{
    FIXME("iface %p, xoffset %u, yoffset %u, image %p : stub!\n", this, xoffset, yoffset, image);
    return E_NOTIMPL;
}

void D3DGLDevice::SetCursorPosition(int x, int y, DWORD flags)
{
    FIXME("iface %p, x %d, y %d, flags 0x%lx : stub!\n", this, x, y, flags);
}

WINBOOL D3DGLDevice::ShowCursor(WINBOOL show)
{
    FIXME("iface %p, show %u : stub!\n", this, show);
    return FALSE;
}

HRESULT D3DGLDevice::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *params, IDirect3DSwapChain9 **schain)
{
    FIXME("iface %p, params %p, schain %p : stub!\n", this, params, schain);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetSwapChain(UINT swapchain, IDirect3DSwapChain9 **schain)
{
    TRACE("iface %p, swapchain %u, schain %p\n", this, swapchain, schain);

    if(swapchain >= mSwapchains.size())
    {
        FIXME("Out of range swapchain (%u >= %u)\n", swapchain, mSwapchains.size());
        return D3DERR_INVALIDCALL;
    }

    *schain = mSwapchains[swapchain];
    (*schain)->AddRef();
    return D3D_OK;
}

UINT D3DGLDevice::GetNumberOfSwapChains()
{
    TRACE("iface %p\n", this);
    return mSwapchains.size();
}

HRESULT D3DGLDevice::Reset(D3DPRESENT_PARAMETERS *params)
{
    FIXME("iface %p, params %p : stub!\n", this, params);

    FIXME("Resetting device with parameters:\n"
          "\tBackBufferWidth            = %u\n"
          "\tBackBufferHeight           = %u\n"
          "\tBackBufferFormat           = %s\n"
          "\tBackBufferCount            = %u\n"
          "\tMultiSampleType            = 0x%x\n"
          "\tMultiSampleQuality         = %lu\n"
          "\tSwapEffect                 = 0x%x\n"
          "\thDeviceWindow              = %p\n"
          "\tWindowed                   = %d\n"
          "\tEnableAutoDepthStencil     = %d\n"
          "\tAutoDepthStencilFormat     = %s\n"
          "\tFlags                      = 0x%lx\n"
          "\tFullScreen_RefreshRateInHz = %u\n"
          "\tPresentationInterval       = 0x%x\n",
          params->BackBufferWidth, params->BackBufferHeight, d3dfmt_to_str(params->BackBufferFormat),
          params->BackBufferCount, params->MultiSampleType, params->MultiSampleQuality,
          params->SwapEffect, params->hDeviceWindow, params->Windowed,
          params->EnableAutoDepthStencil, d3dfmt_to_str(params->AutoDepthStencilFormat),
          params->Flags, params->FullScreen_RefreshRateInHz, params->PresentationInterval);

    return D3D_OK;
}

HRESULT D3DGLDevice::Present(const RECT *srcRect, const RECT *dstRect, HWND dstWindowOverride, const RGNDATA *dirtyRegion)
{
    TRACE("iface %p, srcRect %p, dstRect %p, dstWindowOverride %p, dirtyRegion %p : semi-stub\n",
          this, srcRect, dstRect, dstWindowOverride, dirtyRegion);

    return mSwapchains[0]->Present(srcRect, dstRect, dstWindowOverride, dirtyRegion, 0);
}

HRESULT D3DGLDevice::GetBackBuffer(UINT swapchain, UINT backbuffer, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **bbuffer)
{
    TRACE("iface %p, swapchain %u, backbuffer %u, type 0x%x, bbuffer %p\n", this, swapchain, backbuffer, type, bbuffer);

    if(swapchain >= mSwapchains.size())
    {
        WARN("Out of range swapchain (%u >= %u)\n", swapchain, mSwapchains.size());
        return D3DERR_INVALIDCALL;
    }

    return mSwapchains[swapchain]->GetBackBuffer(backbuffer, type, bbuffer);
}

HRESULT D3DGLDevice::GetRasterStatus(UINT swapchain, D3DRASTER_STATUS *status)
{
    TRACE("iface %p, swapchain %u, status %p\n", this, swapchain, status);

    if(swapchain >= mSwapchains.size())
    {
        WARN("Out of range swapchain (%u >= %u)\n", swapchain, mSwapchains.size());
        return D3DERR_INVALIDCALL;
    }

    return mSwapchains[swapchain]->GetRasterStatus(status);
}

HRESULT D3DGLDevice::SetDialogBoxMode(WINBOOL enable)
{
    FIXME("iface %p, enable %u : stub!\n", this, enable);
    return E_NOTIMPL;
}

void D3DGLDevice::SetGammaRamp(UINT swapchain, DWORD flags, const D3DGAMMARAMP *ramp)
{
    FIXME("iface %p, swapchain %u, flags 0x%lx, ramp %p : stub!\n", this, swapchain, flags, ramp);
}

void D3DGLDevice::GetGammaRamp(UINT swapchain, D3DGAMMARAMP *ramp)
{
    FIXME("iface %p, swapchain %u, ramp %p : stub!\n", this, swapchain, ramp);
}

HRESULT D3DGLDevice::CreateTexture(UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DTexture9 **texture, HANDLE *handle)
{
    TRACE("iface %p, width %u, height %u, levels %u, usage 0x%lx, format %s, pool 0x%x, texture %p, handle %p\n", this, width, height, levels, usage, d3dfmt_to_str(format), pool, texture, handle);

    if(handle)
    {
        WARN("Non-NULL handle specified\n");
        return D3DERR_INVALIDCALL;
    }
    if(!texture)
    {
        WARN("NULL texture storage specified\n");
        return D3DERR_INVALIDCALL;
    }

    HRESULT hr = D3D_OK;
    DWORD realusage = mAdapter.getUsage(D3DRTYPE_TEXTURE, format);
    if((usage&realusage) != usage)
    {
        usage &= ~D3DUSAGE_AUTOGENMIPMAP;
        if((usage&realusage) != usage)
        {
            ERR("Invalid usage flags, 0x%lx / 0x%lx\n", usage, realusage);
            return D3DERR_INVALIDCALL;
        }
        WARN("AUTOGENMIPMAP requested, but unavailable (usage: 0x%lx)\n", realusage);
        hr = D3DOK_NOAUTOGEN;
    }

    D3DSURFACE_DESC desc;
    desc.Format = format;
    desc.Type = D3DRTYPE_TEXTURE;
    desc.Usage = usage;
    desc.Pool = pool;
    desc.MultiSampleType = D3DMULTISAMPLE_NONE;
    desc.MultiSampleQuality = 0;
    desc.Width = width;
    desc.Height = height;

    D3DGLTexture *tex = new D3DGLTexture(this);
    if(!tex->init(&desc, levels))
    {
        delete tex;
        return D3DERR_INVALIDCALL;
    }

    *texture = tex;
    (*texture)->AddRef();

    return hr;
}

HRESULT D3DGLDevice::CreateVolumeTexture(UINT width, UINT height, UINT depth, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DVolumeTexture9 **texture, HANDLE *handle)
{
    FIXME("iface %p, width %u, height %u, depth %u, levels %u, usage 0x%lx, format %s, pool 0x%x, texture %p, handle %p : stub!\n", this, width, height, depth, levels, usage, d3dfmt_to_str(format), pool, texture, handle);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateCubeTexture(UINT edgeLength, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DCubeTexture9 **texture, HANDLE *handle)
{
    FIXME("iface %p, edgeLength %u, levels %u, usage 0x%lx, format %s, pool 0x%x, texture %p, handle %p : stub!\n", this, edgeLength, levels, usage, d3dfmt_to_str(format), pool, texture, handle);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateVertexBuffer(UINT length, DWORD usage, DWORD fvf, D3DPOOL pool, IDirect3DVertexBuffer9 **vbuffer, HANDLE *handle)
{
    TRACE("iface %p, length %u, usage 0x%lx, fvf 0x%lx, pool 0x%x, vbuffer %p, handle %p\n", this, length, usage, fvf, pool, vbuffer, handle);

    if(handle)
    {
        WARN("Non-NULL handle specified\n");
        return D3DERR_INVALIDCALL;
    }
    if(!vbuffer)
    {
        WARN("NULL vertex buffer storage specified\n");
        return D3DERR_INVALIDCALL;
    }

    D3DGLBufferObject *vbuf = new D3DGLBufferObject(this, D3DGLBufferObject::VBO);
    if(!vbuf->init_vbo(length, usage, fvf, pool))
    {
        delete vbuf;
        return D3DERR_INVALIDCALL;
    }

    *vbuffer = vbuf;
    (*vbuffer)->AddRef();
    return D3D_OK;
}

HRESULT D3DGLDevice::CreateIndexBuffer(UINT length, DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DIndexBuffer9 **ibuffer, HANDLE *handle)
{
    TRACE("iface %p, length %u, usage 0x%lx, format %s, pool 0x%x, vbuffer %p, handle %p\n", this, length, usage, d3dfmt_to_str(format), pool, ibuffer, handle);

    if(handle)
    {
        WARN("Non-NULL handle specified\n");
        return D3DERR_INVALIDCALL;
    }
    if(!ibuffer)
    {
        WARN("NULL index buffer storage specified\n");
        return D3DERR_INVALIDCALL;
    }

    D3DGLBufferObject *ibuf = new D3DGLBufferObject(this, D3DGLBufferObject::IBO);
    if(!ibuf->init_ibo(length, usage, format, pool))
    {
        delete ibuf;
        return D3DERR_INVALIDCALL;
    }

    *ibuffer = ibuf;
    (*ibuffer)->AddRef();
    return D3D_OK;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
HRESULT D3DGLDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, WINBOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, WINBOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetDepthStencilSurface(IDirect3DSurface9 *depthstencil)
{
    FIXME("iface %p, depthstencil %p : stub!\n", this, depthstencil);
    if(depthstencil) depthstencil->AddRef();
    depthstencil = mDepthStencil.exchange(depthstencil);
    if(depthstencil) depthstencil->Release();
    return D3D_OK;
}

HRESULT D3DGLDevice::GetDepthStencilSurface(IDirect3DSurface9 **depthstencil)
{
    TRACE("iface %p, depthstencil %p\n", this, depthstencil);
    *depthstencil = mDepthStencil.load();
    if(*depthstencil)
        (*depthstencil)->AddRef();
    return D3D_OK;
}

HRESULT D3DGLDevice::BeginScene()
{
    TRACE("iface %p : semi-stub\n", this);

    if(mInScene.exchange(true))
    {
        ERR("Already in scene\n");
        return D3DERR_INVALIDCALL;
    }
    // TODO: Prepare any GL state? Depends on what's allowed or not to be
    // called within a 'scene'.

    return D3D_OK;
}

HRESULT D3DGLDevice::EndScene()
{
    TRACE("iface %p : semi-stub\n", this);

    if(!mInScene.exchange(false))
    {
        ERR("Not in scene\n");
        return D3DERR_INVALIDCALL;
    }
    // TODO: Flush GL?

    return D3D_OK;
}

HRESULT D3DGLDevice::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetViewport(D3DVIEWPORT9* pViewport)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetMaterial(const D3DMATERIAL9 *material)
{
    FIXME("iface %p, material %p : stub!\n", this, material);
    mQueue.lock();
    mMaterial = *material;
    mQueue.sendAndUnlock<MaterialSet>(mMaterial);
    return D3D_OK;
}

HRESULT D3DGLDevice::GetMaterial(D3DMATERIAL9 *material)
{
    FIXME("iface %p, material %p : stub!\n", this, material);
    mQueue.lock();
    *material = mMaterial;
    mQueue.unlock();
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetLight(DWORD Index, CONST D3DLIGHT9*)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetLight(DWORD Index, D3DLIGHT9*)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::LightEnable(DWORD Index, WINBOOL Enable)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetLightEnable(DWORD Index, WINBOOL* pEnable)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetClipPlane(DWORD Index, CONST float* pPlane)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetClipPlane(DWORD Index, float* pPlane)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
    FIXME("iface %p, state %s, value 0x%lx : stub!\n", this, d3drs_to_str(state), value);

    if(state == D3DRS_DITHERENABLE)
    {
        mQueue.lock();
        mRenderState[state] = value;
        mQueue.sendAndUnlock<StateEnable>(GL_DITHER, value!=0);
        return D3D_OK;
    }

    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetRenderState(D3DRENDERSTATETYPE state, DWORD *value)
{
    FIXME("iface %p, state %s, value %p : stub!\n", this, d3drs_to_str(state), value);

    if(state >= mRenderState.size())
    {
        WARN("State out of range (%u >= %u)\n", state, mRenderState.size());
        return D3DERR_INVALIDCALL;
    }

    *value = mRenderState[state];
    return D3D_OK;
}

HRESULT D3DGLDevice::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::BeginStateBlock()
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD *value)
{
    FIXME("iface %p, stage %lu, type %s, value %p : stub!\n", this, stage, d3dtss_to_str(type), value);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
    FIXME("iface %p, stage %lu, type %s, value 0x%lx : stub!\n", this, stage, d3dtss_to_str(type), value);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD *value)
{
    FIXME("iface %p, sampler %lu, type %s, value %p : stub!\n", this, sampler, d3dsamp_to_str(type), value);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
{
    FIXME("iface %p, sampler %lu, type %s, value 0x%lx : stub!\n", this, sampler, d3dsamp_to_str(type), value);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::ValidateDevice(DWORD* pNumPasses)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetCurrentTexturePalette(UINT PaletteNumber)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetCurrentTexturePalette(UINT *PaletteNumber)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetScissorRect(CONST RECT* pRect)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetScissorRect(RECT* pRect)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetSoftwareVertexProcessing(WINBOOL bSoftware)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

WINBOOL D3DGLDevice::GetSoftwareVertexProcessing()
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetNPatchMode(float nSegments)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

float D3DGLDevice::GetNPatchMode()
{
    FIXME("iface %p : stub!\n", this);
    return 0.0f;
}

HRESULT D3DGLDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetFVF(DWORD FVF)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetFVF(DWORD* pFVF)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetVertexShader(IDirect3DVertexShader9* pShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetVertexShaderConstantB(UINT StartRegister, CONST WINBOOL* pConstantData, UINT  BoolCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetVertexShaderConstantB(UINT StartRegister, WINBOOL* pConstantData, UINT BoolCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetPixelShader(IDirect3DPixelShader9* pShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::SetPixelShaderConstantB(UINT StartRegister, CONST WINBOOL* pConstantData, UINT  BoolCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::GetPixelShaderConstantB(UINT StartRegister, WINBOOL* pConstantData, UINT BoolCount)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::DeletePatch(UINT Handle)
{
    FIXME("iface %p : stub!\n", this);
    return E_NOTIMPL;
}

HRESULT D3DGLDevice::CreateQuery(D3DQUERYTYPE type, IDirect3DQuery9 **query)
{
    FIXME("iface %p, type %s, query %p : stub!\n", this, d3dquery_to_str(type), query);
    return E_NOTIMPL;
}
