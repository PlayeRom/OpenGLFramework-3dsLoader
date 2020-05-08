
#include "StdAfx.h"
#include "../OpenGLFramework/Console.h"
#include "../OpenGLFramework/GameControl.h"
#include "VertexArrays.h"
#include "../Extensions/ARBVertexBufferObject.h"
#include "../Extensions/ARBMultiTexturing.h"
#include "../Lighting/Lighting.h"
#include "TextureLoader.h"
#include "EmbossBump.h"
#include "3DSLoader.h"

C3DSLoader::C3DSLoader( LPCTSTR lp3dsFileName )
{
	m_pGameCtrl = CGameControl::GetInstance();

	Search3DSFile( _T("3dsobjects") );

	if( lp3dsFileName ) {
		if( Load3DSObject( lp3dsFileName ) )
			CreateVertexArrays();
	}
}

C3DSLoader::~C3DSLoader()
{
	DeleteAllObjects();
	Delete3DSFileName();
}

GLvoid C3DSLoader::Search3DSFile( LPCTSTR lpCurDir )
{
	Delete3DSFileName();

	::SetCurrentDirectory( lpCurDir ); 

	WIN32_FIND_DATA wfd;
	HANDLE hFile = ::FindFirstFile( _T("*.*"), &wfd ); 

	BOOL bIsFile = TRUE;
	while( bIsFile ) {
		TCHAR cPath[ MAX_3DS_NAME ];
		::GetFullPathName( wfd.cFileName, 255, cPath, NULL );

		if( !( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			&& ( CUHF::SearchStringInString( cPath, _T(".3ds") ) || CUHF::SearchStringInString( cPath, _T(".3DS") ) ) )
		{
			//cPath zawiera pe³n¹ sciezka C:\...\3dsobjecst\file.3ds
			//dlatego tez obcinamy j¹ do samej: 3dsobjects\file.3ds
			TCHAR* pdest = CUHF::SearchStringInString( cPath, _T("3dsobjects") );
			int iPos = static_cast< int >( pdest - cPath ); //pozycja od 3dsobjects\file.3ds

			S3DSFileName s3DSFileName;
			m_a3DSFileName.push_back( s3DSFileName );
			GLint iIndex = Get3DSFileNameSize() - 1;

			unsigned int i;
			for( i = 0; i < CUHF::GetStringLength( cPath ); ++i )
				m_a3DSFileName[ iIndex ].cFileName[ i ] = cPath[ iPos + i ];
			m_a3DSFileName[ iIndex ].cFileName[ i ] = 0;
		}

		bIsFile = ::FindNextFile( hFile, &wfd );
	}

    if( lpCurDir[ 0 ] > 3 )
		::SetCurrentDirectory( _T("..") );
}

TCHAR* C3DSLoader::Get3DSFileName( GLint iIndex )
{
	if( iIndex < 0 || iIndex >= Get3DSFileNameSize() )
		return NULL;

	return m_a3DSFileName[ iIndex ].cFileName;
}

GLvoid C3DSLoader::Delete3DSFileName()
{
	m_a3DSFileName.clear();
}

GLboolean C3DSLoader::ReLoad3DSObject( GLint iIndex )
{
	if( iIndex < 0 || iIndex >= Get3DSFileNameSize() )
		return GL_FALSE;

	m_pGameCtrl->GetEmbossBump()->ClearAllArrays();
	DeleteAllObjects();
	m_pGameCtrl->GetVertexArrays()->ClearAllVertexArrays();
	m_pGameCtrl->GetVertexArrays()->ClearAllVertexArraysElem();

	if( Load3DSObject( m_a3DSFileName[ iIndex ].cFileName ) ) {
		CreateVertexArrays();
		return GL_TRUE;
	}
	
	return GL_FALSE;
}

GLboolean C3DSLoader::ReLoad3DSObject( LPCTSTR lpFileName )
{
	m_pGameCtrl->GetEmbossBump()->ClearAllArrays();
	DeleteAllObjects();
	m_pGameCtrl->GetVertexArrays()->ClearAllVertexArrays();
	m_pGameCtrl->GetVertexArrays()->ClearAllVertexArraysElem();

	if( CUHF::GetStringLength( lpFileName ) > 0 && Load3DSObject( lpFileName ) )
	{
		CreateVertexArrays();
		return GL_TRUE;
	}
	
	return GL_FALSE;
}

GLboolean C3DSLoader::Load3DSObject( LPCTSTR lpFileName )
{
	FILE *hFile = NULL;
	errno_t error = CUHF::OpenFile( &hFile, lpFileName, _T("rb") );
	if( error ) {
		CONSOLE_ADDTEXT( CConsole::EError, "ERROR: C3DSLoader::Load3DSObject - %s load error", lpFileName );
		return GL_FALSE;
	}

	S3DSChunk sChunk = { 0 };					// fragment danych
	char cObjectName[ MAX_3DS_NAME ] = { 0 };	// nazwa obiektu

	// odczytujemy obiekt z pliku 3DS
	while( fread( &sChunk, sizeof( sChunk ), 1, hFile ) )
	{
		switch( sChunk.usChunkID )
		{
			case MAIN_3DS:
				// pusty chunk
				break;

			case EDIT_3DS:
				// nastepny pusty chunk
				break;

			case EDIT_OBJ_BLOCK:
			{
				// tutaj zapisany jest lancuch znakow zakonczony
				// znakiem 0, stanowiacy nazwe obiektu
				unsigned short i = 0;
				char c = 'z';
				while( c != '\0' ) {
					fread( &c, 1, 1, hFile );
					if( i < MAX_3DS_NAME )
						cObjectName[ i++ ] = c;
				}
				cObjectName[ MAX_3DS_NAME - 1 ] = '\0';

				S3DSObject s3DSobject;
				m_a3DSObjects.push_back( s3DSobject );
				AddNameObject( cObjectName );

				CONSOLE_ADDTEXT_NORMAL( "3DSObject name: %s", cObjectName );

				break;
			}
			case OBJ_TRIMESH:
				// kolejny pusty chunk, ktory nie mozemy przeskoczyc
				// w innym razie przeskoczylibysmy wszystkie chunki
				// wchodzace w jego sklad, a tego nie chcemy, bo interesuja
				// nas zapisane w nich dane
				break;

			case TRI_VERTEXLIST:
			{
				AddVerticesObject( hFile );
				break;
			}
			case TRI_MAPPINGCOORDS:
			{
				AddTexCoordsObject( hFile );
				break;
			}
			case TRI_FACELIST:
			{
				AddFacesObject( hFile );
				break;
			}
			case TRI_MATERIAL:
			{
				AddMaterialObject( hFile );
				break;
			}
			default:
				// jezeli jakis chunka nas kompletnie nie interesuje
				// przeskajujemy jego zwartosc, musimy jednak pamietac
				// o odjeciu rozmiaru naglowka chunka od przesuniecia
				fseek( hFile, sChunk.ulChunkLength - sizeof( sChunk ), SEEK_CUR );
				break;
		}
	}

	fclose( hFile );

//	if( !CGameControl::GetInstance()->IsSmoothNormals() )
//		ComputeVertexNormals(); // obliczamy wektory normalne dla wierzcholkow

	CheckObjects();

	::SecureZeroMemory( m_cActualFileName, sizeof( m_cActualFileName ) );
	for( GLuint i = 0; i < CUHF::GetStringLength( lpFileName ); ++i )
		m_cActualFileName[ i ] = lpFileName[ i ];

	return GL_TRUE;
}

GLvoid C3DSLoader::AddNameObject( const char* lpName, GLint iIndex /*= -1*/ )
{
	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() )
		iIndex = Get3DSObjectsSize() - 1;

	GLuint uiLen = static_cast<GLuint>( strlen( lpName ) );
#ifdef UNICODE
	m_a3DSObjects[ iIndex ].lpName = new TCHAR[ uiLen + 1 ];
	for( GLuint i = 0; i < uiLen; ++i )
		m_a3DSObjects[ iIndex ].lpName[ i ] = lpName[ i ];
	m_a3DSObjects[ iIndex ].lpName[ uiLen ] = 0;
#else
	m_a3DSObjects[ iIndex ].lpName = new TCHAR[ uiLen + 1 ];
	strcpy( m_a3DSObjects[ iIndex ].lpName, lpName );
#endif

	m_a3DSObjects[ iIndex ].pVertices = NULL;
	m_a3DSObjects[ iIndex ].pNormals = NULL;
	m_a3DSObjects[ iIndex ].pFaces = NULL;
	m_a3DSObjects[ iIndex ].pTexCoords = NULL;
	m_a3DSObjects[ iIndex ].usVerticesNum = 0;
	m_a3DSObjects[ iIndex ].usNormalsNum = 0;
	m_a3DSObjects[ iIndex ].usFacesNum = 0;
	m_a3DSObjects[ iIndex ].usTexCoordsNum = 0;
}

