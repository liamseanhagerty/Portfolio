/******************************************************************************
 *         Author: Liam Hagerty
 *        Project: Pong With Wii-ware
 *   Date Started: August 12th, 2007
 * Date Completed: October 10th, 2007
 *         Module: Main Game Module
 *        Version: 1.3
 *    Description: This is the main game module. It handles all the playing
 *                 functions and events.
 *****************************************************************************/
#region Using Statements
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Media;
using WiimoteLib;
#endregion

namespace PongWiimoteCSharp
{
    public partial class frmMain : Form
    {		
		#region Constants
		const short P1PaddleSpeed = 25, AIPaddleSpeed = 5, BallSpeed = 5;
		const short MinimumHeight = 5, MaximumHeight = 250, WinningScore = 7;
		#endregion
		
		#region Global Variables
		int BallSpeedX = 5, BallSpeedY = 0;
		short P1Score = 0, P2Score = 0, AIVictories = 0;
		int OriginalBallX = 0, OriginalBallY = 0;
        int OriginalPaddle1Y = 0, OriginalPaddle2Y = 0;		
        bool HasHit = false, AIPlayer = false, WiiRemotePresent = false;
		SoundPlayer BallHit = new SoundPlayer("Queue.wav");
		SoundPlayer ScorePoint = new SoundPlayer("scifi1.wav");
		SoundPlayer Victory = new SoundPlayer("Theme.WAV");
		Label AIWins = new Label();
		Wiimote Remote = new Wiimote();
		#endregion

		#region Form Events
		public frmMain()
        {
            /******************************************************************
             *  PreCondition: Program load
             * PostCondition: Starting form will be initialized
             *   Description: Form Constructor
             *     Algorithm: Set-up the Main Form                
             *****************************************************************/
            InitializeComponent();            
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: Form loading
             * PostCondition: The original position of the ball will be recorded
             *   Description: This function tracks the original ball position
             *                  (the center of the field) and original paddle 
             *                   positions for use in resetting the game 
             *     Algorithm: Record the original positions of the ball and the
             *                  two player paddles
			 *                Dynamically add the AI victory label
			 *                Try to connect to the Wii Remote via:
			 *                  Use the alternate polling method
			 *                  Attempt to establish a connection
			 *                  Note successful connection
			 *                  Turn on the Player 1 LED
			 *                Catch any exception thrown by the Remote and:
			 *                  Display a MessageBox that the device could
			 *                    not be connected
             *****************************************************************/
            //Record the original Ball and Paddle coordinates
            OriginalBallX = pbxBall.Location.X;
            OriginalBallY = pbxBall.Location.Y;            
            OriginalPaddle1Y = pbxPlayer1Paddle.Location.Y;            
            OriginalPaddle2Y = pbxPlayer2Paddle.Location.Y;
            
			//Dynamically add a label for victories against the AI
			AIWins.Parent = this;
			AIWins.Show();
			AIWins.Left = 250;
			AIWins.Top = 37;
			AIWins.TextAlign = ContentAlignment.MiddleCenter;
			AIWins.Text = "Wins VS AI:";
			AIWins.Visible = true;

			try
			{
				//Connect to the Wii Remote
				Remote.AltWriteMethod = true;
				Remote.Connect();
				WiiRemotePresent = true;

				//Light up the player 1 LED so the player knows the device is ready
				Remote.SetLEDs(true, false, false, false);
			}
			catch
			{
				//Inform the user that no Wii Remote was found
				MessageBox.Show("Unable to connect to the Wii Remote. Is one on?", "Wii Remote Connection Error.",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
        }

		private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The form is closing
			 * PostCondition: If a Wii Remote is connected, it will be disconnected
			 *   Description: This event terminates any communication with the
			 *                  Wii Remote
			 *     Algorithm: If a Wii Remote is connected,
			 *                  Turn off the Player 1 LED
			 *                  Disconnect the Wii Remote
			 *****************************************************************/
			//If there's a Wii Remote connected, disconnect from it
			if (WiiRemotePresent)
			{
				Remote.SetLEDs(false, false, false, false); //Turn off the LEDs
				Remote.Disconnect();
			}
		}
		#endregion

		#region Menu Bars
		private void mnuFileNewgame_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: Main form has loaded successfully
             * PostCondition: The Game Board will be hidden and the player buttons
             *                  will be displayed
             *   Description: This item ends the current game and resets all the
             *                  play variables in addition to inquiring for the
             *                  number of players
             *     Algorithm: Reset the Game()
             *****************************************************************/
			//Reset the Game
            ResetGame();          
        }

