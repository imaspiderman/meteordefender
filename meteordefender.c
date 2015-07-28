#include <libgccvb.h>
#include "G3d.h"
#include "Font.h"
#include "Game.h"

volatile u32 tick;
vector3d crossH;//cross hair vectors
u8 accelerator;
u8 frameSkip = 0;
u8 frameSkipCount = 0;

#define CAM_MAX_SPEED 100
#define CAM_ACCELERATION 10
#define MAX_GAME_BOUNDS 6000
s32 meteorSpeed;

#define MAX_GAME_NODES 20
u8 gameNodeIdx = 0;
node gameNodes[MAX_GAME_NODES];
#define T_NODE_METEOR 1

#define SOUND_TIMER 8
#define FPS_THROTTLE (1000000/20/SOUND_TIMER/20)

#include "G3d.c"

object m1,m2,m3,m4,m5,m6;
u8 fire = 0;

int main(){
	u8 run,hit;
	vector3d vLowLeft1, vUpRight1, vLowLeft2, vUpRight2;

	crossH.sx = (SCREEN_WIDTH>>1);
	crossH.sy = (SCREEN_HEIGHT>>1);

	run = 1;
	accelerator = 0;
	meteorSpeed = 200;

	vbInit();
	initCam();
	initObjects();

	while(run){
		u8 i;
		tick = 0;
		accelerator += 1;
		accelerator &= 0x03;
		fire = 0;

		handleInput();

		vLowLeft2.sx = crossH.sx-3;
		vLowLeft2.sy = crossH.sy+3;
		vUpRight2.sx = crossH.sx+3;
		vUpRight2.sy = crossH.sy-3;

		//This will loop through all the nodes in the game
		for(i=0; i<gameNodeIdx; i++){
			if(gameNodes[i].health == 0) continue;

			//process the nodes based on type
			if(gameNodes[i].type == T_NODE_METEOR){
				//if((accelerator & 0x03) == 3){
				//	calculateSpeed(gameNodes[i].anim[gameNodes[i].animFrame]);
				//}
				g3d_moveObject(gameNodes[i].anim[gameNodes[i].animFrame]);
				if(!frameSkip) g3d_drawObject(gameNodes[i].anim[gameNodes[i].animFrame]);
				getHitSquare(gameNodes[i].anim[gameNodes[i].animFrame],&vLowLeft1,&vUpRight1);
				detectHit(&vLowLeft1,&vUpRight1,&vLowLeft2,&vUpRight2, &hit);

				if(fire == 1 && hit == 1){
					gameNodes[i].health = 0;
				}
			}

			//handle animation frames
			if((gameNodes[i].animFrame + 1) < gameNodes[i].animTotalFrames){
				gameNodes[i].animFrame++;
			}else{
				gameNodes[i].animFrame = 0;
			}
			
			/********************************
			Perform some simple frame skipping
			*********************************/
			if(tick > FPS_THROTTLE){
				frameSkip = 1;
				frameSkipCount++;
			}
			else{
				frameSkip = 0;
				frameSkipCount = 0;
			}
		}
		//Cross hairs are drawn directly to the screen so they
		//are not processed as a game node.
		drawCrossHairs(&crossH);		

		//Throttle the frame rate
		while(tick < FPS_THROTTLE);
		if(tick <= FPS_THROTTLE)tick = 0;
		else tick -= FPS_THROTTLE;

		//Flips the frame buffers
		screenControl();
	}

	return 0;
}

