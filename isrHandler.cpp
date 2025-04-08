#include "isrHandler.h"
#include "isr.h"
#include "../utils/IndexBlob.h"


ISREndDoc *ISRHandler::OpenISREndDoc( IndexReadHandler *indexRead )
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

ISRWord *ISRHandler::OpenISRWord( char *word, IndexReadHandler *indexRead )
   {
   ISRWord *isrWord = new ISRWord;

   const SerialTuple *tuple = indexRead->Find(word);

   if ( tuple == nullptr )
      return nullptr;

   const SerialPostingList *postingList = tuple->Value();

   isrWord->SetPostingList(postingList);

   isrWord->EndDoc = OpenISREndDoc(indexRead);

   isrWord->Seek(0); // seek to the first occurrence

   return isrWord;
   }

void ISRHandler::CloseISRWord( ISRWord *isrWord )
   {
   CloseISREndDoc(isrWord->EndDoc);
   delete isrWord;
   isrWord = nullptr;
   }


ISROr *ISRHandler::OpenISROr( ISR **terms, unsigned int NumberOfTerms, IndexReadHandler *indexRead )
   {
   ISROr *isrOr = new ISROr;
   isrOr->Terms = terms;
   isrOr->NumberOfTerms = NumberOfTerms;
   isrOr->EndDoc = OpenISREndDoc(indexRead);
   isrOr->Seek(0);
   return isrOr;
   }

void ISRHandler::CloseISROr( ISROr *isror )
   {
   CloseISREndDoc(isror -> EndDoc);
   delete isror;
   isror = nullptr;
   }


ISRAnd *ISRHandler::OpenISRAnd( ISR **terms, unsigned int NumberOfTerms, IndexReadHandler *indexRead) 
   {
   ISRAnd *isrAnd = new ISRAnd;
   isrAnd->Terms = terms;
   isrAnd->NumberOfTerms = NumberOfTerms;
   isrAnd->EndDoc = OpenISREndDoc(indexRead);
   isrAnd->Seek(0);
   return isrAnd;
   }

void ISRHandler::CloseISRAnd( ISRAnd *isrand )
   {
      CloseISREndDoc(isrand->EndDoc);
      delete isrand;
      isrand = nullptr;
   }

ISRPhrase *ISRHandler::OpenISRPhrase( ISR **terms, unsigned int NumberOfTerms, IndexReadHandler *indexRead )
   {
   ISRPhrase *isrPhrase = new ISRPhrase;
   isrPhrase->Terms = terms;
   isrPhrase->NumberOfTerms = NumberOfTerms;
   isrPhrase->EndDoc = OpenISREndDoc(indexRead);
   isrPhrase->Seek(0);
   return isrPhrase;
   }

void ISRHandler::CloseISRPhrase( ISRPhrase *isrphrase ) {
   CloseISREndDoc(isrphrase->EndDoc);
   delete isrphrase;
   isrphrase = nullptr;
}

ISRContainer *ISRHandler::OpenISRContainer( ISR **contained, ISR **excluded, unsigned int countContained, unsigned int countExcluded, IndexReadHandler *indexRead)
   {
   // excluded can have nullptr
   ISRContainer *isrcontainer = new ISRContainer;
   isrcontainer -> Contained = contained;
   isrcontainer -> Excluded = excluded;
   isrcontainer -> CountContained = countContained;
   isrcontainer -> CountExcluded = countExcluded;
   isrcontainer -> EndDoc = OpenISREndDoc(indexRead);
   isrcontainer -> Seek(0);
   return isrcontainer;
   }

void ISRHandler::CloseISRContainer( ISRContainer *isrcontainer ){
   CloseISREndDoc(isrcontainer->EndDoc);
   delete isrcontainer;
   isrcontainer = nullptr;
}