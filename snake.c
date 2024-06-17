#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>

//Tried to make unicode characters work so that I could get the head of the snake point to its direction.
//Will attempt again in the future.
/*
#ifndef NCURSES_WIDECHAR
#if defined(_XOPEN_SOURCE_EXTENDED) || (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE - 0 >= 500))
#define NCURSES_WIDECHAR 1
#else
#define NCURSES_WIDECHAR 0
#endif
#endif
*/

#define HEIGTH 20
#define WIDTH 40

//Structure that holds the coordinates and the address of the next snake body. Basic linked list structure.
struct linked_snake{
    int xVal;
    int yVal;
    struct linked_snake* link;
}; typedef struct linked_snake linked_snake;

//Probably didn't need this. But it looks cool. (I think.)
struct fruit{
    int xVal;
    int yVal;
    int score;
}; typedef struct fruit fruit;

//Function that returns the tail in linked_snake type. Note that the function returns the tail of the snake, not the head. 
//This is to make moving the snake a little more efficient, but also makes the code much less readable. Did this stop me from writing this mess? 
//Appearently it didn't. I am sorry for all the headache that I might've caused for those who try to read the code. I'll try to make it as readable as possible.    
linked_snake* init_snake()
{
    srand((unsigned int)time(NULL));

    //Creating the link for tail.    
    linked_snake* tail = (linked_snake*)malloc(sizeof(linked_snake));
    tail->xVal = (rand()%(int)(WIDTH*0.8))+(WIDTH*0.1); 
    tail->yVal = (rand()%(int)(HEIGTH*0.8))+(HEIGTH*0.1); 
    tail->link = (linked_snake*)malloc(sizeof(linked_snake));
    
    //Creating the body after the tail
    tail->link->xVal = tail->xVal + 1;
    tail->link->yVal = tail->yVal;
    tail->link->link = (linked_snake*)malloc(sizeof(linked_snake));
    
    //Creating the body two after the tail, head as of the beginning of the game
    tail->link->link->xVal = tail->xVal + 2;
    tail->link->link->yVal = tail->yVal;
    tail->link->link->link = NULL;

    return tail;
}





//Function to spawn a 'pear'
void spawn_pear(linked_snake* tail, fruit* pear)
{
    int x, y;
    linked_snake *traverse = tail;
    srand((unsigned int)time(NULL));
    
    x = (rand()%(int)(WIDTH*0.8))+(WIDTH*0.1);
    y = (rand()%(int)(HEIGTH*0.8))+(HEIGTH*0.1);
    
    //This while loop and the if statement inside checks if the coordinates of the newly spawned 'pear' are already occupied by the snake.
    while(traverse != NULL)
    {
        if(x == traverse->xVal && y == traverse->yVal)
        {
            traverse = tail;
            x = (rand()%(int)(WIDTH*0.8))+(WIDTH*0.1);
            y = (rand()%(int)(HEIGTH*0.8))+(HEIGTH*0.1);
        }
        traverse = traverse->link;
    }
    
    pear->xVal = x;
    pear->yVal = y;
    
    return;
}


 linked_snake* grow (linked_snake* tail)
{
    linked_snake *newTail = (linked_snake*)malloc(sizeof(linked_snake));
    if(newTail == NULL)
    {
        fprintf(stdout, "Failed to grow the body: Not enough memory in heap.");
        return tail;
    }
    //All this does is to assign the coordinates for the new tail, one 'behind' the previous tail.
    newTail->xVal = 2*tail->xVal - tail->link->xVal;
    newTail->yVal = 2*tail->yVal - tail->link->yVal;
    newTail->link = tail;

    return (newTail);
}
  

