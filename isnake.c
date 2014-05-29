/*
 * Intelligent SNAKE
 *
 *   Description: A simple snake clone written in C and SDL
 *   Author: Kevin MacMartin
 *   E-Mail: prurigro@gmail.com
 *
 * This project is licensed under the the MIT License (MIT):
 *
 * Copyright (c) 2014 Kevin MacMartin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * This projected is licensed under the terms of the MIT license
 *
*/

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

/*TITLE OF THE WINDOW*/
#define GAMENAME "Intelligent Snake"

/*FONT AND FONTSIZE*/
#define DEFAULT_FONT_FILE "DroidSans-Bold.ttf"
#define DEFAULT_FONT_SIZE 22

/*HEIGHT AND WIDTH OF EACH TILE*/
#define TILEWIDTH 20
#define TILEHEIGHT 20

/*MIN+MAX+DEFAULT TILE ROWS AND COLUMNS*/
#define MIN_TILESWIDE 30
#define MIN_TILESHIGH 20
#define MAX_TILESWIDE 80
#define MAX_TILESHIGH 50
#define DEFAULT_TILESWIDE 50
#define DEFAULT_TILESHIGH 30

/*MIN+MAX+DEFAULT SNAKESPEED*/
#define MIN_SNAKESPEED 1
#define MAX_SNAKESPEED 9
#define DEFAULT_SNAKESPEED 1

/*SNAKE LENGTH (+1 FOR BUFFER)*/
#define MIN_SNAKELENGTH 4
#define MAX_SNAKELENGTH 36
#define DEFAULT_SNAKELENGTH 4

/*NUMBER OF NPCs (# OF BLOCKS + 1 FOR FOOD)*/
#define MIN_NPCCOUNT 1
#define MAX_NPCCOUNT 41
#define DEFAULT_NPCCOUNT 21

/*AMOUNT OF FOOD EATEN BEFORE SNAKESPEED INCREASES*/
#define ACCEL_FREQ 3

/*WHETHER TO DISPLAY COMMANDLINE OUTPUT DURING GAMEPLAY*/
#define CONSOLE_OUTPUT true

/*CONSTANT VALUES*/
const int colourFood[3] = {255, 0, 0}; //Bright Red
const int colourHead[3] = {215, 95, 95}; //Red
const int colourBody[3] = {135, 215, 255}; //Blue
const int colourTail[3] = {255, 215, 135}; //Yellow
const int colourBlock[3] = {234, 234, 234}; //White
const int colourTiles[3] = {38, 38, 38}; //Dark Grey
const int colourBackground[3] = {48, 48, 48}; //Light Grey
const int colourTextGameOver[3] = {234, 234, 234}; //White
const int colourTextLabel[3] = {215, 95, 95}; //Red
const int colourTextData[3] = {135, 215, 255}; //Blue

/*ENUMERATIONS FOR MORE READABLE CODE*/
enum direction {Up, Down, Left, Right}; //Movement directions
enum gameParams {QuitGame, TilesHigh, TilesWide, NPCCount, SnakeSpeed, SnakeLength, SnakeDirection, SnakeScore};

/*GAME FUNCTIONS*/
void gameLoop(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], SDL_Event* event);
int moveSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], enum direction newDirection);
bool collisionDetect(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], enum direction newDirection);
void collisionDetectFood(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8]);
bool gameEventPoll(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], SDL_Event* event);
bool scrollSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8]);
void updateSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8]);
void loadNPCs(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8]);
void randomLocation(int** ppSprites, int (*gameParameters)[8], int* location[2]);
void drawText(SDL_Surface* screen, char* string, int size, int x, int y, SDL_Colour colour);
void updateRect(SDL_Surface* screen, SDL_Rect** ppTiles, int position[2], const int colour[3]);

/*COMMANDLINE FUNCTIONS*/
void configureGame(int argc, char** args, int (*gameParameters)[8]);
void printHelpMenu(char filename[]);
void printErrorHelp(char filename[]);

