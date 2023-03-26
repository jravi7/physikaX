function(phi_add_asset_to_target PHI_TARGET)
    cmake_parse_arguments(
        PHI
        ""
        ""
        "ASSETS"
        ${ARGN}
    )

    SET(ASSET_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/assets)
    
    foreach(ASSET_FILE ${PHI_ASSETS})
        add_custom_command(TARGET ${PHI_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_FILE} $<TARGET_FILE_DIR:${PHI_TARGET}>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Copying asset ${ASSET_FILE}"
        )    
    endforeach()
    
    
endfunction()

############################################
# name: phi_add_exectuable
# Description: Wrapper for add_executable 
#              Invokes clang-format as a 
#              pre-build step.
############################################
function(phi_add_executable PHI_TARGET)
    cmake_parse_arguments(
        PHI
        "WIN32"
        ""
        "SOURCES;SHADERS"
        ${ARGN}
    )
    
    if(PHI_WIN32)
        add_executable(${PHI_TARGET} WIN32 ${PHI_SOURCES} ${PHI_UNPARSED_ARGUMENTS})
    else()
        add_executable(${PHI_TARGET} ${PHI_SOURCES} ${PHI_UNPARSED_ARGUMENTS})
    endif()


    if (PHI_SHADERS)
        phi_add_asset_to_target(${PHI_TARGET} ASSETS ${PHI_SHADERS})
    endif()

    # Run clang-format as pre-build event.    
    add_custom_command(
        TARGET ${PHI_TARGET}
        PRE_BUILD
        COMMAND clang-format -i -style=file ${PHI_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting target ${PHI_TARGET}"
    )
endfunction()

############################################
# name: phi_add_library
# Description: Wrapper for add_library 
#              Invokes clang-format as a 
#              pre-build step.
############################################
function(phi_add_library PHI_TARGET)
    cmake_parse_arguments(
        PHI
        "STATIC;SHARED"
        ""
        "SOURCES"
        ${ARGN}
    )
    
    if(PHI_STATIC)
        add_library(${PHI_TARGET} STATIC ${PHI_SOURCES} ${PHI_UNPARSED_ARGUMENTS})
    elseif(PHI_SHARED)
        add_library(${PHI_TARGET} SHARED ${PHI_SOURCES} ${PHI_UNPARSED_ARGUMENTS})
    else()
        add_executable(${PHI_TARGET} MODULE ${PHI_SOURCES} ${PHI_UNPARSED_ARGUMENTS})
    endif()

    # Run clang-format as pre-build event.    
    add_custom_command(
        TARGET ${PHI_TARGET}
        PRE_BUILD
        COMMAND clang-format -i -style=file ${PHI_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting target ${PHI_TARGET}"
    )
endfunction()

