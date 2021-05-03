#include <iostream>
#include <time.h>
#include "SimpleSDLGame.h"

/*****************************/
// Using a Visual Leak Deteck /
//#include "vld.h"   ////////////
/*****************************/

int main(int argc, char** argv)
{
	SimpleSDLGame game;

	game.Init();
	game.Update();
	game.ShutDown();
	return 0;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////

Use WSAD to control the direction, 
Use Upper Arrow to fire bullet, 
Use Space to boost speed.

////////////////////////////////////////////////////////////////////////////////////////////
*/
