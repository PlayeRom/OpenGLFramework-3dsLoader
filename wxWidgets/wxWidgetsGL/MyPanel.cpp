
#include "StdAfx.h"
#include "MyGLCanvas.h"
#include "ReSizeDlg.h"
#include "Open3dsDlg.h"
#include "Save3DObjDlg.h"
#include "..\OpenGLFramework\LoadTxtParser.h"
#include "MyPanel.h"

BEGIN_EVENT_TABLE( CMyPanel, wxPanel )
END_EVENT_TABLE()

CMyPanel::CMyPanel( wxFrame* pFrame, const wxPoint& pos, const wxSize& size, long style )
					: wxScrolledWindow( pFrame, wxID_ANY, pos, size, style )
{
#if wxUSE_GLCANVAS
	m_pMyGLCanvas = NULL;
#endif
}

CMyPanel::~CMyPanel()
{
}

void CMyPanel::Initialize()
{
#if wxUSE_GLCANVAS

	int aAttribList[] = { WX_GL_RGBA,
						  WX_GL_DOUBLEBUFFER,
						  //WX_GL_SAMPLE_BUFFERS, GL_TRUE, // Multi-sampling
						  WX_GL_DEPTH_SIZE, 32,
						  WX_GL_STENCIL_SIZE, 1,
						  0, 0 };
	m_pMyGLCanvas = new CMyGLCanvas( this, wxID_ANY, wxPoint( 5, 5 ), GetMyApp()->GetSizeOfUserOGL(), wxSUNKEN_BORDER, aAttribList );
	m_pMyGLCanvas->Initialize();

#endif // wxUSE_GLCANVAS

	SetScrollbars();
}

void CMyPanel::SetScrollbars()
{
	int iPixelsPerUnit = 10; // ilosc pikseli o ktora przesunie sie suwak (pojednyczny skok)
	int iStatusBarHeight = 0;//GetMyFrame()->GetSizeStatusBar().GetHeight();

	// Ustalenie szerokosci i wysokosci wirtualnego okna. Ustawione jest tak, ¿e okno z oryginalnym rozmiarem, tj.
	// m_wxSizeOfScreen lub wiekszym, nie potrzebuje suwakow, dopiero po zmniejszeniu okna
	// uwaki sie ukaza. 
	int iNoUnitX = ( GetMyApp()->GetSizeOfWindow().GetWidth() - ( ( FRAME_SIZE_X * 2 ) +
																  ( BORDER_X * 2 ) +
																  ( EDGE_X * 2 ) +
																  VSCROLL_X ) ) / iPixelsPerUnit;
	int iNoUnitY = ( GetMyApp()->GetSizeOfWindow().GetHeight() - ( ( FRAME_SIZE_Y * 2 ) +
																   ( BORDER_Y * 2 ) +
																   ( EDGE_Y * 2 ) +
																   wxSYS_HSCROLL_Y +
																   CAPTION_Y +
																   MENU_Y +
																   iStatusBarHeight ) ) / iPixelsPerUnit;

	wxScrolledWindow::SetScrollbars( iPixelsPerUnit, iPixelsPerUnit, iNoUnitX, iNoUnitY );
}

void CMyPanel::ReSizeOGLDlg()
{
#if wxUSE_GLCANVAS

	CReSizeDlg dlg( m_pMyGLCanvas->GetSize(), this, wxID_ANY, wxT("Set resolution of OpenGL screen") );
	dlg.ShowModal();

#endif // wxUSE_GLCANVAS
}

void CMyPanel::ReSizeOGL( const wxSize& sizeNew )
{
#if wxUSE_GLCANVAS

	if( m_pMyGLCanvas )
		m_pMyGLCanvas->SetSize( sizeNew );

#endif // wxUSE_GLCANVAS
}

wxSize CMyPanel::GetSizeOfGLCanvas() const
{
#if wxUSE_GLCANVAS
	return m_pMyGLCanvas ? m_pMyGLCanvas->GetSize() : wxDefaultSize;
#else
	return wxDefaultSize;
#endif
}

void CMyPanel::SetFpsDlg()
{
	wxString strTitle = wxT("Set limit of FPS");
	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		strTitle = wxT("Set minimum of FPS");

	long lResult = ::wxGetNumberFromUser( strTitle,
										  wxT("New value of FPS (1-1000):"),
										  strTitle,
										  GetMyApp()->GetUserStoreData().m_iFps,
										  1, 1000, this );

	if( lResult > -1 )
	{
		GetMyApp()->SetUserStoreData().m_iFps = static_cast< int >( lResult );
		m_pMyGLCanvas->SetTimer();
	}
}

