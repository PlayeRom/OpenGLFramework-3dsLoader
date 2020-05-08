/*! \file
	\brief Modu� makr.
*/

#ifndef _DEFINE_H_
#define _DEFINE_H_

/// Nazwa frameworka
#define FRAME_NAME _T(" PlayeRom OpenGLFramework v.1.05 ")

/// Nazwa gry/programu.
#define GAME_NAME _T("OpenGLFramework 3ds Loader")
/// Adres strony.
#define HOMEPAGE _T("www.your.homepage.com")

/// \name Wersja gry - ka�dy cz�on mo�e by� w zakresie od 0 do 255.
//@{
#define MAJOR_VERSION 			1
#define MINOR_VERSION 			0
#define RELEASE_VERSION 		0
//@}

/// \name Wersja pliku konfiguracyjnego - ka�dy cz�on mo�e by� w zakresie od 0 do 255.
//@{
#define MAJOR_VERSION_FILE 		1
#define MINOR_VERSION_FILE 		0
#define RELEASE_VERSION_FILE 	0
//@}

/// Nazwa pliku konfiguracyjnego.
#define SETTING_FILE_NAME "setting.cfg"

/// Maksymalna d�ugo�� tekstu w jednej linii dla konsoli.
#define CONSOLE_MSG_LENGTH 128

#define MAX_3DS_NAME 256

#define _3DSLOADER_

/// Sta�a PI.
#define PI 3.141592f//6535897932384626433832795f
/// Makro przekszta�cenia stopni na radiany.
#define DEG2RAD(x) ((x*PI)/180.0f)

// UWAGA! U�ycie, b�d� nie UNICODE ustawiamy we w�a�ciwo�ciach projektu:
// Alt+F7 -> Configuration Properties -> General -> Character set

//################################################################
/// Jakiej biblioteki d�wi�kowej u�y�?
/*! Je�eli makro to zostanie ustawione na 1 to do odgrywania d�wi�k�w i muzyki u�yta zostanie biblioteka OpenAL.
Je�eli ustawimy to makro na 0 to u�yta zostanie biblioteka FMOD. Domy�lnie makro _USE_SOUND_OPENAL_ jest zdefiniowane. */
#define _USE_SOUND_OPENAL_	1

/// Czy w OpenAL u�ywa� formatu pliku muzycznego OGG?
/** Ustawiaj�c to makro na 1 udost�pnimy mo�liwo�� odgrywania plik�w OGG poprzez bibliotek� OpenAL. Oznacza to,
�e makro \ref _USE_SOUND_OPENAL_ tak�e musi by� ustawione na 1. */
#define _USE_OGG_		0
//################################################################

/// Zapytanie o tryb pe�noekranowy.
/*! Zdefiniowanie tego makra spowoduje, i� w trakcie uruchamiania naszej aplikacji, zostaniemy zapytani czy program
ma si� uruchomi� w trybie pe�noekranowym czy te� nie. Je�eli makro jest niezdefiniowane (i tak jest domy�lnie) � to
program zawsze uruchomi si� na pe�nym ekranie. */
#define _QUESTION_FULLSCREEN_	1

/// Czy pobra� systemow� rozdzielczo�� i kolory?
/*! Je�eli makro to jest zdefiniowane to w trakcie inicjalizacji okna, rozdzielczo�� i gl�bia kolor�w zostan� pobrane
z ustawien systemowych pulpitu. Je�eli chcemy aby u�yte by�y warto�ci zdefiniowane przez nas (patrz konstruktor klasy
CWindowData), makro nale�y dezaktywowa�. */
#define _USE_SYSTEM_RES_AND_BITSPERPIXEL_	1

#if !_USE_SYSTEM_RES_AND_BITSPERPIXEL_
	// Tutaj ustaw w�asn�, sta�� rozdzielczo�� ekranu oraz g��bi� kolor�w
	#define _SCREEN_WIDTH_	800
	#define _SCREEN_HEIGHT_	600
	#define _SCREEN_BITS_	32
#endif

/// Czy u�y� trybu tylko 2D?
/*! Zdefiniuj poni�sze makro je�eli twoja gra u�ywa� ma jedynie trybu 2D.
Je�eli gra ma wykorzystywa� grafik� 3D, makro nale�y dezaktywowa�. */
#define _USE_2D_ONLY_	0

// Wy��czenie warning�w o niezgodno�ci z ISO
//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_NON_CONFORMING_SWPRINTFS

//################################################################
/// Jakiej czcionki u�y� w konsoli? 
/*! Jakiej czcionki (sposobu rysowania) u�y� w konsoli?
Je�eli oba makra dezaktywowane to u�yta zostajnie CSDLFont. Zalecane u�ycie CSDLFont. */
#define _USE_CONSOLE_WITH_MY_FONT_		0
#define _USE_CONSOLE_WITH_BITMAP_FONT_	0

// Je�eli zdefiniowane oba _USE_CONSOLE_WITH_MY_FONT_ i _USE_CONSOLE_WITH_BITMAP_FONT_ to u�ywamy CSDLFont
#if _USE_CONSOLE_WITH_MY_FONT_
	#if _USE_CONSOLE_WITH_BITMAP_FONT_
		#undef _USE_CONSOLE_WITH_MY_FONT_
		#undef _USE_CONSOLE_WITH_BITMAP_FONT_
	#endif
#endif
//################################################################

/// Czy u�y� profilera?
/** Wszelkie wywo�ania funkcji CProfiler::Begin oraz CProfiler::End dobrze jest wywo�ywa� za pomoc� makr
PROFILER_BEGIN oraz PROFILER_END. W�wczas przypisuj�c _USE_PROFILER_ 1 lub 0 w �atwy spos�b b�dziemy mogli aktywowa�
i dezaktywowa� profiler w aplikacji. */
#define _USE_PROFILER_	0

#if _USE_PROFILER_
	#define PROFILER_BEGIN( strDesc )	CProfiler::GetInstance()->Begin( strDesc );
	#define PROFILER_END()				CProfiler::GetInstance()->End();
	#define PROFILER_FLUSH()			CProfiler::GetInstance()->Flush();
#else
	#define PROFILER_BEGIN( strDesc )
	#define PROFILER_END()
	#define PROFILER_FLUSH()
#endif

/// Maksymalna odleg�o�� tylnej p�aszczyzny obcinania.
#define MAX_DISTANCE_VIEW 5000.0f

#define DEFAULT_SOUND_VOLUME			90
#define DEFAULT_MUSIC_VOLUME			60

#define USE_WXWIDGETS_CONTAINER			1

#endif //_DEFINE_H_
