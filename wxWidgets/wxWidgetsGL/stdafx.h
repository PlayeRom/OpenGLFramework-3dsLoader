#pragma once

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/gdicmn.h>
#include <wx/aboutdlg.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/numdlg.h>
#include "../OpenGLFramework/FrameworkWrapper.h"
#include "MyApp.h"
#include "MyFrame.h"

DECLARE_APP( CMyApp )

#define GetMyApp()				( ( CMyApp* )&::wxGetApp() )
#define GetMyFrame()			GetMyApp()->GetMyFrame()

#define TITLE_APP		wxT("3DSLoader")
#define VERSION			wxT("1.01")
#define FILE_SETTINGS	wxT("settings.cfg")

//pozycja i rozmiar okna aplikacji
#define WINDOW_POS_X		0
#define WINDOW_POS_Y		0

// Szerokosc i wysokosc obramowania okna
#define FRAME_SIZE_X		wxSystemSettings::GetMetric( wxSYS_FRAMESIZE_X )
#define FRAME_SIZE_Y		wxSystemSettings::GetMetric( wxSYS_FRAMESIZE_Y )

// Wysokosc belki aplikacji
#define CAPTION_Y			wxSystemSettings::GetMetric( wxSYS_CAPTION_Y )

// Wyskoœæ belki z menu aplikacji
#define MENU_Y				wxSystemSettings::GetMetric( wxSYS_MENU_Y )

#define BORDER_X			wxSystemSettings::GetMetric( wxSYS_BORDER_X )
#define BORDER_Y			wxSystemSettings::GetMetric( wxSYS_BORDER_Y )

#define EDGE_X				wxSystemSettings::GetMetric( wxSYS_EDGE_X )
#define EDGE_Y				wxSystemSettings::GetMetric( wxSYS_EDGE_Y )

// Szerokoœæ pionowego scrollbara
#define VSCROLL_X			wxSystemSettings::GetMetric( wxSYS_VSCROLL_X )

// Wysokoœæ poziomego scrollbara
#define HSCROLL_Y			wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y )

// Rozdzielczosc ekranu
#define SCREEN_WIDTH		wxSystemSettings::GetMetric( wxSYS_SCREEN_X )
#define SCREEN_HEIGHT		wxSystemSettings::GetMetric( wxSYS_SCREEN_Y )