        private void mnuFileExit_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: Main form has loaded successfully
             * PostCondition: The Application will close
             *   Description: This item closes the Pong application
             *     Algorithm: Close the Application
             *****************************************************************/			
			//Exit the Game
            Application.Exit();			
        }

        private void mnuHelpHelp_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The form has loaded successfully
             * PostCondition: The Instructions Box will be displayed
             *   Description: This menu item displays an informative Messagebox
             *                  regarding the play controls
             *     Algorithm: Show the Instructions Messagebox
             *****************************************************************/
			//Show the instructions pop-up
            MessageBox.Show("Player One: Up and Down Arrow Keys\nPlayer Two: Mouse" +
				"\n\nPlayer One may use the Wii Remote in either orientation as follows,\n" +
			"D-Pad Up/Down: Move paddle\nA/1: Toggle Rumble\nB/2: LED Lightshow\n"+
			"+/-: Change Background Color\nHome: Reset the Game", "Instructions",
			MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void mnuHelpAbout_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The main form has loaded correctly
             * PostCondition: The About form will be displayed
             *   Description: This item displays the About form
             *     Algorithm: Create a new frmAbout object
             *                Display the new object
             *****************************************************************/
			//Create and Display a new About form
            frmAbout About = new frmAbout();
            About.ShowDialog();
        }
		
		private void mnuOptionsFont_Click(object sender, EventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The form has loaded correctly
			 * PostCondition: The Game Font will be modified accordingly
			 *   Description: This menu item opens a font dialog that allows the
			 *                  player to choose the font on the game controls
			 *     Algorithm: Pause the Game if it's running
			 *                Let the player choose a new font
			 *                Resume the Game if applicable
			 *                If the player chose a new font, apply it
			 *                Else, do nothing
			 *****************************************************************/
			bool AIPlaying = false, GameInProgress = false;

			//Pause the Game
			if (tmrAI.Enabled)
			{
				AIPlaying = true;
				tmrAI.Enabled = false;
			}
			if (tmrBall.Enabled)
			{
				GameInProgress = true;
				tmrBall.Enabled = false;
			}

			//Open the Font Chooser Dialog
			DialogResult result = fntdGameFont.ShowDialog();

			//Resume the Game
			if (GameInProgress)
				tmrBall.Enabled = true;
			if (AIPlaying)
				tmrAI.Enabled = true;

			//If the user didn't make any changes
			if (result == DialogResult.Cancel)
				//Do nothing
				return;

			//Assign the new font to all the game controls
			lblPlayer1Score.Font = fntdGameFont.Font;
			lblPlayer2Score.Font = fntdGameFont.Font;
			btnSinglePlayer.Font = fntdGameFont.Font;
			btnMulitplayer.Font = fntdGameFont.Font;
		}

