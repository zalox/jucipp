cmake_minimum_required (VERSION 2.8.4)

set(application_name "juci")

project (${application_name})

add_subdirectory("src")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux|FreeBSD|OpenBSD|NetBSD")
    install(FILES "${CMAKE_SOURCE_DIR}/share/juci.desktop"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
endif()
