#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define uint unsigned int
#define READ_FROM_FILE 0
#define DEBUG 1

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
//uint** chart = NULL, 
uint** pathMatrix = NULL; // Input Calorie Chart
node** header = NULL; // Linked-list based Memorization

/* Function Prototypes */
void doDynamicProg(uint **chart, uint nDays);
void doRecursion(uint **chart, uint nDays, bool memorization);

// Recurrence function
uint calories(uint **chart, uint dayIndex, enum foodChoice choice, bool memorization);

//Memorization related functions
bool searchInMemory(uint dayIndex, enum foodChoice choice, uint *result);
void storeInMemory(uint result, uint dayIndex, enum foodChoice choice);

// Miscellaneous functions
uint max(uint a, uint b);

// Functions related to printing Schedule
void printSchedule(uint **chart, uint nDays, uint finalDayChoice);
enum foodChoice choiceOnNDay(uint val1, uint val2, uint val3);

int main()
{
    uint **chart, i, nDays;

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

    // Input
    for(i=0;i<nDays;i++)
        scanf("%u %u %u\n", &chart[i][0], &chart[i][1], &chart[i][2]);
       
    // Recursion without Memorization approach
    printf("\nRecursion Without Memorization\n");
    doRecursion(chart, nDays, 0);
    
    // Recursion with Memorization approach
    header = (node**)malloc(sizeof(node*));
    *header = NULL;
    printf("\nRecursion with Memorization\n");
    doRecursion(chart, nDays, 1);
    
    // Dynamic Programming approach
    printf("\nDynamic Programming\n");
    doDynamicProg(chart, nDays);

    // Deallocate Memory
    for(i=0;i<nDays;i++)
    {
        free(chart[i]);
        free(pathMatrix[i]);
    }
    free(chart);
    free(pathMatrix);

    return 0;
}

/* Performs Bottom-up Dynamic Programming approach */
void doDynamicProg(uint **chart, uint nDays)
{
    uint **dpMatrix, i, j, finalDayChoice;
    struct timespec start, end;
    
    //Sanity check
    if(chart == NULL || nDays==0)
    {
        printf("Input Error.\n");
        return;
    }
    
	// Initialize pathMatrix for reuse
    for(i=0;i<nDays;i++)
    {
        for(j=0;j<3;j++)
            pathMatrix[i][j] = 0;
    }
	
    // Allocate Memory
    dpMatrix = (uint**)malloc(sizeof(uint*) * nDays);
    for(i=0;i<nDays;i++)
        dpMatrix[i] = (uint*)malloc(sizeof(uint) * 3);
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
    dpMatrix[0][fasting] = chart[0][fasting];
    dpMatrix[0][low]     = chart[0][low];
    dpMatrix[0][high]    = chart[0][high];
    //dpMatrix[0][2] = 0;

    for(i=1;i<nDays;i++)
    {
        // Fasting -> L-f | H-f
        dpMatrix[i][fasting]   = chart[i][fasting] + max(dpMatrix[i-1][low], dpMatrix[i-1][high]);
        pathMatrix[i][fasting] = dpMatrix[i-1][low] >= dpMatrix[i-1][high] ? low : high;

        // Low Calorie Food -> L-L | H-L
        dpMatrix[i][low]   = chart[i][low] + max(dpMatrix[i-1][low], dpMatrix[i-1][high]);
        pathMatrix[i][low] = dpMatrix[i-1][low] >= dpMatrix[i-1][high] ? low : high;

        // High Calorie Food -> f-H
        dpMatrix[i][high]   = chart[i][high] + dpMatrix[i-1][fasting];
        pathMatrix[i][high] = fasting;
    }
 
    if(DEBUG)
    {
        printf("Maximum Calories consumed: %u\n", max( dpMatrix[nDays-1][0], max(dpMatrix[nDays-1][1], dpMatrix[nDays-1][2])));
        // Print the schedule
        finalDayChoice = choiceOnNDay(dpMatrix[nDays-1][fasting], dpMatrix[nDays-1][low], dpMatrix[nDays-1][high]);
        printSchedule(chart, nDays, finalDayChoice);
    }
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    size_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Dynamic Programming execution time: %zu microseconds\n", delta_us);
    
    // Deallocate Memory
    for(i=0;i<nDays;i++)
        free(dpMatrix[i]);
    free(dpMatrix);

    return;
}