void initObjects(){

	g3d_initObject(&m1,(objectData*)meteor);
	g3d_initObject(&m2,(objectData*)meteor);
	g3d_initObject(&m3,(objectData*)meteor);
	g3d_initObject(&m4,(objectData*)meteor);
	g3d_initObject(&m5,(objectData*)meteor);
	g3d_initObject(&m6,(objectData*)meteor);

	addNode(&m1,3,T_NODE_METEOR);
	addNode(&m2,3,T_NODE_METEOR);
	addNode(&m3,3,T_NODE_METEOR);
	addNode(&m4,3,T_NODE_METEOR);
	addNode(&m5,3,T_NODE_METEOR);
	addNode(&m6,3,T_NODE_METEOR);

	//init object positions / speeds etc..
	cam.worldPosition.y = F_NUM_UP(80);

	m1.worldPosition.z = F_NUM_UP(7000);
	m1.worldPosition.x = -6000;
	m1.worldPosition.y = F_NUM_UP(2500);
	m1.worldSpeed.z = meteorSpeed;
	m1.worldScale.x = F_NUM_UP(2);
	m1.worldScale.y = F_NUM_UP(2);
	m1.worldScale.y = F_NUM_UP(2);
	g3d_copyVector3d(&m1.worldPosition,&m1.moveTo);

	m2.worldPosition.z = F_NUM_UP(7000);
	m2.worldPosition.x = 6000;
	m2.worldPosition.y = F_NUM_UP(3000);
	m2.worldSpeed.z = meteorSpeed;
	m2.rotation.x = 4;
	m2.rotation.z = 4;
	m2.worldScale.x = F_NUM_UP(1);
	m2.worldScale.y = F_NUM_UP(1);
	m2.worldScale.z = F_NUM_UP(1);
	g3d_copyVector3d(&m2.worldPosition,&m2.moveTo);

	m3.worldPosition.z = F_NUM_UP(7000);
	m3.worldPosition.x = 5000;
	m3.worldPosition.y = F_NUM_UP(3500);
	m3.worldSpeed.z = meteorSpeed;
	m3.rotation.x = 16;
	m3.rotation.z = 16;
	m3.worldScale.x = F_NUM_UP(1);
	m3.worldScale.y = F_NUM_UP(1);
	m3.worldScale.z = F_NUM_UP(1);
	g3d_copyVector3d(&m3.worldPosition,&m3.moveTo);

	m4.worldPosition.z = F_NUM_UP(7000);
	m4.worldPosition.x = -5000;
	m4.worldPosition.y = F_NUM_UP(3500);
	m4.worldSpeed.z = meteorSpeed;
	m4.rotation.x = 16;
	m4.rotation.z = 16;
	m4.worldScale.x = F_NUM_UP(1);
	m4.worldScale.y = F_NUM_UP(1);
	m4.worldScale.z = F_NUM_UP(1);
	g3d_copyVector3d(&m4.worldPosition,&m4.moveTo);

	m5.worldPosition.z = F_NUM_UP(7000);
	m5.worldPosition.x = 3000;
	m5.worldPosition.y = F_NUM_UP(3000);
	m5.worldSpeed.z = meteorSpeed;
	m5.rotation.x = 16;
	m5.rotation.z = 4;
	m5.worldScale.x = F_NUM_UP(2);
	m5.worldScale.y = F_NUM_UP(2);
	m5.worldScale.y = F_NUM_UP(2);
	g3d_copyVector3d(&m5.worldPosition,&m5.moveTo);

	m6.worldPosition.z = F_NUM_UP(7000);
	m6.worldPosition.x = -3000;
	m6.worldPosition.y = F_NUM_UP(3500);
	m6.worldSpeed.z = meteorSpeed;
	m6.rotation.x = 4;
	m6.rotation.z = 4;
	m6.worldScale.x = F_NUM_UP(1);
	m6.worldScale.y = F_NUM_UP(1);
	m6.worldScale.z = F_NUM_UP(1);
	g3d_copyVector3d(&m6.worldPosition,&m6.moveTo);

}

/**************************************
Calculates the speed to send the meteor
in a straight(ish) line
**************************************/
void inline calculateSpeed(object* o){
	s32 z;
	z = o->worldPosition.z / o->worldSpeed.z;
	if(z==0) z = 1;
	o->worldSpeed.x = (o->moveTo.x - o->worldPosition.x)/z;
	o->worldSpeed.y = (o->moveTo.y - o->worldPosition.y)/z;
	if(o->worldSpeed.x < 0) o->worldSpeed.x = (~o->worldSpeed.x)+1;
	if(o->worldSpeed.y < 0) o->worldSpeed.y = (~o->worldSpeed.y)+1;
}

