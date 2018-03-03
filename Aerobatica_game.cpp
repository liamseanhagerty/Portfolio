/******************************************************************************
*        Title: Aerobatica
* Date Started: April 10th, 2009
*    Developer: Liam Hagerty
*       Module: Game Code Module
*  Description: This module contains all the game logic and flow procedures
*      Version: 1.0
******************************************************************************/
#pragma region Includes
#include "game.h"        //Game Definitions Header
#include "musicPlayer.h" //Header for sound implementation
#pragma endregion

#pragma region Global Variables
LPDIRECT3DTEXTURE9 spriteSheetPointer, spriteSheetMirrorPointer;
SPRITE playerJet, enemyVulcanJet, enemyUnguidedMissileJet, enemyHelicopter;
SPRITE enemyBomber, playerBullet, enemyBullet, missile, homingMissile;
long start = GetTickCount();
LPD3DXSPRITE spriteHandlerPointer;
LPDIRECT3DSURFACE9 backgroundPointer;
HRESULT resultHandle;
extern LPDIRECT3DDEVICE9 direct3DDevicePointer;
extern LPDIRECT3DSURFACE9 backbufferPointer;
MUSICPLAYER gameMusic;
#pragma endregion

int Game_Init(HWND windowHandle)
{
	/**************************************************************************
	*  PreCondition: The window has been set up correctly
	* PostCondition: The start-up game settings will be initialized
	*   Description: Initializes the game
	*     Algorithm: Seed the Random Number Generator
	*                Initialize the Keyboard
	*                Create the Sprite Handler object
	*                Load the Sprites' Textures()
	*                Load the Background
	*                Set the default Sprites' Properties()
	*                Show the instructions
	*                Load and play the Music
	**************************************************************************/

	//set random number seed
	srand(time(NULL));	

	//Initialize Keyboard
	if (!Init_Keyboard(windowHandle))
	{
		MessageBox(windowHandle, "Error initializing the keyboard", "Error", MB_OK);
		return 0;
	}

	//create the Sprite Handler object
	resultHandle = D3DXCreateSprite(direct3DDevicePointer, &spriteHandlerPointer);

	//make sure the sprite handler was created successfully
	if (resultHandle != D3D_OK)
		return 0;

	//load the sprite animations
	if(!Load_Animations())
		return 0;

	//load the background
	backgroundPointer = LoadSurface("sky9.jpg",D3DCOLOR_XRGB(255,0,255));

	//Set the default data for the sprites
	Set_Sprites_Properties();

	//Initialize the sound handler
	gameMusic.MPInit();

	//Show controls
	//MessageBox(windowHandle, "Controls:\nArrows to move\nSpacebar to fire","Controls",MB_OK);

	//play the Background music
	gameMusic.PlaySongMP("Chopper.mid");

	//return okay
	return 1;
}

void Game_Run(HWND windowHandle)
{
	/**************************************************************************
	*  PreCondition: The Game_Init function has run successfully
	* PostCondition: All steps will be performed to keep the game properly updated
	*   Description: Main Game Loop
	*     Algorithm: Make sure the Direct 3D Device is still valid
	*                Determine if significant delay has passed (maintain frame rate)
	*                  Reset framerate timer
	*                  Check if the player has won
	*                  Check if the player has lost
	*                  Move the enemy planes
	*                  Move all the discharged firearms
	*                  Check for hit enemies
	*                Check for input()
	*                Draw the next frame on the Backbuffer(Rendering)
	*                Copy the Backbuffer to the screen
	**************************************************************************/		

	//make sure the Direct3D Device is valid
	if (direct3DDevicePointer == NULL)
		return;	

	//after short delay, ready for next frame?
	//this keeps the game running at a steady frame rate
	if(GetTickCount() - start >= 30)
	{
		//reset timing
		start = GetTickCount();		

		//Check Victory Condition
		if(enemyBomber.destroyed)
		{
			//Congratulate the player then tell Windows to end the program
			MessageBox(windowHandle,"Congratulations, you have cleared the skies!","Victory!",MB_OK);
			PostMessage(windowHandle, WM_DESTROY, 0, 0);
		}

		//Check Loss Conditions
		if(Check_Loss())
		{
			//Inform the player of their defeat and tell Windows to end the program
			MessageBox(windowHandle,"You have been shot down. Better luck next time!","Game Over",MB_OK);
			PostMessage(windowHandle, WM_DESTROY, 0, 0);
		}

		//Move the enemy planes
		Move_Enemies();

		//move the bullets and missiles
		Move_Weaponry();

		//Check for enemy planes being shot down
		Check_Scoring();
	}

	//Check for keyboard input
	Check_Input(windowHandle);

	//start rendering
	if(direct3DDevicePointer->BeginScene())
	{
		//erase the entire background
		direct3DDevicePointer->StretchRect(backgroundPointer,NULL,backbufferPointer,
			NULL,D3DTEXF_NONE);

		//start the Sprite Handler
		spriteHandlerPointer->Begin(D3DXSPRITE_ALPHABLEND);

		//Draw the Sprites
		Draw_Sprites();

		//stop drawing
		spriteHandlerPointer->End();

		//stop rendering
		direct3DDevicePointer->EndScene();
	}

	//display the back buffer on the screen
	direct3DDevicePointer->Present(NULL, NULL, NULL, NULL);
}

