#ifndef FHhashQPwFind_h
#define FHhashQPwFind_h

#include "FHhashQP.h"

/****************************************
 *
 * Class Prototype
 *
 ****************************************/

template <class Object, typename KeyType>
class FHhashQPwFind : public FHhashQP<Object> {
protected:
   int myHashKey(const KeyType& key) const;
   int findPosKey( const KeyType& key ) const;
public:
   const Object find(const KeyType& key);
   
   class NotFoundException {};
};

#endif /* FHhashQPwFind_h */

/****************************************
 *
 * Protected Member Function Definitions
 *
 ****************************************/

template <class Object, typename KeyType>
int FHhashQPwFind<Object, KeyType>::myHashKey(const KeyType& key) const {
   
   int hashVal = Hash(key) % this->mTableSize;
   if (hashVal < 0)
      hashVal += this->mTableSize;
   
   return hashVal;
}

template <class Object, typename KeyType>
int FHhashQPwFind<Object, KeyType>::findPosKey(const KeyType &key) const {
   int kthOddNum = 1;
   int index = myHashKey(key);
   
   while ( this->mArray[index].state        != this->EMPTY and
          getKey(this->mArray[index].data) != key ) {
      index     += kthOddNum;    // k squared = (k-1) squared + kth odd #
      kthOddNum += 2;            // compute next odd #
      if ( index >= this->mTableSize )
         index -= this->mTableSize;
   }
   return index;
}

/****************************************
 *
 * Public Member Function Definitions
 *
 ****************************************/

template <class Object, typename KeyType>
const Object FHhashQPwFind<Object, KeyType>::find(const KeyType& key) {
   int index = findPosKey(key);
   typename FHhashQPwFind<Object, KeyType>::HashEntry found = this->mArray[index];
   
   if (found.state == this->EMPTY)
      throw typename FHhashQPwFind<Object, KeyType>::NotFoundException();
   return found.data;
}



   //// Client ////



#include "EBookEntry.h"

int getKey( const EBookEntry & item) {
   return item.getETextNum();
}

   //string getKey(const EBookEntry& item) {
   //   return item.getCreator();
   //}

int Hash(const int& key) {
   return key;
}

int Hash(const string& key) {
   unsigned int retVal = 0;
   for(unsigned int k = 0; k < key.length( ); k++ )
      retVal = 37 * retVal + key[k];
   return retVal;
}

int Hash(const EBookEntry& item) {
   return Hash(item.getETextNum());
}

   //int Hash(const EBookEntry& item) {
   //   return Hash(item.getCreator());
   //}

#include <iostream>
#include <cstdlib>
#include "FHhashQPwFind.h"

#define NUM_RANDOM_INDICES 10

int main() {
   
   string filePath("/Users/justin/Documents/Documents/Projects/XCode Projects/CS2C/ADT Files/txt files/catalog-short4.txt");
   EBookEntryReader bookInput(filePath);
   int numBooks = bookInput.getNumBooks();
   cout << bookInput.getFileName() << endl
   << numBooks                << endl;
   
   FHhashQPwFind<EBookEntry, int> hashTable; // for ID equality
   EBookEntry::setSortType(EBookEntry::SORT_BY_ID);
      //      FHhashQPwFind<EBookEntry, string> hashTable; // for any string equality
      //      EBookEntry::setSortType(EBookEntry::SORT_BY_CREATOR);
   
      // populate hashTable
   for (int i = 0 ; i < numBooks ; ++i)
      hashTable.insert(bookInput[i]);
   
   cout << "hashTable size = " << hashTable.size() << endl;
   
      // generate array of random indices
   int randomIndices[NUM_RANDOM_INDICES];
   srand(static_cast<unsigned int>(time(NULL)));
   for (int& val : randomIndices)
      val = rand() % numBooks;
   
      // attempt to find on the selected key
   cout << "The same random books from the hash table \n" << endl;
   for (int k = 0; k < NUM_RANDOM_INDICES; k++) {
      try {
         EBookEntry book =
         hashTable.find( bookInput[ randomIndices[k] ].getETextNum() );
            //         EBookEntry book =
            //         hashTable.find( bookInput[ randomIndices[k] ].getCreator() );
         
         cout << "(#" << k << ") " << book.getTitle() << endl
         << book.getCreator() << endl
         << book.getSubject() << endl
         << book.getETextNum() << endl;
      } catch (...) {
         cout << "not found" << endl;
      } cout << endl;
   }
   
      // test known failures exceptions:
   try {
      EBookEntry book = hashTable.find( -3 );
         //       EBookEntry book = hashTable.find( "Jack Kerouac" );
      
   } catch (...) {
      cout << "not found" << endl;
   }
   
   try {
      EBookEntry book = hashTable.find( 0 );
         //      EBookEntry book = hashTable.find( "Oprah Winfrey" );
      
   } catch (...) {
      cout << "not found" << endl;
   }
   
   try {
      EBookEntry book = hashTable.find( -1 );
         //      EBookEntry book = hashTable.find( "Donald Trump" );
      
   } catch (...) {
      cout << "not found" << endl;
   }
   
}

