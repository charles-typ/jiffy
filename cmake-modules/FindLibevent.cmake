# Find LibEvent: an event notification library (http://libevent.org/)
#
# Usage:
# LIBEVENT_INCLUDE_DIR, where to find LibEvent headers
# LIBEVENT_LIBRARY, LibEvent libraries
# Libevent_FOUND, If false, do not try to use libevent

set(LIBEVENT_ROOT CACHE PATH "Root directory of libevent installation")
set(LibEvent_EXTRA_PREFIXES /usr/local /opt/local "$ENV{HOME}" ${LIBEVENT_ROOT})
foreach (prefix ${LibEvent_EXTRA_PREFIXES})
  list(APPEND LibEvent_INCLUDE_PATHS "${prefix}/include")
  list(APPEND LibEvent_LIBRARIES_PATHS "${prefix}/lib")
endforeach ()

# Looking for "event.h" will find the Platform SDK include dir on windows
# so we also look for a peer header like evhttp.h to get the right path
find_path(LIBEVENT_INCLUDE_DIR evhttp.h event.h PATHS ${LibEvent_INCLUDE_PATHS})

# "lib" prefix is needed on Windows in some cases
# newer versions of libevent use three libraries
find_library(LIBEVENT_LIBRARY NAMES event event_core event_extra libevent PATHS ${LibEvent_LIBRARIES_PATHS})

if (LIBEVENT_LIBRARY AND LIBEVENT_INCLUDE_DIR)
  set(Libevent_FOUND TRUE)
  set(LIBEVENT_LIBRARY ${LIBEVENT_LIBRARY})
else ()
  set(Libevent_FOUND FALSE)
endif ()

if (Libevent_FOUND)
  if (NOT Libevent_FIND_QUIETLY)
    message(STATUS "Found libevent: ${LIBEVENT_LIBRARY}")
  endif ()
else ()
  if (LibEvent_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find libevent.")
  endif ()
  message(STATUS "libevent NOT found.")
endif ()

mark_as_advanced(
        LIBEVENT_LIBRARY
        LIBEVENT_INCLUDE_DIR
)