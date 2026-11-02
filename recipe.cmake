function(_recipe_flatland_source)
	set(FLATLAND_REF "main")
	if(CL_VERSION_REQ)
		set(FLATLAND_REF "${CL_REQ_VERSION}")
	endif()

	cl_import_source(
		NAME flatland
		REPO https://github.com/gradylink/flatland.git
		REF ${FLATLAND_REF}
		OPTIONS "FLATLAND_BUILD_EXAMPLE OFF"
	)
endfunction()