void Game_End(HWND windowHandle)
{
	/**************************************************************************
	*  PreCondition: The Game Loop has finished
	* PostCondition: All resources will be freed for future use
	*   Description: This function performs all post-game clean-up
	*     Algorithm: Free the Surface
	*                Free the Background
	*                Free the Sprite Handler
	*                Free the Sound Effects
	**************************************************************************/

	//free the surfaces
	spriteSheetPointer->Release();
	spriteSheetMirrorPointer->Release();

	//free the background
	if(backgroundPointer != NULL)
		backgroundPointer->Release();

	//free the sprite handler
	if(spriteHandlerPointer != NULL)
		spriteHandlerPointer->Release();

	//free the sound file
	gameMusic.MPRelease();
}

void Check_Input(HWND windowHandle)
{
	/**************************************************************************
	*  PreCondition: The game loop is running
	* PostCondition: Keyboard presses will be checked and updated
	*   Description: This function handles all actions associated with the keyboard
	*     Algorithm: Update the keyboard state
	*                If the Left Arrow is pressed,
	*                  face the jet left and move left, though not offscreen
	*                Else If the Right Arrow is pressed,
	*                  face the jet right and move right, though not offscreen
	*                If the Up Arrow is pressed,
	*                  move the jet up, though not offscreen
	*                Else If the Down Arrow is pressed,
	*                  move the jet down, though not offscreen
	*                If the Spacebar is pressed,
	*                  Fire the bullet from the front of the jet if one is not present
	*                If the Escape Key is pressed,
	*                  end the game and application
	**************************************************************************/
	//update the keyboard
	Poll_Keyboard();

	//check for left arrow
	if(Key_Down(DIK_LEFT))
	{
		//Check if the player is trying to go offscreen
		if(playerJet.xCoordinate > 0)
			playerJet.xCoordinate -= playerJet.xSpeed; //Move left
		//Face the player left
		playerJet.faceRight = false;		
	}
	//check for right arrow
	else 		
		if(Key_Down(DIK_RIGHT))
		{
			//Check if player is trying to go offscreen
			if(playerJet.xCoordinate + playerJet.width < 1000)
				playerJet.xCoordinate += playerJet.xSpeed; //Move right
			//Face the player right
			playerJet.faceRight = true;				
		}
	
	//check for up arrow
	if(Key_Down(DIK_UP))
	{
		//Check if the player is trying to go offscreen
		if(playerJet.yCoordinate > 0)
			playerJet.yCoordinate -= playerJet.ySpeed; //Move up	
	}
	//check for down arrow
	else 
	{
		if(Key_Down(DIK_DOWN))	
			//Check if the player is trying to go offscreen
			if(playerJet.yCoordinate + playerJet.height < 700)
				playerJet.yCoordinate += playerJet.ySpeed;	//Move down
	}

	//check for Space Bar
	if(Key_Down(DIK_SPACE))
	{
		//Make sure the player is not reloading
		if(!playerBullet.onscreen)
		{
			//Fire the bullet from the jet front
			if(playerJet.faceRight)
			{
				//Put the bullet ahead of the player and face it right
				playerBullet.xCoordinate = playerJet.xCoordinate + playerJet.width + 5;
				playerBullet.faceRight = true;

				//If the previous bullet's vector is backwards, fix it
				if(playerBullet.xSpeed < 0)
					playerBullet.xSpeed = -playerBullet.xSpeed;
			}
			//Bullet will travel left
			else
			{
				//Put the bullet ahead of the player and face it left
				playerBullet.xCoordinate = playerJet.xCoordinate - 5;
				playerBullet.faceRight = false;

				//If the previous bullet's vector is backwards, fix it
				if(playerBullet.xSpeed > 0)
					playerBullet.xSpeed = -playerBullet.xSpeed;
			}

			//Fire the bullet from the jet center
			playerBullet.yCoordinate = playerJet.yCoordinate + playerJet.height / 2;
			playerBullet.onscreen = true;
		}
	}

	//check for escape key to exit program
	if (Key_Down(DIK_ESCAPE))
		PostMessage(windowHandle, WM_DESTROY, 0, 0); //End the program
}

