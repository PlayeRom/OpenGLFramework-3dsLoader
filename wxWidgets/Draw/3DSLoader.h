#pragma once

class CGameControl;
class CVector2;
class CVector3;

#pragma pack( push )
#pragma pack( 1 )

struct S3DSFace
{
	// indeksy wierzcholkow tworzacych face
	unsigned short v1, v2, v3;
};

struct S3DSMaterials
{
	TCHAR*			lpName;
	unsigned short*	pFacesID;
	unsigned short	usFacesNum;
};

struct S3DSObject
{
	TCHAR*		lpName;
	CVector3*	pVertices;
	CVector3*	pNormals;
	S3DSFace*	pFaces;
	CVector2*	pTexCoords;
	std::vector< S3DSMaterials > m_aMaterials;

	unsigned short usVerticesNum;
	unsigned short usNormalsNum;
	unsigned short usFacesNum;
	unsigned short usTexCoordsNum;
};

struct S3DSFileName
{
	TCHAR cFileName[ MAX_3DS_NAME ];
};

#pragma pack( pop )

class C3DSLoader
{
	// identyfikatory podstawowych chunkow
	enum E3DSChunkID
	{
		MAIN_3DS			= 0x4D4D,
		EDIT_3DS			= 0x3D3D,
		EDIT_OBJ_BLOCK		= 0x4000,	// obiekt
		OBJ_TRIMESH			= 0x4100,
		TRI_VERTEXLIST		= 0x4110,	// wierzcholki
		TRI_MAPPINGCOORDS	= 0x4140,	// koordynaty tekstur
		TRI_FACELIST		= 0x4120,	// poligony
		TRI_MATERIAL		= 0x4130,	// tekstury
		OBJ_LIGHT			= 0x4600,
		OBJ_CAMERA			= 0x4700
	};

#pragma pack( push )
#pragma pack( 1 )

	struct S3DSChunk
	{
		// 2 bajtowy identyfikator fragmentu pliku 3DS
		unsigned short int usChunkID;

		// dlugosc fragmentu pliku, zawiera rozmiar chunka jezeli pole wchodzace
		// w sklad chunka posiada rozmiar 14 bajtow to rozmiar calego chunka w tym
		// polu bedzie wynosil 20 (14 + 6 bajtow chunka)
		unsigned long ulChunkLength;
	};

#pragma pack( pop )

public:
	C3DSLoader( LPCTSTR lp3dsFileName = NULL );
	~C3DSLoader();

	GLvoid Search3DSFile( LPCTSTR lpCurDir );
	GLvoid Delete3DSFileName();
	TCHAR* Get3DSFileName( GLint iIndex );

	GLboolean ReLoad3DSObject( GLint iIndex );
	GLboolean ReLoad3DSObject( LPCTSTR lpFileName );

	GLvoid CreateVertexArrays();

	inline GLint Get3DSObjectsSize() const	{ return static_cast< GLint >( m_a3DSObjects.size() ); }
	inline GLint Get3DSFileNameSize() const { return static_cast< GLint >( m_a3DSFileName.size() ); }
	GLint GetVericesNum( GLint iIndex ) const;
	GLint GetTexCoordsNum( GLint iIndex ) const;

	S3DSObject Get3DSObject( GLint iIndex = 0 ) const;
	TCHAR* GetActualFileName();
	TCHAR* GetActualFileNameWithoutExt();

	GLvoid DrawEmbossBumpObject( GLuint uiLight, GLint iBaseTexture, GLint iEmbossBumpTexture );
	GLvoid DrawEmbossBumpObjectElem( GLuint uiLight, GLint iBaseTexture, GLint iEmbossBumpTexture );

private:
	CGameControl *m_pGameCtrl;

	GLboolean Load3DSObject( LPCTSTR lpFileName );

	std::vector< S3DSObject > m_a3DSObjects;
	GLvoid AddNameObject( const char* lpName, GLint iIndex = -1 );
	GLvoid AddVerticesObject( FILE *hFile );
	GLvoid AddFacesObject( FILE *hFile );
	GLvoid AddTexCoordsObject( FILE *hFile );
	GLvoid AddMaterialObject( FILE *hFile );
	GLvoid DeleteOneObject( GLint iIndex );
	GLvoid DeleteAllObjects();

__declspec( deprecated )
	GLvoid ComputeVertexNormals();

	GLvoid CalcNormalsSmooth();
	GLvoid CalcNormalsFlat();
	GLvoid CalcNormalsAux();

	GLvoid CheckObjects();

	std::vector< S3DSFileName > m_a3DSFileName;

	TCHAR m_cActualFileName[ MAX_3DS_NAME ];
	TCHAR m_cActualFileNameWithoutPath[ MAX_3DS_NAME ];
	TCHAR m_cFileNameWithoutExt[ MAX_3DS_NAME ];

	GLvoid FindIdenticVertices( LPCTSTR lpFileName );
};