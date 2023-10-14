// xtree internal header

#ifndef XTREE
#define XTREE

template< typename Ty >
struct TreeNode
{
	using NodePtr = TreeNode*;
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
	using NodePtr = TreeNode< Ty >*;

	using value_type = Ty;
	using size_type = size_t;

	TreeVal()
		: myHead(new TreeNode< value_type >),
		mySize(0)
	{
		myHead->left = myHead;
		myHead->parent = myHead;
		myHead->right = myHead;
		myHead->color = 1;
		myHead->isNil = 1;
	}

	~TreeVal()
	{
		clear(myHead->parent);
		delete myHead;
	}

	// Removes all elements from the set object (which are destroyed)
	void clear(TreeNode< value_type >* node)
	{
		if (!node->isNil) // node is not an external node
		{
			clear(node->left);
			clear(node->right);
			delete node;
		}
	}

	// rebalance for insertion; n points to the inserted node
	void reBalance(TreeNode< value_type >* n)				//n have parent, but grandparent may be myhead  //X
	{  // n->parent (g) cannot be the root
		TreeNode< value_type >* p = n->parent;
		TreeNode< value_type >* g = p->parent;
		TreeNode< value_type >* u; // uncle of n

		if (g == myHead)  return;

		if (p == g->left)
			u = g->right;
		else if (p == g->right)
			u = g->left;
		else
			return;

		//p is black, do nothing (case 2.)
		if (p->color == 1) {
			return;
		}

		if (u->color == 0) // ( red )
		{
			p->color = 1;
			u->color = 1;

			if (g->parent != myHead) // g is not the root
			{
				g->color = 0;
				if (g->parent->color == 0)
					reBalance(g);
			}
		}
		else // u->color == 1 ( black )
		{
			if (n == p->left && p == g->left)
			{
				p->color = 1;
				g->color = 0;
				rightRotation(p);
			}
			if (n == p->left && p == g->right)
			{
				n->color = 1;
				g->color = 0;
				rightRotation(n);
				leftRotation(n);
			}
			if (n == p->right && p == g->left)
			{
				n->color = 1;
				g->color = 0;
				leftRotation(n);
				rightRotation(n);
			}
			if (n == p->right && p == g->right)
			{
				p->color = 1;
				g->color = 0;
				leftRotation(p);
			}
		}
	}

	// rotate right at g, where p = g->left
	void rightRotation(TreeNode< value_type >* p)
	{
		TreeNode< value_type >* g = p->parent;
		TreeNode<value_type>* PR = p->right; // right child of p

		/*if (PR == myHead)
			cout << "PR" << endl;*/

		if (g->parent!=myHead)
		{
			//let g's parent point to p
			if (g->parent->left == g) {
				g->parent->left = p;
			}
			else {
				g->parent->right = p;
			}
		}
		else
		{
			myHead->parent = p;
		}

		p->parent = g->parent;
		p->right = g;             //p up, g down
		g->parent = p;

		if (PR != myHead) {			//if PR is myhead, PR's parent is root
			PR->parent = g;
			g->left = PR;
		}
		else {
			g->left = myHead;
		}
	}

	// rotate left at g, where p = g->right
	void leftRotation(TreeNode< value_type >* p)
	{
		TreeNode< value_type >* g = p->parent;
		TreeNode<value_type>* PL = p->left; // left child of p

		/*if (PL == myHead)
			cout << "PL" << endl;*/

		if(g->parent != myHead) {			//g is not root
			//let g's parent point to p
			if (g->parent->left == g) {
				g->parent->left = p;
			}
			else
			{
				g->parent->right = p;			
			}
		}
		else			//g is root
		{
			myHead->parent = p;			//myhead point to new root p
		}

		p->parent = g->parent;
		p->left = g;
		g->parent = p;

		if (PL != myHead)
		{
			PL->parent = g;
			g->right = PL;
		}
		else
		{
			g->right = myHead;
		}
	}

