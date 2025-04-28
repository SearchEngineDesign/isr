#pragma once

#include "isr.h"
#include "../index/index.h"

class ISRHandler {

public:

   void SetIndexReadHandlerPtr(IndexReadHandler *iRead);

   // open ISREndDoc
   ISREndDoc *OpenISREndDoc( );
   void CloseISREndDoc( ISREndDoc *isrEndDoc );

   void CloseISR( ISR *isr );

   // open ISRWord
   ISRWord *OpenISRWord( char *word );
   // void CloseISRWord( ISRWord *isrWord );

   ISROr *OpenISROr(ISR **terms, unsigned int NumberOfTerms);
   // void CloseISROr( ISROr *isror );

   ISRAnd *OpenISRAnd(ISR **terms, unsigned int NumberOfTerms );
   // void CloseISRAnd( ISRAnd *isrand );

   ISRPhrase *OpenISRPhrase( ISR **terms, unsigned int NumberOfTerms );
   // void CloseISRPhrase ( ISRPhrase *isrphrase );


   ISRContainer *OpenISRContainer( ISR **contained, ISR **excluded, unsigned int countContained, unsigned int countExcluded );
   // void CloseISRContainer( ISRContainer *isrcontainer );


private:
   IndexReadHandler *indexRead;
};
