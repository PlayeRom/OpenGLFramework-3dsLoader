
#include "stdafx.h"
#include "MyPanel.h"
#include "MyFrame.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
	#include "icon.xpm"
#endif

BEGIN_EVENT_TABLE( CMyFrame, wxFrame )
	EVT_MENU( EMenuFileOpen3ds,			CMyFrame::OnMenuFileOpen3ds )
	EVT_MENU( EMenuFileLoadTxt,			CMyFrame::OnMenuFileLoadTxt )
	EVT_MENU( EMenuFileExportTo3DObj,	CMyFrame::OnMenuFileExportTo3DObj )
	EVT_MENU( EMenuFileExit,			CMyFrame::OnMenuFileExit )
	EVT_MENU( EMenuViewResetSizes,		CMyFrame::OnMenuViewResetSizes )
	EVT_MENU( EMenuViewResizeOGL,		CMyFrame::OnMenuViewResizeOGL )
	EVT_MENU( EMenuViewShowFPS,			CMyFrame::OnMenuViewShowFPS )
	EVT_MENU( EMenuViewSetFPS,			CMyFrame::OnMenuViewSetFPS )
	EVT_MENU( EMenuViewImpPerf,			CMyFrame::OnMenuViewImprovePerf )
	EVT_MENU( EMenuViewSmoothNomrals,	CMyFrame::OnMenuViewSmoothNormals )
	EVT_MENU( EMenuViewUseIndexing,		CMyFrame::OnMenuViewUseIndexing )
	EVT_MENU( EMenuHelpAbout,			CMyFrame::OnMenuHelpAbout )
	EVT_CLOSE( CMyFrame::OnClose )
END_EVENT_TABLE()

CMyFrame::CMyFrame( const wxString& title,
					const wxPoint& pos,
					const wxSize& size,
					long style )
					: wxFrame( NULL, wxID_ANY, title, pos, size, style )
{
	m_pStatusBar = NULL;
	m_pMyPanel = NULL;

	SetIcon( wxICON( icon ) );

	CreateMenu();
	CreateStatusBar();
}

void CMyFrame::Initialize()
{
	CreateMyPanel();
}

void CMyFrame::CreateMenu()
{
	m_pMenuFile = new wxMenu();
	m_pMenuView = new wxMenu();
	m_pMenuHelp = new wxMenu();

	m_pMenuFile->Append( EMenuFileOpen3ds,			wxT("&Open 3ds && textures...\tAlt-O"),	wxT("Open 3ds file and textures") );
	m_pMenuFile->Append( EMenuFileLoadTxt,			wxT("&Load 3ds && textures from txt file..."),	wxT("Open 3ds file and textures from txt file") );
	m_pMenuFile->AppendSeparator();
	m_pMenuFile->Append( EMenuFileExportTo3DObj,	wxT("&Export to 3DObj file..."),	wxT("Export to 3DObj from 3ds") );
	m_pMenuFile->AppendSeparator();
	m_pMenuFile->Append( EMenuFileExit,				wxT("E&xit\tAlt-X"),				wxT("Quit this program") );
	
	m_pMenuView->Append( EMenuViewResetSizes,		wxT("Reset sizes..."),				wxT("Reset sizes of windows on default") );
	m_pMenuView->Append( EMenuViewResizeOGL,		wxT("&Resize...\tAlt-R"),			wxT("Change size of rendered screen") );
	m_pMenuView->AppendSeparator();
	m_pMenuView->AppendCheckItem( EMenuViewImpPerf,	wxT("&Improve performance\tAlt-A"),	wxT("Actions increase performance of rendered screen") );
	m_pMenuView->AppendCheckItem( EMenuViewShowFPS,	wxT("&Show FPS\tAlt-F"),			wxT("Show FPS on rendered screen") );
	m_pMenuView->Append( EMenuViewSetFPS,			wxT("&Set FPS...\tAlt-S"),			wxT("Change FPS of rendered screen") );
	m_pMenuView->AppendSeparator();
	m_pMenuView->AppendCheckItem( EMenuViewSmoothNomrals,	wxT("&Smooth normals"),		wxT("Normals vectors calculated for smooth edges") );
	m_pMenuView->AppendCheckItem( EMenuViewUseIndexing,		wxT("&Use indexing"),		wxT("The object will be draw with used indexing of vertices or not.") );

	m_pMenuHelp->Append( EMenuHelpAbout,			wxT("&About...\tF1"),				wxT("Show about dialog") );

	m_pMenuBar = new wxMenuBar();
	m_pMenuBar->Append( m_pMenuFile, wxT("&File") );
	m_pMenuBar->Append( m_pMenuView, wxT("&View") );
	m_pMenuBar->Append( m_pMenuHelp, wxT("&Help") );

	SetMenuBar( m_pMenuBar );
}

void CMyFrame::CreateStatusBar()
{
	m_pStatusBar = wxFrame::CreateStatusBar( 2 );
	m_pStatusBar->SetStatusText( wxT("Welcome to PlayeRom 3DSLoader!"), 0 );
}

