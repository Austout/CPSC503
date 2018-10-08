#include "main.h"

	// Include the OculusVR SDK
#include "GL/CAPI_GLE.h"    
#include "OVR_CAPI_GL.h"    
#include "GL/CAPI_GLE_GL.h"    
#include "Extras/OVR_Math.h"
#include "Extras/OVR_CAPI_Util.h"
// the below function is mostly from oculus sdk guide
//https://developer.oculus.com/documentation/pcsdk/latest/concepts/dg-render/#dg_render
void Application()
{
    ovrResult result = ovr_Initialize(nullptr);
    if (OVR_FAILURE(result))
        return;

    ovrSession session;
    ovrGraphicsLuid luid;
    result = ovr_Create(&session, &luid);
    if (OVR_FAILURE(result))
    {
        ovr_Shutdown();
        return;
    }

    ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);
    ovrSizei resolution = hmdDesc.Resolution;
   
    OVR::Sizei recommenedTex0Size = ovr_GetFovTextureSize(session, ovrEye_Left, 
                                                            hmdDesc.DefaultEyeFov[0], 1.0f);
    OVR::Sizei recommenedTex1Size = ovr_GetFovTextureSize(session, ovrEye_Right,
                                                            hmdDesc.DefaultEyeFov[1], 1.0f);
    OVR::Sizei bufferSize;

    bufferSize.w  = recommenedTex0Size.w + recommenedTex1Size.w;
    bufferSize.h = recommenedTex0Size.h < recommenedTex1Size.h?  recommenedTex1Size.h :  recommenedTex0Size.h;
    ovrTextureSwapChain textureSwapChain = 0;

    ovrTextureSwapChainDesc desc = {};
    desc.Type = ovrTexture_2D;
    desc.ArraySize = 1;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = bufferSize.w;
    desc.Height = bufferSize.h;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;

    if (ovr_CreateTextureSwapChainGL(session, &desc, &textureSwapChain) == ovrSuccess)
    {
        // Sample texture access:
        unsigned int texId;
        ovr_GetTextureSwapChainBufferGL(session, textureSwapChain, 0, &texId);
        // glBindTexture(GL_TEXTURE_2D, texId);

        
        ovrEyeRenderDesc eyeRenderDesc[2];
        ovrPosef      hmdToEyeViewPose[2];
        eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
        eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
        hmdToEyeViewPose[0] = eyeRenderDesc[0].HmdToEyePose;
        hmdToEyeViewPose[1] = eyeRenderDesc[1].HmdToEyePose;

        // Initialize our single full screen Fov layer.
        ovrLayerEyeFov layer;
        layer.Header.Type      = ovrLayerType_EyeFov;
        layer.Header.Flags     = 0;
        layer.ColorTexture[0]  = textureSwapChain;
        layer.ColorTexture[1]  = textureSwapChain;
        layer.Fov[0]           = eyeRenderDesc[0].Fov;
        layer.Fov[1]           = eyeRenderDesc[1].Fov;
        layer.Viewport[0]      = OVR::Recti(0, 0,                bufferSize.w / 2, bufferSize.h);
        layer.Viewport[1]      = OVR::Recti(bufferSize.w / 2, 0, bufferSize.w / 2, bufferSize.h);
        // ld.RenderPose and ld.SensorSampleTime are updated later per frame.


        // render loop code: 

        // Get both eye poses simultaneously, with IPD offset already included.
        double displayMidpointSeconds = GetPredictedDisplayTime(session, 0);
        ovrTrackingState hmdState = ovr_GetTrackingState(session, displayMidpointSeconds, ovrTrue);
        ovr_CalcEyePoses(hmdState.HeadPose.ThePose, hmdToEyeViewPose, layer.RenderPose);
        
    }
    ovr_Destroy(session);
    ovr_Shutdown();
}
int main(){
	Application();
	return 0;
}
