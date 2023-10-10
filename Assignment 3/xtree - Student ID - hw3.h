// xtree internal header

#ifndef XTREE
#define XTREE

template< typename Ty >
struct TreeNode
{
   using NodePtr = TreeNode *;
   using value_type = Ty;

   NodePtr    left;   // left subtree, or smallest element if head
   NodePtr    parent; // parent, or root of tree if head
   NodePtr    right;  // right subtree, or largest element if head
   char       color;  // 0: Red, 1: Black; Black if head
   char       isNil;  // 1 only if head (also nil) node
   value_type myval;  // the stored value, unused if head
};

// CLASS TEMPLATE TreeVal
template< typename Ty >
class TreeVal
{
public:
   using NodePtr = TreeNode< Ty > *;

   using value_type = Ty;
   using size_type  = size_t;

   TreeVal()
      : myHead( new TreeNode< value_type > ),
        mySize( 0 )
   {
      myHead->left = myHead;
      myHead->parent = myHead;
      myHead->right = myHead;
      myHead->color = 1;
      myHead->isNil = 1;
   }

   ~TreeVal()
   {
      clear( myHead->parent );
      delete myHead;
   }

   // Removes all elements from the set object (which are destroyed)
   void clear( TreeNode< value_type > *node )
   {
      if( !node->isNil ) // node is not an external node
      {
         clear( node->left );
         clear( node->right );
         delete node;
      }
   }

   // rebalance for insertion; n points to the inserted node
   void reBalance( TreeNode< value_type > *n )
   {  // n->parent cannot be the root
      TreeNode< value_type > *p = n->parent;
      TreeNode< value_type > *g = p->parent;
      TreeNode< value_type > *u; // uncle of n

      if( p == g->left )
         u = g->right;
      else
         u = g->left;

      if( u->color == 0 ) // ( red )
      {
         p->color = 1;
         u->color = 1;

         if( g->parent != myHead ) // g is not the root
         {
            g->color = 0;
            if( ( g->parent )->color == 0 )
               reBalance( g );
         }
      }
      else // u->color == 1 ( black )
      {
         if( n == p->left && p == g->left )
         {
            p->color = 1;
            g->color = 0;
            rightRotation( p );
         }
         if( n == p->left && p == g->right )
         {
            n->color = 1;
            g->color = 0;
            rightRotation( n );
            leftRotation( n );
         }
         if( n == p->right && p == g->left )
         {
            n->color = 1;
            g->color = 0;
            leftRotation( n );
            rightRotation( n );
         }
         if( n == p->right && p == g->right )
         {
            p->color = 1;
            g->color = 0;
            leftRotation( p );
         }
      }
   }

   // rotate right at g, where p = g->left
   void rightRotation( TreeNode< value_type > *p )
   {
      TreeNode< value_type > *g = p->parent;



   }

   // rotate left at g, where p = g->right
   void leftRotation( TreeNode< value_type > *p )
   {
      TreeNode< value_type > *g = p->parent;



   }

   // erase erasedNode provided that the degree of erasedNode is at most one
   // erasedNode points to the node to be deleted
   // erasedNode == M in "Ch 3 Sec 9.pptx"
   void eraseDegreeOne( TreeNode< value_type > *erasedNode )
   {
      TreeNode< value_type > *child;
      if( erasedNode->left != myHead )
         child = erasedNode->left;
      else if( erasedNode->right != myHead )
         child = erasedNode->right;
      else
         child = myHead;

      if( erasedNode == myHead->parent ) // erasedNode points to the root; Cases 2 & 3 in "Ch 3 Sec 9.pptx"
      {
         // set child to be the new root



      }
      else
      {




         if( erasedNode->color == 1 ) 
            if( child->color == 0 ) // Case 2 in "Ch 3 Sec 9.pptx"
               child->color = 1;
            else                   // Case 4 in "Ch 3 Sec 9.pptx"
               fixUp( child, erasedNode->parent );
      }

      delete erasedNode;
      mySize--;
   }

   // rebalance for deletion; Case 4 in "Ch 3 Sec 9.pptx"
   void fixUp( TreeNode< value_type > *N, TreeNode< value_type > *P )
   {



   }

   NodePtr myHead;   // pointer to head node
   size_type mySize; // number of elements
};

// CLASS TEMPLATE Tree
template< typename Traits >
class Tree // ordered red-black tree for map/multimap/set/multiset
{
public:
   using value_type = typename Traits::value_type;

protected:
   using ScaryVal = TreeVal< value_type >;

public:
   using key_type      = typename Traits::key_type;
   using key_compare   = typename Traits::key_compare;

   using size_type       = size_t;

   Tree( const key_compare &parg )
      : keyCompare( parg ),
        scaryVal()
   {
   }

   ~Tree()
   {
   }

   // Extends the container by inserting a new element,
   // effectively increasing the container size by one.
   void insert( const value_type &val )
   {
      if( scaryVal.mySize == 0 )
      {
         TreeNode< value_type > *root = new TreeNode< value_type >;
         root->myval = val;
         root->left = scaryVal.myHead;
         root->right = scaryVal.myHead;
         root->isNil = 0;
         root->parent = scaryVal.myHead;
         root->color = 1;
         scaryVal.myHead->left = root;
         scaryVal.myHead->parent = root;
         scaryVal.myHead->right = root;
         scaryVal.mySize = 1;
      }
      else
      {                           // scaryVal.myHead->parent points to the root
         TreeNode< value_type > *tryNode = scaryVal.myHead->parent; // tryNode points to the root
         TreeNode< value_type > *result = nullptr;
         while( tryNode != scaryVal.myHead )
         {
            result = tryNode;
//              keyCompare.operator()( val, tryNode->myval )
            if( keyCompare( val, tryNode->myval ) ) // if( val < tryNode->myval )
               tryNode = tryNode->left;
            else if( keyCompare( tryNode->myval, val ) ) // if( val > tryNode->myval )
               tryNode = tryNode->right;
            else
               return;
         }



      }
   }

   // Removes from the set container a single element whose value is val
   // This effectively reduces the container size by one, which are destroyed.
   // Returns the number of elements erased.
   size_type erase( const key_type &val )
   {
      TreeNode< key_type > *erasedNode = scaryVal.myHead->parent; // erasedNode points to the root
      while( erasedNode != scaryVal.myHead && val != erasedNode->myval )
      {
//           keyCompare.operator()( val, erasedNode->myval )
         if( keyCompare( val, erasedNode->myval ) ) // if( val < erasedNode->myval )
            erasedNode = erasedNode->left;
         else
            erasedNode = erasedNode->right;
      }

      if( erasedNode == scaryVal.myHead ) // not found
         return 0;
      else // found
      {  // deletes the node pointed by erasedNode



         return 1;
      }
   }

private:
   key_compare keyCompare;
   ScaryVal scaryVal;
};

#endif // XTREE