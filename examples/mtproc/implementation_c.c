#include "implementation_c.h"
#include "methodtable_c.h"
#include <string.h>
#include <malloc.h>

#define TRUE  1
#define FALSE 0
#define UNUSED(x) ( (void)(x) )

typedef struct
{
   char buf; 
   ContentIterator input;
   FormatOutput output;
}
MethodData;

static BOOL getNext( ContentIterator* this_, void* buffer, unsigned int buffersize)
{
   char* in = (char*)this_->m_ptr;
   unsigned int nn = this_->m_size - this_->m_pos;

   if (buffersize == 0)
   {
      this_->m_state = Error;
      this_->m_errorCode = 1;
      return FALSE;
   }
   else if (nn == 0)
   {
      if (this_->m_gotEoD)
      {
         this_->m_state = Open;
         return FALSE;
      }
      else
      { 
         this_->m_state = EndOfMessage;
         return FALSE;
      }
   }
   else
   {
      this_->m_state = Open;
      *(char*)buffer = *in;
      this_->m_pos += 1;
      return TRUE;
   }
}

static BOOL print( FormatOutput* this_, int type_, void* element, unsigned int elementsize)
{
   char* out = (char*)this_->m_ptr + this_->m_pos;
   unsigned int nn = this_->m_size - this_->m_pos;
   UNUSED(type_);

   if (elementsize > nn)
   {
      return FALSE;
   }
   else
   {
      memcpy( out, element, elementsize);
      this_->m_pos += elementsize;
      return TRUE;
   }
}

static int echo( MethodContext* ctx, unsigned int argc, const char** argv)
{
   MethodData* data = (MethodData*)ctx->data;
   UNUSED(argc);
   UNUSED(argv);

   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &data->input;
      ctx->formatOutput = &data->output;
   }
   if (data->buf != 0)
   {
      if (!data->output.m_print( &data->output, 0, &data->buf, 1)) return 0;
      data->buf = 0;
   }
   while (ctx->contentIterator->m_getNext( ctx->contentIterator, &data->buf, 1))
   {
      if (!data->output.m_print( &data->output, 0, &data->buf, 1)) return 0;
      data->buf = 0;
   }
   return 0;
}

static int printarg( MethodContext* ctx, unsigned int argc, const char** argv)
{
   MethodData* data = (MethodData*)ctx->data;
   UNUSED(argc);
   UNUSED(argv);

   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &data->input;
      ctx->formatOutput = &data->output;
   }
   return 0;
}

static const Method methodTable[3] = {{"echo",&echo,TRUE},{"parg",&printarg,FALSE},{0,0,FALSE}};

static MethodDataP createMethodData(void)
{
   MethodData* rt = (MethodData*)calloc( sizeof(MethodData), 1);
   if (rt == NULL) return NULL;
   rt->input.m_getNext = &getNext;
   rt->output.m_print = &print;
   return (MethodDataP)rt;
}

static void destroyMethodData( MethodDataP d)
{
   free( d);
}

static Implementation implementation = { methodTable, 0, &createMethodData, &destroyMethodData };

void* mtproc_getImplementation(void)
{
   return &implementation;
}


