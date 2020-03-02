#include <stdlib.h> 
#include "myLib.h"
#include "game.h"
#include "Asteroid.h"

// Variables
USER user;
BULLET bullets[BULLETCOUNT];
ASTEROID asteroids[ASTEROIDCOUNT];
int asteroidsRemaining;
int livesRemaining;
int timeRemaining;
int time;


//SAFE Box location and dimensions
int safeCol = SCREENWIDTH - 10;
int safeRow = SCREENHEIGHT - 20;
int safeHeight = 20;
int safeWidth = 10;


// where all items are intiliazed
void initializeGame(){

    
    initializeUser();
    initializeBullets();
    initializeAsteroids();
    asteroidsRemaining = ASTEROIDCOUNT;
    livesRemaining = LIVESCOUNT;
    timeRemaining = TIMEREMAINING;
    time = TIME;

    DMANow(3,AsteroidPal,PALETTE,AsteroidPalLen);
    // Load the custom game colors to the end
    unsigned short colors[NUMCOLORS] = {BLACK, BLUE, GREEN, RED, WHITE, GRAY};
    for (int i = 0; i < NUMCOLORS; i++) {
        PALETTE[256-NUMCOLORS+i] = colors[i];
    }

}

//Updates: User,falling rectanlges, checks for boundry
// Updates the game each frame
void updateGame(){

    updateUser();
    updateBoundry();

    time--;
    if(time % 100 == 0){
        timeRemaining--;
    }


    //Updates all Bullets
    for( int i = 0; i < BULLETCOUNT; i++ ){

        updateBullet(&bullets[i]);

    }

    //Updates all asteroids
    for( int i = 0; i < ASTEROIDCOUNT; i++){

        updateAsteroid(&asteroids[i]);

    }
}

//Draws user, bullets, asteroids
void drawGame(){

    fillScreen(BLACKS);
    drawUser();

    //Draws all bullets
    for(int i = 0; i < BULLETCOUNT; i++){

        drawBullet(&bullets[i]);

    }

    //Draws all asteroids
    for(int i = 0; i < ASTEROIDCOUNT; i++){
        
        drawAsteroid(&asteroids[i]);

    }

}


//stops using controlled rectangle from going off righ,left,or bottom of screen
void updateBoundry(){

    //Left
    if (user.col < 0) { 
		user.oldCol = user.col;
		user.col = 0;
	}
    //Right
	if (user.col + user.width > SCREENWIDTH ) {
		user.oldCol = user.col;
		user.col = (SCREENWIDTH - user.width);
	}
    //Top
	if (user.row < 0) {
		user.oldRow = user.row;
		user.row = 0;
	}
    //Bottom
	if (user.row + user.height > SCREENHEIGHT ) {
		user.oldRow = user.row;
		user.row = (SCREENHEIGHT - user.height);
	}


}

//Object pooling for user
void initializeUser(){

    user.row = SCREENHEIGHT - user.height;
    user.col = SCREENWIDTH / 2;
    user.udel = 1;
    user.height = 10;
    user.width = 5;
    user.color = REDS;
    user.bulletTimer = 20;

}

//functionality to move user controlled rectangle
void updateUser(){
    
    //To move right
    if((BUTTON_HELD(BUTTON_RIGHT)) || (BUTTON_PRESSED(BUTTON_RIGHT))){
        
        user.col += user.udel;

    }

    //To move left
    if((BUTTON_HELD(BUTTON_LEFT)) || (BUTTON_PRESSED(BUTTON_LEFT))){
        
        user.col -= user.udel;
    
    }

    //To move down
    if((BUTTON_HELD(BUTTON_DOWN)) || (BUTTON_PRESSED(BUTTON_DOWN))){

        user.row += user.udel;
        
    }

    //To move up
    if((BUTTON_HELD(BUTTON_UP)) || (BUTTON_PRESSED(BUTTON_UP))){

        user.row -= user.udel;

    }

    //To fire bullets by pressing B
    if((BUTTON_PRESSED(BUTTON_B)) && (user.bulletTimer >= 20)){

        fireBullet();
        user.bulletTimer = 0;

    }

    user.bulletTimer++;
    //Checks all asteroids for collision with user 
    for(int i = 0; i < ASTEROIDCOUNT; i++){

        if(collision(user.col, user.row, user.width, user.height,asteroids[i].col, asteroids[i].row, asteroids[i].width, asteroids[i].height)){

            //game is over      
            livesRemaining--;

        }
    }

}

