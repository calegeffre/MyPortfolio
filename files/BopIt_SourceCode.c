#include <stdio.h>
#include <stdlib.h>

/* Initialization Functions */
void	  LEDBARInit(void);
void 	  DIPSWInit(void);
void	  PBSwInit(void);	
void  	RGB_LEDInit(void);
void 	  sysTickInit(void);
void	  RIT128x96x4Init(int freq);
void    potentiometersInit(void);

/* Input and Output Functions */			
void    sysTickWait1mS(int waitTime);
void    RIT128x96x4Clear(void); 
void    RIT128x96x4StringDraw(const char* letter, int xx, int yy, int intensity);
void	  playerCountdown(int player);
void    flashColor(char color, char color2);
void    LEDBAROutput(int value);
void 	  turnOn(char color);
void 	  turnOff(char color);
int 	  read_PBSwitchNum(int SwitchNumber);
int		  read_Switches(void);
int     readPotentiometer0(void);
int     startRound();
int     binaryLives(int lives);

//*****************************************************************************
//
// Main Program:
//
//*****************************************************************************


int main(void) {

	//Initializing the variables used in the project.
	int potenValue0;
  int pB1;
	int test;
	int timer;
	int pot;
	int switches;
	
	int roundCount;		  // 5 rounds max
	int instruction;
	int lives1;         // Each player starts with 5 lives
	int lives2;
	int loseLife1 = 1;  // If player isn't fast enough or does wrong instruction
	int loseLife2 = 1;
	int startGame = 0;
	int inCount = 10;   // 10 instructions per player per round
								
	//Initializing the LEDBAR, RGB LED, DIPSwitches and Pushbuttons, and a wait timer
	LEDBARInit();
	DIPSWInit();
	PBSwInit();
	RGB_LEDInit();
	sysTickInit();
	potentiometersInit();
	RIT128x96x4Init(1000000);
	 
	/* Main Part of the Program starts here */
	
	//Flash to say we are getting ready to go!
	turnOn('B');
	sysTickWait1mS(250);
	turnOff('B');

	//Clear messages on the screen
	RIT128x96x4Clear();
	
	pB1 = read_PBSwitchNum(1);
	lives1 = 2;
  lives2 = 2;	
	roundCount = 0;
	
	// Main program loop		
  while(lives1 > 0 && lives2 > 0 && roundCount < 10){
						
		// Welcome text, push PB1 to begin
    while(read_PBSwitchNum(1) && !startGame){ 
		  	RIT128x96x4StringDraw("Let's Play Bop-It!", 5,00,15);
	  	  RIT128x96x4StringDraw("Press Button To Begin", 00,25,15);
				if(!read_PBSwitchNum(1)){
					startGame = 1;
				}
		}		

		// Countdown until Player 1 starts
		LEDBAROutput(binaryLives(lives1));
	  playerCountdown(1);
		loseLife1 = startRound();
		
	  RIT128x96x4Clear();
		
	  if(loseLife1 == 1){
		  lives1 = lives1 - 1;
			LEDBAROutput(binaryLives(lives1));
      RIT128x96x4StringDraw("You Lose A Life", 5,00,15);
			RIT128x96x4StringDraw("Pass It!", 5,75,15);
		  flashColor('R','R');
	    sysTickWait1mS(2000);
	  }
	  else{
		  RIT128x96x4StringDraw("Round Complete!", 5,00,15);
	    sysTickWait1mS(2000);
	  }
  	
		// Countdown until Player 2 starts
		LEDBAROutput(binaryLives(lives2));
    playerCountdown(2);
		
		loseLife2 = startRound();

    if(loseLife2 == 1){
		  lives2 = lives2 - 1;
			LEDBAROutput(binaryLives(lives2));
      RIT128x96x4StringDraw("You Lose A Life", 5,00,15);
			RIT128x96x4StringDraw("Pass It!", 5,75,15);
		  flashColor('R','R');
	    sysTickWait1mS(2000);
	  }
	  else{
		  RIT128x96x4StringDraw("Round Complete!", 5,00,15);
	    sysTickWait1mS(2000);
	  }
    
		// If both players completed round, whoever did it slower loses a life
    if(loseLife1 != 1 && loseLife2 != 1 && loseLife1 > loseLife2){
			lives1 = lives1 - 1;
			RIT128x96x4StringDraw("P1 Loses A Life", 5,00,15);
	    sysTickWait1mS(1000);
		}
		if(loseLife1 != 1 && loseLife2 != 1 && loseLife2 > loseLife1){
			lives2 = lives2 - 1;
		  RIT128x96x4StringDraw("P2 Loses A Life", 5,00,15);
	    sysTickWait1mS(1000);
		}
    
    // Go on to next round
		roundCount++;
		
  } //end of while() loop
	
		RIT128x96x4Clear();

	// If player 2 ran out of lives, or player 1 had more after 10 rounds
	// Player 1 wins
	if (lives1 > lives2){
		RIT128x96x4StringDraw("Player One Wins!", 5,00,15);
	  flashColor('G','B');
	}
	// If player 1 ran out of lives, or player 2 had more after 10 rounds
	// Player 2 wins
	else if (lives2 > lives1){
		RIT128x96x4StringDraw("Player Two Wins!", 5,00,15);
	  flashColor('B','G');
	}
	// Same lives after 10 rounds or both lost lives on same round
	else {
		RIT128x96x4StringDraw("You Tied!", 5,00,15);
		flashColor('R','B');
		flashColor('G','R');
	}
	
	  // Game is over, option to play again
		sysTickWait1mS(5000);
	  RIT128x96x4Clear();
		RIT128x96x4StringDraw("Play Again?!", 5,00,15);
		RIT128x96x4StringDraw("Press Reset --> ", 30,50,15);	  
    sysTickWait1mS(100);
    
}  	// end of main