	// erase erasedNode provided that the degree of erasedNode is at most one
	// erasedNode points to the node to be deleted
	// erasedNode == M in "Ch 3 Sec 9.pptx"
	void eraseDegreeOne(TreeNode< value_type >* erasedNode)
	{
		TreeNode< value_type >* child;
		if (erasedNode->left != myHead)                      //erasdNode only has one child or no child
			child = erasedNode->left;
		if (erasedNode->right != myHead)					
			child = erasedNode->right;
		else
			child = myHead;				//eraseNode don't have internal child, but have leaf child

		if (erasedNode == myHead->parent) // erasedNode points to the root; Cases 2 & 3 in "Ch 3 Sec 9.pptx"
		{
			// set child to be the new root

			child->parent = myHead;
			child->left = myHead;
			child->right = myHead;
			child->color = 1;
			myHead->parent = child;
			myHead->left = child;
			myHead->right = child;

		}
		else			//erasedNode is not root 
		{
			if (erasedNode->color == 1) {     //erasedNode is black  
				
				if (child != myHead) {			//erasedNode have internal child 
					child->parent = erasedNode->parent;
				}
				
				if (erasedNode->parent->left == erasedNode)				//erasedNode not the root, so the parent is not myhead 
					erasedNode->parent->left = child;
				else 
					erasedNode->parent->right = child;

				if (child->color == 0) // Case 2 in "Ch 3 Sec 9.pptx"
					child->color = 1;  
				else 
					fixUp(child, erasedNode->parent);			// Case 4 in "Ch 3 Sec 9.pptx"
			}
			else {        //erasedNode is red (case 1.)
				if (child != myHead) {
					child->parent = erasedNode->parent;
				}
				if (erasedNode->parent->left == erasedNode) {
					erasedNode->parent->left = child;
				}
				else {
					erasedNode->parent->right = child;
				}
			}
		}
		if (erasedNode->myval == myHead->left->myval) {
			myHead->left = erasedNode->parent;
		}
		if (erasedNode->myval == myHead->right->myval) {
			myHead->right = erasedNode->parent;
		}
		
		delete erasedNode;
		mySize--;
	}

