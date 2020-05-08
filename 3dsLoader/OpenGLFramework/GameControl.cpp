// GameControl.cpp: implementation of the CGameControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Console.h"
#include "../Fonts/BitmapFont.h"
#include "../Fonts/OutlineFont.h"
#include "../Fonts/TextureFont.h"
#include "../Fonts/SDLFont.h"
#include "../Fps/Fps.h"
#include "../Sound/MasterVolume.h"
#include "WindowData.h"
#include "Cursor.h"
#include "../Extensions/ARBMultiTexturing.h"
#include "../Extensions/WGLEXTSwapControl.h"
#include "../Extensions/ARBVertexBufferObject.h"
#include "../Draw/VertexArrays.h"
#include "../Draw/3DObjManager.h"
#include "../Draw/EmbossBump.h"
#include "../Network/NetworkManager.h"
#include "../Draw/3DSLoader.h"
#include "GameControl.h"

#define MOVE_OBJ_SEED		0.1f

#if _USE_SOUND_OPENAL_
	#pragma comment( lib, "alut.lib" )
	#pragma comment( lib, "OpenAL32.lib" )
	#if _USE_OGG_
		#pragma comment( lib, "ogg.lib" )
		#pragma comment( lib, "vorbisfile.lib" )
	#endif // _USE_OGG_
#else // use FMOD
	#pragma comment( lib, "fmodvc.lib" )
#endif // !_USE_SOUND_OPENAL_

GLvoid MsgExit( GLvoid );

CGameControl* CGameControl::Construct()
{
	return CSingletonBase::Construct( new CGameControl );
}

CGameControl::CGameControl()
{
	m_pWin = CWindowData::GetInstance();

	m_bUseSmoothNormals = GL_TRUE;

	m_fRotX = m_fRotY = m_fRotZ = 0.0f;
	m_fPosZ = -200.0f;
	m_fPosX = m_fPosY = 0.0f;
	m_iCurX = 1;
	m_iCurY = 0;
	m_bMouseHoldL = GL_FALSE;
	m_bMouseHoldR = GL_FALSE;
	m_bShowNormals = GL_FALSE;
	m_fNormalLength = 1.0f;
	m_ePolygonMode = GL_FILL;
	m_bLighting = GL_TRUE;
	m_bTexturing = GL_TRUE;
	m_bSmooth = GL_TRUE;
	m_bBlending = GL_FALSE;
	m_iShowAxis = 1;
	m_bToggleArrowsControl = GL_FALSE;
	m_bCullFace = GL_TRUE;
	m_bEmbossBump = GL_TRUE;
	m_bDrawFrom3DSLoader = GL_TRUE;
	m_bDrawWithIndexing = GL_TRUE;

	m_cPosObj = 0.0f;
	m_fMoveObjSeed = MOVE_OBJ_SEED;

	m_cRot90 = 0.0f;
	m_cScale = 1.0f;
	m_cLenObj = 0.0f;

	m_iTex = -1;
	m_iTexBump = -1;
	m_i3DObjIndex = -1;

	::SecureZeroMemory( m_bKeys, 256 );
	
	//przy restarcie ponizszych zmieninnych nie mozna zmieniac
	m_bIs2D = GL_FALSE;
	m_pQuadric = NULL;
	
	m_aIndicesQuadAllScreen = new GLint[ 8 ];

	m_bToggleArrowsLightPos = GL_FALSE;
}

CGameControl::~CGameControl()
{
	DeleteObjects();
}

GLboolean CGameControl::CreateObjects()
{
	SetIndicesQuadAllScreen();

	m_pMatrixOp = new CMatrixOperations();
	if( !m_pMatrixOp ) return GL_FALSE;

	m_pConsole = new CConsole();
	if( !m_pConsole ) return GL_FALSE;

	m_pFps = new CFps();
	if( !m_pFps ) return GL_FALSE;

	m_pSpeedControl = new CSpeedControl();
	if( !m_pSpeedControl ) return GL_FALSE;

	m_pBitmapFont = NULL;
	
	m_pOutlineFont = NULL;

	m_pSDLFont = new CSDLFont( "fonts/FreeSans.ttf", 12 );
	if( !m_pSDLFont ) return GL_FALSE;

	m_pARBMultiTexturing = CARBMultiTexturing::Construct();
	if( !m_pARBMultiTexturing ) return GL_FALSE;

	m_pWglExtSwapCtrl = CWGLEXTSwapControl::Construct();
	if( !m_pWglExtSwapCtrl ) return GL_FALSE;

	m_pTextureLoader = new CTextureLoader();
	if( !m_pTextureLoader ) return GL_FALSE;

	m_pTextureFont = new CTextureFont();
	if( !m_pTextureFont ) return GL_FALSE;

#if _USE_SOUND_OPENAL_
	m_pOpenALManager = COpenALManager::Construct();
	if( !m_pOpenALManager ) return GL_FALSE;
#else
	m_pSoundFMOD = new CSoundFMOD();
	if( !m_pSoundFMOD ) return GL_FALSE;
#endif

	m_pMasterVolume = CMasterVolume::Construct();
	if( !m_pMasterVolume ) return GL_FALSE;

	m_pLighting = new CLighting();
	if( !m_pLighting ) return GL_FALSE;

	m_pCursor = new CCursor();
	if( !m_pCursor ) return GL_FALSE;

	m_pEmbossBump = new CEmbossBump();
	if( !m_pEmbossBump ) return GL_FALSE;

	m_pVertexArrays = CVertexArrays::Construct();
	if( !m_pVertexArrays ) return GL_FALSE;

	m_pStencilShadow = new CStencilShadow();
	if( !m_pStencilShadow ) return GL_FALSE;

	m_p3DObjManager = new C3DObjManager();
	if( !m_p3DObjManager ) return GL_FALSE;

	m_pParticles = new CParticles();
	if( !m_pParticles ) return GL_FALSE;

	m_pBillboard = new CBillboard();
	if( !m_pBillboard ) return GL_FALSE;

	m_pNetworkManager = new CNetworkManager();
	if( !m_pNetworkManager ) return GL_FALSE;
	
	m_pRMessageBox = new CRMessageBox();
	if( !m_pRMessageBox ) return GL_FALSE;

	m_p3DSLoader = new C3DSLoader();
	if( !m_p3DSLoader ) return GL_FALSE;

	return GL_TRUE;
}

GLvoid CGameControl::DeleteObjects()
{
	if( m_pQuadric ) {
		gluDeleteQuadric( m_pQuadric );
		m_pQuadric = NULL;
	}

	delete [] m_aIndicesQuadAllScreen;
	delete m_pSpeedControl;
	delete m_pTextureLoader;
	delete m_pConsole;
	delete m_pFps;
#ifndef _USE_SOUND_OPENAL_
	delete m_pSoundFMOD;
#endif
	delete m_pMasterVolume;
	delete m_pSDLFont;
	delete m_pLighting;
	delete m_pTextureFont;
	delete m_pCursor;
	delete m_pVertexArrays;
	delete m_pMatrixOp;
	delete m_p3DObjManager;
	delete m_pEmbossBump;
	delete m_pParticles;
	delete m_pBillboard;
	delete m_pStencilShadow;
	delete m_pNetworkManager;
	delete m_pRMessageBox;
	delete m_p3DSLoader;

	//tylko dla 3ds loadera
	m_aVerticesForScale.clear();
	m_a3DTransf.clear();
}

GLboolean CGameControl::Initialization()
{
	if( !CreateObjects() ) {
		__LOG( _T("ERROR: CGameControl::CreateObjects - create objects failed") );
		return GL_FALSE;
	}

	SetLights();

	CreateAllAnimCtrl();
	
	if( !LoadAllTextures() ) {
		__LOG( _T("ERROR: CGameControl::Initialization() - LoadAllTextures() failed") );
		return GL_FALSE;
	}

	if( !CreateAllParticles() ) {
		__LOG( _T("ERROR: CGameControl::Initialization() - CreateAllParticles() failed") );
		return GL_FALSE;
	}

	if( !LoadAll3DObjFiles() ) {
		__LOG( _T("ERROR: CGameControl::Initialization() - LoadAll3DObjFiles() failed") );
		return GL_FALSE;
	}

	if( !LoadAllSounds() ) {
		__LOG( _T("ERROR: CGameControl::Initialization() - LoadAllSounds() failed") );
		return GL_FALSE;
	}

	if( !CMultiLanguage::GetInstance()->LoadLanguage( GetWinData()->SettingFile().iIndexLanguage ) ) {
		__LOG( _T("ERROR: CGameControl::Initialization() - LoadLanguage() failed") );
		return GL_FALSE;
	}
	
	SetVSync();
	CreateVertices();
	
	//obliczamy dlugosc obiektu o indeksie 0
	ComputeLengthObject( 0, m_p3DSLoader->GetVericesNum( 0 ) );

	CreateQuadric();

	CreateVerticesForScale();

	return GL_TRUE;
}