/*MAIN LOOP*/
int main(int argc, char* args[])
{
    int x, y, gameParameters[8];
    int** ppSprites = NULL;
    int* pSprites = NULL;
    SDL_Surface* screen = NULL;
    SDL_Rect** ppTiles = NULL;
    SDL_Rect* pTiles = NULL;
    SDL_Event event;

    //START RANDOM SEED TO HELP MAKE RANDOM NUMBERS MORE RANDOM
    srand(time(NULL));

    //CONFIGURE GAME SETTINGS USING DEFAULTS AND USER INPUT
    configureGame(argc, args, &gameParameters);

    //INITIALIZE SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);

    //INITIALIZE SDL_ttf
    if (TTF_Init() != 0)
    {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(TTF_Quit);

    //INITIALIZE THE MAIN SURFACE
    if ((screen = SDL_SetVideoMode((TILEWIDTH * gameParameters[TilesWide]), (TILEHEIGHT * gameParameters[TilesHigh]) + 35, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT)) == NULL)
    {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //SET THE INPUT QUEUE TO IGNORE INPUT VIA MOUSE MOVEMENT
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

    //SET WINDOW TITLE
    SDL_WM_SetCaption(GAMENAME, NULL);

    //SET BACKGROUND COLOUR
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, colourBackground[0], colourBackground[1], colourBackground[2]));

    while (gameParameters[QuitGame] == 0)
    {
        //INIT TILES ARRAY
        pTiles = malloc(gameParameters[TilesWide] * sizeof(SDL_Rect));
        ppTiles = malloc(gameParameters[TilesHigh] * sizeof(SDL_Rect*));
        for (x = 0; x < gameParameters[TilesHigh]; x++)
            ppTiles[x] = malloc(gameParameters[TilesWide] * sizeof(SDL_Rect));
        for (x = 0; x < gameParameters[TilesHigh]; x++)
            for (y = 0; y < gameParameters[TilesWide]; y++)
            {
                ppTiles[x][y].w = TILEWIDTH - (TILEWIDTH / 10);
                ppTiles[x][y].h = TILEHEIGHT - (TILEHEIGHT / 10);
                ppTiles[x][y].x = ((ppTiles[x][y].w + (TILEWIDTH / 10)) * y);
                ppTiles[x][y].y = ((ppTiles[x][y].h + (TILEHEIGHT / 10)) * x);
                SDL_FillRect(screen, &ppTiles[x][y], SDL_MapRGB(screen->format, colourTiles[0], colourTiles[1], colourTiles[2]));
            }

        //INIT SPRITES ARRAY
        pSprites = malloc(2 * sizeof(int));
        ppSprites = malloc((gameParameters[NPCCount] + MAX_SNAKELENGTH) * sizeof(int*)); //The array is defined by the snake's maximum size so it can grow during gameplay
        for (x = 0; x < gameParameters[NPCCount] + MAX_SNAKELENGTH; x++)
        {
            ppSprites[x] = malloc(2 * sizeof(int));
            ppSprites[x][0] = 0;
            ppSprites[x][1] = 0;
        }

        //SET SNAKE START POSITION
        for (x = gameParameters[NPCCount]; x < gameParameters[NPCCount] + gameParameters[SnakeLength]; x++)
        {
            ppSprites[x][0] = 3;
            ppSprites[x][1] = (3 + gameParameters[NPCCount] + gameParameters[SnakeLength] - DEFAULT_SNAKELENGTH + 2 - x);
        }

        //SET NPC LOCATIONS
        for (x = 0; x < gameParameters[NPCCount]; x++)
            randomLocation(ppSprites, &gameParameters, &(ppSprites)[x]);

        //GAME LOOP
        gameLoop(screen, ppTiles, ppSprites, &gameParameters, &event);

        //FREE MEMORY AND RESET POINTERS
        free(ppTiles);
        free(ppSprites);
        ppSprites = NULL;
        pSprites = NULL;

        //RESET GAME SETTINGS USING DEFAULTS AND USER INPUT
        if (gameParameters[QuitGame] == 0) configureGame(argc, args, &gameParameters);
    }

    //FREEING THE SCREEN SURFACE BEFORE EXITING
    SDL_FreeSurface(screen);

    //EXIT THE PROGRAM
    exit(EXIT_SUCCESS);
}

