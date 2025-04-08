#include "isrHandler.h"
#include "isr.h"
#include "../utils/IndexBlob.h"


ISREndDoc *ISRHandler::OpenISREndDoc( IndexReadHandler *indexRead ) {
   ISREndDoc *endDoc = new ISREndDoc;

   const SerialPostingList *postingList = indexRead->Find("%")->Value(); // TODO: deal with word
   // std::cout << "number of eod: " << postingList->posts << std::endl;
   endDoc->SetPostingList(postingList);

   endDoc->Seek(0); // seek to the first occurrence

   return endDoc;

}


void ISRHandler::CloseISREndDoc( ISREndDoc *isrEndDoc ) {
   delete isrEndDoc;
   isrEndDoc = nullptr;
}

ISRWord *ISRHandler::OpenISRWord( char *word, IndexReadHandler *indexRead ) {
   ISRWord *isrWord = new ISRWord;

   const SerialPostingList *postingList = indexRead->Find(word)->Value(); // TODO: deal with word

   isrWord->SetPostingList(postingList);

   isrWord->EndDoc = OpenISREndDoc(indexRead);

   isrWord->Seek(0); // seek to the first occurrence

   return isrWord;
}

void ISRHandler::CloseISRWord( ISRWord *isrWord ) {
   CloseISREndDoc(isrWord->EndDoc);
   delete isrWord;
   isrWord = nullptr;
}


ISROr *ISRHandler::OpenISROr( ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms )
{
   ISROr *isrOr = new ISROr;
   isrOr->Terms = terms;
   isrOr->NumberOfTerms = NumberOfTerms;
   isrOr->EndDoc = OpenISREndDoc(indexRead);
   isrOr->Seek(0);
   return isrOr;
}

 ISRAnd *ISRHandler::OpenISRAnd( ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms) {
   ISRAnd *isrAnd = new ISRAnd;
   isrAnd->Terms = terms;
   isrAnd->NumberOfTerms = NumberOfTerms;
   isrAnd->EndDoc = OpenISREndDoc(indexRead);
   isrAnd->Seek(0);
   return isrAnd;
}

ISRPhrase *ISRHandler::OpenISRPhrase( ISR **terms, IndexReadHandler *indexRead, unsigned int NumberOfTerms ) {
   ISRPhrase *isrPhrase = new ISRPhrase;
   isrPhrase->Terms = terms;
   isrPhrase->NumberOfTerms = NumberOfTerms;
   isrPhrase->EndDoc = OpenISREndDoc(indexRead);
   isrPhrase->Seek(0);
   return isrPhrase;
}