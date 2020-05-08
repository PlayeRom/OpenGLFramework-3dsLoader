#pragma once

class CMyFrame;

#define MAX_PATH_LEN 	512

const wxSize g_aResolutions[] = { wxSize( 2048, 1536 ),
								  wxSize( 1920, 1440 ),
								  wxSize( 1600, 1200 ),
								  wxSize( 1280,  960 ),
								  wxSize( 1152,  864 ),
								  wxSize( 1024,  768 ),
								  wxSize(  800,  600 ),
								  wxSize(  640,  480 ),
								  wxSize(  320,  240 ),
								  wxSize(    0,    0 )	// ostatni element zawsze 0
								};

class CMyApp : public wxApp
{
private:
	struct SUserStoreData
	{
		bool		m_bMaximized;
		bool		m_bShowFps;
		wxPoint		m_wxPosOfWindow;
		wxSize		m_wxSizeOfScreen;	// rozdzielczosc monitora
		wxSize		m_wxSizeOfOGL;		// rozdzielczosc okna OpenGL
		wxSize		m_wxSizeOfWindow;	// rozmiar okna calej apliakcji
		int			m_iFps;

		// Je¿eli true to jakakolwiek akcja (ruch myszki, klawisze) zwiêksza wydajnoœæ, poprzez nielimitowane zwiêkszenie FPS.
		// Je¿eli true to g_iFpsLimit nie oznacza ju¿ limitu FPS, ale minimaln¹ wartoœæ FPS.
		bool		m_bImprovePerf;
		bool		m_bSmoothNormals;
		bool		m_bUseIndexing;

		wxChar		m_str3dsFile[ MAX_PATH_LEN ];
		wxChar		m_strTexFile[ MAX_PATH_LEN ];
		wxChar		m_strBumpFile[ MAX_PATH_LEN ];
	};

public:
	virtual bool OnInit();
	virtual int OnExit();

	CMyFrame* GetMyFrame() const			{ return m_pMyFrame; }
	const wxPoint& GetPosOfWindow() const	{ return m_wxPosOfWindow; }
	const wxSize& GetSizeOfScreen() const	{ return m_wxSizeOfScreen; }
	const wxSize& GetSizeOfOGL() const		{ return m_wxSizeOfOGL; }
	const wxSize& GetSizeOfUserOGL() const	{ return m_sUserStoreData.m_wxSizeOfOGL; }
	const wxSize& GetSizeOfWindow() const	{ return m_wxSizeOfWindow; }

	int GetResolutionsArraySize() const;

	bool LoadConfig();
	bool SaveConfig();

	const SUserStoreData& GetUserStoreData() const { return m_sUserStoreData; }
	SUserStoreData& SetUserStoreData() { return m_sUserStoreData; }

	const wxString& GetPathToExe() const	{ return m_strPathToExe; }
	const wxString& GetPathWithExe() const	{ return m_strPathWithExe; }

private:
	void OnQueryEndSession( wxCloseEvent& eEvent );
	void OnEndSession( wxCloseEvent& eEvent );

	void ComputeWindowsSizes();

	wxString GetCmdLine( int iIndex ) const;
	void InitialPaths();

	CMyFrame*	m_pMyFrame;

	wxString	m_strPathToExe;
	wxString	m_strPathWithExe;

	// Zmienne wyliczone (defaultowe). Kopie za zmianami u¿ytkownika (ktore sa saveowane) przechowuje SUserStoreData.
	wxPoint		m_wxPosOfWindow;
	wxSize		m_wxSizeOfScreen;	// rozdzielczosc monitora
	wxSize		m_wxSizeOfOGL;		// rozdzielczosc okna OpenGL
	wxSize		m_wxSizeOfWindow;	// rozmiar okna calej apliakcji

	SUserStoreData	m_sUserStoreData;

	DECLARE_EVENT_TABLE()
};
