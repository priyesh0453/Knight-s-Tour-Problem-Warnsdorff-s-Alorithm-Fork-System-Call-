#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int N;
#define noOfMoves (N*N)

static int generalPathX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
static int generalPathY[8] = {1, 2, 2, 1, -1, -2, -2, -1};

int rowIndex[2502], colIndex[2502];

int validMove(int path[noOfMoves], int Inext, int Jnext)
{
	if(Inext < 0 || Jnext < 0 || Inext >= N || Jnext >= N || path[Jnext * N + Inext] != -1)
    {
        return 0;
    }
    return 1;
}

int numberOfEmptyIndexes(int path[noOfMoves], int Inext, int Jnext)
{
	int count = 0, i = 0, inext, jnext;
	
	for(; i < 8; i++)
	{
	    inext = Inext + generalPathX[i];
	    jnext = Jnext + generalPathY[i];
	    
		if(validMove(path, inext, jnext) == 1)
		{
			count++;
		}
	}
	return count;
}

int nextIndex(int path[noOfMoves], int *Inext, int *Jnext)
{
	int inext, jnext, noOfEmptyIndexes, leastNearBy_EmptyAdjacentIndex = -1, minNumberOf_EmptyAdjacentIndexes = 9;
	
	int *map = mmap(NULL, sizeof *map, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(fork() == 0)
	{
		int startIndex = rand() % 8, count = 0, i;
		
		for(; count < 8; count++)
		{
			i = (startIndex + count) % 8;
			inext = *Inext + generalPathX[i];
			jnext = *Jnext + generalPathY[i];
			noOfEmptyIndexes = numberOfEmptyIndexes(path, inext, jnext);
			
			if((validMove(path, inext, jnext)) && (noOfEmptyIndexes < minNumberOf_EmptyAdjacentIndexes))
			{
				leastNearBy_EmptyAdjacentIndex = i;
				minNumberOf_EmptyAdjacentIndexes = noOfEmptyIndexes;
			}
		}
		*map = leastNearBy_EmptyAdjacentIndex;
		exit(0);
	}
	else
	{
		wait(NULL);
		
		if(*map == -1)
		{
			munmap(map, sizeof *map);
			return 0;
		}
		
		inext = *Inext + generalPathX[*map];
		jnext = *Jnext + generalPathY[*map];

		path[jnext * N + inext] = path[(*Jnext) * N + (*Inext)] + 1;

		*Inext = inext;
		*Jnext = jnext;
		
		munmap(map, sizeof *map);
		
		return 1;
	}
}

int KnightMoves(int StartX, int StartY)
{
	int path[noOfMoves], count = 0, quo, rem, Inext = StartX, Jnext = StartY, i = 0, lim = noOfMoves - 2, lim1 = noOfMoves - 1;
    memset(path, -1, sizeof(path));
    
	path[Jnext * N + Inext] = 1; 
	
	for(; i <= lim; i++)
	{
		if(nextIndex(path, &Inext, &Jnext) == 0)
		{
			return 0;
		}
	}

    for(; count <= lim1; count++)
    {
        quo = count / N;
        rem = count % N;
        rowIndex[path[count]] = quo;
        colIndex[path[count]] = rem;
    }
	return 1;
}

int main(int argc, char *argv[])
{
    if(argc != 4)
	{
		printf("Usage: ./Knight.out grid_size StartX StartY");
		exit(-1);
	}

	N = atoi(argv[1]);
	int StartX = atoi(argv[2]);
	int StartY = atoi(argv[3]);
    
    if(N > 0 && N <= 50 && (StartX >= 0 && StartX < N) && (StartY >= 0 && StartY < N))
    {
        if(N == 1)
        {
            printf("0,0|");
        }
        else if(N <= 4 && N > 1)
        {
            printf("No Possible Tour");
        }
        else
        {
            if(KnightMoves(StartX, StartY) == 0)
            {
                printf("No Possible Tour");
            }
            else
            {
                int i = 1;
                
                for(; i <= noOfMoves; i++)
                {
                    printf("%d,%d|", rowIndex[i], colIndex[i]);
                }
            }
        }
    }
    else
    {
        printf("Invalid Input!");
    }
	return 0;
}