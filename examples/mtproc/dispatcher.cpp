#include "dispatcher.hpp"
#include "methodtable.hpp"
#include "logger.hpp"

using namespace _SMERP::mtproc;
using namespace _SMERP::protocol;

void CommandDispatcher::resetCommand()
{
   m_methodIdx = 0;
   m_command = unknown;
   m_lineBuffer.init();
   m_argBuffer.init();
   m_cmdBuffer.init();

   if (m_instance)
   {
      m_state = Init;
      m_context.init( m_instance->data);
   }
   else
   {
      m_state = Null;
      m_context.init( m_instance->data);
   }
}

void CommandDispatcher::init( const char** protocolCmds, Instance* instance)
{
   m_instance = instance;
   m_parser.init();
   resetCommand();
   
   if (protocolCmds)
   {
      for( unsigned int ii=0; protocolCmds[ ii]; ii++)
      {
         m_parser.add( protocolCmds[ ii]);
      }
   }
   if (instance)
   {
      m_context.data = instance->data;
      m_context.contentIterator = 0;

      if (instance->mt)
      {
         for( unsigned int ii=0; instance->mt[ii].call && instance->mt[ii].name; ii++)
         {
            m_parser.add( instance->mt[ii].name);
         }
      }
   }
}

void CommandDispatcher::protocolInput( protocol::InputBlock::iterator& start, protocol::InputBlock::iterator& end, bool eoD)
{
   if (m_state == WaitForInput)
   {
      LOG_DATA << "command handler got input";
      m_state = Running;
   }
   else
   {
      LOG_ERROR << "illegal state (unexpected protocol input)";
      throw (IllegalState());
   }
   if (m_context.contentIterator)
   {
      m_context.contentIterator->protocolInput( (void*)&*start, end-start, eoD);
   }
   else
   {
      LOG_ERROR << "illegal state: got input without recipient (content iterator disapeared)";
      throw (IllegalState());
   }
}

bool CommandDispatcher::getOutput( void** output, unsigned int* outputsize)
{
   if (!m_context.output) return false;
   if (!m_context.output->pos()) return false;
   *output = m_context.output->ptr();
   *outputsize = m_context.output->pos();
   return true;
}

void CommandDispatcher::setOutputBuffer( void* buf, unsigned int bufsize)
{
   if (m_context.output)
   {
      m_context.output->init( buf, bufsize);
   }
   else
   {
      LOG_ERROR << "illegal state: declare output buffer without format output object existing";
      throw (IllegalState());
   }
}

CommandDispatcher::CommandDispatcher( Instance* instance)
   :m_argBuffer( &m_lineBuffer)
{
   init( instance);
}

CommandDispatcher::~CommandDispatcher()
{
   if (m_instance) delete m_instance;
}

CommandDispatcher::Command CommandDispatcher::getCommand( protocol::InputBlock::iterator& itr, protocol::InputBlock::iterator& eoM)
{
   switch (m_state)
   {
      case Running:
      case WaitForInput:
      case Null:
      case Init:
      {
         resetCommand();
         int ci = m_parser.getCommand( itr, eoM, m_cmdBuffer);
         if (ci >= unknown && ci < method)
         {
            m_command = (Command)ci;
            m_methodIdx = 0;
         }
         else
         {
            m_command = method; 
            m_methodIdx = (unsigned int)ci - (unsigned int)method;
         }
         m_state = Selected;
         //no break here !
      }
      case Selected:
      {
         if (m_command == method)
         {
            if (!ProtocolParser::getLine( itr, eoM, m_argBuffer)) return unknown;
         }
         else
         {
            if (!ProtocolParser::skipSpaces( itr, eoM) && !ProtocolParser::consumeEOLN( itr, eoM)) return unknown;
            return m_command;
         }
         //no break here !
      }
      case ArgumentsParsed:
      {
         return m_command;
      }
   }
   LOG_ERROR << "illegal state (end of getCommand)";
   throw (IllegalState());
}

CommandDispatcher::IOState CommandDispatcher::call( int& returnCode)
{
   switch (m_state)
   {
      case Null:
      case Init:
      case Selected:
      { 
         LOG_ERROR << "illegal call in this state (not running)";
         throw (IllegalState());
      }
      case ArgumentsParsed:
      {
         LOG_DEBUG << "call of '" << m_instance->mt[ m_methodIdx].name << "'";
         m_state = Running;
         //no break here !
      }
      case WaitForInput:
         LOG_ERROR << "called without input";
         //no break here !
         
      case Running:
      {
         returnCode = m_instance->mt[ m_methodIdx].call( &m_context, m_argBuffer.argc(), m_argBuffer.argv());
         if (returnCode != 0)
         {
            LOG_ERROR << "error " << returnCode << " calling '" << m_instance->mt[ m_methodIdx].name << "'";
            resetCommand();
            return Close;
         }
         if (m_context.contentIterator)
         {
            switch (m_context.contentIterator->state())
            {
               case protocol::Generator::Init:
               case protocol::Generator::Processing:
                  return WriteOutput;

               case protocol::Generator::Error:
                  returnCode = m_context.contentIterator->getError();
                  LOG_ERROR << "error " << returnCode << ") in generator calling '" << m_instance->mt[ m_methodIdx].name << "'";
                  resetCommand();
                  return Close;

               case protocol::Generator::EndOfInput:
                  if (m_context.output && m_context.output->pos())
                  {
                     return WriteOutput;
                  }
                  else
                  {
                     resetCommand();
                     return Close;
                  }

               case protocol::Generator::EndOfMessage:
                  m_state = WaitForInput;
                  return ReadInput;
            }
         }
         else
         {
            if (m_context.output && m_context.output->pos())
            {
               return WriteOutput;
            }
            else
            {
               resetCommand();
               return Close;
            }
         }
      }
   }
   return Close;
}

const char* CommandDispatcher::getCapabilities()
{
   m_lineBuffer.init();
   unsigned int ii;
   if (m_instance && m_instance->mt)
   {
      for (ii=0; m_instance->mt[ii].call && m_instance->mt[ii].name; ii++)
      {
         if (ii>0)
         {
            m_lineBuffer.push_back( ',');
            m_lineBuffer.push_back( ' ');
         }
         m_lineBuffer.append( m_instance->mt[ii].name);
      }
   }
   return m_lineBuffer.c_str();
}      