GLvoid C3DSLoader::AddVerticesObject( FILE *hFile )
{
	GLint iIndex = Get3DSObjectsSize() - 1;

	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::AddVerticesObject") );
		return;
	}

	fread( ( char* )&m_a3DSObjects[ iIndex ].usVerticesNum, 2, 1, hFile );
	if( m_a3DSObjects[ iIndex ].usVerticesNum != 0 ) {
		m_a3DSObjects[ iIndex ].pVertices = new CVector3[ m_a3DSObjects[ iIndex ].usVerticesNum ];
		for( GLuint i = 0; i < m_a3DSObjects[ iIndex ].usVerticesNum; ++i ) {
			// w tej czesci pliku 3DS zapisane sa wierzcholki
			// modelu, dla kazdego sa to 3 wartosci typu float,
			// kolejno wspolrzedne x, y i z, ich laczny rozmiar
			// to cale 12 bajtow
			fread( ( char* )&m_a3DSObjects[ iIndex ].pVertices[ i ],
					sizeof( m_a3DSObjects[ iIndex ].pVertices[ i ] ), 1, hFile );
		}
	}

	m_a3DSObjects[ iIndex ].pNormals = new CVector3[ m_a3DSObjects[ iIndex ].usVerticesNum ];
	m_a3DSObjects[ iIndex ].usNormalsNum = m_a3DSObjects[ iIndex ].usVerticesNum;

	CONSOLE_ADDTEXT_NORMAL( _T("3DSObject vertices number: %d"), m_a3DSObjects[ iIndex ].usVerticesNum );
}

