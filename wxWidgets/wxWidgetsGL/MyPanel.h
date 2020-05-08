#pragma once

#if wxUSE_GLCANVAS
	class CMyGLCanvas;
#endif

class CMyPanel : public wxScrolledWindow
{
public:
	CMyPanel( wxFrame* pFrame,
			  const wxPoint& pos = wxDefaultPosition,
			  const wxSize& size = wxDefaultSize,
			  long style = wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
	virtual ~CMyPanel();

	void Initialize();

	void ReSizeOGLDlg();
	void ReSizeOGL( const wxSize& sizeNew );
	wxSize GetSizeOfGLCanvas() const;

	void DoShowFps( bool bShow );
	void DoSmoothNormals( bool bSmooth );
	void DoUseIndexing( bool bUse );
	void SetFpsDlg();

	bool ReLoad3dsFileAndTextures();

	void Open3dsFile();
	void Load3dsFromTxtFile();
	void Export3dsTo3DObj();

private:
	void SetScrollbars();
	void FillPathAux( const wxString& strPathSour, wxChar strPathDest[ MAX_PATH_LEN ] );

#if wxUSE_GLCANVAS
	CMyGLCanvas*	m_pMyGLCanvas;
#endif

	DECLARE_EVENT_TABLE()
};
