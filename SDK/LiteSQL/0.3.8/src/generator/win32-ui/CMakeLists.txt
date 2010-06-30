set(litesql-gen-ui_SOURCES 
			LitesqlDocument.cpp LitesqlDocument.hpp
      LitesqlMDIChild.cpp
			LitesqlMDIChild.h
			main.cpp
			MainMDIfrm.cpp
			MainMDIfrm.h
			MDIFrameApp.cpp
			MDIFrameApp.h
			resource.h
			Resource.rc
			Text.cpp
			Text.h
			)


add_executable(litesql-gen-ui WIN32 ${litesql-gen-ui_SOURCES})

target_link_libraries (litesql-gen-ui litesql litesql-util model ${EXPAT_LIBRARIES} ${WIN32_PP_LIBRARIES} comctl32.lib)

if (MSVC)
  set_property(TARGET litesql-gen-ui APPEND PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS ${EXPAT_DEFINITIONS})
  set_property(TARGET litesql-gen-ui APPEND PROPERTY LINK_FLAGS_DEBUG)
endif(MSVC)

install(TARGETS litesql-gen-ui RUNTIME DESTINATION bin COMPONENT generator)

INCLUDE_DIRECTORIES(.. ../../library ${EXPAT_INCLUDE_DIRS} ${WIN32_PP_INCLUDE_DIRS})