bool Load_Animations()
{
	/**************************************************************************
	*  PreCondition: Direct 3D was initialized
	* PostCondition: The skins associated with the game sprites will be loaded
	*   Description: This function loads and partitions the sprite sheet for
	*                  each of the game's sprites
	*     Algorithm: Load the normal sprite sheet
	*                Make sure the sheet loaded correctly
	*                Load the mirrored sprite sheet
	*                Make sure the sheet loaded correctly
	**************************************************************************/
	//load the normal sprite animations
	spriteSheetPointer = LoadTexture("sprite sheet copy.jpg",D3DCOLOR_XRGB(255,255,255));

	//make sure the sprite animations were loaded successfully
	if (spriteSheetPointer == NULL)
		return false;

	//load the mirrored sprite animations
	spriteSheetMirrorPointer = LoadTexture("sprite sheet copy mirror.jpg",
		D3DCOLOR_XRGB(255,255,255));

	//make sure the mirrored sprite animations were loaded correctly
	if (spriteSheetMirrorPointer == NULL)
		return false;

	//everything loaded fine
	return true;
}

void Set_Sprites_Properties()
{
	/**************************************************************************
	*  PreCondition: The Direct 3D Device is valid
	* PostCondition: The default properties of the game sprites will be set
	*   Description: This function sets all the default properties of the sprites
	*     Algorithm: Set the Player Sprite's properties
	*                Set the Player Bullet Sprite's properties
	*                Set the Vulcan Enemy Sprite's properties
	*                Set the Enemy Bullet Sprite's properties
	*                Set the Missile Enemy Sprite's properties
	*                Set the Missile Sprite's properties
	*                Set the Homing Missile Sprite's properties
	*                Set the Helicopter Enemy Sprite's properties
	*                Set the Bomber Enemy Sprite's properties
	**************************************************************************/
	//initialize the player sprite's properties
	playerJet.xCoordinate = 100;
	playerJet.yCoordinate = 350;
	playerJet.xSpeed = 5;
	playerJet.ySpeed = 5;
	playerJet.width = 126;
	playerJet.height = 47;
	playerJet.faceRight = true;
	playerJet.destroyed = false;
	playerJet.onscreen = true;

	//initialize the player's bullet sprite properties
	playerBullet.xCoordinate = -200;
	playerBullet.yCoordinate = -200;
	playerBullet.xSpeed = 50;
	playerBullet.ySpeed = 0;
	playerBullet.width = 317;
	playerBullet.height = 36;
	playerBullet.faceRight = true;
	playerBullet.destroyed = false;
	playerBullet.onscreen = false;

	//initialize the enemy vulcan jet sprite's properties
	enemyVulcanJet.xCoordinate = 700;
	enemyVulcanJet.yCoordinate = -100;
	enemyVulcanJet.xSpeed = 0;
	enemyVulcanJet.ySpeed = 5;
	enemyVulcanJet.width = 140;
	enemyVulcanJet.height = 33;
	enemyVulcanJet.faceRight = false;
	enemyVulcanJet.destroyed = false;
	enemyVulcanJet.onscreen = false;

	//initialize the enemy's bullet sprite properties
	enemyBullet.xCoordinate = 1200;
	enemyBullet.yCoordinate = 900;
	enemyBullet.xSpeed = 5;
	enemyBullet.ySpeed = 0;
	enemyBullet.width = 317;
	enemyBullet.height = 36;
	enemyBullet.faceRight = false;
	enemyBullet.destroyed = false;
	enemyBullet.onscreen = false;

	//initialize the enemy missile jet sprite's properties
	enemyUnguidedMissileJet.xCoordinate = 1100;
	enemyUnguidedMissileJet.yCoordinate = 600;
	enemyUnguidedMissileJet.xSpeed = -5;
	enemyUnguidedMissileJet.ySpeed = 0;
	enemyUnguidedMissileJet.width = 128;
	enemyUnguidedMissileJet.height = 32;
	enemyUnguidedMissileJet.faceRight = false;
	enemyUnguidedMissileJet.destroyed = false;
	enemyUnguidedMissileJet.onscreen = false;

	//initialize the missile sprite's properties
	missile.xCoordinate = 1500;
	missile.yCoordinate = 1000;
	missile.xSpeed = -5;
	missile.ySpeed = 0;
	missile.width = 509;
	missile.height = 40;
	missile.faceRight = false;
	missile.destroyed = false;
	missile.onscreen = false;

	//initialize the homing missile sprite's properties
	homingMissile.xCoordinate = 1300;
	homingMissile.yCoordinate = 500;
	homingMissile.xSpeed = 5;
	homingMissile.ySpeed = 5;
	homingMissile.width = 509;
	homingMissile.height = 40;
	homingMissile.faceRight = false;
	homingMissile.destroyed = false;
	homingMissile.onscreen = false;

	//initialize the enemy helicopter sprites's properties
	enemyHelicopter.xCoordinate = -200;
	enemyHelicopter.yCoordinate = 200;
	enemyHelicopter.xSpeed = 5;
	enemyHelicopter.ySpeed = 5;
	enemyHelicopter.width = 144;
	enemyHelicopter.height = 41;
	enemyHelicopter.faceRight = true;
	enemyHelicopter.destroyed = false;
	enemyHelicopter.onscreen = false;

	//initialize the enemy bomber sprite's properties
	enemyBomber.xCoordinate = 500;
	enemyBomber.yCoordinate = 800;
	enemyBomber.xSpeed = 0;
	enemyBomber.ySpeed = -5;
	enemyBomber.width = 309;
	enemyBomber.height = 98;
	enemyBomber.faceRight = false;
	enemyBomber.destroyed = false;
	enemyBomber.onscreen = false;
}

