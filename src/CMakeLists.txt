add_definitions(-DBOOST_LOG_DYN_LINK)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -pthread -Wall -Wno-reorder")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup")
  link_directories(/usr/local/lib /usr/local/opt/gettext/lib)
  include_directories(/usr/local/opt/gettext/include)
  set(CMAKE_MACOSX_RPATH 1)
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/opt/X11/lib/pkgconfig")
endif()

if(UNIX) #Checking if compiling on Ubuntu that has a buggy menu system
  find_program(LSB_RELEASE_BIN lsb_release)
  if(LSB_RELEASE_BIN)
    execute_process(COMMAND ${LSB_RELEASE_BIN} -is
      OUTPUT_VARIABLE DISTRIBUTION OUTPUT_STRIP_TRAILING_WHITESPACE)
    if((DISTRIBUTION STREQUAL Ubuntu) OR (DISTRIBUTION STREQUAL LinuxMint))
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJUCI_UBUNTU_BUGGED_MENU")
    endif()
  endif()
endif()

if(MSYS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJUCI_CMAKE_INSTALL_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"")
endif()

find_package(LibClang REQUIRED)
find_package(Boost 1.55 COMPONENTS thread log system filesystem REQUIRED)
find_package(ASPELL REQUIRED)
find_package(PythonLibs REQUIRED)
set(PYBIND11_INCLUDE_DIR "../pybind11/include")
set(LIBCLANGMM_INCLUDE_DIR ../libclangmm/src)

include(FindPkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(GTKSVMM gtksourceviewmm-3.0 REQUIRED)

set(global_includes
   ${Boost_INCLUDE_DIRS}
   ${GTKMM_INCLUDE_DIRS}
   ${GTKSVMM_INCLUDE_DIRS}
   ${LIBCLANG_INCLUDE_DIRS}
   ${LIBCLANGMM_INCLUDE_DIR}
   ${ASPELL_INCLUDE_DIR}
   ${PYTHON_INCLUDE_DIRS}
   ${PYBIND11_INCLUDE_DIR}
)

set(global_libraries
  ${LIBCLANG_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTKSVMM_LIBRARIES}
  ${Boost_LIBRARIES}
  ${ASPELL_LIBRARIES}
  ${PYTHON_LIBRARIES}
)

set(library_files
    api.cc
    api.h
    python_api.cc
    cmake.cc
    cmake.h
    config.cc
    config.h
    dialogs.h
    directories.cc
    directories.h
    entrybox.cc
    entrybox.h
    filesystem.cc
    filesystem.h
    juci.cc
    juci.h
    logging.h
    menu.cc
    menu.h
    notebook.cc
    notebook.h
    selectiondialog.cc
    selectiondialog.h
    singletons.cc
    singletons.h
    source.cc
    source.h
    source_clang.cc
    source_clang.h
    terminal.h
    tooltips.cc
    tooltips.h
    window.cc
    window.h
    
    ../libclangmm/src/CodeCompleteResults.cc
    ../libclangmm/src/CompilationDatabase.cc
    ../libclangmm/src/CompileCommand.cc     
    ../libclangmm/src/CompileCommands.cc    
    ../libclangmm/src/CompletionString.cc   
    ../libclangmm/src/Cursor.cc
    ../libclangmm/src/Diagnostic.cc
    ../libclangmm/src/Index.cc
    ../libclangmm/src/SourceLocation.cc
    ../libclangmm/src/SourceRange.cc
    ../libclangmm/src/Token.cc
    ../libclangmm/src/Tokens.cc
    ../libclangmm/src/TranslationUnit.cc
    ../libclangmm/src/Utility.cc
)

if(MSYS)
  list(APPEND library_files terminal_win.cc dialogs_win.cc)
else(MSYS)
  list(APPEND library_files terminal.cc dialogs.cc)
endif(MSYS)

add_executable(${application_name} main.cc)
add_library(${library_name} SHARED ${library_files})

include_directories(${global_includes})
target_link_libraries(${library_name} ${global_libraries})
target_link_libraries(${application_name} ${library_name})
set_target_properties(${library_name} PROPERTIES PREFIX "")

