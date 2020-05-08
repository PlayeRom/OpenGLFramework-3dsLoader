#pragma once

class CMyPanel;

class CMyFrame : public wxFrame
{
public:
	// IDs for the controls and the menu commands
	enum EMenuIds
	{
		EMenuViewResetSizes		= 1,
		EMenuViewResizeOGL,
		EMenuViewShowFPS,
		EMenuViewSetFPS,
		EMenuViewImpPerf,
		EMenuViewSmoothNomrals,
		EMenuViewUseIndexing,

		EMenuFileOpen3ds,
		EMenuFileLoadTxt,
		EMenuFileExportTo3DObj,
		EMenuFileExit,

		EMenuHelpAbout
	};

	CMyFrame( const wxString& title,
			  const wxPoint& pos = wxDefaultPosition,
			  const wxSize& size = wxDefaultSize,
			  long style = wxDEFAULT_FRAME_STYLE );

	void Initialize();

	void DoClose( bool bForce = true );

	wxSize GetSizeStatusBar() const;
	CMyPanel* GetMyPanel() const	{ return m_pMyPanel; }

	void SetMenuCheckBox( bool bCheck, EMenuIds eMenuIds );

protected:
	void OnMenuFileOpen3ds( wxCommandEvent& eEvent );
	void OnMenuFileLoadTxt( wxCommandEvent& eEvent );
	void OnMenuFileExportTo3DObj( wxCommandEvent& eEvent );
	void OnMenuFileExit( wxCommandEvent& eEvent );

	void OnMenuViewResetSizes( wxCommandEvent& eEvent );
	void OnMenuViewResizeOGL( wxCommandEvent& eEvent );
	void OnMenuViewShowFPS( wxCommandEvent& eEvent );
	void OnMenuViewSetFPS( wxCommandEvent& eEvent );
	void OnMenuViewImprovePerf( wxCommandEvent& eEvent );
	void OnMenuViewSmoothNormals( wxCommandEvent& eEvent );
	void OnMenuViewUseIndexing( wxCommandEvent& eEvent );
	
	void OnMenuHelpAbout( wxCommandEvent& eEvent );
	void OnClose( wxCloseEvent& eEvent );

private:
	void CreateMenu();
	void CreateStatusBar();
	void CreateMyPanel();

	wxMenuBar*		m_pMenuBar;
	wxMenu*			m_pMenuFile;
	wxMenu*			m_pMenuView;
	wxMenu*			m_pMenuHelp;

	wxStatusBar*	m_pStatusBar;
	CMyPanel*		m_pMyPanel;

	DECLARE_EVENT_TABLE()
};