/*GAME LOOP*/
void gameLoop(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], SDL_Event* event)
{
    SDL_Colour SDL_ColourBackground = {colourBackground[0], colourBackground[1], colourBackground[2]};
    SDL_Colour SDL_ColourTextLabel = {colourTextLabel[0], colourTextLabel[1], colourTextLabel[2]};
    SDL_Colour SDL_ColourTextData = {colourTextData[0], colourTextData[1], colourTextData[2]};
    SDL_Colour SDL_ColourTextGameOver = {colourTextGameOver[0], colourTextGameOver[1], colourTextGameOver[2]};
    int scoreLabelPosition[2] = {15, (TILEHEIGHT * (*gameParameters)[TilesHigh]) + 4};
    int scoreDataPosition[2] = {95, (TILEHEIGHT * (*gameParameters)[TilesHigh]) + 4};
    int speedLabelPosition[2] = {(TILEWIDTH * (*gameParameters)[TilesWide]) - 100, (TILEHEIGHT * (*gameParameters)[TilesHigh]) + 4};
    int speedDataPosition[2] = {(TILEWIDTH * (*gameParameters)[TilesWide]) - 25, (TILEHEIGHT * (*gameParameters)[TilesHigh]) + 4};
    int gameOverMsgPosition[5] = {(TILEWIDTH * ((*gameParameters)[TilesWide] / 2)) - 147, (TILEWIDTH * ((*gameParameters)[TilesWide] / 2)) - 50, (TILEWIDTH * ((*gameParameters)[TilesWide] / 2)) + 61, (TILEWIDTH * ((*gameParameters)[TilesWide] / 2)) + 82, (TILEHEIGHT * (*gameParameters)[TilesHigh]) + 9};
    char* gameOverMsg[4] = {"GAME OVER", "SPACE to RESTART", " or ", "ESC to QUIT"};
    char tempString[2][3] = {"-1", "-1"};

    //LOAD BLOCKS AND FOOD
    loadNPCs(screen, ppTiles, ppSprites, gameParameters);

    //DRAW LABELS FOR SCORE AND SPEED RESPECTIVELY
    drawText(screen, "SCORE", DEFAULT_FONT_SIZE, scoreLabelPosition[0], scoreLabelPosition[1], SDL_ColourTextLabel);
    drawText(screen, "SPEED", DEFAULT_FONT_SIZE, speedLabelPosition[0], speedLabelPosition[1], SDL_ColourTextLabel);

    //LOOP UNTIL GAME IS FINISHED
    while (1)
    {
        //UPDATE THE SNAKE'S SCORE WHEN IT CHANGES
        if (atoi(tempString[0]) != (*gameParameters)[SnakeScore])
        {
            if (atoi(tempString[0]) != -1)
                drawText(screen, tempString[0], DEFAULT_FONT_SIZE, scoreDataPosition[0], scoreDataPosition[1], SDL_ColourBackground);
            sprintf(tempString[0], "%d", (*gameParameters)[SnakeScore]);
            drawText(screen, tempString[0], DEFAULT_FONT_SIZE, scoreDataPosition[0], scoreDataPosition[1], SDL_ColourTextData);
        }
        //UPDATE THE SNAKE'S SPEED WHEN IT CHANGES
        if (atoi(tempString[1]) != (*gameParameters)[SnakeSpeed])
        {
            if (atoi(tempString[1]) != -1)
                drawText(screen, tempString[1], DEFAULT_FONT_SIZE, speedDataPosition[0], speedDataPosition[1], SDL_ColourBackground);
            sprintf(tempString[1], "%d", (*gameParameters)[SnakeSpeed]);
            drawText(screen, tempString[1], DEFAULT_FONT_SIZE, speedDataPosition[0], speedDataPosition[1], SDL_ColourTextData);
        }

        if (gameEventPoll(screen, ppTiles, ppSprites, gameParameters, event) == false) break;
        updateSnake(screen, ppTiles, ppSprites, gameParameters);
        SDL_Flip(screen);

        SDL_Delay(250 / ((*gameParameters)[SnakeSpeed] + 3));

        if (scrollSnake(screen, ppTiles, ppSprites, gameParameters) == false) break;
        updateSnake(screen, ppTiles, ppSprites, gameParameters);
        SDL_Flip(screen);

        SDL_Delay(250 / ((*gameParameters)[SnakeSpeed] + 3));

        if (gameEventPoll(screen, ppTiles, ppSprites, gameParameters, event) == false) break;
        updateSnake(screen, ppTiles, ppSprites, gameParameters);
        SDL_Flip(screen);
    }

    //DISPLAY GAME OVER MESSAGE AND WAIT FOR INPUT
    if ((*gameParameters)[QuitGame] == 0)
    {
        //DISPLAY GAMEOVER MESSAGES
        drawText(screen, gameOverMsg[0], DEFAULT_FONT_SIZE - 7, gameOverMsgPosition[0], gameOverMsgPosition[4], SDL_ColourTextGameOver);
        drawText(screen, gameOverMsg[1], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[1], gameOverMsgPosition[4], SDL_ColourTextData);
        drawText(screen, gameOverMsg[2], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[2], gameOverMsgPosition[4], SDL_ColourTextGameOver);
        drawText(screen, gameOverMsg[3], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[3], gameOverMsgPosition[4], SDL_ColourTextData);
        SDL_Flip(screen);

        //WAIT A MOMENT TO ENSURE INPUT FROM THE GAME ISN'T CAUGHT
        SDL_Delay(250 / ((*gameParameters)[SnakeSpeed] + 3));

        //CAPTURE SDL_QUIT TO EXIT, ESCAPE TO EXIT, OR SPACEBAR TO RESTART
        while (1)
        {
            if (SDL_PollEvent(event))
            {
                if ((*event).type == SDL_QUIT)
                {
                    (*gameParameters)[QuitGame] = 1;
                    break;
                }
                else if ((*event).type == SDL_KEYDOWN)
                {
                    if (((*event).key.keysym.sym == SDLK_ESCAPE) || ((*event).key.keysym.sym == SDLK_q))
                    {
                        (*gameParameters)[QuitGame] = 1;
                        break;
                    }
                    else if (((*event).key.keysym.sym == SDLK_SPACE) || ((*event).key.keysym.sym == SDLK_RETURN)) break;
                }
            }
        }
        drawText(screen, gameOverMsg[0], DEFAULT_FONT_SIZE - 7, gameOverMsgPosition[0], gameOverMsgPosition[4], SDL_ColourBackground);
        drawText(screen, gameOverMsg[1], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[1], gameOverMsgPosition[4], SDL_ColourBackground);
        drawText(screen, gameOverMsg[2], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[2], gameOverMsgPosition[4], SDL_ColourBackground);
        drawText(screen, gameOverMsg[3], DEFAULT_FONT_SIZE - 9, gameOverMsgPosition[3], gameOverMsgPosition[4], SDL_ColourBackground);
        SDL_Flip(screen);
    }

    drawText(screen, "SCORE", DEFAULT_FONT_SIZE, scoreLabelPosition[0], scoreLabelPosition[1], SDL_ColourBackground);
    drawText(screen, "SPEED", DEFAULT_FONT_SIZE, speedLabelPosition[0], speedLabelPosition[1], SDL_ColourBackground);
    if (atoi(tempString[0]) != -1)
        drawText(screen, tempString[0], DEFAULT_FONT_SIZE, scoreDataPosition[0], scoreDataPosition[1], SDL_ColourBackground);
    if (atoi(tempString[1]) != -1)
        drawText(screen, tempString[1], DEFAULT_FONT_SIZE, speedDataPosition[0], speedDataPosition[1], SDL_ColourBackground);
}