// Round functionality, picks random instruction, waits for user intput, if correct next instruction,
// if incorrect player loses life
int startRound(){
	int loseLife = 0;
	int inCount = 10;
	int instruction;
	int pot;
	int switches;
	int time = 0;
	
	while(inCount > 0 && !loseLife){
			 
			 instruction =  rand();
		   instruction = instruction % 3;
			
			 // Bop it
       if (instruction == 0){
				 RIT128x96x4StringDraw("Bop It!", 5,00,15);
				 pot = readPotentiometer0();
				 switches = read_Switches();
				 
				 // If they Twist or Switch, lose life
				 while(read_PBSwitchNum(1) && !loseLife){
					 
           if ((readPotentiometer0() <= .1 * pot) || read_Switches() != switches ){
						 loseLife = 1;
					 }
					 time++;
				 }
		   	 RIT128x96x4Clear();
				 sysTickWait1mS(250);
			 }
			 
			 // Twist it
			 if (instruction == 1){
			   RIT128x96x4StringDraw("Twist It!", 5,00,15);
				 pot = readPotentiometer0();
				 switches = read_Switches();
				 
				 // If they Bop or Switch, lose life
				 while(readPotentiometer0() >= .5 * pot && !loseLife && (readPotentiometer0() <= 1.5 * pot) ) {
				   if ((read_Switches() != switches) || (!read_PBSwitchNum(1))){
					   loseLife = 1;
				   }
					  time++;
				 }
			 	 RIT128x96x4Clear();
				 sysTickWait1mS(250);
			 }

			 // Switch it
			 if (instruction == 2){
  		   RIT128x96x4StringDraw("Switch It!", 5,00,15);
				 pot = readPotentiometer0();
				 switches = read_Switches();
				 
				 // If they Bop or Twist, lose life
				 while(read_Switches() == switches && !loseLife){
				   if ((readPotentiometer0() <= .1 * pot) || (!read_PBSwitchNum(1))){
						 loseLife = 1;
					 }
					 time++;
				 }
				 RIT128x96x4Clear();
				 sysTickWait1mS(250);		 
			 }
			 inCount--;
		 }
	
	// Lose a life
	if (loseLife == 1){
	 return loseLife;
	}
	// If no mistakes, return the time it took them
	return time;
}


// Function to countdown whoevers turn it is
void	  playerCountdown(int player){
	
	  if (player == 1){
    RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 1 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("3", 50,25,15);
    sysTickWait1mS(1000);
		
		RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 1 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("2", 50,25,15);
    sysTickWait1mS(1000);
		
		RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 1 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("1", 50,25,15);
    sysTickWait1mS(1000);
		
	  RIT128x96x4Clear();
    }
	
	  if (player == 2){
	  RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 2 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("3", 50,25,15);
    sysTickWait1mS(1000);
		
		RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 2 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("2", 50,25,15);
    sysTickWait1mS(1000);

		
		RIT128x96x4Clear();
	  RIT128x96x4StringDraw("Player 2 Begins In:", 5,00,15);
    RIT128x96x4StringDraw("1", 50,25,15);
    sysTickWait1mS(1000);
		
	  RIT128x96x4Clear();
		}
		
		return;
}

// Function to flash the LED
void flashColor(char color, char color2){
  
	turnOn(color);
	sysTickWait1mS(250);
	turnOff(color);
	sysTickWait1mS(250);

	turnOn(color2);
	sysTickWait1mS(250);
	turnOff(color2);
	sysTickWait1mS(250);

	turnOn(color);
	sysTickWait1mS(250);
	turnOff(color);
}

// Function to convert number of lives to correct binary for LEDBar
int binaryLives(int lives){

	if (lives == 0){
		return 0;
	}
	if (lives == 1){
		return 1;
	}
	if (lives == 2){
		return 3;
	}
  if (lives == 3){
		return 7;
	}
	if (lives == 4) {
		return 15;
	}
	if (lives == 5) {
		return 31;
	}
	return 0;
}



