/**
 *	Author: Ben Cowley, 2008.
 *	Header for Deriver.cpp;
 */
#ifndef INC_DERIVER_H
#define INC_DERIVER_H

//Feature derivation functions - AGGREGATE
float Aggregate1_LivesSDev( deque<PacState> arg );
float Aggregate2_ChryBoolSum( deque<PacState> arg );
float Aggregate3_CycleCountPerStateSDev( deque<PacState> arg );

//Feature derivation functions - SIMPLE
deque<float> Simple1_Levels( deque<PacState> arg );
deque<float> Simple2_LivesChng( deque<PacState> arg );
float Simple4_TeleportUse( deque<PacState> arg );

//Feature derivation functions - AGGRESSION
float Aggression4_HuntAfterPill( deque<PacState> arg );
float Aggression6_ChaseŽDots( deque<PacState> arg );

//Feature derivation functions - PLANNING
deque<float> Planning1_LureToPill( deque<PacState> arg );
float Planning7_PutOffGstHse( deque<PacState> arg );

//Feature derivation functions - CAUTION
deque<float> Caution1_ThreatPrcptn( deque<PacState> arg );
float Caution4_CaughtAfterHunt( deque<PacState> arg );
float Caution5_MovesWithoutGain( deque<PacState> arg );
float Caution7_KilledAtHouse( deque<PacState> arg );

//Feature derivation functions - SPEED


//Feature derivation functions - DECISIVENESS
float Decisive2_Osclltng( deque<PacState> arg );

//Feature derivation functions - RESOURCE
deque<float> Resource_TimeAndNumChry( deque<PacState> arg );

//Feature derivation functions - THOROUGHNESS
float Thorough2_Hoover( deque<PacState> arg );

//Feature derivation functions - CONTROL SKILL


#endif /* INC_DERIVER_H */