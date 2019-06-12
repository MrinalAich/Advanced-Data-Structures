#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define uint unsigned int
#define READ_FROM_FILE 0
#define DEBUG 0

/* Data Structures */
/* Enum for the food type */
typedef enum foodChoice {
    fasting = 0,
    low = 1,
    high = 2
} fchoice;

/* Enum for result of the operation */
enum Result {
    FAILURE = 0,
    SUCCESS = 1
};

/* Memory element */
typedef struct memory_element {
    uint day;
    fchoice choice;
    uint calorieConsumed;
} memory;

/* Linked list to maintain memory */
typedef struct ll {
    memory data;
    struct ll *next;
} node;

/* Global Variable */
uint **chart = NULL, **pathMatrix = NULL; // Input Calorie Chart
node** header = NULL; // Linked-list based Memorization

/* Function Prototypes */
// Recurrence function
uint calories(uint dayIndex, enum foodChoice choice);

//Memorization related functions
bool searchInMemory(uint dayIndex, enum foodChoice choice, uint *result);
void storeInMemory(uint result, uint dayIndex, enum foodChoice choice);

// Functions related to Printing Schedule
void printSchedule(uint nDays, uint choice);
enum foodChoice choiceOnNDay(uint val1, uint val2, uint val3);

// Miscellaneous functions
uint max(uint a, uint b);
void printMatrix(uint** matrix, uint nDays);

int main()
{
    uint i, day, nDays, result, val1, val2;
    uint fastChoice, lowChoice, highChoice, finalDaychoice;

    // Read input from File
    if(READ_FROM_FILE)
       freopen("i.txt", "r", stdin);

    scanf("%u", &nDays);

    // Memory Allocation
    chart = (uint**)malloc(sizeof(uint*) * nDays);
    pathMatrix = (uint**)malloc(sizeof(uint*) * nDays);
    for(i=0;i<nDays;i++)
    {
        chart[i] = (uint*)malloc(sizeof(uint) * 3);
        pathMatrix[i] = (uint*)malloc(sizeof(uint) * 3);
    }

    header = (node**)malloc(sizeof(node*));
    *header = NULL;

    // Input
    for(day=0;day<nDays;day++)
        scanf("%u %u %u\n", &chart[day][fasting], &chart[day][low], &chart[day][high]);

    if(DEBUG)
        printMatrix(chart, nDays);

    if(nDays == 1)
    {
        result = max( chart[nDays-1][fasting], max(chart[nDays-1][low], chart[nDays-1][high]));
        finalDaychoice = choiceOnNDay(chart[nDays-1][fasting], chart[nDays-1][low], chart[nDays-1][high]);
    }
    else
    {
        // Recurrence based on memorization

        // Fasting
        val1 = calories(nDays-2,low);
        val2 = calories(nDays-2,high);
        fastChoice = chart[nDays-1][fasting] + max(val1, val2);
        pathMatrix[nDays-1][fasting] = (val1 >= val2) ? low : high;

        // Low Calorie Food
        val1 = calories(nDays-2,low);
        val2 = calories(nDays-2,high);
        lowChoice  = chart[nDays-1][low] + max(val1, val2);
        pathMatrix[nDays-1][low] = (val1 >= val2) ? low : high;

        // High Calorie Food
        highChoice = chart[nDays-1][high] + calories(nDays-2,fasting);
        pathMatrix[nDays-1][high] = fasting;

        // Result
        result = max( max(fastChoice, lowChoice), highChoice);
        finalDaychoice = choiceOnNDay(fastChoice, lowChoice, highChoice);
    }

    printf("Maximum Calories consumed: %u", result);
    // Print the schedule
    printSchedule(nDays, finalDaychoice);

    // Deallocate Memory
    for(i=0;i<nDays;i++)
    {
        free(chart[i]);
        free(pathMatrix[i]);
    }
    free(chart);
    free(pathMatrix);

    node *prev, *current = *header;
    while(current != NULL)
    {
        prev = current;
        current = current->next;
        free(prev);
    }
    
    return 0;
}

