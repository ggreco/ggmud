cmake_minimum_required (VERSION 2.6)

# this must be done before anything else...
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")

project(GGMud)
set(VERSION "0.9.5")

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
pkg_check_modules(GTHREAD REQUIRED gthread-2.0)

#set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_BUILD_TYPE distribution)
SET(CMAKE_C_FLAGS_DISTRIBUTION "-O3")

# definitions
add_definitions("-DTELNET_SUPPORT -DWITH_LUA -DENABLE_MCCP")

# necessario per l'icona
if (WIN32)
    ENABLE_LANGUAGE(RC)
    set(ADDITIONAL_SOURCES icon.rc)
    set(ADDITIONAL_LIBS ws2_32)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mms-bitfields")
else ()
    if (APPLE)
        #        set (CMAKE_C_FLAGS "-m32")
        add_definitions("-DGTKQUARTZ")
        message(STATUS "OSX compilation detected, gtk libraries: ${GTK2_LIBRARIES}.")
        configure_file(Info.plist.in "${PROJECT_BINARY_DIR}/Info.plist")        
        set(ADDITIONAL_SOURCES gtk-mac-menu.m)
        set(ADDITIONAL_LIBS ${CARBON_LIBRARY} ${ADDITIONAL_LIBS} "objc")
    else()
        add_definitions("-DENABLE_BINRELOC -DBR_PTHREADS=0")
        set(ADDITIONAL_LIBS dl m)
    endif()
# kludge necessario perche' cmake al momento non sa della separazione di gdk-pixbuf
    find_path(GDK_PIXBUF_INCLUDE_DIRS NAMES gdk-pixbuf/gdk-pixbuf.h PATHS
            /usr/include/gdk-pixbuf-2.0
            /usr/include)
    if (GDK_PIXBUF_INCLUDE_DIRS)
         set(GTK2_INCLUDE_DIRS "${GDK_PIXBUF_INCLUDE_DIRS}" "${GTK2_INCLUDE_DIRS}")
# Remove duplicates when cmake will be fixed
         list(REMOVE_DUPLICATES GTK2_INCLUDE_DIRS)
    endif (GDK_PIXBUF_INCLUDE_DIRS)
endif ()

if (GTK2_LIBRARY_DIRS)
    link_directories("${GTK2_LIBRARY_DIRS}")
endif()

include_directories(tt lua zlib sq3 httpfetch ${GTK2_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
# configure_file(version.h.in "${PROJECT_BINARY_DIR}/inc/version.h")

add_subdirectory(tt)
add_subdirectory(lua)
add_subdirectory(zlib)
add_subdirectory(sq3)
add_subdirectory(httpfetch)

add_executable(ggmud WIN32 alias.c ansi.c font.c fileopen.c help.c history.c log.c logviewer.c
         macro.c net.c preferences.c ggmud.c triggers.c window.c wizard.c
         variables.c highlights.c gags.c timers.c complete.c prefix.c
         telnet.c lua.c interface.c callbacks.c support.c socks.c pixmaps.c
		 buttons.c autoupdate.c msp.c chat.c sub.c
                           ${ADDITIONAL_SOURCES}
              )

target_link_libraries(ggmud tt lua sq3 httpfetch zlib ${GTK2_LIBRARIES} ${ADDITIONAL_LIBS} ${GTHREAD_LIBRARIES})


install(TARGETS ggmud DESTINATION bin)
install(FILES gg_help.txt DESTINATION bin)