/*
 Sample Run





   //// Run w/ Strings ////

/Users/justin/Documents/Documents/Projects/XCode Projects/CS2C/ADT Files/txt files/catalog-short4.txt
550
hashTable size = 472
The same random books from the hash table

(#0)
Velasquez, Pedro
Central America -- Antiquities
29388

(#1) Comedies by Holberg : Jeppe of the Hill, The Political Tinker, Erasmus Montanus
Holberg, Ludvig, baron, 1684-1754
Readers (Primary)
5749

(#2) Eine Schwierigkeit der Psychoanalyse
Freud, Sigmund, 1856-1939
Psychoanalysis
29097

(#3) The Diplomatic Correspondence of the American Revolution, Vol. IX
Starzl, Roman Frederick, 1899-1976
United States -- Foreign relations -- 1775-1783
29438

(#4) Three Years in the Sixth Corps
A Concise Narrative of Events in the Army of the Potomac,
from 1861 to the Close of the Rebellion, April, 1865
Stevens, George T.
Chinese -- California -- San Francisco
21976

(#5) National Strategy for Combating Terrorism
February 2003
United States. Executive Office of the President
United States -- Military policy
29185

(#6) Over the top with the 25th
Chronicle of events at Vimy Ridge and Courcellette
Lewis, R.
World War, 1914-1918 -- Personal narratives, Canadian
26930

(#7) Stalky &amp; Co.
Kipling, Rudyard, 1865-1936
Schools -- Fiction
28940

(#8) The Bad Child's Book of Beasts
Belloc, Hilaire, 1870-1953
Children's poetry, English
27175

(#9) Selections from the Speeches and Writings of Edmund Burke
Burke, Edmund, 1729-1797
Essays
3286

not found
not found
not found
Program ended with exit code: 0



   //// Run w/ ints ////

/Users/justin/Documents/Documents/Projects/XCode Projects/CS2C/ADT Files/txt files/catalog-short4.txt
550
hashTable size = 550
The same random books from the hash table

(#0) Punch, or the London Charivari, Volume 152, February 28, 1917
Various
English wit and humor -- Periodicals
14639

(#1) An Orkney Maid
Barr, Amelia Edith Huddleston, 1831-1919
Orkney (Scotland) -- Fiction
29752

(#2) The Tiger
Blake, William, 1757-1827
Poetry
21177

(#3) Our American Cousin
Taylor, Tom, 1817-1880
Comedies
3158

(#4) The Stretton Street Affair
Le Queux, William, 1864-1927
Detective and mystery stories
27147

(#5) The Harbor
Poole, Ernest, 1880-1950
Fiction
29932

(#6) Kit of Greenacre Farm
Forrester, Izola L. (Izola Louise), 1878-1944
English wit and humor -- Periodicals
15029

(#7) On the Portraits of English Authors on Gardening,
with Biographical Notices of Them, 2nd edition, with
considerable additions
Felton, Samuel
Gardening -- Bibliography
25773

(#8) Nach Amerika! Fünfter Band
Ein Volksbuch
Gerstäcker, Friedrich, 1816-1872
PT
29102

(#9) The Children's Book of Christmas Stories
Wit, Augusta de, 1864-1939
Christmas stories
28308

not found
not found
not found
Program ended with exit code: 0