void Draw_Sprites()
{
	/**************************************************************************
	*  PreCondition: Direct 3D was initialized correctly
	* PostCondition: The desired sprites will be drawn to the backbuffer
	*   Description: This function draws the game sprites to the backbuffer
	*     Algorithm: Get the positions of the sprites
	*                Create and initialize Drawing Rectangles for the Sprites
	*                If the sprite is facing right, use normal sprite sheet
	*                Else, use the mirrored sprite sheet for drawing
	**************************************************************************/
	//Create the vectors to update sprite positions
	D3DXVECTOR3 playerPosition((float)playerJet.xCoordinate, (float)playerJet.yCoordinate, 0);
	D3DXVECTOR3 enemyVulcanPosition((float)enemyVulcanJet.xCoordinate, (float)enemyVulcanJet.yCoordinate, 0);
	D3DXVECTOR3 enemyMissilePosition((float)enemyUnguidedMissileJet.xCoordinate,
		(float)enemyUnguidedMissileJet.yCoordinate, 0);
	D3DXVECTOR3 enemyHelicopterPosition((float)enemyHelicopter.xCoordinate, (float)enemyHelicopter.yCoordinate, 0);
	D3DXVECTOR3 enemyBomberPosition((float)enemyBomber.xCoordinate, (float)enemyBomber.yCoordinate, 0);
	D3DXVECTOR3 playerBulletPosition((float)playerBullet.xCoordinate, (float)playerBullet.yCoordinate, 0);
	D3DXVECTOR3 enemyBulletPosition((float)enemyBullet.xCoordinate, (float)enemyBullet.yCoordinate, 0);

	//Create the drawing rectangles for the sprites
	RECT playerRectangle, enemyVulcanRectangle, enemyMissileRectangle;
	RECT enemyHelicopterRectangle, enemyBomberRectangle, playerBulletRectangle;
	RECT enemyBulletRectangle;

	//Initialize the rectangles
	ZeroMemory(&playerRectangle, sizeof(playerRectangle));
	ZeroMemory(&enemyVulcanRectangle, sizeof(enemyVulcanRectangle));
	ZeroMemory(&enemyMissileRectangle, sizeof(enemyMissileRectangle));
	ZeroMemory(&enemyHelicopterRectangle, sizeof(enemyHelicopterRectangle));
	ZeroMemory(&enemyBomberRectangle, sizeof(enemyBomberRectangle));
	ZeroMemory(&playerBulletRectangle, sizeof(playerBulletRectangle));
	ZeroMemory(&enemyBulletRectangle, sizeof(enemyBulletRectangle));

	//configure and draw the player rectangle
	if(playerJet.faceRight)
		Draw_To_Backbuffer(playerJet, playerRectangle, playerPosition, 22, 33, 144, 76);
	else
		Draw_To_Backbuffer(playerJet, playerRectangle, playerPosition, 1855, 33, 1975, 76);

	//configure and draw the Vulcan Jet rectangle
	if(!enemyVulcanJet.destroyed)
		if(enemyVulcanJet.faceRight)
			Draw_To_Backbuffer(enemyVulcanJet, enemyVulcanRectangle, enemyVulcanPosition, 36, 526, 174, 557);
		else
			Draw_To_Backbuffer(enemyVulcanJet, enemyVulcanRectangle, enemyVulcanPosition, 1825, 526, 1961, 557);
	else
		enemyVulcanJet.xCoordinate = -500;

	//configure and draw the Missile Jet rectangle
	if(!enemyUnguidedMissileJet.destroyed)
		if(enemyUnguidedMissileJet.faceRight)
			Draw_To_Backbuffer(enemyUnguidedMissileJet, enemyMissileRectangle, enemyMissilePosition, 38, 782, 165, 812);
		else
			Draw_To_Backbuffer(enemyUnguidedMissileJet, enemyMissileRectangle, enemyMissilePosition, 1834, 782, 1960, 812);
	else
		enemyUnguidedMissileJet.xCoordinate = -500;

	//configure and draw the Helicopter rectangle
	if(!enemyHelicopter.destroyed)
		if(enemyHelicopter.faceRight)
			Draw_To_Backbuffer(enemyHelicopter, enemyHelicopterRectangle, enemyHelicopterPosition, 29, 1078, 170, 1120);
		else
			Draw_To_Backbuffer(enemyHelicopter, enemyHelicopterRectangle, enemyHelicopterPosition, 1825, 1078, 1969, 1120);
	else
		enemyHelicopter.xCoordinate = -500;

	//configure and draw the Bomber rectangle	
	if(enemyBomber.faceRight)
		Draw_To_Backbuffer(enemyBomber, enemyBomberRectangle, enemyBomberPosition, 0, 249, 335, 345);
	else
		Draw_To_Backbuffer(enemyBomber, enemyBomberRectangle, enemyBomberPosition, 1663, 249, 2000, 345);

	//Configure and draw the player bullet rectangle
	if(playerBullet.faceRight)
		Draw_To_Backbuffer(playerBullet, playerBulletRectangle, playerBulletPosition, 584, 307, 945, 343);
	else
		Draw_To_Backbuffer(playerBullet, playerBulletRectangle, playerBulletPosition, 1053, 307, 1379, 343);

	//Configure and draw the enemy bullet rectangle
	if(enemyBullet.faceRight)
		Draw_To_Backbuffer(enemyBullet, enemyBulletRectangle, enemyBulletPosition, 584, 307, 945, 343);
	else
		Draw_To_Backbuffer(enemyBullet, enemyBulletRectangle, enemyBulletPosition, 1053, 307, 1379, 343);
}

