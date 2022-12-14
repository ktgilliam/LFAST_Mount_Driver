########## LFAST Controller Utilities Unit Tests##############

# set our include directories to look for header files

include_directories( ${CMAKE_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

# cmake -DBUILD_TESTS=true -GNinja ..


#=================================================================================================#
#========================================= gtest  ================================================#
#=================================================================================================#
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#=================================================================================================#
#========================================= project test executables ==============================#
#=================================================================================================#

#### Bash Wrapper tests
add_executable(
  bash_wrapper_tests
  bash_wrapper_tests.cc
)

target_link_libraries(
  bash_wrapper_tests
  bash_wrapper
  GTest::gtest_main
)

#### comms tests
add_executable(
  lfast_comms_tests
  lfast_comms_tests.cc
)

target_link_libraries(
  lfast_comms_tests
  lfast_comms
  GTest::gtest_main
)


add_executable(
  math_util_tests
  math_util_tests.cc
)
target_link_libraries(
  math_util_tests
  GTest::gtest_main
)

# #### mount driver tests
# add_executable(
#   lfast_mount_driver_tests
#   lfast_mount_driver_tests.cc
# )

# target_link_libraries(
#   lfast_mount_driver_tests
#   lfast_mount_driver_testable
#   GTest::gtest_main
# )

#### Bringing it all together
include(GoogleTest)
gtest_discover_tests(bash_wrapper_tests)
gtest_discover_tests(lfast_comms_tests)

