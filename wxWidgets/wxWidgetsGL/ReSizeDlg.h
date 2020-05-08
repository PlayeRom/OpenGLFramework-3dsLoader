#pragma once

#include <wx/dialog.h>

class CMyPanel;

class CReSizeDlg : public wxDialog
{
public:
	CReSizeDlg( wxSize actualSize,
				wxWindow* pParent,
				wxWindowID id,
				const wxString& title,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxDEFAULT_DIALOG_STYLE,
				const wxString& name = wxT("dialogBox") );
	~CReSizeDlg();

	wxSize GetNewSize() const;

protected:
	enum
	{
		EComboWidth,
		EComboHeight,
		EButtonOk,
		EButtonCancel,
		EButtonDefault
	};

	void OnOK( wxCommandEvent& eEvent );
	void OnCancel( wxCommandEvent& eEvent );
	void OnDefault( wxCommandEvent& eEvent );
	void OnClose( wxCloseEvent& eEvent );
	void OnComboBoxWidth( wxCommandEvent& eEvent );
	void OnComboBoxHeight( wxCommandEvent& eEvent );

private:
	CMyPanel*		m_pMyPanel;

	wxComboBox*		m_pComboWidth;
	wxComboBox*		m_pComboHeight;
	wxButton*		m_pButtonOK;
	wxButton*		m_pButtonCancel;
	wxButton*		m_pButtonDefault;

	wxArrayString	m_aWidthArray;
	wxArrayString	m_aHeightArray;
	wxSize			m_sizeActual;

	void BuildResolutionArrays();
	void ExecuteNewSize();
	void ExecuteDefaultSize();
	void CancelSize();

	DECLARE_EVENT_TABLE()
};
