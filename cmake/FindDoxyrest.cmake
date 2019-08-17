find_program(DOXYREST_EXECUTABLE NAMES doxyrest
    PATH_SUFFIXES bin
    DOC "Doxyrest doxygen to reSt converter."
)
 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Doxyrest DEFAULT_MSG
    DOXYREST_EXECUTABLE
)
 
mark_as_advanced(DOXYREST_EXECUTABLE)