void CMyFrame::CreateMyPanel()
{
	m_pMyPanel = new CMyPanel( this );
	m_pMyPanel->Initialize();
}

wxSize CMyFrame::GetSizeStatusBar() const
{
	if( !m_pStatusBar )
		return wxSize( 0, 0 );

	return m_pStatusBar->GetSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Zdarzenia menu //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CMyFrame::OnMenuFileOpen3ds( wxCommandEvent& eEvent )
{
	if( m_pMyPanel )
		m_pMyPanel->Open3dsFile();
}

void CMyFrame::OnMenuFileLoadTxt( wxCommandEvent& eEvent )
{
	if( m_pMyPanel )
		m_pMyPanel->Load3dsFromTxtFile();
}

void CMyFrame::OnMenuFileExportTo3DObj( wxCommandEvent& WXUNUSED( eEvent ) )
{
	if( m_pMyPanel )
		m_pMyPanel->Export3dsTo3DObj();
}

void CMyFrame::OnMenuFileExit( wxCommandEvent& WXUNUSED( eEvent ) )
{
	DoClose();
}

void CMyFrame::OnMenuViewResetSizes( wxCommandEvent& WXUNUSED( eEvent ) )
{
	if( ::wxMessageBox( wxT("Do you want to reset windows sizes on default?"), TITLE_APP, wxYES_NO, this ) == wxYES )
	{
		SetPosition( GetMyApp()->GetPosOfWindow() );
		SetSize( GetMyApp()->GetSizeOfWindow() );
		m_pMyPanel->SetSize( GetMyApp()->GetSizeOfWindow() );
	}
}

void CMyFrame::OnMenuViewResizeOGL( wxCommandEvent& WXUNUSED( eEvent ) )
{
	m_pMyPanel->ReSizeOGLDlg();
}

void CMyFrame::OnMenuViewShowFPS( wxCommandEvent& eEvent )
{
	GetMyApp()->SetUserStoreData().m_bShowFps = eEvent.IsChecked();
	m_pMyPanel->DoShowFps( eEvent.IsChecked() ); // poinformuj OpenGL o zdarzeniu
}

void CMyFrame::OnMenuViewSetFPS( wxCommandEvent& eEvent )
{
	m_pMyPanel->SetFpsDlg();
}

void CMyFrame::OnMenuViewImprovePerf( wxCommandEvent& eEvent )
{
	GetMyApp()->SetUserStoreData().m_bImprovePerf = eEvent.IsChecked();
}

void CMyFrame::OnMenuViewSmoothNormals( wxCommandEvent& eEvent )
{
	GetMyApp()->SetUserStoreData().m_bSmoothNormals = eEvent.IsChecked();
	m_pMyPanel->DoSmoothNormals( eEvent.IsChecked() ); // poinformuj OpenGL o zdarzeniu
}

void CMyFrame::OnMenuViewUseIndexing( wxCommandEvent& eEvent )
{
	GetMyApp()->SetUserStoreData().m_bUseIndexing = eEvent.IsChecked();
	m_pMyPanel->DoUseIndexing( eEvent.IsChecked() ); // poinformuj OpenGL o zdarzeniu
}

void CMyFrame::OnMenuHelpAbout( wxCommandEvent& WXUNUSED( eEvent ) )
{
	wxAboutDialogInfo info;
	info.SetIcon( wxICON( icon ) );
    info.SetName( TITLE_APP );
    info.SetVersion( VERSION );
    info.SetCopyright( wxT("                        (C) 2008 PlayeRom                        ") );
	info.SetDescription( wxT("Built by ") + wxString( wxVERSION_STRING ) + wxT("\n") );
	info.SetWebSite( wxT("www.playerom.republika.pl\n") );

	::wxAboutBox( info );
}

void CMyFrame::DoClose( bool bForce )
{
	Close( bForce );
}

void CMyFrame::OnClose( wxCloseEvent& eEvent )
{
	bool bCanClose = true;

	switch( ::wxMessageBox( wxT("Do you really want to exit?"), TITLE_APP, wxICON_QUESTION | wxYES_NO, this ) )
	{
		case wxNO:
			bCanClose = false;
			break;

		case wxYES:
		default:
			break;
	}

	if( bCanClose )
	{
		GetMyApp()->SaveConfig();

		// Naprawa crasha, ktory wystepowal tylko pod Vista w wersji Release.
		// Normalnie powinno byæ Destroy().
		::PostQuitMessage( 0 ); //Destroy();
	}
	else
	{
		if( eEvent.CanVeto() )
			eEvent.Veto();
	}
}

void CMyFrame::SetMenuCheckBox( bool bCheck, EMenuIds eMenuIds )
{
	if( m_pMenuView )
	{
		wxMenuItem* pItem = m_pMenuView->FindChildItem( eMenuIds );
		if( pItem )
			pItem->Check( bCheck );
	}
}