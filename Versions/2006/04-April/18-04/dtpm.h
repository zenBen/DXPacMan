/**	
 *	Author: Ben Cowley, 2006.
 *	Header file for the specific Decision Theory Player Modelling functions.
 */

#ifndef _DTPM_H
#define _DTPM_H

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

/**
 *	Function prototypes
 */
void getSpritePos(D3DXVECTOR3 spritePos);
double utility(D3DXVECTOR3 g_PacAction/*var gameState*/);
void probabilityOfState();//int xPacLoc, int yPacLoc);
double planOfActions(int timeHorizon);
void availableActions();

void printToFile();
void print(char* string);
void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar);

/**
 *	Global fields.
 */
int xIndex = 0;
int yIndex = 0;

/**
 *	Decision Theory functions and fields for predicting player utility
 */
void getSpritePos(D3DXVECTOR3 spritePos)
{
	//Obtain x and y pos values from the PacMan sprite pointer
	FLOAT xPosInitial = spritePos.x;
	FLOAT yPosInitial = spritePos.y;

	//Adjust position values to obtain PacMap index values
	xIndex = (int)((xPosInitial - GRIDTLX)/SPRITE_WIDTH);
	yIndex = (int)((yPosInitial - GRIDTLY)/SPRITE_HEIGHT);
}

