#include <libgccvb.h>
#include "G3d.h"
#include "Font.h"
#include "Game.h"

volatile u32 tick;
vector3d crossH;//cross hair vectors
u8 accelerator;
u8 frameSkip = 0;
u8 frameSkipCount = 0;

#define CAM_MAX_SPEED 15
s32 meteorSpeed;

#define MAX_GAME_NODES 20
u8 gameNodeIdx = 0;
node gameNodes[MAX_GAME_NODES];
#define T_NODE_GROUND 1
#define T_NODE_CITY 2
#define T_NODE_METEOR 3

#define SOUND_TIMER 8
#define FPS_THROTTLE (1000000/20/SOUND_TIMER/30)

#include "G3d.c"

object g,c1,c2,c3,c4,c5,c6,m1,m2,m3,m4,m5,m6;

int main(){	
	u8 run;

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
		
		handleInput();
				
		
		//This will loop through all the nodes in the game
		for(i=0; i<gameNodeIdx; i++){
			//Frame skip
			if(frameSkip) break;
			
			//process the nodes based on type
			if(gameNodes[i].type == T_NODE_GROUND){
				g3d_drawObject(gameNodes[i].anim[gameNodes[i].animFrame]);
			}
			if(gameNodes[i].type == T_NODE_CITY){
				g3d_drawObject(gameNodes[i].anim[gameNodes[i].animFrame]);
			}
			if(gameNodes[i].type == T_NODE_METEOR){
				if((accelerator & 0x03) == 3){
					calculateSpeed(gameNodes[i].anim[gameNodes[i].animFrame]);
				}
				g3d_moveObject(gameNodes[i].anim[gameNodes[i].animFrame]);
				g3d_drawObject(gameNodes[i].anim[gameNodes[i].animFrame]);				
			}
			
			//handle animation frames
			if((gameNodes[i].animFrame + 1) < gameNodes[i].animTotalFrames){
				gameNodes[i].animFrame++;
			}else{
				gameNodes[i].animFrame = 0;
			}
		}		
		//Cross hairs are drawn directly to the screen so they
		//are not processed as a game node.
		drawCrossHairs(&crossH);
		
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
	
	g3d_initObject(&g,(objectData*)ground);
	g3d_initObject(&c1,(objectData*)city);
	g3d_initObject(&c2,(objectData*)city);
	g3d_initObject(&c3,(objectData*)city);
	g3d_initObject(&c4,(objectData*)city);
	g3d_initObject(&c5,(objectData*)city);
	g3d_initObject(&c6,(objectData*)city);
	g3d_initObject(&m1,(objectData*)meteor);
	g3d_initObject(&m2,(objectData*)meteor);
	g3d_initObject(&m3,(objectData*)meteor);
	g3d_initObject(&m4,(objectData*)meteor);
	g3d_initObject(&m5,(objectData*)meteor);
	g3d_initObject(&m6,(objectData*)meteor);
	
	addNode(&g,0,T_NODE_GROUND);
	addNode(&c1,3,T_NODE_CITY);
	addNode(&c2,3,T_NODE_CITY);
	addNode(&c3,3,T_NODE_CITY);
	addNode(&c4,3,T_NODE_CITY);
	addNode(&c5,3,T_NODE_CITY);
	addNode(&c6,3,T_NODE_CITY);
	addNode(&m1,3,T_NODE_METEOR);
	addNode(&m2,3,T_NODE_METEOR);
	addNode(&m3,3,T_NODE_METEOR);
	addNode(&m4,3,T_NODE_METEOR);
	addNode(&m5,3,T_NODE_METEOR);
	addNode(&m6,3,T_NODE_METEOR);
	
	//init object positions / speeds etc..
	c1.worldPosition.z = F_NUM_UP(500);
	c1.worldPosition.x = -4000;
	c1.worldPosition.y = 0;
	
	c2.worldPosition.z = F_NUM_UP(500);
	c2.worldPosition.x = 0;
	c2.worldPosition.y = 0;
	
	c3.worldPosition.z = F_NUM_UP(500);
	c3.worldPosition.x = 4000;
	c3.worldPosition.y = 0;
	
	c4.worldPosition.z = F_NUM_UP(1000);
	c4.worldPosition.x = -4000;
	c4.worldPosition.y = 0;
	
	c5.worldPosition.z = F_NUM_UP(1000);
	c5.worldPosition.x = 0;
	c5.worldPosition.y = 0;
	
	c6.worldPosition.z = F_NUM_UP(1000);
	c6.worldPosition.x = 4000;
	c6.worldPosition.y = 0;
	
	g.worldPosition.z = F_NUM_UP(200);
	g.worldScale.z = F_NUM_UP(2);
	g.properties.doClip = 0;//don't clip the ground
	cam.worldPosition.y = F_NUM_UP(80);
	cam.worldRotation.x = -8;
	
	m1.worldPosition.z = F_NUM_UP(8000);
	m1.worldPosition.x = F_NUM_UP(-1000);
	m1.worldPosition.y = F_NUM_UP(2500);
	m1.worldSpeed.z = meteorSpeed;
	m1.moveTo.x = c2.worldPosition.x;
	m1.moveTo.y = c2.worldPosition.y;
	m1.moveTo.z = c2.worldPosition.z;	
	m1.rotation.x = 16;
	m1.rotation.z = 4;
	m1.worldScale.x = F_NUM_UP(2);
	m1.worldScale.y = F_NUM_UP(2);
	m1.worldScale.y = F_NUM_UP(2);
	
	m2.worldPosition.z = F_NUM_UP(8000);
	m2.worldPosition.x = F_NUM_UP(2000);
	m2.worldPosition.y = F_NUM_UP(3000);
	m2.worldSpeed.z = meteorSpeed;
	m2.moveTo.x = c1.worldPosition.x;
	m2.moveTo.y = c1.worldPosition.y;
	m2.moveTo.z = c1.worldPosition.z;
	m2.rotation.x = 4;
	m2.rotation.z = 4;
	m2.worldScale.x = F_NUM_UP(4);
	m2.worldScale.y = F_NUM_UP(4);
	m2.worldScale.z = F_NUM_UP(4);
	
	m3.worldPosition.z = F_NUM_UP(8000);
	m3.worldPosition.x = F_NUM_UP(3000);
	m3.worldPosition.y = F_NUM_UP(3500);
	m3.worldSpeed.z = meteorSpeed;
	m3.moveTo.x = c4.worldPosition.x;
	m3.moveTo.y = c4.worldPosition.y;
	m3.moveTo.z = c4.worldPosition.z;
	m3.rotation.x = 16;
	m3.rotation.z = 16;
	m3.worldScale.x = F_NUM_UP(1);
	m3.worldScale.y = F_NUM_UP(1);
	m3.worldScale.z = F_NUM_UP(1);
	
	m4.worldPosition.z = F_NUM_UP(8000);
	m4.worldPosition.x = F_NUM_UP(-3000);
	m4.worldPosition.y = F_NUM_UP(3500);
	m4.worldSpeed.z = meteorSpeed;
	m4.moveTo.x = c3.worldPosition.x;
	m4.moveTo.y = c3.worldPosition.y;
	m4.moveTo.z = c3.worldPosition.z;
	m4.rotation.x = 16;
	m4.rotation.z = 16;
	m4.worldScale.x = F_NUM_UP(1);
	m4.worldScale.y = F_NUM_UP(1);
	m4.worldScale.z = F_NUM_UP(1);
	
	m5.worldPosition.z = F_NUM_UP(8000);
	m5.worldPosition.x = F_NUM_UP(-2000);
	m5.worldPosition.y = F_NUM_UP(3000);
	m5.worldSpeed.z = meteorSpeed;
	m5.moveTo.x = c5.worldPosition.x;
	m5.moveTo.y = c5.worldPosition.y;
	m5.moveTo.z = c5.worldPosition.z;	
	m5.rotation.x = 16;
	m5.rotation.z = 4;
	m5.worldScale.x = F_NUM_UP(2);
	m5.worldScale.y = F_NUM_UP(2);
	m5.worldScale.y = F_NUM_UP(2);
	
	m6.worldPosition.z = F_NUM_UP(8000);
	m6.worldPosition.x = F_NUM_UP(2200);
	m6.worldPosition.y = F_NUM_UP(3500);
	m6.worldSpeed.z = meteorSpeed;
	m6.moveTo.x = c6.worldPosition.x;
	m6.moveTo.y = c6.worldPosition.y;
	m6.moveTo.z = c6.worldPosition.z;
	m6.rotation.x = 4;
	m6.rotation.z = 4;
	m6.worldScale.x = F_NUM_UP(4);
	m6.worldScale.y = F_NUM_UP(4);
	m6.worldScale.z = F_NUM_UP(4);	

}

