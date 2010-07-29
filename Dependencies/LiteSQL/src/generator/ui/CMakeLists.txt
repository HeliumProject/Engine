set(PROGRAM_visual_litesql "Visual-Litesql")
set(PROGRAM_visual_litesql_SOURCES  
		VisualLitesqlApp.cpp VisualLitesqlApp.h 
		MainFrame.cpp MainFrame.h
		LitesqlDocument.h LitesqlDocument.cpp
		LitesqlView.h LitesqlView.cpp
		GenerateView.h GenerateView.cpp
    ui.cpp ui.h
    LitesqlObjectPanel.cpp LitesqlObjectPanel.h
    LitesqlFieldPanel.cpp LitesqlFieldPanel.h
    LitesqlMethodPanel.cpp LitesqlMethodPanel.h
    LitesqlRelationPanel.cpp LitesqlRelationPanel.h
    LitesqlDatabasePanel.cpp LitesqlDatabasePanel.h
    LitesqlGeneratePanel.cpp LitesqlGeneratePanel.h
    ddx.cpp ddx.h
)
#		treelistctrl/include/wx/treelistctrl.h treelistctrl/src/treelistctrl.cpp


include(${CMAKE_ROOT}/Modules/FindwxWidgets.cmake)
include(${CMAKE_ROOT}/Modules/UsewxWidgets.cmake)

# Here you can define what libraries of wxWidgets you need for your
# application. You can figure out what libraries you need here;
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
set(wxWidgets_USE_LIBS base core adv)

# We need the Find package for wxWidgets to work
find_package(wxWidgets)

##---------------------------------------------------
## Actual config file starts here
##--------------------------------------------------- 
 
# Did we find wxWidgets ? This condition will fail
# for as long as the internal vars do not point to
# the proper wxWidgets configuration
if(wxWidgets_FOUND)

    # Include wxWidgets macros
    include(${wxWidgets_USE_FILE})

    # We define the include paths here, our minimal source dir is one, 
    # and also the include dirs defined by wxWidgets
    include_directories(${wxWidgets_INCLUDE_DIRS} 
                        ${EXPAT_INCLUDE_DIRS}
                        ../../library 
                        ../../generator
                        treelistctrl/include 
                        )
 
	# Here we define the executable 
	# the above paths and defines will be used in this build
	
	add_executable(${PROGRAM_visual_litesql} WIN32 MACOSX_BUNDLE ${PROGRAM_visual_litesql_SOURCES})

	#ui-access
	target_link_libraries(${PROGRAM_visual_litesql} ${wxWidgets_LIBRARIES} 
													litesql-util 
													model 
													${EXPAT_LIBRARIES})

	if (MSVC)
  		target_link_libraries(${PROGRAM_visual_litesql} gdiplus.lib)
	endif(MSVC)
	install(TARGETS ${PROGRAM_visual_litesql} 
				RUNTIME DESTINATION bin 
				BUNDLE DESTINATION /Applications)	

else(wxWidgets_FOUND)
    # For convenience. When we cannot continue, inform the user
    MESSAGE("wxWidgets not found!")
endif(wxWidgets_FOUND)

