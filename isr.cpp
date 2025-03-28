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
   return postingList->getDocCount();
   }

unsigned ISR::GetNumberOfOccurrences()
   {
   return postingList->getUseCount();
   }

const Post *ISR::GetCurrentPost()
   {
   return curr;
   }

void ISR::SetCurrentPost( Post *p )
   {
   curr = p;
   }

void ISR::SetPostingList( PostingList *pl )
   {
   postingList = pl;
   }


// ISRWord

const Post *ISRWord::Next( )
   {
   // do a next on the term, then return match
   return curr ++;
   }

const Post *ISRWord::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 8;
   std::pair<size_t, size_t> p = postingList->getSeekTable()[highBits];
   size_t offset = p.first;
   size_t actualLocation = p.second;

   const vector<Post> *list = postingList->getList();
   // seek from synchronization point to target
   while ( actualLocation < target )
      {
      if ( offset + 1 < (*list).size() ) 
         {
         actualLocation += GetCustomUtf8((*list)[offset+1].getData()); // TODO: unsigned char* and char*
         offset ++;
         }
      else
         {
         // reach the end and no match, return nullptr
         return nullptr;
         }
      }

   // update start location
   start = actualLocation;
   curr = &(*list)[offset];

   // TODO: update end(?)
   
   // update end of document

   if ( actualLocation > DocumentEnd->GetStartLocation( ) )
      DocumentEnd->Seek( actualLocation );
   
   matchingDocument = DocumentEnd->GetMatchingDoc();

   return curr;
   }

const Post *ISRWord::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
   return Seek( DocumentEnd->GetStartLocation( ) + 1 );  
   }


// ISREndDoc

const Post *ISREndDoc::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 8;
   std::pair<size_t, size_t> p = postingList->getSeekTable()[highBits];
   size_t offset = p.first;
   size_t actualLocation = p.second;

   const vector<Post> *list = postingList->getList();
   // seek from synchronization point to target
   while ( actualLocation < target )
      {
      actualLocation += GetCustomUtf8((*list)[offset+1].getData());
      offset ++;
      }

   // update start location
   start = actualLocation;
   curr = &(*list)[offset];
   matchingDocument = offset;

   return curr;
   }

const Post *ISREndDoc::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
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