GLvoid C3DSLoader::AddFacesObject( FILE *hFile )
{
	GLint iIndex = Get3DSObjectsSize() - 1;

	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::AddFacesObject") );
		return;
	}

	fread( ( char* )&m_a3DSObjects[ iIndex ].usFacesNum, 2, 1, hFile );
	if( m_a3DSObjects[ iIndex ].usFacesNum != 0 ) {
		m_a3DSObjects[ iIndex ].pFaces = new S3DSFace[ m_a3DSObjects[ iIndex ].usFacesNum ];
		// dla kazdego faca w plika zapisane sa 4 wartosci
		// typu unsigned short, pierwsze trzy to numery
		// wierzcholkow tworzacych face, ostatnia, czwarta
		// wartosc zawiera dodatkowe informacje o face
		// (pomijamy ja)
		short int temp[ 4 ] = { 0 };
		for( GLuint i = 0; i < m_a3DSObjects[ iIndex ].usFacesNum; ++i ) {
			fread( ( char* )&temp, sizeof( temp ), 1, hFile );
			memcpy( ( char* )&m_a3DSObjects[ iIndex ].pFaces[ i ], temp, sizeof( m_a3DSObjects[ iIndex ].pFaces[ i ] ) );
		}
	}
	CONSOLE_ADDTEXT_NORMAL( _T("3DSObject faces number: %d"), m_a3DSObjects[ iIndex ].usFacesNum );
}

