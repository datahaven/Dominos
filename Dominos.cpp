// Dominos.cpp : Defines the entry point for the console application.
//
// Dominos Puzzle Maker
// by Adrian Dale 2008

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef struct Domino
{
	int first;
	int second;
} DominoType;

#define MAX_GRID 18

char Grid[MAX_GRID][MAX_GRID];

// In case we ever want to do non-standard dominoes
// ie with more or less than six spots
#define MAX_DOM 30
int maxDomSpot = 6;
DominoType DominoSet[MAX_DOM];

int xGridSize = 8;
int yGridSize = 7;

int makePuzzle();
void showGrid();
int makeDominoSet();
int shuffleDominoSet();
int solvePuzzle();

int WorstCountSoFar;
int WorstPuzzleSoFar;
int SingleSolutionCount;
int PuzzleId;

int main(int argc, char* argv[])
{
	printf("Adrian's Domino Puzzle Maker\n");

	// Set up the random number generator
	// - same seed every time for predictable test results
	
	WorstCountSoFar=0;
	SingleSolutionCount=0;
	for (int i=0; i < 100; i++)
	{
		PuzzleId = i;	
        srand(i);
		//printf("Puzzle number %d:\n", i);
		if ( makePuzzle() != 0 )
		{
				showGrid();
			printf("Error for i=%d\n", i);
			
			break;
		}
			showGrid();
		//printf("\n");
		solvePuzzle();
	}

	printf("WorstSolutionCountSoFar=%d for puzzle %d\n", WorstCountSoFar, WorstPuzzleSoFar);
	printf("Number of single solution puzzles found so far: %d\n", SingleSolutionCount);
	

	return 0;
}

// makeDominoSet creates the dominoes
int makeDominoSet()
{
	int d=0;
	for (int i=0; i<maxDomSpot+1; i++)
	{
		for(int j=i; j<maxDomSpot+1; j++)
		{
		//	 printf("(%d, %d), ", i, j);
			DominoSet[d].first = i+'0';
			DominoSet[d].second = j+'0';
			d++;
		}
	}

	return 0;
}

// Does what it says on the tin by swapping
// randomly chosen pairs of dominoes
int shuffleDominoSet()
{
	for( int i=0; i<1000; ++i )
	{
		// TODO 28 should not be hard coded
		double r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * (28);
		int a = (int)r;
		r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * (28);
		int b = (int)r;
		//printf("rnd=(%d,%d),",a,b);
		DominoType temp;
		temp.first = DominoSet[a].first;
		temp.second = DominoSet[a].second;
		DominoSet[a].first = DominoSet[b].first;
		DominoSet[a].second = DominoSet[b].second;
		DominoSet[b].first = temp.first;
		DominoSet[b].second = temp.second;
	}

//	printf("\n\n");
//	for ( int d=0; d<28; d++)
//		printf("(%d, %d), ", DominoSet[d].first, DominoSet[d].second);
//	printf("\n");
	return 0;
}


// The coordinates of the two domino squares
typedef struct DominoLocation
{
	int x;
	int y;
	bool orientHorizontal;
} DominoLocationType;

typedef vector<DominoLocationType> DominoLocationList;

// A list of possible places each of the 28 dominoes could go.
DominoLocationList AvailableMoves[28];

// solvePuzzle attempts to solve the puzzle currently
// stored in Grid

char gridCopy[8][8];
int tryOutMoves( DominoLocation prevMove, int dominoNo );

int SolutionCount;

