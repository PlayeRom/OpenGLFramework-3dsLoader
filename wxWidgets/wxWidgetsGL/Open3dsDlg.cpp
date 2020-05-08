
#include "StdAfx.h"
#include "Open3dsDlg.h"

BEGIN_EVENT_TABLE( COpen3dsDlg, wxDialog )
	EVT_BUTTON( EButton3dsFile,		COpen3dsDlg::OnButton3dsFile )
	EVT_BUTTON( EButtonTexFile,		COpen3dsDlg::OnButtonTexFile )
	EVT_BUTTON( EButtonBumpFile,	COpen3dsDlg::OnButtonBumpFile )
	EVT_BUTTON( EButtonOk,			COpen3dsDlg::OnOK )
	EVT_BUTTON( EButtonCancel,		COpen3dsDlg::OnCancel )
	EVT_CLOSE(						COpen3dsDlg::OnClose )
END_EVENT_TABLE()

COpen3dsDlg::COpen3dsDlg( wxWindow* pParent,
						  wxWindowID id,
						  const wxString& title,
						  const wxPoint& pos,
						  const wxSize& size ,
						  long style,
						  const wxString& name )
						  : wxDialog( pParent, id, title, pos, size, style, name )
{
	int iSeparateBetweenLines = 8;
	wxPoint ptIniPoint( 10, 10 );
	wxSize sizeTextCtrl( 600, 24 );
	wxSize sizeChoseButton( 50, 24 );
	wxPoint ptButton;

	wxStaticText* pText = new wxStaticText( this, wxID_ANY, wxT("3ds &file:"), ptIniPoint );
	
	ptIniPoint += wxPoint( 0, pText->GetSize().GetHeight() + iSeparateBetweenLines );
	m_pTxtCtrl3dsFile = new wxTextCtrl( this, ETxtCtrl3dsFile, wxT(""), ptIniPoint, sizeTextCtrl );
	m_pTxtCtrl3dsFile->SetValue( GetMyApp()->SetUserStoreData().m_str3dsFile );

	ptButton = ptIniPoint + wxPoint( sizeTextCtrl.GetWidth() + iSeparateBetweenLines, 0 );
	m_pButton3dsFile = new wxButton( this, EButton3dsFile, wxT("..."), ptButton, sizeChoseButton );

	/////////////////////////////////////////

	ptIniPoint += wxPoint( 0, sizeTextCtrl.GetHeight() + iSeparateBetweenLines );
	pText = new wxStaticText( this, wxID_ANY, wxT("&Texture file:"), ptIniPoint );

	ptIniPoint += wxPoint( 0, pText->GetSize().GetHeight() + iSeparateBetweenLines );
	m_pTxtCtrlTexFile = new wxTextCtrl( this, ETxtCtrlTexFile, wxT(""), ptIniPoint, sizeTextCtrl );
	m_pTxtCtrlTexFile->SetValue( GetMyApp()->SetUserStoreData().m_strTexFile );

	ptButton = ptIniPoint + wxPoint( sizeTextCtrl.GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonTexFile = new wxButton( this, EButtonTexFile, wxT("..."), ptButton, sizeChoseButton );

	/////////////////////////////////////////

	ptIniPoint += wxPoint( 0, sizeTextCtrl.GetHeight() + iSeparateBetweenLines );
	pText = new wxStaticText( this, wxID_ANY, wxT("&Bump-texture file:"), ptIniPoint );

	ptIniPoint += wxPoint( 0, pText->GetSize().GetHeight() + iSeparateBetweenLines );
	m_pTxtCtrlBumpFile = new wxTextCtrl( this, ETxtCtrlBumpFile, wxT(""), ptIniPoint, sizeTextCtrl );
	m_pTxtCtrlBumpFile->SetValue( GetMyApp()->SetUserStoreData().m_strBumpFile );

	ptButton = ptIniPoint + wxPoint( sizeTextCtrl.GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonBumpFile = new wxButton( this, EButtonBumpFile, wxT("..."), ptButton, sizeChoseButton );

	/////////////////////////////////////////
	/////////////////////////////////////////

	ptIniPoint += wxPoint( 0, sizeTextCtrl.GetHeight() + ( iSeparateBetweenLines ) );
	m_pButtonOK = new wxButton( this, EButtonOk, wxT("&OK"), ptIniPoint );

	ptIniPoint += wxPoint( m_pButtonOK->GetSize().GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonCancel = new wxButton( this, EButtonCancel, wxT("&Cancel"), ptIniPoint );

	SetAffirmativeId( EButtonOk );
	SetEscapeId( EButtonCancel );
}

COpen3dsDlg::~COpen3dsDlg()
{
}

void COpen3dsDlg::OnButton3dsFile( wxCommandEvent& WXUNUSED( eEvent ) )
{
	OpenFile( true, m_pTxtCtrl3dsFile );
}

void COpen3dsDlg::OnButtonTexFile( wxCommandEvent& WXUNUSED( eEvent ) )
{
	OpenFile( false, m_pTxtCtrlTexFile );
}

void COpen3dsDlg::OnButtonBumpFile( wxCommandEvent& WXUNUSED( eEvent ) )
{
	OpenFile( false, m_pTxtCtrlBumpFile );
}

void COpen3dsDlg::OnOK( wxCommandEvent& WXUNUSED( eEvent ) )
{
	EndModal( 1 );
}

void COpen3dsDlg::OnCancel( wxCommandEvent& WXUNUSED( eEvent ) )
{
	EndModal( 0 );
}

void COpen3dsDlg::OnClose( wxCloseEvent& WXUNUSED( eEvent ) )
{
	EndModal( 0 );
}

void COpen3dsDlg::OpenFile( bool b3ds, wxTextCtrl*& out_pTexCtrl )
{
	wxString strWildCard;
	if( b3ds )
		strWildCard = wxT("3ds (*.3ds)|*.3ds");
	else // image
	{
		strWildCard = wxT("Image files: |*.bmp;*.tga;*.jeh;");
		strWildCard += wxT("|BMP (*.bmp)|*.bmp");
		strWildCard += wxT("|TGA (*.tga)|*.tga");
		strWildCard += wxT("|JEH (*.jeh)|*.jeh");
	}

	wxString strMsg( wxT("Open file") );
	wxString strDefPath( wxT(".") );

	wxString strFileName( ::wxFileSelector( strMsg, strDefPath, wxT(""), wxT(""),
											strWildCard, wxFD_OPEN | wxFD_FILE_MUST_EXIST, this ) );

	if ( !strFileName.IsEmpty() )
	{
		wxFileName fileName( strFileName );
		fileName.MakeRelativeTo( GetMyApp()->GetPathToExe() );

		out_pTexCtrl->SetValue( fileName.GetFullPath() );
	}
}