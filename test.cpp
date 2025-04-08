#include "isr.h"
#include "../index/index.h"
#include "../utils/IndexBlob.h"
#include "../utils/Utf8.h"
#include "isrHandler.h"
#include <cstddef>

int main() {
  // 1. shallow copy
  // 2. utf8 only 6 bytes
  // 3. how to find title (decoration?)
  // 4. seg fault

   // TODO: 1. modify isr.cpp (to serial)
   // 2. Read Utf8 with deco
   // 3. test

   IndexReadHandler readHandler = IndexReadHandler();
   readHandler.ReadIndex("../index/0");
   const SerialTuple *tup = readHandler.Find("Hamilton");
   string str = string(tup->Key()->c_str());
   std::cout << str << std::endl;
   assert(string(tup->Key()->c_str()) == string("Hamilton"));

   const SerialPostingList *list = tup->Value();

   std::cout << list->posts << std::endl;

   // const SerialString *str = readHandler.getDocument(0);
   // const SerialPost *eof = readHandler.Find("%")->Value()->getPost(0);
   // assert(eof->data[0] == static_cast<char>(231));
   // for (int i = 0; i < 10; i++) {
   //   const SerialPost *p = list->getPost(i);
   //   size_t num = GetCustomUtf8(reinterpret_cast<const Utf8 *>(p));
   //    std::cout << num << " ";
   // }
   // std::cout << "\n";

   ISRHandler isrHandler;
   char word[] = "Hamilton";
   ISRWord *isrWord = isrHandler.OpenISRWord(word, &readHandler);


   if (isrWord->Seek(0) != nullptr) {
      std::cout << isrWord->GetMatchingDoc() << std::endl;
   }

   // assert(string(str->c_str()) == string("https://baseURL1"));
   // const SerialString *str2 = readHandler.getDocument(1);
   // assert(string(str2->c_str()) == string("https://baseURL2"));
}