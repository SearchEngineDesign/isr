#pragma once
#include "../index/index.h"
#include "../utils/IndexBlob.h"

class ISREndDoc;

class ISR
{
public:
   virtual const SerialPost *Next() = 0; // -> 
   virtual const SerialPost *NextDocument() = 0; // -> return 
   virtual const SerialPost *Seek(Location target) = 0; // -> return delta

   Location GetStartLocation(); // -> return start
   Location GetEndLocation(); // -> return end
   size_t GetMatchingDoc(); // return result

   unsigned GetDocumentCount();
   unsigned GetNumberOfOccurrences();

   const SerialPost *GetCurrentPost();
   void SetCurrentPost(const SerialPost *p);
   void SetPostingList( const SerialPostingList *pl );

   ISREndDoc *EndDoc;

protected:
   const SerialPostingList *postingList; 
   const SerialPost *curr; // current post 
   Location start = 0, end = 0;

   size_t matchingDocument; // final result
};

class ISREndDoc
{
public:
   const SerialPost *Seek(Location target); // -> return delta
   const SerialPost *NextDocument(); // -> return 
   const SerialPost *Next();

   // type-specific data
   size_t GetDocumentLength();
   size_t GetTitleLength();
   size_t GetUrlLength();

   void SetDocumentLength(size_t length);
   void SetTitleLength(size_t length);
   void SetUrlLength(size_t length);

   Location GetStartLocation(); // -> return start
   Location GetEndLocation(); // -> return end
   size_t GetMatchingDoc(); // return result

   const SerialPost *GetCurrentPost();
   void SetCurrentPost(const SerialPost *p);
   void SetPostingList( const SerialPostingList *pl );

private:
   size_t documentLength = 0, titleLength = 0, urlLength = 0;
   const SerialPostingList *postingList; 
   const SerialPost *curr; // current post 
   Location start = 0, end = 0;
   size_t matchingDocument;

};


class ISRWord : public ISR
{
public:
   const SerialPost *Next(); // -> 
   const SerialPost *NextDocument(); // -> return 
   const SerialPost *Seek(Location target); // -> return delta
   // ISREndDoc *EndDoc;

};



class ISROr : public ISR
{
public:
   ISR **Terms;
   unsigned int NumberOfTerms;
   Location GetStartLocation();
   Location GetEndLocation();
   const SerialPost *Seek(Location target);
   const SerialPost *Next();
   const SerialPost *NextDocument();

   // ISREndDoc *EndDoc;

private:
   unsigned nearestTerm = 0;
   Location nearestStartLocation, nearestEndLocation;
};

class ISRAnd : public ISR
{
public:
   ISR **Terms;
   unsigned int NumberOfTerms;
   const SerialPost *Seek(Location target);
   const SerialPost *Next();
   const SerialPost *NextDocument();
   // ISREndDoc *EndDoc;

private:
   unsigned int nearestTerm = 0, farthestTerm = 0;
   Location nearestStartLocation, farthestStartLocation;
};

class ISRPhrase : public ISR
{
public:
   ISR **Terms;
   unsigned int NumberOfTerms;
   const SerialPost *Seek(Location target);
   const SerialPost *Next();
   const SerialPost *NextDocument();

   // ISREndDoc *EndDoc;

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
   const SerialPost *Seek( Location target );
   const SerialPost *Next( ); // next container isr
   const SerialPost *NextDocument();

   ISR **Contained, **Excluded;
   // ISREndDoc *EndDoc;
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