GLvoid C3DSLoader::AddTexCoordsObject( FILE *hFile )
{
	GLint iIndex = Get3DSObjectsSize() - 1;

	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::AddTexCoordsObject") );
		return;
	}

	fread( ( char* )&m_a3DSObjects[ iIndex ].usTexCoordsNum, 2, 1, hFile );
	if( m_a3DSObjects[ iIndex ].usTexCoordsNum != 0 ) {
		m_a3DSObjects[ iIndex ].pTexCoords = new CVector2[ m_a3DSObjects[ iIndex ].usTexCoordsNum ];
		for( GLuint i = 0; i < m_a3DSObjects[ iIndex ].usTexCoordsNum; ++i )
			fread( ( char* )&m_a3DSObjects[ iIndex ].pTexCoords[ i ],
					sizeof( m_a3DSObjects[ iIndex ].pTexCoords[ i ] ), 1, hFile );
	}

	CONSOLE_ADDTEXT_NORMAL( _T("3DSObject texture coordinates number: %d"), m_a3DSObjects[ iIndex ].usTexCoordsNum );
}

GLvoid C3DSLoader::AddMaterialObject( FILE *hFile )
{
	GLint iIndex = Get3DSObjectsSize() - 1;

	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::AddTexCoordsObject") );
		return;
	}

	char cName[ MAX_3DS_NAME ];
	unsigned short i = 0;
	char c = 'z';
	while( c != '\0' ) {
		fread( &c, 1, 1, hFile );
		if( i < MAX_3DS_NAME )
			cName[ i++ ] = c;
	}
	cName[ MAX_3DS_NAME - 1 ] = '\0';

	S3DSMaterials sMaterials;
	m_a3DSObjects[ iIndex ].m_aMaterials.push_back( sMaterials );
	GLint iIndexMaterial = static_cast<GLint>( m_a3DSObjects[ iIndex ].m_aMaterials.size() - 1 );

	GLuint uiLen = static_cast<GLuint>( strlen( cName ) );
#ifdef UNICODE
	m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName = new TCHAR[ uiLen + 1 ];
	for( GLuint i = 0; i < uiLen; ++i )
		m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName[ i ] = cName[ i ];
	m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName[ uiLen ] = 0;
#else
	m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName = new TCHAR[ uiLen + 1 ];
	strcpy( m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName, cName );
#endif

	CONSOLE_ADDTEXT_NORMAL( _T("Material name: %s"), m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].lpName );

	//odczytujemy identifikatory trojkatow ktorym przypisana jest dana tekstura
	fread( ( char* )&m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].usFacesNum, 2, 1, hFile );
	unsigned short usFacesNum = m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].usFacesNum;
	if( usFacesNum != 0 ) {
		m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].pFacesID = new unsigned short[ usFacesNum ];
		for( GLuint i = 0; i < usFacesNum; ++i ) {
			fread( ( char* )&m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].pFacesID[ i ],
					sizeof( m_a3DSObjects[ iIndex ].m_aMaterials[ iIndexMaterial ].pFacesID[ i ] ),
					1, hFile );
		}
	}
}

GLvoid C3DSLoader::DeleteOneObject( GLint iIndex )
{
	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::DeleteOneObject") );
		return;
	}

	delete [] m_a3DSObjects[ iIndex ].lpName;
	delete [] m_a3DSObjects[ iIndex ].pVertices;
	delete [] m_a3DSObjects[ iIndex ].pNormals;
	delete [] m_a3DSObjects[ iIndex ].pFaces;
	delete [] m_a3DSObjects[ iIndex ].pTexCoords;

	for( GLuint i = 0; i < m_a3DSObjects[ iIndex ].m_aMaterials.size(); ++i ) {
		delete [] m_a3DSObjects[ iIndex ].m_aMaterials[ i ].lpName;
		delete [] m_a3DSObjects[ iIndex ].m_aMaterials[ i ].pFacesID;
	}
	m_a3DSObjects[ iIndex ].m_aMaterials.clear();

	m_a3DSObjects.erase( m_a3DSObjects.begin() + iIndex );
}

GLvoid C3DSLoader::DeleteAllObjects()
{
	for( GLint i = Get3DSObjectsSize() - 1; i >= 0; --i )
		DeleteOneObject( i );

	m_a3DSObjects.clear();
}

