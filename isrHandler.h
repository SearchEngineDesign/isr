#pragma once

#include "isr.h"
#include "../index/index.h"

class ISRHandler {

public:

   // open ISREndDoc
   ISREndDoc *OpenISREndDoc( IndexReadHandler *indexRead );
   void CloseISREndDoc( ISREndDoc *isrEndDoc );

   // open ISRWord
   ISRWord *OpenISRWord( char *word, IndexReadHandler *indexRead );
   void CloseISRWord( ISRWord *isrWord );

   //

   ISROr *OpenISROr(ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms);
   ISRAnd *OpenISRAnd(ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms);
   ISRPhrase *OpenISRPhrase( ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms);
};
