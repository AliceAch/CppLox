include(FetchContent)

#=====================================
# Helper functions and macros
#=====================================

# Adds external dependency into a project
function(add_external_dependency dependency)
  add_subdirectory(${${dependency}_SOURCE_DIR} ${${dependency}_BINARY_DIR})
endfunction()

#=====================================
# Options and initial settings
#=====================================

message(STATUS "Fetching third party libraries")
option(LINK_DEPS_STATIC CACHE ON)

set(FMT_VERSION 7.1.3)

if (NOT LINK_DEPS_STATIC)
  set(BUILD_SHARED_LIBS ON)
else()
  set(BUILD_SHARED_LIBS OFF)
endif()

#=====================================
# FetchContent declarations
#=====================================
FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt"
  GIT_TAG 7bdf0628b1276379886c7f6dda2cef2b3b374f0b # 7.1.3
)
FetchContent_MakeAvailable(fmt)