void Draw_To_Backbuffer(SPRITE entity, RECT spriteRectangle, D3DXVECTOR3 position,
						long leftX, long topY, long rightX, long bottomY)
{
	/**************************************************************************
	*  PreCondition: WINAPI has been initialized
	* PostCondition: The sprite will be drawn to the backbuffer
	*   Description: This function sets the coordinates for the image source of
	*                  a sprite from a sheet and then draws them to the backbuffer
	*     Algorithm: Set the left-most x coordinate
	*                Set the top-most y coordinate
	*                Set the right-most x coordinate
	*                Set the bottom-most y coordinate
	*                If the sprite is facing right, use the normal sprite sheet
	*                Else use the mirrored sprite sheet for drawing
	**************************************************************************/	

	//Set the rectangle parameters for the source file
	spriteRectangle.left = leftX;
	spriteRectangle.top = topY;
	spriteRectangle.right = rightX;
	spriteRectangle.bottom = bottomY;

	//draw the sprite
	if(entity.faceRight)
		spriteHandlerPointer->Draw(spriteSheetPointer, &spriteRectangle, NULL,
			&position, D3DCOLOR_XRGB(255,255,255));
	else
		spriteHandlerPointer->Draw(spriteSheetMirrorPointer, &spriteRectangle, NULL,
			&position, D3DCOLOR_XRGB(255,255,255));
}

