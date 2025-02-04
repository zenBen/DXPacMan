/**
 *	Author: Ben Cowley, 2008.
 *	Decision Theory Player Modelling functions for predicting player utility
 */
#include "main.h"
#include "dtpm.h"

//New Utility functions based on most important derived features from rule ensemble
float utility_A4_HuntAfterPill( deque<PacState> arg );
float utility_Agg1_LivesSDev( deque<PacState> arg );

//Legacy utility calculation functions - must replace with scaled down versions of the feature derivation functions.
double utilityDots(PacState pS);
double utilityPills(PacState pS, int risk);
double utilityGhostHunt(PacState pS, int risk);
double utilityGhosts(PacState pS, int risk);

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////             UTILITY FUNCTIONS                                              /////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Description: Utility of a state is a measure of the best possible end result that can be obtained from that state - i.e. we assume an optimally played game
 * Arguments:	parameters of the utility calculation formulae - when the formulae are ready, this will supply parameters for each in order of occurence of use.
 */
double utility( deque <PacState> States )
{
	//Weight a feature set polynomial or something similar
	double utility = 0.0;
	float util = 0.0;
	int s = States.size();
	PacState pS;
/**/
	util = utility_A4_HuntAfterPill( States );
	utility += util;
	sprintf_s( out, 99, "%f", util );
	print( "HuntAfter:," ); print( out );

	util = utility_Agg1_LivesSDev( States );
	utility += util;
	sprintf_s( out, 99, "%f", util );
	print( "LivesSDev:," ); print( out ); print( "," );
/**/
	for( int i = 0; i < s; i++ )
	{
		pS = States[i];

		sprintf_s(out, 99, "%d", pS.State);
		print( out ); print( "," );
		sprintf_s(out, 99, "%d", pS.getProb());
		print( out ); print( "," );
	}
print( "\n" );
/*	Here we call the various functions to determine utility	*/
//	utility = utilityA4( States ) + utilityOtherFeature( States ) + etc

	return utility;
}

/**
 *	Description	: Derivation of Clementine code which counts the number of times this condition is met:
 *	((@DIFF1(dst_PacG1) <= 0 and dst_PacG1 < 5) or 
 *	(@DIFF1(dst_PacG2) <= 0 and dst_PacG2 < 5)) and 
 *	(@SINCE0(PacAtkBool = 1,20) < 10) and 
 *	PacAtkBool = 0
 */
float utility_A4_HuntAfterPill( deque<PacState> arg )
{
	float output = -1.0;
	PacState pS;
	int s = arg.size(), S = actual_States.size();

	if( S > 10 )
	{
		output++;
		for( int i = 0; i < s; i++ )
		{
			pS = arg[i];
			while( pS.PacAttack == false && actual_States[S-10+i].PacAttack == true && i < s-1)
			{				
				int d0 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
				int d1 = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );
			
				pS = arg[++i];

				int d0next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[0], pS.yGhost[0] );
				int d1next = manhattanDist( pS.xPac, pS.yPac, pS.xGhost[1], pS.yGhost[1] );			
				
				if( (d0next < 5 &&  d0next < d0) || (d1next < 5 &&  d1next < d1) )
				{
					output++;
				}
			}
		}
	}
	return output;
}

/**
 *	Description: Standard deviation of the player�s Lives over the period of arg
 */
float utility_Agg1_LivesSDev( deque<PacState> arg )
{
	float output = 0.0;
	float s = arg.size();
	float avgLives = 0.0;

	for( int i = 0; i < s; i++ )
	{
		avgLives += (float)arg[i].Lives;
	}
	avgLives /= s;

	for( int i = 0; i < s; i++ )
	{
		output += (float)((arg[i].Lives - avgLives) * (arg[i].Lives - avgLives));
	}
	if( output > 0 )
		output = sqrtf(output / (s-1));

	return output;
}

/*****************************************************************************************/
/*	OLDER UTILITY FUNCTIONS WHICH CONSIDER A COMPLETE SINGLE STATE, NOT MULTIPLE STATES	 */
/*****************************************************************************************/

/**
 * Description: Utility of Dots is number and density of clusters, plus proximity to Pacman.
 * Arguments:	PacState to evaluate
 */
double utilityDots( PacState pS )
{
	double util = 0.0;
	//Calculate the utility of each dot left on the map.
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++)
			if(pS.dtpMap[i][j] == 4)
			{
				util += 9.0;
				//Distribution density gives utility - singular dots are less utile
				double inc = (double)localDots(i, j, 1, pS.dtpMap);
				util += inc;
				if(inc > 1.0)
					util += 0.33 * (localDots(i, j, 2, pS.dtpMap) - inc);
				//Distance from Pacman reduces utility - close dots are easier to eat, less long-term risk.
				util -= 1.0 / myHeuristicDist(pS.xPac, pS.yPac, i, j);
			}

	return util;
}