/*MOVES THE SNAKE IN THE DESIRED DIRECTION*/
int moveSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], enum direction newDirection)
{
    int x;

    //MOVEMENT
    for (x = (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 1; x >= (*gameParameters)[NPCCount]; x--)
    {
        if (x == (*gameParameters)[NPCCount])
        {
            //MOVE THE HEAD OF THE SNAKE TO THE NEW DIRECTION
            switch (newDirection)
            {
            case Up:
                ppSprites[x][0] = ppSprites[x][0] - 1;
                (*gameParameters)[SnakeDirection] = 0;
                break;
            case Down:
                ppSprites[x][0] = ppSprites[x][0] + 1;
                (*gameParameters)[SnakeDirection] = 1;
                break;
            case Left:
                ppSprites[x][1] = ppSprites[x][1] - 1;
                (*gameParameters)[SnakeDirection] = 2;
                break;
            case Right:
                ppSprites[x][1] = ppSprites[x][1] + 1;
                (*gameParameters)[SnakeDirection] = 3;
                break;
            }
        }
        else
        {
            //MOVE THE BODY/TAIL OF THE SNAKE TO THE PIECE AHEAD OF IT
            ppSprites[x][0] = ppSprites[x - 1][0];
            ppSprites[x][1] = ppSprites[x - 1][1];
        }
    }
}

/*DETECT+HANDLE WHEN THE SNAKE COLLIDES WITH WALLS, BLOCKS, ITSELF OR FOOD*/
bool collisionDetect(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], enum direction newDirection)
{
    int x;

    switch (newDirection)
    {
    case Up:
        for (x = 0; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 2; ++x)
        {
            if (((ppSprites[(*gameParameters)[NPCCount]][0] - 1) == ppSprites[x][0]) && (ppSprites[(*gameParameters)[NPCCount]][1] == ppSprites[x][1]))
            {
                if (x == 0) collisionDetectFood(screen, ppTiles, ppSprites, gameParameters);
                else return false;
            }
        }
        if ((ppSprites[(*gameParameters)[NPCCount]][0] - 1) < 0) return false;
        moveSnake(screen, ppTiles, ppSprites, gameParameters, Up);
        break;
    case Down:
        for (x = 0; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 2; ++x)
        {
            if (((ppSprites[(*gameParameters)[NPCCount]][0] + 1) == ppSprites[x][0]) && (ppSprites[(*gameParameters)[NPCCount]][1] == ppSprites[x][1]))
            {
                if (x == 0) collisionDetectFood(screen, ppTiles, ppSprites, gameParameters);
                else return false;
            }
        }
        if ((ppSprites[(*gameParameters)[NPCCount]][0] + 1) >= (*gameParameters)[TilesHigh]) return false;
        moveSnake(screen, ppTiles, ppSprites, gameParameters, Down);
        break;
    case Left:
        for (x = 0; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 2; ++x)
        {
            if (((ppSprites[(*gameParameters)[NPCCount]][1] - 1) == ppSprites[x][1]) && (ppSprites[(*gameParameters)[NPCCount]][0] == ppSprites[x][0]))
            {
                if (x == 0) collisionDetectFood(screen, ppTiles, ppSprites, gameParameters);
                else return false;
            }
        }
        if ((ppSprites[(*gameParameters)[NPCCount]][1] - 1) < 0) return false;;
        moveSnake(screen, ppTiles, ppSprites, gameParameters, Left);
        break;
    case Right:
        for (x = 0; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 2; ++x)
        {
            if (((ppSprites[(*gameParameters)[NPCCount]][1] + 1) == ppSprites[x][1]) && (ppSprites[(*gameParameters)[NPCCount]][0] == ppSprites[x][0]))
            {
                if (x == 0) collisionDetectFood(screen, ppTiles, ppSprites, gameParameters);
                else return false;
            }
        }
        if ((ppSprites[(*gameParameters)[NPCCount]][1] + 1) >= (*gameParameters)[TilesWide]) return false;;
        moveSnake(screen, ppTiles, ppSprites, gameParameters, Right);
        break;
    }
    return true;
}