void CMyPanel::DoShowFps( bool bShow )
{
	if( m_pMyGLCanvas )
		m_pMyGLCanvas->DoShowFps( bShow );
}

void CMyPanel::DoSmoothNormals( bool bSmooth )
{
	if( m_pMyGLCanvas )
		m_pMyGLCanvas->DoSmoothNormals( bSmooth );
}

void CMyPanel::DoUseIndexing( bool bUse )
{
	if( m_pMyGLCanvas )
		m_pMyGLCanvas->DoUseIndexing( bUse );
}

bool CMyPanel::ReLoad3dsFileAndTextures()
{
	if( m_pMyGLCanvas )
		return m_pMyGLCanvas->ReLoad3dsFileAndTextures();

	return false;
}

void CMyPanel::FillPathAux( const wxString& strPathSour, wxChar strPathDest[ MAX_PATH_LEN ] )
{
	if( strPathSour.IsEmpty() )
		::SecureZeroMemory( strPathDest, sizeof( strPathDest ) );
	else
		wcscpy_s( strPathDest, MAX_PATH_LEN, strPathSour.c_str() );
}

void CMyPanel::Open3dsFile()
{
	COpen3dsDlg dlg( this, wxID_ANY, wxT("Open 3ds file & textures"), wxDefaultPosition, wxSize( 700, 250 ) );
	if( dlg.ShowModal() )
	{
		FillPathAux( dlg.Get3dsFile(), GetMyApp()->SetUserStoreData().m_str3dsFile );
		FillPathAux( dlg.GetTexFile(), GetMyApp()->SetUserStoreData().m_strTexFile );
		FillPathAux( dlg.GetBumpFile(), GetMyApp()->SetUserStoreData().m_strBumpFile );
		
		if( !ReLoad3dsFileAndTextures() && dlg.Get3dsFile().Length() > 0 )
		{
			::wxLogError( wxT("Load 3ds file \"%s\" failed. Incorrect path or file name."), dlg.Get3dsFile() );
		}
	}
}

void CMyPanel::Load3dsFromTxtFile()
{
	wxString strWildCard( wxT("TXT (*.txt)|*.txt") );
	wxString strMsg( wxT("Open txt file") );
	wxString strDefPath( wxT(".") );

	wxString strFileName( ::wxFileSelector( strMsg, strDefPath, wxT(""), wxT(""),
											strWildCard, wxFD_OPEN | wxFD_FILE_MUST_EXIST, this ) );

	if ( !strFileName.IsEmpty() )
	{
		wxFileName fileName( strFileName );
		fileName.MakeRelativeTo( GetMyApp()->GetPathToExe() );

		CLoadTxtParser txtParser;
		txtParser.ParseFile( fileName.GetFullPath() );
		
		FillPathAux( txtParser.Get3dsFileName(), GetMyApp()->SetUserStoreData().m_str3dsFile );
		FillPathAux( txtParser.GetTextureName(), GetMyApp()->SetUserStoreData().m_strTexFile );
		FillPathAux( txtParser.GetBumpName(), GetMyApp()->SetUserStoreData().m_strBumpFile );

		if( !ReLoad3dsFileAndTextures() )
		{
			if( wxString( txtParser.Get3dsFileName() ).Length() > 0 )
				::wxLogError( wxT("Load 3ds file \"%s\" failed. Incorrect path or file name."), txtParser.Get3dsFileName() );
			else
				::wxLogError( wxT("Load txt file \"%s\" failed. Probably this file including incorrect data."), fileName.GetFullPath() );
		}
	}
}

void CMyPanel::Export3dsTo3DObj()
{
	CSave3DObjDlg dlg( this, wxID_ANY, wxT("Export to 3DObj file"), wxDefaultPosition, wxSize( 700, 250 ) );
	if( dlg.ShowModal() )
	{
		wxFileName fileName( dlg.Get3DObjFile() );

		// sprawdz czy plik juz istnieje, jezeli tak to zapytaj o nadpisanie
		bool bExport = true;
		if( fileName.FileExists() )
		{
			if( ::wxMessageBox( wxT("The file ") + fileName.GetFullPath() + wxT(" already exist. Do you want to overwrite this file?"),
								wxT("Export to 3DObj"), wxYES_NO, this ) == wxNO )
			{
				bExport = false;
			}
		}

		if( bExport )
		{
			if( m_pMyGLCanvas && !m_pMyGLCanvas->Export3dsTo3DObj( fileName.GetPath(), dlg.IsIndexing(), dlg.IsFixed() ) )
				::wxLogError( wxT("Export 3ds file to \"%s\" failed."), fileName.GetFullPath() );
			else
				::wxLogMessage( wxT("Export 3ds file to \"%s\" success."), fileName.GetFullPath() );
		}
	}
}