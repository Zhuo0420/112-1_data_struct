// xhash internal header

#ifndef XHASH
#define XHASH

#include <list>
using std::list;

#include "type_traits.h"

// CLASS TEMPLATE VectorVal
template< typename ValueType >
class VectorVal             //hash table, array ����Ӥ������@��bucket�A�]��list�O�h��value�զ����h�ӵu�� list �Ҧꦨ���@�Ӫ��� list�A�@��pointer���V���list���Y�A�t�@�ӫ��V���A�ΨӰϤ��h�qlist
{
public:
    using value_type = ValueType;
    using pointer = value_type*;

    VectorVal()
        : myFirst(),      //vector���Ĥ@�Ӥ���
        myLast(),       //vector���Ψ쪺�̫�@�Ӥ������U�@��
        myEnd()         //vector���̫�@�Ӥ������U�@��
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
    using size_type = size_t;

    HashVec()
        : myData()    //��class�@�Q�غc�N�~��myData (VecctorVal ������)
    {
    }

    // set the elements stored here to cells copies of val �N���B�s�x�������]�m���x�s�� Val ���ƥ�
    void assignGrow(const size_type cells, const value_type val)       //�N�I�s��function�������X�j�� cells �ƶq�� bucket
    {
        size_t oldsize = static_cast<size_type>(myData.myLast - myData.myFirst);
        value_type* newVec = new value_type[cells];
        if (oldsize != 0) {
            delete[] myData.myFirst;
        }
        myData.myFirst = newVec;
        myData.myEnd = newVec + cells;
        myData.myLast = newVec + cells;
        ::std::uninitialized_fill(newVec, newVec + cells, val);
    }

    ~HashVec()
    {
        if (myData.myFirst != nullptr)
            delete[] myData.myFirst;
    }

    VectorVal< value_type > myData;      //MyData�� VecctorVal ������A��vector��������
};

// CLASS TEMPLATE Hash
template< typename Traits >
class Hash // hash table -- list with vector of iterators for quick access (iterators = �O�ЪŶ���}�A�ϥΤ�k�p����)
{
protected:
    using MyList = list< typename Traits::value_type >;      //MyList �� list< typename Traits::value_type >���O�W 
    using KeyCompare = typename Traits::key_compare;             //typename ������r�A�Ψӧi�D�sĶ�����U�Ӫ��W�٬O�@�������W��

public:
    using key_type = typename Traits::key_type;
    using value_type = typename MyList::value_type;
    using size_type = typename MyList::size_type;

    using iterator = typename MyList::const_iterator;
    using const_iterator = typename MyList::const_iterator;

    static const size_type MinBuckets = 8; // must be a positive power of 2

protected:
    Hash(const KeyCompare& parg) // construct empty hash table
        : traitsObj(parg),
        myList(),
        myVec(),
        mask(MinBuckets - 1),
        maxidx(MinBuckets)
    {
        myVec.assignGrow(MinBuckets * 2, myList.end());
    }

public:
    // Returns the number of elements in the unordered_set container.
    size_type size() const
    {
        return myList.size();         //list�����ӼơA��n�P�ǤJ�ƭ�(insert)���ƶq�@�˦h
    }

    // Returns the number of buckets in the unordered_set container.
    size_type bucket_count() const
    {
        return maxidx;    //buckets���ƶq
    }

    // Returns the bucket number where the element with value keyVal is located.
    size_type bucket(const key_type& keyVal) const     //�ǤJhash val�ȡA��X�o��key�b����bucket(�^��bucketNumber)
    {
        //�I�soperator()�A�Nkey�ȶǤJ�A��FNV_la hash function �o��hash value�A�A�Pmask��bitwise and�A�o�Xbucket��m
        return traitsObj(keyVal) & mask;        //traitsObj��Traits���A���@�Ӫ���ATtaits���κA�ѫŧiHash����ɨM�w
        //      return traitsObj.operator()( keyVal ) & mask;
        //      return traitsObj( keyVal ) % maxidx;        //��maxidx(bucket��)��2��n����ɡA�ϥ� & or % ���׬۵�
        //      return traitsObj.operator()( keyVal ) % maxidx;
    }

    // Returns the number of elements in bucket n.
    size_type bucket_size(size_type n) const       //�^��bucket n �������Ӽ� (��bucket���V���@�p��list��node���h�֭�=������hashKey��hashValue���h��)
    {
        if (n >= maxidx)
            return 0;

        // bucket[ n ] is empty
        if (myVec.myData.myFirst[2 * n] == myList.end())
            return 0;

        //--------------------------------------------------

        if (&(myVec.myData.myFirst[2 * n]) == &(myVec.myData.myFirst[2 * n + 1])) { //�P�@��key���Y��bucket���V�P�@��list node
            return 1;
        }
        ;
        size_type counter = 1;
        iterator it = myVec.myData.myFirst[2 * n + 1];            //��key�Ȫ�bucket�ҫ��V���@�p��list���Y
        for (; it != myVec.myData.myFirst[2 * n]; ++it) {
            counter++;
        }
        return counter;
    }

