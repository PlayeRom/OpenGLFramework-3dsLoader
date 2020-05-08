
#include "StdAfx.h"
#include "MyGLCanvas.h"

#if wxUSE_GLCANVAS

BEGIN_EVENT_TABLE( CMyGLCanvas, wxGLCanvas )
	EVT_SIZE( CMyGLCanvas::OnSize )
	EVT_PAINT( CMyGLCanvas::OnPaint )
	EVT_ERASE_BACKGROUND( CMyGLCanvas::OnEraseBackground )
	EVT_LEFT_DOWN( CMyGLCanvas::OnMouseLDown )
	EVT_LEFT_UP( CMyGLCanvas::OnMouseLUp )
	EVT_RIGHT_DOWN( CMyGLCanvas::OnMouseRDown )
	EVT_RIGHT_UP( CMyGLCanvas::OnMouseRUp )
	EVT_MOUSEWHEEL( CMyGLCanvas::OnMouseWheel )
	EVT_MOTION( CMyGLCanvas::OnMouseMove )
	EVT_KEY_DOWN( CMyGLCanvas::OnKeyDown )
	EVT_KEY_UP( CMyGLCanvas::OnKeyUp )
	EVT_TIMER( CMyGLCanvas::ETimerId, CMyGLCanvas::OnTimer )
END_EVENT_TABLE()

CMyGLCanvas::CMyGLCanvas( wxWindow* pParent,
						  wxWindowID id /*= wxID_ANY*/,
						  const wxPoint& pos /*= wxDefaultPosition*/,
						  const wxSize& size /*= wxDefaultSize*/,
						  long style /*= 0*/,
						  int* attribList /*= 0*/,
						  const wxString& name /*= _T("GLCanvas")*/ )
						  : wxGLCanvas( pParent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, name, attribList )
{
	m_pFramework = NULL;
	m_wxTimer.SetOwner( this, CMyGLCanvas::ETimerId );
	m_iTimerCouner = 0;
}

CMyGLCanvas::~CMyGLCanvas()
{
	SetCurrent();

	m_wxTimer.Stop();

	delete m_pFramework;
	m_pFramework = NULL;
}

void CMyGLCanvas::Initialize()
{
	SetCurrent();

	m_pFramework = new CFrameworkWrapper();
	if( m_pFramework )
	{
		m_pFramework->Run( GetSize().GetWidth(), GetSize().GetHeight() );
		m_pFramework->SetWindowHandle( GetHandle() );

		DoShowFps( GetMyApp()->GetUserStoreData().m_bShowFps );
		DoSmoothNormals( GetMyApp()->GetUserStoreData().m_bSmoothNormals );

		ReLoad3dsFileAndTextures();
	}

	SetTimer();
}

void CMyGLCanvas::SetTimer()
{
	if( m_wxTimer.IsRunning() )
		m_wxTimer.Stop();

	m_wxTimer.Start( 1000 / GetMyApp()->GetUserStoreData().m_iFps, false );  // zapewnia FPS na m_iFps
}

void CMyGLCanvas::OnPaint( wxPaintEvent& WXUNUSED( eEvent ) )
{
	wxPaintDC dc( this );

	if( m_pFramework )
	{
		SetCurrent();

		m_pFramework->DrawGLScene();
	
		SwapBuffers();
	}
}

void CMyGLCanvas::OnSize( wxSizeEvent& eEvent )
{
	wxGLCanvas::OnSize( eEvent );

	if( m_pFramework )
	{
		SetCurrent();

		m_pFramework->RestartWindow( eEvent.GetSize().GetWidth(), eEvent.GetSize().GetHeight() );
		ReLoad3dsFileAndTextures();

		Refresh( false );
	}
}

void CMyGLCanvas::OnEraseBackground( wxEraseEvent& WXUNUSED( eEvent ) )
{
	// Do nothing, to avoid flashing on MSW
}

void CMyGLCanvas::OnMouseLDown( wxMouseEvent& eEvent )
{
	if( m_pFramework )
		m_pFramework->MouseLButtonDown( eEvent.GetX(), eEvent.GetY() );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnMouseLUp( wxMouseEvent& eEvent )
{
	if( m_pFramework )
		m_pFramework->MouseLButtonUp( eEvent.GetX(), eEvent.GetY() );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnMouseRDown( wxMouseEvent& eEvent )
{
	if( m_pFramework )
		m_pFramework->MouseRButtonDown( eEvent.GetX(), eEvent.GetY() );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnMouseRUp( wxMouseEvent& eEvent )
{
	if( m_pFramework )
		m_pFramework->MouseRButtonUp( eEvent.GetX(), eEvent.GetY() );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnMouseWheel( wxMouseEvent& eEvent )
{
	if( m_pFramework )
		m_pFramework->MouseWheel( eEvent.GetWheelRotation() );

	//eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnMouseMove( wxMouseEvent& eEvent )
{
	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnKeyDown( wxKeyEvent& eEvent )
{
	wxUint32 uiKeyCode = eEvent.GetRawKeyCode();
	if( uiKeyCode == 0 )
		return;

	if( m_pFramework )
		m_pFramework->KeyDown( uiKeyCode );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnKeyUp( wxKeyEvent& eEvent )
{
	wxUint32 uiKeyCode = eEvent.GetRawKeyCode();
	if( uiKeyCode == 0 )
		return;

	if( m_pFramework )
		m_pFramework->KeyUp( uiKeyCode );

	eEvent.Skip();

	if( GetMyApp()->GetUserStoreData().m_bImprovePerf )
		Refresh( false );
}

void CMyGLCanvas::OnTimer( wxTimerEvent& eEvent )
{
	if( ++m_iTimerCouner >= GetMyApp()->GetUserStoreData().m_iFps ) // minela sekunda
	{
		m_iTimerCouner = 0;

		if( m_pFramework )
			m_pFramework->Timer();
	}

	Refresh( false );
}

void CMyGLCanvas::DoShowFps( bool bShow )
{
	if( m_pFramework )
		m_pFramework->DoShowFps( bShow );
}

void CMyGLCanvas::DoSmoothNormals( bool bSmooth )
{
	if( m_pFramework )
	{
		m_pFramework->DoSmoothNormals( bSmooth );
		ReLoad3dsFileAndTextures();
	}
}

void CMyGLCanvas::DoUseIndexing( bool bUse )
{
	if( m_pFramework )
	{
		m_pFramework->DoUseIndexing( bUse );
		ReLoad3dsFileAndTextures();
	}
}

bool CMyGLCanvas::ReLoad3dsFileAndTextures()
{
	if( m_pFramework )
		return m_pFramework->ReLoad3dsFileAndTextures( GetMyApp()->GetUserStoreData().m_str3dsFile,
													   GetMyApp()->GetUserStoreData().m_strTexFile,
													   GetMyApp()->GetUserStoreData().m_strBumpFile )
													   ? true : false;
	return false;
}

bool CMyGLCanvas::Export3dsTo3DObj( wxString strPath, bool bIndexing, bool bFixed )
{
	if( m_pFramework )
	{
		if( !strPath.IsEmpty() )
		{
			int iIndex = static_cast< int >( strPath.Length() ) - 1;
			if( strPath[ iIndex ] != wxChar('\\') && strPath[ iIndex ] != wxChar('/') )
				strPath.Append( wxChar('\\') );
		}

		return m_pFramework->Export3dsTo3DObj( strPath.c_str(), bIndexing, bFixed ) ? true : false;
	}

	return false;
}

#endif // wxUSE_GLCANVAS