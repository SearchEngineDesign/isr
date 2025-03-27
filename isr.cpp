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
      actualLocation += GetCustomUtf8((*list)[offset+1].getData()); // TODO: unsigned char* and char*
      offset ++;
      }

   // update start location
   start = actualLocation;
   curr = &(*list)[offset];

   // TODO: update end(?)
   
   // update end of document

   if ( actualLocation > DocumentEnd->GetStartLocation( ) )
      DocumentEnd->NextDocument();
   
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


const Post *ISRContainer::Seek( Location target )
   {
   // seek all the included ISRs to the first occurrence beginning at the target location
   for ( int i = 0; i < CountContained; i ++ )
      {
      Contained[ i ]->Seek( target );  
      }

   // move the document end ISR to just past the furthest contained ISR
   EndDoc->Seek( Contained[ farthestTerm ]->GetStartLocation( ) + 1 );  
   }


const Post *ISRContainer::Next( )
   {
   return Seek( Contained[ nearestContained ]->GetStartLocation( ) + 1 );
   }


