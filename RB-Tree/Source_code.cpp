#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <list>
using namespace std;

#define uint unsigned int

#define SUCCESS 1
#define FAILURE 0
#define TRUE 1
#define FALSE 0
#define NULL_NODE 0xffffff

#define LOG_ERROR(x) printf("%s", x);
#define LOG_INFO(x) printf("%s", x);

#define DEBUG 0
#define READ_FROM_FILE 0

class RBTree {

    /* Data Strutures */
    // Enumerates the two colors of the RB Tree
    typedef enum rb_color {
        BLACK = 0,
        RED = 1
    } node_color;

    // Structure of each node of the Tree
    typedef struct rb_node {
        rb_node* parent, *left, *right;
        node_color color;
        uint val;
    } node;
        
    // Structure used during printing of the Tree
    typedef struct rb_node_rep {
        uint value;
        node_color color;
    } rb_rep;

    // Tree Sentinel
    node sentinel;
    // Tree Root
    node *root;
    uint maxIndex;

/* Private Member Functions */
// Read Sub-Function
bool read_Node_Desc(uint &x, node_color &color);
bool checkValWithAncestors(uint value, std::list<uint> lstList, std::list<uint> gtrList);
bool add_Node(uint index, uint value, node_color color);
void getNodePath(uint index, std::stack<uint> &st);

// Property check related sub-functions
bool checkBSTProp(node *current, bool &errorFlag, std::list<uint> lstList, std::list<uint> gtrList);
bool checkRedRedConflict(node *current);
bool checkBlackHeight(node *root, uint &blackHt);
bool checkRedsChildren(node *current);

// Delete related sub-functions
void RB_TRANSPLANT(node *u, node* v);
bool RB_FIND(uint value, node **toDelete);
node* RB_SUCCESSOR(node *current);
void RB_DELETE_FIXUP(node *x);

// Rotation related sub-functions
void LEFT_ROTATE(node *current);
void RIGHT_ROTATE(node *current);

// Tree Representation sub-functions
void printINORDER(node *root);
void print_StoreNodeInfo(node *current, rb_rep *node_rep, uint index);

// Miscellaneous
void init_sentinel();
void deleteTree(node *root);

public: // Public Member Functions 

RBTree(); // Constructor
~RBTree(); // Destructor
bool read_Tree(); // Read function
void print_RB_Tree(char* message); // Tree Representation
bool check_RB_Properties(); // Property function
void RB_DELETE(uint value); // Delete function
};

// Miscellaneous functions
bool ruint(uint &x);

int main()
{
    uint num, value;
    RBTree tree;

    // Input from File
    if(READ_FROM_FILE)
        freopen("i.txt", "r", stdin);

    // Read Tree
    if( SUCCESS != tree.read_Tree())
        return 0;

    // Check for RB Properties in the Tree
    if( SUCCESS != tree.check_RB_Properties())
        return 0;

    // Delete Nodes from RB Tree
    ruint(num);
    while(num--)
    {
        ruint(value);
        tree.RB_DELETE(value);
        tree.print_RB_Tree((char*)"Resultant Tree");
    }
    return 0;
}

/* Checks for Inorder Successor of the 'current' Node */
RBTree::node* RBTree::RB_SUCCESSOR(node *current)
{
    while(current->left != &sentinel)
        current = current->left;

    return current;
}

