cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_dotnet_source_files
  dotnetRuntimeEnvironment.cpp
  mod_command_dotnet.cpp
  dotnetRuntimeEnvironmentConfig.hpp
  comauto/variantAssignment.hpp
  comauto/function.hpp
  comauto/utils.hpp
  comauto/procProviderDispatch.hpp
  comauto/typelib.cpp
  comauto/clr.hpp
  comauto/utils.cpp
  comauto/clr.cpp
  comauto/variantInputFilter.hpp
  comauto/function.cpp
  comauto/variantToString.hpp
  comauto/variantToString.cpp
  comauto/procProviderDispatch.cpp
  comauto/typelib.hpp
  comauto/variantAssignment.cpp
  comauto/variantInputFilter.cpp
  dotnetRuntimeEnvironmentConfig.cpp
  dotnetRuntimeEnvironment.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_command_dotnet MODULE ${mod_command_dotnet_source_files})

set_target_properties(mod_command_dotnet PROPERTIES PREFIX "")

target_link_libraries(mod_command_dotnet dotnet wolframe_serialize)
