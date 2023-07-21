set(BUILD_XAUDIO_WIN10 OFF CACHE BOOL "" FORCE)
set(BUILD_XAUDIO_REDIST OFF CACHE BOOL "" FORCE)
set (BUILD_DXIL_SHADERS OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/directxtk12)

set_target_properties(DirectXTK12 PROPERTIES FOLDER 3rdParty/directxtk12)