######################################################################################
### 1) Include the sources
######################################################################################

INCLUDE_DIRECTORIES(${PARADISEO_EO_SRC_DIR}/src)
INCLUDE_DIRECTORIES(${PARADISEO_MOEO_SRC_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

######################################################################################


######################################################################################
### 2) Define your target(s): just the flow-shop lib here
######################################################################################

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

SET (SCN_SOURCES
		SCN.cpp
		)

ADD_LIBRARY(lSCN STATIC ${SCN_SOURCES})

######################################################################################