/**
 * Description: Utility of Ghosts is proximity of Ghosts to Pacman, and how threatening their deployment is
 *				Here we have to account to a degree for the ability of humans to lookahead, to predict. Utility is not just the current state, but the relationship of current state to future states.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityGhosts(PacState pS, int risk)
{
	double util = 0.0;

	//Calculate if Pacman is trapped by ghosts, and how many exits he has if not
//	int locBound = 4;	int locale = localGhosts(xPac, yPac, locBound);		if( locale > 1 ){}

	//The average distance of the ghosts is good if large
	int dist = 0;
	for(int i = 0; i < NUM_GS; i++)
		dist += myHeuristicDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]);
	util += 100.0 - (dist / NUM_GS);

	//The distribution size of the ghosts is good if small.
	util += 100.0 / avgDistribution(pS.xGhost, pS.yGhost);

	// The distance by which ghosts increase an A* path from Pacman to his inverse x,y indicates bad utility
	int temp = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
	int tempi = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		pS.dtpMap[pS.xGhost[i]][pS.yGhost[i]] = 1;
		tempi = a_Star(pS.xPac, pS.yPac, 20-pS.xPac, 20-pS.yPac, true, pS);
		util += (temp - tempi) * 10.0;
		temp = tempi;
	}

//	if( a_Star(xPac, yPac, 20-xPac, 20-yPac, true) < locBound )		return -100.0; //no way out! Check this
	
	return util;
}

/**
 * Description: Utility of Pills is proximity of one Pill to Pacman, then utilityGhostHunt(). This function only called if !PacAttack.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityPills(PacState pS, int risk)
{
	double util = 0.0;

	if(pS.Pills == 0)
		return 0;
		
	util = pS.Pills * 50;
	deque <int> mDist; //Deque of distances between things
	int aS = 0;

	deque <int> xPill;
	deque <int> yPill;
	//Midpoint between Pacman and the centroid of all the Ghosts
	int xCentroid = 0, yCentroid = 0;
	for(int i = 0; i < NUM_GS; i++)
	{
		xCentroid += pS.xGhost[i];
		yCentroid += pS.yGhost[i];
	}
	xCentroid = ((xCentroid / NUM_GS) + pS.xPac) / 2;
	yCentroid = ((yCentroid / NUM_GS) + pS.yPac) / 2;
	
	//Get the closest pill's x and y
	if(pS.dtpMap[1][3] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 1, 3) );
		xPill.push_back(1);
		yPill.push_back(3);
	}
	if(pS.dtpMap[18][3] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 18, 3) );
		xPill.push_back(18);
		yPill.push_back(3);
	}
	if(pS.dtpMap[1][15] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 1, 15) );
		xPill.push_back(1);
		yPill.push_back(15);
	}
	if(pS.dtpMap[18][15] == 5){
		mDist.push_back( myHeuristicDist(xCentroid, yCentroid, 18, 15) );
		xPill.push_back(18);
		yPill.push_back(15);
	}
	
	//Note the index of the closest Pill and derive utility for it by distance
	mDist.push_front(lowest(mDist));

	aS = a_Star(pS.xPac, pS.yPac, xPill[mDist[0]], yPill[mDist[0]], false, pS);
	if( aS == 0 )
		aS++;

	//Utility is heavily influenced by initial proximity of Pacman to Pill - this is a counter-weight
	util -= 100.0 * (double)aS;

	// Reset xPac yPac vars to the value's of the closest Pill, from whence Pacman will launch his attack.
	pS.xPac = xPill[mDist[0]];
	pS.yPac = yPill[mDist[0]];

	util += utilityGhostHunt(pS, risk);

	return util;
}

/**
 * Description: Utility of Ghost Hunt is the chained proximity of Ghosts to Pacman.
 * Arguments:	PacState to evaluate
 *				risk : a measure of how vulnerable Pacman is (# lives)
 */
double utilityGhostHunt(PacState pS, int risk)
{
	double util = 0.0;
	deque <int> mDist; //Deque of distances between things
	deque <int> chain; //Deque of indexes to the ghost array
	double inc = 0.0;
	int aS = 0;

	//Start with closest ghost to Pacman...
	for(int i = 0; i < NUM_GS; i++)
		mDist.push_back( manhattanDist(pS.xPac, pS.yPac, pS.xGhost[i], pS.yGhost[i]) );
	//The index of the lowest ghost manhattan distance measure to Pacman
	chain.push_back(lowest(mDist));

	//...then the closest to the closest ghost (as we assume Pacman moved there to eat him), etc.
	for(int i = 0; i < NUM_GS-1; i++)
	{
		mDist.clear();
		for(int j = 0; j < NUM_GS; j++)
		{
			bool jump = false;
			for(unsigned int k = 0; k < chain.size(); k++)
				if(j == chain[k])
					jump = true;
			if(!jump)
				mDist.push_back( manhattanDist(pS.xGhost[chain[i]], pS.yGhost[chain[i]], pS.xGhost[j], pS.yGhost[j]) );
			else
				mDist.push_back( 99 );
		}
		chain.push_back(lowest(mDist));
	}

	//Now get the distance to the closest ghost
	aS = a_Star(pS.xPac, pS.yPac, pS.xGhost[chain[0]], pS.yGhost[chain[0]], false, pS);
	if( aS == 0 )
		aS++;
	//Utility of catching first ghost is relative to the points obtained
	inc = 100 / (double)aS;
	util += inc;

	//Utility of catching later ghosts is more heavily penalised by distance and risk, but grows expontentially, as with the points scored
	for(int i = 1; i < chain.size(); i++)
	{
		aS = a_Star(pS.xGhost[chain[i-1]], pS.yGhost[chain[i-1]], pS.xGhost[chain[i]], pS.yGhost[chain[i]], false, pS);
		if( aS == 0 )
			aS++;
		inc = (pow(2.0, i) * 100.0) / ((double)aS * (i + risk));
		util += inc;
	}
	return util;
}

//End of file