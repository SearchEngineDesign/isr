#include "isr.h"

// ISRWord

Post *ISRWord::Next( )
   {
   // do a next on the term, then return match
   return curr ++;
   }

Post *ISRWord::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 8;
   std::pair<size_t, size_t> p = postingList->getSeekTable()[highBits];
   size_t offset = p.first;
   size_t actualLocation = p.second;

   // seek from synchronization point to target
   while ( actualLocation < target )
      {
      actualLocation += GetCustomUtf8(postingList->getList()[ offset + 1 ].getData());
      offset ++;
      }

   // update start location
   start = actualLocation;
   curr = &postingList->getList( )[ offset ];

   // TODO: update end(?)
   
   // update end of document

   if ( actualLocation > DocumentEnd->GetStartLocation( ) )
      DocumentEnd->NextDocument();
   
   matchingDocument = DocumentEnd->docID;

   return curr;
   }

Post *ISRWord::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
   return Seek( DocumentEnd->GetStartLocation( ) + 1 );  
   }


Location ISRWord::GetStartLocation()
   {
      return start;
   }

Location ISRWord::GetEndLocation()
   {
      return end;
   }

unsigned ISRWord::GetDocumentCount()
   {
      return postingList->getDocCount();
   }

unsigned ISRWord::GetNumberOfOccurrences()
   {
      return postingList->getUseCount();
   }

Post *ISRWord::GetCurrentPost()
   {
      return curr;
   }

size_t ISRWord::GetMatchingDoc()
   {
   return matchingDocument;
   }



// ISREndDoc

Post *ISREndDoc::Seek ( Location target )
   {
   // Seek ISR to the first occurrence beginning at the target location

   // use synchronization point
   uint8_t highBits = target >> 8;
   std::pair<size_t, size_t> p = postingList->getSeekTable()[highBits];
   size_t offset = p.first;
   size_t actualLocation = p.second;

   // seek from synchronization point to target
   while ( actualLocation < target )
      {
      actualLocation += GetCustomUtf8(postingList->getList()[ offset + 1 ].getData());
      offset ++;
      }

   // update start location
   start = actualLocation;
   curr = &postingList->getList( )[ offset ];
   matchingDocument = offset;


   return curr;
   }

Post *ISREndDoc::NextDocument( )
   {
   // Seek ISR to the first occurrence just past the end of this document
   return Seek( start + 1 );  
   }


Location ISREndDoc::GetStartLocation()
   {
   return start;
   }

unsigned ISREndDoc::GetDocumentCount()
   {
      return postingList->getDocCount();
   }


Post *ISREndDoc::GetCurrentPost()
   {
      return curr;
   }

// set current post to p
void ISREndDoc::SetCurrentPost(Post *p) 
   {
   curr = p;
   }


// set document, title, url lengths
unsigned ISREndDoc::SetDocumentLength(size_t length)
   {
   documentLength = length;
   }

unsigned ISREndDoc::SetTitleLength(size_t length)
   {
   titleLength = length;
   }

unsigned ISREndDoc::SetUrlLength(size_t length)
   {
   urlLength = length;
   }

// get document, title, url lengths
unsigned ISREndDoc::GetDocumentLength()
   {
   return documentLength;
   }

unsigned ISREndDoc::GetTitleLength()
   {
   return titleLength;
   }

unsigned ISREndDoc::GetUrlLength()
   {
   return urlLength;
   }



ISRContainer::ISRContainer( unsigned int countContained, unsigned int countExcluded )
   : CountContained( countContained ), CountExcluded( countExcluded )
   {
   // set ISR
   if ( CountContained > 0 )
      {
      Contained = new ISR*[CountContained];  
      for ( unsigned int i = 0; i < CountContained; i ++ )
         Contained[ i ] = new ISR( );  
      }
   
   if ( CountExcluded > 0 )
      Excluded = new ISR( );  

   EndDoc = new ISREndDoc( );  


   // set nearest location and term
   }


ISRContainer::~ISRContainer( )
   {
   for ( int i = 0; i < CountContained; i ++ )
      delete Contained[ i ];  
   delete[ ] Contained;  
   delete Excluded;  
   delete EndDoc;  
   }


Post *ISRContainer::Seek( Location target )
   {
   // seek all the included ISRs to the first occurrence beginning at the target location
   for ( int i = 0; i < CountContained; i ++ )
      {
      Contained[ i ]->Seek( target );  
      }

   // move the document end ISR to just past the furthest contained ISR
   EndDoc->Seek( Contained[ farthestTerm ]->GetStartLocation( ) + 1 );  
   }


Post *ISRContainer::Next( )
   {
   return Seek( Contained[ nearestContained ]->GetStartLocation( ) + 1 );
   }