/* Deletes a node with 'value' from the Tree */
void RBTree::RB_DELETE(uint value)
{
    /* Reference : CLRS Section 13.4 */
    node *x, *y, *z;
    node_color yOriCol;
    
    // Sanity check
    if(SUCCESS != RB_FIND(value, &z))
    {
        printf("(%u)Value not present.\n", value);
        return;
    }

    printf("\nDeleting Node : %u\n", value);
    if(DEBUG)
        print_RB_Tree((char*)"Before Delete");

    y = z;
    yOriCol = y->color;

    if(z->left == &sentinel)
    {
        x = z->right;
        RB_TRANSPLANT(z, z->right);
    }
    else if(z->right == &sentinel)
    {
        x = z->left;
        RB_TRANSPLANT(z, z->left);
    }
    else
    {
        node *y = RB_SUCCESSOR(z->right);
        yOriCol = y->color;
        x = y->right;

        if(y->parent == z)
            x->parent = y;
        else
        {
            RB_TRANSPLANT(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        RB_TRANSPLANT(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    print_RB_Tree((char*)"Successor node transplanted ");

    if(yOriCol == BLACK)
        RB_DELETE_FIXUP(x);
}

/* Restores all RB Properties after Node-Deletion 
   Sub-function : RB_DELETE */
void RBTree::RB_DELETE_FIXUP(node *x)
{
    /* Reference : CLRS Section 13.4 */
    node *w;
    while(x != root && x->color == BLACK)
    {
        if( x == x->parent->left)
        {
            w = x->parent->right;
            if(w->color == RED) // Case 1.
            {
                w->color = BLACK;
                x->parent->color = RED;
                LEFT_ROTATE(x->parent);
                w = x->parent->right;
                print_RB_Tree((char*)"Case 1");
            }

            if(w->left->color == BLACK && w->right->color == BLACK) // Case 2.
            {
                w->color = RED;
                x = x->parent;
                print_RB_Tree((char*)"Case 2");
            }
            else
            {
                if(w->right->color == BLACK) // Case 3.
                {
                    w->left->color = BLACK;
                    w->color = RED;
                    RIGHT_ROTATE(w);
                    w = x->parent->right;
                    print_RB_Tree((char*)"Case 3");
                }
                w->color = x->parent->color; // Case 4.
                x->parent->color = BLACK;
                w->right->color = BLACK;
                LEFT_ROTATE(x->parent);
                x = root;
                print_RB_Tree((char*)"Case 4");
            }
        }
        else
        {
            w = x->parent->left;
            if(w->color == RED) // Case 1.
            {
                w->color = BLACK;
                x->parent->color = RED;
                RIGHT_ROTATE(x->parent);
                w = x->parent->left;
                print_RB_Tree((char*)"Case 1");
            }

            if(w->right->color == BLACK && w->left->color == BLACK) // Case 2.
            {
                w->color = RED;
                x = x->parent;
                print_RB_Tree((char*)"Case 2");
            }
            else
            {
                if(w->left->color == BLACK) // Case 3.
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    LEFT_ROTATE(w);
                    w = x->parent->left;
                    print_RB_Tree((char*)"Case 3");
                }
                w->color = x->parent->color; // Case 4.
                x->parent->color = BLACK;
                w->left->color = BLACK;
                RIGHT_ROTATE(x->parent);
                x = root;
                print_RB_Tree((char*)"Case 4");
            }
        }
    }
    x->color = BLACK;
}

/* Searches for the Node with 'value' 
   Returns: Pointer to the node in 'toDelete' */
bool RBTree::RB_FIND(uint value, node **toDelete)
{
    node *current = root;
    while(current != &sentinel)
    {
        if(current->val == value)
        {
            *toDelete = current;
            return SUCCESS;
        }
        else if(current->val < value)
            current = current->right;
        else if(current->val > value)
            current = current->left;
    }
    return FAILURE;
}

/* Fast IO operation */
bool ruint(uint &x)
{
    register char c = getchar_unlocked();
    x = 0;
    if(c == EOF)
        return FAILURE;
    while (!(c>=48 && c<=57))
        c = getchar_unlocked();

    for(;(c>47 && c<58);c=getchar_unlocked())
        x = (x<<1) + (x<<3) + c - 48;

    return SUCCESS;
}

/* Reads Node Description */
bool RBTree::read_Node_Desc(uint &x, node_color &color)
{
    register char c = getchar_unlocked();
    if (c == '\n' || c == EOF)
        return FAILURE;
    else
    {
        x = 0;
        while (!(c>=48 && c<=57))
        {
            if( c == 'X') // Null Node
            {
                x = NULL_NODE;
                return SUCCESS;
            }
            else if(c == EOF || c == '\n' )
                return FAILURE;
            c = getchar_unlocked();
        }

        for(;(c>47 && c<58);c=getchar_unlocked())
            x = (x<<1) + (x<<3) + c - 48;

        if( c == 'R' || c == 'B')
            color = (c == 'R') ? RED : BLACK;
        else
        {
            LOG_ERROR("Invalid Input Color. Only Red or Black Nodes!\n");
            return FAILURE;
        }
    }
    return SUCCESS;
}

/* Store the Node Information used for Print
   Sub-function: print_RB_Tree */
void RBTree::print_StoreNodeInfo(node *current, rb_rep *node_rep, uint index)
{
    node_rep[index].value = current->val;
    node_rep[index].color = current->color;

    if(current->left != &sentinel)
        print_StoreNodeInfo(current->left, node_rep, (2*index)+1);

    if(current->right != &sentinel)
        print_StoreNodeInfo(current->right, node_rep, (2*index)+2);
}

/* Prints the RB Tree in Array Representation */
void RBTree::print_RB_Tree(char* message)
{
    uint it;
    rb_rep node_rep[32];
    for(it = 0; it < maxIndex; it++)
        node_rep[it].value = 0xffffffff;

    if(message != NULL)
        printf("%s: ", message);

    print_StoreNodeInfo(root, node_rep, 0);

    for(it = 0; it < maxIndex; it++)
        node_rep[it].value != 0xffffffff ? printf("%u%c ", node_rep[it].value, node_rep[it].color == RED ? 'R' : 'B') : printf("X ");

    printf("\n");
}

/* Deallocates Tree */
void RBTree::deleteTree(node *current_root)
{
    node* current = current_root;

    if(current->left != &sentinel)
        deleteTree(current->left);

    if(current->right != &sentinel)
        deleteTree(current->right);

    free(current);
}

/* Tree Constructor */
RBTree::RBTree()
{
    root = &sentinel;

    // Initialize sentinel
    init_sentinel();
}

/* Tree Destructor */
RBTree::~RBTree()
{
    // Deallocates Tree
    deleteTree(root);
    root = NULL;
}

/* Gets Node's path from the root */
void RBTree::getNodePath(uint index, std::stack<uint> &st)
{
    if(DEBUG) printf("%u: ", index);
    while(!st.empty())
        st.pop();

    while(index != 0)
    {
        if(DEBUG) printf("%u ", index);
        st.push(index);
        if (index % 2 == 1) // Odd node
            index = (index-1)/2;
        else // if((index % 2 == 0) // Even node
            index = (index-2)/2;
    }
    if(DEBUG) printf("\n");
}

/* Adds a node into the Tree */
bool RBTree::add_Node(uint index, uint value, node_color color)
{
    if( index == 0 )
    {
        root = (node*)malloc(sizeof(node));
        root->color = color;
        root->left = root->right = root->parent = &sentinel;
        root->val = value;
    }
    else
    {
        // Stack maintains the node's path starting from the root
        std::stack<uint> st;
        getNodePath(index, st);

        uint i,j,k;
        node *temp, *current = root;
        temp = (node*)malloc(sizeof(node));
        temp->color = color;
        temp->val = value;
        temp->left = temp->right = temp->parent = &sentinel;

        while(!st.empty())
        {
            i = st.top();
            st.pop();

            if(i % 2 == 0) // Right child
            {
                if(current->right != &sentinel)
                    current = current->right;
                else
                {
                    if(st.empty())
                    {
                        temp->parent = current;
                        current->right = temp;
                    }
                    else
                    {
                        LOG_ERROR("Value to be inserted has a NULL ancestor.\n");
                        return FAILURE;
                    }
                }
            }
            else //if( i % 2 == 1) Left Child
            {
                if(current->left != &sentinel)
                    current = current->left;
                else
                {
                    if(st.empty())
                    {
                        temp->parent = current;
                        current->left = temp;
                    }
                    else
                    {
                        LOG_ERROR("Value to be inserted has a NULL ancestor.\n");
                        return FAILURE;
                    }
                }
            }
        }
    }
    return SUCCESS;
}

/* Reads Tree description */
bool RBTree::read_Tree()
{
    uint value, index = 0;
    node_color color;

    while( SUCCESS == read_Node_Desc(value, color))
    {
        if(value != NULL_NODE)
        {
            if( FAILURE == add_Node(index, value, color)) {
                LOG_ERROR("Invalid Input.\n");
                return FAILURE;
            }
        }
        index++;
    }
    maxIndex = index;
    return SUCCESS;
}

/* Initializes the Tree's sentinel node */
void RBTree::init_sentinel()
{
    sentinel.color = BLACK;
    sentinel.left = sentinel.parent = sentinel.right = NULL;
    sentinel.val = 0;
}

/* Checks all RB Tree Properties */
bool RBTree::check_RB_Properties()
{
/*
    1. Tree should be a BST (Binary Search Tree).
    2. All nodes should be either Red or Black.
    3. Root and leaf-nodes(sentinel) are Black.
    4. Parent of every Red node is Black | Every Red Node has Black Children.
    5. All simple paths from a node to a descendant leaf of X should have the same #of Black nodes.
*/
    // Sanity Check
    if(root == &sentinel)
    {
        LOG_ERROR("Tree is empty!!!\n");
        return FAILURE;
    }

    // Property 1. BST
    bool errorFlag = FALSE;
    std::list<uint> lstList, gtrList;
    checkBSTProp(root, errorFlag, lstList, gtrList);
    if(TRUE == errorFlag)
    {
        LOG_INFO("Invalid Tree: Tree is not a BST.\n");
        return FAILURE;
    }

    // Property 2. All nodes be either RED or BLACK : Check during Input

    // Property 3. Black Root
    if( BLACK != root->color)
    {
        LOG_INFO("Root is RED. Tree does not contain a black root!\n");
        return FAILURE;
    }

    // Property 4. Parent of every Red node is Black OR  Every Red Node has Black Children.
    if( SUCCESS != checkRedRedConflict(root))
    {
        LOG_INFO("Red-Red Conflict. A RED node contains a Red Child!\n");
        return FAILURE;
    }

    // Property 5. Black Height of all the nodes should be balanced.
    uint blackHt = 0;
    if( SUCCESS != checkBlackHeight(root, blackHt))
    {
        LOG_INFO("Black Height of the Tree is not balanced!\n");
        return FAILURE;
    }
    else
        if(DEBUG) printf("Black Height of the Tree : %u\n", blackHt);

    printf("\nTree is a RB-Tree. Verified!\n");
    return SUCCESS;
 }

/* Checks the value for Lesser and Greater than its appropriate Ancestors */
bool RBTree::checkValWithAncestors(uint value, std::list<uint> lstList, std::list<uint> gtrList)
{
    std::list<uint>::iterator it;
    for(it = lstList.begin(); it!=lstList.end(); it++)
        if(*it > value) return FAILURE;

    for(it = gtrList.begin(); it!=gtrList.end(); it++)
        if(*it <= value) return FAILURE;

    return SUCCESS;
}

/* Checks for BST Property in the Tree 
   Returns: Whether tree is BST or not by using 'errorFlag' */
bool RBTree::checkBSTProp(node *current, bool &errorFlag, std::list<uint> lstList, std::list<uint> gtrList)
{
    if(current == &sentinel) {
        LOG_ERROR("Programming Error!!!\n");
        return FAILURE;
    }
    
    /* Checks the Current node's value with all its ancestors value */
    if( FAILURE == checkValWithAncestors(current->val, lstList, gtrList)){
        errorFlag = TRUE;
        return FAILURE;
    }

    if(current->left != &sentinel) 
    {
        std::list<uint> tempGtrList = gtrList;
        tempGtrList.push_front(current->val);
        if( FAILURE == checkBSTProp(current->left, errorFlag, lstList, tempGtrList))
            return FAILURE;
    }

    if(current->right != &sentinel) 
    {
        std::list<uint> tempLstList = lstList;
        tempLstList.push_front(current->val);
        if( FAILURE == checkBSTProp(current->right, errorFlag, tempLstList, gtrList))
            return FAILURE;
    }
    return SUCCESS;
}

/* Checks whether node has RED children */
bool RBTree::checkRedsChildren(node *current)
{
    if( current->left != &sentinel)
        if(current->left->color == RED) return TRUE;

    if( current->right != &sentinel)
        if(current->right->color == RED) return TRUE;

    return FALSE;
}

/* Checks Red-Red Conflict in Tree */
bool RBTree::checkRedRedConflict(node *current)
{
    if(current->color == RED)
    {
        if( TRUE == checkRedsChildren(current))
            return FAILURE;
    }

    if( current->left != &sentinel)
        if(SUCCESS != checkRedRedConflict(current->left)) return FAILURE;

    if( current->right != &sentinel)
        if(SUCCESS != checkRedRedConflict(current->right)) return FAILURE;

    return SUCCESS;
}

/* Checks the Black Height of the Tree */
bool RBTree::checkBlackHeight(node *current, uint &blackHt)
{
    uint leftBlackHt = 0, rightBlackHt = 0;

    // Black Height of Left SubTree
    if(current->left != &sentinel)
    {
        if( SUCCESS != checkBlackHeight(current->left, leftBlackHt))
            return FAILURE;
    }

    // Black Height of Right SubTree
    if(current->right != &sentinel)
    {
        if( SUCCESS != checkBlackHeight(current->right, rightBlackHt))
            return FAILURE;
    }

    if( leftBlackHt != rightBlackHt)
    {
        if(DEBUG)
            printf("Black Height mismatch at Node|%u|. \nLeft SubTree Black Ht: |%u| \nRight SubTree Black Ht: |%u|\n",  current->val, leftBlackHt, rightBlackHt);

        return FAILURE;
    }
    else
        current->color == RED ? blackHt = rightBlackHt : blackHt = rightBlackHt+1;

    return SUCCESS;
}

/* Moves the Node-'v' to the location of Node-'u' */
void RBTree::RB_TRANSPLANT(node *u, node* v)
{
    /* Reference : CLRS Section 13.4 */
    if(u->parent == &sentinel)
        root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

/* Rotates tree Anti-clockwise w.r.t. Node='x' */
void RBTree::LEFT_ROTATE(node *x)
{
    /* Reference : CLRS Section 13.2 */
    /*
        |                       |
        O (x)                   O (y)
       / \         --->        / \
      a   O (y)           (x) O   c
         / \                 / \
        b   c               a   b 

    */
    node *y = x->right;
    x->right = y->left;
    if( y->left != &sentinel)
        y->left->parent = x;

    y->parent = x->parent;
    if(x->parent == &sentinel)
        root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

/* Rotates tree Clockwise w.r.t. Node='x' */
void RBTree::RIGHT_ROTATE(node *x)
{
    /* Reference : CLRS Section 13.2 */
    /*
        |                    |    
   (x)  O                    O (y)
       / \      --->        / \
  (y) O   c                a   O (x)
     / \                      / \
    a   b                    b   c
    */
    node *y = x->left;
    x->left = y->right;
    if( y->right != &sentinel)
        y->right->parent = x;

    y->parent = x->parent;
    if(x->parent == &sentinel)
        root = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;
}

/* Prints Inorder Tree Traversal */
void RBTree::printINORDER(node *current_root)
{
    node *current = current_root;
    if(current != &sentinel)
    {
        if(current->left != &sentinel)
            printINORDER(current->left);
        printf("%u | (%c)\n", current->val, (current->color == RED)? 'R' : 'B');
        if(current->right != &sentinel)
            printINORDER(current->right);
    }
}
