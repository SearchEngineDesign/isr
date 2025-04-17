#include "isrHandler.h"
#include "isr.h"
#include "../utils/IndexBlob.h"
#include <cstddef>

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

   if ( isrWord->EndDoc == nullptr )
      return nullptr;  

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
   if ( isrOr->EndDoc == nullptr )
      return nullptr; 
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
   if ( isrAnd->EndDoc == nullptr )
      return nullptr;
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
   if ( isrPhrase->EndDoc == nullptr )
      return nullptr;
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
   if ( isrcontainer->EndDoc == nullptr )
      return nullptr;
   isrcontainer -> Seek(0);
   return isrcontainer;
   }

// void ISRHandler::CloseISRContainer( ISRContainer *isrcontainer ){
//    CloseISREndDoc(isrcontainer->EndDoc);
//    delete isrcontainer;
//    isrcontainer = nullptr;
// }