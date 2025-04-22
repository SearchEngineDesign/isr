#include <climits>
#include "isr.h"
#include "../index/index.h"

// ISR

Location ISR::GetStartLocation() // -> return start
   {
   return start;  
   }  

Location ISR::GetEndLocation() // -> return end
   {
   return end;
   }

size_t ISR::GetMatchingDoc() // return result
   {
   return matchingDocument;
   }

unsigned ISR::GetDocumentCount()
   {
   return postingList->documentCount;
   }

unsigned ISR::GetNumberOfOccurrences()
   {
   return postingList->posts;
   }

const SerialPost *ISR::GetCurrentPost()
   {
   return curr;
   }

void ISR::SetCurrentPost( const SerialPost *p )
   {
   curr = p;
   }

void ISR::SetPostingList( const SerialPostingList *pl )
   {
   postingList = pl;
   }


// ISRWord

const SerialPost *ISRWord::Next( )
   {
   // do a next on the term, then return match
   return Seek( start + 1 );  
   }

const SerialPost *ISRWord::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 56;
   // std::cout << (int)highBits << std::endl;
   size_t idx = 0;
   while (highBits > 0) {
      highBits >>= 1;
      idx++;
   }
   if (idx >= postingList->seekIndex)
      return nullptr;
   const std::pair<size_t, size_t> *p = postingList->getSeekPair( idx ); 

   size_t offset = p->first;
   size_t actualLocation = p->second;

   // std::cout << "offset: " << offset << " actual loc: " << actualLocation << std::endl;

   // seek from synchronization point to target
   

   // if using seek table
   while ( actualLocation < target )
      {
      if ( offset + 1 < postingList->posts ) // TODO: check
         {
         // std::cout << "data 1: " << offset + 1 << std::endl;
         const SerialPost *data = postingList->getPost(offset+1);
         // std::cout << "data: " << offset + 1 << std::endl;
         actualLocation += GetCustomUtf8(reinterpret_cast<const Utf8 *>(data));
         // actualLocation += GetCustomUtf8((*list)[offset+1].getData()); // TODO: unsigned char* and char*
         offset ++;
         // std::cout << "in loop: offset: " << offset <<  " actual loc: " << actualLocation << std::endl;
         }
      else
         {
         // reach the end and no match, return nullptr
         return nullptr;
         }
      }

   // update start location
   start = actualLocation;
   // std::cout << "offset: " << offset <<  " actual loc: " << actualLocation << std::endl;
   // std::cout << "data 2: " << offset << std::endl;
   curr = postingList->getPost(offset);
   
   // std::cout << "test\n";
   // TODO: update end(?)

   // update end of document
   // std::cout << "doc end start loc: " << EndDoc->GetStartLocation() << std::endl;
   if ( actualLocation > EndDoc->GetStartLocation( ) || actualLocation < EndDoc->GetStartLocation() - EndDoc->GetDocumentLength() ) {
      // std::cout << "docend seek\n";
      EndDoc->Seek( actualLocation );
   }
   
   // std::cout << "end if\n";

   matchingDocument = EndDoc->GetMatchingDoc();
   return curr;
   }

const SerialPost *ISRWord::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
   return Seek( EndDoc->GetStartLocation( ) + 1 );  
   }

// ISREndDoc

Location ISREndDoc::GetStartLocation() // -> return start
   {
   return start;  
   }  

Location ISREndDoc::GetEndLocation() // -> return end
   {
   return end;
   }

size_t ISREndDoc::GetMatchingDoc() // return result
   {
   return matchingDocument;
   }

const SerialPost *ISREndDoc::GetCurrentPost()
   {
   return curr;
   }

void ISREndDoc::SetCurrentPost( const SerialPost *p )
   {
   curr = p;
   }

void ISREndDoc::SetPostingList( const SerialPostingList *pl )
   {
   postingList = pl;
   }