/*
 * Obliczanie normalnych, tak jak w 3ds max - smooth normals
 */
GLvoid C3DSLoader::ComputeVertexNormals()
{
	// Nigdy nie wywolywane. Metoda potepiona, wprowadzajaca zamieszanie w obliczaniu normalnych.
	// Obecnie zamiast niej wywolywane sa: C3DSLoader::CalcNormalsSmooth oraz C3DSLoader::CalcNormalsFlat
	// odpowiednio dla smooth and flat normals.

	CVector3 cVecNormal;
	for( GLint obj = 0; obj < Get3DSObjectsSize(); ++obj ) {
		if( m_a3DSObjects[ obj ].lpName == NULL
			|| m_a3DSObjects[ obj ].pVertices == NULL
			|| m_a3DSObjects[ obj ].pNormals == NULL
			|| m_a3DSObjects[ obj ].pFaces == NULL
//			|| m_a3DSObjects[ obj ].pTexCoords == NULL
			)
			continue;

		for( GLuint i = 0; i < m_a3DSObjects[ obj ].usVerticesNum; ++i ) {
			cVecNormal.x = cVecNormal.y = cVecNormal.z = 0.0f;
			for( GLuint j = 0; j < m_a3DSObjects[ obj ].usFacesNum; ++j ) {
				if( m_a3DSObjects[ obj ].pFaces[ j ].v1 == i ||
					m_a3DSObjects[ obj ].pFaces[ j ].v2 == i ||
					m_a3DSObjects[ obj ].pFaces[ j ].v3 == i )
				{
					cVecNormal += m_pGameCtrl->GetVertexArrays()->CreateFlatNormal( m_a3DSObjects[ obj ].pVertices[ m_a3DSObjects[ obj ].pFaces[ j ].v1 ],
																					m_a3DSObjects[ obj ].pVertices[ m_a3DSObjects[ obj ].pFaces[ j ].v2 ],
																					m_a3DSObjects[ obj ].pVertices[ m_a3DSObjects[ obj ].pFaces[ j ].v3 ],
																					GL_FALSE );
				}
			}

			cVecNormal.Normalize();
			m_a3DSObjects[ obj ].pNormals[ i ] = cVecNormal;
		}
	}
}

GLvoid C3DSLoader::CheckObjects()
{
	GLboolean bRemove;
	for( GLint i = Get3DSObjectsSize() - 1; i >= 0; --i ) {
		bRemove = GL_FALSE;
		if( m_a3DSObjects[ i ].lpName == NULL) {
			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - 3DSobject without name") );
			bRemove = GL_TRUE;
		}
		if( m_a3DSObjects[ i ].pVertices == NULL) {
			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - 3DSobject without vertices") );
			bRemove = GL_TRUE;
		}
		if( m_a3DSObjects[ i ].pFaces == NULL) {
			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - 3DSobject without faces") );
			bRemove = GL_TRUE;
		}
		if( m_a3DSObjects[ i ].pNormals == NULL) {
			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - 3DSobject without normals") );
			bRemove = GL_TRUE;
		}
		if( m_a3DSObjects[ i ].pTexCoords == NULL) {
//			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - 3DSobject without texture coordinates") );
//			bRemove = GL_TRUE;
//			m_a3DSObjects[ i ].pTexCoords = new CVector2[ m_a3DSObjects[ i ].usVerticesNum ];
//			m_a3DSObjects[ i ].usTexCoordsNum = m_a3DSObjects[ i ].usVerticesNum;
//			memset( m_a3DSObjects[ i ].pTexCoords, 0, sizeof( m_a3DSObjects[ i ].pTexCoords ) );
		}
		if( m_a3DSObjects[ i ].usVerticesNum != m_a3DSObjects[ i ].usNormalsNum ) {
			CONSOLE_ADDTEXT( CConsole::EError, _T("ERROR: C3DSLoader::CheckObjects - number vertices different number normals") );
			bRemove = GL_TRUE;
		}

		if( bRemove )
			DeleteOneObject( i );
	}
}