int Check_Collision(SPRITE Sprite1, SPRITE Sprite2)
{
	/**************************************************************************
	*  PreCondition: The Sprites have been initialized
	* PostCondition: It will be determined if two Sprites are colliding
	*   Description: This function determines if two sprites are colliding
	*     Algorithm: Load the first Sprite's coordinates into a RECT
	*                Load the second Sprite's coordinates into a RECT
	*                Call IntersectRect() to determine collision validity
	**************************************************************************/
	RECT spriteRectangle1, spriteRectangle2, intersectionRectangle;

	//Initialize the first rectangle
	spriteRectangle1.left = Sprite1.xCoordinate;
	spriteRectangle1.top = Sprite1.yCoordinate;
	spriteRectangle1.right = Sprite1.xCoordinate + Sprite1.width;
	spriteRectangle1.bottom = Sprite1.yCoordinate + Sprite1.height;

	//Initialize the second rectangle
	spriteRectangle2.left = Sprite2.xCoordinate;
	spriteRectangle2.top = Sprite2.yCoordinate;
	spriteRectangle2.right = Sprite2.xCoordinate + Sprite2.width;
	spriteRectangle2.bottom = Sprite2.yCoordinate + Sprite2.height;

	//Determine collision
	return IntersectRect(&intersectionRectangle, &spriteRectangle1, &spriteRectangle2);
}

