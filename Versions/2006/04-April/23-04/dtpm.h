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
	//256 possible states given any distinct PacAction
	int sttSpc = 0;
	//By recording '0123 == NSEW' in each indice, we can record the move of each ghost from 1 - 4
	int GhostMove[4][2][256];
	int ghostIndices[4][2];
	float fProbs[4][4][4][4];
	char out[25];

	//All states have equal prob in a random distribution
	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					fProbs[p1][p2][p3][p4] = 1;
				}
			}
		}
	}

	getSpritePos(g_ShadowSpritePos);
	ghostIndices[0][0] = xIndex; ghostIndices[0][1] = yIndex;
	getSpritePos(g_SpeedySpritePos);
	ghostIndices[1][0] = xIndex; ghostIndices[1][1] = yIndex;
	getSpritePos(g_PokeySpritePos);
	ghostIndices[2][0] = xIndex; ghostIndices[2][1] = yIndex;
	getSpritePos(g_BashfulSpritePos);
	ghostIndices[3][0] = xIndex; ghostIndices[3][1] = yIndex;

	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 2; p2++)
		{
			for(int p3 = 0; p3 < 256; p3++)
			{
				GhostMove[p1][p2][p3] = ghostIndices[p1][p2];
			}
		}
	}

	int count = -1;
	for(int g1 = 0; g1 < 4; g1++)
	{
	for(int g2 = 0; g2 < 4; g2++)
	{
	for(int g3 = 0; g3 < 4; g3++)
	{
	for(int g4 = 0; g4 < 4; g4++)
	{
		count++;
		switch(g1)
		{
		case 0:
			if( PacMap[ghostIndices[0][0]][ghostIndices[0][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[0][0]][ghostIndices[0][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[0][0] + 1 ][ghostIndices[0][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[0][0] - 1 ][ghostIndices[0][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[0][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 2nd Ghost
		switch(g2)
		{
		case 0:
			if( PacMap[ghostIndices[1][0]][ghostIndices[1][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[1][0]][ghostIndices[1][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[1][0] + 1 ][ghostIndices[1][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[1][0] - 1 ][ghostIndices[1][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[1][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 3rd Ghost
		switch(g3)
		{
		case 0:
			if( PacMap[ghostIndices[2][0]][ghostIndices[2][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[2][0]][ghostIndices[2][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[2][0] + 1 ][ghostIndices[2][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[2][0] - 1 ][ghostIndices[2][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[2][0][count]--;
			break;
		default:
			break;
		}
		//Check all around 4th Ghost
		switch(g4)
		{
		case 0:
			if( PacMap[ghostIndices[3][0]][ghostIndices[3][1] - 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]--;
			break;
		case 1:
			if( PacMap[ghostIndices[3][0]][ghostIndices[3][1] + 1 ] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][1][count]++;
			break;
		case 2:
			if( PacMap[ghostIndices[3][0] + 1 ][ghostIndices[3][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][0][count]++;
			break;
		case 3:
			if( PacMap[ghostIndices[3][0] - 1 ][ghostIndices[3][1]] == 1 )
				fProbs[g1][g2][g3][g4] = 0;
			else
				GhostMove[3][0][count]--;
			break;
		default:
			break;
		}
	}
	}}}

	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					if( fProbs[p1][p2][p3][p4] == 1 )
						sttSpc++;
				}
			}
		}
	}

	print("\n#P(S):"); itoa(sttSpc, out, 10); print(out);
	print("\t3:0, 4:0\t\t\t\t3:0, 4:1\t\t\t\t3:0, 4:2\t\t\t\t3:0, 4:3\t\t\t\t3:1, 4:0\t\t\t\t3:1, 4:1\t\t\t\t3:1, 4:2\t\t\t\t3:1, 4:3\t\t\t\t3:2, 4:0\t\t\t\t3:2, 4:1\t\t\t\t3:2, 4:2\t\t\t\t3:2, 4:3\t\t\t\t3:3, 4:0\t\t\t\t3:3, 4:1\t\t\t\t3:3, 4:2\t\t\t\t3:3, 4:3\n");
	print("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	count = -1;
	for(int p1 = 0; p1 < 4; p1++)
	{
		for(int p2 = 0; p2 < 4; p2++)
		{
			print("\n0:"); itoa(p1, out, 10); print(out);
			print(", 1:"); itoa(p2, out, 10); print(out);
			print("|\t");
			for(int p3 = 0; p3 < 4; p3++)
			{
				for(int p4 = 0; p4 < 4; p4++)
				{
					count++;
					fProbs[p1][p2][p3][p4] /= sttSpc;
					if(fProbs[p1][p2][p3][p4] == 0)
						print("0\t\t\t\t\t");
					else
					{
						for(int g2 = 0; g2 < 4; g2++)
						{
							print("G"); itoa(g2, out, 10); print(out); print(":");
							for(int g3 = 0; g3 < 2; g3++)
							{
								itoa(GhostMove[g2][g3][count], out, 10); 
								if(GhostMove[g2][g3][count] < 10) print(" ");
								print(out); print(",");
							}
						}
						print("\t");
					}
				}
			}
		}
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
	int moveCount[5] = {0, 0, 0, 0, 0};
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