/*HANDLES COLLISION WITH FOOD*/
void collisionDetectFood(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8])
{
    //INCREASE THE SNAKE'S SIZE IF IT'S NOT ALREADY THE MAXIMUM
    if ((*gameParameters)[SnakeLength] < MAX_SNAKELENGTH) (*gameParameters)[SnakeLength]++;

    //INCREASE THE SNAKE'S SPEED WHEN THE SCORE IS DIVISIBLE BY ACCEL_FREQ
    if ((((*gameParameters)[SnakeScore] % ACCEL_FREQ) == 0) && ((*gameParameters)[SnakeScore] != 0) && ((*gameParameters)[SnakeSpeed] < MAX_SNAKESPEED)) (*gameParameters)[SnakeSpeed]++;

    //INCREASE THE SNAKE'S SCORE
    (*gameParameters)[SnakeScore]++;

    //SET FOOD PIECE IN NEW LOCATION
    randomLocation(ppSprites, gameParameters, &(ppSprites)[0]);
    updateRect(screen, ppTiles, ppSprites[0], colourFood);

    SDL_Flip(screen);
}

/*CAPTURES INPUT AND GENERATES APPROPRIATE RESPONSE*/
bool gameEventPoll(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8], SDL_Event* event)
{
    bool playerAlive = true;

    if (SDL_PollEvent(event))
    {
        switch ((*event).type)
        {
        case SDL_QUIT:
            playerAlive = false;
            (*gameParameters)[QuitGame] = 1;
            break;
        case SDL_KEYDOWN:
            //WHEN KEY IS PRESSED, CHECK WHICH AND RESPOND ACCORDINGLY
            switch ((*event).key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
            case SDLK_k:
                if (((*gameParameters)[SnakeDirection] != Up) && ((*gameParameters)[SnakeDirection] != Down))
                    playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Up));
                break;
            case SDLK_DOWN:
            case SDLK_s:
            case SDLK_j:
                if (((*gameParameters)[SnakeDirection] != Down) && ((*gameParameters)[SnakeDirection] != Up))
                    playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Down));
                break;
            case SDLK_LEFT:
            case SDLK_a:
            case SDLK_h:
                if (((*gameParameters)[SnakeDirection] != Left) && ((*gameParameters)[SnakeDirection] != Right) && ((*gameParameters)[SnakeDirection] != -1))
                    playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Left));
                break;
            case SDLK_RIGHT:
            case SDLK_d:
            case SDLK_l:
                if (((*gameParameters)[SnakeDirection] != Right) && ((*gameParameters)[SnakeDirection] != Left))
                    playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Right));
                break;
            case SDLK_ESCAPE:
            case SDLK_q:
                playerAlive = false;
                (*gameParameters)[QuitGame] = 1;
                break;
            }
            break;
        }
    }
    return playerAlive;
}

