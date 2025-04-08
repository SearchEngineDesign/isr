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

  

  
};