GLvoid C3DSLoader::CreateVertexArrays()
{
	CVector2 cTexCoords[ 3 ];

	for( GLint i = 0; i < Get3DSObjectsSize(); ++i ) {
		for( GLint j = 0; j < m_a3DSObjects[ i ].usFacesNum; ++j ) {
			GLint v1 = m_a3DSObjects[ i ].pFaces[ j ].v1;
			GLint v2 = m_a3DSObjects[ i ].pFaces[ j ].v2;
			GLint v3 = m_a3DSObjects[ i ].pFaces[ j ].v3;

			if( m_a3DSObjects[ i ].pTexCoords ) {
				cTexCoords[ 0 ] = m_a3DSObjects[ i ].pTexCoords[ v1 ];
				cTexCoords[ 1 ] = m_a3DSObjects[ i ].pTexCoords[ v2 ];
				cTexCoords[ 2 ] = m_a3DSObjects[ i ].pTexCoords[ v3 ];
			}
			else
				::SecureZeroMemory( &cTexCoords, sizeof( CVector2 ) * 3 );

			m_pGameCtrl->GetVertexArrays()->AddVertex( m_a3DSObjects[ i ].pVertices[ v1 ] );
			m_pGameCtrl->GetVertexArrays()->AddTexCoord( cTexCoords[ 0 ] );
			m_pGameCtrl->GetVertexArrays()->AddColorSingle( 1.0f, 1.0f, 1.0f );

			m_pGameCtrl->GetVertexArrays()->AddVertex( m_a3DSObjects[ i ].pVertices[ v2 ] );
			m_pGameCtrl->GetVertexArrays()->AddTexCoord( cTexCoords[ 1 ] );
			m_pGameCtrl->GetVertexArrays()->AddColorSingle( 1.0f, 1.0f, 1.0f );

			m_pGameCtrl->GetVertexArrays()->AddVertex( m_a3DSObjects[ i ].pVertices[ v3 ] );
			m_pGameCtrl->GetVertexArrays()->AddTexCoord( cTexCoords[ 2 ] );
			m_pGameCtrl->GetVertexArrays()->AddColorSingle( 1.0f, 1.0f, 1.0f );
		}
	}

	if( m_pGameCtrl->IsSmoothNormals() )
		CalcNormalsSmooth();
	else
		CalcNormalsFlat();

	//z indeksowaniem
	GLint iStart = 0;
	for( GLint i = 0; i < Get3DSObjectsSize(); ++i ) {
		m_pGameCtrl->GetVertexArrays()->CreateIndices( iStart, GetVericesNum( i ) );
		iStart += GetVericesNum( i );
	}
	//wyswietlenie rzeczywistej ilosci wierzcholow
	CONSOLE_ADDTEXT_NORMAL( _T("Real unique vertices number = %d"), m_pGameCtrl->GetVertexArrays()->GetVerticesElemSize() );

	m_pGameCtrl->GetVertexArrays()->InitialArrays();
}

/*
 * Obliczanie normalnych dla wierzcholków, normalne moga sie powtarzac, ale kazda
 * normalna dla danego wierzolka jest identyczna, tj. nie ma kilku roznych normalnych
 * wychodzacych z tego samego wierzcholka, dzieki temu obiekt jest ladnie cieniowany
 */
GLvoid C3DSLoader::CalcNormalsSmooth()
{
	m_pGameCtrl->GetVertexArrays()->CalcAndAddNormalsSmooth( GL_TRIANGLES, 0, m_pGameCtrl->GetVertexArrays()->GetVerticesSize() );

	CalcNormalsAux();
}

GLvoid C3DSLoader::CalcNormalsFlat()
{
	m_pGameCtrl->GetVertexArrays()->CalcAndAddNormalsFlat( GL_TRIANGLES, 0, m_pGameCtrl->GetVertexArrays()->GetVerticesSize() );
	
	CalcNormalsAux();
}

