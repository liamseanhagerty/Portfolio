TITLE Color Matrix						(main.asm)

;/*****************************************************************************
;*                                                                            *
;*                           Las Positas College                              *
;*                                 CS 21                                      *
;*   Assignment: Lab 9 EC                                                     *
;*       Author: Liam Hagerty                                                 *
;*     Revision: 1                                                            *
;*         Date: 2-21-2008                                                    *
;*                                                                            *
;*   Problem Description: Problem 8 on page 149                               *
;*                                                                            *
;*   Procedures:                                                              *
;*   No  Name                        Summary Description                      *
;*   -- --------  ------------------------------------------------------------*
;*   1. main      Entry point to program (and only procedure)                 *
;*                                                                            *
;*****************************************************************************/

INCLUDE Irvine32.inc

.data

.code
;/*****************************************************************************
;*                                                                            *
;*   Procedure: main                                                          *
;*                                                                            *
;*   Detailed Description: This program displays all the possible color       *
;*     combinations of text and background for a console application.         *
;*                                                                            *
;*   Calling Parameters:                                                      *
;*   ** Not Applicable **                                                     *
;*                                                                            *
;*   Variables Modified:                                                      *
;*   ** Not Applicable **                                                     *
;*                                                                            *
;*   Registers Used:                                                          *
;*   eax, ecx, al                                                             *
;*                                                                            *
;*   Macros Used:                                                             *
;*   1. mWinDebug                                                             *
;*                                                                            *
;*   Procedures Called:                                                       *
;*   SetTextColor, WriteChar, Crlf, WaitMsg                                   *
;*                                                                            *
;*****************************************************************************/
main PROC
    ;//Initilization Logic
    mov ecx,16
    mov eax,0
    
L1: ;//Outer loop for background color
    push ecx
    mov ecx,16

  L2: ;//Inner loop for text color      
      call SetTextColor
      inc eax
	 push eax
      
      ;//Write the characters to the screen
      mov al,'0'
      call WriteChar
	 pop eax
      loop L2
      
    ;//Restore the ecx register
    pop ecx
    
    ;//Get everything ready for the next pass    
    call Crlf
    loop L1
    
    ;//Pause the program so we can see the results
     mov eax,white+(black*16)
	call SetTextColor
	call WaitMsg
	exit
main ENDP

END main