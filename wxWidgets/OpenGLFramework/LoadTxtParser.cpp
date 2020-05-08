#include "StdAfx.h"
#include "LoadTxtParser.h"

CLoadTxtParser::CLoadTxtParser( GLboolean bLoadNow )
{
	::SecureZeroMemory( m_tc3ds, BUFFER_SIZE );
	::SecureZeroMemory( m_tcTexture, BUFFER_SIZE );
	::SecureZeroMemory( m_tcBump, BUFFER_SIZE );

	if( bLoadNow )
		ParseFile( _T("load.txt") );
}

CLoadTxtParser::~CLoadTxtParser()
{
}

GLboolean CLoadTxtParser::ParseFile( LPCTSTR lpFileName )
{
	::SecureZeroMemory( m_tc3ds, BUFFER_SIZE );
	::SecureZeroMemory( m_tcTexture, BUFFER_SIZE );
	::SecureZeroMemory( m_tcBump, BUFFER_SIZE );

	FILE *pFile = NULL;
	if( _tfopen_s( &pFile, lpFileName, _T("rt") ) != 0 )
		return GL_FALSE;

	TCHAR tcTempBuff[ BUFFER_SIZE ];
	while( _fgetts( tcTempBuff, BUFFER_SIZE - 1, pFile ) != NULL ) {
		if( m_tc3ds[ 0 ] == 0 )
			ParseSearchString( tcTempBuff, _T("3ds"), 3, m_tc3ds );
		if( m_tcTexture[ 0 ] == 0 )
			ParseSearchString( tcTempBuff, _T("texture"), 7, m_tcTexture );
		if( m_tcBump[ 0 ] == 0 )
			ParseSearchString( tcTempBuff, _T("bump"), 4, m_tcBump );
	}

	fclose( pFile );

	return GL_TRUE;
}

GLboolean CLoadTxtParser::ParseSearchString( const TCHAR *pBuffer, const TCHAR *pSearch, GLint iSearchLength, TCHAR *pReturnBuffer )
{
	::SecureZeroMemory( pReturnBuffer, BUFFER_SIZE );

	GLint iIndexReturnBuffer = 0;
	GLboolean bFoundTag = GL_FALSE;
	GLint iCounterSearch = 0;
	for( GLint i = 0; i < BUFFER_SIZE; ++i ) {
		if( ( pBuffer[ i ] == ';' || pBuffer[ i ] == 0 ) && !bFoundTag )
			break;

		if( pBuffer[ i ] == '=' ) {
			for( GLint j = 0, k = 0; j < iSearchLength; ++j, ++k ) {
				if( k > i )
					break;

				if( pBuffer[ k ] == pSearch[ j ] )
					++iCounterSearch;
				else{
					iCounterSearch = 0;
					--j;
				}
				if( iCounterSearch == iSearchLength ) {
					bFoundTag = GL_TRUE; //znalazl szukany pSearch
					break;
				}
			}
			continue; //przeskocz '='
		}

		if( bFoundTag ) {
			if( pBuffer[ i ] == ';' ) {
				pReturnBuffer[ iIndexReturnBuffer++ ] = 0;
				break;
			}
			else if( pBuffer[ i ] > 32 || pBuffer[ i ] == 0 )
				pReturnBuffer[ iIndexReturnBuffer++ ] = pBuffer[ i ];
			
			if( pBuffer[ i ] == 0 )
				break;
		}
	}

	return bFoundTag;
}