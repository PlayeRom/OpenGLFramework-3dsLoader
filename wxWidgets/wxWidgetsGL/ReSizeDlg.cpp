
#include "StdAfx.h"
#include "MyPanel.h"
#include "ReSizeDlg.h"

BEGIN_EVENT_TABLE( CReSizeDlg, wxDialog )
	EVT_BUTTON( EButtonOk,		CReSizeDlg::OnOK )
	EVT_BUTTON( EButtonCancel,	CReSizeDlg::OnCancel )
	EVT_BUTTON( EButtonDefault,	CReSizeDlg::OnDefault )
	EVT_CLOSE(					CReSizeDlg::OnClose )
	EVT_COMBOBOX( EComboWidth,	CReSizeDlg::OnComboBoxWidth )
	EVT_COMBOBOX( EComboHeight, CReSizeDlg::OnComboBoxHeight )
END_EVENT_TABLE()

CReSizeDlg::CReSizeDlg( wxSize actualSize,
					    wxWindow* pParent,
						wxWindowID id,
						const wxString& title,
						const wxPoint& pos,
						const wxSize& size,
						long style,
						const wxString& name )
						: wxDialog( pParent, id, title, pos, size, style, name )
{
	m_pMyPanel = dynamic_cast< CMyPanel* >( pParent );
	m_sizeActual = actualSize;

	BuildResolutionArrays();

	wxString strWidth = wxT("");
	wxString strHeight = wxT("");

	strWidth.Printf( wxT("%d"), m_sizeActual.GetWidth() );
	strHeight.Printf( wxT("%d"), m_sizeActual.GetHeight() );

	int iSeparateBetweenLines = 8;
	wxPoint ptIniPoint( 10, 10 );
	wxSize sizeCombo( 100, 24 );

	wxStaticText* pTextWitdh = new wxStaticText( this, wxID_ANY, wxT("Width in pixels"), ptIniPoint, sizeCombo );

	ptIniPoint += wxPoint( sizeCombo.GetWidth() + iSeparateBetweenLines, 0 );
	new wxStaticText( this, wxID_ANY, wxT("Height in pixels"), ptIniPoint, sizeCombo );


	ptIniPoint = wxPoint( 10, pTextWitdh->GetSize().GetHeight() + ( iSeparateBetweenLines * 2 ) );
	m_pComboWidth = new wxComboBox( this, EComboWidth, strWidth, ptIniPoint, sizeCombo, m_aWidthArray, wxCB_DROPDOWN /*| wxCB_READONLY*/ );

	ptIniPoint += wxPoint( sizeCombo.GetWidth() + iSeparateBetweenLines, 0 );
	m_pComboHeight = new wxComboBox( this, EComboHeight, strHeight, ptIniPoint, sizeCombo, m_aHeightArray, wxCB_DROPDOWN /*| wxCB_READONLY*/ );


	ptIniPoint = wxPoint( 10, 10 + pTextWitdh->GetSize().GetHeight() + sizeCombo.GetHeight() + ( iSeparateBetweenLines * 4 ) );
	m_pButtonOK = new wxButton( this, EButtonOk, _T("OK"), ptIniPoint );

	ptIniPoint += wxPoint( m_pButtonOK->GetSize().GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonCancel = new wxButton( this, EButtonCancel, _T("Cancel"), ptIniPoint );

	ptIniPoint += wxPoint( m_pButtonCancel->GetSize().GetWidth() + iSeparateBetweenLines, 0 );
	m_pButtonDefault = new wxButton( this, EButtonDefault, _T("Default"), ptIniPoint );

	SetAffirmativeId( EButtonOk );
	SetEscapeId( EButtonCancel );
}

CReSizeDlg::~CReSizeDlg()
{
	m_aWidthArray.Clear();
	m_aHeightArray.Clear();
}

void CReSizeDlg::OnOK( wxCommandEvent& WXUNUSED( eEvent ) )
{
	ExecuteNewSize();

	EndModal( 1 );
}

void CReSizeDlg::OnCancel( wxCommandEvent& WXUNUSED( eEvent ) )
{
	CancelSize();
}

void CReSizeDlg::OnDefault( wxCommandEvent& WXUNUSED( eEvent ) )
{
	ExecuteDefaultSize();
}

void CReSizeDlg::OnClose( wxCloseEvent& WXUNUSED( eEvent ) )
{
	CancelSize();
}

void CReSizeDlg::OnComboBoxWidth( wxCommandEvent& WXUNUSED( eEvent ) )
{
	ExecuteNewSize();
}

void CReSizeDlg::OnComboBoxHeight( wxCommandEvent& WXUNUSED( eEvent ) )
{
	ExecuteNewSize();
}

void CReSizeDlg::BuildResolutionArrays()
{
	m_aWidthArray.Clear();
	m_aHeightArray.Clear();

	for( int i = GetMyApp()->GetResolutionsArraySize() - 1; i >= 0; --i )
	{
		wxString strWidth;
		wxString strHeight;

		strWidth.Printf( wxT("%d"), g_aResolutions[ i ].GetWidth() );
		strHeight.Printf( wxT("%d"), g_aResolutions[ i ].GetHeight() );

		m_aWidthArray.Add( strWidth );
		m_aHeightArray.Add( strHeight );
	}
}

wxSize CReSizeDlg::GetNewSize() const
{
	if( m_pComboWidth->GetValue().IsEmpty() || m_pComboHeight->GetValue().IsEmpty() )
		return wxDefaultSize;
	
	return wxSize( _wtoi( m_pComboWidth->GetValue().c_str() ),
				   _wtoi( m_pComboHeight->GetValue().c_str() ) );
}

void CReSizeDlg::ExecuteNewSize()
{
	wxSize sizeNew( GetNewSize() );
	if( sizeNew != wxDefaultSize && m_pMyPanel )
		m_pMyPanel->ReSizeOGL( sizeNew );
}

void CReSizeDlg::ExecuteDefaultSize()
{
	wxString strWidth = wxT("");
	wxString strHeight = wxT("");

	strWidth.Printf( wxT("%d"), GetMyApp()->GetSizeOfOGL().GetWidth() );
	strHeight.Printf( wxT("%d"), GetMyApp()->GetSizeOfOGL().GetHeight() );

	m_pComboWidth->SetValue( strWidth );
	m_pComboHeight->SetValue( strHeight );

	ExecuteNewSize();
}

void CReSizeDlg::CancelSize()
{
	if( m_pMyPanel )
		m_pMyPanel->ReSizeOGL( m_sizeActual );

	EndModal( 0 );
}