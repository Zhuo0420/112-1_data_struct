// algorithm standard header

#ifndef ALGORITHM
#define ALGORITHM

namespace MyNamespace
{
   template< typename RanIt, typename Ty, typename Pr >
   inline void pushHeapByIndex( RanIt first, ptrdiff_t hole, ptrdiff_t top, Ty &val, Pr pred )      //�� up-heap operation
   {
       first[hole] = val;
       ptrdiff_t tmp = hole;        //�̫�@�Ӥ�����index (�@�}�linsert����m)
       
       while (tmp > 0) {        //�p�Gtmp�٥�����root�A����j��
           ptrdiff_t parentIndex = (tmp - 1) / 2;
           if (pred(first[parentIndex], first[tmp])) {       //�bmax heap (big=>small) ���A�e��(parent node����)�p��ᶵ(�s���J����)���ܰ����ȥ洫
               first[tmp] = first[parentIndex];
               first[parentIndex] = val;
               tmp = parentIndex;
           }
           else  break;          //�e�����j��ᶵ�A���J�Ȩ�F���T��m�A���X�j��
       }
   }

   // push *(last - 1) onto heap at [first, last - 1), using pred
   template< typename RanIt, typename Pr >
   inline void push_heap( RanIt first, RanIt last, Pr pred )    //first = vector���Ĥ@�Ӧ�}, last = vector ���̫�@�Ӧ�}���U�@��
   {
      ptrdiff_t count = last - first;       //�����`�Ӽ�
      if( count >= 2 )      //��container������
      {
         --last;        //�ѳ̫�@�Ӥ������U�@���ܬ����V�̫�@�Ӥ���
         typename RanIt::value_type val = *last;        //�ŧi����vector���Ȫ����A�� val = vector�̫�@�Ӥ�����
         --count;           //�̫�@�Ӥ�����index
         pushHeapByIndex( first, count, 0, val, pred );         
      }
   }

   template< typename RanIt, typename Ty, typename Pr >
   inline void popHeapHoleByIndex( RanIt first, ptrdiff_t hole, ptrdiff_t bottom, Ty &val, Pr pred )    //�� down-heap
   {       
       Ty tmpVal = val;
       Ty childNum;       //�Ȧs�l�`�I num
       ptrdiff_t tmp = hole;        //�n�V�U�� down-heap ��index��m       
       ptrdiff_t childIndex;       //�Ȧs�l�`�I index
       ptrdiff_t LChildIndex, RChildIndex;

       while (tmp < bottom) {

           childNum = NULL;
           childIndex = NULL;

           LChildIndex = tmp * 2 + 1;
           RChildIndex = tmp * 2 + 2;

           if (RChildIndex < bottom) {      //two children, both child excit
               if (pred(first[LChildIndex], first[RChildIndex]) || first[LChildIndex] == first[RChildIndex]) {             //�e���p��ᶵ (���k�l�`�I���) ���p�󵥩�k
                   childNum = first[RChildIndex];
                   childIndex = RChildIndex;
               }
               else {                       //���`�I�j��k�`�I
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
           else  break;     //�l�`�I val�S���j�� parent node �`�I�� val
       }
       first[tmp] = val;
   }

   // pop *first to *(last - 1) and reheap, using pred
   template< typename RanIt, typename Pr >
   inline void pop_heap( RanIt first, RanIt last, Pr pred )
   {
      if( last - first >= 2 )           //�{�������ܤ֨��
      {
         --last;        //�̫�@�Ӥ�����index
         typename RanIt::value_type val = *last;            //�̫�@�Ӥ�������
         popHeapHoleByIndex( first, 0, last - first, val, pred );       //last - first = �{������Ӽ�
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