GLvoid C3DSLoader::CalcNormalsAux()
{
	GLint iNormals = m_pGameCtrl->GetVertexArrays()->GetNormalsSize();

	//przepisanie tablicy normalnych z VertexArray do m_a3DSObjects[ x ].pNormals
	for( GLint i = 0; i < Get3DSObjectsSize(); ++i )
	{
		delete [] m_a3DSObjects[ i ].pNormals;
		m_a3DSObjects[ i ].pNormals = new CVector3[ iNormals ];
		m_a3DSObjects[ i ].usNormalsNum = static_cast< unsigned short >( iNormals );

		for( GLint j = 0; j < m_a3DSObjects[ i ].usNormalsNum; ++j )
		{
			//if m_a3DSObjects[ i ].usNormalsNum != normals z vertex arrays -> crash
			m_a3DSObjects[ i ].pNormals[ j ].x = m_pGameCtrl->GetVertexArrays()->GetNormal( j ).x;
			m_a3DSObjects[ i ].pNormals[ j ].y = m_pGameCtrl->GetVertexArrays()->GetNormal( j ).y;
			m_a3DSObjects[ i ].pNormals[ j ].z = m_pGameCtrl->GetVertexArrays()->GetNormal( j ).z;
		}
	}
}

GLint C3DSLoader::GetVericesNum( GLint iIndex ) const
{
	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() )
		return 0;

	return static_cast<GLint>( m_a3DSObjects[ iIndex ].usFacesNum * 3 );
}

GLint C3DSLoader::GetTexCoordsNum( GLint iIndex ) const
{
	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() )
		return 0;

	return static_cast<GLint>( m_a3DSObjects[ iIndex ].usTexCoordsNum );
}

S3DSObject C3DSLoader::Get3DSObject( GLint iIndex /*= 0*/ ) const
{
	if( iIndex < 0 || iIndex >= Get3DSObjectsSize() ) {
		S3DSObject s3dsObj = { 0 };
		return s3dsObj;
	}
	return m_a3DSObjects[ iIndex ];
}

TCHAR* C3DSLoader::GetActualFileName()
{
	// pozbywamy sie nazy katalogu z m_cActualFileName i zwracamy sama nazwe pliku z rozszerzeniem.
	GLint i = MAX_3DS_NAME - 1;
	for( ; i >= 0; --i )
	{
		if( m_cActualFileName[ i ] == '\\' || m_cActualFileName[ i ] == '/' )
			break;
	}

	if( i > 0 ) // jest katalog
	{
		GLint k = 0;
		for( GLint j = i + 1; j < MAX_3DS_NAME; ++j, ++k )
			m_cActualFileNameWithoutPath[ k ] = m_cActualFileName[ j ];
		
		m_cActualFileNameWithoutPath[ k + 1 ] = 0;

		return m_cActualFileNameWithoutPath;
	}

	return m_cActualFileName;
}

TCHAR* C3DSLoader::GetActualFileNameWithoutExt()
{
	::wsprintf( m_cFileNameWithoutExt, _T("%s"), GetActualFileName() );

	GLint iSize = static_cast< GLint >( CUHF::GetStringLength( m_cFileNameWithoutExt ) );
	for( GLint i = iSize - 1; i >= 0; --i ) {
		if( m_cFileNameWithoutExt[ i ] == '.' ) {
			m_cFileNameWithoutExt[ i ] = 0;
			break;
		}
	}
	return m_cFileNameWithoutExt;
}

GLvoid C3DSLoader::DrawEmbossBumpObject( GLuint uiLight, GLint iBaseTexture, GLint iEmbossBumpTexture )
{
	m_pGameCtrl->GetVertexArrays()->DisableClientState();

	m_pGameCtrl->GetEmbossBump()->VMatMult( m_pGameCtrl->GetLighting()->GetPosition( uiLight ) );
	m_pGameCtrl->GetEmbossBump()->SetUpBumps( 0, 0, GetVericesNum( 0 ) );
	m_pGameCtrl->GetVertexArrays()->InitialArraysWithEmbossBump();
	m_pGameCtrl->GetARBMultiTexturing()->BindMultiTextures( iEmbossBumpTexture, 2, GL_TRUE ); //8 = indeks pierwszej tekstury, 2 = ilosc tekstur
	glDisable( GL_BLEND );
//	glDisable( GL_LIGHTING );

	GLint iStart = 0;
	for( GLint i = 0; i < Get3DSObjectsSize(); ++i ) {
		glDrawArrays( GL_TRIANGLES, iStart, GetVericesNum( i ) );
		iStart += GetVericesNum( i );
	}

	m_pGameCtrl->GetARBMultiTexturing()->DisableMultiTeksturing(); //uaktywniamy tylko pierwsza jednostke teksturujaca

	glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );
	glEnable( GL_BLEND );
