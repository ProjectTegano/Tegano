#ifndef _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"

namespace _SMERP {
namespace mtproc {

namespace generator {

struct CharIsoLatin1
{
   typedef protocol::Generator Generator;

   enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};
   static bool GetNext( Generator* this_, void* buffer, unsigned int buffersize)
   {
      char* in = (char*)this_->ptr();
      unsigned int nn = this_->size();

      if (buffersize == 0)
      {
         this_->setState( Generator::Error, ErrBufferTooSmall);
         return false;
      }
      else if (nn == 0)
      {
         this_->setState( this_->gotEoD()?Generator::EndOfInput:Generator::EndOfMessage, Ok);
         return false;
      }
      else
      {
         this_->setState( Generator::Processing, Ok);
         *(char*)buffer = *in;
         this_->skip( 1);
         return true;
      }
   }
};

}//namespace generator1
}}//namespace
#endif


