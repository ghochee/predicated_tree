find_package(Doxygen)
find_package(Sphinx)
find_package(Doxyrest)

if (NOT (Doxygen_FOUND AND Sphinx_FOUND AND Doxyrest_FOUND))
    message(WARNING "Documentation generator tools not found. Skipping.")
    return()
endif()

get_filename_component(DOXYREST_BINDIR ${DOXYREST_EXECUTABLE} DIRECTORY)
set(DOXYREST_ROOT ${DOXYREST_BINDIR}/../../../../doxyrest)

set(DOCUMENTATION_OVERVIEW_PAGE
    ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md)
set(DOCUMENTATION_INSTALLATION_PAGE
    ${CMAKE_CURRENT_SOURCE_DIR}/installation.md)
set(DOCUMENTATION_TUTORIAL_PAGE
    ${CMAKE_CURRENT_SOURCE_DIR}/tutorial.md)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/conf.py
    ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper.sh
    ${CMAKE_CURRENT_BINARY_DIR}/wrapper.sh @ONLY)

set(DOXYGEN_CASE_SENSE_NAMES NO)
set(DOXYGEN_DOT_MULTI_TARGETS NO)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_HAVE_DOT NO)
set(DOXYGEN_MULTILINE_CPP_IS_BRIEF YES)
set(DOXYGEN_PROJECT_NAME "Predicated Tree")
set(DOXYGEN_QUIET YES)
set(DOXYGEN_XML_OUTPUT _xml)
set(DOXYGEN_WARNINGS NO)

# TODO(ghochee): This following option is not picked up by cmake correctly.
set(DOXYGEN_CLANG_ASSISTED_PARSING YES)

doxygen_add_docs(doxygen_docs
    ${CMAKE_SOURCE_DIR}/include
    ${DOCUMENTATION_OVERVIEW_PAGE}
    ${DOCUMENTATION_INSTALLATION_PAGE}
    ${DOCUMENTATION_TUTORIAL_PAGE}
    COMMENT "Generating API documentation with doxygen")

add_custom_target(doxyrest_docs
    ${DOXYREST_EXECUTABLE}
        -c ${CMAKE_CURRENT_SOURCE_DIR}/doxyrest-config.lua
        -F ${DOXYREST_ROOT}/frame/cfamily
        -F ${DOXYREST_ROOT}/frame/common
    DEPENDS doxygen_docs)

# Call the sphinx-build command with `wrapper.sh` to convert argument to
# environment variable. Setting environment variable directly here (via
# set(ENV{...} ...)) does not work because cmake environment is not passed to
# subshell. More information in wrapper.sh.
add_custom_target(docs
    ${CMAKE_CURRENT_BINARY_DIR}/wrapper.sh 
    ${DOXYREST_ROOT}/sphinx 
    ${SPHINX_EXECUTABLE}
        -c ${CMAKE_CURRENT_BINARY_DIR}
        -q -b html
        _rst/ html/
    DEPENDS doxyrest_docs)
