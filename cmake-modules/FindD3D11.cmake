
########## Modified from Source: https://github.com/microsoft/DirectXShaderCompiler/blob/master/cmake/modules/FindD3D12.cmake##########
#### Modifications: 
#       Explicitly used for finding d3d11 libs
#       Only search for Win x64 libs. 
#       Do not look for 32bit and arm based libs. 

# Find the win10 SDK path.
if ("$ENV{WIN11_SDK_PATH}$ENV{WIN10_SDK_VERSION}" STREQUAL "" )
  get_filename_component(WIN11_SDK_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0;InstallationFolder]" ABSOLUTE CACHE)
  get_filename_component(TEMP_WIN10_SDK_VERSION "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0;ProductVersion]" ABSOLUTE CACHE)
  get_filename_component(WIN10_SDK_VERSION ${TEMP_WIN10_SDK_VERSION} NAME)
elseif(TRUE)
  set (WIN11_SDK_PATH $ENV{WIN11_SDK_PATH})
  set (WIN10_SDK_VERSION $ENV{WIN10_SDK_VERSION})
endif ("$ENV{WIN11_SDK_PATH}$ENV{WIN10_SDK_VERSION}" STREQUAL "" )

# WIN11_SDK_PATH will be something like C:\Program Files (x86)\Windows Kits\10
# WIN10_SDK_VERSION will be something like 10.0.14393 or 10.0.14393.0; we need the
# one that matches the directory name.

if (IS_DIRECTORY "${WIN11_SDK_PATH}/Include/${WIN10_SDK_VERSION}.0")
  set(WIN10_SDK_VERSION "${WIN10_SDK_VERSION}.0")
endif (IS_DIRECTORY "${WIN11_SDK_PATH}/Include/${WIN10_SDK_VERSION}.0")


# Find the d3d11 and dxgi include path, it will typically look something like this.
# C:\Program Files (x86)\Windows Kits\10\Include\10.0.10586.0\um\d3d11.h
# C:\Program Files (x86)\Windows Kits\10\Include\10.0.10586.0\shared\dxgi1_4.h
find_path(D3D11_INCLUDE_DIR    # Set variable D3D11_INCLUDE_DIR
          d3d11.h                # Find a path with d3d11.h
          HINTS "${WIN11_SDK_PATH}/Include/${WIN10_SDK_VERSION}/um"
          DOC "path to WIN10 SDK header files"
          HINTS
          )

find_path(DXGI_INCLUDE_DIR    # Set variable DXGI_INCLUDE_DIR
          dxgi1_4.h           # Find a path with dxgi1_4.h
          HINTS "${WIN11_SDK_PATH}/Include/${WIN10_SDK_VERSION}/shared"
          DOC "path to WIN10 SDK header files"
          HINTS
          )

find_library(D3D11_LIBRARY NAMES d3d11.lib
               HINTS ${WIN11_SDK_PATH}/Lib/${WIN10_SDK_VERSION}/um/x64 )



find_library(DXGI_LIBRARY NAMES dxgi.lib
               HINTS ${WIN11_SDK_PATH}/Lib/${WIN10_SDK_VERSION}/um/x64 )


set(D3D11_LIBRARIES ${D3D11_LIBRARY} ${DXGI_LIBRARY})
set(D3D11_INCLUDE_DIRS ${D3D11_INCLUDE_DIR} ${DXGI_INCLUDE_DIR} ${D3DX12_INCLUDE_DIR})


include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set D3D11_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(D3D11  DEFAULT_MSG
                                  D3D11_INCLUDE_DIRS D3D11_LIBRARIES)

mark_as_advanced(D3D11_INCLUDE_DIRS D3D11_LIBRARIES)