//Utility of one move.
//We have random ghost movement, so all states are equally probable.
double utility(D3DXVECTOR3 g_PacAction/*var gameState*/)
{
	double util = 0.0;
	int totalPills = NumPills;
	int ghostIndices[4][2];
	getSpritePos(g_ShadowSpritePos);
	ghostIndices[0][0] = xIndex; ghostIndices[0][1] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndices[1][0] = xIndex; ghostIndices[1][1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndices[2][0] = xIndex; ghostIndices[2][1] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndices[3][0] = xIndex; ghostIndices[3][1] = yIndex;

	getSpritePos(g_PacAction);

	for(int i = 0; i < 4; i++)
	{
		if(xIndex == ghostIndices[i][0] && yIndex == ghostIndices[i][1])
		{		
			util -= 500;
			break;
		}
		if(GameLives > 5)
		{
			//Ghost will be within 2 moves - 0-1x, 0-1y
			if(abs(ghostIndices[i][0] - xIndex) < 2 && abs(ghostIndices[i][1] - yIndex) < 2)
			{
				util -= 100;
				break;
			}
		}
	}

	if(PacMap[xIndex][yIndex] > 2) //If we're not moving into a wall
	{
		if (PacMap[xIndex][yIndex] == 4)
		{
			util += 5;
			totalPills--;
		}
		else if (PacMap[xIndex][yIndex] == 5)
		{
			GhostAttack = false; // ghosts now run away - or at least need to be programmed to

			util += 50;
			totalPills--;
		}
		else if (PacMap[xIndex][yIndex] == 3) 	
			//No immediate utility but teleporting may the precursor to some plan
			util += 0;
	}else{
		util -= 2; //negative utility for pointlessly running into walls
	}

	return util;
}

void probabilityOfState()//int xPacLoc, int yPacLoc)
{
	//64 possible states for any distinct PacAction
	int GameStates[20][20][256];
	float fProbs[256];//[4][4][4];
	char out[25];

	for(int z = 0; z < 256; z++)
	{
		fProbs[z] = 1;
		for(int x = 0; x < 20; x++)
		{
			for(int y = 0; y < 20; y++)
			{
				GameStates[x][y][z] = PacMap[x][y];
			}
		}
	}

	int ghostIndices[4][2];
	getSpritePos(g_ShadowSpritePos);
	ghostIndices[0][0] = xIndex; ghostIndices[0][1] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndices[1][0] = xIndex; ghostIndices[1][1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndices[2][0] = xIndex; ghostIndices[2][1] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndices[3][0] = xIndex; ghostIndices[3][1] = yIndex;

	//Iterate each state - use stSp as a count index
	int stSp = 0, stSp2 = 0, stSp3 = 0;
	// Ghost 1
	for(int g1 = -3; g1 < 4; g1 += 2)
	{
		if( abs(g1) > 1 )
		{
			if( PacMap[ghostIndices[0][0]][ghostIndices[0][1] + (g1/3)] == 1 )
			{
				fProbs[stSp] += 1; 
				//We want to also set the GameState index to show the ghost position, so invert the int
				GameStates[ghostIndices[0][0]][ghostIndices[0][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[0][0]][ghostIndices[0][1] + (g1/3)][stSp] *= -1;
		}else{
			if( PacMap[ghostIndices[0][0] + g1][ghostIndices[0][1]] == 1 )
			{
				fProbs[stSp] += 1;
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[0][0]][ghostIndices[0][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[0][0] + g1][ghostIndices[0][1]][stSp] *= -1;
		}
		stSp++;
//	}

	// Ghost 2
	for(int g2 = -3; g2 < 4; g2 += 2)
	{
		if( abs(g2) > 1 )
		{
			if( PacMap[ghostIndices[1][0]][ghostIndices[1][1] + (g2/3)] == 1 )
			{
				fProbs[stSp] += 1; 
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[1][0]][ghostIndices[1][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[1][0]][ghostIndices[1][1] + (g2/3)][stSp] *= -1;
		}else{
			if( PacMap[ghostIndices[1][0] + g2][ghostIndices[1][1]] == 1 )
			{
				fProbs[stSp] += 1;
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[1][0]][ghostIndices[1][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[1][0] + g2][ghostIndices[1][1]][stSp] *= -1;
		}
		stSp++;
//	}

	// Ghost 3
	for(int g3 = -3; g3 < 4; g3 += 2)
	{
		if( abs(g3) > 1 )
		{
			if( PacMap[ghostIndices[2][0]][ghostIndices[2][1] + (g3/3)] == 1 )
			{
				fProbs[stSp] += 1; 
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[2][0]][ghostIndices[2][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[2][0]][ghostIndices[2][1] + (g3/3)][stSp] *= -1;
		}else{
			if( PacMap[ghostIndices[2][0] + g3][ghostIndices[2][1]] == 1 )
			{
				fProbs[stSp] += 1;
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[2][0]][ghostIndices[2][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[2][0] + g3][ghostIndices[2][1]][stSp] *= -1;
		}
		stSp++;
//	}

	// Ghost 4
	for(int g4 = -3; g4 < 4; g4 += 2)
	{
		if( abs(g4) > 1 )
		{
			if( PacMap[ghostIndices[3][0]][ghostIndices[3][1] + (g4/3)] == 1 )
			{
				fProbs[stSp] += 1; 
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[3][0]][ghostIndices[3][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[3][0]][ghostIndices[3][1] + (g4/3)][stSp] *= -1;
		}else{
			if( PacMap[ghostIndices[3][0] + g4][ghostIndices[3][1]] == 1 )
			{
				fProbs[stSp] += 1;
				//We want to also set the GameState index to show the ghost position
				GameStates[ghostIndices[3][0]][ghostIndices[3][1]][stSp] *= -1;
			}else
				//We want to set the GameState index to show the moved ghost position
				GameStates[ghostIndices[3][0] + g4][ghostIndices[3][1]][stSp] *= -1;
		}
		stSp++;
	}
	}}}

	print( "Log File: \n");

	for(int p = 0; p < 256; p++)
	{
		fProbs[p] /= 256;
		sprintf(out, "%G", fProbs[p]);
		print(out); 
		if( (p+1) % 4 == 0 ) print("\n");
		else print("  \t");
	}
	//If state i has total freedom, fProbs[i] = 1/64
	//If state i has 1 restriction, fProbs[i] = 2/64
	//If state i has 2restrictions, fProbs[i] = 4/64...?
}

double planOfActions(int timeHorizon)
{
	double utilityOfPlan = 0.0;
	
	//Obtain the states available after #timeHorizon actions
	while( timeHorizon > 0 )
	{
		timeHorizon--;
		//currentState->addChildren(possibleStates);
		//currentState = currentState.mostUtileChild;
	}

	//Apply the Decision Theory formula
//	utilityOfPlan = Sum of : (the utilities of states possible after this action)x(the probabilities of those states occuring)

	return utilityOfPlan;
}

void availableActions()
{
	getSpritePos(g_PacmanSpritePos);

	//Checking: Up, Down, Right, Left / North, South, East, West / y-1, y+1, x+1, x-1 / 0, 1, 2, 3
	int moveCount[5];
	if(PacMap[xIndex][yIndex-1] > 2)
		moveCount[0] = 1;
	if(PacMap[xIndex][yIndex+1] > 2)
		moveCount[1] = 1;
	if(PacMap[xIndex+1][yIndex] > 2)
		moveCount[2] = 1;
	if(PacMap[xIndex-1][yIndex] > 2)
        moveCount[3] = 1;

	moveCount[4] = moveCount[0] + moveCount[1] + moveCount[2] + moveCount[3];

	//Either iterate through first moves in available directions to find best utility, or
	//Project to end of plans(seq of moves), heuristic the ghosts and pick best utility (more human-natural)

}

/**
 *	Logging functions and fields for outputting debug info (not exactly Stealth!)
 */
FILE *fileout = fopen("evalData/printout.txt", "wt");	//Open an output File object

void logPos(D3DXVECTOR3 spritePos, char *name, char *endChar)
{
	getSpritePos(spritePos);

	char numBin[25];
	print(name);
	if(xIndex < 10) print(" ");
	print(itoa(xIndex, numBin, 10));
	if(yIndex < 10) print(",  ");
	else print(", ");
	print(itoa(yIndex, numBin, 10));
	print(endChar);
}

//Flush and close the output File
void printToFile()
{
	fflush(fileout);
	fclose(fileout);
}

//Puts its string argument to a global output stream.
void print(char* string)
{
	fputs(string, fileout);
}

#endif