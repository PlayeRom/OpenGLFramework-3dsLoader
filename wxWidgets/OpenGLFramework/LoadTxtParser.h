#pragma once

#define BUFFER_SIZE 256

/*
klasa parusje np. taka zawartosc pliku:

; komentarz
3ds = player01.3DS
texture = player01.bmp
bump = player01bump.bmp

*/

class CLoadTxtParser
{
public:
	CLoadTxtParser( GLboolean bLoadNow = GL_FALSE );
	~CLoadTxtParser();

	GLboolean ParseFile( LPCTSTR lpFileName );

	LPCTSTR Get3dsFileName() { return m_tc3ds; }
	LPCTSTR GetTextureName() { return m_tcTexture; }
	LPCTSTR GetBumpName() { return m_tcBump; }

	GLboolean IsUse() {
		return ( m_tc3ds[ 0 ] == 0 ) ? GL_FALSE : GL_TRUE;
	}

	GLboolean IsTexture() {
		return ( ( m_tcTexture[ 0 ] == '?' && m_tcTexture[ 1 ] == 0 ) || m_tcTexture[ 0 ] == 0 ) ? GL_FALSE : GL_TRUE;
	}

	GLboolean IsBump() {
		return ( ( m_tcBump[ 0 ] == '?' && m_tcBump[ 1 ] == 0 ) || m_tcBump[ 0 ] == 0 ) ? GL_FALSE : GL_TRUE;
	}

private:
	GLboolean ParseSearchString( const TCHAR *pBuffer, const TCHAR *pSearch, GLint iSearchLength, TCHAR *pReturnBuffer );

	TCHAR m_tc3ds[ BUFFER_SIZE ];
	TCHAR m_tcTexture[ BUFFER_SIZE ];
	TCHAR m_tcBump[ BUFFER_SIZE ];
};