		private void mnuOptionsColorsBall_Click(object sender, EventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The form has loaded correctly
			 * PostCondition: The Ball's Color will be modified accordingly
			 *   Description: This menu item opens a color dialog that allows
			 *                  the player to modify the Ball's color
			 *     Algorithm: Pause the Game if it is running
			 *                Get the desired color from the user
			 *                Resume the game if applicable
			 *                If the user changed his/her mind,
			 *                  do nothing
			 *                Change the color of the Ball to the chosen color
			 *****************************************************************/
			bool AIPlaying = false, GameInProgress = false;

			//Pause the Game
			if (tmrAI.Enabled)
			{
				AIPlaying = true;
				tmrAI.Enabled = false;
			}
			if (tmrBall.Enabled)
			{
				GameInProgress = true;
				tmrBall.Enabled = false;
			}

			//Open the Color Chooser Dialog
			DialogResult result = clrdColorPicker.ShowDialog();

			//Resume the Game
			if (GameInProgress)
				tmrBall.Enabled = true;
			if (AIPlaying)
				tmrAI.Enabled = true;

			//If the user didn't make any changes
			if (result == DialogResult.Cancel)
				//Do nothing
				return;

			//Assign the chosen color to the Ball
			pbxBall.BackColor = clrdColorPicker.Color;
		}

		private void mnuOptionsColorP1Paddle_Click(object sender, EventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The form has loaded correctly
			 * PostCondition: Player 1's Paddle Color will be changed
			 *   Description: This menu item opens a color dialog that allows
			 *                  the player to modify the 1st Player Paddle color
			 *     Algorithm: Pause the Game if it is running
			 *                Get the desired color from the user
			 *                Resume the game if applicable
			 *                If the user changed his/her mind,
			 *                  do nothing
			 *                Change the color of Player 1's Paddle to the color
			 *****************************************************************/
			bool AIPlaying = false, GameInProgress = false;

			//Pause the Game
			if (tmrAI.Enabled)
			{
				AIPlaying = true;
				tmrAI.Enabled = false;
			}
			if (tmrBall.Enabled)
			{
				GameInProgress = true;
				tmrBall.Enabled = false;
			}

			//Open the Color Dialog for Color Selection
			DialogResult result = clrdColorPicker.ShowDialog();

			//Resume the Game
			if (GameInProgress)
				tmrBall.Enabled = true;
			if (AIPlaying)
				tmrAI.Enabled = true;

			//If the user didn't make any changes
			if (result == DialogResult.Cancel)
				//Do nothing
				return;

			//Assign the chosen color to the Ball
			pbxPlayer1Paddle.BackColor = clrdColorPicker.Color;
		}

		private void mnuOptionsColorsP2Paddle_Click(object sender, EventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The form has loaded correctly
			 * PostCondition: Player 2's Paddle Color will be changed
			 *   Description: This menu item opens a color dialog that allows
			 *                  the player to modify the 2nd Player Paddle color
			 *     Algorithm: Pause the Game if it is running
			 *                Get the desired color from the user
			 *                Resume the Game if applicable
			 *                If the user changed his/her mind,
			 *                  do nothing
			 *                Change the color of Player 2's Paddle to the color
			 *****************************************************************/
			bool AIPlaying = false, GameInProgress = false;

			//Pause the Game
			if (tmrAI.Enabled)
			{
				AIPlaying = true;
				tmrAI.Enabled = false;
			}
			if (tmrBall.Enabled)
			{
				GameInProgress = true;
				tmrBall.Enabled = false;
			}

			//Open the Color Dialog for Color Selection
			DialogResult result = clrdColorPicker.ShowDialog();

			//Resume the Game
			if (GameInProgress)
				tmrBall.Enabled = true;
			if (AIPlaying)
				tmrAI.Enabled = true;

			//If the user didn't make any changes
			if (result == DialogResult.Cancel)
				//Do nothing
				return;

			//Assign the chosen color to the Ball
			pbxPlayer2Paddle.BackColor = clrdColorPicker.Color;
		}
		#endregion

        #region Buttons
        private void btnSinglePlayer_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The Main form has loaded correctly
             * PostCondition: The game will be started with the AI enabled
             *   Description: This button shows all the game elements and
             *                enables the in-game AI
             *     Algorithm: Show the Paddles and Ball
             *                Hide the player selection buttons
			 *                If a Wii Remote is connected,
			 *                  Enable the Wii Remote Input Timer
             *                Enable the Ball Movement Timer
             *                Enable the AI for player 2
			 *                Update the Status Bar
             *****************************************************************/
            //Show the Ball and Paddles
            pbxBall.Visible = true;
            pbxPlayer1Paddle.Visible = true;
            pbxPlayer2Paddle.Visible = true;
            
