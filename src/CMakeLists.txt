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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMSYS_PROCESS_USE_SH -DJUCI_CMAKE_INSTALL_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\"")
endif()

find_package(LibClang REQUIRED)

find_package(Boost 1.54 COMPONENTS thread log regex system filesystem REQUIRED)
find_package(ASPELL REQUIRED)
find_package(PythonLibs 3 REQUIRED)
set(PYBIND11_INCLUDE_DIR ../pybind11/include)
set(LIBCLANGMM_INCLUDE_DIR ../libclangmm/src)
set(TINY_PROCESS_INCLUDE_DIR ../tiny-process-library)

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
   ${TINY_PROCESS_INCLUDE_DIR}
)

set(global_libraries
  ${LIBCLANG_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTKSVMM_LIBRARIES}
  ${Boost_LIBRARIES}
  ${ASPELL_LIBRARIES}
  ${PYTHON_LIBRARIES}
)

set(application_files
    cmake.cc
    cmake.h
    config.cc
    config.h
    dialogs.cc
    dialogs.h
    directories.cc
    directories.h
    entrybox.cc
    entrybox.h
    files.h
    filesystem.cc
    filesystem.h
    juci.cc
    juci.h
    logging.h
    main.cc
    menu.cc
    menu.h
    notebook.cc
    notebook.h
    python_api.cc
    python_api.h
    python_interpreter.cc
    python_interpreter.h
    source_python.h
    source_python.cc
    selectiondialog.cc
    selectiondialog.h
    source.cc
    source.h
    source_clang.cc
    source_clang.h
    terminal.cc
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
    ../libclangmm/src/Diagnostic.cc
    ../libclangmm/src/Index.cc
    ../libclangmm/src/SourceLocation.cc
    ../libclangmm/src/SourceRange.cc
    ../libclangmm/src/Token.cc
    ../libclangmm/src/Tokens.cc
    ../libclangmm/src/TranslationUnit.cc    
    ../libclangmm/src/Utility.cc
    
    ../tiny-process-library/process.cpp)

if(MSYS)
  list(APPEND application_files dialogs_unix.cc ../tiny-process-library/process_win.cpp)
else()
  list(APPEND application_files dialogs_unix.cc ../tiny-process-library/process_unix.cpp)
endif()

include_directories(${global_includes})
add_executable(${application_name} ${application_files})
target_link_libraries(${application_name} ${global_libraries})

install(TARGETS ${application_name}
  RUNTIME DESTINATION bin
)
