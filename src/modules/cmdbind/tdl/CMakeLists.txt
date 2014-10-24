cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_tdl_source_files
  tdlTransactionInput.hpp
  tdlTransactionFunction.cpp
  transactionProgram.hpp
  tdlTransactionInput.cpp
  tdlAuditStep.hpp
  tdlTransactionFunction.hpp
  tdl/preprocCallStatement.hpp
  tdl/preprocElementReference.hpp
  tdl/commandDefinition.hpp
  tdl/elementReference.hpp
  tdl/subroutineCallStatement.hpp
  tdl/preprocBlock.cpp
  tdl/preprocCallStatement.cpp
  tdl/preprocElementReference.cpp
  tdl/subroutineCallStatement.cpp
  tdl/embeddedStatement.hpp
  tdl/embeddedStatement.cpp
  tdl/parseUtils.cpp
  tdl/auditElementReference.cpp
  tdl/auditElementReference.hpp
  tdl/parseUtils.hpp
  tdl/commandDefinition.cpp
  tdl/preprocBlock.hpp
  tdl/auditCallStatement.cpp
  tdl/elementReference.cpp
  tdl/auditCallStatement.hpp
  loadTransactionProgram.cpp
  loadTransactionProgram.hpp
  mod_command_tdl.cpp
  tdl2vmTranslator.hpp
  tdlTransactionPreprocStep.cpp
  vm/errorHintTable.hpp
  vm/nameTable.hpp
  vm/tagTable.hpp
  vm/patchArgumentMap.hpp
  vm/program.cpp
  vm/program.hpp
  vm/subroutine.hpp
  vm/selectorPathSet.hpp
  vm/subroutine.cpp
  vm/inputStructure.hpp
  vm/selectorPathSet.cpp
  vm/selectorPath.hpp
  vm/tagTable.cpp
  vm/selectorPath.cpp
  vm/inputStructure.cpp
  transactionProgram.cpp
  tdl2vmTranslator.cpp
  tdlTransactionPreprocStep.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  .
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_command_tdl MODULE ${mod_command_tdl_source_files})

set_target_properties(mod_command_tdl PROPERTIES PREFIX "")

target_link_libraries(mod_command_tdl
  wolframe
  wolframe_serialize
  wolframe_database
)