/**************************************
Handle the input for the game
**************************************/
void handleInput(){
	u32 buttons;
	buttons = vbReadPad();
	if(K_A & buttons) fire = 1;
	if(K_LD & buttons){//Move camera down (inverted y axis)
		if(accelerator == 0) cam.worldSpeed.y += F_NUM_UP((cam.worldSpeed.y < 0)?(CAM_ACCELERATION):(CAM_ACCELERATION >> 1));
	}
	if(K_LU & buttons){//Move camera up (inverted y axis)
		if(accelerator == 0) cam.worldSpeed.y -= F_NUM_UP((cam.worldSpeed.y > 0)?(CAM_ACCELERATION):(CAM_ACCELERATION >> 1));
	}
	if(K_LL & buttons){//Move camera left
		if(accelerator == 0) cam.worldSpeed.x -= F_NUM_UP((cam.worldSpeed.x > 0)?(CAM_ACCELERATION):(CAM_ACCELERATION >> 1));
	}
	if(K_LR & buttons){//Move camera right
		if(accelerator == 0) cam.worldSpeed.x += F_NUM_UP((cam.worldSpeed.x < 0)?(CAM_ACCELERATION):(CAM_ACCELERATION >> 1));
	}
	if(!((K_LL|K_LR) & buttons)){//Decelerate the camera movement
	    if(accelerator == 0) {
			if(cam.worldSpeed.x < 0) cam.worldSpeed.x += F_NUM_UP(CAM_ACCELERATION >> 2)+1;
			if(cam.worldSpeed.x > 0) cam.worldSpeed.x -= F_NUM_UP(CAM_ACCELERATION >> 2)+1;
			if(cam.worldSpeed.y < 0) cam.worldSpeed.y += F_NUM_UP(CAM_ACCELERATION >> 2)+1;
			if(cam.worldSpeed.y > 0) cam.worldSpeed.y -= F_NUM_UP(CAM_ACCELERATION >> 2)+1;
		}
	}
	//Cap the speed of the camera
	if(cam.worldSpeed.x < -F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.x = -F_NUM_UP(CAM_MAX_SPEED);
	if(cam.worldSpeed.x > F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.x = F_NUM_UP(CAM_MAX_SPEED);
	if(cam.worldSpeed.y < -F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.y = -F_NUM_UP(CAM_MAX_SPEED);
	if(cam.worldSpeed.y > F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.y = F_NUM_UP(CAM_MAX_SPEED);
	//Update the camera position
	cam.worldPosition.x+=cam.worldSpeed.x;
	cam.worldPosition.y+=cam.worldSpeed.y;
	//Keep the camera in the game area
	if(cam.worldPosition.x <= -F_NUM_UP(MAX_GAME_BOUNDS)){
		cam.worldPosition.x=-F_NUM_UP(MAX_GAME_BOUNDS);
		cam.worldSpeed.x = 0;
	}
	if(cam.worldPosition.x >= F_NUM_UP(MAX_GAME_BOUNDS)){
		cam.worldPosition.x=F_NUM_UP(MAX_GAME_BOUNDS);
		cam.worldSpeed.x = 0;
	}
	if(cam.worldPosition.y >= F_NUM_UP(MAX_GAME_BOUNDS)){
		cam.worldPosition.y = F_NUM_UP(MAX_GAME_BOUNDS);
		cam.worldSpeed.y = 0;
	}
	if(cam.worldPosition.y <= -F_NUM_UP(MAX_GAME_BOUNDS)){
		cam.worldPosition.y = -F_NUM_UP(MAX_GAME_BOUNDS);
		cam.worldSpeed.y = 0;
	}
	//Move the cross hairs up and down
	/*
	if(K_LD & buttons){
		crossH.sy -= 2;
		if(crossH.sy <= 0) crossH.sy = 0;
	}
	if(K_LU & buttons){
		crossH.sy += 2;
		if(crossH.sy >= (SCREEN_HEIGHT>>1)) crossH.sy = (SCREEN_HEIGHT>>1);
	}
	*/
}

/***************************************************
Adds a node the game node array and initializes it
with an object and sets that objects health value
along with its type for how to process it
types are:
T_NODE_GROUND (ex ground)
T_NODE_CITY (ex city)
T_NODE_METEOR (meteor)
****************************************************/
void addNode(object* o, u8 h, u8 t){
	if(gameNodeIdx >= MAX_GAME_NODES) return;
	gameNodes[gameNodeIdx].type = t;
	gameNodes[gameNodeIdx].health = h;
	gameNodes[gameNodeIdx].currentHealth = h;
	gameNodes[gameNodeIdx].animFrame = 0;
	gameNodes[gameNodeIdx].animSpeed = 0;
	gameNodes[gameNodeIdx].animTotalFrames = 1;
	gameNodes[gameNodeIdx].anim[0] = o;
	gameNodeIdx++;
}

/********************************************************
This function needs to be called after the draw object
function. The global vertex buffer will contain all the
rendered vertices of an object. We need to loop through
them and figure out a square on the screen that would
represent the object.
********************************************************/
void getHitSquare(object* o, vector3d* lowLeft, vector3d* upRight){
	u32 i;
	u32 loop;

	loop = o->objData->vertexSize / 3;

	for(i=0; i<loop; i++){
	  if (i!=0){
	    if(vertexBuffer[i].sx < lowLeft->sx){
	      lowLeft->sx = vertexBuffer[i].sx;
	    }
	    if(vertexBuffer[i].sy > lowLeft->sy){
	      lowLeft->sy = vertexBuffer[i].sy;
	    }
	    if(vertexBuffer[i].sx > upRight->sx){
	      upRight->sx = vertexBuffer[i].sx;
	    }
	    if(vertexBuffer[i].sy < upRight->sy){
	      upRight->sy = vertexBuffer[i].sy;
	    }
	  }else{
	    lowLeft->sx = vertexBuffer[i].sx;
	    lowLeft->sy = vertexBuffer[i].sy;
	    upRight->sx = vertexBuffer[i].sx;
	    upRight->sy = vertexBuffer[i].sy;
	  }
	}
}

/***********************************************************
This function will use the lower left and upper right screen
coordinates of 2 squares and see if they over lap
***********************************************************/
void detectHit(vector3d* vLowLeft1, vector3d* vUpRight1, vector3d* vLowLeft2, vector3d* vUpRight2, u8* h){
	*h = 0;

	if(!(vLowLeft2->sx > vUpRight1->sx ||
             vUpRight2->sx < vLowLeft1->sx ||
             vUpRight2->sy > vLowLeft1->sy ||
             vLowLeft2->sy < vUpRight1->sy)) *h = 1;
}

/**************************************
Draw a set of cross hairs on the screen
**************************************/
void inline drawCrossHairs(vector3d* pos){
    vector3d v1, v2;
	v1.sx = pos->sx;
	v2.sx = pos->sx;
	v1.sy = pos->sy;
	v2.sy = pos->sy;
	v1.z = 0;
	v2.z = 0;

	v1.sy += 10;
	v2.sy += 10;
	v1.sx += -8;
	v2.sx += 8;
	g3d_drawLine(&v1,&v2,(u8)1);
	v1.sy += -20;
	v2.sy += -20;
	g3d_drawLine(&v1,&v2,(u8)1);
	v1.sx += 3;
	v2.sx += -3;
	v1.sy += 3;
	v2.sy += 3;
	g3d_drawLine(&v1,&v2,(u8)1);
	v1.sy += 14;
	v2.sy += 14;
	g3d_drawLine(&v1,&v2,(u8)1);
}

/***********************************************
Init the camera variables
***********************************************/
void initCam(){
	cam.worldPosition.x = 0;
	cam.worldPosition.y = 0;
	cam.worldPosition.z = F_NUM_UP(-200);
	cam.moveTo.x        = 0;
	cam.moveTo.y        = 0;
	cam.moveTo.z        = 0;
	cam.worldRotation.x = 0;
	cam.worldRotation.y = 0;
	cam.worldRotation.z = 0;
	cam.worldSpeed.x    = 0;
	cam.worldSpeed.y    = 0;
	cam.worldSpeed.z    = 0;
	cam.d               = F_NUM_UP(256);
}

/***********************
Set up the timer handler
***********************/
void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();

	tick++;

	timer_int(1);
	timer_enable(1);
}

/**********************************
Pretty standard code to get the VB
up and running
***********************************/
void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();

	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;

	HW_REGS[WCR] = 1;

	VIP_REGS[FRMCYC] = 0;

	copymem((u8*)CharSeg3, PVB_FONT, 0x2000);

	WA[31].head = WRLD_ON;
	WA[31].w = 380;
	WA[31].h = 224;
	WA[30].head = WRLD_END;

	tim_vector = (u32)timeHnd;
	timer_freq(1);
	timer_set(SOUND_TIMER);
	timer_enable(1);
	timer_int(1);
}

/*******************************
Controls the timing of the screen
refresh. Borrowed from the Hunter game
written by DanB.
*******************************/
void screenControl(void){
	u32* t;

	static u16 pgflip = XPBSY1;
	pgflip = ~pgflip & 0x000C;

	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] | XPEN);
	while(!(VIP_REGS[XPSTTS] & pgflip));
	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] & ~XPEN);//Disable drawing

	t = currentFrameBuffer;
	currentFrameBuffer = nextFrameBuffer;
	nextFrameBuffer = t;
}
