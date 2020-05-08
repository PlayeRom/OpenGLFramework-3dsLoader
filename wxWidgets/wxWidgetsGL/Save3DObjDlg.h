#pragma once

#include <wx/dialog.h>

class CSave3DObjDlg : public wxDialog
{
public:
	CSave3DObjDlg( wxWindow* pParent,
				   wxWindowID id,
				   const wxString& title,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxDEFAULT_DIALOG_STYLE,
				   const wxString& name = wxT("dialogBox") );
	~CSave3DObjDlg();

	//wxString Get3DObjFile() const		{ return m_pTxtCtrl3DObjFile->GetValue(); }
	wxFileName Get3DObjFile() const { return wxFileName( m_pTxtCtrl3DObjFile->GetValue() ); }
	bool IsFixed() const			{ return m_pCheckBoxFixed->IsChecked(); }
	bool IsIndexing() const			{ return m_pCheckBoxIndexing->IsChecked(); }

protected:
	enum
	{
		ETxtCtrl3DObjFile,
		EButton3DObjFile,
		ECheckBoxFixed,
		ECheckBoxIndexing,
		EButtonOk,
		EButtonCancel
	};

	void OnButton3DObjFile( wxCommandEvent& eEvent );
	void OnCheckBoxFixed( wxCommandEvent& eEvent );

	void OnOK( wxCommandEvent& eEvent );
	void OnCancel( wxCommandEvent& eEvent );
	void OnClose( wxCloseEvent& eEvent );

private:
	wxString GetDefaultPathAndFullName();
	wxString GetFullFileName();

	wxTextCtrl*		m_pTxtCtrl3DObjFile;
	wxButton*		m_pButton3DObjFile;
	wxCheckBox*		m_pCheckBoxFixed;
	wxCheckBox*		m_pCheckBoxIndexing;

	wxButton*		m_pButtonOK;
	wxButton*		m_pButtonCancel;

	DECLARE_EVENT_TABLE()
};
