PROJECT(admesh C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(GNUInstallDirs)
#===========================================================
IF(NOT(CMAKE_C_FLAGS))
  SET(CMAKE_BUILD_TYPE Release)
  SET(CMAKE_C_FLAGS  "-Wall -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2")
ENDIF (NOT(CMAKE_C_FLAGS))
#===========================================================
FIND_LIBRARY(M_LIB m)
#===========================================================
ADD_EXECUTABLE(admesh admesh.c connect.c stl_io.c stlinit.c util.c normals.c shared.c)
TARGET_LINK_LIBRARIES(admesh ${M_LIB})
INSTALL(TARGETS admesh DESTINATION ${CMAKE_INSTALL_BINDIR})
#===========================================================
INSTALL(FILES ADMESH.DOC block.stl DESTINATION ${CMAKE_INSTALL_DOCDIR})
INSTALL(FILES admesh.1 DESTINATION ${CMAKE_INSTALL_MANDIR})
#===========================================================
