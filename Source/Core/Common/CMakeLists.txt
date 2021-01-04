add_library(common
  Analytics.cpp
  CDUtils.cpp
  ColorUtil.cpp
  CommonFuncs.cpp
  Config/Config.cpp
  Config/ConfigInfo.cpp
  Config/Layer.cpp
  Crypto/AES.cpp
  Crypto/bn.cpp
  Crypto/ec.cpp
  ENetUtil.cpp
  File.cpp
  FileSearch.cpp
  FileUtil.cpp
  GekkoDisassembler.cpp
  Hash.cpp
  HttpRequest.cpp
  IniFile.cpp
  JitRegister.cpp
  Logging/LogManager.cpp
  MathUtil.cpp
  MD5.cpp
  MemArena.cpp
  MemoryUtil.cpp
  MsgHandler.cpp
  NandPaths.cpp
  Network.cpp
  PcapFile.cpp
  PerformanceCounter.cpp
  Profiler.cpp
  QoSSession.cpp
  SDCardUtil.cpp
  SettingsHandler.cpp
  StringUtil.cpp
  SymbolDB.cpp
  SysConf.cpp
  Thread.cpp
  Timer.cpp
  TraversalClient.cpp
  UPnP.cpp
  Version.cpp
  x64ABI.cpp
  x64Emitter.cpp
)

target_link_libraries(common PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  ${CURL_LIBRARIES}
  enet
  ${MBEDTLS_LIBRARIES}
  ${VTUNE_LIBRARIES}
)

if (APPLE)
  target_link_libraries(common
  PRIVATE
    ${APPKIT_LIBRARY}
    ${COREFOUNDATION_LIBRARY}
    ${IOK_LIBRARY}
  )
endif()

if(ANDROID)
  target_sources(common PRIVATE
    Logging/ConsoleListenerDroid.cpp
  )
  target_link_libraries(common PRIVATE iconv)
elseif(WIN32)
  target_sources(common PRIVATE
    Logging/ConsoleListenerWin.cpp
  )
else()
  target_sources(common PRIVATE
    Logging/ConsoleListenerNix.cpp
  )
endif()

if(_M_ARM_64)
  target_sources(common PRIVATE
    Arm64Emitter.cpp
    ArmCPUDetect.cpp
    GenericFPURoundMode.cpp
  )
else()
  if(_M_X86) #X86
    target_sources(common PRIVATE
      x64FPURoundMode.cpp
      x64CPUDetect.cpp
    )
  else() # Generic
    target_sources(common PRIVATE
      GenericFPURoundMode.cpp
      GenericCPUDetect.cpp
    )
  endif()
endif()

# OpenGL Interface
target_sources(common PRIVATE
  GL/GLContext.cpp
  GL/GLContext.h
  GL/GLUtil.cpp
  GL/GLUtil.h
  GL/GLExtensions/GLExtensions.cpp
  GL/GLExtensions/GLExtensions.h
)

target_sources(common PRIVATE
      GL/GLInterface/EGL.cpp
      GL/GLInterface/EGL.h
      GL/GLInterface/EGLDRM.cpp
      GL/GLInterface/EGLDRM.h
)
target_include_directories(common PRIVATE ${EGL_INCLUDE_DIRS} ${LIBDRM_INCLUDE_DIRS} ${LIBGBM_INCLUDE_DIRS})
target_link_libraries(common PUBLIC ${EGL_LIBRARIES} ${LIBDRM_LIBRARIES} ${LIBGBM_LIBRARIES})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(common PUBLIC dl rt)
endif()

if(USE_UPNP)
  target_link_libraries(common PRIVATE Miniupnpc::miniupnpc)
endif()

if(OPROFILE_FOUND)
  target_link_libraries(common PRIVATE ${OPROFILE_LIBRARIES})
endif()

if(UNIX)
  # Posix networking code needs to be fixed for Windows
  add_executable(traversal_server TraversalServer.cpp)
  if(SYSTEMD_FOUND)
    target_link_libraries(traversal_server ${SYSTEMD_LIBRARIES})
  endif()
  if(HAIKU)
    target_link_libraries(traversal_server network)
  endif()
elseif(WIN32)
  target_link_libraries(common PRIVATE "-INCLUDE:enableCompatPatches")
endif()