int solvePuzzle()
{
	// reset our domino set to unshuffled
	makeDominoSet();

	// TEST - This supposedly has 20160 solutions? I can't find any
	// From http://www.iwriteiam.nl/HaDPF.html
	/*
	int NewGrid[8][8] = {
	{0, 0, 0, 2, 0, 3, 0, 4 },
	{0, 1, 2, 1, 3, 1, 4, 1 },
	{0, 5, 0, 6, 2, 2, 2, 4},
	{5, 1, 6, 1, 2, 3, 4, 3},
	{2, 5, 2, 6, 4, 4, 4, 6},
	{5, 4, 6, 3, 4, 5, 6, 5},
	{1, 1, 3, 3, 3, 5, 6, 6},
	{0,0,0,0,0,0,0,0}};
	for (int o=0;o<8;o++)
		for(int p=0;p<7;p++)
			Grid[o][p] = NewGrid[p][o] + '0';
		showGrid();
	*/	
	// TEST ENDS

			// TEST - number 74 from book
	/*
		printf("\n");
	int NewGrid2[8][8] = 
	{{ 1,1,3,1,1,4,3,3},
	{5,6,4,6,0,3,2,6},
	{3,2,5,2,5,0,2,3},
	{1,0,5,6,1,6,4,5},
	{4,4,2,4,2,0,4,3},
	{2,6,2,1,6,5,0,0},
	{0,6,0,3,5,4,5,1},
	{0,0,0,0,0,0,0,0}};
	for (int o=0;o<8;o++)
		for(int p=0;p<7;p++)
			Grid[o][p] = NewGrid2[p][o] + '0';
		showGrid();
	*/	
	// TEST ENDS

	// TEST - solver loops on
/*	int NewGrid[8][8] =
	{{4,0,0,1,2,2,2,6},
	{5,5,1,6,3,2,2,4},
	{6,1,1,2,1,0,2,6},
	{5,4,1,5,5,3,6,6},
	{3,5,4,4,5,6,4,1},
	{6,3,3,4,2,0,5,3},
	{0,3,3,0,0,4,0,1}};
	for (int o=0;o<8;o++)
		for(int p=0;p<7;p++)
			Grid[o][p] = NewGrid[p][o] + '0';
		showGrid();
*/
		//TEST END

		// TEST - solver loops on
	/*
	int NewGrid[8][8] =
	{{5,1,2,5,3,4,6,3},
	{5,4,1,2,2,2,1,6},
	{5,4,3,0,4,5,3,4},
	{6,6,4,6,6,4,0,6},
	{5,2,4,5,1,1,0,2},
	{3,2,1,3,1,3,2,0},
	{3,5,0,0,6,0,1,0}};
	for (int o=0;o<8;o++)
		for(int p=0;p<7;p++)
			Grid[o][p] = NewGrid[p][o] + '0';
		showGrid();
*/

	// Hard code some test defaults
	xGridSize = 8;
	yGridSize = 7;

	// Populate the AvailableMoves list for each domino in turn
	for (int dominoNo=0; dominoNo < 28; dominoNo++ )
	{
		AvailableMoves[dominoNo].clear();

		// Try out the horizontals
		for (int y=0; y<yGridSize; y++ )
		{
			for (int x=0; x<xGridSize-1; x++ )
			{
				if ( (DominoSet[dominoNo].first == Grid[x][y] &&
					  DominoSet[dominoNo].second == Grid[x+1][y] ) ||
                     (DominoSet[dominoNo].second == Grid[x][y] &&
					  DominoSet[dominoNo].first == Grid[x+1][y] ) )
				{
					DominoLocationType loc;
					loc.orientHorizontal = true;
					loc.x = x;
					loc.y = y;
					AvailableMoves[dominoNo].push_back(loc);
					//printf("(%c,%c) can go at (%d,%d) horizontally\n",
					//	    DominoSet[dominoNo].first, DominoSet[dominoNo].second,
					//		x, y);
				}
			}
		}
		// And the verticals
		for (int y=0; y<yGridSize-1; y++ )
		{
			for (int x=0; x<xGridSize; x++ )
			{
				if ( (DominoSet[dominoNo].first == Grid[x][y] &&
					  DominoSet[dominoNo].second == Grid[x][y+1] ) ||
                     (DominoSet[dominoNo].second == Grid[x][y] &&
					  DominoSet[dominoNo].first == Grid[x][y+1] ) )
				{
					DominoLocationType loc;
					loc.orientHorizontal = false;
					loc.x = x;
					loc.y = y;
					AvailableMoves[dominoNo].push_back(loc);
					//printf("(%c,%c) can go at (%d,%d) vertically\n",
					//	    DominoSet[dominoNo].first, DominoSet[dominoNo].second,
					//		x, y);
				}
			}
		}
	}

	// Blank out the working grid that is used for solving the puzzle
	for (int y=0;y<8;y++)
		for (int x=0;x<8;x++)
			gridCopy[x][y] = ' ';

	DominoLocation nullMove;
	nullMove.orientHorizontal = true;
	nullMove.x = 100;
	nullMove.y = 0;
	SolutionCount = 0;
	tryOutMoves( nullMove, 0 );

	printf("Puzzle %d has %d solutions\n", PuzzleId, SolutionCount);

	if (SolutionCount > WorstCountSoFar)
	{
		WorstCountSoFar = SolutionCount;
		WorstPuzzleSoFar = PuzzleId;
	}

	if ( SolutionCount == 1 )
	{
		SingleSolutionCount++;
	}

	return 0;
}

