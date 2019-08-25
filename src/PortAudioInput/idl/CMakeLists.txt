#
include("${OPENRTM_DIR}/cmake/idl_compile.cmake")

set(idls ${CMAKE_CURRENT_SOURCE_DIR}/BasicDataType.idl)

OPENRTM_COMPILE_IDL_FILES(${idls})
set(ALL_IDL_SRCS ${ALL_IDL_SRCS} PARENT_SCOPE)
FILTER_LIST("ALL_IDL_SRCS" "hh$" idl_headers)
