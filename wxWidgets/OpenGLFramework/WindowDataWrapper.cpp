
#include "StdAfx.h"
#include "WindowDataWrapper.h"

CWindowDataWrapper* CWindowDataWrapper::Construct()
{
	return static_cast< CWindowDataWrapper* >( CSingletonBase::Construct( new CWindowDataWrapper ) );
}

CWindowDataWrapper::CWindowDataWrapper() : CWindowData()
{
}

CWindowDataWrapper::~CWindowDataWrapper()
{
}

GLboolean CWindowDataWrapper::Initialization( GLint iWidth, GLint iHeight )
{
	if( iWidth > 0 && iHeight > 0 )
	{
		m_iDesktopWidth = iWidth;
		m_iDesktopHeight = iHeight;
	}

//******************** nie zmieniaj: *******************************************
	GLboolean bResult = GL_TRUE;
	
	m_pMultiLang = CMultiLanguage::Construct();
	if( !m_pMultiLang ) bResult = GL_FALSE; 
	
	m_pLogger = new CLogger( LOG_FILE_NAME );
	if( !m_pLogger ) bResult = GL_FALSE; 
	
	m_pARBMultisample = new CARBMultisample();
	if( !m_pARBMultisample ) bResult = GL_FALSE; 

	GetMultiLang()->InitializeLanguage();

	if( !LoadSettingFile()  ) //blad odczytu pliku setting, wiec zapisujemy z ustawieniami domyœlnymi
		SaveSettingFile( GL_TRUE );

	if( m_sSettingFile.bClearOldLog )
		GetLogger()->ClearOldLog();

	if( IsCommandLine( _T("/logger") ) )
		GetLogger()->SetActive( GL_TRUE ); //otwieramy loga

//******************************************************************************

	return bResult;
}

GLboolean CWindowDataWrapper::CreateGLWindow( WNDPROC* /*pWndProc*/, const GLint /*iIcon*/, const GLint /*iCursor*/ )
{
	return GL_FALSE;
}

GLvoid CWindowDataWrapper::KillGLWindow()
{
}

GLvoid CWindowDataWrapper::RestartWindow()
{
}

GLvoid CWindowDataWrapper::FullscreenSwitch()
{
}

GLvoid CWindowDataWrapper::SetAntialiasingSamples( GLint /*iSamples*/, GLboolean /*bRestart*/ /*= GL_FALSE*/ )
{
}

GLboolean CWindowDataWrapper::CreateInstance()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::RegisterMyClass()
{
	return GL_FALSE;
}

GLvoid CWindowDataWrapper::SetDeviceOnFullscreen()
{
}

GLboolean CWindowDataWrapper::CreateMyWindow()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::GetDeviceContent()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::SetPixelFormat()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::CreateRenderingContext()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::LoadSettingFile()
{
	return GL_FALSE;
}

GLboolean CWindowDataWrapper::SaveSettingFile( GLboolean bDefaultSave /*= GL_FALSE*/ )
{
//	FILE *hFile = NULL;
//	if( fopen_s( &hFile, SETTING_FILE_NAME, "wb" ) != 0 )
//		return GL_FALSE;

	if( bDefaultSave ) {
		m_sSettingFile.iAntialiasing = m_pARBMultisample->GetSamples();

		m_sSettingFile.ubMajorVersion = ( GLubyte )MAJOR_VERSION_FILE;
		m_sSettingFile.ubMinorVersion = ( GLubyte )MINOR_VERSION_FILE;
		m_sSettingFile.ubReleaseVersion = ( GLubyte )RELEASE_VERSION_FILE;

#if _USE_SYSTEM_RES_AND_BITSPERPIXEL_
		m_sSettingFile.iWidth = m_iDesktopWidth;
		m_sSettingFile.iHeight = m_iDesktopHeight;
		m_sSettingFile.ubBitsPerPixel = m_ubBitsPerPixel;
#else
		//tutaj mozemy podac recznie wlasne ustawienia
		m_sSettingFile.iWidth = _SCREEN_WIDTH_;
		m_sSettingFile.iHeight = _SCREEN_HEIGHT_;
		m_sSettingFile.ubBitsPerPixel = _SCREEN_BITS_;
#endif
		m_sSettingFile.bFullScreen = GL_TRUE;
		m_sSettingFile.bVSync = GL_TRUE;
		m_sSettingFile.bClearOldLog = GetLogger()->IsClearOldLog();
		m_sSettingFile.iIndexLanguage = 0;
		m_sSettingFile.bSound = GL_TRUE;
		m_sSettingFile.bMusic = GL_TRUE;
		m_sSettingFile.iVolumeSound = DEFAULT_SOUND_VOLUME;
		m_sSettingFile.iVolumeMusic = DEFAULT_MUSIC_VOLUME;
		m_sSettingFile.iBrightness = 0;
	}

//	fwrite( &m_sSettingFile, sizeof( SSettingFile ), 1, hFile );
//	fclose( hFile );

	return GL_TRUE;
}