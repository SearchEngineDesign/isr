#include "../index/index.h"

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
   virtual Post *Next(); // -> 
   virtual Post *NextDocument(); // -> return 
   virtual Post *Seek(Location target); // -> return delta
   virtual Location GetStartLocation(); // -> return start  
   virtual Location GetEndLocation(); // -> return end
   virtual size_t GetMatchingDoc(); // return result

protected:
   PostingList *postingList; 
   Post *curr; // current post 
   Location start, end;

   size_t matchingDocument; // final result
};

class ISREndDoc : public ISR
{
public:
   Post *Seek(Location target); // -> return delta
   Post *NextDocument(); // -> return 
   Location GetStartLocation(); // -> return start
   size_t GetMatchingDoc(); // return result  

   unsigned GetDocumentCount();
   virtual Post *GetCurrentPost();
   void SetCurrentPost(Post *p);
   void SetPostingList( PostingList *pl );

   // type-specific data
   unsigned GetDocumentLength();
   unsigned GetTitleLength();
   unsigned GetUrlLength();

   unsigned SetDocumentLength(size_t length);
   unsigned SetTitleLength(size_t length);
   unsigned SetUrlLength(size_t length);

private:
   size_t documentLength = 0, titleLength = 0, urlLength = 0;

};


class ISRWord : public ISR
{
public:
   Post *Next(); // -> 
   Post *NextDocument(); // -> return 
   Post *Seek(Location target); // -> return delta
   Location GetStartLocation(); // -> return start  
   Location GetEndLocation(); // -> return end
   size_t GetMatchingDoc(); // return result

   unsigned GetDocumentCount();
   unsigned GetNumberOfOccurrences();
   virtual Post *GetCurrentPost();
   void SetCurrentPost(Post *p);

   void SetPostingList( PostingList *pl );

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

   ISRContainer( unsigned int countContained, unsigned int countExcluded );  // TODO: think about init
   ~ISRContainer( );  

   // Location Next( );
   Post *Seek( Location target );
   Post *Next( ); // next container isr

   ISR **Contained,*Excluded;
   ISREndDoc *EndDoc;
   unsigned int CountContained, CountExcluded;

private:
   unsigned int nearestContained, farthestTerm;
   Location nearestStartLocation, nearestEndLocation;
};