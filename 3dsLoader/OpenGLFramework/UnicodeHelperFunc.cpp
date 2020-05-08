
#include "stdafx.h"
#include "UnicodeHelperFunc.h"

wchar_t		CUHF::m_strWCharText[ UHF_MAX_BUFFER ][ UHF_MAX_STRING ] = { 0 };
char		CUHF::m_strCharText[ UHF_MAX_BUFFER][ UHF_MAX_STRING ] = { 0 };
int			CUHF::m_iActialStringBuffer = -1;

size_t CUHF::GetStringLength( LPCTSTR lpString )
{
	if( !lpString )
		return 0;

#ifdef UNICODE
	return wcslen( lpString );
#else
	return strlen( lpString );
#endif
}

errno_t CUHF::CopyStringToString( LPTSTR lpDest, rsize_t uiDestSize, LPCTSTR lpSrc )
{
	if( !lpDest || !lpSrc )
		return -1;

#ifdef UNICODE
	return wcscpy_s( lpDest, uiDestSize, lpSrc );
#else
	return strcpy_s( lpDest, uiDestSize, lpSrc );
#endif
}

errno_t CUHF::AppendStringToString( LPTSTR lpDest, rsize_t uiDestSize, LPCTSTR lpSrc )
{
	if( !lpDest || !lpSrc )
		return -1;

#ifdef UNICODE
	return wcscat_s( lpDest, uiDestSize, lpSrc );
#else
	return strcat_s( lpDest, uiDestSize, lpSrc );
#endif
}

LPCTSTR CUHF::SearchStringInString( LPCTSTR lpStr, LPCTSTR lpSubStr )
{
	if( !lpStr || !lpSubStr )
		return NULL;

#ifdef UNICODE
	return wcsstr( lpStr, lpSubStr );
#else
	return strstr( lpStr, lpSubStr );
#endif
}

LPTSTR CUHF::SearchStringInString( LPTSTR lpStr, LPCTSTR lpSubStr )
{
	if( !lpStr || !lpSubStr )
		return NULL;

#ifdef UNICODE
	return wcsstr( lpStr, lpSubStr );
#else
	return strstr( lpStr, lpSubStr );
#endif
}

int CUHF::CompareStrings( LPCTSTR lpString1, LPCTSTR lpString2 )
{
	if( !lpString1 || !lpString2 )
		return -1;

#ifdef UNICODE
	return wcscmp( lpString1, lpString2 );
#else
	return strcmp( lpString1, lpString2 );
#endif
}

errno_t CUHF::OpenFile( FILE** ppFile, LPCTSTR lpFileName, LPCTSTR lpMode )
{
	if( !ppFile || !lpFileName || !lpMode )
		return -1;

#ifdef UNICODE
	return _wfopen_s( ppFile, lpFileName, lpMode );
#else
	return fopen_s( ppFile, lpFileName, lpMode );
#endif
}

LPTSTR CUHF::GetStringLineFromFile( LPTSTR lpDest, int iSizeInWords, FILE* pFile )
{
	if( !lpDest || !pFile )
		return NULL;

#ifdef UNICODE
	return fgetws( lpDest, iSizeInWords, pFile );
#else
	return fgets( lpDest, iSizeInWords, pFile );
#endif
}

int CUHF::StringToInt( LPCTSTR lpStr )
{
#ifdef UNICODE
	return _wtoi( lpStr );
#else
	return atoi( lpStr );
#endif
}

int CUHF::VSPrintf( LPTSTR lpBuffer, size_t uiSizeInBytes, LPCTSTR lpFormat, va_list argptr )
{
	if( !lpBuffer || !lpFormat )
		return -1;

#ifdef UNICODE
	return vswprintf_s( lpBuffer, uiSizeInBytes, lpFormat, argptr );
#else
	return vsprintf_s( lpBuffer, uiSizeInBytes, lpFormat, argptr );
#endif
}

void CUHF::ConvertCharToWchar( wchar_t* strWCharText, int iWcharLen, const char* strCharText )
{
	if( !strWCharText || !strCharText )
		return;

	//konwertujemy na unicode
	for( int i = 0; i < iWcharLen; ++i ) {
		strWCharText[ i ] = strCharText[ i ];
		if( strCharText[ i ] == 0 )
			break;
	}
}

wchar_t* CUHF::ConvertCharToWchar( const char* strCharText, int iCharLen )
{
	if( !strCharText )
		return NULL;

	if( ++m_iActialStringBuffer >= UHF_MAX_BUFFER )
		m_iActialStringBuffer = 0;

	::SecureZeroMemory( m_strWCharText[ m_iActialStringBuffer ], UHF_MAX_STRING );
	//konwertujemy na unicode
	for( int i = 0; i < min( iCharLen, UHF_MAX_STRING ); ++i ) {
		m_strWCharText[ m_iActialStringBuffer ][ i ] = strCharText[ i ];
		if( strCharText[ i ] == 0 )
			break;
	}

	return m_strWCharText[ m_iActialStringBuffer ];
}

void CUHF::ConvertWcharToChar( char* strCharText, int iCharLen, const wchar_t* strWCharText )
{
	if( !strCharText || !strWCharText )
		return;
	//konwertujemy na ascii z unicode
	for( int i = 0; i < iCharLen; ++i ) {
		strCharText[ i ] = static_cast< char >( strWCharText[ i ] );
		if( strCharText[ i ] == 0 )
			break;
	}
}

char* CUHF::ConvertWcharToChar( const wchar_t* strWCharText, int iWcharLen )
{
	if( !strWCharText )
		return NULL;

	if( ++m_iActialStringBuffer >= UHF_MAX_BUFFER )
		m_iActialStringBuffer = 0;

	::SecureZeroMemory( m_strCharText[ m_iActialStringBuffer ], UHF_MAX_STRING );

	for( int i = 0; i < min( iWcharLen, UHF_MAX_STRING ); ++i ) {
		m_strCharText[ m_iActialStringBuffer ][ i ] = static_cast< char >( strWCharText[ i ] );
		if( m_strCharText[ m_iActialStringBuffer ][ i ] == 0 )
			break;
	}

	return m_strCharText[ m_iActialStringBuffer ];
}