/* Recursive function: to get maximum cumulative calories intake
   for a particular food Choice at that particular day */
uint calories(uint dayIndex, enum foodChoice choice)
{
    uint result = 0, val1 = 0, val2 = 0;

    // Check in memory
    if(searchInMemory(dayIndex, choice, &result) == SUCCESS)
        return result;

    if(dayIndex == 0)
        result = chart[0][choice];
    else
    {
        switch(choice)
        {
        case fasting:
            val1 = calories(dayIndex-1, low);
            val2 = calories(dayIndex-1, high);
            result = chart[dayIndex][choice] + max(val1, val2);
            pathMatrix[dayIndex][choice] = (val1 >= val2) ? low : high;
            break;

        case low:
            val1 = calories(dayIndex-1, low);
            val2 = calories(dayIndex-1, high);
            result = chart[dayIndex][choice] + max(val1, val2);
            pathMatrix[dayIndex][choice] = (val1 >= val2) ? low : high;
            break;

        case high:
            result = chart[dayIndex][choice] + calories(dayIndex-1, fasting);
            pathMatrix[dayIndex][choice] = fasting;
            break;
        }
    }

    // Store in memory
    storeInMemory(result, dayIndex, choice);

    return result;
}

/* Search for the tuple in the memory */
bool searchInMemory(uint dayIndex, enum foodChoice choice, uint *result)
{
    // Sanity Check
    if(*header == NULL || result == NULL)
        return FAILURE;

    node *current = NULL;
    current = *header;
    while(current != NULL)
    {
        if(current->data.choice == choice && current->data.day == dayIndex)
        {
            *result = current->data.calorieConsumed;
            break;
        }
        else
            current = current->next;
    }
    return current == NULL ? FAILURE : SUCCESS;
}

/* Store the result in a tuple in memory */
void storeInMemory(uint result, uint dayIndex, enum foodChoice choice)
{    
    if(*header == NULL)
    {
        *header = (node*)malloc(sizeof(node));
        (*header)->data.calorieConsumed = result;
        (*header)->data.choice = choice;
        (*header)->data.day = dayIndex;
        (*header)->next = NULL;
    }
    else
    {
        // Store memory at the front
        node *temp = (node*)malloc(sizeof(node));
        temp->data.calorieConsumed = result;
        temp->data.choice = choice;
        temp->data.day = dayIndex;
        temp->next = *header;
        *header = temp;
    }
}

/* Prints the schedule using backtracking approach */
void printSchedule(uint nDays, uint choice)
{
    int i;
    printf("\nSchedule :\n");
    for(i = nDays-1; i>=0; i--)
    {
        if(choice == 0)
            printf("Day %d : Fasting\n", i+1);
        else if(choice == 1)
            printf("Day %d : Low Calorie Food\n", i+1);
        else // choice == 2
            printf("Day %d : High Calorie Food\n", i+1);
        choice = pathMatrix[i][choice];
    }
}

/* Returns the choice of food among the 3 types 
   (fastChoice, lowChoice, highChoice) */
enum foodChoice choiceOnNDay(uint val1, uint val2, uint val3)
{
    if(val1 >= val2 && val1 >= val3)
        return fasting;
    else if(val2 >= val3 && val2 >= val1)
        return low;
    else if(val3 >= val2 && val3 >= val1)
        return high;
}

/* Returns maximum of two unsigned values */
uint max(uint a, uint b)
{
    return  a > b ? a : b;
}

/* Debugging : Prints the 2-D Matrix */
void printMatrix(uint** matrix, uint nDays)
{
    uint day;
    printf("\nInput Chart:(Fast, Low, High)\n");
    for(day=0;day<nDays;day++)
        printf("Day %u : %d\t%d\t%d\n", day+1, matrix[day][fasting], matrix[day][low], matrix[day][high]);
}
