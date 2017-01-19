# GG: Potrebbe essere necessario cambiare i path e il nome degli eseguibili di seguito
#     a seconda della posizione del crosscompiler

# this one is important
SET(CMAKE_SYSTEM_NAME Darwin)

# specify the cross compiler
SET(CMAKE_C_COMPILER   i686-apple-darwin10-gcc) 
SET(CMAKE_CXX_COMPILER i686-apple-darwin10-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/lib/apple/SDKs/MacOSX10.5.sdk/usr)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)