/**************************************
Calculates the speed to send the meteor
in a straight(ish) line
**************************************/
void inline calculateSpeed(object* o){
	s32 z;
	z = o->worldPosition.z / o->worldSpeed.z;
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
	if(K_LL & buttons){//Move camera left
		if(accelerator == 0) cam.worldSpeed.x -= F_NUM_UP((cam.worldSpeed.x > 0)?(3):(1));
	}
	if(K_LR & buttons){//Move camera right
		if(accelerator == 0) cam.worldSpeed.x += F_NUM_UP((cam.worldSpeed.x < 0)?(3):(1));
	}
	if(!((K_LL|K_LR) & buttons)){//Decelerate the camera movement
	    if(accelerator == 0) {
			if(cam.worldSpeed.x < 0) cam.worldSpeed.x += F_NUM_UP(1);
			if(cam.worldSpeed.x > 0) cam.worldSpeed.x -= F_NUM_UP(1);
		}
	}
	//Cap the speed of the camera
	if(cam.worldSpeed.x < -F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.x = -F_NUM_UP(CAM_MAX_SPEED);
	if(cam.worldSpeed.x > F_NUM_UP(CAM_MAX_SPEED)) cam.worldSpeed.x = F_NUM_UP(CAM_MAX_SPEED);
	//Update the camera position
	cam.worldPosition.x+=cam.worldSpeed.x;
	//Keep the camera in the game area
	if(cam.worldPosition.x <= -6000){
		cam.worldPosition.x=-6000;
		cam.worldSpeed.x = 0;
	}
	if(cam.worldPosition.x >= 6000){
		cam.worldPosition.x=6000;
		cam.worldSpeed.x = 0;
	}
	//Move the cross hairs up and down
	if(K_LD & buttons){
		crossH.sy -= 2;
		if(crossH.sy <= 0) crossH.sy = 0;
	}
	if(K_LU & buttons){
		crossH.sy += 2;
		if(crossH.sy >= (SCREEN_HEIGHT>>1)) crossH.sy = (SCREEN_HEIGHT>>1);
	}
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
void getHitSquare(vector3d* lowLeft, vector3d* upRight){
	u32 i;
	
	for(i=0; i<numVertices; i++){
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
u8 detectHit(vector3d* vLowLeft1, vector3d* vUpRight1, vector3d* vLowLeft2, vector3d* vUpRight2){
	u8 r;
	r = 0;

	return r;
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