const SerialPost *ISREndDoc::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 56;
   size_t idx = 0;
   while (highBits > 0) {
      highBits >>= 1;
      idx++;
   }
   if (idx >= postingList->seekIndex)
      return nullptr;
   const std::pair<size_t, size_t> *p = postingList->getSeekPair( idx );
   size_t offset = p->first;
   size_t actualLocation = p->second;

   // std::cout << "eod high bit: " << idx << " offset: " << offset << " actual location: " << actualLocation <<  "\n";
   // std::cout << "number of eod: " << postingList->posts << "\n";

   // seek from synchronization point to target
   while ( actualLocation < target )
      {
      if ( offset + 1 < postingList->posts )
         {
         // actualLocation += GetCustomUtf8((*list)[offset+1].getData());
         size_t delta = GetCustomUtf8(reinterpret_cast<const Utf8*>(postingList->getPost(offset+1)));
         SetDocumentLength(delta);
         actualLocation += delta;
         offset ++;
         matchingDocument = offset; // set doc id of this document
         }
      else
         return nullptr;
      }

   // update start location
   start = actualLocation;
   curr = postingList->getPost(offset);
   matchingDocument = offset;
   return curr;
   }

   

const SerialPost *ISREndDoc::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
   return Seek( start + 1 );  
   }

const SerialPost *ISREndDoc::Next()
   {
   return Seek( start + 1 );  
   }

// set document, title, url lengths
void ISREndDoc::SetDocumentLength(size_t length)
   {
   documentLength = length;
   }

void ISREndDoc::SetTitleLength(size_t length)
   {
   titleLength = length;
   }

void ISREndDoc::SetUrlLength(size_t length)
   {
   urlLength = length;
   }

// get document, title, url lengths
size_t ISREndDoc::GetDocumentLength()
   {
   return documentLength;
   }

size_t ISREndDoc::GetTitleLength()
   {
   return titleLength;
   }

size_t ISREndDoc::GetUrlLength()
   {
   return urlLength;
   }


// ISRContainer

// Do Open ISR in index ( including assign Contained and Excluded )

// ISRContainer::ISRContainer( unsigned int countContained, unsigned int countExcluded )
//    : CountContained( countContained ), CountExcluded( countExcluded )
//    {
//    // set ISR
//    if ( CountContained > 0 )
//       {
//       Contained = new ISR*[CountContained];  
//       for ( unsigned int i = 0; i < CountContained; i ++ )
//          Contained[ i ] = new ISR( );  
//       }
   
//    if ( CountExcluded > 0 )
//       {
//       Excluded = new ISR*[countExcluded];  
//       for ( unsigned int i = 0; i < countExcluded; i ++ )
//          Excluded[ i ] = new ISR( );  
//       }

//    EndDoc = new ISREndDoc( );  

//    }


// ISRContainer::~ISRContainer( )
//    {
//    for ( int i = 0; i < CountContained; i ++ )
//       delete Contained[ i ];  
//    delete[ ] Contained;  

//    for ( int i = 0; i < CountExcluded; i ++ )
//       delete Excluded[ i ];  
//    delete[ ] Excluded;  

//    delete EndDoc;  
//    }