/*MOVES SNAKE AUTOMATICALLY IN WHICHEVER DIRECTION IT WENT LAST*/
bool scrollSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8])
{
    bool playerAlive = true;

    switch ((*gameParameters)[SnakeDirection])
    {
    case Up:
        playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Up));
        break;
    case Down:
        playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Down));
        break;
    case Left:
        playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Left));
        break;
    case Right:
        playerAlive = (collisionDetect(screen, ppTiles, ppSprites, gameParameters, Right));
        break;
    default:
        break;
    }
    return playerAlive;
}

/*REDRAW THE SNAKE BASED ON CURRENT VALUES*/
void updateSnake(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8])
{
    int x;

    for (x = (*gameParameters)[NPCCount]; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength]; x++)
    {
        if (x == (*gameParameters)[NPCCount]) updateRect(screen, ppTiles, ppSprites[x], colourHead);
        else if (x == ((*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 1)) updateRect(screen, ppTiles, ppSprites[x], colourTiles);
        else if (x == ((*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength] - 2)) updateRect(screen, ppTiles, ppSprites[x], colourTail);
        else updateRect(screen, ppTiles, ppSprites[x], colourBody);
    }
}

/*DRAW NPCs BASED ON ON CURRENT VALUES*/
void loadNPCs(SDL_Surface* screen, SDL_Rect** ppTiles, int** ppSprites, int (*gameParameters)[8])
{
    int x, startNPCs = 0;

    for (x = startNPCs; x < (*gameParameters)[NPCCount]; x++)
    {
        if (x == startNPCs) updateRect(screen, ppTiles, ppSprites[x], colourFood);
        else updateRect(screen, ppTiles, ppSprites[x], colourBlock);
    }
}