//	glEnable( GL_LIGHTING );
	m_pGameCtrl->GetVertexArrays()->DisableClientState();
	m_pGameCtrl->GetVertexArrays()->InitialArrays( GL_TRUE );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	m_pGameCtrl->GetTexLoader()->SetTexture( iBaseTexture ); //base_bump

	iStart = 0;
	for( GLint i = 0; i < Get3DSObjectsSize(); ++i ) {
		glDrawArrays( GL_TRIANGLES, iStart, GetVericesNum( i ) );
		iStart += GetVericesNum( i );
	}

	m_pGameCtrl->GetVertexArrays()->DisableClientState();
}

GLvoid C3DSLoader::DrawEmbossBumpObjectElem( GLuint uiLight, GLint iBaseTexture, GLint iEmbossBumpTexture )
{
	m_pGameCtrl->GetVertexArrays()->DisableClientState();

	m_pGameCtrl->GetEmbossBump()->VMatMult( m_pGameCtrl->GetLighting()->GetPosition( uiLight ) );
	m_pGameCtrl->GetEmbossBump()->SetUpBumpsElem( 0, GetVericesNum( 0 ) );
	m_pGameCtrl->GetVertexArrays()->InitialArraysWithEmbossBumpElem();
	m_pGameCtrl->GetARBMultiTexturing()->BindMultiTextures( iEmbossBumpTexture, 2, GL_TRUE ); //8 = indeks pierwszej tekstury, 2 = ilosc tekstur
	glDisable( GL_BLEND );
//	glDisable( GL_LIGHTING );

	m_pGameCtrl->GetVertexArrays()->LockArrays( 0, m_pGameCtrl->GetVertexArrays()->GetVerticesElemSize() );
	for( GLint i = 0; i < m_pGameCtrl->GetVertexArrays()->GetIndicesSize(); ++i )
		glDrawElements( GL_TRIANGLES, m_pGameCtrl->GetVertexArrays()->GetSubIndicesSize( i ), GL_UNSIGNED_SHORT, m_pGameCtrl->GetVertexArrays()->GetSubIndices( i ) );
	m_pGameCtrl->GetVertexArrays()->UnlockArrays();

	m_pGameCtrl->GetARBMultiTexturing()->DisableMultiTeksturing(); //uaktywniamy tylko pierwsza jednostke teksturujaca

	glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );
	glEnable( GL_BLEND );
//	glEnable( GL_LIGHTING );
	m_pGameCtrl->GetVertexArrays()->DisableClientState();
	m_pGameCtrl->GetVertexArrays()->InitialArraysElem();
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	m_pGameCtrl->GetTexLoader()->SetTexture( iBaseTexture ); //base_bump

	m_pGameCtrl->GetVertexArrays()->LockArrays( 0, m_pGameCtrl->GetVertexArrays()->GetVerticesElemSize() );
	for( GLint i = 0; i < m_pGameCtrl->GetVertexArrays()->GetIndicesSize(); ++i )
		glDrawElements( GL_TRIANGLES, m_pGameCtrl->GetVertexArrays()->GetSubIndicesSize( i ), GL_UNSIGNED_SHORT, m_pGameCtrl->GetVertexArrays()->GetSubIndices( i ) );
	m_pGameCtrl->GetVertexArrays()->UnlockArrays();

	m_pGameCtrl->GetVertexArrays()->DisableClientState();
	glDisable( GL_BLEND );
}