GLvoid CGameControl::CreateVertices()
{
	//Przyklad wykorzystania VertexArray

	//sami podajemy normalna
//	m_pVertexArrays->AddFullVertex(-1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
//	m_pVertexArrays->AddFullVertex( 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f );
//	m_pVertexArrays->AddFullVertex( 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f );
//	m_pVertexArrays->AddFullVertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f );
//	m_pVertexArrays->InitialArrays();
/*
	m_p3DSLoader->Load3DSObject( std::string( "penta.3ds" ) );//_T("penta.3ds") );
	m_p3DSLoader->CreateVertexArrays();
	m_pVertexArrays->InitialArrays();
*/
/*
	//normalna zostanie wyliczona
	m_pVertexArrays->AddFullVertexCalcNormal(-1.0f,-1.0f, 1.0f, 0.0f, 0.0f );
	m_pVertexArrays->AddFullVertexCalcNormal( 1.0f,-1.0f, 1.0f, 1.0f, 0.0f );
	m_pVertexArrays->AddFullVertexCalcNormal( 1.0f, 1.0f, 1.0f, 1.0f, 1.0f );
	m_pVertexArrays->AddFullVertexCalcNormal(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f );
	m_pVertexArrays->CalcAndAddNormalsFlat( GL_QUADS, 0, 4 ); //wyliczamy normalna
	m_pVertexArrays->InitialArrays();
*/
//	m_pVertexArrays->InitialArraysWithMultiTex( 2 );
/*
	m_pARBVertexBufferObject->CreateVertexBuffer( m_pVertexArrays->GetVerticesArray(), m_pVertexArrays->GetVertexSize() * 3 * sizeof( GLfloat ) );
	m_pARBVertexBufferObject->CreateNormalBuffer( m_pVertexArrays->GetNormalArray(), m_pVertexArrays->GetNormalsSize() * 3 * sizeof( GLfloat ) );
	m_pARBVertexBufferObject->CreateColorBuffer( m_pVertexArrays->GetColorsArray(), m_pVertexArrays->GetColorsSize() * 3 * sizeof( GLfloat ) );
	m_pARBVertexBufferObject->CreateTexCoordBuffer( m_pVertexArrays->GetTexCoordArray(), m_pVertexArrays->GetTexCoordsSize() * 2 * sizeof( GLfloat ) );
*/
//	GetVertexArrays()->GetARBVBO()->SetUse( GL_FALSE );
}

GLvoid CGameControl::SetVSync()
{
	if( GetWinData()->SettingFile().bVSync )
		GetWGLEXTSwapControl()->EnableVSync();
	else
		GetWGLEXTSwapControl()->DisableVSync();
}

GLvoid CGameControl::CreateQuadric()
{
	if( m_pQuadric ) {
		gluDeleteQuadric( m_pQuadric );
		m_pQuadric = NULL;
	}
	//*************** dla kwadrykow *********************
    m_pQuadric = gluNewQuadric(); 
	//
	//gluQuadricNormals( m_pQuadric, GLU_NONE ); //bez normalnych
	//gluQuadricNormals( m_pQuadric, GLU_FLAT ); //naormalne na p³aszczyznach
    gluQuadricNormals( m_pQuadric, GLU_SMOOTH ); //normalne na wierzcholkach
	//
	//gluQuadricDrawStyle( m_pQuadric, GLU_LINE );
	//gluQuadricDrawStyle( m_pQuadric, GLU_POINT );
	//gluQuadricDrawStyle( m_pQuadric, GLU_SILHOUETTE );
	gluQuadricDrawStyle( m_pQuadric, GLU_FILL ); //domyslnie
	//
	//gluQuadricOrientation( m_pQuadric, GLU_INSIDE ); //normalne do wewnatrz kwadryki, np. gdy kamera wernatrz kwadryki
	gluQuadricOrientation( m_pQuadric, GLU_OUTSIDE ); //domyslnie - normalna na zewnatrz kwadryki
    //
	gluQuadricTexture( m_pQuadric, GL_FALSE );
	//***************************************************
}

GLvoid CGameControl::SetLights()
{
	m_cPosLight0.x = 2.0f;
	m_cPosLight0.y = 1.0f;
	m_cPosLight0.z = -5.0f;
	m_cPosLight0.w = 1.0f;
	//******************** oswietlenie **************************
	GetLighting()->SetAmbient( GL_LIGHT0 );						//ustaw oswietlenie otoczenia
	GetLighting()->SetDiffuse( GL_LIGHT0, 0.7f, 0.7f, 0.7f );						//ustaw oswietlenie rozpraszajace
	GetLighting()->SetSpecular( GL_LIGHT0 );						//Ustaw swiatlo odb³ysków
	GetLighting()->SetPosition( GL_LIGHT0, m_cPosLight0 );	//ustaw pozycje swiatla
	GetLighting()->SetMatSpecular();
	GetLighting()->SetMatShinness();
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHTING );
	//***********************************************************
}

GLvoid CGameControl::CreateAllAnimCtrl()
{
	//TODO: Tutaj wywo³uj metody GetSpeedCtrl()->CreateAnimationControl();
}

GLboolean CGameControl::LoadAllTextures()
{
	if( !GetTexLoader() )
		return GL_FALSE;

	//Tutaj nalezy wpisywac kolejne tekstury jake chcemy wykorzystac, wedle ponizszego wzoru
	//TODO: tutaj wczytuj kolejne tekstury

	GetTexLoader()->LoadTexMipmaps( _T("textures/test.bmp") );		//index 0
	GetTexLoader()->LoadTexLowQuality( _T("textures/console.bmp") );	//index 1

	return GL_TRUE;
}

GLboolean CGameControl::CreateAllParticles()
{
	return GL_TRUE;
}

GLboolean CGameControl::LoadAll3DObjFiles()
{
	if( !Get3DObjManager() )
		return GL_FALSE;

	//TODO: tutaj wczytuj pliki .3DObj
//	m_iTex = GetTexLoader()->LoadTexMipmaps( _T("textures/player01.bmp") ); //10
//	m_iTexBump = GetTexLoader()->LoadEmbossBump( _T("textures/player01bump.bmp") );	//index 11, 12
//	m_i3DObjIndex = Get3DObjManager()->Create3DObject( _T("objects/boss01.3DObj"), m_iTex, -1, m_iTexBump ); //index 0

	// Musi byæ wywo³ana na samym koñcu
	Get3DObjManager()->Initialization();
	return GL_TRUE;
}

GLboolean CGameControl::LoadAllSounds()
{
#if _USE_SOUND_OPENAL_

	if( !GetOpenALManager() )
		return GL_FALSE;

	if( !GetOpenALManager()->IsInitOK() )
		return GL_FALSE;

	//TODO: Tutaj wczytuj wszelkie dŸwiêki dla biblioteki OpenAL
	GetOpenALManager()->LoadSound( "sounds/test.wav" );					// index 0

#ifdef _USE_OGG_
	GetOpenALManager()->LoadSound( "sounds/test.ogg", AL_TRUE, AL_TRUE );	// index 1
	GetOpenALManager()->DoUpdateAllStream();
#endif

	return AL_TRUE;

#else // FMOD

	if( !GetSoundFMOD() )
		return GL_FALSE;

	//TODO: Tutaj wczytuj wszelkie dŸwiêki dla biblioteki FMOD

	GetSoundFMOD()->LoadSample( "sounds/test.wav" );

	return AL_TRUE;
#endif
}

GLvoid CGameControl::Exit()
{
	GetRMessageBox()->DoMessageBox( CMultiLanguage::GetInstance()->GetLangText( 1 ), CRMessageBox::EMsgYesNo, MsgExit, NULL );
}

GLvoid MsgExit( GLvoid ) //odpowiedz na klikniecie na tak w msgboxie przy wychodzeniu z gry
{
	::PostQuitMessage( 0 );
}

GLvoid CGameControl::RestartObjects()
{
	//przy restartowaniu nalezy dla kazdego obiektu klasy CSDLFont wywolac funkcje ClearStorageBuffer
	for( GLint i = static_cast< GLint >( g_aPointerSDLFont.size() - 1 ); i >= 0; --i ) {
		if( g_aPointerSDLFont[ i ] )
			g_aPointerSDLFont[ i ]->ClearStorageBuffer();
	}

	delete m_p3DObjManager;
	delete m_pTextureLoader;
	delete m_pTextureFont;
	delete m_pVertexArrays;
	delete m_p3DSLoader;

	m_pTextureLoader = new CTextureLoader();
	m_pTextureFont = new CTextureFont();
	GetEmbossBump()->ClearAllArrays();
	m_pVertexArrays = CVertexArrays::Construct();
	GetStencilShadow()->RestartObjects();
	m_p3DObjManager = new C3DObjManager();

	GetConsole()->RestartObjects();
	GetRMessageBox()->RestartObjects();
	
	CVector4 cActualPosLight0( m_cPosLight0 ); // SetLights ustawi defaultowa pozycje swiatla, wiec tutaj ja zapamietujemy i przywracamy
	SetLights();
	m_cPosLight0 = cActualPosLight0;
	GetLighting()->SetPosition( GL_LIGHT0, m_cPosLight0 );

	m_p3DSLoader = new C3DSLoader();

	LoadAllTextures();
	LoadAll3DObjFiles();
	
	SetVSync();
	CreateVertices();

	m_fRotX = m_fRotY = m_fRotZ = 0.0f;
	m_fPosZ = -200.0f;
	m_fPosX = m_fPosY = 0.0f;
	m_iCurX = 1;
	m_iCurY = 0;
	m_bMouseHoldL = GL_FALSE;
	m_bMouseHoldR = GL_FALSE;
	m_bShowNormals = GL_FALSE;
	m_fNormalLength = 1.0f;
	m_ePolygonMode = GL_FILL;
	m_bLighting = GL_TRUE;
	m_bTexturing = GL_TRUE;
	m_bSmooth = GL_TRUE;
	m_bBlending = GL_FALSE;
	m_iShowAxis = 1;
	m_bToggleArrowsControl = GL_FALSE;

	m_cPosObj.x = m_cPosObj.y = m_cPosObj.z = 0.0f;
	m_fMoveObjSeed = MOVE_OBJ_SEED;

	m_cRot90.x = m_cRot90.y = m_cRot90.z = 0.0f;
	m_cScale.x = m_cScale.y = m_cScale.z = 1.0f;
	m_cLenObj.x = m_cLenObj.y = m_cLenObj.z = 0.0f;
	
	CreateQuadric();
}

