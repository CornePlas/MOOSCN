######################################################################################
### 1) Include the sources
######################################################################################

INCLUDE_DIRECTORIES(${PARADISEO_EO_SRC_DIR}/src
                    ${PARADISEO_MOEO_SRC_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src)

######################################################################################


######################################################################################
### 2) Specify where CMake can find the libraries
######################################################################################

LINK_DIRECTORIES(${PARADISEO_EO_BIN_DIR}/lib)
LINK_DIRECTORIES(${PARADISEO_MOEO_BIN_DIR}/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)
######################################################################################


######################################################################################
### 3) Define your target: just an executable here
######################################################################################
ADD_EXECUTABLE(SCN_NSGAII SCN_NSGAII.cpp)
ADD_DEPENDENCIES(SCN_NSGAII lSCN)
ADD_EXECUTABLE(SCN_SPEA2 SCN_SPEA2.cpp)

######################################################################################


######################################################################################
### 4) Link the librairies for your executable
######################################################################################

# Only if you need to link libraries
TARGET_LINK_LIBRARIES(SCN_NSGAII moeo eo eoutils lSCN)
TARGET_LINK_LIBRARIES(SCN_SPEA2 moeo eo eoutils lSCN)

######################################################################################


######################################################################################
### 5) Copy the instances and the "param" file in the build path for an easy use.
### 
### --> run the "make install" target to copy the parameter file / instances
###	in the directory where you build the application
######################################################################################

ADD_CUSTOM_TARGET(install DEPENDS ${CMAKE_SOURCE_DIR}/application/NSGAII.param
				  ${CMAKE_SOURCE_DIR}/application)
ADD_CUSTOM_COMMAND(
    TARGET install
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different
      ${CMAKE_SOURCE_DIR}/application/NSGAII.param
      ${CMAKE_BINARY_DIR}/application)

######################################################################################