/*A HELPER FUNCTION TO RANDOMLY PLACE NPCs WITH SOME INTELLIGENCE*/
void randomLocation(int** ppSprites, int (*gameParameters)[8], int* location[2])
{
    int x, randLocation[2];
    bool isAcceptable = false;

    while (!isAcceptable)
    {
        isAcceptable = true;
        randLocation[0] = (rand() % ((*gameParameters)[TilesHigh] - 2)) + 1;
        randLocation[1] = (rand() % ((*gameParameters)[TilesWide] - 2)) + 1;

        //DON'T LOAD NPCs ONTO SNAKE OR OTHER NPCs
        for (x = 0; x < (*gameParameters)[NPCCount] + (*gameParameters)[SnakeLength]; ++x)
            if (((ppSprites[x][0]) == randLocation[0]) && ((ppSprites[x][1]) == randLocation[1])) isAcceptable = false;

        //DON'T LOAD NPCs DIRECTLY NEXT TO THE SNAKE'S HEAD
        if (((((ppSprites[(*gameParameters)[NPCCount]][0]) - 1) == randLocation[0]) && (((ppSprites[(*gameParameters)[NPCCount]][1])) == randLocation[1])) || //ABOVE THE SNAKE'S HEAD
                ((((ppSprites[(*gameParameters)[NPCCount]][0]) + 1) == randLocation[0]) && (((ppSprites[(*gameParameters)[NPCCount]][1])) == randLocation[1])) || //BELOW THE SNAKE'S HEAD
                ((((ppSprites[(*gameParameters)[NPCCount]][0])) == randLocation[0]) && (((ppSprites[(*gameParameters)[NPCCount]][1]) - 1) == randLocation[1])) || //LEFT OF THE SNAKE'S HEAD
                ((((ppSprites[(*gameParameters)[NPCCount]][0])) == randLocation[0]) && (((ppSprites[(*gameParameters)[NPCCount]][1]) + 1) == randLocation[1]))) //RIGHT OF THE SNAKE'S HEAD
            isAcceptable = false;
    }

    //SET THE GIVEN NPC'S LOCATION TO THE GENERATED COORDINATES
    (*location)[0] = randLocation[0];
    (*location)[1] = randLocation[1];
}

//DRAW TEXT WHEN REQUIRED
void drawText(SDL_Surface* screen, char* string, int size, int x, int y, SDL_Colour colour)
{
    SDL_Colour SDL_ColourBackground = {colourBackground[0], colourBackground[1], colourBackground[2]};
    SDL_Rect coordinates = {x, y};
    SDL_Surface *newString = NULL;
    TTF_Font *font;

    //LOAD THE FONT
    font = TTF_OpenFont(DEFAULT_FONT_FILE, size);
    if (!font) fprintf(stderr, "Error loading %s", DEFAULT_FONT_FILE);

    newString = TTF_RenderText_Shaded(font, string, colour, SDL_ColourBackground);
    SDL_BlitSurface(newString, NULL, screen, &coordinates);

    TTF_CloseFont(font);
    SDL_FreeSurface(newString);
}

/*LOW LEVEL FUNCTION TO BE RUN BY HIGHER LEVEL ONES FOR UPDATING TILES*/
void updateRect(SDL_Surface* screen, SDL_Rect** ppTiles, int position[2], const int colour[3])
{
    SDL_FillRect(screen, &ppTiles[position[0]][position[1]], SDL_MapRGB(screen->format, colour[0], colour[1], colour[2]));
}

