#pragma once

#include "Framework.h"

class CWindowDataWrapper;

class CFrameworkWrapper : public CFramework
{
public:
	CFrameworkWrapper();
	virtual ~CFrameworkWrapper();

	GLint Run( GLint iWidth = 0, GLint iHeight = 0 );
	GLvoid Destroy();

	GLvoid ReSizeGLScene();
	GLvoid Timer();

	GLvoid KeyDown( GLuint uiKeyCode );
	GLvoid KeyUp( GLuint uiKeyCode );

	GLboolean MouseLButtonDown( GLint iX, GLint iY );
	GLboolean MouseLButtonUp( GLint iX, GLint iY );
	GLboolean MouseRButtonDown( GLint iX, GLint iY );
	GLboolean MouseRButtonUp( GLint iX, GLint iY );
	GLboolean MouseWheel( GLint iScrollLines );

	GLvoid SetWindowHandle( GLvoid* pHandle );

	GLboolean RestartWindow( GLint iWidth, GLint iHeight );

	GLboolean DrawGLScene();

	GLvoid DoShowFps( GLboolean bShow );
	GLvoid DoSmoothNormals( GLboolean bShooth );
	GLvoid DoUseIndexing( GLboolean bUse );
	GLboolean ReLoad3dsFileAndTextures( LPCTSTR lp3dsFile, LPCTSTR lpTexFile, LPCTSTR lpBumpFile );
	GLboolean Export3dsTo3DObj( LPCTSTR lpPath, GLboolean bIndexing, GLboolean bFixed );

	static CWindowDataWrapper*		g_pWinData;					///< WskaŸnik na obiekt klasy singletonu CWindowData.
};