bool Check_Loss()
{
	/**************************************************************************
	*  PreCondition: The game loop is running
	* PostCondition: It will be determined if the player has been destroyed
	*   Description: This function checks collision of the player against all
	*                  possible enemy sprites
	*     Algorithm: Check player collision against enemy bullet
	*                Check player collision against enemy missile
	*                Check player collision against enemy homing missile
	*                Check player collision against vulcan jet
	*                Check player collision against missile jet
	*                Check player collision against helicopter
	*                If all of the above fail, player has not lost
	**************************************************************************/
	//Check collision against an enemy bullet
	if(Check_Collision(playerJet,enemyBullet))
	{
		playerJet.destroyed = true;
		enemyBullet.xCoordinate = -200;
		enemyBullet.yCoordinate = -200;
		return true;
	}
	else
		//Check collision against an enemy dumbfire missile
		if(Check_Collision(playerJet, missile))
		{
			playerJet.destroyed = true;
			missile.xCoordinate = 1500;
			missile.yCoordinate = 1000;
			return true;
		}
		else
			//check collision against an enemy homing missile
			if(Check_Collision(playerJet, homingMissile))
			{
				playerJet.destroyed = true;
				homingMissile.xCoordinate = 1300;
				homingMissile.yCoordinate = 500;
				return true;
			}
			else
				//check if the player rammed the vulcan jet
				if(Check_Collision(playerJet, enemyVulcanJet))
				{
					playerJet.destroyed = true;
					enemyVulcanJet.destroyed = true;
					return true;
				}
				else
					//check if the player rammed the missile jet
					if(Check_Collision(playerJet, enemyUnguidedMissileJet))
					{
						playerJet.destroyed = true;
						enemyUnguidedMissileJet.destroyed = true;
						return true;
					}
					else
						//check if the player rammed the helicopter
						if(Check_Collision(playerJet, enemyHelicopter))
						{
							playerJet.destroyed = true;
							enemyHelicopter.destroyed = true;
							return true;
						}	

	//Player is OK
	return false;
}

void Move_Enemies()
{
	/**************************************************************************
	*  PreCondition: The enemy sprites have been allocated properly
	* PostCondition: The enemy sprites will be moved
	*   Description: This function keeps the enemy planes moving onscreen
	*     Algorithm: If the Vulcan Jet is still flying,
	*                  Make its entrance, then move it up and down the screen
	*                Else If the Missile Jet is still flying,
	*                  Make its entrance, then move it left and right on screen
	*                Else If the Helicopter is still flying,
	*                  Make its entrance, then move it diagonally on screen
	*                Else If the Bomber is still flying,
	*                  Make its entrance, then movie it up and down the screen
	**************************************************************************/

	//If the Vulcan Jet hasn't been shot down
	if(!enemyVulcanJet.destroyed)
	{
		//move the vulcan jet
		enemyVulcanJet.yCoordinate += enemyVulcanJet.ySpeed;

		//note entrance onscreen
		if(enemyVulcanJet.yCoordinate > 0 && enemyVulcanJet.yCoordinate < 700)
			enemyVulcanJet.onscreen = true;		

		//if the vulcan jet has moved offscreen, change its direction
		if(enemyVulcanJet.onscreen)
			if(enemyVulcanJet.yCoordinate + enemyVulcanJet.height > 700 || enemyVulcanJet.yCoordinate < 0)
				enemyVulcanJet.ySpeed = -enemyVulcanJet.ySpeed;
	}
	else
		//If the Missile Jet hasn't been shot down
		if(!enemyUnguidedMissileJet.destroyed)
		{
			//move the missile jet
			enemyUnguidedMissileJet.xCoordinate += enemyUnguidedMissileJet.xSpeed;

			//note entrance onscreen
			if(enemyUnguidedMissileJet.xCoordinate > 0 && enemyUnguidedMissileJet.xCoordinate + enemyUnguidedMissileJet.width  < 1000)
				enemyUnguidedMissileJet.onscreen = true;		

			//if the missile jet has moved offscreen, change its direction
			if(enemyUnguidedMissileJet.onscreen)				
				if(enemyUnguidedMissileJet.xCoordinate < 0 || 
					enemyUnguidedMissileJet.xCoordinate + enemyUnguidedMissileJet.width > 1000)
				{
					enemyUnguidedMissileJet.xSpeed = -enemyUnguidedMissileJet.xSpeed;
					enemyUnguidedMissileJet.faceRight = !enemyUnguidedMissileJet.faceRight;
				}
		}
		else
			//If the Helicopter hasn't been shot down
			if(!enemyHelicopter.destroyed)
			{
				//move the helicopter
				enemyHelicopter.xCoordinate += enemyHelicopter.xSpeed;
				enemyHelicopter.yCoordinate += enemyHelicopter.ySpeed;

				//note entrance onscreen
				if((enemyHelicopter.xCoordinate > 0 && enemyHelicopter.xCoordinate < 1000) &&
					(enemyHelicopter.yCoordinate > 0 && enemyHelicopter.yCoordinate < 700))
					enemyHelicopter.onscreen = true;		

				//if the helicopter has moved offscreen, change its direction
				if(enemyHelicopter.onscreen)
				{
					//if the helicopter moves offscreen, change its direction
					if(enemyHelicopter.xCoordinate < 0 || enemyHelicopter.xCoordinate + enemyHelicopter.width > 1000)
					{
						enemyHelicopter.xSpeed = -enemyHelicopter.xSpeed;
						enemyHelicopter.faceRight = !enemyHelicopter.faceRight;
					}
					if(enemyHelicopter.yCoordinate < 0 || enemyHelicopter.yCoordinate + enemyHelicopter.height > 700)
						enemyHelicopter.ySpeed = -enemyHelicopter.ySpeed;
				}
			}
			else
				//If the Bomber Boss hasn't been shot down
				if(!enemyBomber.destroyed)
				{
					//move the bomber
					enemyBomber.yCoordinate += enemyBomber.ySpeed;

					//note entrance onscreen
					if(enemyBomber.yCoordinate > 0 && enemyBomber.yCoordinate + enemyBomber.height < 700)
						enemyBomber.onscreen = true;		

					//if the vulcan jet has moved offscreen, change its direction
					if(enemyBomber.onscreen)
						//if the Bomber moves offscreen, change its direction
						if(enemyBomber.yCoordinate < 0 || enemyBomber.yCoordinate + enemyBomber.height > 700)
							enemyBomber.ySpeed = -enemyBomber.ySpeed;
				}
			return;
}

