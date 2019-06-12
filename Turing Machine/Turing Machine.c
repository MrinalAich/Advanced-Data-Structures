#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#define uint unsigned int
#define ushort unsigned short
#define MAX_FINAL_STATE 16
#define INVALID_STATE 0xffff
#define DEBUG 0

/* Enumeration of the tape symbol */
typedef enum tapeSym {
    ZERO = 0,
    ONE = 1,
    BLANK = 2
} tape_sym;

/* Enumeration of the tape movement */
typedef enum tapeMove {
    LEFT = 0,
    RIGHT = 1,
    STAY = 2
} tape_move;

/* Structure for doubly linked list for the Tape*/
typedef struct dll {
    ushort val;
    struct dll *next, *prev;
} node;

/* Maintains the 'Current State' and points to the DLL node on the tape */
typedef struct currentTapePtr {
    node* ptr;
    uint state_val;
} current_tape_ptr;

/* Element of the Transition function matrix */
typedef struct matrixElem {
    uint next_state;
    bool bool_to_wrt;
    tape_move movement;
} matrix_elem;

/* Reads a transition from the input
   Returns: Whether to continue reading transitions */
bool readTransition(uint* cs, uint* cts, uint* ns, uint* nts, tape_move* mv)
{
#define BUFF_LEN 5
    char a[BUFF_LEN], b,c[BUFF_LEN],d,e[BUFF_LEN],ch;
    ushort i,x;
    scanf("%[^,],%c,%[^,],%c,%s\n", a,&b,c,&d,e);
    // Current State
    {
        x = 0;
        for(i=1,ch=a[i];(ch>47 && ch<58);ch=a[++i])
            x = (x<<1) + (x<<3) + ch - 48;
        *cs = x;
    }

    // Current Tape Symbol
    *cts = (b == 'b') ? BLANK : (uint)(b - 48);

    // Next State
    {
        x = 0;
        for(i=1,ch=c[i];(ch>47 && ch<58);ch=c[++i])
            x = (x<<1) + (x<<3) + ch - 48;
        *ns = x;
    }

    // Tape Symbol to Write
    *nts = (uint)d - 48;

    // Movement
    *mv = (e[0] == 'L') ? LEFT : (e[0] == 'R') ? RIGHT : STAY;

    return e[1] == ';' ? 1 : 0;
}

/* Adds a transition to the transition function matrix */
void AddTransToMatrix(matrix_elem **mat, uint cs, uint cts, uint ns, uint nts, tape_move mv)
{
    // (Tape Symbol, from State) -> (Next State, Symbol To Write, Movement)
    mat[cts][cs].next_state  = ns;
    mat[cts][cs].bool_to_wrt = (bool)nts;
    mat[cts][cs].movement    = mv;
}

/* Debugging : Gets the Movement name in character format, used for display */
char get_moveName(tape_move mv)
{
	if (mv == LEFT) return 'L';
	else if (mv == RIGHT) return 'R';
	else return 'S';
}

/* Debugging : Gets the Tape symbol in character format, used for display */
char get_ts(tape_sym sy)
{
	if(sy == ZERO) return '0';
	else if (sy == ONE) return '1';
	else return 'b';
}

/* Debugging : Prints the tape contents */
void print_tape(node *head)
{
    node *current = head;
    if(head == NULL)
        return;

    while(current != NULL)
    {
        printf("|%u |", current->val);
        current = current->next;
    }
    printf("\n");
}

/* Debugging : Prints the Transition Function in readable format */
void print_transition_func(matrix_elem **mat, uint n)
{
	uint i,j;
    printf("(cs,ts)->(ns,nts,mv)\n");

    for(i=0;i<3;i++)
    {
        for(j=0;j<n;j++) printf("(q%u,%c)->(q%u,%c,%c)\t",
        	j,get_ts((tape_sym)i),mat[i][j].next_state,
            get_ts((tape_sym)mat[i][j].bool_to_wrt),get_moveName(mat[i][j].movement));
        printf("\n");
    }
}

/* Deallocates the doubly linked list's dynamically allocated memory */
void deallocate_dll(node **header)
{
    node *current, *prev;
    current = prev = *header;
    while(current != NULL )
    {
        prev = current;
        current = current->next;

        free(prev);
    }
    *header = NULL;
}

/* Adds a tape symbol onto the tape */
void tape_append(node **header, ushort val)
{
    node *current, *temp;

    if( *header == NULL) 
    {
        *header = (node*)malloc(sizeof(node));
        (*header)->val = val;
        (*header)->next = NULL;
        (*header)->prev = NULL;
    }
    else
    {
        current = *header;
        temp = (node*)malloc(sizeof(node));
        temp->val = val;
        temp->next = NULL;

        while( current->next != NULL)
            current = current->next;

        current->next = temp;
        temp->prev = current;
    }
}

/* Reads States from the Input based on the prescribed format
   Returns: States in an array and the number of states */
