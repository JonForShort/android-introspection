cmake_minimum_required (VERSION 3.10.2)

set (boost-modules --with-program_options)

add_custom_target (boost
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source
  COMMAND ./bootstrap.sh --prefix=${DIR_PROJECT_OUT}
  COMMAND ./b2 link=static cxxflags=-fPIC linkflags=-pie ${boost-modules} install)
