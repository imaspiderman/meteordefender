#ifndef __game_H
#define __game_H

#ifndef __G3d_H
#include "G3d.h"
#endif

typedef struct node{
	u8 type;
	u8 health;
	u8 currentHealth;
	u8 animFrame;//current animation frame
	u8 animSpeed;//speed of animation. use 0 for still frame.
	u8 animTotalFrames;//Total frames of animation
	object* anim[10];//Pointers to objects used for animation
} node;

typedef struct event{
	u32 frameTime;//timer in game frames to call event
	void (*eventFunc);//function pointer
} event;

const s32 city[]={
    63,//total vertices
	34,//total indexes
	2,//face size
	//vertices
	-1000,0,0, -1000,200,0,
	-800,200,0, -800,0,0,
	-800,500,0, -400,500,0,
	-400,0,0, -500,500,0,
	-500,600,0, -200,600,0,
	-200,0,0, -300,600,0,
	-300,700,0, 0,700,0,
	0,0,0, 0,500,0,
	300,500,0, 300,0,0,
	300,700,0, 1000,700,0,
	1000,0,0,
	//indexes
	0,1, 1,2, 2,3, 3,4,
	4,5, 5,6, 7,8, 8,9,
	9,10, 11,12, 12,13,	13,14, 
	15,16, 16,17, 16,18, 18,19, 
	19,20
};

const s32 ground[]={
	24,//total size of vertices
	12,//total indexes
	2,//face size
	//vertices
	-6000,0,0, -6000,0,8000,
	-2000,0,0, -2000,0,8000,
	2000,0,0, 2000,0,8000,
	6000,0,0, 6000,0,8000,
	//indexes
	0,1, 0,6, 6,7, 7,1,
	2,3, 4,5
};

const s32 meteor[]={
	18,//total size of vertices
	24,//total indexes
	2,//face size
	//vertices
	-200,0,-200, -200,0,200,
	200,0,200, 200,0,-200,
	0,200,0, 0,-200,0,
	//indexes
	0,1, 1,2, 2,3, 3,0,
	0,4, 4,2, 2,5, 5,0,
	1,4, 4,3, 3,5, 5,1
};

void vbInit();
void screenControl();
void initCam();
void handleInput();
void initObjects();
void inline drawCrossHairs(vector3d*);
void inline calculateSpeed(object* o);
void timeHnd(void);
void addNode(object*,u8,u8);
void getHitSquare(vector3d*,vector3d*);
u8 detectHit(vector3d*,vector3d*,vector3d*,vector3d*);
#endif