const SerialPost *ISRContainer::Seek( Location target )
   {
   bool found = false;  

   while ( !found )
      {
      // seek all the included ISRs to the first occurrence beginning at the target location
      nearestStartLocation = INT_MAX;
      farthestStartLocation = 0;
      for (int i = 0; i < CountContained; i++)
      {
         const SerialPost *result = Contained[ i ]->Seek( target );  
         if ( result == nullptr )
            return nullptr;  // if any ISR reaches the end, there is no match
         // update nearest and farthest terms
         Location loc = Contained[ i ]->GetStartLocation();  
         if ( loc < nearestStartLocation )
            {
            nearestContained = i;
            nearestStartLocation = loc;
            }
         if ( loc > farthestStartLocation )
            {
            farthestContained = i;
            farthestStartLocation = loc;
            }
         }
      for ( int i = 0; i < CountExcluded; i ++ )
         Excluded[ i ]->Seek( target );  

      Location docEnd, docBegin;

      while ( true )
         {
         // move the document end ISR to just past the furthest contained ISR
         const SerialPost *result = EndDoc->Seek( Contained[ farthestContained ]->GetStartLocation( ) + 1 );  
         if ( result == nullptr )
            return nullptr;  // if any ISR reaches the end, there is no match
         
         // calculate document begin location
         docEnd = EndDoc->GetStartLocation( );  
         docBegin = docEnd - EndDoc->GetDocumentLength( );  

         // seek all the other contained terms to past the document begin
         bool allWithinDoc = true;
         nearestStartLocation = INT_MAX;
         farthestStartLocation = 0;
         for ( int i = 0; i < CountContained; i ++ )
            {
            const SerialPost *result = Contained[ i ]->Seek( docBegin );  
            if ( result == nullptr )
               return nullptr;  // if any ISR reaches the end, there is no match
            // update nearest and farthest terms
            Location loc = Contained[ i ]->GetStartLocation();  
            if ( loc < nearestStartLocation )
               {
               nearestContained = i;
               nearestStartLocation = loc;
               }
            if ( loc > farthestStartLocation )
               {
               farthestContained = i;
               farthestStartLocation = loc;
               }

            if ( Contained[ i ]->GetStartLocation() > docEnd )
               allWithinDoc = false;  // not in this document
            }

         // if all ISRs within the document, break the loop
         if ( allWithinDoc )
            break;
         }

      // seek all the excluded ISRs to the first occurrence beginning at the document begin location
      found = true;
      for ( int i = 0; i < CountExcluded; i ++ )
         {
         const SerialPost *result = Excluded[ i ]->Seek( docBegin );  
         
         if ( result != nullptr && Excluded[ i ]->GetStartLocation() < docEnd ) // if any excluded ISR falls within the document
            {
            // reset the target to one past the end of the document
            target = docEnd + 1;
            found = false;  
            break;  
            }
         }
      }

   // find document
   matchingDocument = EndDoc->GetMatchingDoc();

   return Contained[0]->GetCurrentPost();  // return something, not nullptr
   }


const SerialPost *ISRContainer::Next( )
   {
   return Seek( Contained[ nearestContained ]->GetStartLocation( ) + 1 );
   }

const SerialPost *ISRContainer::NextDocument()
   {
   return Seek( EndDoc->GetStartLocation( ) + 1 );
   }


// ISRAnd

const SerialPost *ISRAnd::Seek( Location target )
   {
   // Seek all the ISRs to the first occurrence beginning at the target location.
   nearestStartLocation = INT_MAX;
   farthestStartLocation = 0;
   for ( int i = 0; i < NumberOfTerms; i ++ )
      {
      const SerialPost *result = Terms[ i ]->Seek( target );  
      if ( result == nullptr )
         return nullptr;  // if any ISR reaches the end, there is no match
      // update nearest and farthest terms
      Location loc = Terms[ i ]->GetStartLocation();  
      if ( loc < nearestStartLocation )
         {
         nearestTerm = i;  
         nearestStartLocation = loc;  
         }
      if ( loc > farthestStartLocation )
         {
         farthestTerm = i;  
         farthestStartLocation = loc;  
         }
      }

   Location docEnd, docBegin;  

   while ( true )
      {
      // move the document end ISR to just past the farthest word ISR
      const SerialPost *result = EndDoc->Seek( Terms[ farthestTerm ]->GetStartLocation( ) + 1 );  
      if ( result == nullptr )
         return nullptr;  // if any ISR reaches the end, there is no match
      // calculate document begin location
      docEnd = EndDoc->GetStartLocation( );  
      docBegin = docEnd - EndDoc->GetDocumentLength( ); 
      // seek all the other terms to past the document begin
      bool allWithinDoc = true;
      nearestStartLocation = INT_MAX;
      farthestStartLocation = 0;
      for ( int i = 0; i < NumberOfTerms; i ++ )
         {
         const SerialPost *result = Terms[ i ]->Seek( docBegin ); 
         if ( result == nullptr ) {
            return nullptr; // if any ISR reaches the end, there is no match
         }
            
         // update nearest and farthest terms
         Location loc = Terms[ i ]->GetStartLocation();  
         if ( loc < nearestStartLocation )
            {
            nearestTerm = i;  
            nearestStartLocation = loc;  
            }
         if ( loc > farthestStartLocation )
            {
            farthestTerm = i;  
            farthestStartLocation = loc;  
            }

         if ( Terms[ i ]->GetStartLocation() > docEnd )
            allWithinDoc = false;  // not in this document
         }
      // if all ISRs within the document, break the loop
      if ( allWithinDoc )
         break;
      }

   // find document
   matchingDocument = EndDoc->GetMatchingDoc();

   return Terms[0]->GetCurrentPost(); // return something, not nullptr
   }

