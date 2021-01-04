add_executable(ishiiruka-nogui
  Platform.cpp
  Platform.h
  #PlatformHeadless.cpp
  MainNoGUI.cpp
)

if(ENABLE_X11 AND X11_FOUND)
  #target_sources(ishiiruka-nogui PRIVATE PlatformX11.cpp)
endif()

if(WIN32)
  #target_sources(ishiiruka-nogui PRIVATE PlatformWin32.cpp)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  #target_sources(ishiiruka-nogui PRIVATE PlatformFBDev.cpp)
  target_sources(ishiiruka-nogui PRIVATE PlatformDRM.cpp)
endif()

set_target_properties(ishiiruka-nogui PROPERTIES OUTPUT_NAME ishiiruka-emu-nogui)

target_link_libraries(ishiiruka-nogui
PRIVATE
  core
  uicommon
  cpp-optparse
)

set(CPACK_PACKAGE_EXECUTABLES ${CPACK_PACKAGE_EXECUTABLES} ishiiruka-nogui)
install(TARGETS ishiiruka-nogui RUNTIME DESTINATION ${bindir})

