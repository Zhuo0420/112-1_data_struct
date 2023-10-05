// xhash internal header

#ifndef XHASH
#define XHASH

#include <list>
using std::list;

#include "type_traits.h"

// CLASS TEMPLATE VectorVal
template< typename ValueType >
class VectorVal             //hash table, array 中兩個元素為一個bucket，因為list是多個value組成的多個短的 list 所串成的一個長的 list，一個pointer指向單個list的頭，另一個指向尾，用來區分多段list
{
public:
   using value_type = ValueType;
   using pointer    = value_type *;

   VectorVal()
      : myFirst(),      //vector的第一個元素
        myLast(),       //vector有用到的最後一個元素的下一個
        myEnd()         //vector的最後一個元素的下一個
   {
   }

   pointer myFirst; // pointer to beginning of array
   pointer myLast;  // pointer to current end of sequence
   pointer myEnd;   // pointer to end of array
};

template< typename Iter >
struct HashVec
{
   using value_type = Iter;
   using size_type  = size_t;

   HashVec()
      : myData()    //此class一被建構就繼承myData (VecctorVal 的物件)
   {
   }

   // set the elements stored here to cells copies of val 將此處存儲的元素設置為儲存格 Val 的副本
   void assignGrow( const size_type cells, const value_type val )       //將呼叫此function的物件擴大至 cells 數量的 bucket
   {
       size_t oldsize = static_cast<size_type>(myData.myLast - myData.myFirst);
       size_type Osize = myData.myLast - myData.myFirst;
       value_type* newVec = new value_type[cells];
       //value_type* tmpArr = new value_type[Osize];
       if (oldsize != 0) {
           /*for (size_type i = 0; i != Osize; i++) {
               tmpArr[i] = myData.myFirst[i];
           }*/
           delete[] myData.myFirst;
       }
       myData.myFirst = newVec;
       myData.myEnd = newVec + cells;
       myData.myLast = newVec + cells;
       ::std::uninitialized_fill(newVec, newVec + cells, val);
       /*for (size_type i = 0; i != Osize; i++) {
           myData.myFirst[i] = tmpArr[i];
       }
       delete[] tmpArr;*/
   }

   ~HashVec()
   {
      if( myData.myFirst != nullptr )
         delete[] myData.myFirst;
   }

   VectorVal< value_type > myData;      //MyData為 VecctorVal 的物件，為vector中的元素
};

// CLASS TEMPLATE Hash
template< typename Traits >
class Hash // hash table -- list with vector of iterators for quick access (iterators = 記憶空間位址，使用方法如指標)
{
protected:
   using MyList         = list< typename Traits::value_type >;      //MyList 為 list< typename Traits::value_type >的別名 
   using KeyCompare     = typename Traits::key_compare;             //typename 為關鍵字，用來告訴編譯器接下來的名稱是一個類型名稱

public:
   using key_type       = typename Traits::key_type;
   using value_type     = typename MyList::value_type;
   using size_type      = typename MyList::size_type;

   using iterator       = typename MyList::const_iterator;
   using const_iterator = typename MyList::const_iterator;

   static const size_type MinBuckets = 8; // must be a positive power of 2

protected:
   Hash( const KeyCompare &parg ) // construct empty hash table
      : traitsObj( parg ),
        myList(),
        myVec(),
        mask( MinBuckets - 1 ),
        maxidx( MinBuckets )
   {     
      myVec.assignGrow( MinBuckets * 2, myList.end() );
   }

public:
   // Returns the number of elements in the unordered_set container.
   size_type size() const
   {
      return myList.size();         //list元素個數，剛好與傳入數值(insert)的數量一樣多
   }

   // Returns the number of buckets in the unordered_set container.
   size_type bucket_count() const
   {
      return maxidx;    //buckets的數量
   }

   // Returns the bucket number where the element with value keyVal is located.
   size_type bucket( const key_type& keyVal ) const     //傳入hash val值，算出這個key在哪個bucket(回傳bucketNumber)
   {
       //呼叫operator()，將key值傳入再做FNV_la hash function 得到hash value，再與mask做bitwise and，得出bucket(vector)位置
      return traitsObj( keyVal ) & mask;        //traitsObj為Traits型態的一個物件，Ttaits的形態由宣告Hash物件時決定
//      return traitsObj.operator()( keyVal ) & mask;
//      return traitsObj( keyVal ) % maxidx;        //當maxidx(bucket數)為2的n次方時，使用 & or % 答案相等
//      return traitsObj.operator()( keyVal ) % maxidx;
   }

   // Returns the number of elements in bucket n.
   size_type bucket_size( size_type n ) const       //回傳bucket n 的元素個數 (此bucket指向的一小串list的node有多少個=對應此hashKey的hashValue有多少)
   {
      if( n >= maxidx )
         return 0;

      // bucket[ n ] is empty
      if( myVec.myData.myFirst[ 2 * n ] == myList.end() )
         return 0;

      //--------------------------------------------------

      if (&(myVec.myData.myFirst[2 * n]) == &(myVec.myData.myFirst[2 * n + 1])) { //同一個key的頭尾bucket指向同一個list node
          return 1;
      }

      size_type counter = 0;
      iterator it = myVec.myData.myFirst[2 * n];            //此key值的bucket所指向的一小條list的頭
      for (; it != myVec.myData.myFirst[2 * n + 1]; it++) {
          counter++;
      }
      return ++counter;
   }

