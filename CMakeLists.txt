cmake_minimum_required (VERSION 2.8.4)

set(application_name "juci")
set(library_name "libjuci")

project (${application_name})

add_subdirectory("src")

find_program(XDG_DESKTOP_MENU_EXECUTABLE xdg-desktop-menu)
if(XDG_DESKTOP_MENU_EXECUTABLE)
  file(MAKE_DIRECTORY "/usr/share/desktop-directories") #Workaround for https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=730621
  install(CODE "execute_process(COMMAND ${XDG_DESKTOP_MENU_EXECUTABLE} install --novendor share/juci.desktop)")
endif()
