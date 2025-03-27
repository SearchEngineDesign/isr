#pragma once
#include "../index/index.h"

class ISR
{
public:
   virtual const Post *Next(); // -> 
   virtual const Post *NextDocument(); // -> return 
   virtual const Post *Seek(Location target); // -> return delta

   Location GetStartLocation(); // -> return start
   Location GetEndLocation(); // -> return end
   size_t GetMatchingDoc(); // return result

   unsigned GetDocumentCount();
   unsigned GetNumberOfOccurrences();

   const Post *GetCurrentPost();
   void SetCurrentPost(Post *p);
   void SetPostingList( PostingList *pl );

protected:
   PostingList *postingList; 
   const Post *curr; // current post 
   Location start, end;

   size_t matchingDocument; // final result
};

class ISREndDoc : public ISR
{
public:
   const Post *Seek(Location target); // -> return delta
   const Post *NextDocument(); // -> return 

   // type-specific data
   size_t GetDocumentLength();
   size_t GetTitleLength();
   size_t GetUrlLength();

   void SetDocumentLength(size_t length);
   void SetTitleLength(size_t length);
   void SetUrlLength(size_t length);

private:
   size_t documentLength = 0, titleLength = 0, urlLength = 0;

};


class ISRWord : public ISR
{
public:
   const Post *Next(); // -> 
   const Post *NextDocument(); // -> return 
   const Post *Seek(Location target); // -> return delta

private:
   ISREndDoc *DocumentEnd;
};



class ISROr : public ISR
{
public:
   ISR **Terms;
   unsigned NumberOfTerms;
   Location GetStartLocation();
   Location GetEndLocation();
   Post *Seek(Location target);
   Post *Next();
   Post *NextDocument();

private:
   unsigned nearestTerm;
   Location nearestStartLocation, nearestEndLocation;
};

class ISRAnd : public ISR
{
public:
   ISR **Terms;
   unsigned NumberOfTerms;
   Post *Seek(Location target);
   Post *Next()
   {
      return Seek(nearestStartLocation + 1);
   }

private:
   unsigned nearestTerm, farthestTerm;
   Location nearestStartLocation, nearestEndLocation;
};

class ISRPhrase : public ISR
{
public:
   ISR **Terms;
   unsigned NumberOfTerms;
   Post *Seek(Location target);
   Post *Next()
   {
      // Finds overlapping phrase matches.
      return Seek(nearestStartLocation + 1);
   }

private:
   unsigned nearestTerm, farthestTerm;
   Location nearestStartLocation, nearestEndLocation;
};


class ISRContainer : public ISR
{
public:

   // ISRContainer( unsigned int countContained, unsigned int countExcluded );  // TODO: think about init
   // ~ISRContainer( );  

   // TODO: open ISR function in index

   // Location Next( );
   const Post *Seek( Location target );
   const Post *Next( ); // next container isr

   ISR **Contained, **Excluded;
   ISREndDoc *EndDoc;
   unsigned int CountContained, CountExcluded;

private:
   unsigned int nearestContained = 0, farthestContained = 0;
   // Location nearestStartLocation, farthestStartLocation;
};


class Dictionary
{
public:
   ISR *OpenIsr(char *token);
   // The first line of index.txt, in order
   size_t GetNumberOfWords();
   size_t GetNumberOfUniqueWords(); // number of posting list
   size_t GetNumberOfDocuments();

private:
   size_t numOfWords, numOfUniqueWords, numOfDocuments;
   size_t SetNum(size_t numOfWords, size_t numOfUniqueWords, size_t numOfDocuments);
};