/*
                1
Directions:    2 3
                4
*/
void game_cycle(linked_snake **tail, int direction, fruit *pear, int *gameOver)
{
    linked_snake *traverse = (*tail);
    
    //Assigning the coordinates of the next body to the previous body to move the snake. 
    while(traverse->link != NULL)
    {
        traverse->xVal = traverse->link->xVal;
        traverse->yVal = traverse->link->yVal;

        traverse = traverse->link;
    }
    //After the end of the while loop the 'traverse' points to the end of the list or the head of the snake(Remember the list starts from the tail.).
    //I save the head to use for collision & 'pear detection':

    linked_snake *head = traverse;
    traverse = (*tail);

    switch(direction)
    {
        case 1:
            head->yVal--; break;
        case 2: 
            head->xVal--; break;
        case 3: 
            head->xVal++; break;
        case 4: 
            head->yVal++; break;
    }

    //Detection of whether a pear is eaten
    if(head->xVal == pear->xVal && head->yVal == pear->yVal)
    {
        spawn_pear((*tail), pear);
        (*tail) = grow((*tail));
        (*pear).score++;
    }
    
    if((head->xVal == 0 || head->xVal == WIDTH - 1 || head->yVal == 0 || head->yVal == HEIGTH -1))
    {
        (*gameOver) = 1;
        return;
    }

    while(traverse->link != NULL)
    {
        if(head->xVal == traverse->xVal && head->yVal == traverse->yVal)
        {
            (*gameOver) = 1;
            return;
        }
        traverse = traverse ->link;
    }
    

    return;
}


//Notice that if subsequent inputs are received too fast, every input will be queued. 
//This method relies on the fact that 3 different inputs will be received upon pressing an arrow key. 
//This is not a good method (I believe) so I will be changing this method to use the keypad() method.
int kbhit (void)
{
    int ch = getch();

    if (ch !=ERR)
    {
        ungetch(ch);
        return 1;
    }
    else
        return 0;
}





int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    wchar_t down[] = {2304, L'\0'};


    linked_snake *tail = init_snake();
    linked_snake *traverse = tail;
    fruit pear;
    pear.xVal = 5;
    pear.yVal = 5;
    pear.score = 0;

    int direction = 1, gameOver = 0;
    
    for(int i = 0; i < 5; i++)
        tail = grow(tail);


    while(TRUE)

    {
        if(kbhit())
        {
            if(getch() == '\033')
            {
                getch();
                switch(getch())
                {
                case 'A':
                    if(direction == 4)
                        break;
                    direction = 1; break;
                case 'D':
                    if(direction == 3)
                        break;
                    direction = 2; break;
                case 'C':
                    if(direction == 2)
                        break;
                    direction = 3; break;
                case 'B':
                    if(direction == 1)
                        break;
                    direction = 4; break;
                
                }
            }
            
        }

        //The commented sequence here prints the coordinates to the screen.
        /*
        traverse = tail;
        for(int i = 0; traverse!=NULL; i++)
        {   
            printw("Coordinate of body %d: [%d,%d]\n"
            , i+1, traverse->xVal, traverse->yVal);
            traverse = traverse->link;
        }
        printw("Coordinate for the pear: [%d,%d]\n"
        , pear.xVal, pear.yVal);
        printw("Direction: %d"
        , direction);
        
        refresh();
        napms(1500);
        */
        
        
        game_cycle(&tail, direction, &pear, &gameOver);
        if(gameOver)
            break;        


        for(int i = 0; i < WIDTH; i++)
        {
            mvwaddch(stdscr, 0, i, '#');
            mvwaddch(stdscr, HEIGTH-1, i, '#');
        }
        for(int i = 0; i < HEIGTH; i++)
        {
            mvwaddch(stdscr, i, 0, '#');
            mvwaddch(stdscr, i, WIDTH-1, '#');
        }


        traverse = tail;
        while(traverse != NULL)
        {
            mvwaddch(stdscr, traverse->yVal, traverse->xVal, 'o');
            traverse=traverse->link;
        }
    
        
        mvwaddch(stdscr, pear.yVal, pear.xVal, '+');
        char pearStr[50];
        
        sprintf(pearStr, "Your score: %d", pear.score);
        mvwaddstr(stdscr, 0, 60, "-------------------------------");
        mvwaddstr(stdscr, 1, 70, pearStr);
        mvwaddstr(stdscr, 2, 60, "-------------------------------");
        
        //Unicode characters don't work as of yet. See the top of the code for more information.
        /*
        mvaddwstr(0, 150, down);
        */


        refresh();
        napms(150);
        erase();
    }
    
    endwin();
    return (0);
}