GLboolean CGameControl::KeyDown( GLuint uiKey )
{
	if( GetConsole()->KeyDown( uiKey ) )
		return GL_TRUE;
	
	if( GetRMessageBox()->KeyDown( uiKey ) )
		return GL_TRUE;

	//aby klawisz przeszedl do funkcji CGameControl::Keyboard()
	//gdy nie zostal skonsumowany:
	m_bKeys[ uiKey ] = GL_TRUE;

	return GL_TRUE;
}

GLboolean CGameControl::KeyUp( GLuint uiKey )
{
	if( GetConsole()->KeyUp( uiKey ) )
		return GL_TRUE;
	
	//tutaj mo¿emy przekazac klawisz dalej do naszej klasy
	//lub obs³u¿yc go w CGameControl:
	m_bKeys[ uiKey ] = GL_FALSE;
	
	return GL_TRUE;
}

GLboolean CGameControl::MouseLButtonDown( GLint iX, GLint iY )
{
	if( GetRMessageBox()->MouseLButtonDown( iX, iY ) )
		return GL_TRUE;
	
	ptCursorPos.x = ptCursorOldPos.x = iX;
	ptCursorPos.y = ptCursorOldPos.y = iY;
	m_bMouseHoldL = GL_TRUE;
	if( !CWindowData::GetInstance()->SettingFile().bFullScreen )
		::ClientToScreen( CWindowData::GetInstance()->GetHWND(), &ptCursorPos );
	/*
	tutaj przetwarzamy klikniecie myszy lub
	przekazujemy je dalej do innej klasy
	np:

	switch( GameMode ) {
		case EMainMenu:
			obiektKalsy1->MouseLButtonDown( iX, iY );
			break;
		case ESingleGame:
			obiektKalsy2->MouseLButtonDown( iX, iY );
			break;
		case EOptions:
			obiektKalsy3->MouseLButtonDown( iX, iY );
			break;
	}
	*/
	return GL_TRUE;
}

GLboolean CGameControl::MouseLButtonUp( GLint /*iX*/, GLint /*iY*/)
{
	m_bMouseHoldL = GL_FALSE;
	return GL_TRUE;
}

GLboolean CGameControl::MouseRButtonDown( GLint iX, GLint iY )
{
	//tutaj przetwarzamy klikniecie prawego przycisku
	//myszy lub przekazujemy je dalej do innej klasy

	ptCursorPos.x = ptCursorOldPos.x = iX;
	ptCursorPos.y = ptCursorOldPos.y = iY;
	m_bMouseHoldR = GL_TRUE;
	if( !CWindowData::GetInstance()->SettingFile().bFullScreen )
		::ClientToScreen( CWindowData::GetInstance()->GetHWND(), &ptCursorPos );
	
	return GL_TRUE;
}

GLboolean CGameControl::MouseRButtonUp( GLint /*iX*/, GLint /*iY*/)
{
	m_bMouseHoldR = GL_FALSE;
	return GL_TRUE;
}

GLboolean CGameControl::MouseWheel( GLint iScrollLines )
{
	if( GetConsole()->MouseWheel( iScrollLines ) )
		return GL_TRUE;
	
	//tutaj implementujemy obsluge rolki myszy
	//parametr iScrollLines mowi nam o ile linii i w ktora strone 
	//wykonano obrot, wartosc ujemna - w dol, wartosc dodatnia - w gore

	m_fPosZ += ( static_cast< GLfloat >( iScrollLines ) * 5.0f );
	
	return GL_TRUE;
}

