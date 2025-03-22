#include "index/index.h"

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

class ISR
{
public:
   virtual Post *Next();
   virtual Post *NextDocument();
   virtual Post *Seek(Location target);
   virtual Location GetStartLocation();
   virtual Location GetEndLocation();
};

class ISRWord : public ISR
{
public:
   unsigned GetDocumentCount();
   unsigned GetNumberOfOccurrences();
   virtual Post *GetCurrentPost();
};

class ISREndDoc : public ISRWord
{
public:
   unsigned GetDocumentLength();
   unsigned GetTitleLength();
   unsigned GetUrlLength();
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
   ISR **Contained,*Excluded;
   ISREndDoc *EndDoc;
   unsigned CountContained, CountExcluded;
   Post *Seek( Location target );
   Post *Next( )
   {
   Seek( Contained[ nearestContained ]->GetStartlocation( ) + 1 );
   }

private:
   unsigned nearestTerm, farthestTerm;
   Location nearestStartLocation, nearestEndLocation;
};