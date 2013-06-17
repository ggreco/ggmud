# GG: Potrebbe essere necessario cambiare i path e il nome degli eseguibili di seguito
#     a seconda della posizione del crosscompiler

# this one is important
SET(CMAKE_SYSTEM_NAME Windows)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i586-mingw32msvc-gcc) 
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
SET(CMAKE_RC_COMPILER i586-mingw32msvc-windres)
SET(CMAKE_RC_COMPILE_OBJECT "${CMAKE_RC_COMPILER} -O coff -I${CMAKE_CURRENT_SOURCE_DIR} <SOURCE> <OBJECT>")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)

