//
// FILE:
// concord.h
//
// FUNCTION:
// The lagConcord class builds a concordance.
//
// METHODS:
// The GetTopTupleContainingWord() returns a tuple id that contains the
//    word as the first word.  The returned tupleid is the tuple 
//    id with the highest count.  
//
// HISTORY:
// January 1997 Linas Vepstas

#ifndef __LAG_CONCORD_H__
#define __LAG_CONCORD_H__

#include "config.h"

class lagGenericConcordTable :
   public lagGenericWordTable
{
   public:
      lagGenericConcordTable (void);
      virtual ~lagGenericConcordTable ();

      int GetTopTupleContainingWord (int word);

      void Dump (void);

   protected:

      class Concord {
         public:
            Concord * next;
            lagGenericWordTable::Helper * where;
#ifdef LAG_USE_OVERLOADED_NEW
            void * operator new (size_t s);
            static int memblocks;
            static int memleft;
            static char * mempool;
#endif // LAG_USE_OVERLOADED_NEW
      };
      Concord ** concordance;

      virtual void AddID (Helper *);

      int num_concords;
};
   
#endif // __LAG_CONCORD_H__

// ================= END OF FILE ==================


