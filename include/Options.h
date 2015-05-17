/**
 * All the options here.
 *
 * @author Zhengrong Wang
 */

#ifndef OPTIONS_HEADER
#define OPTIONS_HEADER

#include "GlobalHeader.h"
#include "Geometry.h"

// Store the options.
struct Options {

	/*****************************************************
	* Parameters for detecors.
	*****************************************************/
	feat scaleMin;
	feat scaleMax;
	feat scaleStep;
	feat binaryThre;
	int slideStep;
	int evidence;		// Number of neighboring detection to make sure.
	int modelHeight;
	int modelWidth;
	float invPerimeterRatio;	// Used in connected components analysis.
	float minAreaRatio;
	float maxAreaRatio;

	/*****************************************************
	* Parameters for particle fitler.
	*****************************************************/
	int nParticles;
	Rect target;
	Point2D initVelocity;

	/*****************************************************
	* Parameters for strong classifiers.
	*****************************************************/
	int numSelectors;
	int numWeakClassifiers;
	int numBackups;

	/*****************************************************
	* Parameteres for match score.
	*****************************************************/
	float distWeight;
	float velocityThre;
	float velocitySigmaConst;
	float matchThre;

	/****************************************************
	* Parameters for targets list.
	****************************************************/
	int targetsFreeListCapacity;		// capacity for targets free list.
	float detectionWeight;				//
};

#endif