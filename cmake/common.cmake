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

macro(vy_set_target_output)
  vy_set_target_output_variant(${PROJECT_NAME} "debug" "")
  vy_set_target_output_variant(${PROJECT_NAME} "debug" "DEBUG")
  vy_set_target_output_variant(${PROJECT_NAME} "release" "RELEASE")
  vy_set_target_output_variant(${PROJECT_NAME} "release" "RELWITHDEBINFO")
  vy_set_target_output_variant(${PROJECT_NAME} "release" "MINSIZEREL")
endmacro()

macro(vy_set_sources)
  file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.hpp *.cs)
  target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
  message("target: ${PROJECT_NAME} source files: ${SOURCE_FILES}")
endmacro()
