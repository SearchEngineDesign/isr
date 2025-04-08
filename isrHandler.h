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

   ISROr *OpenISROr(ISR **terms, unsigned int NumberOfTerms, IndexReadHandler *indexRead);
   void CloseISROr( ISROr *isror );

   ISRAnd *OpenISRAnd(ISR **terms, unsigned int NumberOfTerms , IndexReadHandler *indexRead);
   void CloseISRAnd( ISRAnd *isrand );

   ISRPhrase *OpenISRPhrase( ISR **terms, unsigned int NumberOfTerms, IndexReadHandler *indexRead );
   void CloseISRPhrase ( ISRPhrase *isrphrase );


   ISRContainer *OpenISRContainer( ISR **contained, ISR **excluded, unsigned int countContained, unsigned int countExcluded, IndexReadHandler *indexRead );
   void CloseISRContainer( ISRContainer *isrcontainer );
};
