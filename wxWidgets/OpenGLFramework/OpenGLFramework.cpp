//################################################################
//#   
//#  OpenGLFramework
//#  Autor: PlayeRom
//#  Kontakt: romek21@op.pl
//#  www.playerom.prv.pl
//#
//#################################################################

#include "stdafx.h"
#include "Framework.h"
#include "OpenGLFramework.h"

#ifdef _DEBUG
//	#include <vld.h>
#endif

#if !USE_WXWIDGETS_CONTAINER

int WINAPI WinMain( HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/ )
{
	CFramework cFramework;
	return cFramework.Run();
}

#endif // !USE_WXWIDGETS_CONTAINER