   // Inserts a new element in the unordered_set.
   // The element is inserted only if it is not equivalent to any other element
   // already in the container ( elements in an unordered_set have unique values ).
   // This effectively increases the container size by one.
   void insert( const value_type &val )
   {
      if( find( val ) == myList.end() )
      {
         if( myList.size() == maxidx )      //myList.size() = 最大的元素個數 (要insert元素需先擴大bucket)
         {
            if( maxidx == 8 || maxidx == 64 )
               maxidx *= 8;
            else
               maxidx *= 2;

            myVec.assignGrow( 2 * maxidx, myList.end() );       //擴大bucket
            mask = maxidx - 1;
            MyList oldList(myList);         //copy constructor 創建oldList時把myList中的內容複製進去 
            myList.clear();         //myList內容清掉
            size_type tmp = NULL;
            for (iterator it = oldList.begin(); it != oldList.end(); it++) {
                putIn(*it);
                //將舊list(傳入值*it(傳值呼叫))中的內容存進新的list(呼叫此function的物件)
            }
            
            oldList.clear();
         }
         putIn( val );
      }
   }

   // Removes from the unordered_set a single element.
   // This effectively reduces the container size by one.
   void erase( const key_type &keyVal )
   {
      iterator it = find( keyVal );         //此值的bucket

      if( it != myList.end() ) // found
      {
          size_type keyValue = bucket(keyVal);
          if (myVec.myData.myFirst[2 * keyValue] == myVec.myData.myFirst[2 * keyValue + 1]) {       //此小串list只有一個node
              //if (myVec.myData.myFirst[2 * keyValue] != myList.end()) {     ////此小串list只有一個node且在最尾端 (head node 要指向原本的倒數第二個)
              //    myList.erase(keyVal);
              //}
              myList.erase(it);
              myVec.myData.myFirst[2 * keyValue] = myList.end();
              myVec.myData.myFirst[2 * keyValue + 1] = myList.end();
          }
          else {        //此key值有複數node
              if (*(myVec.myData.myFirst[2 * keyValue]) == keyVal) {        //此 node 在小串 list 中的頭
                  myVec.myData.myFirst[2 * keyValue] = ++it;
                  it--;
                  myList.erase(it);
              }
              else if (*(myVec.myData.myFirst[2 * keyValue + 1]) == keyVal) {       //此 node 在小串 list 中的尾
                  myVec.myData.myFirst[2 * keyValue + 1] = --it;
                  it++;
                  myList.erase(it);
              }
              else {
                  myList.erase(it);
              }
          }
      }
   }

   // Searches the unordered_set for an element with keyVal as value and
   // returns an iterator to it if found, otherwise it returns myList.end()
   iterator find(const key_type& keyVal)      //再list中找到有著傳入val的node，再將指向此node的iterator回傳 (keyval為要找到的值)
   {
       //size_type bucketNo = bucket(keyVal);         //bucketNo = bucket number 放在 vector 中的 index
       //if (myVec.myData.myFirst[2 * bucketNo] == myList.end()) {        //沒有找到 (此位置沒有被放入過此key值得的東西)
       //    return myList.end();         
       //}
       ////vector中已有此bucket值
       //iterator it = myVec.myData.myFirst[2 * bucketNo + 1];                //it = 此 key 值在 list 中尾端的位址
       ////++it;
       //for (iterator it2 = myVec.myData.myFirst[2 * bucketNo]; it2 != it; ++it2) {          //X
       //    if (keyVal == *it2) {
       //        return it2;
       //    }
       //}
       //return myList.end();

       iterator itFirst = myList.begin();
       iterator itEnd = myList.end();
       for (; itFirst != itEnd; itFirst++) {
           if (*itFirst == keyVal)  return itFirst;
       }
       return myList.end();

   }

private:
   // put a new element in the unordered_set when myVec is large enough
   void putIn( const value_type &val )          //將中的內容存進新的vector
   {
       size_type keyValue = bucket(val);       //得Key值
       iterator it = myList.end();
       if (myVec.myData.myFirst[2 * keyValue] == it) {        //此bucket還未有元素
           myList.push_back(val);           //在list最後插入value
           myVec.myData.myFirst[2 * keyValue] = --it;         //此key值的頭尾皆指向新插入的val的位置的iterator
           myVec.myData.myFirst[2 * keyValue + 1] = myVec.myData.myFirst[2 * keyValue];
       }
       else {           //此key中已有元素 (插進此小串list的頭的位置，keyValue*2指向此處)
           iterator it = myVec.myData.myFirst[2 * keyValue];        //插入元素前，小串list的第一個的iterator
           myList.insert(it, val);
           myVec.myData.myFirst[2 * keyValue] = --it;
       }
       //cout << val << " " << keyValue << "\t";
       /*for (value_type i : myList) {
           cout << i << " ";
       }
       cout << endl << endl;*/
   }

protected:
   Traits traitsObj;          // traits to customize behavior
   MyList myList;             // list of elements, must initialize before myVec (myList 為 STL 的 list 所宣告的物件，用來將value串起來)
   HashVec< iterator > myVec; // "vector" of list iterators for buckets:    (自定義的vector，裡面裝位置，指向list中的value)
                              // each bucket is 2 iterators denoting
                              // the closed range of elements in the bucket,
                              // or both iterators set to end() if the bucket is empty.
   size_type mask;            // the key mask, equals maxidx - 1
   size_type maxidx;          // current maximum key value, must be a power of 2
};

#endif // XHASH