/*PARSES COMMANDLINE OPTIONS AND GENERATES APPROPRIATE RESPONSE*/
void configureGame(int argc, char** args, int (*gameParameters)[8])
{
    int parsecount = 1;

    //SET DEFAULT GAME PARAMETERS
    (*gameParameters)[QuitGame] = 0;
    (*gameParameters)[TilesWide] = DEFAULT_TILESWIDE;
    (*gameParameters)[TilesHigh] = DEFAULT_TILESHIGH;
    (*gameParameters)[NPCCount] = DEFAULT_NPCCOUNT;
    (*gameParameters)[SnakeSpeed] = DEFAULT_SNAKESPEED;
    (*gameParameters)[SnakeLength] = DEFAULT_SNAKELENGTH;
    (*gameParameters)[SnakeDirection] = -1;
    (*gameParameters)[SnakeScore] = 0;

    //PARSE COMMANDLINE FOR SETTINGS
    while (parsecount < argc)
    {
        //HELP MENU
        if (strcmp(args[parsecount], "-h") == 0)
        {
            printHelpMenu(args[0]);
            exit(EXIT_SUCCESS);
        }
        //RESOLUTION
        else if (strcmp(args[parsecount], "-g") == 0)
        {
            if ((parsecount + 2) < argc)
            {
                if (((atoi(args[parsecount + 1]) >= MIN_TILESWIDE) && (atoi(args[parsecount + 1]) <= MAX_TILESWIDE)) && ((atoi(args[parsecount + 2]) >= MIN_TILESHIGH) && (atoi(args[parsecount + 2]) <= MAX_TILESHIGH)))
                {
                    (*gameParameters)[TilesWide] = atoi(args[parsecount + 1]);
                    (*gameParameters)[TilesHigh] = atoi(args[parsecount + 2]);
                    parsecount = parsecount + 3;
                }
                else
                {
                    printErrorHelp(args[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printErrorHelp(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        //NUMBER OF BLOCKS
        else if (strcmp(args[parsecount], "-b") == 0)
        {
            if ((parsecount + 1) < argc)
            {
                if ((atoi(args[parsecount + 1]) >= (MIN_NPCCOUNT - 1)) && (atoi(args[parsecount + 1]) <= (MAX_NPCCOUNT - 1)))
                {
                    (*gameParameters)[NPCCount] = atoi(args[parsecount + 1]) + 1;
                    parsecount = parsecount + 2;
                }
                else
                {
                    printErrorHelp(args[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printErrorHelp(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        //SNAKE'S LENGTH
        else if (strcmp(args[parsecount], "-l") == 0)
        {
            if ((parsecount + 1) < argc)
            {
                if ((atoi(args[parsecount + 1]) >= (MIN_SNAKELENGTH - 1)) && (atoi(args[parsecount + 1]) <= (MAX_SNAKELENGTH - 1)))
                {
                    (*gameParameters)[SnakeLength] = atoi(args[parsecount + 1]) + 1;
                    parsecount = parsecount + 2;
                }
                else
                {
                    printErrorHelp(args[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printErrorHelp(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        //SNAKE'S SPEED
        else if (strcmp(args[parsecount], "-s") == 0)
        {
            if ((parsecount + 1) < argc)
            {
                if ((atoi(args[parsecount + 1]) >= MIN_SNAKESPEED) && (atoi(args[parsecount + 1]) <= MAX_SNAKESPEED))
                {
                    (*gameParameters)[SnakeSpeed] = atoi(args[parsecount + 1]);
                    parsecount = parsecount + 2;
                }
                else
                {
                    printErrorHelp(args[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printErrorHelp(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        //FAIL IF ANYTHING ELSE
        else
        {
            printErrorHelp(args[0]);
            exit(EXIT_FAILURE);
        }
    }
}

/*PRINT THE HELP MENU TO THE COMMANDLINE*/
void printHelpMenu(char filename[])
{
    fprintf(stdout, "  Usage: %s [options]\n", filename);
    fprintf(stdout, "  Options:\n");
    fprintf(stdout, "    -g [width] [height]\tSet the grid size: between [%d]x[%d] and [%d]x[%d] (DEFAULT: [%d]x[%d])\n", MIN_TILESWIDE, MIN_TILESHIGH, MAX_TILESWIDE, MAX_TILESHIGH, DEFAULT_TILESWIDE, DEFAULT_TILESHIGH);
    fprintf(stdout, "    -b [blocks]\t\tSet the number of blocks: between [%d] and [%d] (DEFAULT: [%d])\n", MIN_NPCCOUNT - 1, MAX_NPCCOUNT - 1, DEFAULT_NPCCOUNT - 1);
    fprintf(stdout, "    -l [length]\t\tSet the snake's starting length: between [%d] and [%d] (DEFAULT: [%d])\n", MIN_SNAKELENGTH - 1, MAX_SNAKELENGTH - 1, DEFAULT_SNAKELENGTH - 1);
    fprintf(stdout, "    -s [speed]\t\tSet the snake's starting speed: between [%d] and [%d] (DEFAULT: [%d])\n", MIN_SNAKESPEED, MAX_SNAKESPEED, DEFAULT_SNAKESPEED);
    fprintf(stdout, "    -h\t\t\tDisplay help information\n");
}

/*DISPLAYS AN ERROR MESSAGE BEFORE CALLING THE HELP MENU FUNCTION*/
void printErrorHelp(char filename[])
{
    fprintf(stderr, "  Error: invalid input\n\n");
    printHelpMenu(filename);
}

