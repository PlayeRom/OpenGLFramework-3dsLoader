
#include "stdafx.h"
#include "MyFrame.h"
#include "MyPanel.h"
#include "MyApp.h"

//#include <vld.h> // Visual Leak Detector

#if !wxUSE_GLCANVAS
	#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

IMPLEMENT_APP( CMyApp )

BEGIN_EVENT_TABLE( CMyApp, wxApp )
	EVT_QUERY_END_SESSION( CMyApp::OnQueryEndSession )
	EVT_END_SESSION( CMyApp::OnEndSession )
END_EVENT_TABLE()

bool CMyApp::OnInit()
{
	InitialPaths();
	LoadConfig();

	m_pMyFrame = new CMyFrame( TITLE_APP,
							   m_sUserStoreData.m_wxPosOfWindow,
							   m_sUserStoreData.m_wxSizeOfWindow,
							   wxMINIMIZE_BOX |
							   wxMAXIMIZE_BOX |
							   wxRESIZE_BORDER |
							   wxSYSTEM_MENU |
							   wxCAPTION |
							   wxCLOSE_BOX |
							   wxCLIP_CHILDREN );

	m_pMyFrame->SetMenuCheckBox( m_sUserStoreData.m_bShowFps,		CMyFrame::EMenuViewShowFPS );
	m_pMyFrame->SetMenuCheckBox( m_sUserStoreData.m_bImprovePerf,	CMyFrame::EMenuViewImpPerf );
	m_pMyFrame->SetMenuCheckBox( m_sUserStoreData.m_bSmoothNormals, CMyFrame::EMenuViewSmoothNomrals );
	m_pMyFrame->SetMenuCheckBox( m_sUserStoreData.m_bUseIndexing,	CMyFrame::EMenuViewUseIndexing );
	m_pMyFrame->Initialize();
	m_pMyFrame->Maximize( m_sUserStoreData.m_bMaximized );
	m_pMyFrame->Show( true );

	return true;
}

int CMyApp::OnExit()
{
	return wxApp::OnExit();
}

void CMyApp::OnQueryEndSession( wxCloseEvent& WXUNUSED( eEvent ) )
{
	m_pMyFrame->DoClose( true );
}

void CMyApp::OnEndSession( wxCloseEvent& WXUNUSED( eEvent ) )
{
	m_pMyFrame->DoClose( true );
}

void CMyApp::ComputeWindowsSizes()
{
#ifdef __WXMSW__
	m_wxSizeOfScreen = ::wxGetClientDisplayRect().GetSize();
#else
	m_wxSizeOfScreen = wxSize( SCREEN_WIDTH, SCREEN_HEIGHT );
#endif

	// oblicz wielkosc okna OpenGL
	int iSize = GetResolutionsArraySize();
	for( int i = 0; i < iSize - 1; ++i )
	{
		if( m_wxSizeOfScreen.GetWidth() > g_aResolutions[ i ].GetWidth() &&
			m_wxSizeOfScreen.GetHeight() > g_aResolutions[ i ].GetHeight() )
		{
			m_wxSizeOfOGL = g_aResolutions[ i + 1 ];
			break;
		}
	}

	// ustaw wielkosc okna aplikacji na caly dostepny obszar ekranu
	m_wxSizeOfWindow = m_wxSizeOfScreen;

	///////////////////////////////////////////////////////////////////////////
	// przypisanie tych samych zmiennych struktorze
	m_sUserStoreData.m_wxPosOfWindow	= wxPoint( WINDOW_POS_X, WINDOW_POS_Y );
	m_sUserStoreData.m_wxSizeOfScreen	= m_wxSizeOfScreen;
	m_sUserStoreData.m_wxSizeOfOGL		= m_wxSizeOfOGL;
	m_sUserStoreData.m_wxSizeOfWindow	= m_wxSizeOfWindow;
}

int CMyApp::GetResolutionsArraySize() const
{
	int i = 0;
	for( ; g_aResolutions[ i ].GetWidth() > 0 && g_aResolutions[ i ].GetHeight() > 0; ++i )
	{
	}

	return i; // iloœæ poprawnych rodzielczoœci (bez 0x0)
}

wxString CMyApp::GetCmdLine( int iIndex ) const
{
	if( iIndex < 0 && iIndex >= this->argc )
		return wxT("");

	return this->argv[ iIndex ];
}

void CMyApp::InitialPaths()
{
	// pobierz katalog do pliku exe z parametru
	m_strPathToExe = wxT("");
	m_strPathWithExe = GetCmdLine( 0 );
	if( !m_strPathWithExe.IsEmpty() )
	{
		wxString strVolume, strName, strExt;
		wxFileName::SplitPath( m_strPathWithExe, &strVolume, &m_strPathToExe, &strName, &strExt );
		m_strPathToExe = strVolume + wxT(":") + m_strPathToExe + wxT("\\");

		::wxSetWorkingDirectory( m_strPathToExe );
	}
}

bool CMyApp::LoadConfig()
{
	m_sUserStoreData.m_bMaximized = false;
	m_sUserStoreData.m_bShowFps = true;
	m_sUserStoreData.m_iFps = 10;
	m_sUserStoreData.m_bImprovePerf = true;
	m_sUserStoreData.m_bSmoothNormals = true;
	m_sUserStoreData.m_bUseIndexing = true;
	::SecureZeroMemory( m_sUserStoreData.m_str3dsFile, sizeof( m_sUserStoreData.m_str3dsFile ) );
	::SecureZeroMemory( m_sUserStoreData.m_strTexFile, sizeof( m_sUserStoreData.m_strTexFile ) );
	::SecureZeroMemory( m_sUserStoreData.m_strBumpFile, sizeof( m_sUserStoreData.m_strBumpFile ) );

	ComputeWindowsSizes();

	// Pozbycie sie komunikatu, ze plik nie istnieje przy pierwszym uruchomieniu
	wxFileName fileName( FILE_SETTINGS );
	if( !fileName.FileExists() )
		return false;

	wxFileInputStream input( FILE_SETTINGS );
	if( !input.IsOk() )
		return false;

	input.Read( &m_sUserStoreData, sizeof( SUserStoreData ) );

	return true;
}

bool CMyApp::SaveConfig()
{
	if( !m_pMyFrame || !m_pMyFrame->GetMyPanel() )
		return false;

	m_sUserStoreData.m_bMaximized		= m_pMyFrame->IsMaximized();
	m_sUserStoreData.m_wxPosOfWindow	= m_pMyFrame->GetPosition();
	m_sUserStoreData.m_wxSizeOfOGL		= m_pMyFrame->GetMyPanel()->GetSizeOfGLCanvas();
	m_sUserStoreData.m_wxSizeOfWindow	= m_pMyFrame->GetSize();

	wxFileOutputStream output( FILE_SETTINGS );
	if( !output.IsOk() )
		return false;

	output.Write( &m_sUserStoreData, sizeof( SUserStoreData ) );

	return true;
}