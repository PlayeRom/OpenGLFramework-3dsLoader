
#include "StdAfx.h"
#include "Save3DObjDlg.h"

BEGIN_EVENT_TABLE( CSave3DObjDlg, wxDialog )
	EVT_BUTTON( EButton3DObjFile,	CSave3DObjDlg::OnButton3DObjFile )
	EVT_CHECKBOX( ECheckBoxFixed,	CSave3DObjDlg::OnCheckBoxFixed )
	EVT_BUTTON( EButtonOk,			CSave3DObjDlg::OnOK )
	EVT_BUTTON( EButtonCancel,		CSave3DObjDlg::OnCancel )
	EVT_CLOSE(						CSave3DObjDlg::OnClose )
END_EVENT_TABLE()

const wxString g_strDefDir( wxT("objects\\") );

CSave3DObjDlg::CSave3DObjDlg( wxWindow* pParent,
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
	m_pCheckBoxFixed = NULL;

	wxStaticText* pText = new wxStaticText( this, wxID_ANY, wxT("Save as 3DObj file:"), ptIniPoint );
	
	ptIniPoint += wxPoint( 0, pText->GetSize().GetHeight() + iSeparateBetweenLines );
	m_pTxtCtrl3DObjFile = new wxTextCtrl( this, ETxtCtrl3DObjFile, wxT(""), ptIniPoint, sizeTextCtrl, wxTE_READONLY );
	m_pTxtCtrl3DObjFile->SetValue( GetDefaultPathAndFullName() );

	ptButton = ptIniPoint + wxPoint( sizeTextCtrl.GetWidth() + iSeparateBetweenLines, 0 );
	m_pButton3DObjFile = new wxButton( this, EButton3DObjFile, wxT("..."), ptButton, sizeChoseButton );

	ptIniPoint += wxPoint( 0, sizeChoseButton.GetHeight() + iSeparateBetweenLines );
	m_pCheckBoxFixed = new wxCheckBox( this, ECheckBoxFixed, wxT("&Fixed type (e.g. for OpenGL ES)"), ptIniPoint );
	m_pCheckBoxFixed->SetValue( false );

	ptIniPoint += wxPoint( 0, sizeChoseButton.GetHeight() + iSeparateBetweenLines );
	m_pCheckBoxIndexing = new wxCheckBox( this, ECheckBoxIndexing, wxT("&Indexing of vertices (recommended)"), ptIniPoint );
	m_pCheckBoxIndexing->SetValue( true );

	/////////////////////////////////////////
	/////////////////////////////////////////

	ptIniPoint += wxPoint( 0, m_pCheckBoxIndexing->GetSize().GetHeight() + iSeparateBetweenLines );
	m_pButtonOK = new wxButton( this, EButtonOk, wxT("&Save"), ptIniPoint );

	ptIniPoint += wxPoint( m_pButtonOK->GetSize().GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonCancel = new wxButton( this, EButtonCancel, wxT("&Cancel"), ptIniPoint );

	SetAffirmativeId( EButtonOk );
	SetEscapeId( EButtonCancel );
}

CSave3DObjDlg::~CSave3DObjDlg()
{
}

void CSave3DObjDlg::OnButton3DObjFile( wxCommandEvent& WXUNUSED( eEvent ) )
{
	wxString strDefDir( GetMyApp()->GetPathToExe() + g_strDefDir );

	wxString strDir( ::wxDirSelector( wxDirSelectorPromptStr, strDefDir, wxDD_DEFAULT_STYLE, wxDefaultPosition, this ) );
	if( !strDir.IsEmpty() )
	{
		wxFileName fileName( strDir + wxT("\\") + GetFullFileName() );
		if( fileName.IsAbsolute() )
			fileName.MakeRelativeTo( GetMyApp()->GetPathToExe() );

		m_pTxtCtrl3DObjFile->SetValue( fileName.GetFullPath() );
	}
}

void CSave3DObjDlg::OnCheckBoxFixed( wxCommandEvent& WXUNUSED( eEvent ) )
{
	wxFileName fileName( m_pTxtCtrl3DObjFile->GetValue() );
	m_pTxtCtrl3DObjFile->SetValue( wxString( fileName.GetPath() + wxT("\\") + GetFullFileName() ) );
}

void CSave3DObjDlg::OnOK( wxCommandEvent& WXUNUSED( eEvent ) )
{
	EndModal( 1 );
}

void CSave3DObjDlg::OnCancel( wxCommandEvent& WXUNUSED( eEvent ) )
{
	EndModal( 0 );
}

void CSave3DObjDlg::OnClose( wxCloseEvent& WXUNUSED( eEvent ) )
{
	EndModal( 0 );
}

wxString CSave3DObjDlg::GetDefaultPathAndFullName()
{
	return wxString( g_strDefDir + GetFullFileName() );
}

wxString CSave3DObjDlg::GetFullFileName()
{
	wxFileName fileName( GetMyApp()->GetUserStoreData().m_str3dsFile );
	wxString strFileName( fileName.GetName() );

	if( strFileName.IsEmpty() )
		return strFileName;

	wxString strFixed( wxT("") );
	if( m_pCheckBoxFixed && m_pCheckBoxFixed->IsChecked() )
		strFixed = wxT("_fixed");

	return wxString( strFileName + strFixed + wxT(".3DObj") );
}