void readStates(uint* state_array, uint* no_states)
{
    ushort x = 0;
    char ch = 0;
    *no_states = 0;
    do
    {
        x = 0;
        while (!(ch>=48 && ch<=57))
            ch = getchar_unlocked();

        for(;(ch>47 && ch<58);ch=getchar_unlocked())
            x = (x<<1) + (x<<3) + ch - 48;

        state_array[*no_states] = x;
        (*no_states)++;

    } while(ch != ';');
}

/* Prints Instantaneous description of the Turing Machine */
void print_instantaneous_desc(node *header, current_tape_ptr tptr)
{
    node *current = header;
    while(current != NULL )
    {
        if( current == tptr.ptr )
            printf("*%c%d*", 'q', tptr.state_val);
        if( current->val == 0 || current->val == 1 )
            printf("%u", current->val);
        current = current->next;
    }
    printf("\n");
}

/* Retrieves Next Transition based on current state and tape symbol */
bool get_next_transition(matrix_elem **mat, current_tape_ptr tptr, uint* next_state, uint* next_tp_sym, tape_move* mv)
{
    if( tptr.ptr == NULL )
    {
        printf("Error in Current Ptr!!!\n");
        return 0;
    }

    if( mat[(tptr.ptr)->val][tptr.state_val].next_state == INVALID_STATE)
    {
        printf("TM Halts as Transition is not defined.\n");
        return 0;
    }

    *next_state  = mat[(tptr.ptr)->val][tptr.state_val].next_state;
    *next_tp_sym = mat[(tptr.ptr)->val][tptr.state_val].bool_to_wrt;
    *mv          = mat[(tptr.ptr)->val][tptr.state_val].movement;

    return 1;
}

/* Updates the tape contents, updates the tape_pointer and does a movement */
void update_tape(node **header, current_tape_ptr *tptr, uint ns, uint nts, tape_move mv)
{
    node *current = tptr->ptr, *temp;

    tptr->state_val = ns;

    current->val = nts;
    if( mv == RIGHT)
    {
        if( current->next == NULL )
        {
            temp = (node*)malloc(sizeof(node));
            temp->next = NULL;
            temp->val = BLANK;
            temp->prev = current;
            current->next = temp;
        }
        (*tptr).ptr = current->next;
    }
    else if(mv == LEFT)
    {
        if(current->prev == NULL)
            printf("Terminating: Hit Left Boundary.\n");
        else
            (*tptr).ptr = current->prev;
    }
}

/* Checks whether One of the final states of the TM has reached or not */
bool is_final_state_reached(current_tape_ptr tptr, uint nf, uint* fstate)
{
    while(nf)
    {
        if(tptr.state_val == fstate[nf-1])
            return 1;
        nf--;
    }
    return 0;
}

/* Working of the Turing Machine */
void run_Turing_Machine(node **header, matrix_elem **mat, uint n, current_tape_ptr tptr, uint nf, uint* fstate)
{
    uint ns, nts;
    tape_move mv;
    print_instantaneous_desc(*header, tptr);

    while(!is_final_state_reached(tptr, nf, fstate) && get_next_transition(mat, tptr, &ns, &nts, &mv))
    {
        update_tape(header, &tptr, ns, nts, mv);
        print_instantaneous_desc(*header, tptr);
    }

    return;
}

int main()
{
    uint i,j;
    bool cntd_Read;
    char ch = 0;
    uint no_states = 0,no_final_states = 0, final_states[MAX_FINAL_STATE];
    uint current_state, next_state, current_tape_sym, next_tape_sym;
    tape_move movement;
    matrix_elem **trans_matrix;
    node *header = NULL;
    current_tape_ptr tptr;

    //freopen("i.txt", "r", stdin);

    //Input
    // Total number of States
    readStates(final_states, &no_states);

    trans_matrix = (matrix_elem**)malloc(sizeof(matrix_elem*) * 3);
    for(i=0;i<3;i++)
    {
        trans_matrix[i] = (matrix_elem*)malloc(sizeof(matrix_elem) * no_states);
        for(j = 0; j < no_states; j++ )
        {
            trans_matrix[i][j].bool_to_wrt = 0;
            trans_matrix[i][j].movement = STAY;
            trans_matrix[i][j].next_state = INVALID_STATE;
        }
    }

    // Final States
    readStates(final_states, &no_final_states);

    // Transition Matrix
    do {
        cntd_Read = readTransition(&current_state, &current_tape_sym, &next_state, &next_tape_sym, &movement);
        AddTransToMatrix(trans_matrix, current_state, current_tape_sym, next_state, next_tape_sym, movement);
    } while(cntd_Read);

    // Tape Input
    while(!(ch>=48 && ch<=57))
        ch = getchar();
    for(; ch>47 && ch<58; ch = getchar())
        tape_append(&header, ch-48);

    if(DEBUG) print_tape(header);
    if(DEBUG) print_transition_func(trans_matrix,no_states);
    
    // Initial State
    tptr.state_val = 0;
    tptr.ptr = header;

    // Run Turing Machine
    run_Turing_Machine(&header, trans_matrix, no_states, tptr, no_final_states, final_states);

    // Deallocate memory
    free(trans_matrix);
    
    deallocate_dll(&header);

    return 0;
}