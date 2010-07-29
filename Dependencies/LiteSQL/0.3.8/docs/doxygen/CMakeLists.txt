set(DOXYGEN_SKIP_DOT true)
include(FindDoxygen)


if(DOXYGEN_FOUND)
	get_source_file_property(DOXYFILE_IN doxygen.conf.in LOCATION)
	get_filename_component(srcdir ${DOXYFILE_IN} PATH)

	set(PACKAGE_VERSION ${PROJECT_VERSION})
	set(DOXYGEN_OUTPUT_DIR ..)
	configure_file(doxygen.conf.in ${srcdir}/doxygen.conf @ONLY)
  
  	add_custom_target(	doc 
				${DOXYGEN_EXECUTABLE} doxygen.conf 
				WORKING_DIRECTORY ${srcdir})
install(DIRECTORY ${srcdir}/../html 
	DESTINATION doc 
	COMPONENT "Documentation" 
	PATTERN ".svn" EXCLUDE)
else(DOXYGEN_FOUND)
  message("Doxygen not found, no documentation is generated")
endif(DOXYGEN_FOUND)
