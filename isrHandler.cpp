#include "isrHandler.h"
#include "isr.h"
#include "../utils/IndexBlob.h"

void ISRHandler::SetIndexReadHandler( IndexReadHandler *iRead ) 
   {
   indexRead = iRead;
   }


ISREndDoc *ISRHandler::OpenISREndDoc(  )
   {
   ISREndDoc *endDoc = new ISREndDoc;

   const SerialTuple *tuple = indexRead->Find("%");

   if ( tuple == nullptr )
      return nullptr;

   const SerialPostingList *postingList = tuple->Value();

   // std::cout << "number of eod: " << postingList->posts << std::endl;
   endDoc->SetPostingList(postingList);

   endDoc->Seek(0); // seek to the first occurrence

   return endDoc;

   }


void ISRHandler::CloseISREndDoc( ISREndDoc *isrEndDoc )
   {
   delete isrEndDoc;
   isrEndDoc = nullptr;
   }

ISRWord *ISRHandler::OpenISRWord( char *word )
   {
   ISRWord *isrWord = new ISRWord;

   const SerialTuple *tuple = indexRead->Find(word);

   if ( tuple == nullptr )
      return nullptr;

   const SerialPostingList *postingList = tuple->Value();

   isrWord->SetPostingList(postingList);

   isrWord->EndDoc = OpenISREndDoc( );

   isrWord->Seek(0); // seek to the first occurrence

   return isrWord;
   }


void ISRHandler::CloseISR( ISR *isr )
   {
   if ( isr != nullptr )
      CloseISREndDoc(isr->EndDoc);
   delete isr;
   isr = nullptr;
   std::cout << "call close";
   }

// void ISRHandler::CloseISRWord( ISRWord *isrWord )
//    {
//    if ( isrWord != nullptr )
//       CloseISREndDoc(isrWord->EndDoc);
//    delete isrWord;
//    isrWord = nullptr;
//    }


ISROr *ISRHandler::OpenISROr( ISR **terms, unsigned int NumberOfTerms )
   {
   ISROr *isrOr = new ISROr;
   isrOr->Terms = terms;
   isrOr->NumberOfTerms = NumberOfTerms;
   isrOr->EndDoc = OpenISREndDoc( );
   isrOr->Seek(0);
   return isrOr;
   }

// void ISRHandler::CloseISROr( ISROr *isror )
//    {
//    CloseISREndDoc(isror -> EndDoc);
//    delete isror;
//    isror = nullptr;
//    }


ISRAnd *ISRHandler::OpenISRAnd( ISR **terms, unsigned int NumberOfTerms ) 
   {
   ISRAnd *isrAnd = new ISRAnd;
   isrAnd->Terms = terms;
   isrAnd->NumberOfTerms = NumberOfTerms;
   isrAnd->EndDoc = OpenISREndDoc( );
   isrAnd->Seek(0);
   return isrAnd;
   }

// void ISRHandler::CloseISRAnd( ISRAnd *isrand )
//    {
//       CloseISREndDoc(isrand->EndDoc);
//       delete isrand;
//       isrand = nullptr;
//    }

ISRPhrase *ISRHandler::OpenISRPhrase( ISR **terms, unsigned int NumberOfTerms )
   {
   ISRPhrase *isrPhrase = new ISRPhrase;
   isrPhrase->Terms = terms;
   isrPhrase->NumberOfTerms = NumberOfTerms;
   isrPhrase->EndDoc = OpenISREndDoc( );
   isrPhrase->Seek(0);
   return isrPhrase;
   }

// void ISRHandler::CloseISRPhrase( ISRPhrase *isrphrase ) {
//    CloseISREndDoc(isrphrase->EndDoc);
//    delete isrphrase;
//    isrphrase = nullptr;
// }

ISRContainer *ISRHandler::OpenISRContainer( ISR **contained, ISR **excluded, unsigned int countContained, unsigned int countExcluded )
   {
   // excluded can have nullptr
   ISRContainer *isrcontainer = new ISRContainer;
   isrcontainer -> Contained = contained;
   isrcontainer -> Excluded = excluded;
   isrcontainer -> CountContained = countContained;
   isrcontainer -> CountExcluded = countExcluded;
   isrcontainer -> EndDoc = OpenISREndDoc( );
   isrcontainer -> Seek(0);
   return isrcontainer;
   }

// void ISRHandler::CloseISRContainer( ISRContainer *isrcontainer ){
//    CloseISREndDoc(isrcontainer->EndDoc);
//    delete isrcontainer;
//    isrcontainer = nullptr;
// }