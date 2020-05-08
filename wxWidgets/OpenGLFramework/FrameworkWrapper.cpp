
#include "StdAfx.h"
#include "../Fps/Fps.h"
#include "../Draw/3DObjManager.h"
#include "WindowDataWrapper.h"
#include "GameControl.h"
#include "FrameworkWrapper.h"

CWindowDataWrapper* CFrameworkWrapper::g_pWinData = NULL;

CFrameworkWrapper::CFrameworkWrapper() : CFramework()
{
}

CFrameworkWrapper::~CFrameworkWrapper()
{
	Destroy();
}

GLint CFrameworkWrapper::Run( GLint iWidth, GLint iHeight )
{
	CBrightness::Construct();

	g_pWinData = CWindowDataWrapper::Construct();
	CFramework::g_pWinData = g_pWinData;

	if( !g_pWinData->Initialization( iWidth, iHeight ) )
	{
		::MessageBox( NULL, _T("Window initialization error"), GAME_NAME, MB_OK | MB_ICONEXCLAMATION );
		return 0;
	}

	g_pWinData->SettingFile().bFullScreen = GL_FALSE;

	srand( ( unsigned )time( NULL ) );

	g_pWinData->ReSizeGLScene();	// Ustawia perspektywê

	if( !InitalizationGL() ) // Inicjalizuje nasze nowo utworzone okno
	{
		::MessageBox( NULL, _T("OpenGL initialization error"), GAME_NAME, MB_OK | MB_ICONEXCLAMATION );
		return 0;
	}

	return 1;
}

GLvoid CFrameworkWrapper::Destroy()
{
	if( g_pGameCtrl )
	{
		delete g_pGameCtrl;
		g_pGameCtrl = NULL;
	}
}

GLvoid CFrameworkWrapper::ReSizeGLScene()
{
	if( g_pWinData )
		g_pWinData->ReSizeGLScene();
}

GLvoid CFrameworkWrapper::Timer()
{
	if( g_pGameCtrl )
		g_pGameCtrl->Timer();
}

GLvoid CFrameworkWrapper::KeyDown( GLuint uiKeyCode )
{
	if( g_pGameCtrl )
		g_pGameCtrl->KeyDown( uiKeyCode );
}

GLvoid CFrameworkWrapper::KeyUp( GLuint uiKeyCode )
{
	if( g_pGameCtrl )
		g_pGameCtrl->KeyUp( uiKeyCode );
}

GLboolean CFrameworkWrapper::MouseLButtonDown( GLint iX, GLint iY )
{
	if( !g_pGameCtrl )
		return GL_FALSE;

	return g_pGameCtrl->MouseLButtonDown( iX, iY );
}

GLboolean CFrameworkWrapper::MouseLButtonUp( GLint iX, GLint iY )
{
	if( !g_pGameCtrl )
		return GL_FALSE;

	return g_pGameCtrl->MouseLButtonUp( iX, iY );
}

GLboolean CFrameworkWrapper::MouseRButtonDown( GLint iX, GLint iY )
{
	if( !g_pGameCtrl )
		return GL_FALSE;

	return g_pGameCtrl->MouseRButtonDown( iX, iY );
}

GLboolean CFrameworkWrapper::MouseRButtonUp( GLint iX, GLint iY )
{
	if( !g_pGameCtrl )
		return GL_FALSE;

	return g_pGameCtrl->MouseRButtonUp( iX, iY );
}

GLboolean CFrameworkWrapper::MouseWheel( GLint iWheelRotatin )
{
	GLboolean bResult = GL_FALSE;

	if( !g_pGameCtrl )
		return bResult;

	g_iCompleteWheelRotate += iWheelRotatin;
	if( abs( g_iCompleteWheelRotate ) >= WHEEL_DELTA )
	{
		bResult = g_pGameCtrl->MouseWheel( g_iCompleteWheelRotate / WHEEL_DELTA );
		g_iCompleteWheelRotate %= WHEEL_DELTA;
	}

	return bResult;
}

GLvoid CFrameworkWrapper::SetWindowHandle( GLvoid* pHandle )
{
	if( g_pWinData )
		g_pWinData->SetHWND( ( HWND )pHandle );
}

GLboolean CFrameworkWrapper::RestartWindow( GLint iWidth, GLint iHeight )
{
	g_pWinData->SetRestart( GL_TRUE );

	// ustaw nowa rozdzielczosc okna opengl
	g_pWinData->SettingFile().iWidth = iWidth;
	g_pWinData->SettingFile().iHeight = iHeight;

	g_pWinData->ReSizeGLScene();	// Ustawia perspektywê

	if( !InitalizationGL() ) // Inicjalizuje nasze nowo utworzone okno
	{
		::MessageBox( NULL, _T("OpenGL initialization error"), GAME_NAME, MB_OK | MB_ICONEXCLAMATION );
		Destroy();

		return GL_FALSE;
	}

	g_pWinData->SetRestart( GL_FALSE );
	g_pWinData->SaveSettingFile();

	return GL_TRUE;
}

GLboolean CFrameworkWrapper::DrawGLScene()
{
	GLboolean result = CFramework::DrawGLScene();

	g_pGameCtrl->Keyboard();

	return result;
}

GLvoid CFrameworkWrapper::DoShowFps( GLboolean bShow )
{
	g_pGameCtrl->GetFps()->ShowFps( bShow );
}

GLvoid CFrameworkWrapper::DoSmoothNormals( GLboolean bShooth )
{
	g_pGameCtrl->SetUseSmoothNormals( bShooth );
}

GLvoid CFrameworkWrapper::DoUseIndexing( GLboolean bUse )
{
	g_pGameCtrl->SetUseIndexing( bUse );
}

GLboolean CFrameworkWrapper::ReLoad3dsFileAndTextures( LPCTSTR lp3dsFile, LPCTSTR lpTexFile, LPCTSTR lpBumpFile )
{
	return g_pGameCtrl->ReLoad3dsFileAndTextures( lp3dsFile, lpTexFile, lpBumpFile );
}

GLboolean CFrameworkWrapper::Export3dsTo3DObj( LPCTSTR lpPath, GLboolean bIndexing, GLboolean bFixed )
{
	return g_pGameCtrl->Get3DObjManager()->SaveObject( lpPath, bIndexing, bFixed,
													   g_pGameCtrl->GetScaleVector(),
													   g_pGameCtrl->GetAddingVector(),
													   g_pGameCtrl->GetRot90XYZ(),
													   g_pGameCtrl->IsSmoothNormals() );
}