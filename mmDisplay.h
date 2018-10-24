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

class mmDisplay
{
	public:
    	mmDisplay();
	    ~mmDisplay();
	 
 
		WORD				m_Width;
		WORD				m_Height;
		WORD				m_Depth;
 
		DWORD				m_FrameCount;
		WORD m_ViewWidth,m_ViewHeight;
	 
		BOOL m_IsOpen;
 
		BOOL RenderScene(); 
		void ClearScreen();
		void BeginScene();
		void EndScene();
		void Close();
		void UpdateScreenBuffer(char *source,WORD Depth);
		void MakeScreenBuffer();
		HRESULT Open(WORD Width, WORD Height);

 };

#endif
