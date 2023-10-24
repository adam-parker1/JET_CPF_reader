# Plugins to build

# Dependencies
set ( CMAKE_CXX_STANDARD 17 ) 
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17" )
set ( CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=c++17" )
set ( CMAKE_CXX_EXTENSIONS ON )

set( CONFIGS
    JET_CPF_reader.cfg
)

add_subdirectory(curlpp)

include( plugins )
uda_plugin(
    NAME JET_CPF
    ENTRY_FUNC JETCPFReader
    DESCRIPTION "Plugin to read JET CPF data"
    EXAMPLE "JETCPFReader::get()"
    LIBNAME JET_CPF_reader
    SOURCES JET_CPF_reader.cpp
    CONFIG_FILE ${CONFIGS}
    EXTRA_INCLUDE_DIRS
      ${UDA_CLIENT_INCLUDE_DIRS}
      curlpp/include
      ext_include
    EXTRA_LINK_DIRS
      ${UDA_CLIENT_LIBRARY_DIRS}
    EXTRA_LINK_LIBS
      ${UDA_CLIENT_LIBRARIES}
      uda_cpp
      curlpp
)
