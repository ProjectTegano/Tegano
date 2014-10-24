cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(employee_assignment_convert_source_files
  employee_assignment_convert.cpp
  employee_assignment_convert.hpp
  employee_assignment_convert_exec.cpp
  employee_assignment_convert_exec.hpp
  mod_employee_assignment_convert.cpp
)

add_library(mod_employee_assignment_convert MODULE ${employee_assignment_convert_source_files})

set_target_properties(mod_employee_assignment_convert PROPERTIES PREFIX "")

target_link_libraries(mod_employee_assignment_convert
  wolframe_serialize
)