            //Hide the Player Select Buttons
            btnSinglePlayer.Visible = false;
            btnMulitplayer.Visible = false;

			//Check for a Wii Remote
			if (WiiRemotePresent)
				tmrWiimoteInput.Enabled = true;
            
			//Start the Ball Moving
            tmrBall.Enabled = true;			

			//Start the AI Playing for Player 2
            AIPlayer = true;
            tmrAI.Enabled = true;
			
			//Make note that the Game has started
			sbrGameState.Text = "Location: Playing Game";
        }

        private void btnMulitplayer_Click(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The Main form has loaded correctly
             * PostCondition: The game will be started with 2 humans
             *   Description: This button begins a 2 human game
             *     Algorithm: Show the Paddles and the Ball
             *                Hide the player selection buttons
			 *                If a Wii Remote is connected,
			 *                  Enable the Wii Remote Input Timer
             *                Enable the Ball Movement Timer
			 *                Update the Status Bar
             *****************************************************************/
            //Show the Ball and Paddles
            pbxBall.Visible = true;
            pbxPlayer1Paddle.Visible = true;
            pbxPlayer2Paddle.Visible = true;
            
            //Hide the Player Select Buttons
            btnSinglePlayer.Visible = false;
            btnMulitplayer.Visible = false;

			//Check for a Wii Remote
			if (WiiRemotePresent)
				tmrWiimoteInput.Enabled = true;
            
			//Start the Ball Moving
            tmrBall.Enabled = true;

			//Make note that the Game has started
			sbrGameState.Text = "Location: Playing Game";
        }
        #endregion

        #region Timers
        private void tmrBall_Tick(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The game has started in either mode
             * PostCondition: The main game loop will be active
             *   Description: This timer handles all the game checks and input
             *     Algorithm: Turn off the Help Tooltips
             *                Keep the form in focus
             *                Check if Player 1 has won,
             *                  If so, reset the game and congratulate
			 *                  If the AI was playing, increment the AI Victories
             *                Else check if Player 2 has won,
             *                  If so, reset the game and congratulate
			 *                In both cases, play the victory music
             *                Check if ball has hit player 1's paddle,
             *                  If not, Check if ball has hit player 2's paddle,
             *                     If not, Check if ball has hit the floor or ceiling,
             *                       If not, check if ball has hit the scoring walls
             *                If the Ball was hit by a paddle at any time,
             *                  move ball in Y Direction
             *                Move Ball in X direction
             *****************************************************************/
			//Disable the Help Tooltips so they don't get in the way
            ttpInformation.Active = false;
            
			//Make sure the form is in focus so the keyboard input works correctly
            this.Focus();

			//Check if Player 1 has won
            if (P1Score == WinningScore)
            {
				//If the AI was playing, increment the AI Victory Counter
				if (tmrAI.Enabled)
				{
					AIVictories++;
					lblAIWinScore.Text = AIVictories.ToString();
				}

				//Reset the game to the initial screen and show a MessageBox congratulating Player 1
                ResetGame();
				Victory.Play();
                MessageBox.Show("Player 1 Wins!", "Winner!", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);				
                return;
            }
            
			//Check if Player 2 has won
            if (P2Score == WinningScore)
            {
				//Reset the game to the initial screen and show a MessageBox congratulating Player 2
                ResetGame();
				Victory.Play();
                MessageBox.Show("Player 2 Wins!", "Winner!", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);				
                return;
            }

			//Check if the Ball has hit Player 1's Paddle
            if (!CheckHitP1Paddle())
				//Check if the Ball has hit Player 2's Paddle
                if (!CheckHitP2Paddle())
					//Check if the Ball has hit the Floor or the Ceiling
                    if (!CheckHitFloorCeiling())
						//Check if the Ball Hit the Walls for scoring
                        CheckHitWalls();
			
			//Check if the Ball has historically hit anything not a scoring wall
            if (HasHit)
				//Start the Ball moving vertically
                pbxBall.Top += BallSpeedY;

			//Move the Ball horizontally
            pbxBall.Left += BallSpeedX;            
        }

        private void tmrAI_Tick(object sender, EventArgs e)
        {
            /******************************************************************
             *  PreCondition: The game has started in Single Player mode
             * PostCondition: The Computer will control Player 2's Paddle
             *   Description: This timer handles all of the logic associated with
             *                  handling a Pong Paddle
             *     Algorithm: DO NOT Set Player 2's Paddle Y to the Ball's Y (undefeatable)
             *                If Player 2's Paddle is lower than Ball, raise
             *                  Paddle up a few pixels
             *                Else If Player 2's Paddle is higher than the Ball,
             *                  lower paddle a few pixels
             *****************************************************************/
            //Undefeatable AI, Code included for future reference purposes
            //pbxPlayer2Paddle.Top = pbxBall.Top;

			//If the AI's Paddle is above the Ball
            if (pbxPlayer2Paddle.Top > pbxBall.Top && pbxPlayer2Paddle.Top > pnlPlayingField.Top)
				//Lower the AI's paddle
                pbxPlayer2Paddle.Top -= AIPaddleSpeed;
            else
				//If the AI's Paddle is below the Ball
                if (pbxPlayer2Paddle.Top < pbxBall.Top && pbxPlayer2Paddle.Top < pnlPlayingField.Height-pbxPlayer2Paddle.Height)
					//Raise the AI's paddle
                    pbxPlayer2Paddle.Top += AIPaddleSpeed;
        }

		private void tmrWiimoteInput_Tick(object sender, EventArgs e)
		{
			/******************************************************************
			 *  PreCondition: The game has started and a Wiimote is present
			 * PostCondition: Player 1's Paddle will be moved accordingly
			 *   Description: This timer checks for input from the Nintendo Wii
			 *                  Remote and adjusts Player 1's Paddle accordingly
			 *     Algorithm: If a Wii Remote is connected,
			 *                  If Up or Right on Plus Pad is pressed,
			 *                    Move Player 1's Paddle Up
			 *                  Else If Down or Left on Plus Pad is pressed,
			 *                    Move Player 1's Paddle Down
			 *                  If the A or 1 Button is pressed,
			 *                    If the rumble is on,
			 *                      Turn off the Remote's Rumble
			 *                    Else,
			 *                      Turn on the Remote's Rumble
			 *                  If the B or 2 button is pressed,
			 *                    Make a lightshow on the Remote's LEDs
			 *                  If the Home button is pressed,
			 *                    Reset the Game()
			 *                  If the + button is pressed,
			 *                    Set the field's color to Gold
			 *                  If the - button is pressed,
			 *                    Set the field's color to Pink
			 *****************************************************************/
			//Check if the Wii Remote is connected
			if (WiiRemotePresent)
			{
				if (Remote.WiimoteState.ButtonState.Up || Remote.WiimoteState.ButtonState.Right)
					//Move paddle up
					pbxPlayer1Paddle.Top -= P1PaddleSpeed;
				else
					if (Remote.WiimoteState.ButtonState.Down || Remote.WiimoteState.ButtonState.Left)
						//Move paddle down
						pbxPlayer1Paddle.Top += P1PaddleSpeed;

				if (Remote.WiimoteState.ButtonState.A || Remote.WiimoteState.ButtonState.One)
					//If the rumble's on, turn it off
					if (Remote.WiimoteState.Rumble)
						Remote.SetRumble(false);
					else
						Remote.SetRumble(true); //Turn on the rumble

				if (Remote.WiimoteState.ButtonState.B || Remote.WiimoteState.ButtonState.Two)
				{
					//Make an LED lightshow
					Remote.SetLEDs(false, true, false, false);
					Remote.SetLEDs(false, false, true, false);
					Remote.SetLEDs(false, false, false, true);
					Remote.SetLEDs(false, false, true, false);
					Remote.SetLEDs(false, true, false, false);
					Remote.SetLEDs(true, false, false, false);
				}

				if (Remote.WiimoteState.ButtonState.Home)
				{
					//Reset the Game
					ResetGame();
				}

				if (Remote.WiimoteState.ButtonState.Plus)
				{
					//Change the playing field's color to Gold
					pnlPlayingField.BackColor = Color.Goldenrod;
				}

				if (Remote.WiimoteState.ButtonState.Minus)
				{
					//Change the playing field's color to Pink
					pnlPlayingField.BackColor = Color.Pink;
				}
						
			}
		}
        #endregion

        #region Game Checks
        private bool CheckHitP1Paddle()
        {
            /******************************************************************
             *  PreCondition: The game has started
             * PostCondition: Wether the ball has collided will be determined
             *   Description: This function determines if the ball has hit Player
             *                1's paddle
             *     Algorithm: If ball is colliding in X direction and has a Y
             *                  coordinate coinciding with the paddle, reverse
             *                  the ball direction
             *                Check if hit top of paddle
             *                   Set ball Y movement to Up
             *                Check if hit bottom of paddle
             *                   Set ball Y movement to Down
             *                If any of the above occur, play collision sound			 
             *****************************************************************/
            //If ball has hit between top and bottom of paddle
            if (pbxBall.Left <= (pbxPlayer1Paddle.Left + pbxPlayer1Paddle.Size.Width) && 
				pbxBall.Top >= pbxPlayer1Paddle.Top && 
				pbxBall.Top <= (pbxPlayer1Paddle.Top + pbxPlayer1Paddle.Size.Height))
            {
                //Reverse the Ball direction
                BallSpeedX = -BallSpeedX;

                //If ball has hit top half of paddle
                if (pbxBall.Top > pbxPlayer1Paddle.Top && 
					pbxBall.Top <= (pbxPlayer1Paddle.Top + pbxPlayer1Paddle.Size.Height / 2))
                {
                    //Make Ball Angle Up
                    HasHit = true;
                    BallSpeedY = -BallSpeed;
                }
                else
                {
                    //If ball has hit bottom half of paddle
                    if (pbxBall.Top > pbxPlayer1Paddle.Top &&
						pbxBall.Top > (pbxPlayer1Paddle.Top + pbxPlayer1Paddle.Size.Height / 2))
                    {
                        //Make Ball Angle Down
                        HasHit = true;
                        BallSpeedY = BallSpeed;
                    }
                }
				//Play the Collision Sound
				BallHit.Play();
				pbxBall.Left++;
				if (BallSpeedX > 0)
					BallSpeedX++;
				else
					BallSpeedX--;
                return true;
            }
            return false;
        }

        private bool CheckHitP2Paddle()
        {
            /******************************************************************
             *  PreCondition:The game has started
             * PostCondition: Wether the ball has collided will be determined
             *   Description: This function determines if the ball has hit Player
             *                2's paddle
             *     Algorithm: If ball is colliding in X direction and has a Y
             *                  coordinate coinciding with the paddle, reverse
             *                  the ball direction
             *                Check if hit top of paddle
             *                   Set ball Y movement to Up
             *                Check if hit bottom of paddle
             *                   Set ball Y movement to Down
             *                If any of the above occur, play the collision sound
             *****************************************************************/
            //If ball has hit between top and bottom of paddle
            if (pbxBall.Left >= pbxPlayer2Paddle.Left - pbxPlayer2Paddle.Size.Width &&
				pbxBall.Top >= pbxPlayer2Paddle.Top &&
				pbxBall.Top <= (pbxPlayer2Paddle.Top + pbxPlayer2Paddle.Size.Height))
            {
                //Reverse the Ball direction
                BallSpeedX = -BallSpeedX;

                //If ball has hit top half of paddle
                if (pbxBall.Top > pbxPlayer2Paddle.Top &&
					pbxBall.Top <= (pbxPlayer2Paddle.Top + pbxPlayer2Paddle.Size.Height / 2))
                {
                    //Make Ball Angle Up
                    HasHit = true;
                    BallSpeedY = -BallSpeed;
                }
                else
                {
                    //If Ball has hit bottom half of Paddle
                    if (pbxBall.Top > pbxPlayer2Paddle.Top && 
						pbxBall.Top > (pbxPlayer2Paddle.Top + pbxPlayer2Paddle.Size.Height / 2))
                    {
                        //Make Ball Angle Down
                        HasHit = true;
                        BallSpeedY = BallSpeed;
                    }
                }
				//Play the Collision Sound
				pbxBall.Left--;
				if (BallSpeedX > 0)
					BallSpeedX++;
				else
					BallSpeedX--;
				BallHit.Play();
                return true;
            }
            return false;
        }

        private bool CheckHitFloorCeiling()
        {
            /******************************************************************
             *  PreCondition: The game has started
             * PostCondition: It will be determined if the ball has hit a floor
             *                  or ceiling
             *   Description: This function checks if the ball has collided with
             *                  the game floor or ceiling
             *     Algorithm: If the Ball is above or touching the ceiling,
			 *                  Set the Ball's Y-travel direction down
			 *                Else if the Ball is below or touching the floor,
			 *                  Set the Ball's Y-travel direction up
			 *                In both cases, play the collision sound
             *****************************************************************/
            //If the ball is above/hitting the ceiling
			if (pbxBall.Location.Y <= MinimumHeight)
			{
				//Send the Ball traveling Down and play the collision sound
				BallSpeedY = BallSpeed;
				BallHit.Play();
				return true;
			}
			else
				//If the Ball is below/hitting the floor
				if (pbxBall.Location.Y >= (pnlPlayingField.Location.Y + MaximumHeight))
				{
					//Send the Ball travelling Up and play the collision sound
					BallSpeedY = -BallSpeed;
					BallHit.Play();
					return true;
				}
            return false;
        }

        private void CheckHitWalls()
        {
			/******************************************************************
			 *  PreCondition: Game has started
			 * PostCondition: Determined if a player has scored a point
			 *   Description: This function determines if either player has scored
			 *                  a point
			 *     Algorithm: If ball X coordinate < field left then,
			 *                  increment player 2's score
			 *                  reset the ball
			 *                  play Scoring sound
			 *                Else if ball x coordinate > field right then,
			 *                  increment player 1's score
			 *                  reset the ball
			 *                  play Scoring sound
			 *****************************************************************/
			//If the Ball passes the Left Wall
            if (pbxBall.Location.X <= pnlPlayingField.Location.X - 20)
            {
                //Increment Player 2's Score
                P2Score++;
                lblPlayer2Score.Text = "Player 2 Score: " + P2Score;

                //Reset the Ball
                pbxBall.Left = OriginalBallX;
                pbxBall.Top = OriginalBallY;
                BallSpeedY = 0;
				if (BallSpeedX > 0)
					BallSpeedX = 5;
				else
					BallSpeedX = -5;
                HasHit = false;

				//Play Scoring Sound
				ScorePoint.Play();
            }
            else
                //If Ball passes Right Wall
                if (pbxBall.Location.X >= pnlPlayingField.Location.X + 705)
                {
                    //Increment Player 1's Score
                    P1Score++;
                    lblPlayer1Score.Text = "Player 1 Score: " + P1Score;

                    //Reset the Ball
                    pbxBall.Left = OriginalBallX;
                    pbxBall.Top = OriginalBallY;
                    BallSpeedY = 0;
					if (BallSpeedX > 0)
						BallSpeedX = 5;
					else
						BallSpeedX = -5;
                    HasHit = false;

					//Play Scoring Sound
					ScorePoint.Play();
                }			
        }        

        private void frmMain_KeyDown(object sender, KeyEventArgs e)
        {
            /******************************************************************
             *  PreCondition: The game has started
             * PostCondition: If neccessary, P1's paddle will be moved
             *   Description: This event determines if the up or down arrow was
             *                  pressed and moves P1's paddle accordingly
             *     Algorithm: If up key is pressed, move paddle up field
             *                Else if down key is pressed, move paddle down field
             *****************************************************************/
            //Check if Up Key is pressed
			if (e.KeyData == Keys.Up)
			{
				if (pbxPlayer1Paddle.Top > 0)
					//Move paddle up
					pbxPlayer1Paddle.Top -= P1PaddleSpeed;
			}
			else
				//Check if Down Key is pressed
				if (e.KeyData == Keys.Down)
					if(pbxPlayer1Paddle.Top < pnlPlayingField.Height - pbxPlayer1Paddle.Height)
					//Move paddle down
					pbxPlayer1Paddle.Top += P1PaddleSpeed;
        }

        private void ResetGame()
        {
            /******************************************************************
             *  PreCondition: Either player has scored enough points to win
             * PostCondition: The game will be reset
             *   Description: This function resets the ball and paddles' positions
             *                  and prompts for the number of players
             *     Algorithm: Disable the Ball, Wii Remote, and AI Timers
             *                Reset Ball to original location
             *                Reset Paddles to original locations
             *                Reset scores to 0
             *                Hide the Game Board
             *                Show the player select buttons and Information Tooltips
			 *                Update the game status bar
             *****************************************************************/
            //Disable Timers
            tmrBall.Enabled = false;
			tmrWiimoteInput.Enabled = false;

			//Check if the AI was playing
            if (AIPlayer)
            {
				//Disable the AI
                AIPlayer = false;
                tmrAI.Enabled = false;
            }            

            //Reset the Ball, Paddles, and Scores and hide them
            pbxBall.Top = OriginalBallY;
            pbxBall.Left = OriginalBallX;
            pbxPlayer1Paddle.Top = OriginalPaddle1Y;
            pbxPlayer2Paddle.Top = OriginalPaddle2Y;
            P1Score = 0;
            P2Score = 0;
			BallSpeedX = 5;
            lblPlayer1Score.Text = "Player 1 Score: 0";
            lblPlayer2Score.Text = "Player 2 Score: 0";
            pbxBall.Visible = false;
            pbxPlayer1Paddle.Visible = false;
            pbxPlayer2Paddle.Visible = false;
			pnlPlayingField.BackColor = Color.Gainsboro;
            
            //Show the Player Select Buttons and the Information Tooltips
            btnMulitplayer.Visible = true;
            btnSinglePlayer.Visible = true;
            ttpInformation.Active = true;

			//Make note that the Game has ended
			sbrGameState.Text = "Location: Main Screen";
        }
        #endregion

        #region Field Events
        private void pnlPlayingField_MouseMove(object sender, MouseEventArgs e)
        {
            /******************************************************************
             *  PreCondition: The game has started
             * PostCondition: P2's paddle will be updated
             *   Description: This event updates P2's paddle with mouse movement
             *     Algorithm: If the AI player is off, set paddle Y coordinate
             *                  to mouse Y coordinate
             *****************************************************************/
            //If the game has started and Player 2 is not the AI
            if (tmrBall.Enabled && !AIPlayer)
				if(e.Y > 0 && e.Y < pnlPlayingField.Height - pbxPlayer2Paddle.Height)
				//Move Player 2's Paddle to the current mouse height
                pbxPlayer2Paddle.Top = e.Y;
        }        
        #endregion		
    }
}