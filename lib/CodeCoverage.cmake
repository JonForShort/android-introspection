if ("$ENV{CODE_COVERAGE}" STREQUAL "1")
  set(MY_CXX_FLAGS_DEBUG "${MY_CXX_FLAGS_DEBUG} --coverage")
endif()