	// rebalance for deletion; Case 4 in "Ch 3 Sec 9.pptx"
	void fixUp(TreeNode< value_type >* N, TreeNode< value_type >* P)  //N is leaf (myhead), P is parent of N
	{
		TreeNode<value_type>* S; // sibling of N

		if (P->left == N)
			S = P->right;
		else 
			S = P->left;

		TreeNode<value_type>* SL = S->left; // left child of S
		TreeNode<value_type>* SR = S->right; // right child of S

		if (S->color == 0) {             //N is red (case 4.1) (S must have two internal child)
			if (P->left == N) {			 //4.1.1 n is left child of p
				leftRotation(S);         //S up, P down
				P->color = 0;
				S->color = 1;
				fixUp(N, P);

				/*SL->color = 0;
				p->color = 1;*/
			}
			else if(P->right == N){						//n is right child of p
				rightRotation(S);        //S up, P down
				P->color = 0;
				S->color = 1;
				fixUp(N, P);
				
				/*SR->color = 0;
				p->color = 1;*/
			}
		}
		else {           //s is black
			if (SL->color == 1 && SR->color == 0 && P->right == N) {            //s is black, SR is red, SL is black. case 4.3.2

				leftRotation(SR);
				SR->color = 1;
				S->color = 0;
				fixUp(N, P);			//fit case 4.2.2
				
			}
			else if(SL->color == 0 && SR->color == 1 && P->left == N){           //s is black, SL is red, SR is black, N is left child of P. 4.3.1

				rightRotation(SL);
				SL->color = 1;
				S->color = 0;
				fixUp(N, P);			//fit case 4.2.1

			}
			else if (SL->color == 1 && SR->color == 1 && P->color == 0) { //case 4.4
				P->color = 1;
				S->color = 0;
			}
			else if (SL->color == 1 && SR->color == 1 && P->color == 1) {   //all black, case 4.5
				S->color = 0;
				if(P != myHead->parent)
					fixUp(P, P->parent);
			}
			else if (SR->color == 0 && P->left == N) {				//case 4.2.1
				leftRotation(S);
				S->color = P->color;
				P->color = 1;
				SR->color = 1;
			}
			else if(SL->color == 0 && P->right == N){				//case 4.2.2
				rightRotation(S);
				S->color = P->color;
				P->color = 1;
				SL->color = 1;
			}			
		}
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
	using key_type = typename Traits::key_type;
	using key_compare = typename Traits::key_compare;

	using size_type = size_t;

	Tree(const key_compare& parg)
		: keyCompare(parg),
		scaryVal()
	{
	}

	~Tree()
	{
	}

	// Extends the container by inserting a new element,
	// effectively increasing the container size by one.
	void insert(const value_type& val)             
	{
		if (scaryVal.mySize == 0) // empty tree (case 1.)
		{
			TreeNode< value_type >* root = new TreeNode< value_type >;
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
		else                        // not empty tree 
		{                           // scaryVal.myHead->parent points to the root
			TreeNode< value_type >* tryNode = scaryVal.myHead->parent; // tryNode points to the root
			TreeNode< value_type >* result = nullptr;
			while (tryNode != scaryVal.myHead) // tryNode is not the leaf
			{
				result = tryNode;			//at beginning, result points to the root
				//              keyCompare.operator()( val, tryNode->myval )
				if (keyCompare(val, tryNode->myval)) // if( val < tryNode->myval )
					tryNode = tryNode->left;
				else if (keyCompare(tryNode->myval, val)) // if( val > tryNode->myval || val == tryNode->myval)
					tryNode = tryNode->right;
				else
					return;
				
			}

			//inserts a new node
			TreeNode< value_type >* newNode = new TreeNode< value_type >;
			newNode->parent = result;
			newNode->color = 0;
			newNode->isNil = 0;
			newNode->myval = val;
			newNode->left = scaryVal.myHead;
			newNode->right = scaryVal.myHead;

			if (keyCompare(val, result->myval)) // val < newNode->parent->myval
				result->left = newNode;
			else if (keyCompare(result->myval, val))										 // val >= newNode->parent->myval
				result->right = newNode;
			else return;

			//change myhead's left and right point to maxnum and minnum
			if (keyCompare(val, scaryVal.myHead->left->myval)) {
				scaryVal.myHead->left = newNode;
			}
			if (keyCompare(scaryVal.myHead->right->myval, val)) {
				scaryVal.myHead->right = newNode;
			}

			scaryVal.mySize++;
			scaryVal.reBalance(newNode);
		}
	}

	// Removes from the set container a single element whose value is val
	// This effectively reduces the container size by one, which are destroyed.
	// Returns the number of elements erased.
	size_type erase(const key_type& val)
	{
		TreeNode< key_type >* erasedNode = scaryVal.myHead->parent; // erasedNode points to the root

		while (erasedNode != scaryVal.myHead && val != erasedNode->myval)		//search the node to be erased
		{
			//keyCompare.operator()( val, erasedNode->myval )
			if (keyCompare(val, erasedNode->myval)) // if( val < erasedNode->myval )
				erasedNode = erasedNode->left;
			else
				erasedNode = erasedNode->right;
		}

		if (erasedNode == scaryVal.myHead) // not found
			return 0;
		else // found
		{  // deletes the node pointed by erasedNode

			TreeNode<value_type>* tryNode = new TreeNode<value_type>;         //tryNode point to the biggest val in erasedNode's left subtree
			tryNode = erasedNode;

			if (erasedNode->right != scaryVal.myHead) {
				tryNode = erasedNode->right;
				while (tryNode->left != scaryVal.myHead)         //find the smallest val in erasedNode's right subtree
				{
					tryNode = tryNode->left;
				}
			}
			else if (erasedNode->left != scaryVal.myHead) {				
				tryNode = erasedNode->left;
				while (tryNode->right != scaryVal.myHead)         //find the biggest val in erasedNode's left subtree
				{
					tryNode = tryNode->right;
				}
			}
			else {		 //erasedNode is the root and it is the only node in the tree, or erasedNode is a leaf
				scaryVal.eraseDegreeOne(erasedNode);              //erasedNode is a internal node
				return 1;
			}

			erasedNode->myval = tryNode->myval;               //replace erasedNode's val with tryNode's val

			scaryVal.eraseDegreeOne(tryNode);              //erase tryNode
			return 1;
		}
	}

private:
	key_compare keyCompare;
	ScaryVal scaryVal;
};

#endif // XTREE
