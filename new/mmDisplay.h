#ifndef mmDisplay_H
#define mmDisplay_H

/* Copyright MovingModels Inc, 1999

Module:
----------------------------------------------------------
	mmDisplay

Function:
----------------------------------------------------------
	Handles DirectDraw and Direct3D

Revision History:
----------------------------------------------------------
	07/31/99 - jrb


*/

class mmDisplay : public CObject
{
	public:
    	mmDisplay();
	    ~mmDisplay();
	    DECLARE_SERIAL(mmDisplay);
		BOOL m_WindowMode;

		LPDIRECTDRAW			m_DDDevice;
		LPDIRECTDRAW4			m_DDDevice4;
        LPDIRECTDRAWCLIPPER 	m_DDClipper;

		LPDIRECTDRAWSURFACE4     m_FrontBuffer;
		LPDIRECTDRAWSURFACE4     m_BackBuffer;
		LPDIRECTDRAWSURFACE4     m_ZBuffer;
		LPDIRECTDRAWSURFACE4	 m_ScreenBuffer;

		LPDIRECTDRAWPALETTE		m_Palette;

		DWORD					m_CoOpFlags;
		DDPIXELFORMAT			m_ZBufferFormat;

		BOOL m_InScene;

		LPDIRECT3D3			m_D3DObject;
		LPDIRECT3DDEVICE	m_D3DDevice;
		LPDIRECT3DDEVICE2	m_D3DDevice2;
		LPDIRECT3DDEVICE3	m_D3DDevice3;
		LPDIRECT3DVIEWPORT3	m_Viewport;

		LPDIRECT3DMATERIAL3	m_BackgroundMaterial;

		D3DMATRIX			m_View;
		D3DMATRIX			m_Identity;
		D3DMATRIX			m_World;
		D3DMATRIX			m_Projection;
		D3DMATRIXHANDLE		m_hView;
		D3DMATRIXHANDLE		m_hWorld;
		D3DMATRIXHANDLE		m_hProjection;

	    D3DLIGHT2			m_LightDesc;
		LPDIRECT3DLIGHT		m_D3DLight;
		D3DCOLOR			m_AmbientColor;
		D3DCOLOR			m_MaterialColor;

		char				m_D3DDeviceName[6][512];
		char				m_D3DDeviceDesc[6][512];
		GUID				m_D3DDeviceGUID[16];
		WORD				m_NumD3DDevices;
		WORD				m_Width;
		WORD				m_Height;
		WORD				m_Depth;
		DDPIXELFORMAT		m_TextureFormat;
		DDPIXELFORMAT		m_HighColorFormat;
		DDPIXELFORMAT		m_PalFormat;
		float m_FOV;
		D3DTEXTUREHANDLE	m_TextureHandle;
		D3DMATERIALHANDLE	m_MaterialHandle;
		D3DMATRIX			m_WorldTransform,m_ViewTransform,m_ProjectionTransform;
		D3DMATRIX			m_WorldView,m_WorldViewProj;
		BOOL				m_WorldDirty,m_ViewDirty,m_ProjectionDirty;
		DWORD				m_FrameCount;
		WORD m_ViewWidth,m_ViewHeight;
		HWND m_hWnd;
		BOOL m_IsOpen;
		DDPIXELFORMAT *m_TextureFormats;
		WORD m_NumTextureFormats;
		bool m_SquareOnly;
		DWORD m_TextureOpCaps;
		BOOL m_SceneReady;
		DWORD m_RenderStates[256];
		DWORD m_LightStates[256];
		bool m_RenderStatesDirty[256];
		bool m_LightStatesDirty[256];



		HRESULT Create( DWORD CoOpFlags,bool IsWindow);
		HRESULT Open(WORD Width, WORD Height, WORD Depth,HWND hWnd);
		HRESULT CreateZBuffer(LPDIRECT3D3 lpD3D);
		HRESULT Close();
		char *ShowError(HRESULT error);
		void SetClipper(HWND hWnd);
		HRESULT CreateViewport(WORD Width, WORD Height);
		HRESULT InitRenderState();
		BOOL RenderScene();
		BOOL RenderRect(RECT *src);
		BOOL SetCamera(D3DVECTOR *Position,D3DVECTOR *Orientation);
		HRESULT SetFogMode(BOOL Enable,D3DFOGMODE Mode,float Start,float End,D3DCOLOR FogColor);
		void SetFieldOfView(float FOV);
		HRESULT SetAmbientLight(D3DCOLOR AmbientColor);
		void ClearScreen();
		HRESULT SetLightState(D3DLIGHTSTATETYPE LightStateType,DWORD LightState);
		HRESULT SetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType,LPD3DMATRIX lpD3DMatrix);
		HRESULT SetRenderState(D3DRENDERSTATETYPE RenderStateType,DWORD RenderState);
		HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType,D3DVERTEXTYPE d3dvtVertexType,D3DVERTEX *lpvVertices,DWORD dwVertexCount,LPWORD dwIndices,DWORD dwIndexCount,DWORD dwFlags);
		BOOL IsVisible(D3DVERTEX *box);
		void BeginScene();
		void EndScene();
		void MakeScreenBuffer();
		void UpdateScreenBuffer(CWnd *view,char *source,WORD Depth);
		bool GetCaps();

		void BufferRenderState(D3DRENDERSTATETYPE RenderStateType,DWORD RenderState);
		void BufferLightState(D3DLIGHTSTATETYPE LightStateType,DWORD LightState);
		void ClearLightStates();
		void ClearRenderStates();
		void FlushRenderStates();
		void FlushLightStates();
 };

#endif
