REM Delete Garbage Files
del *.ncb

@call cd Draw
@call __clean_debug.bat
@call cd..

@call cd Extensions
@call __clean_debug.bat
@call cd..

@call cd Fonts
@call __clean_debug.bat
@call cd..

@call cd Fps
@call __clean_debug.bat
@call cd..

@call cd Lighting
@call __clean_debug.bat
@call cd..

@call cd Logger
@call __clean_debug.bat
@call cd..

@call cd Fonts
@call __clean_debug.bat
@call cd..

@call cd MatrixOperations
@call __clean_debug.bat
@call cd..

@call cd MultiLanguage
@call __clean_debug.bat
@call cd..

@call cd Network
@call __clean_debug.bat
@call cd..

@call cd OpenGLFramework
@call __clean_debug.bat
@call cd..

@call cd Sound
@call __clean_debug.bat
@call cd..

@call cd SpeedControl
@call __clean_debug.bat
@call cd..

@call cd wxWidgetsGL
@call __clean_debug.bat
@call cd..

@call cd __DebugOutput
@call __clean_debug.bat
@call cd..

@call cd __ReleaseOutput
@call __clean_debug.bat
@call cd..