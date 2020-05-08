#pragma once

#include "WindowData.h"

class CWindowDataWrapper : public CWindowData
{
public:
	static CWindowDataWrapper* Construct();
	virtual ~CWindowDataWrapper();

	GLboolean Initialization( GLint iWidth, GLint iHeight );

	GLboolean CreateGLWindow( WNDPROC* pWndProc, const GLint iIcon, const GLint iCursor );
	GLvoid KillGLWindow();

	GLvoid RestartWindow();
	GLvoid FullscreenSwitch();
	GLvoid SetAntialiasingSamples( GLint iSamples, GLboolean bRestart = GL_FALSE );

	GLvoid SetHWND( HWND hWnd )	{ m_hWnd = hWnd; }

	GLboolean LoadSettingFile();
	GLboolean SaveSettingFile( GLboolean bDefaultSave = GL_FALSE );

private:
	CWindowDataWrapper();

	GLboolean CreateInstance();
	GLboolean RegisterMyClass();
	GLvoid SetDeviceOnFullscreen();
	GLboolean CreateMyWindow();
	GLboolean GetDeviceContent();
	GLboolean SetPixelFormat();
	GLboolean CreateRenderingContext();
};