// tryOutMoves is a recursive function which trys out
// all possible placings for the specified domino in the 
// current grid. It tries out all possible placings for
// the dominos and keeps a count of the number of 
// solutions found.
//
// Return value is not used.
// It takes a description of the previous move and the 
// number of the domino to place next.
//
// It assumes that the gridCopy global contains a valid
// set of moves so far from previous calls to the function.
// It also assumes that the AvailableMoves global contains
// a list of valid moves available for each domino.
// This is set up in solvePuzzle()
//
// From the gridCopy position at the time the function
// is called, the function tries out each possible move
// for the specified dominoNo in turn. For each move it 
// calls tryOutMoves again for the next domino.
//
// Note that to maintain the validity of gridCopy it must
// undo the previously tried move for this domino before
// trying the next one.
//
// Once all possible moves have been tried for this domino
// the previous move for the domino before this one must
// be undone so that the gridCopy remains valid for the
// caller of tryOutMoves
int tryOutMoves( DominoLocation prevMove, int dominoNo )
{
	int y;
/*	
	      printf("Level=%d\n", dominoNo);
			for (y=0; y<yGridSize; y++)
			{
				for (int x=0; x<xGridSize; x++)
					printf("%c", gridCopy[x][y]);

				printf("\n");
			}
			printf("\n");
			
		
			 getchar();
		*/
		if ( dominoNo == 28 )
		{
			/*
			printf("\n");
			for ( y=0; y<yGridSize; y++)
			{
				for (int x=0; x<xGridSize; x++)
					printf("%c", gridCopy[x][y]);

				printf("\n");
			}
			printf("\n");
*/
			SolutionCount++;
			return 1;
		}

	// iterate over possible moves for this domino
	DominoLocationList::iterator move;
	DominoLocation ourPrevMove;
	ourPrevMove.x  = 100; // Dummy value which we check for later

	for (move=AvailableMoves[dominoNo].begin(); move != AvailableMoves[dominoNo].end(); move++)
	{
		bool moveOK = false;
		if ( move->orientHorizontal == true )
		{
			if ( gridCopy[move->x][move->y] == ' ' &&
				 gridCopy[move->x+1][move->y] == ' ' )
			{
				// We haven't yet populated this space on the grid,
				// so move is OK
				moveOK = true;

				// Undo the previous move
				if ( ourPrevMove.x != 100 )
				{
					if (ourPrevMove.orientHorizontal == true)
					{
						gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
						gridCopy[ourPrevMove.x+1][ourPrevMove.y] = ' ';
					}
					else
					{
						gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
						gridCopy[ourPrevMove.x][ourPrevMove.y+1] = ' ';
					}
				}

				// Fill in the grid squares to say they are now taken
				gridCopy[move->x][move->y] = gridCopy[move->x + 1][move->y] = dominoNo+'A';
			}

		}
		else
		{
			if ( gridCopy[move->x][move->y] == ' ' &&
				 gridCopy[move->x][move->y + 1] == ' ' )
			{
				// We haven't yet populated this space on the grid,
				// so move is OK
				moveOK = true;

				// Undo the previous move
				if ( ourPrevMove.x != 100 )
				{
					if (ourPrevMove.orientHorizontal == true)
					{
						gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
						gridCopy[ourPrevMove.x+1][ourPrevMove.y] = ' ';
					}
					else
					{
						gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
						gridCopy[ourPrevMove.x][ourPrevMove.y+1] = ' ';
					}
				}
				// Fill in the grid squares to say they are now taken
				gridCopy[move->x][move->y] = gridCopy[move->x][move->y + 1] = dominoNo+'A';
			}
		}
	
		if ( moveOK == false )
		{
			// This move is no longer available, so no need to try out
			// pieces further down the tree.
			continue;
		}
				
		ourPrevMove = *move;

		tryOutMoves(*move, dominoNo + 1);

	}

	// We've tried all the moves for this domino, so now we need to 
	// undo the previous move that the caller of this function made
	if ( prevMove.x != 100 )
	{
		if (prevMove.orientHorizontal == true)
		{
			gridCopy[prevMove.x][prevMove.y] = ' ';
			gridCopy[prevMove.x+1][prevMove.y] = ' ';
		}
		else
		{
			gridCopy[prevMove.x][prevMove.y] = ' ';
			gridCopy[prevMove.x][prevMove.y+1] = ' ';
		}
	}

	// Also need to undo the last move we made at this level
	if (ourPrevMove.x != 100)
	{
		if (ourPrevMove.orientHorizontal == true)
		{
			gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
			gridCopy[ourPrevMove.x+1][ourPrevMove.y] = ' ';
		}
		else
		{
			gridCopy[ourPrevMove.x][ourPrevMove.y] = ' ';
			gridCopy[ourPrevMove.x][ourPrevMove.y+1] = ' ';
		}
	}

	return 1;

}

