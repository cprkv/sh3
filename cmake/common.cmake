set(vy_target_architecture ${CMAKE_C_COMPILER_ARCHITECTURE_ID} CACHE INTERNAL "vy_target_architecture")
message(VERBOSE "vy_target_architecture: ${vy_target_architecture}")


include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_OUTPUT)

if(vy_target_architecture STREQUAL "")
  message(FATAL_ERROR "no target architecture")
else()
  message("vy_target_architecture: ${vy_target_architecture}")
endif()


function(vy_set_target_output_variant target suffix variant)
  if(variant STREQUAL "")
    set_target_properties(${target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${VY_BIN}/${suffix}
      PDB_OUTPUT_DIRECTORY ${VY_BIN}/${suffix}
      LIBRARY_OUTPUT_DIRECTORY ${VY_BIN}/${suffix}/lib
      ARCHIVE_OUTPUT_DIRECTORY ${VY_BIN}/${suffix}/lib
    )
  else()
    set_target_properties(${target} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${variant} ${VY_BIN}/${suffix}
      PDB_OUTPUT_DIRECTORY_${variant} ${VY_BIN}/${suffix}
      LIBRARY_OUTPUT_DIRECTORY_${variant} ${VY_BIN}/${suffix}/lib
      ARCHIVE_OUTPUT_DIRECTORY_${variant} ${VY_BIN}/${suffix}/lib
    )
  endif()
endfunction()


macro(vy_set_target_output_name target)
  vy_set_target_output_variant(${target} "debug" "")
  vy_set_target_output_variant(${target} "debug" "DEBUG")
  vy_set_target_output_variant(${target} "release" "RELEASE")
  vy_set_target_output_variant(${target} "release" "RELWITHDEBINFO")
  vy_set_target_output_variant(${target} "release" "MINSIZEREL")
endmacro()


function(vy_configure_compiler target)
  set(ipo_enable "$<AND:$<NOT:$<CONFIG:Debug>>:$<BOOL:${IPO_SUPPORTED}>>")
  
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
    INTERPROCEDURAL_OPTIMIZATION $<IF:${ipo_enable},ON,OFF>
    # MSVC_DEBUG_INFORMATION_FORMAT EditAndContinue
  )
  # $<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Debug>>:/analyze>
  # -fanalyzer

  set(is_msvc "$<CXX_COMPILER_ID:MSVC>")
  #set(is_debug "$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Debug>>")
  set(is_debug "$<CONFIG:Debug>")
  set(is_release "$<NOT:${is_debug}>")
  set(debug_msvc "$<AND:${is_msvc},${is_debug}>")
  set(release_msvc "$<AND:${is_msvc},${is_release}>")
  set(is_gnu_like "$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>")

  set(setup_gnu_warnings

    # disable warnings
    -Wno-c++98-compat -Wno-c++98-compat-pedantic
    -Wno-pre-c++14-compat -Wno-c++14-compat
    -Wno-pre-c++17-compat
    -Wno-pre-c++20-compat
    -Wno-extra-semi -Wno-extra-semi-stmt
    -Wno-language-extension-token
    -Wno-ctad-maybe-unsupported
    -Wno-unsafe-buffer-usage # we dont do rust here

    # not errors, but still warnings
    #-Wno-error=switch-enum
    #-Wno-error=shadow-uncaptured-local
    -Wno-switch-enum
    -Wno-shadow-uncaptured-local
  )

  target_compile_options(${PROJECT_NAME} PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU>:-fanalyzer>
    $<${is_gnu_like}:-Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror ${setup_gnu_warnings}>
    #$<${debug_msvc}:/ZI>
    $<${release_msvc}:/guard:cf>
  )
  target_link_options(${PROJECT_NAME} PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/CETCOMPAT>
    $<$<CXX_COMPILER_ID:GNU>:-fanalyzer>
    $<${release_msvc}:/guard:cf>
  )
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<${is_debug}:_DEBUG>
  )
endfunction()


macro(vy_set_target_output)
  vy_set_target_output_name(${PROJECT_NAME})
endmacro()