const Post *ISRContainer::Seek( Location target )
   {
   bool found = false;  

   while ( !found )
      {
      // seek all the included ISRs to the first occurrence beginning at the target location
      for ( int i = 0; i < CountContained; i ++ )
         {
         const Post *result = Contained[ i ]->Seek( target );  
         if ( result == nullptr )
            return nullptr;  // if any ISR reaches the end, there is no match
         // update nearest and farthest terms
         Location loc = Contained[ i ]->GetStartLocation();  
         if ( loc < Contained[ nearestContained ]->GetStartLocation( ) )
            nearestContained = i;  
         if ( loc > Contained[ farthestContained ]->GetStartLocation( ) )
            farthestContained = i;  
         }
      for ( int i = 0; i < CountExcluded; i ++ )
         Excluded[ i ]->Seek( target );  

      Location docEnd, docBegin;

      while ( true )
         {
         // move the document end ISR to just past the furthest contained ISR
         const Post *result = EndDoc->Seek( Contained[ farthestContained ]->GetStartLocation( ) + 1 );  
         if ( result == nullptr )
            return nullptr;  // if any ISR reaches the end, there is no match
         
         // calculate document begin location
         docEnd = EndDoc->GetStartLocation( );  
         docBegin = docEnd - EndDoc->GetDocumentLength( );  

         // seek all the other contained terms to past the document begin
         bool allWithinDoc = true;
         for ( int i = 0; i < CountContained; i ++ )
            {
            const Post *result = Contained[ i ]->Seek( docBegin );  
            if ( result == nullptr )
               return nullptr;  // if any ISR reaches the end, there is no match

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
         Excluded[ i ]->Seek( docBegin );  
         
         if ( Excluded[ i ]->GetStartLocation() < docEnd ) // if any excluded ISR falls within the document
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


const Post *ISRContainer::Next( )
   {
   return Seek( Contained[ nearestContained ]->GetStartLocation( ) + 1 );
   }


// ISRAnd

const Post *ISRAnd::Seek( Location target )
   {
   // Seek all the ISRs to the first occurrence beginning at the target location.
   for ( int i = 0; i < NumberOfTerms; i ++ )
      {
      const Post *result = Terms[ i ]->Seek( target );  
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
      const Post *result = EndDoc->Seek( Terms[ farthestTerm ]->GetStartLocation( ) + 1 );  
      if ( result == nullptr )
         return nullptr;  // if any ISR reaches the end, there is no match
      
      // calculate document begin location
      docEnd = EndDoc->GetStartLocation( );  
      docBegin = docEnd - EndDoc->GetDocumentLength( );  
   
      // seek all the other terms to past the document begin
      bool allWithinDoc = true;
      for ( int i = 0; i < NumberOfTerms; i ++ )
         {
         const Post *result = Terms[ i ]->Seek( docBegin );  
         if ( result == nullptr )
            return nullptr;  // if any ISR reaches the end, there is no match

         if ( Terms[ i ]->GetStartLocation() > docEnd )
            allWithinDoc = false;  // not in this document
         }

      // if all ISRs within the document, break the loop
      if ( allWithinDoc )
         break;
      }

   // find document
   matchingDocument = EndDoc->GetMatchingDoc();
   return Terms[0]->GetCurrentPost();  // return something, not nullptr
   }

const Post *ISRAnd::Next()
   {
   return Seek( nearestStartLocation + 1 );  
   }

const Post *ISRAnd::NextDocument()
   {
   return Seek( EndDoc->GetStartLocation( ) + 1 );  
   }

const Post *ISRPhrase::Seek(Location target) {
   // Step 1: Seek all ISRs to the first occurrence beginning at the target location
   for (unsigned i = 0; i < NumberOfTerms; i++) {
       const Post *p = Terms[i]->Seek(target);
       if (p == nullptr) {
           // If any ISR reaches the end, there is no match (Step 4)
           return nullptr;
       }
   }
   
   bool phraseFound = false;
   
   while (!phraseFound) {
       // Find the furthest term (Step 2)
       farthestTerm = 0;
       farthestStartLocation = Terms[0]->GetStartLocation();
       
       for (unsigned i = 1; i < NumberOfTerms; i++) {
           Location currentStart = Terms[i]->GetStartLocation();
           if (currentStart > farthestStartLocation) {
               farthestTerm = i;
               farthestStartLocation = currentStart;
           }
       }
       
       // Find the nearest term
       nearestTerm = 0;
       nearestStartLocation = Terms[0]->GetStartLocation();
       
       for (unsigned i = 1; i < NumberOfTerms; i++) {
           Location currentStart = Terms[i]->GetStartLocation();
           if (currentStart < nearestStartLocation) {
               nearestTerm = i;
               nearestStartLocation = currentStart;
           }
       }
       
       // Check if all terms are properly aligned for a phrase
       bool allAligned = true;
       
       for (unsigned i = 0; i < NumberOfTerms; i++) {
           // Calculate expected position: furthestTerm should be at position 
           // furthestStartLocation, so i should be at (furthestStartLocation - (farthestTerm - i))
           Location expectedLocation = farthestStartLocation - (farthestTerm - i);
           Location currentLocation = Terms[i]->GetStartLocation();
           
           if (currentLocation != expectedLocation) {
               allAligned = false;
               
               // Step 3: If any term is past the desired location, try again
               // Seek this term to the expected location
               const Post *p = Terms[i]->Seek(expectedLocation);
               
               if (p == nullptr) {
                   // If any ISR reaches the end, there is no match (Step 4)
                   return nullptr;
               }
               
               // If after seeking, the term is still not at expected location,
               // we need to start over with new positions
               break;
           }
       }
       
       // If all terms are aligned, we've found a matching phrase
       if (allAligned) {
           phraseFound = true;
       }
   }
   
   // Update phrase information
   nearestEndLocation = Terms[NumberOfTerms - 1]->GetEndLocation();
   
   // Set the current post
   curr = Terms[nearestTerm]->GetCurrentPost();
   
   // Return the current post
   return curr;
}