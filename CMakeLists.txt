# Define the project name
# Have to suppress this warning for sanity
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
project(lfast_mount_controller LANGUAGES C CXX VERSION 0.0.1)
cmake_minimum_required(VERSION 3.10)

# Driver development setup guide: http://docs.indilib.org/drivers/project-setup.html

# specify the C++ standard
# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INSTALL_PREFIX /usr)
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(RULES_INSTALL_DIR "/etc/udev/rules.d")
set(BUILD_TESTS FALSE)


#=================================================================================================#
#========================================= Version Info  =========================================#
#=================================================================================================#
# these will be used to set the version number in config.h and our driver's xml file
set(CDRIVER_VERSION_MAJOR 0)
set(CDRIVER_VERSION_MINOR 1)


#=================================================================================================#
#========================================= Dependencies  =========================================#
#=================================================================================================#
# add our cmake_modules folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/02_CMake_Modules/")


find_package(Modbus REQUIRED)


#Require INDI 1.4 or later
message("Checking for INDI Library")
find_package(INDI 1.4 REQUIRED)
find_package(Nova REQUIRED)

if(INDI_FOUND)
   include_directories(${INDI_INCLUDE_DIR})
   add_subdirectory(00_Utils)
   add_subdirectory(01_Mount_Driver)
   add_subdirectory(05_AHRS_Driver)
   
   if (BUILD_TESTS)
	message("BUILD_TESTS=TRUE, unit tests will be built.")
	enable_testing()
	include(CTest)
	add_subdirectory(03_Unit_Tests)
	
   else ()
        message("BUILD_TESTS=FALSE, unit tests will NOT be built.")
        
   endif ()
endif(INDI_FOUND)


#find_package(ZLIB REQUIRED)
#find_package(GSL REQUIRED)