const SerialPost *ISRAnd::Next()
   {
   return Seek( nearestStartLocation + 1 );  
   }


const SerialPost *ISRAnd::NextDocument()
   {
   return Seek( EndDoc->GetStartLocation( ) + 1 );
   }

// ISROr
const SerialPost *ISROr::Seek( Location target )
   {
      nearestStartLocation = INT_MAX;
      // nearestEndLocation = 0;

      bool flag = false; // if all terms reach the end, return nullptr

      for ( int i = 0; i < NumberOfTerms; i ++ )
         {
         if ( Terms[ i ] != nullptr ) 
            {
            const SerialPost *result = Terms[ i ] -> Seek(target);
            if ( result )
               {
               flag = true;
               Location loc = Terms[ i ]->GetStartLocation();
               if ( loc <= nearestStartLocation )
                  {
                  nearestTerm = i;
                  nearestStartLocation = loc;
                  }
               }
            }
         }
      
      if ( !flag )
         return nullptr;

      matchingDocument = Terms[ nearestTerm ] -> GetMatchingDoc();
      EndDoc->Seek(Terms[nearestTerm]->EndDoc->GetStartLocation() - 1);
      return Terms[nearestTerm]->GetCurrentPost();
   }

const SerialPost *ISROr::NextDocument()
   {
   return Seek( EndDoc->GetStartLocation( ) + 1 );
   }

const SerialPost *ISROr::Next()
   {
   return Seek( nearestStartLocation + 1 );  
   }

Location ISROr::GetStartLocation()
   {
      return nearestStartLocation;
   }

Location ISROr::GetEndLocation()
   {
      return nearestEndLocation;
   }


// ISRPhrase

const SerialPost *ISRPhrase::Seek( Location target )
   {
      nearestStartLocation = INT_MAX;
      nearestEndLocation = 0;
      for (int i = 0; i < NumberOfTerms; i++)
      {
         const SerialPost *result = Terms[ i ]->Seek(target);

         if ( result == nullptr )
            return nullptr;

         Location loc = Terms[ i ]->GetStartLocation();
         if (loc < nearestStartLocation)
            {
            nearestStartLocation = loc;
            nearestTerm = i;
            }
         if (loc > nearestEndLocation)
            {
            nearestEndLocation = loc;
            farthestTerm = i;
            }
         }
      // 1, 5;3, 6;2, 7 
      // next: new target = neareststartloc + 1
      // neareststartloc: 
      // std::cout<<"nearestStartLocation: "<<nearestStartLocation<<std::endl;
      // std::cout << Terms[0]->GetStartLocation() << std::endl;
      // std::cout << Terms[1]->GetStartLocation() << std::endl;

      for ( int i = 1; i < NumberOfTerms; i ++ )
         {
         if (Terms[i]->GetStartLocation() != Terms[i - 1]->GetStartLocation() + 1 )
            return Next();
         }

      Location docEnd, docBegin;
      const SerialPost *result = EndDoc->Seek(Terms[farthestTerm]->GetStartLocation() + 1);
      if ( result == nullptr )
         return nullptr;

      docEnd = EndDoc->GetStartLocation( );  
      docBegin = docEnd - EndDoc->GetDocumentLength( );  

      bool allWithinDoc = true;
      for (unsigned i = 0; i < NumberOfTerms; ++i)
         {
         if (Terms[i]->GetStartLocation() < docBegin || Terms[i]->GetStartLocation() > docEnd)
            {
            allWithinDoc = false;
            break;
            }
         }

      if (allWithinDoc)
         {
         matchingDocument = EndDoc->GetMatchingDoc();
         return Terms[0]->GetCurrentPost();
         }
      else
         return Next();
   }

const SerialPost *ISRPhrase::Next()
   {
   return Seek( nearestStartLocation + 1 );
   }


const SerialPost *ISRPhrase::NextDocument()
   {
   return Seek( EndDoc->GetStartLocation( ) + 1 );
   }