// algorithm standard header

#ifndef ALGORITHM
#define ALGORITHM

namespace MyNamespace
{
   template< typename RanIt, typename Ty, typename Pr >
   inline void pushHeapByIndex( RanIt first, ptrdiff_t hole, ptrdiff_t top, Ty &val, Pr pred )      //做 up-heap operation
   {
       first[hole] = val;
       ptrdiff_t tmp = hole;        //最後一個元素的index (一開始insert的位置)
       
       while (tmp > 0) {        //如果tmp還未走到root，執行迴圈
           ptrdiff_t parentIndex = (tmp - 1) / 2;
           if (pred(first[parentIndex], first[tmp])) {       //在max heap (big=>small) 中，前項(parent node的值)小於後項(新插入的值)的話執行兩值交換
               first[tmp] = first[parentIndex];
               first[parentIndex] = val;
               tmp = parentIndex;
           }
           else  break;          //前項不大於後項，表插入值到達正確位置，跳出迴圈
       }
   }

   // push *(last - 1) onto heap at [first, last - 1), using pred
   template< typename RanIt, typename Pr >
   inline void push_heap( RanIt first, RanIt last, Pr pred )    //first = vector的第一個位址, last = vector 的最後一個位址的下一個
   {
      ptrdiff_t count = last - first;       //元素總個數
      if( count >= 2 )      //此container有元素
      {
         --last;        //由最後一個元素的下一個變為指向最後一個元素
         typename RanIt::value_type val = *last;        //宣告有著vector中值的型態的 val = vector最後一個元素值
         --count;           //最後一個元素的index
         pushHeapByIndex( first, count, 0, val, pred );         
      }
   }

   template< typename RanIt, typename Ty, typename Pr >
   inline void popHeapHoleByIndex( RanIt first, ptrdiff_t hole, ptrdiff_t bottom, Ty &val, Pr pred )    //做 down-heap
   {       
       Ty tmpVal = val;
       Ty childNum;       //暫存子節點 num
       ptrdiff_t tmp = hole;        //要向下做 down-heap 的index位置       
       ptrdiff_t childIndex;       //暫存子節點 index
       ptrdiff_t LChildIndex, RChildIndex;

       while (tmp < bottom) {

           childNum = NULL;
           childIndex = NULL;

           LChildIndex = tmp * 2 + 1;
           RChildIndex = tmp * 2 + 2;

           if (RChildIndex < bottom) {      //two children, both child excit
               if (pred(first[LChildIndex], first[RChildIndex]) || first[LChildIndex] == first[RChildIndex]) {             //前項小於後項 (左右子節點比較) 左小於等於右
                   childNum = first[RChildIndex];
                   childIndex = RChildIndex;
               }
               else {                       //左節點大於右節點
                   childNum = first[LChildIndex];
                   childIndex = LChildIndex;
               }
           }
           else if (LChildIndex < bottom) {         //one child
               childNum = first[LChildIndex];
               childIndex = LChildIndex;
           }
           else  break;         //no child (leaf, stop)

           if (pred(val, childNum)) {
               first[tmp] = childNum;
               first[childIndex] = val;
               tmp = childIndex;
           }
           else  break;     //子節點 val沒有大於 parent node 節點的 val
       }
       first[tmp] = val;
   }

   // pop *first to *(last - 1) and reheap, using pred
   template< typename RanIt, typename Pr >
   inline void pop_heap( RanIt first, RanIt last, Pr pred )
   {
      if( last - first >= 2 )           //現有元素至少兩個
      {
         --last;        //最後一個元素的index
         typename RanIt::value_type val = *last;            //最後一個元素的值
         popHeapHoleByIndex( first, 0, last - first, val, pred );       //last - first = 現有原素個數
      }
   }

   // make [first, last) into a heap, using pred
   template< typename RanIt, typename Pr >
   inline void make_heap( RanIt first, RanIt last, Pr pred )
   {
      ptrdiff_t bottom = last - first;
      ptrdiff_t hole = bottom / 2;
      while( hole > 0 )
      {
         // reheap top half, bottom to top
         --hole;
         typename RanIt::value_type val = *( first + hole );
         popHeapHoleByIndex( first, hole, bottom, val, pred );
      }
   }
}

#endif // ALGORITHM