void Move_Weaponry()
{
	/**************************************************************************
	*  PreCondition: The sprites have been set up correctly
	* PostCondition: The weaponry sprites will be updated
	*   Description: This function moves and updates all the weapon sprites
	*     Algorithm: If the player has fired a bullet, move it on screen
	*                If the enemy has fired a bullet, move it on screen
	*                If the enemy fires a missile, move it on screen
	**************************************************************************/

	//If the player has fired a shot
	if(playerBullet.onscreen)
	{
		//move the bullet
		playerBullet.xCoordinate += playerBullet.xSpeed;

		//if the bullet goes off the screen, make note the shot is finished
		if(playerBullet.xCoordinate > 1000 || playerBullet.xCoordinate < 0 - playerBullet.width)
			playerBullet.onscreen = false;
	}

	//If the enemy has fired a shot
	if(enemyBullet.onscreen)
	{
		//move the bullet
		enemyBullet.xCoordinate += enemyBullet.xSpeed;

		//if the bullet goes off the screen, make note the shot is finished
		if(enemyBullet.xCoordinate < 0 - enemyBullet.width)
			enemyBullet.onscreen = false;
	}

	//If the enemy has fired a missile
	if(missile.onscreen)
	{
		//move the missile
		missile.xCoordinate += missile.xSpeed;

		//if the missile goes off the screen, make note the shot is finished
		if(missile.xCoordinate < 0 - missile.width)
			missile.onscreen = false;
	}
}

void Check_Scoring()
{
	/**************************************************************************
	*  PreCondition: The enemy sprites have been initialized
	* PostCondition: The appropriate enemy will be checked for being hit by the player
	*   Description: This function checks if the player has hit an enemy with a
	*                  bullet
	*     Algorithm: Check if the player hit the Vulcan Jet
	*                Check if the player hit the Missile Jet
	*                Check if the player hit the Helicopter
	*                Check if the player hit the Bomber
	**************************************************************************/
	//Check if the player's bullet hit the Vulcan Jet
	if(Check_Collision(playerBullet, enemyVulcanJet))
		enemyVulcanJet.destroyed = true;
	else
		//Check if the player's bullet hit the Missile Jet
		if(Check_Collision(playerBullet, enemyUnguidedMissileJet))
			enemyUnguidedMissileJet.destroyed = true;
		else
			//Check if the player's bullet hit the Helicopter
			if(Check_Collision(playerBullet, enemyHelicopter))
				enemyHelicopter.destroyed = true;
			else
				//Check if the player's bullet hit the Bomber
				if(Check_Collision(playerBullet, enemyBomber))
					enemyBomber.destroyed = true;
}