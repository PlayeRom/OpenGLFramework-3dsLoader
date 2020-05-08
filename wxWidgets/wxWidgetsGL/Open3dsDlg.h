#pragma once

#include <wx/dialog.h>

class COpen3dsDlg :	public wxDialog
{
public:
	COpen3dsDlg( wxWindow* pParent,
				 wxWindowID id,
				 const wxString& title,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = wxDEFAULT_DIALOG_STYLE,
				 const wxString& name = wxT("dialogBox") );
	~COpen3dsDlg();

	wxString Get3dsFile() const		{ return m_pTxtCtrl3dsFile->GetValue(); }
	wxString GetTexFile() const		{ return m_pTxtCtrlTexFile->GetValue(); }
	wxString GetBumpFile() const	{ return m_pTxtCtrlBumpFile->GetValue(); }

protected:
	enum
	{
		ETxtCtrl3dsFile,
		ETxtCtrlTexFile,
		ETxtCtrlBumpFile,
		EButton3dsFile,
		EButtonTexFile,
		EButtonBumpFile,
		EButtonOk,
		EButtonCancel
	};

	void OnButton3dsFile( wxCommandEvent& eEvent );
	void OnButtonTexFile( wxCommandEvent& eEvent );
	void OnButtonBumpFile( wxCommandEvent& eEvent );

	void OnOK( wxCommandEvent& eEvent );
	void OnCancel( wxCommandEvent& eEvent );
	void OnClose( wxCloseEvent& eEvent );

private:
	void OpenFile( bool b3ds, wxTextCtrl*& out_pTexCtrl );

	wxTextCtrl*		m_pTxtCtrl3dsFile;
	wxTextCtrl*		m_pTxtCtrlTexFile;
	wxTextCtrl*		m_pTxtCtrlBumpFile;

	wxButton*		m_pButton3dsFile;
	wxButton*		m_pButtonTexFile;
	wxButton*		m_pButtonBumpFile;

	wxButton*		m_pButtonOK;
	wxButton*		m_pButtonCancel;

	DECLARE_EVENT_TABLE()
};
