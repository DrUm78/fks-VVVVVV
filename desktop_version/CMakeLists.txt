# CMake File for VVVVVV
# Written by Ethan "flibitijibibo" Lee

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(VVVVVV)

# Architecture Flags
IF(APPLE)
	# Wow, Apple is a huge jerk these days huh?
	SET(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
	LINK_DIRECTORIES(/usr/local/lib)
ENDIF()

# Compiler Flags
ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_DEFAULT=0 -DPHYSFS_SUPPORTS_ZIP=1)

# Executable Suffix
IF(APPLE)
	SET(CMAKE_EXECUTABLE_SUFFIX ".osx")
ELSEIF(WIN32)
	# Nothing!
ELSEIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	SET(CMAKE_EXECUTABLE_SUFFIX ".x86_64")
ELSE()
	SET(CMAKE_EXECUTABLE_SUFFIX ".x86")
ENDIF()

# Include Directories
INCLUDE_DIRECTORIES(src tinyxml physfs lodepng)

# Source Lists
SET(VVV_SRC
	src/BinaryBlob.cpp
	src/BlockV.cpp
	src/editor.cpp
	src/Ent.cpp
	src/Entity.cpp
	src/FileSystemUtils.cpp
	src/Finalclass.cpp
	src/Game.cpp
	src/Graphics.cpp
	src/GraphicsResources.cpp
	src/GraphicsUtil.cpp
	src/Input.cpp
	src/KeyPoll.cpp
	src/Labclass.cpp
	src/Logic.cpp
	src/Map.cpp
	src/Music.cpp
	src/Otherlevel.cpp
	src/preloader.cpp
	src/Screen.cpp
	src/Script.cpp
	src/Scripts.cpp
	src/SoundSystem.cpp
	src/Spacestation2.cpp
	src/TerminalScripts.cpp
	src/Textbox.cpp
	src/titlerender.cpp
	src/Tower.cpp
	src/UtilityClass.cpp
	src/WarpClass.cpp
	src/main.cpp
	src/SteamNetwork.c
)
SET(XML_SRC
	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)
SET(PFS_SRC
	physfs/physfs.c
	physfs/physfs_archiver_dir.c
	physfs/physfs_archiver_unpacked.c
	physfs/physfs_archiver_zip.c
	physfs/physfs_byteorder.c
	physfs/physfs_unicode.c
	physfs/physfs_platform_posix.c
	physfs/physfs_platform_unix.c
	physfs/physfs_platform_windows.c
)
IF(APPLE)
	# Are you noticing a pattern with this Apple crap yet?
	SET(PFS_SRC ${PFS_SRC} physfs/physfs_platform_apple.m)
ENDIF()
SET(PNG_SRC lodepng/lodepng.c)

# Executable information
IF(WIN32)
	ADD_EXECUTABLE(vvvvvv WIN32 ${VVV_SRC})
ELSE()
	ADD_EXECUTABLE(vvvvvv ${VVV_SRC})
ENDIF()

# Library information
ADD_LIBRARY(tinyxml-static STATIC ${XML_SRC})
ADD_LIBRARY(physfs-static STATIC ${PFS_SRC} ${PFSP_SRC})
ADD_LIBRARY(lodepng-static STATIC ${PNG_SRC})


set(OPTIMIZATIONS -O3 -flto -fomit-frame-pointer -fexpensive-optimizations -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard -ffast-math -funsafe-math-optimizations -mvectorize-with-neon-quad -ftree-vectorize)
target_compile_options(vvvvvv PUBLIC "$<$<CONFIG:RELEASE>:${OPTIMIZATIONS}>")


# Static Dependencies
TARGET_LINK_LIBRARIES(vvvvvv physfs-static tinyxml-static lodepng-static)

LINK_DIRECTORIES(/home/jack/Desktop/dev/FunKey-OS/SDK/output/host/arm-funkey-linux-musleabihf/sysroot/usr/bin/../../usr/lib)
INCLUDE_DIRECTORIES(/home/jack/Desktop/dev/FunKey-OS/SDK/output/host/arm-funkey-linux-musleabihf/sysroot/usr/bin/../../usr/include/SDL)
ADD_DEFINITIONS(-D_GNU_SOURCE=1 -D_REENTRANT)

target_link_libraries(vvvvvv SDL SDL_mixer SDL_sound pthread)
