#pragma once

#if wxUSE_GLCANVAS

#include <wx/glcanvas.h>

class CFrameworkWrapper;

class CMyGLCanvas : public wxGLCanvas
{
public:
	CMyGLCanvas( wxWindow* pParent,
				 wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = 0,
				 int* attribList = 0,
				 const wxString& name = _T("GLCanvas") );
	virtual ~CMyGLCanvas();

	void Initialize();
	void DoShowFps( bool bShow );
	void DoSmoothNormals( bool bSmooth );
	void DoUseIndexing( bool bUse );
	void SetTimer();

	bool ReLoad3dsFileAndTextures();

	bool Export3dsTo3DObj( wxString strPath, bool bIndexing, bool bFixed );

	enum
	{
		ETimerId = 100
	};

protected:
	void OnPaint( wxPaintEvent& eEvent );
	void OnSize( wxSizeEvent& eEvent );
	void OnEraseBackground( wxEraseEvent& eEvent );
	void OnMouseLDown( wxMouseEvent& eEvent );
	void OnMouseLUp( wxMouseEvent& eEvent );
	void OnMouseRDown( wxMouseEvent& eEvent );
	void OnMouseRUp( wxMouseEvent& eEvent );
	void OnMouseWheel( wxMouseEvent& eEvent );
	void OnMouseMove( wxMouseEvent& eEvent );
	void OnKeyDown( wxKeyEvent& eEvent );
	void OnKeyUp( wxKeyEvent& eEvent );
	void OnTimer( wxTimerEvent& eEvent );

private:
	wxTimer				m_wxTimer;
	CFrameworkWrapper*	m_pFramework;

	int					m_iTimerCouner;

	DECLARE_EVENT_TABLE()
};

#endif // wxUSE_GLCANVAS