// makePuzzle randomly places dominos onto the grid.
// Once it can no longer place pieces randomly, it backtracks
// previous moves and retries the pieces in the opposite
// orientation until it gets all pieces to fit.
//
// Pieces are placed starting at the top left corner, in a
// random orientation. If the orientation does not fit, then the
// piece is tried the other way up. If that orientation also
// does not fit,then the backtrack code is called.
//
// To backtrack, we go back through the undo list and undo
// previous moves in order until we find one that hasn't yet
// had both orientations found. Code then restarts from this position.
//
// If the piece fits, then we place it, then start again at the
// next free grid square to the right. The domino piece number is
// actually an index into an array of shuffled dominoes.
int makePuzzle()
{
	makeDominoSet();
	
	shuffleDominoSet();

	typedef struct MoveData
	{
		int x;
		int y;
		bool orientHorizontal;
		int retryCount;
	} MoveDataType;

	// Erase the grid
	for (int y=0; y<MAX_GRID; y++)
		for (int x=0; x<MAX_GRID; x++)
			Grid[x][y] = ' ';

	// Hard code some test defaults
	xGridSize = 8;
	yGridSize = 7;

	int DomCount = 28; // Should calculate this from maxDomSpot

	// We'll store the moves we need to make to populate the grid in
	// undoList. This is so we can backtrack if we get stuck towards the end
	MoveDataType undoList[30]; // TODO - should use malloc(DomCount * sizeof(MoveDataType)

	for( int m=0; m<sizeof(undoList)/sizeof(MoveDataType); m++ )
		undoList[m].retryCount = 0;

	// For now just assign digits to the grid
	// Later these'll be random dominos
	int nextGridX = 0;
	int nextGridY = 0;
	int i = 0; // Domino number

	while( i<DomCount )
	{
		bool orientHorizontal = true;
		int retryCount = -1;

		// Have we tried this piece before?
		if ( undoList[i].retryCount == 1 )
		{
			// If we have seen it before then we don't want to choose the
			// orientation randomly. Instead we want to try the one we 
			// didn't try before
			orientHorizontal = !undoList[i].orientHorizontal;
			retryCount = undoList[i].retryCount;
		}
		else
		{	
			// Random choice of horiz or vertical orientation
			double r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
			if ( r < 0.5 )
				orientHorizontal = true;
			else
				orientHorizontal = false;

			retryCount = 0;
		}
		
		// See if our piece fits in our chosen orientation, and if not,
		// flip it and try again
		if ( orientHorizontal == true )
		{
			if ( Grid[nextGridX+1][nextGridY] != ' ' || nextGridX+1 >= xGridSize)
			{
				orientHorizontal = !orientHorizontal;
				retryCount++;
			}
		}
		else
		{
			if ( Grid[nextGridX][nextGridY+1] != ' ' || nextGridY+1 >= yGridSize)
			{
				orientHorizontal = !orientHorizontal;
				retryCount++;
			}
		}

		// Re-check. If piece still doesn't fit
		// then we need to back track
		// Yes, this logic could be condensed and backtrackNeeded variable removed
		// but kept like this for now for clarity
		bool backtrackNeeded = false;
		if ( orientHorizontal == true )
		{
			if ( Grid[nextGridX+1][nextGridY] != ' ' || nextGridX+1 >= xGridSize )
			{
				backtrackNeeded = true;
			}
		}
		else
		{
			if ( Grid[nextGridX][nextGridY+1] != ' ' || nextGridY+1 >= yGridSize )
			{
				backtrackNeeded = true;
			}
		}

		if ( backtrackNeeded == true )
		{
			// Need to go back through moves list,
			// undoing moves until we find one where we haven't
			// already tried it both ways round
			int m;
			for (m=i-1; m >=0; m-- )
			{
				// Undo the previous move
				MoveDataType prevMove = undoList[m];
				Grid[prevMove.x][prevMove.y] = ' ';
				if ( prevMove.orientHorizontal == true )
					Grid[prevMove.x+1][prevMove.y] = ' ';
				else
					Grid[prevMove.x][prevMove.y+1] = ' ';

				if ( prevMove.retryCount < 1 )
				{
					// We haven't already tried this piece both ways round,
					// so go back through loop and retry
					undoList[m].retryCount++;
					i = m;
					nextGridX = prevMove.x;
					nextGridY = prevMove.y;
					break;
				}
				else
				{
					// This piece doesn't fit either way, so remove it from the list
					undoList[m].retryCount = 0;
				}
			}

			if ( m < 0 )
			{
				printf("ERROR - ran out of moves to backtrack\n");
				return -1;
			}

			// Don't want to place this piece, so go back to top of loop
			continue;

		}


		// Now place our domino
		// - we pick which is the first spot and which is the second
		// at random. Assume domino set is already shuffled.
		double r = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
		int spotOne;
		int spotTwo;
		if ( r < 0.5 )
		{
			spotOne = DominoSet[i].first;
			spotTwo = DominoSet[i].second;
		}
		else
		{
			spotOne = DominoSet[i].second;
			spotTwo = DominoSet[i].first;
		}


		if ( orientHorizontal == true )
		{
			Grid[nextGridX][nextGridY] = spotOne;
			Grid[nextGridX+1][nextGridY] = spotTwo;
		}
		else
		{
			Grid[nextGridX][nextGridY] = spotOne;
			Grid[nextGridX][nextGridY+1] = spotTwo;
		}

		// Also need to record the move in the undo list
		MoveDataType move;
		move.x = nextGridX;
		move.y = nextGridY;
		move.orientHorizontal = orientHorizontal;
		move.retryCount = retryCount;
		undoList[i] = move;

		// Now find the next free spot in the grid.
		// Working left to right, then top to bottom.
		while ( Grid[nextGridX][nextGridY] != ' ' )
		{
			nextGridX++;
			if (nextGridX == xGridSize)
			{
				nextGridX = 0;
				nextGridY++;
			}
		}

		// Place the next piece
		i++;
	}

	return 0;
}

void showGrid()
{
	// Display the grid
	for (int y=0; y<yGridSize; y++)
	{
		for (int x=0; x<xGridSize; x++)
			printf("%c", Grid[x][y]);

		printf("\n");
	}
}



