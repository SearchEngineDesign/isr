#include "isr.h"
#include "../index/index.h"
#include "../utils/IndexBlob.h"
#include "../utils/Utf8.h"
#include "isrHandler.h"
#include <cstddef>

int main() {

   // read index chunk

   IndexReadHandler readHandler = IndexReadHandler();
   readHandler.ReadIndex("../log/chunks/1");

   // initialize ISRHandler

   ISRHandler isrHandler;
   isrHandler.SetIndexReadHandler(&readHandler);

   // How to define ISRWord

   char word1[] = "wiki";
   ISRWord *isrWord1 = isrHandler.OpenISRWord(word1);
   
   if (isrWord1 == nullptr) {
      isrHandler.CloseISR(isrWord1);
      return 0;
   }

   // How to use ISRWord.Seek

   int i = 0;
   size_t target = 0;
   while (isrWord1->Seek(target) != nullptr)
   {
      std::cout << "matching doc: " << isrWord1 ->GetMatchingDoc() << std::endl;

      if (i == 10)
         break;
      i++;
      target = isrWord1->EndDoc->GetStartLocation() + 1;
      std::cout << "target: " << target << "\n";
   }

   // Close
   isrHandler.CloseISR(isrWord1);
   return 0;

   // char word2[] = "features";
   // ISRWord *isrWord2 = isrHandler.OpenISRWord(word2);


   // // How to use ISRAnd/ISROr/ISRPhrase
   // ISR **terms = new ISR *[2];
   // terms[0] = isrWord1;
   // terms[1] = isrWord2;
   // ISRAnd *isr = isrHandler.OpenISRAnd( terms, 2);

   // int i = 0;
   // size_t target = 0;

   // while (isr->Seek(target) != nullptr)
   // {
   //    std::cout << "matching doc: " << isr ->GetMatchingDoc() << std::endl;

   //    if (i == 5)
   //       break;
   //    i++;
   //    target = isr->EndDoc->GetStartLocation() + 1;
   //    std::cout << "target: " << target << "\n";
   // }


   // // How to use ISRContainer
   // char word3[] = "motherfucking";
   // ISRWord *isrWord3 = isrHandler.OpenISRWord(word3);
   
   // char word4[] = "teachers";
   // ISRWord *isrWord4 = isrHandler.OpenISRWord(word4);

   // // contained word should not be nullptr
   // if (isrWord1 == nullptr || isrWord2 == nullptr)
   //    return;
   // // excluded word could be nullptr

   // ISR **contained = new ISR *[2];
   // ISR **excluded = new ISR *[2];
   // contained[0] = isrWord1;
   // contained[1] = isrWord2;
   // excluded[0] = isrWord3;
   // excluded[1] = isrWord4;

   // ISRContainer *isr = isrHandler.OpenISRContainer(contained,excluded, 2, 2);

   // int i = 0;
   // size_t target = 0;

   // while (isr->Seek(target) != nullptr)
   // {
   //    std::cout << "matching doc: " << isr ->GetMatchingDoc() << std::endl;

   //    if (i == 5)
   //       break;
   //    i++;
   //    target = isr->EndDoc->GetStartLocation() + 1;
   //    std::cout << "target: " << target << "\n";
   // }


   // How to get document url
   
   // std::cout << readHandler.getDocument(0)->c_str() << std::endl;
   // std::cout << readHandler.getDocument(6)->c_str() << std::endl;
   // std::cout << readHandler.getDocument(16)->c_str() << std::endl;
   // std::cout << readHandler.getDocument(3)->c_str() << std::endl;
   // std::cout << readHandler.getDocument(4)->c_str() << std::endl;
}