include_guard(GLOBAL)

cmake_policy(PUSH)
cmake_policy(SET CMP0054 NEW) # quoted if arguments
cmake_policy(SET CMP0057 NEW) # if IN_LIST

find_package(Doxygen QUIET OPTIONAL_COMPONENTS mscgen dia dot)

# Run doxygen
function(doxyfile_docs)
  cmake_parse_arguments(ARGS "ALL;USE_STAMP_FILE;LUALATEX" "WORKING_DIRECTORY;COMMENT;CONFIG_FILE" "" "${ARGN}")

  include(GNUInstallDirs)

  set(DOXYGEN_ALL_TARGETS "")

  if(NOT DOXYGEN_FOUND)
    add_custom_target(docs ${ALL_STRING} "${CMAKE_COMMAND}" -E true COMMENT "${Yellow}Doxygen is not found !!${Reset}")
    add_custom_target(doc-html ${ALL_STRING} "${CMAKE_COMMAND}" -E true COMMENT "${Yellow}Doxygen is not found !!${Reset}")
    add_custom_target(doc-latex ${ALL_STRING} "${CMAKE_COMMAND}" -E true COMMENT "${Yellow}Doxygen is not found !!${Reset}")
    return()
  endif()

  if(NOT ARGS_WORKING_DIRECTORY)
    set(ARGS_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()

  if(DEFINED DOXYGEN_INPUT)
    message(WARNING "DOXYGEN_INPUT is set but it will be ignored. Pass the files and directories directly to the doxygen_add_docs() command instead.")
  endif()
  set(DOXYGEN_INPUT "${ARGS_UNPARSED_ARGUMENTS}")

  # If not already defined, set some relevant defaults based on the
  # assumption that the documentation is for the whole project. Details
  # specified in the project() command will be used to populate a number of
  # these defaults.

  if(NOT DEFINED DOXYGEN_PROJECT_NAME)
    # The PROJECT_NAME tag is a single word (or a sequence of words
    # surrounded by double-quotes, unless you are using Doxywizard) that
    # should identify the project for which the documentation is generated.
    # This name is used in the title of most generated pages and in a few
    # other places. The default value is: My Project.
    set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
  endif()

  if(NOT DEFINED DOXYGEN_PROJECT_NUMBER)
    # The PROJECT_NUMBER tag can be used to enter a project or revision
    # number. This could be handy for archiving the generated documentation
    # or if some version control system is used.
    set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})
  endif()

  if(NOT DEFINED DOXYGEN_PROJECT_BRIEF)
    # Using the PROJECT_BRIEF tag one can provide an optional one line
    # description for a project that appears at the top of each page and
    # should give viewer a quick idea about the purpose of the project.
    # Keep the description short.
    set(DOXYGEN_PROJECT_BRIEF "${PROJECT_DESCRIPTION}")
  endif()

  if(NOT DEFINED DOXYGEN_RECURSIVE)
    # The RECURSIVE tag can be used to specify whether or not
    # subdirectories should be searched for input files as well. CMake
    # projects generally evolve to span multiple directories, so it makes
    # more sense for this to be on by default. Doxygen's default value
    # has this setting turned off, so we override it.
    set(DOXYGEN_RECURSIVE YES)
  endif()

  if(NOT DEFINED DOXYGEN_OUTPUT_DIRECTORY)
    # The OUTPUT_DIRECTORY tag is used to specify the (relative or
    # absolute) path into which the generated documentation will be
    # written. If a relative path is used, Doxygen will interpret it as
    # being relative to the location where doxygen was started, but we need
    # to run Doxygen in the source tree so that relative input paths work
    # intuitively. Therefore, we ensure that the output directory is always
    # an absolute path and if the project provided a relative path, we
    # treat it as relative to the current BINARY directory so that output
    # is not generated inside the source tree.
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
  elseif(NOT IS_ABSOLUTE "${DOXYGEN_OUTPUT_DIRECTORY}")
    get_filename_component(DOXYGEN_OUTPUT_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  if(NOT EXISTS "${DOXYGEN_OUTPUT_DIRECTORY}")
    file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}")
  endif()

  if(NOT DEFINED DOXYGEN_HAVE_DOT)
    # If you set the HAVE_DOT tag to YES then doxygen will assume the dot
    # tool is available from the path. This tool is part of Graphviz (see:
    # http://www.graphviz.org/), a graph visualization toolkit from AT&T
    # and Lucent Bell Labs. The other options in this section have no
    # effect if this option is set to NO.
    # Doxygen's default value is: NO.
    if(DOXYGEN_HAVE_DOT)
      set(DOXYGEN_HAVE_DOT "YES")
    else()
      set(DOXYGEN_HAVE_DOT "NO")
    endif()
  endif()

  if(NOT DEFINED DOXYGEN_DOT_MULTI_TARGETS)
    # Set the DOT_MULTI_TARGETS tag to YES to allow dot to generate
    # multiple output files in one run (i.e. multiple -o and -T options on
    # the command line). This makes dot run faster, but since only newer
    # versions of dot (>1.8.10) support this, Doxygen disables this feature
    # by default.
    # This tag requires that the tag HAVE_DOT is set to YES.
    set(DOXYGEN_DOT_MULTI_TARGETS YES)
  endif()

  if(NOT DEFINED DOXYGEN_GENERATE_LATEX OR NOT DOXYGEN_GENERATE_LATEX)
    # If the GENERATE_LATEX tag is set to YES, doxygen will generate LaTeX
    # output. We only want the HTML output enabled by default, so we turn
    # this off if the project hasn't specified it.
    set(DOXYGEN_GENERATE_LATEX NO)
  else()
    find_package(LATEX OPTIONAL_COMPONENTS PDFLATEX XELATEX LUALATEX BIBTEX BIBER MAKEINDEX XINDY DVIPS DVIPDF PS2PDF PDFTOPS LATEX2HTML HTLATEX)
    if(ARGS_LUALATEX AND LATEX_LUALATEX_FOUND)
      set(LATEX_GENERATED_EXTENSION "pdf")
      set(LATEX_EXECUTABLE ${LUALATEX_COMPILER})
      set(DOXYGEN_GENERATE_LATEX YES)
    elseif(LATEX_PDFLATEX_FOUND)
      set(LATEX_GENERATED_EXTENSION "pdf")
      set(LATEX_EXECUTABLE ${PDFLATEX_COMPILER})
      set(DOXYGEN_GENERATE_LATEX YES)
    elseif(LATEX_FOUND)
      set(LATEX_GENERATED_EXTENSION "dvi")
      set(LATEX_EXECUTABLE ${LATEX_COMPILER})
      set(DOXYGEN_GENERATE_LATEX YES)
    else()
      set(DOXYGEN_GENERATE_LATEX NO)
      message(WARNING "DOXYGEN_GENERATE_LATEX is set to ON but latex compilers are not found ! Disabling the pdf generation !")
    endif()
  endif()

  if(NOT DEFINED DOXYGEN_WARN_FORMAT)
    if(CMAKE_VS_MSBUILD_COMMAND OR CMAKE_VS_DEVENV_COMMAND)
      # The WARN_FORMAT tag determines the format of the warning messages
      # that doxygen can produce. The string should contain the $file,
      # $line and $text tags, which will be replaced by the file and line
      # number from which the warning originated and the warning text.
      # Optionally, the format may contain $version, which will be
      # replaced by the version of the file (if it could be obtained via
      # FILE_VERSION_FILTER).
      # Doxygen's default value is: $file:$line: $text
      set(DOXYGEN_WARN_FORMAT "$file($line) : $text ")
    endif()
  endif()

  if(DEFINED DOXYGEN_WARN_LOGFILE AND NOT IS_ABSOLUTE "${DOXYGEN_WARN_LOGFILE}")
    # The WARN_LOGFILE tag can be used to specify a file to which warning and error
    # messages should be written. If left blank the output is written to standard
    # error (stderr).
    get_filename_component(DOXYGEN_WARN_LOGFILE "${DOXYGEN_WARN_LOGFILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  # Any files from the INPUT that match any of the EXCLUDE_PATTERNS will be
  # excluded from the set of input files. We provide some additional patterns
  # to prevent commonly unwanted things from CMake builds being pulled in.
  #
  # Note that the wildcards are matched against the file with absolute path,
  # so to exclude all test directories for example use the pattern */test/*
  list(APPEND DOXYGEN_EXCLUDE_PATTERNS "*/.git/*" "*/.svn/*" "*/.hg/*" "*/CMakeFiles/*" "*/_CPack_Packages/*" "DartConfiguration.tcl" "CMakeLists.txt" "CMakeCache.txt")

  # Now bring in Doxgen's defaults for those things the project has not
  # already set and we have not provided above
  include("${CMAKE_BINARY_DIR}/CMakeDoxygenDefaults.cmake" OPTIONAL)

  if(NOT ARGS_COMMENT)
    set(ARGS_COMMENT "Generating API documentation with doxygen.")
  endif()

  # Cleanup built HTMLs on "make clean"
  # TODO Any other dirs?
  if(DOXYGEN_GENERATE_HTML)
    if(IS_ABSOLUTE "${DOXYGEN_HTML_OUTPUT}")
      set(args_clean_html_dir "${DOXYGEN_HTML_OUTPUT}")
    else()
      set(args_clean_html_dir "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_HTML_OUTPUT}")
    endif()
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${args_clean_html_dir}")
  endif()

  # Cleanup built HTMLs on "make clean"
  # TODO Any other dirs?
  if(DOXYGEN_GENERATE_LATEX)
    if(IS_ABSOLUTE "${DOXYGEN_LATEX_OUTPUT}")
      set(args_clean_latex_dir "${DOXYGEN_LATEX_OUTPUT}")
    else()
      set(args_clean_latex_dir "${DOXYGEN_OUTPUT_DIRECTORY}/${DOXYGEN_LATEX_OUTPUT}")
    endif()
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${args_clean_latex_dir}")
  endif()

  # Build up a list of files we can identify from the inputs so we can list
  # them as DEPENDS and SOURCES in the custom command/target (the latter
  # makes them display in IDEs). This must be done before we transform the
  # various DOXYGEN_... variables below because we need to process
  # DOXYGEN_INPUT as a list first.
  unset(_sources)
  foreach(item IN LISTS DOXYGEN_INPUT)
    get_filename_component(_abs_item "${item}" ABSOLUTE BASE_DIR "${_args_WORKING_DIRECTORY}")
    get_source_file_property(_isGenerated "${_abs_item}" GENERATED)
    if(_isGenerated OR (EXISTS "${_abs_item}" AND NOT IS_DIRECTORY "${_abs_item}" AND NOT IS_SYMLINK "${_abs_item}"))
      list(APPEND _sources "${_abs_item}")
    elseif(_args_USE_STAMP_FILE)
      message(FATAL_ERROR "Source does not exist or is not a file:\n"
              "    ${_abs_item}\n"
              "Only existing files may be specified when the "
              "USE_STAMP_FILE option is given.")
    endif()
  endforeach()

  # Transform known list type options into space separated strings.
  set(doxygen_list_options
      ABBREVIATE_BRIEF
      ALIASES
      CITE_BIB_FILES
      DIAFILE_DIRS
      DOTFILE_DIRS
      DOT_FONTPATH
      ENABLED_SECTIONS
      EXAMPLE_PATH
      EXAMPLE_PATTERNS
      EXCLUDE
      EXCLUDE_PATTERNS
      EXCLUDE_SYMBOLS
      EXPAND_AS_DEFINED
      EXTENSION_MAPPING
      EXTRA_PACKAGES
      EXTRA_SEARCH_MAPPINGS
      FILE_PATTERNS
      FILTER_PATTERNS
      FILTER_SOURCE_PATTERNS
      HTML_EXTRA_FILES
      HTML_EXTRA_STYLESHEET
      IGNORE_PREFIX
      IMAGE_PATH
      INCLUDE_FILE_PATTERNS
      INCLUDE_PATH
      INPUT
      LATEX_EXTRA_FILES
      LATEX_EXTRA_STYLESHEET
      MATHJAX_EXTENSIONS
      MSCFILE_DIRS
      PLANTUML_INCLUDE_PATH
      PREDEFINED
      QHP_CUST_FILTER_ATTRS
      QHP_SECT_FILTER_ATTRS
      STRIP_FROM_INC_PATH
      STRIP_FROM_PATH
      TAGFILES
      TCL_SUBST)
  foreach(item IN LISTS doxygen_list_options)
    doxygen_list_to_quoted_strings(DOXYGEN_${item})
  endforeach()

  # Transform known single value variables which may contain spaces, such as
  # paths or description strings.
  set(doxygen_quoted_options
      CHM_FILE
      DIA_PATH
      DOCBOOK_OUTPUT
      DOCSET_FEEDNAME
      DOCSET_PUBLISHER_NAME
      DOT_FONTNAME
      DOT_PATH
      EXTERNAL_SEARCH_ID
      FILE_VERSION_FILTER
      GENERATE_TAGFILE
      HHC_LOCATION
      HTML_FOOTER
      HTML_HEADER
      HTML_OUTPUT
      HTML_STYLESHEET
      INPUT_FILTER
      LATEX_FOOTER
      LATEX_HEADER
      LATEX_OUTPUT
      LAYOUT_FILE
      MAN_OUTPUT
      MAN_SUBDIR
      MATHJAX_CODEFILE
      MSCGEN_PATH
      OUTPUT_DIRECTORY
      PERL_PATH
      PLANTUML_JAR_PATH
      PROJECT_BRIEF
      PROJECT_LOGO
      PROJECT_NAME
      QCH_FILE
      QHG_LOCATION
      QHP_CUST_FILTER_NAME
      QHP_VIRTUAL_FOLDER
      RTF_EXTENSIONS_FILE
      RTF_OUTPUT
      RTF_STYLESHEET_FILE
      SEARCHDATA_FILE
      USE_MDFILE_AS_MAINPAGE
      WARN_FORMAT
      WARN_LOGFILE
      XML_OUTPUT)

  # Store the unmodified value of DOXYGEN_OUTPUT_DIRECTORY prior to invoking
  # doxygen_quote_value() below. This will mutate the string specifically for
  # consumption by Doxygen's config file, which we do not want when we use it
  # later in the custom target's commands.
  set(ORIGINAL_DOXYGEN_OUTPUT_DIR ${DOXYGEN_OUTPUT_DIRECTORY})

  foreach(item IN LISTS doxygen_quoted_options)
    doxygen_quote_value(DOXYGEN_${item})
  endforeach()

  if(NOT DEFINED ARGS_CONFIG_FILE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
      set(CONFIG_FILE_TO_CONFIGURE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
    else()
      set(CONFIG_FILE_TO_CONFIGURE "${CMAKE_BINARY_DIR}/CMakeDoxyfile.in")
    endif()
  else()
    set(CONFIG_FILE_TO_CONFIGURE "${ARGS_CONFIG_FILE}")
  endif()

  set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
  configure_file("${CONFIG_FILE_TO_CONFIGURE}" "${CONFIG_FILE}")

  if(ARGS_ALL)
    set(ALL_STRING "ALL")
  endif()

  if(DOXYGEN_GENERATE_LATEX)
    set(DOXYGEN_INPUT_LATEX "${ORIGINAL_DOXYGEN_OUTPUT_DIR}/${DOXYGEN_LATEX_OUTPUT}/refman.tex")
    set(DOXYGEN_OUTPUT_LATEX "${ORIGINAL_DOXYGEN_OUTPUT_DIR}/${DOXYGEN_LATEX_OUTPUT}/refman.${LATEX_GENERATED_EXTENSION}")
  endif()

  # Only create the stamp file if asked to. If we don't create it,
  # the target will always be considered out-of-date.
  if(ARGS_USE_STAMP_FILE)
    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.stamp")
    add_custom_command(
      OUTPUT ${STAMP_FILE} ${DOXYGEN_INPUT_LATEX} VERBATIM
      COMMAND ${CMAKE_COMMAND} -E make_directory ${ORIGINAL_DOXYGEN_OUTPUT_DIR}
      COMMAND "${DOXYGEN_EXECUTABLE}" "${CONFIG_FILE}"
      COMMAND ${CMAKE_COMMAND} -E touch ${STAMP_FILE}
      WORKING_DIRECTORY "${ARGS_WORKING_DIRECTORY}"
      DEPENDS "${CONFIG_FILE}" ${_sources}
      COMMENT "${ARGS_COMMENT}")
  else()
    add_custom_target(generate-doxygen ${ALL_STRING} VERBATIM
                      COMMAND ${CMAKE_COMMAND} -E make_directory ${ORIGINAL_DOXYGEN_OUTPUT_DIR}
                      COMMAND "${DOXYGEN_EXECUTABLE}" "${CONFIG_FILE}"
                      WORKING_DIRECTORY "${ARGS_WORKING_DIRECTORY}"
                      DEPENDS "${CONFIG_FILE}" ${_sources}
                      COMMENT "${ARGS_COMMENT}"
                      SOURCES ${_sources})
  endif()

  if(DOXYGEN_GENERATE_HTML)
    list(APPEND DOXYGEN_ALL_TARGETS "doc-html")
    if(ARGS_USE_STAMP_FILE)
      add_custom_target(doc-html ${ALL_STRING} DEPENDS ${STAMP_FILE} SOURCES ${_sources} COMMENT "Generating html documentation.")
    else()
      add_custom_target(doc-html ${ALL_STRING} DEPENDS generate-doxygen SOURCES ${_sources} COMMENT "Generating html documentation.")
    endif()
    install(DIRECTORY "${ORIGINAL_DOXYGEN_OUTPUT_DIR}/html" DESTINATION "${CMAKE_INSTALL_DOCDIR}" OPTIONAL)
  else()
    add_custom_target(doc-html "${CMAKE_COMMAND}" -E true COMMENT "${Yellow}HTML format not activated for doxygen !${Reset}")
  endif()

  if(DOXYGEN_GENERATE_LATEX)
    list(APPEND DOXYGEN_ALL_TARGETS "doc-latex")
    if(ARGS_USE_STAMP_FILE)
      #-interaction=batchmode
      add_custom_target(doc-latex ${ALL_STRING} DEPENDS "${STAMP_FILE}" SOURCES ${_sources}
                        COMMAND "${LATEX_EXECUTABLE}" -shell-escape -draftmode "${DOXYGEN_INPUT_LATEX}"
                        COMMAND "${LATEX_EXECUTABLE}" -shell-escape "${DOXYGEN_INPUT_LATEX}"
                        WORKING_DIRECTORY "${ORIGINAL_DOXYGEN_OUTPUT_DIR}/${DOXYGEN_LATEX_OUTPUT}"
                        BYPRODUCTS "${DOXYGEN_OUTPUT_LATEX}"
                        COMMENT "Generating LaTeX documentation.")
    else()
      add_custom_target(doc-latex ${ALL_STRING} DEPENDS generate-doxygen SOURCES ${_sources}
                        COMMAND "${LATEX_EXECUTABLE}" -shell-escape -draftmode "${DOXYGEN_INPUT_LATEX}"
                        COMMAND "${LATEX_EXECUTABLE}" -shell-escape "${DOXYGEN_INPUT_LATEX}"
                        WORKING_DIRECTORY "${ORIGINAL_DOXYGEN_OUTPUT_DIR}/${DOXYGEN_LATEX_OUTPUT}"
                        BYPRODUCTS "${DOXYGEN_OUTPUT_LATEX}"
                        COMMENT "Generating LaTeX documentation.")
    endif()
    install(FILES "${DOXYGEN_OUTPUT_LATEX}" DESTINATION "${CMAKE_INSTALL_DOCDIR}/pdf" RENAME "${PROJECT_NAME}_Manual.pdf" OPTIONAL)
  else()
    add_custom_target(doc-latex "${CMAKE_COMMAND}" -E true COMMENT "${Yellow}LaTeX format not activated for doxygen !${Reset}")
  endif()

  string(REGEX REPLACE ";" " " DOXYGEN_ALL_TARGETS_NAMES "${DOXYGEN_ALL_TARGETS}")
  list(LENGTH DOXYGEN_ALL_TARGETS_NAMES DOXYGEN_ALL_TARGETS_NAMES_LENGTH)
  if(NOT ${DOXYGEN_ALL_TARGETS_NAMES_LENGTH} EQUAL 0)
    set(DOCS_TARGET_COMMENT "Generating the docs ( formats: ${DOXYGEN_ALL_TARGETS_NAMES} ).")
  else()
    set(DOCS_TARGET_COMMENT "${Yellow}No formats activated to Doxygen !${Reset}")
  endif()
  add_custom_target(docs ${ALL_STRING} DEPENDS "${DOXYGEN_ALL_TARGETS}" COMMENT "${DOCS_TARGET_COMMENT}")

endfunction()

cmake_policy(POP)