macro(vy_set_sources)
  file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.hpp *.cs)
  target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
  message("target: ${PROJECT_NAME} source files: ${SOURCE_FILES}")
endmacro()


function(vy_get_winsdk_path WINSDK_PATH_OUTPUT WINSDK_VERSION_OUTPUT)
  if(";${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION};$ENV{UCRTVersion};$ENV{WindowsSDKVersion};" MATCHES [=[;(10\.[0-9.]+)[;\]]=])
    set(windows_kits_version "${CMAKE_MATCH_1}")
  else()
    message(FATAL_ERROR "can't determine windows sdk version")
  endif()

  message("windows_kits_version: ${windows_kits_version}")

  get_filename_component(windows_kits_dir
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE
  )
  set(programfilesx86 "ProgramFiles(x86)")

  # message("SEARCHING FOR Lib/${windows_kits_version}/um/${vy_target_architecture}/Version.Lib in:")
  # message("  $ENV{CMAKE_WINDOWS_KITS_10_DIR}")
  # message("  ${windows_kits_dir}")
  # message("  $ENV{ProgramFiles}/Windows Kits/10")
  # message("  $ENV{${programfilesx86}}/Windows Kits/10")

  find_path(WINDOWS_KITS_DIR
    NAMES
      Lib/${windows_kits_version}/um/${vy_target_architecture}/Version.Lib
    PATHS
      $ENV{CMAKE_WINDOWS_KITS_10_DIR}
      "${windows_kits_dir}"
      "$ENV{ProgramFiles}/Windows Kits/10"
      "$ENV{${programfilesx86}}/Windows Kits/10"
    REQUIRED
  )

  set(${WINSDK_PATH_OUTPUT} ${WINDOWS_KITS_DIR} PARENT_SCOPE)
  set(${WINSDK_VERSION_OUTPUT} ${windows_kits_version} PARENT_SCOPE)
endfunction()


# example: vy_link_dx_libraries(${PROJECT_NAME} PUBLIC d3d11 dxgi dxguid)
function(vy_link_dx_libraries target access)
  if(NOT ARGN)
    message(FATAL_ERROR "no dx libraries to link provided")
  endif()

  vy_get_winsdk_path(WINDOWS_SDK_PATH WINDOWS_SDK_VERSION)

  find_program(vy_nodejs
    NAMES node nodejs
    HINTS $ENV{NODE_DIR}
    PATH_SUFFIXES bin
    REQUIRED
  )
  execute_process(COMMAND ${vy_nodejs}
    ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/dx.js
      ${WINDOWS_SDK_PATH}
      ${WINDOWS_SDK_VERSION}
      ${vy_target_architecture}
      ${ARGN}
    OUTPUT_VARIABLE dx_output
    ERROR_VARIABLE dx_output_error
    RESULT_VARIABLE dx_exit_code
  )

  list(LENGTH dx_output dx_output_length)

  if(NOT dx_exit_code EQUAL "0" OR NOT dx_output_length EQUAL "2")
    message("-- dx.js output: \n${dx_output}")
    message("-- dx.js output length: \n${dx_output_length}")
    message("-- dx.js logs: \n${dx_output_error}")
    message(FATAL_ERROR "process ended with bad exit code or invalid output")
  endif()

  if(NOT dx_output_error STREQUAL "")
    message("-- dx.js logs: \n${dx_output_error}--")
  endif()

  list(GET dx_output 0 dx_libs_dir)
  list(GET dx_output 1 dx_include_dir)

  message("directx library dir: '${dx_libs_dir}'")
  message("directx include dir: '${dx_include_dir}'")

  target_include_directories(${target} ${access} ${dx_include_dir})
  target_link_directories(${target} ${access} ${dx_libs_dir})
  target_link_libraries(${target} ${access} ${ARGN})
endfunction()


function(vy_copy_deps target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
      $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${target}>>,copy_if_different,true>
      $<TARGET_RUNTIME_DLLS:${target}>
      $<TARGET_FILE_DIR:${target}>
    COMMAND_EXPAND_LISTS
  )
endfunction()