/* Performs recursion based on the flag 'memorization' */
void doRecursion(uint **chart, uint nDays, bool memorization)
{
    uint result, val1, val2, i, j;
    uint fastChoice, lowChoice, highChoice, finalDaychoice;
    struct timespec start, end;

    //Sanity check
    if(chart == NULL || nDays==0)
    {
        printf("Input Error.\n");
        return;
    }
    
    // Initialize pathMatrix for reuse
    for(i=0;i<nDays;i++)
    {
        for(j=0;j<3;j++)
            pathMatrix[i][j] = 0;
    }
   
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
    if(nDays == 1)
    {
        result = max(chart[nDays-1][fasting], max(chart[nDays-1][low], chart[nDays-1][high]));
        finalDaychoice = choiceOnNDay(chart[nDays-1][fasting], chart[nDays-1][low], chart[nDays-1][high]);
    }
    else
    {
        // Recursive Call

        // Fasting
        val1 = calories(chart,nDays-2,low,memorization);
        val2 = calories(chart,nDays-2,high,memorization);
        fastChoice = chart[nDays-1][fasting] + max(val1, val2);
        pathMatrix[nDays-1][fasting] = (val1 >= val2) ? low : high;

        // Low Calorie Food
        val1 = calories(chart,nDays-2,low,memorization);
        val2 = calories(chart,nDays-2,high,memorization);
        lowChoice  = chart[nDays-1][low] + max(val1, val2);
        pathMatrix[nDays-1][low] = (val1 >= val2) ? low : high;

        // High Calorie Food
        highChoice = chart[nDays-1][high] + calories(chart,nDays-2,fasting,memorization);
        pathMatrix[nDays-1][high] = fasting;

        // Result
        result = max( max(fastChoice, lowChoice), highChoice);
        finalDaychoice = choiceOnNDay(fastChoice, lowChoice, highChoice);
    }
    
    if(DEBUG)
    {
        printf("Maximum Calories consumed: %u\n", result);
        // Print the schedule
        printSchedule(chart, nDays, finalDaychoice);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    size_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Recursion with%s memorization execution time: %zu microseconds\n", memorization ? "" : "out", delta_us);
    
    // Cleanup memory
    if(memorization)
    {
        node *prev, *current = *header;
        while(current != NULL)
        {
            prev = current;
            current = current->next;
            free(prev);
        }
    }

    return;
}

/* Recursive function: to get maximum cumulative calories intake
 for a particular food Choice at that particular day */
uint calories(uint **chart, uint dayIndex, enum foodChoice choice, bool memorization)
{
    uint result, val1, val2;

    // Check in memory
    if(memorization && searchInMemory(dayIndex, choice, &result) == SUCCESS)
        return result;

    if(dayIndex == 0)
        result = chart[0][choice];
    else
    {
        switch(choice)
        {
        case fasting:
            val1 = calories(chart, dayIndex-1, low, memorization);
            val2 = calories(chart, dayIndex-1, high, memorization);
            result = chart[dayIndex][choice] + max(val1, val2);
            pathMatrix[dayIndex][choice] = (val1 >= val2) ? low : high;
            break;

        case low:
            val1 = calories(chart, dayIndex-1, low, memorization);
            val2 = calories(chart, dayIndex-1, high, memorization);
            result = chart[dayIndex][choice] + max(val1, val2);
            pathMatrix[dayIndex][choice] = (val1 >= val2) ? low : high;
            break;

        case high:
            result = chart[dayIndex][choice] + calories(chart, dayIndex-1, fasting, memorization);
            pathMatrix[dayIndex][choice] = fasting;
            break;
        }
    }

    // Store in memory
    if(memorization)
        storeInMemory(result, dayIndex, choice);

    return result;
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

/* Prints the schedule using backtracking approach */
void printSchedule(uint **chart, uint nDays, uint choice)
{
    int i;
    printf("Schedule :\n");
    for(i = nDays-1; i>=0; i--)
    {
        if(choice == 0)
            printf("Day \t%d : Fasting \t\t Calories: %u\n", i+1, chart[i][choice]);
        else if(choice == 1)
            printf("Day \t%d : Low Calorie \t Calories: %u\n", i+1, chart[i][choice]);
        else if(choice == 2)
            printf("Day \t%d : High Calorie \t Calories: %u\n", i+1, chart[i][choice]);
        else
            printf("Programming Error.!!!\n");
        choice = pathMatrix[i][choice];
    }
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

    return;
}

/* Returns maximum of two unsigned values */
uint max(uint a, uint b)
{
    return a > b ? a : b;
}
