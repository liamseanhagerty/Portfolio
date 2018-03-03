/******************************************************************************
*        Title: Aerobatica
* Date Started: April 10th, 2009
*    Developer: Liam Hagerty
*       Module: Game Header
*  Description: This module contains information regarding game constants,
*                 objects, and game run routines
*      Version: 1.0
******************************************************************************/
#ifndef _GAME_H
#define _GAME_H 1

#pragma region Include Files
#include <d3d9.h>   //DirectX 9.0c Main Header
#include <d3dx9.h>  //DirectX 9.0c Extension Header
#include <dxerr9.h> //DirectX 9.0c Error Handling Header
#include <dinput.h> //DirectX 9.0c Input Handling Header
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "dxgraphics.h"
#include "dxinput.h"
#pragma endregion

#pragma region Constants
#define FULLSCREEN 0 //0 = Windowed, 1 = Fullscreen
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700
#pragma endregion

//Sprite Structure
typedef struct
{
	int xCoordinate, yCoordinate;
	int xSpeed, ySpeed;
	int width, height;
	bool faceRight, destroyed, onscreen;
} SPRITE;

#pragma region Function Prototypes
int Game_Init(HWND);
void Game_Run(HWND);
void Game_End(HWND);
void Check_Input(HWND);
bool Load_Animations();
void Set_Sprites_Properties();
void Draw_Sprites();
void Draw_To_Backbuffer(SPRITE, RECT, D3DXVECTOR3, long, long, long, long);
int Check_Collision(SPRITE, SPRITE);
bool Check_Loss();
void Move_Enemies();
void Move_Weaponry();
void Check_Scoring();
#pragma endregion
#endif