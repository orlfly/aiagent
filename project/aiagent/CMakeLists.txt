# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.
#
# Source files.
#

# cefsimple sources.
set(AIAGENT_SRCS
  BrowserView.cpp
  BrowserView.hpp
  BrowserClient.hpp
  LoadHandler.cpp 
  LoadHandler.hpp  
  RenderHandler.cpp  
  RenderHandler.hpp
  JSV8Handler.cpp  
  JSV8Handler.hpp
  CEFGLWindow.cpp
  CEFGLWindow.hpp
  GLCore.cpp
  GLCore.hpp
  GLWindow.cpp
  GLWindow.hpp
  WebStream.cpp
  WebStream.hpp
  common.h
  common.cpp
  sampling.h
  sampling.cpp
  grammar-parser.h
  grammar-parser.cpp
  main.cpp
  )
APPEND_PLATFORM_SOURCES(AIAGENT_SRCS)
source_group(aiagent FILES ${AIAGENT_SRCS})

set(AIAGENT_SRCS
  ${AIAGENT_SRCS}
  )


#
# Shared configuration.
#

# Target executable names.
set(CEF_TARGET "aiagent")
  
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()


#
# Linux configuration.
#

if(OS_LINUX)
  # Executable target.
  add_executable(${CEF_TARGET} ${AIAGENT_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  # Set rpath so that libraries can be placed next to the executable.
  set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
  if (EXISTS "${CEF_BINARY_DIR}/libminigbm.so")
    COPY_FILES("${CEF_TARGET}" "libminigbm.so" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  endif()

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()