GLvoid CGameControl::Enable2D()
{
	if( m_bIs2D )
		return;
	m_bIs2D = GL_TRUE;

	GLint vPort[ 4 ];
  
	glGetIntegerv( GL_VIEWPORT, vPort );
  
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho( 0, vPort[ 2 ], vPort[ 3 ], 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

GLvoid CGameControl::Disable2D()
{
	if( !m_bIs2D )
		return;
	m_bIs2D = GL_FALSE;

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();   
	glMatrixMode( GL_MODELVIEW);
	glPopMatrix();	
}

GLvoid CGameControl::Draw()
{
	GetSpeedCtrl()->RefreshTime();
	GetFps()->IncreaseFps();

//******************** tutaj rysujemy ****************************************
	//wyszsto co jest zawarte miedzy /*****/ jest tylko przyk³adem i oczywiscie
	//nale¿y w to miejsce rysowaæ w³¹sne obiekty, wedle w³asnego uznania
	
	GetCursorPosition();
	
#if _USE_2D_ONLY_
#else //3D:

	Disable2D();
	glEnable( GL_DEPTH_TEST );

	m_bCullFace ? glEnable( GL_CULL_FACE )
				: glDisable( GL_CULL_FACE );

	if( m_bLighting ) {
		GetLighting()->DrawPositionLight( GL_LIGHT0, 1.0f, 0.0f, 0.0f );
		glEnable( GL_LIGHTING );
	}
	else glDisable( GL_LIGHTING );

	glTranslatef( m_fPosX, m_fPosY, m_fPosZ );
	glRotatef( m_fRotX, 1.0f, 0.0f, 0.0f );
	glRotatef( m_fRotY, 0.0f, 1.0f, 0.0f );
	glRotatef( m_fRotZ, 0.0f, 0.0f, 1.0f );

	if( m_iShowAxis > 0 ) {
		GLfloat fLengthOfAxis = 50.0f;
		GLboolean bIsLighting = GL_FALSE;
		if( glIsEnabled( GL_LIGHTING ) ) {
			glDisable( GL_LIGHTING );
			bIsLighting = GL_TRUE;
		}
		glDisable( GL_TEXTURE_2D );
		glBegin( GL_LINES );
			//x
			glColor3f( 1.0f, 0.0f, 0.0f );
			if( m_iShowAxis == 2 ) {
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( -fLengthOfAxis, 0.0f, 0.0f );
			}
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( fLengthOfAxis, 0.0f, 0.0f );
			//y
			glColor3f( 0.0f, 1.0f, 0.0f );
			if( m_iShowAxis == 2 ) {
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( 0.0f, -fLengthOfAxis, 0.0f );
			}
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, fLengthOfAxis, 0.0f );
			//z
			glColor3f( 0.0f, 0.0f, 1.0f );
			if( m_iShowAxis == 2 ) {
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( 0.0f, 0.0f, -fLengthOfAxis );
			}
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glVertex3f( 0.0f, 0.0f, fLengthOfAxis );
		glEnd();
		//rysuj strzalki jako stozki
		GLboolean bIsCullFaceDisable = GL_FALSE;
		if( glIsEnabled( GL_CULL_FACE ) ) {
			glDisable( GL_CULL_FACE );
			bIsCullFaceDisable = GL_TRUE;
		}
		//os X
		glPushMatrix();
		glTranslatef( fLengthOfAxis, 0.0f, 0.0f );
		glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );
		glColor3f( 1.0f, 0.0f, 0.0f );
		gluCylinder( m_pQuadric, 1.0f, 0.0f, 4.0f, 32, 32 );
		glPopMatrix();
		//os Y
		glPushMatrix();
		glTranslatef( 0.0f, fLengthOfAxis, 0.0f );
		glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
		glColor3f( 0.0f, 1.0f, 0.0f );
		gluCylinder( m_pQuadric, 1.0f, 0.0f, 4.0f, 32, 32 );
		glPopMatrix();
		//os Z
		glPushMatrix();
		glTranslatef( 0.0f, 0.0f, fLengthOfAxis );
		glColor3f( 0.0f, 0.0f, 1.0f );
		gluCylinder( m_pQuadric, 1.0f, 0.0f, 4.0f, 32, 32 );
		glPopMatrix();
		if( bIsCullFaceDisable )
			glEnable( GL_CULL_FACE );
		if( bIsLighting )
			glEnable( GL_LIGHTING );
	}

	//polozenie obieku w osi + wlasna pozycja
	glLoadIdentity();
	glTranslatef( m_fPosX, m_fPosY, m_fPosZ );
	glRotatef( m_fRotX, 1.0f, 0.0f, 0.0f );
	glRotatef( m_fRotY, 0.0f, 1.0f, 0.0f );
	glRotatef( m_fRotZ, 0.0f, 0.0f, 1.0f );

	//glScalef( m_cScale.x, m_cScale.y, m_cScale.z );
	//glTranslatef( m_cPosObj.x, m_cPosObj.y, m_cPosObj.z );
	//glRotatef( m_cRot90.x, 1.0f, 0.0f, 0.0f );
	//glRotatef( m_cRot90.y, 0.0f, 1.0f, 0.0f );
	//glRotatef( m_cRot90.z, 0.0f, 0.0f, 1.0f );

	//obracanie mysza
	if( m_bMouseHoldL ) {
		ptCursorOldPos.x = ptCursorPos.x;
		ptCursorOldPos.y = ptCursorPos.y;
		GetCursorPos( &ptCursorPos );

		if( m_iCurX == 0 )
			m_fRotX -= ( static_cast< GLfloat >( ptCursorOldPos.x - ptCursorPos.x ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
		else if( m_iCurX == 1 )
			m_fRotY -= ( static_cast< GLfloat >( ptCursorOldPos.x - ptCursorPos.x ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
		else
			m_fRotZ -= ( static_cast< GLfloat >( ptCursorOldPos.x - ptCursorPos.x ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
		
		if( m_iCurY == 0 )
			m_fRotX -= ( static_cast< GLfloat >( ptCursorOldPos.y - ptCursorPos.y ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
		else if( m_iCurY == 1 )
			m_fRotY -= ( static_cast< GLfloat >( ptCursorOldPos.y - ptCursorPos.y ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
		else
			m_fRotZ -= ( static_cast< GLfloat >( ptCursorOldPos.y - ptCursorPos.y ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );

//		m_fRotY -= ( static_cast< GLfloat >( ptCursorOldPos.x - ptCursorPos.x ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
//		m_fRotX -= ( static_cast< GLfloat >( ptCursorOldPos.y - ptCursorPos.y ) * GetSpeedCtrl()->GetMultiplier() * 100.0f );
	}

	if( m_bMouseHoldR ) {
		ptCursorOldPos.x = ptCursorPos.x;
		ptCursorOldPos.y = ptCursorPos.y;
		GetCursorPos( &ptCursorPos );

		m_fPosX -= ( static_cast< GLfloat >( ptCursorOldPos.x - ptCursorPos.x ) * GetSpeedCtrl()->GetMultiplier() * 10.0f );
		m_fPosY += ( static_cast< GLfloat >( ptCursorOldPos.y - ptCursorPos.y ) * GetSpeedCtrl()->GetMultiplier() * 10.0f );
	}

	glPolygonMode( GL_FRONT_AND_BACK, m_ePolygonMode );

	glShadeModel( m_bSmooth ? GL_SMOOTH : GL_FLAT );

	if( m_bBlending ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glEnable( GL_BLEND );
	}
	else glDisable( GL_BLEND );

	glColor3f( 1.0f, 1.0f, 1.0f );
	if( !m_bTexturing || m_p3DSLoader->GetTexCoordsNum( 0 ) == 0 
		|| m_iTex == -1 )
		glDisable( GL_TEXTURE_2D );
	else
		m_pTextureLoader->SetTexture( m_iTex );

	if( m_bDrawFrom3DSLoader )
	{
		if( m_bEmbossBump && m_bTexturing && m_iTex > -1 && m_iTexBump > -1 )
		{
			if( m_bDrawWithIndexing )
				Get3DSLoader()->DrawEmbossBumpObjectElem( GL_LIGHT0, m_iTex, m_iTexBump );
			else
				Get3DSLoader()->DrawEmbossBumpObject( GL_LIGHT0, m_iTex, m_iTexBump );
		}
		else
		{
			if( m_bDrawWithIndexing )
			{
				GetVertexArrays()->DisableClientState();
				GetVertexArrays()->InitialArraysElem();
				GetVertexArrays()->LockArrays( 0, GetVertexArrays()->GetVerticesElemSize() );

				for( GLint i = 0; i < GetVertexArrays()->GetIndicesSize(); ++i )
					glDrawElements( GL_TRIANGLES, GetVertexArrays()->GetSubIndicesSize( i ), GL_UNSIGNED_SHORT, GetVertexArrays()->GetSubIndices( i ) );
				
				GetVertexArrays()->UnlockArrays();
				GetVertexArrays()->DisableClientState();
			}
			else
			{
				GetVertexArrays()->DisableClientState();
				GetVertexArrays()->InitialArrays();
				GLint iStart = 0;
				for( GLint i = 0; i < Get3DSLoader()->Get3DSObjectsSize(); ++i )
				{
					glDrawArrays( GL_TRIANGLES, iStart, m_p3DSLoader->GetVericesNum( i ) );
					iStart += m_p3DSLoader->GetVericesNum( i );
				}
				GetVertexArrays()->DisableClientState();
			}
		}
	}
	else {
		if( m_i3DObjIndex > -1 )
			Get3DObjManager()->Draw3DObject_Lists( m_i3DObjIndex, m_bEmbossBump, GL_LIGHT0 );
		//Get3DObjManager()->Draw3DObject( 0, m_bEmbossBump, GL_LIGHT0 );
	}
/*
	//kula
	glDisable( GL_LIGHTING );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glDisable( GL_TEXTURE_2D );
	glColor4ub( 0, 255, 255, 128 );
	gluSphere( m_Quadric, 10.0f, 16, 16 );
*/

//	GetVertexArrays()->DrawArrays();
	//glDrawArrays( GL_TRIANGLES, 0, m_p3DSLoader->GetVericesNum( 0 ) );
/*	glTranslatef( 100.0f, 0.0f, 0.0f );
	glDrawArrays( GL_TRIANGLES, 0, m_p3DSLoader->GetVericesNum( 0 ) );
	glTranslatef(-100.0f, 100.0f, 0.0f );
	glDrawArrays( GL_TRIANGLES, 0, m_p3DSLoader->GetVericesNum( 0 ) );
	glTranslatef( 0.0f, -200.0f, 0.0f );
	glDrawArrays( GL_TRIANGLES, 0, m_p3DSLoader->GetVericesNum( 0 ) );
	glTranslatef(-100.0f, 100.0f, 0.0f );
	glDrawArrays( GL_TRIANGLES, 0, m_p3DSLoader->GetVericesNum( 0 ) );
*/
//	GetVertexArrays()->DisableClientState();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glShadeModel( GL_SMOOTH );

	if( m_bShowNormals ) {
		if( m_bDrawWithIndexing )
			GetVertexArrays()->DrawNormalsElem( 0, GetVertexArrays()->GetNormalsElemSize(), m_fNormalLength );
		else
			GetVertexArrays()->DrawNormals( 0, GetVertexArrays()->GetNormalsSize(), m_fNormalLength );
	}

#endif //_USE_2D_ONLY_
	
//*************************** koniec rysowania ************************************
	glLoadIdentity();
	if( m_bToggleArrowsControl ) {
		GetSDLFont()->DrawText( 3, 40, RGB( 255, 255, 255 ), _T("[F3] Mode: object move") );
		GetSDLFont()->DrawTextFormat( 3, 60, GL_FALSE, RGB( 255, 255, 255 ), _T("[<][>] Move seed: %.3f"), m_fMoveObjSeed );
		GetSDLFont()->DrawTextFormat( 3, 80, GL_FALSE, RGB( 255, 255, 255 ), _T("Move: %.3f, %.3f, %.3f"), m_cPosObj.x, m_cPosObj.y, m_cPosObj.z );
	}
	else
		GetSDLFont()->DrawText( 3, 40, RGB( 255, 255, 255 ), _T("[F3] Mode: view"), GL_FALSE );

	if( m_bLighting )
		GetSDLFont()->DrawText( 3, 100, RGB( 0, 255, 0 ), _T("[L]ighting: on") );
	else
		GetSDLFont()->DrawText( 3, 100, RGB( 255, 0, 0 ), _T("[L]ighting: off") );

	if( m_bSmooth )
		GetSDLFont()->DrawText( 3, 120, RGB( 255, 255, 255 ), _T("[S]hade mode: smooth") );
	else
		GetSDLFont()->DrawText( 3, 120, RGB( 255, 255, 255 ), _T("[S]hade mode: flat") );

	if( m_bBlending )
		GetSDLFont()->DrawText( 3, 140, RGB( 0, 255, 0 ), _T("[B]lending: on") );
	else
		GetSDLFont()->DrawText( 3, 140, RGB( 255, 0, 0 ), _T("[B]lending: off") );

	if( m_bTexturing )
		GetSDLFont()->DrawText( 3, 160, RGB( 0, 255, 0 ), _T("[T]exturing: on") );
	else
		GetSDLFont()->DrawText( 3, 160, RGB( 255, 0, 0 ), _T("[T]exturing: off") );

	switch( m_ePolygonMode ) {
		case GL_POINT:
			GetSDLFont()->DrawText( 3, 180, RGB( 255, 255, 255 ), _T("Polygon [M]ode: point") );
			break;
		case GL_LINE:
			GetSDLFont()->DrawText( 3, 180, RGB( 255, 255, 255 ), _T("Polygon [M]ode: line") );
			break;
		case GL_FILL:
			GetSDLFont()->DrawText( 3, 180, RGB( 255, 255, 255 ), _T("Polygon [M]ode: fill") );
			break;
	}

	switch( m_iShowAxis ) {
		case 0:
			GetSDLFont()->DrawText( 3, 200, RGB( 255, 255, 255 ), _T("[A]xes: off") );
			break;
		case 1:
			GetSDLFont()->DrawText( 3, 200, RGB( 255, 255, 255 ), _T("[A]xes: plus only") );
			break;
		case 2:
			GetSDLFont()->DrawText( 3, 200, RGB( 255, 255, 255 ), _T("[A]xes: all") );
			break;
	}

	if( m_bShowNormals )
		GetSDLFont()->DrawText( 3, 220, RGB( 0, 255, 0 ), _T("[N]ormals: on") );
	else
		GetSDLFont()->DrawText( 3, 220, RGB( 255, 0, 0 ), _T("[N]ormals: off") );

	if( m_bEmbossBump )
		GetSDLFont()->DrawText( 3, 240, RGB( 0, 255, 0 ), _T("[E]mboss bump mapping: on") );
	else
		GetSDLFont()->DrawText( 3, 240, RGB( 255, 0, 0 ), _T("[E]mboss bump mapping: off") );

	if( m_bCullFace )
		GetSDLFont()->DrawText( 3, 260, RGB( 0, 255, 0 ), _T("Cull [F]ace: on") );
	else
		GetSDLFont()->DrawText( 3, 260, RGB( 255, 0, 0 ), _T("Cull [F]ace: off") );

	//wyswietlene pozycji obiektu
	GetSDLFont()->DrawTextFormat( 3, 280, GL_FALSE, RGB( 255, 255, 255 ), _T("Pos: %.2f, %.2f, %.2f"), m_fPosX, m_fPosY, m_fPosZ );
	//wyswietlenia obrotow
	GetSDLFont()->DrawTextFormat( 3, 300, GL_FALSE, RGB( 255, 255, 255 ), _T("Rot X: %.2f"), m_fRotX );
	GetSDLFont()->DrawTextFormat( 3, 320, GL_FALSE, RGB( 255, 255, 255 ), _T("Rot Y: %.2f"), m_fRotY );
	GetSDLFont()->DrawTextFormat( 3, 340, GL_FALSE, RGB( 255, 255, 255 ), _T("Rot Z: %.2f"), m_fRotZ );

	GetSDLFont()->DrawTextFormat( 3, 380, GL_FALSE, RGB( 255, 255, 255 ), _T("Scale X: %.2f, Y: %.2f, Z: %.2f"), m_cScale.x, m_cScale.y, m_cScale.z );

	GetSDLFont()->DrawTextFormat( 3, 420, GL_FALSE, RGB( 255, 255, 255 ), _T("Dimensions X: %.2f, Y: %.2f, Z: %.2f"), m_cLenObj.x, m_cLenObj.y, m_cLenObj.z );

	///////////////////////////////////////////////////////////////////

	GetRMessageBox()->Draw();

	GetFps()->DrawFps();

#if _USE_2D_ONLY_
	//jezeli mamy sta³y tryb 2D to mo¿emy wyœwietliæ konsolê tylko za pomoc¹ CSDLFont
	GetConsole()->DrawConsoleWithSDLFont();
#else
	#if _USE_CONSOLE_WITH_MY_FONT_
		GetConsole()->DrawConsoleMyFont();
	#elif _USE_CONSOLE_WITH_BITMAP_FONT_
		GetConsole()->DrawConsoleWithBitmapFont();
	#else
		GetConsole()->DrawConsoleWithSDLFont();
	#endif //_USE_CONSOLE_WITH_MY_FONT_ _USE_CONSOLE_WITH_BITMAP_FONT_
#endif //_USE_2D_ONLY_

	//rysowanie jasnosci, poni¿ej tej instrukcji nie mo¿na ju¿ niczego rysowaæ
	CBrightness::GetInstance()->Draw();
}

GLvoid CGameControl::GetCursorPosition()
{
	::GetCursorPos( &m_ptCursor );
	if( !GetWinData()->SettingFile().bFullScreen ) {
		RECT rcWin, rcClient;
		::GetWindowRect( GetWinData()->GetHWND(), &rcWin );
		::GetClientRect( GetWinData()->GetHWND(), &rcClient );

		//jako, ¿e otrzymana pozycja kursora
		//jest wzgledem ekranu, to dla programu okienkowego trzeba pozycjê
		//kursora przekszta³ciæ tak aby by³a wzglêdem okna klienta:

		//iWidthFrame to szerokosc ramki okna, ramki tej po bokach i od do³u.
		//Obliczam d³ugoœæ okna (zawsze jest wiêksza od ustawionej rozdzielczoœci
		//bo dochodzi w³aœnie ramka) i odejmuje od niej d³ugoœæ okna klienat (czyli X
		//rozdzielczoœci )
		//teraz mam gruboœæ dwóch ramek z lewej i prawej strony okna, aby obliczyæ
		//gruboœæ pojedynczej ramki, dzielimy przez 2
		GLint iWidthFrame = ( ( rcWin.right - rcWin.left - rcClient.right ) / 2 );
		//pozycja kursora wzglêdem okna klienta:
		m_ptCursor.x -= ( rcWin.left + iWidthFrame );
		m_ptCursor.y -= ( rcWin.top + ( ( rcWin.bottom - rcWin.top - rcClient.bottom ) - iWidthFrame ) );
	}
}

GLvoid CGameControl::Timer()
{
	GetFps()->SetTextFps();
	GetFps()->CleanFps();

	for( GLint i = static_cast< GLint >( g_aPointerSDLFont.size() - 1 ); i >= 0; --i ) {
		if( g_aPointerSDLFont[ i ] )
			g_aPointerSDLFont[ i ]->Timer();
	}
#if _USE_SOUND_OPENAL_
	COpenALManager::GetInstance()->Timer();
#endif
}

GLvoid CGameControl::Keyboard()
{
	if( m_bKeys[ VK_RETURN ] ) { //nacisnieto ENTER
		m_bKeys[ VK_RETURN ] = GL_FALSE;

		if( HIBYTE( ::GetKeyState( VK_MENU ) ) ) //kombinacja Alt+Enter
			::ShowWindow( GetWinData()->GetHWND(), SW_MINIMIZE ); //minimalizacja
		else {
			//TODO: Tutaj obsluga samego ENTERa
		}
	}

	if( m_bKeys[ VK_ESCAPE ] ) { //wyjœcie z programu
		m_bKeys[ VK_ESCAPE ] = GL_FALSE;
		Exit();
	}

	if( m_bKeys[ 0xC0 ] ) { //~
		m_bKeys[ 0xC0 ] = GL_FALSE;
		if( GetConsole()->IsConsole() )
			GetConsole()->HideConsole();
		else
			GetConsole()->ShowConsole();
	}

	if( m_bKeys[ VK_F3 ] ) {
		m_bKeys[ VK_F3 ] = GL_FALSE;
		m_bToggleArrowsControl = !m_bToggleArrowsControl;
	}

	if( m_bKeys[ VK_F4 ] ) {
		m_bKeys[ VK_F4 ] = GL_FALSE;
		m_bToggleArrowsLightPos = !m_bToggleArrowsLightPos;
	}

	if( !m_bToggleArrowsControl ) {
		//obracanie
		if( m_bKeys[ VK_LEFT ] ) {
			m_bKeys[ VK_LEFT ] = GL_FALSE;
			m_fRotY -= ( 100.0f * m_pSpeedControl->GetMultiplier() );
		}

		if( m_bKeys[ VK_RIGHT ] ) {
			m_bKeys[ VK_RIGHT ] = GL_FALSE;
			m_fRotY += ( 100.0f * m_pSpeedControl->GetMultiplier() );
		}

		if( m_bKeys[ VK_UP ] ) {
			m_bKeys[ VK_UP ] = GL_FALSE;
			m_fRotX -= ( 100.0f * m_pSpeedControl->GetMultiplier() );
		}

		if( m_bKeys[ VK_DOWN ] ) {
			m_bKeys[ VK_DOWN ] = GL_FALSE;
			m_fRotX += ( 100.0f * m_pSpeedControl->GetMultiplier() );
		}
	}
	else {
		if( m_bToggleArrowsLightPos ) {
			if( m_bKeys[ VK_LEFT ] ) {
				m_bKeys[ VK_LEFT ] = GL_FALSE;
				m_cPosLight0.x -= 1.0f;
			}

			if( m_bKeys[ VK_RIGHT ] ) {
				m_bKeys[ VK_RIGHT ] = GL_FALSE;
				m_cPosLight0.x += 1.0f;
			}

			if( m_bKeys[ VK_UP ] ) {
				m_bKeys[ VK_UP ] = GL_FALSE;
				m_cPosLight0.y += 1.0f;
			}

			if( m_bKeys[ VK_DOWN ] ) {
				m_bKeys[ VK_DOWN ] = GL_FALSE;
				m_cPosLight0.y -= 1.0f;
			}

			if( m_bKeys[ VK_NEXT ] ) { //z w glab ekranu
				m_bKeys[ VK_NEXT ] = GL_FALSE;
				m_cPosLight0.z -= 1.0f;
			}

			if( m_bKeys[ VK_PRIOR ] ) {//z do widza
				m_bKeys[ VK_PRIOR ] = GL_FALSE;
				m_cPosLight0.z += 1.0f;
			}

			GetLighting()->SetPosition( GL_LIGHT0, m_cPosLight0 );
		}
		else {
			//przesuwanie obiektu
			GLboolean bRecalcVerices = GL_FALSE;
			CVector3 cOldPosObj( m_cPosObj );

			if( m_bKeys[ VK_LEFT ] ) {
				m_bKeys[ VK_LEFT ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.x -= m_fMoveObjSeed;
			}

			if( m_bKeys[ VK_RIGHT ] ) {
				m_bKeys[ VK_RIGHT ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.x += m_fMoveObjSeed;
			}

			if( m_bKeys[ VK_UP ] ) {
				m_bKeys[ VK_UP ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.y += m_fMoveObjSeed;
			}

			if( m_bKeys[ VK_DOWN ] ) {
				m_bKeys[ VK_DOWN ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.y -= m_fMoveObjSeed;
			}

			if( m_bKeys[ VK_NEXT ] ) { //z w glab ekranu
				m_bKeys[ VK_NEXT ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.z -= m_fMoveObjSeed;
			}

			if( m_bKeys[ VK_PRIOR ] ) {//z do widza
				m_bKeys[ VK_PRIOR ] = GL_FALSE;
				bRecalcVerices = GL_TRUE;

				m_cPosObj.z += m_fMoveObjSeed;
			}

			if( bRecalcVerices )
			{
				CVector3 cVecTrans( m_cPosObj - cOldPosObj ); // w istocie otrzymujemy m_fMoveObjSeed na odpowiedniej osi z odpowiednim znakiem
				if( m_bDrawWithIndexing )
				{
					for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i )
						GetVertexArrays()->SetVertexElem( i, GetMatrixOp()->TranslateXYZ3( GetVertexArrays()->GetVertexElem( i ), cVecTrans ) );
				}
				else
				{
					for( GLint i = 0; i < GetVertexArrays()->GetVerticesSize(); ++i )
						GetVertexArrays()->SetVertex( i, GetMatrixOp()->TranslateXYZ3( GetVertexArrays()->GetVertex( i ), cVecTrans ) );
				}

				CreateVerticesForScale();
				ComputeLengthObject( 0, m_p3DSLoader->GetVericesNum( 0 ) );

				m_a3DTransf.push_back( S3DTransf( GL_FALSE, cVecTrans ) );
			}

			if( m_bKeys[ 0xBC ] ) {//<
				m_bKeys[ 0xBC ] = GL_FALSE;
				m_fMoveObjSeed -= MOVE_OBJ_SEED;
			}

			if( m_bKeys[ 0xBE ] ) {//>
				m_bKeys[ 0xBE ] = GL_FALSE;
				m_fMoveObjSeed += MOVE_OBJ_SEED;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	GLboolean bReScale = GL_FALSE;

	if( m_bKeys[ VK_HOME ] ) {
		m_bKeys[ VK_HOME ] = GL_FALSE;
		bReScale = GL_TRUE;
		m_cScale -= 0.01f;
	}

	if( m_bKeys[ VK_END ] ) {
		m_bKeys[ VK_END ] = GL_FALSE;
		bReScale = GL_TRUE;
		m_cScale += 0.01f;
	}

	if( bReScale )
	{
		if( m_bDrawWithIndexing )
		{
			for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i )
				GetVertexArrays()->SetVertexElem( i, GetMatrixOp()->ScaleXYZ3( m_aVerticesForScale[ i ], m_cScale ) );
		}
		else
		{
			for( GLint i = 0; i < GetVertexArrays()->GetVerticesSize(); ++i )
				GetVertexArrays()->SetVertex( i, GetMatrixOp()->ScaleXYZ3( m_aVerticesForScale[ i ], m_cScale ) );
		}
		
		ComputeLengthObject( 0, m_p3DSLoader->GetVericesNum( 0 ) );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	if( m_bKeys[ 'A' ] ) {
		m_bKeys[ 'A' ] = GL_FALSE;

		m_iShowAxis++;
		if( m_iShowAxis > 2 )
			m_iShowAxis = 0;
	}

	if( m_bKeys[ 'B' ] ) {
		m_bKeys[ 'B' ] = GL_FALSE;

		m_bBlending = !m_bBlending;
	}

	if( m_bKeys[ 'C' ] ) { //reset katow i polozenia i centrowanie obiektu w osi
		m_bKeys[ 'C' ] = GL_FALSE;

		SetObjectInCenter( 0, m_p3DSLoader->GetVericesNum( 0 ) );
		m_a3DTransf.push_back( S3DTransf( GL_TRUE ) );
	}

	if( m_bKeys[ 'E' ] ) { //emboss bump
		m_bKeys[ 'E' ] = GL_FALSE;

		if( m_iTex == -1 || m_iTexBump == -1 )
			m_bEmbossBump = GL_FALSE;
		else
			m_bEmbossBump = !m_bEmbossBump;
	}

	if( m_bKeys[ 'F' ] ) {
		m_bKeys[ 'F' ] = GL_FALSE;

		m_bCullFace = !m_bCullFace;
	}

	if( m_bKeys[ 'L' ] ) {
		m_bKeys[ 'L' ] = GL_FALSE;

		m_bLighting = !m_bLighting;
	}

	if( m_bKeys[ 'M' ] ) {
		m_bKeys[ 'M' ] = GL_FALSE;

		if( m_ePolygonMode == GL_FILL )
			m_ePolygonMode = GL_LINE;
		else if( m_ePolygonMode == GL_LINE )
			m_ePolygonMode = GL_POINT;
		else
			m_ePolygonMode = GL_FILL;
	}

	if( m_bKeys[ 'N' ] ) {
		m_bKeys[ 'N' ] = GL_FALSE;

		m_bShowNormals = !m_bShowNormals;
	}

	if( m_bKeys[ 'R' ] ) { //reset katow i polozenia i centrowanie obiektu w osi
		m_bKeys[ 'R' ] = GL_FALSE;

		m_fPosX = m_fPosY = 0.0f;
		m_fPosZ = -200.0f;
		m_fRotX = m_fRotY = m_fRotZ = 0.0f;
//		m_fScale = 1.0f;
	}

	if( m_bKeys[ 'S' ] ) {
		m_bKeys[ 'S' ] = GL_FALSE;
		m_bSmooth = !m_bSmooth;
	}

	if( m_bKeys[ 'T' ] ) {
		m_bKeys[ 'T' ] = GL_FALSE;

		if( m_iTex == -1 )
			m_bTexturing = GL_FALSE;
		else
			m_bTexturing = !m_bTexturing;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLboolean bReRotate = GL_FALSE;
	CVector3 cOldRot90( m_cRot90 );

	if( m_bKeys[ 'X' ] ) {
		m_bKeys[ 'X' ] = GL_FALSE;

		m_cRot90.x += 90.0f; // dla nie elem
		bReRotate = GL_TRUE;
	}

	if( m_bKeys[ 'Y' ] ) {
		m_bKeys[ 'Y' ] = GL_FALSE;

		m_cRot90.y += 90.0f; // dla nie elem
		bReRotate = GL_TRUE;
	}

	if( m_bKeys[ 'Z' ] ) {
		m_bKeys[ 'Z' ] = GL_FALSE;

		m_cRot90.z += 90.0f; // dla nie elem
		bReRotate = GL_TRUE;
	}

	if( bReRotate )
	{
		CVector3 cRot90( m_cRot90 - cOldRot90 ); // w sumie 90.0f na odpowiedniej osi z odpowiednim znakiem
		if( m_bDrawWithIndexing )
		{
			for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i ) {
				GetVertexArrays()->SetVertexElem( i, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetVertexElem( i ), cRot90 ) );
				GetVertexArrays()->SetNormalElem( i, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetNormalElem( i ), cRot90 ) );
			}
		}
		else
		{
			for( GLint i = 0; i < GetVertexArrays()->GetVerticesSize(); ++i ) {
				GetVertexArrays()->SetVertex( i, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetVertex( i ), cRot90 ) );
				GetVertexArrays()->SetNormal( i, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetNormal( i ), cRot90 ) );
			}
		}
		
		CreateVerticesForScale();
		ComputeLengthObject( 0, m_p3DSLoader->GetVericesNum( 0 ) );

		m_a3DTransf.push_back( S3DTransf( GL_FALSE, CVector3(), cRot90 ) );
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if( m_bKeys[ VK_NEXT ] ) {
		m_bKeys[ VK_NEXT ] = GL_FALSE;
		m_fPosZ += ( 300.0f * GetSpeedCtrl()->GetMultiplier() );
	}

	if( m_bKeys[ VK_PRIOR ] ) {
		m_bKeys[ VK_PRIOR ] = GL_FALSE;
		m_fPosZ -= ( 300.0f * GetSpeedCtrl()->GetMultiplier() );
	}

	//przyklad wykorzystania funkcji CWindowData::FullscreenSwitch()
	//gdzie klawiszem F2 mamy mozliwosc przelaczania sie pomiedzy trybami
	//pelnoekranowym a okienkowym
	if( m_bKeys[ VK_F2 ] ) {
		m_bKeys[ VK_F2 ] = GL_FALSE;
		GetWinData()->FullscreenSwitch();
	}
}

GLboolean CGameControl::WriteOnKeyboard( GLuint uiKey, LPTSTR lpBuffer, GLint iBufferLenMax )
{
	GLint iLength = static_cast< GLint >( CUHF::GetStringLength( lpBuffer ) );
	
	switch( uiKey )
	{
		case VK_BACK:
		{
			if( iLength > 0 )
				--iLength;
			lpBuffer[ iLength ] = 0;

			return GL_TRUE;
		}
		default:
		{
			if( iLength == 0 && uiKey == VK_SPACE )
				return GL_TRUE; //pierwszym znakiem nie moze byc spacja

			if( iLength >= iBufferLenMax - 1 ) {
				//if( CWindowData::GetInstance()->SettingFile().bSound )
				//	COpenALManager::GetInstance()->PlayNo3D( id_sound_max_string );
				return GL_TRUE; //ograniczenie dlugosci
			}

			if( /*number*/( uiKey >= 0x30 && uiKey <= 0x39 )
				/*alphabet*/|| ( uiKey >= 0x41 && uiKey <= 0x5A ) || uiKey == 0x20 
				/*NUMPAD*/|| ( uiKey >= 0x60 && uiKey <= 0x6F ) 
				/*pozostale ,.;'[]*/|| ( uiKey >= 0xBA && uiKey <= 0xBF ) || ( uiKey >= 0xDB && uiKey <= 0xDE ) )
			{
				lpBuffer[ iLength ] = static_cast< TCHAR >( GetKeyChar( uiKey ) );
				lpBuffer[ iLength + 1 ] = 0;
				return GL_TRUE;
			}
			break;
		}
	}

	return GL_FALSE;
}

GLuint CGameControl::GetKeyChar( GLuint uiKeyCode )
{
	//numpad
	if( uiKeyCode == 0x60 ) return 0x30;
	if( uiKeyCode == 0x61 ) return 0x31;
	if( uiKeyCode == 0x62 ) return 0x32;
	if( uiKeyCode == 0x63 ) return 0x33;
	if( uiKeyCode == 0x64 ) return 0x34;
	if( uiKeyCode == 0x65 ) return 0x35;
	if( uiKeyCode == 0x66 ) return 0x36;
	if( uiKeyCode == 0x67 ) return 0x37;
	if( uiKeyCode == 0x68 ) return 0x38;
	if( uiKeyCode == 0x69 ) return 0x39;
	if( uiKeyCode == 0x6A ) return 0x2A; //*
	if( uiKeyCode == 0x6B ) return 0x2B; //+
	if( uiKeyCode == 0x6C ) return 0x2C; //,
	if( uiKeyCode == 0x6D ) return 0x2D; //-
	if( uiKeyCode == 0x6E ) return 0x2E; //.
	if( uiKeyCode == 0x6F ) return 0x2F; ///

	if( HIBYTE( ::GetKeyState( VK_SHIFT ) ) ) { //kombinacja Shift+...
		//Shift+numer
		if( uiKeyCode == 0x30 ) uiKeyCode = 0x29; //)
		if( uiKeyCode == 0x31 ) uiKeyCode = 0x21; //!
		if( uiKeyCode == 0x32 ) uiKeyCode = 0x40; //@
		if( uiKeyCode == 0x33 ) uiKeyCode = 0x23; //#
		if( uiKeyCode == 0x34 ) uiKeyCode = 0x24; //$
		if( uiKeyCode == 0x35 ) uiKeyCode = 0x25; //%
		if( uiKeyCode == 0x36 ) uiKeyCode = 0x5E; //^
		if( uiKeyCode == 0x37 ) uiKeyCode = 0x26; //&
		if( uiKeyCode == 0x38 ) uiKeyCode = 0x2A; //*
		if( uiKeyCode == 0x39 ) uiKeyCode = 0x28; //(
		//if( uiKeyCode >= 0x41 && uiKeyCode <= 0x5A ) uiKeyCode += 32; //alfabet jet ok bo od razu dostajemy wielkimi
		if( uiKeyCode == 0xBA ) uiKeyCode = 0x3A; // znak :
		if( uiKeyCode == 0xBB ) uiKeyCode = 0x2B; // znak +
		if( uiKeyCode == 0xBC ) uiKeyCode = 0x3C; // znak <
		if( uiKeyCode == 0xBD ) uiKeyCode = 0x5F; // znak _
		if( uiKeyCode == 0xBE ) uiKeyCode = 0x3E; // znak >
		if( uiKeyCode == 0xBF ) uiKeyCode = 0x3F; // znak ?
		if( uiKeyCode == 0xDB ) uiKeyCode = 0x7B; // znak {
		if( uiKeyCode == 0xDC ) uiKeyCode = 0x7C; // znak |
		if( uiKeyCode == 0xDD ) uiKeyCode = 0x7D; // znak }
		if( uiKeyCode == 0xDE ) uiKeyCode = 0x22; // znak "
	}
	else { //bez shifta
		if( uiKeyCode >= 0x41 && uiKeyCode <= 0x5A ) uiKeyCode += 32; //alfabet malymi literami
		if( uiKeyCode == 0xBA ) uiKeyCode = 0x3B; // znak ;
		if( uiKeyCode == 0xBB ) uiKeyCode = 0x3D; // znak =
		if( uiKeyCode == 0xBC ) uiKeyCode = 0x2C; // znak ,
		if( uiKeyCode == 0xBD ) uiKeyCode = 0x2D; // znak -
		if( uiKeyCode == 0xBE ) uiKeyCode = 0x2E; // znak .
		if( uiKeyCode == 0xBF ) uiKeyCode = 0x2F; // znak /
		if( uiKeyCode == 0xDB ) uiKeyCode = 0x5B; // znak [
		if( uiKeyCode == 0xDC ) uiKeyCode = 0x5C; // znak '\'
		if( uiKeyCode == 0xDD ) uiKeyCode = 0x5D; // znak ]
		if( uiKeyCode == 0xDE ) uiKeyCode = 0x27; // znak '
	}

	return uiKeyCode;
}

GLvoid CGameControl::SetIndicesQuadAllScreen()
{
	m_aIndicesQuadAllScreen[ 0 ] = CWindowData::GetInstance()->SettingFile().iWidth;
	m_aIndicesQuadAllScreen[ 1 ] = CWindowData::GetInstance()->SettingFile().iHeight;
	m_aIndicesQuadAllScreen[ 2 ] = CWindowData::GetInstance()->SettingFile().iWidth;
	m_aIndicesQuadAllScreen[ 3 ] = 0;
	m_aIndicesQuadAllScreen[ 4 ] = 0;
	m_aIndicesQuadAllScreen[ 5 ] = CWindowData::GetInstance()->SettingFile().iHeight;
	m_aIndicesQuadAllScreen[ 6 ] = 0;
	m_aIndicesQuadAllScreen[ 7 ] = 0;
}

GLvoid CGameControl::DrawQuadOnAllScreen()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	glVertexPointer( 2, GL_INT, 0, m_aIndicesQuadAllScreen );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

CVector3 CGameControl::GetAddingVector()
{
	return m_cPosObj;
}

CVector3 CGameControl::GetRot90XYZ()
{
	return m_cRot90;
}

CVector3 CGameControl::GetScaleVector()
{
	return m_cScale;
}

CVector3* CGameControl::CreateTempVerticesArray( GLint iFirstVertex, GLsizei iCountVertex )
{
	GLint iSize = m_bDrawWithIndexing ? GetVertexArrays()->GetSubIndicesSize( 0 )
									  : GetVertexArrays()->GetVerticesSize();
	
	if( iCountVertex == 0 || iFirstVertex < 0 || iFirstVertex + iCountVertex > iSize )
		return NULL;

	// Jako ze obiekt moze byc obrocony, dokonujemy obliczen na kopi tablicy wierzcholkow
	CVector3* pTempVertices = new CVector3[ iCountVertex ];

	//przepisanie wierzcholkow do tablicy tymczasowej
	for( GLint i = 0; i < iCountVertex; ++i )
	{
		if( m_bDrawWithIndexing ) {
			GLint iIndex = GetVertexArrays()->GetIndexFromIndices( 0, iFirstVertex + i );
			pTempVertices[ i ] = GetVertexArrays()->GetVertexElem( iIndex );
		}
		else
			pTempVertices[ i ] = GetVertexArrays()->GetVertex( iFirstVertex + i );
	}

	return pTempVertices;
}

/*
Specjalna funkcja dla 3dsloadera, jej zadaniem jest automatyczne ustawienie obiektu
tak aby punkt 0,0,0 globlango ukladu wspolrzednych znajdowal sie dokladnie w 
srodku obiektu
*/
GLvoid CGameControl::SetObjectInCenter( GLint iFirstVertex, GLsizei iCountVertex )
{
	// Jako ze obiekt moze byc obrocony, dokonujemy obliczen na kopi tablicy wierzcholkow
	CVector3* pTempVertices = CreateTempVerticesArray( iFirstVertex, iCountVertex );
	if( !pTempVertices )
		return;

	//szukamy srodka
	CVector2 cMinMaxX( pTempVertices[ 0 ].x, pTempVertices[ 0 ].x );
	CVector2 cMinMaxY( pTempVertices[ 0 ].y, pTempVertices[ 0 ].y );
	CVector2 cMinMaxZ( pTempVertices[ 0 ].z, pTempVertices[ 0 ].z );

	SetMinMaxAxes( pTempVertices, cMinMaxX, cMinMaxY, cMinMaxZ, iCountVertex );

	m_cPosObj.x = -( ( cMinMaxX.x + cMinMaxX.y ) / 2.0f );
	m_cPosObj.y = -( ( cMinMaxY.x + cMinMaxY.y ) / 2.0f );
	m_cPosObj.z = -( ( cMinMaxZ.x + cMinMaxZ.y ) / 2.0f );

	if( m_bDrawWithIndexing ) {
		for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i )
			GetVertexArrays()->SetVertexElem( i, GetMatrixOp()->TranslateXYZ3( GetVertexArrays()->GetVertexElem( i ), m_cPosObj ) );
	}
	else {
		for( GLint i = 0; i < GetVertexArrays()->GetVerticesSize(); ++i )
			GetVertexArrays()->SetVertex( i, GetMatrixOp()->TranslateXYZ3( GetVertexArrays()->GetVertex( i ), m_cPosObj ) );
	}

	CreateVerticesForScale();

	delete [] pTempVertices;

	if( m_cPosObj != 0.0f )
		SetObjectInCenter( iFirstVertex, iCountVertex );
}

GLvoid CGameControl::ComputeLengthObject( GLint iFirstVertex, GLsizei iCountVertex )
{
	// Jako ze obiekt moze byc obrocony, dokonujemy obliczen na kopi tablicy wierzcholkow
	CVector3* pTempVertices = CreateTempVerticesArray( iFirstVertex, iCountVertex );
	if( !pTempVertices )
		return;

	//jezeli obrocilismy obiekt, to przeliczamy wierzcholki wzgledem oboru
//	for( GLint i = 0; i < iCountVertex; ++i ) {
//		pTempVertices[ i ] = GetMatrixOp()->ScaleXYZ3( pTempVertices[ i ], m_cScale );
//		pTempVertices[ i ] = GetMatrixOp()->RotateXYZ3( pTempVertices[ i ], m_cRot90 );
//		pTempVertices[ i ] = GetMatrixOp()->TranslateXYZ3( pTempVertices[ i ], m_cPosObj );
//	}

	//szukamy wartosci min i max dla kazdej z osi
	CVector2 cMinMaxX( pTempVertices[ 0 ].x, pTempVertices[ 0 ].x );
	CVector2 cMinMaxY( pTempVertices[ 0 ].y, pTempVertices[ 0 ].y );
	CVector2 cMinMaxZ( pTempVertices[ 0 ].z, pTempVertices[ 0 ].z );

	SetMinMaxAxes( pTempVertices, cMinMaxX, cMinMaxY, cMinMaxZ, iCountVertex );

	m_cLenObj.x = CVector2( 0.0f, cMinMaxX.x ).CalcDistance2D( CVector2( 0.0f, cMinMaxX.y ) );
	m_cLenObj.y = CVector2( 0.0f, cMinMaxY.x ).CalcDistance2D( CVector2( 0.0f, cMinMaxY.y ) );
	m_cLenObj.z = CVector2( 0.0f, cMinMaxZ.x ).CalcDistance2D( CVector2( 0.0f, cMinMaxZ.y ) );

	delete [] pTempVertices;
}

GLvoid CGameControl::SetMinMaxAxes( const CVector3* pTempVertices,
									CVector2& out_cMinMaxX, CVector2& out_cMinMaxY, CVector2& out_cMinMaxZ,
									GLint iCountVertex )
{
	// W CVector2 wartosc x oznacza: minimalna wartosc, a y: maksymalna wartosc, czyli np.:
	// out_cMinMaxX.x = minX
	// out_cMinMaxX.y = maxX
	//
	// out_cMinMaxY.x = minY
	// out_cMinMaxY.y = maxY, itd

	for( GLint i = 1; i < iCountVertex; ++i ) {
		if( pTempVertices[ i ].x < out_cMinMaxX.x ) out_cMinMaxX.x = pTempVertices[ i ].x;
		if( pTempVertices[ i ].x > out_cMinMaxX.y ) out_cMinMaxX.y = pTempVertices[ i ].x;

		if( pTempVertices[ i ].y < out_cMinMaxY.x ) out_cMinMaxY.x = pTempVertices[ i ].y;
		if( pTempVertices[ i ].y > out_cMinMaxY.y ) out_cMinMaxY.y = pTempVertices[ i ].y;

		if( pTempVertices[ i ].z < out_cMinMaxZ.x ) out_cMinMaxZ.x = pTempVertices[ i ].z;
		if( pTempVertices[ i ].z > out_cMinMaxZ.y ) out_cMinMaxZ.y = pTempVertices[ i ].z;
	}
}

GLvoid CGameControl::CreateVerticesForScale()
{
	//kopiwanie wierzcholkow do m_aVerticesForScale - tylko dla 3ds loadera
	m_aVerticesForScale.clear();

	if( m_bDrawWithIndexing )
	{
		for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i )
			m_aVerticesForScale.push_back( GetVertexArrays()->GetVertexElem( i ) );
	}
	else
	{
		for( GLint i = 0; i < GetVertexArrays()->GetVerticesSize(); ++i )
			m_aVerticesForScale.push_back( GetVertexArrays()->GetVertex( i ) );
	}
}

GLboolean CGameControl::ReLoad3dsFileAndTextures( LPCTSTR lp3dsFile, LPCTSTR lpTexFile, LPCTSTR lpBumpFile )
{
	GLboolean bOldTexturing = m_bTexturing;
	GLboolean bOldEmbossBump = m_bEmbossBump;

	m_bDrawFrom3DSLoader = lp3dsFile ? Get3DSLoader()->ReLoad3DSObject( lp3dsFile )
									 : GL_FALSE;

	GetTexLoader()->DeleteTexture( m_iTex );
	GetTexLoader()->DeleteTexture( m_iTexBump );
	m_iTex = -1;
	m_iTexBump = -1;

	if( m_bDrawFrom3DSLoader )
	{
		if( lpTexFile && CUHF::GetStringLength( lpTexFile ) > 0 )
			m_iTex = GetTexLoader()->LoadTexMipmaps( lpTexFile );

		if( lpBumpFile && CUHF::GetStringLength( lpBumpFile ) > 0 )
			m_iTexBump = GetTexLoader()->LoadEmbossBump( lpBumpFile );

		ReTransform3D();
	}

	m_bTexturing = ( m_iTex > -1 && m_bDrawFrom3DSLoader ) ? GL_TRUE : GL_FALSE;
	m_bEmbossBump = ( m_iTexBump > -1 && m_bTexturing ) ? GL_TRUE : GL_FALSE;

	if( m_bTexturing && !bOldTexturing )
		m_bTexturing = GL_FALSE; // uzytkownik wczesniej wylaczyl teksturowanie

	if( m_bEmbossBump && !bOldEmbossBump )
		m_bEmbossBump = GL_FALSE;

	return m_bDrawFrom3DSLoader;
}

GLvoid CGameControl::ReTransform3D()
{
	// Przywrocenie transformacji 3D urzytkownika, na przeladowanym obiekcie
	// Przeksztalcamy wierzcholki obiektu, wg aktualnego przesunieca, rotacji, skalowania

	GLint iSize = static_cast< GLint >( m_a3DTransf.size() );
	if( iSize <= 0 )
		return;

	m_cPosObj = 0.0f;
	m_cRot90 = 0.0f;

	for( GLint i = 0; i < iSize; ++i )
	{
		if( m_a3DTransf[ i ].m_bSetInCenter )
			SetObjectInCenter( 0, m_p3DSLoader->GetVericesNum( 0 ) );
		else
		{
			// translacja
			if( m_a3DTransf[ i ].m_cPosObj != 0.0f )
			{
				m_cPosObj += m_a3DTransf[ i ].m_cPosObj;

				for( GLint j = 0; j < GetVertexArrays()->GetVerticesElemSize(); ++j )
					GetVertexArrays()->SetVertexElem( j, GetMatrixOp()->TranslateXYZ3( GetVertexArrays()->GetVertexElem( j ), m_a3DTransf[ i ].m_cPosObj ) );
			}
			else if( m_a3DTransf[ i ].m_cRot90 != 0.0f )
			{
				m_cRot90 += m_a3DTransf[ i ].m_cRot90;

				for( GLint j = 0; j < GetVertexArrays()->GetVerticesElemSize(); ++j )
				{
					// rotacja wierzcholkow
					GetVertexArrays()->SetVertexElem( j, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetVertexElem( j ), m_a3DTransf[ i ].m_cRot90 ) );
					// rotacja normalnych
					GetVertexArrays()->SetNormalElem( j, GetMatrixOp()->RotateXYZ3( GetVertexArrays()->GetNormalElem( j ), m_a3DTransf[ i ].m_cRot90 ) );
				}
			}
		}
	}

	CreateVerticesForScale();

	// skalowanie
	for( GLint i = 0; i < GetVertexArrays()->GetVerticesElemSize(); ++i )
		GetVertexArrays()->SetVertexElem( i, GetMatrixOp()->ScaleXYZ3( m_aVerticesForScale[ i ], m_cScale ) );

	ComputeLengthObject( 0, m_p3DSLoader->GetVericesNum( 0 ) );
}