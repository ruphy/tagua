find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

if(NOT GETTEXT_MSGFMT_EXECUTABLE)
  message("NOTE: msgfmt not found. Translations will *not* be installed")
else(NOT GETTEXT_MSGFMT_EXECUTABLE)
  set(catalogname tagua) 
  
  file(GLOB PO_FILES *.po)
  
  foreach(_poFile ${PO_FILES})
    message("Found translation: ${_poFile}")
    get_filename_component(_lang ${_poFile} NAME_WE)
    set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
    
    add_custom_command(OUTPUT ${_gmoFile}
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
            DEPENDS ${_poFile})
    add_custom_target(translations ALL DEPENDS ${_gmoFile})
    install(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES/ RENAME ${catalogname}.mo)
  endforeach(_poFile ${PO_FILES})
endif(NOT GETTEXT_MSGFMT_EXECUTABLE)
