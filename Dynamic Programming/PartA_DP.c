#include <stdio.h>
#include <stdlib.h>

#define uint unsigned int
#define READ_FROM_FILE 0
#define DEBUG 0

/* Function Prototypes */ 
// Functions related to printing Schedule
void printSchedule(uint** dpMatrix, uint **pathMatrix, uint nDays);
uint getChoiceOnNDay(uint* dpMatrix, uint nDays);

// Miscellaneous functions
uint max(uint a, uint b);
void printDPMatrix(uint** matrix, uint rows);

int main()
{
    uint i,nDays;
    uint fast, low, high, **dpMatrix, **pathMatrix;

    // Read input from File
    if(READ_FROM_FILE)
        freopen("i.txt", "r", stdin);

    scanf("%u", &nDays);

    // Memory Allocation
    dpMatrix = (uint**)malloc(sizeof(uint*) * nDays);
    pathMatrix = (uint**)malloc(sizeof(uint*) * nDays);
    for(i=0;i<nDays;i++)
    {
        dpMatrix[i] = (uint*)malloc(sizeof(uint) * 3);
        pathMatrix[i] = (uint*)malloc(sizeof(uint) * 3);
    }

    // Input
    scanf("%u %u %u\n", &fast, &low, &high);

    dpMatrix[0][0] = fast;
    dpMatrix[0][1] = low;
    dpMatrix[0][2] = high;

    for(i=1;i<nDays;i++)
    {
        scanf("%u %u %u\n", &fast, &low, &high);

        // Fasting -> L-f | H-f
        dpMatrix[i][0] = max(dpMatrix[i-1][1], dpMatrix[i-1][2]) + fast;
        pathMatrix[i][0] = dpMatrix[i-1][1] >= dpMatrix[i-1][2] ? 1 : 2;

        // Low Calorie Food -> L-L | H-L
        dpMatrix[i][1] = max(dpMatrix[i-1][1], dpMatrix[i-1][2]) + low;
        pathMatrix[i][1] = dpMatrix[i-1][1] >= dpMatrix[i-1][2] ? 1 : 2;

        // High Calorie Food -> f-H
        dpMatrix[i][2] = dpMatrix[i-1][0] + high;
        pathMatrix[i][2] = 0;
    }

    if(DEBUG)
        printDPMatrix(dpMatrix, nDays);

    printf("Maximum Calories consumed: %u\n", max( dpMatrix[nDays-1][0], max(dpMatrix[nDays-1][1], dpMatrix[nDays-1][2])));

    // Print the schedule
    printSchedule(dpMatrix, pathMatrix, nDays);

    // Deallocate Memory 
    for(i=0;i<nDays;i++)
    {
        free(dpMatrix[i]);
        free(pathMatrix[i]);
    }
    free(dpMatrix);
    free(pathMatrix);
    
    return 0;
}

/* Prints the schedule using backtracking approach */
void printSchedule(uint** dpMatrix, uint **pathMatrix, uint nDays)
{
    int i, choice;
	choice = getChoiceOnNDay(dpMatrix[nDays-1], nDays);

    printf("Schedule :\n");
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

/* Returns the choice on the N-th Day */
uint getChoiceOnNDay(uint* dpMatrix, uint nDays)
{
    if(dpMatrix[0] >= dpMatrix[1] && dpMatrix[0] >= dpMatrix[2])
        return 0;
    else if(dpMatrix[1] >= dpMatrix[2] && dpMatrix[1] >= dpMatrix[0])
        return 1;
    else if(dpMatrix[2] >= dpMatrix[0] && dpMatrix[2] >= dpMatrix[1])
        return 2;
}

/* Returns maximum of two unsigned values */
uint max(uint a, uint b)
{
    return a > b ? a : b;
}

/* Debugging : Prints the 2-D Dynamic Programming Table */
void printDPMatrix(uint** matrix, uint rows)
{
    uint i;
    printf("\nDynamic Programming Table:\n");
    for(i=0;i<rows;i++)
        printf("%d\t%d\t%d\n", matrix[i][0], matrix[i][1], matrix[i][2]);
}