    // Inserts a new element in the unordered_set.
    // The element is inserted only if it is not equivalent to any other element
    // already in the container ( elements in an unordered_set have unique values ).
    // This effectively increases the container size by one.
    void insert(const value_type& val)
    {
        if (find(val) == myList.end())
        {
            if (myList.size() == maxidx)      //myList.size() = �{�b�������Ӽ� (�ninsert�����ݥ��X�jbucket)
            {
                if (maxidx == 8 || maxidx == 64)
                    maxidx *= 8;
                else
                    maxidx *= 2;

                myVec.assignGrow(2 * maxidx, myList.end());       //�X�jbucket
                MyList oldList(myList);         //copy constructor �Ы�oldList�ɧ�myList�������e�ƻs�i�h 
                myList.clear();         //myList���e�M��
                for (iterator it = oldList.begin(); it != oldList.end(); ++it) {
                    putIn(*it);         //�N��list(�ǤJ��*it(�ǭȩI�s))�������e�s�i�s��list(�I�s��function������)
                }
                putIn(val);
                oldList.clear();
            }
            putIn(val);
        }
    }

    // Removes from the unordered_set a single element.
    // This effectively reduces the container size by one.
    void erase(const key_type& keyVal)
    {
        iterator it = find(keyVal);

        if (it != myList.end()) // found
        {
            size_type keyValue = bucket(keyVal);
            if (myVec.myData.myFirst[2 * keyValue] == myVec.myData.myFirst[2 * keyValue + 1]) {       //���p��list�u���@��node
                if (myVec.myData.myFirst[2 * keyValue] != myList.end()) {     ////���p��list�u���@��node�B�b�̧��� (head node �n���V�쥻���˼ƲĤG��)
                    iterator it2 = ++myList.end();
                }
                myVec.myData.myFirst[2 * keyValue] = myList.end();
                myVec.myData.myFirst[2 * keyValue + 1] = myList.end();
            }
            else if (*(myVec.myData.myFirst[2 * keyValue + 1]) == keyVal) {        //��key�Ȧ��Ƽ�node
                myVec.myData.myFirst[2 * keyValue + 1] = ++it;
            }
            else if (*(myVec.myData.myFirst[2 * keyValue]) == keyVal) {
                myVec.myData.myFirst[2 * keyValue] = --it;
            }

            myList.erase(it);
        }
    }

    // Searches the unordered_set for an element with keyVal as value and
    // returns an iterator to it if found, otherwise it returns myList.end()
    iterator find(const key_type& keyVal)      //�Alist����즳�۶ǤJkey��node�A�A�N���V��node��iterator�^��
    {
        size_type bucketNo = bucket(keyVal);         //bucket number
        if (myVec.myData.myFirst[2 * bucketNo] == myList.end()) {
            return myList.end();
        }
        iterator it = myVec.myData.myFirst[2 * bucketNo + 1];
        ++it;
        for (iterator it2 = myVec.myData.myFirst[2 * bucketNo]; it2 != it; ++it2) {
            if (keyVal == *it2) {
                return it2;
            }
        }
        return myList.end();
    }

private:
    // put a new element in the unordered_set when myVec is large enough
    void putIn(const value_type& val)          //�N�������e�s�i�s��vector
    {
        size_type keyValue = bucket(val);       //�oKey��
        if (myVec.myData.myFirst[2 * keyValue] == myList.end()) {        //��bucket�٥�������
            myList.push_back(val);           //�blist�̫ᴡ�Jvalue
            myVec.myData.myFirst[2 * keyValue] = find(keyValue);         //��key�Ȫ��Y���ҫ��V�s���J��val����m��iterator
            myVec.myData.myFirst[2 * keyValue + 1] = myVec.myData.myFirst[2 * keyValue];
        }
        else {           //��key���w������ (���i���p��list���Y����m�AkeyValue*2+1���V���B)
            iterator it = myVec.myData.myFirst[2 * keyValue + 1];        //���J�����e�A�p��list���Ĥ@�Ӫ�iterator
            myList.insert(it, val);
            myVec.myData.myFirst[2 * keyValue + 1] = --it;
        }
    }

protected:
    Traits traitsObj;          // traits to customize behavior
    MyList myList;             // list of elements, must initialize before myVec (myList �� STL �� list �ҫŧi������A�ΨӱNvalue��_��)
    HashVec< iterator > myVec; // "vector" of list iterators for buckets:    (�۩w�q��vector�A�̭��˦�m�A���Vlist����value)
    // each bucket is 2 iterators denoting
    // the closed range of elements in the bucket,
    // or both iterators set to end() if the bucket is empty.
    size_type mask;            // the key mask, equals maxidx - 1
    size_type maxidx;          // current maximum key value, must be a power of 2
};

#endif // XHASH