//Clears previous frame and draws user and safe 
void drawUser(){

    drawRect(user.col, user.row, user.width, user.height, REDS);

}

//Pooling for bullets
void initializeBullets(){

    for(int i = 0; i < BULLETCOUNT; i++) {

        bullets[i].height = 2;
        bullets[i].width = 1;
        bullets[i].col = 0;
        bullets[i].row = -2;
        bullets[i].rdel = -2;
		bullets[i].color = WHITES;
        bullets[i].active = 0;
        

    }

}

//Sets bullet to active and fires it from user
//Finds first inactive bullet and initialize/set it active
void fireBullet(){

    for(int i = 0; i < BULLETCOUNT; i++){

        if(bullets[i].active == 0){

            bullets[i].row = user.row;
            bullets[i].col = user.col + user.width / 2 + bullets[i].width / 2;
            bullets[i].active = 1;
            break;

        }

    }

}

//Draws bullet by calling drawRect and utilzes a pointer thats passed in
void drawBullet(BULLET *b){

    if(b -> active){
        
        drawRect( b-> col, b -> row, b -> width, b -> height, b -> color);

    }

}

//Pooling for asteroids
void initializeAsteroids(){

    for (int i = 0; i < ASTEROIDCOUNT; i++) {

		asteroids[i].row = rand() % 110;
		asteroids[i].col = rand() % 130 + 10;
		asteroids[i].rdel = 1;
		asteroids[i].cdel = 1;
        asteroids[i].active = 1;
        
        if(i == 1){

            asteroids[i].isAsteroid = 1;
    
        }

        if(asteroids[i].isAsteroid){

            asteroids[i].height = 32;
            asteroids[i].width = 40;

        }else{

            asteroids[i].height = 10;
		    asteroids[i].width = 10;

        }
        


        
	}


}

//Takes in pointer and bounces asteroids from edges
void updateAsteroid(ASTEROID* a){

    if( a -> active){

        if (a->row <= 0 || a->row + a->height-1 >= SCREENHEIGHT - 1){
			
            a->rdel *= -1;
    
        }
		if (a->col <= 0 || a->col + a->width-1 >= SCREENWIDTH - 1){
			
            a->cdel *= -1;

        }

        
        a->row += a->rdel;
		a->col += a->cdel;

        //Iterates through bullets and checks collision with passed in asteroid pointer
        for(int i = 0; i < BULLETCOUNT; i++){

			if(bullets[i].active == 1){

				if(collision(bullets[i].col, bullets[i].row, bullets[i].width, bullets[i].height,a->col, a->row, a->width, a->height)){
					
                    bullets[i].active = 0;
					a->active = 0;
                    
					asteroidsRemaining--;
				}

			}
			
		}

    }

}

// Takes in pointer and calls drawRect if it is active
void drawAsteroid(ASTEROID* a) {
	if(a->active) {
        
        if(a-> isAsteroid){

            drawImage(a->col, a->row, a->width, a->height, AsteroidBitmap);

        }
        else{

		    drawRect(a->col, a->row, a->width, a->height, BLUES);

        }

	}

}

// Takes in pointer and if its active then moves it up and updates bullet
void updateBullet(BULLET* b) {

	if (b->active) {

		b->row += b->rdel;

		if (b->row < 0){

			b->active = 0;

		}
	}
}