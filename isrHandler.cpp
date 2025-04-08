#include "isrHandler.h"
#include "isr.h"
#include "../utils/IndexBlob.h"


ISREndDoc *ISRHandler::OpenISREndDoc( IndexReadHandler *indexRead ) {
   ISREndDoc *endDoc = new ISREndDoc;

   const SerialPostingList *postingList = indexRead->Find("%")->Value(); // TODO: deal with word
   std::cout << "number of eod: " << postingList->posts << std::endl;
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

   isrWord->DocumentEnd = OpenISREndDoc(indexRead);

   isrWord->Seek(0); // seek to the first occurrence

   return isrWord;
}

void ISRHandler::CloseISRWord( ISRWord *isrWord ) {
   CloseISREndDoc(isrWord->DocumentEnd);
   delete isrWord;
   isrWord = nullptr;
}

