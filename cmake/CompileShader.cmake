include(FindGLSLangValidator)

find_program(GLSLangValidator REQUIRED)

macro (compile_shader TARGET GLSL_FILE SPIRV_FILE)
  if (NOT ${ARGC} EQUAL 3)
    message(SEND_ERROR "COMPILE_SHADER takes 3 arguments")
  endif (NOT ${ARGC} EQUAL 3)
  add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${GLSLANGVALIDATOR_EXECUTABLE} -V -o "${SPIRV_FILE}" "${GLSL_FILE}"
  )
endmacro (compile_shader TARGET GLSL_FILE SPIRV_FILE)
