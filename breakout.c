//
// breakout.c
//
//
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

double x_velocity;
double y_velocity;

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // wait for click before starting
    waitForClick();

    x_velocity = 2.0; // how do I make this random?
    y_velocity = 3.5;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // Scoreboard
        updateScoreboard(window, label, points);

        // move ball
        move(ball, x_velocity, y_velocity);

        pause(10);

        // check for mouse event.
        GEvent event = getNextEvent(MOUSE_EVENT);

        // Lock the paddle X to the cursor.
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
                double y = 500;
                setLocation(paddle, x, y);
            }
        }


        GObject object = detectCollision(window, ball);

        if (object != NULL)
        {
            // If the ball hits the paddle.
            if (object == paddle)
            {
                y_velocity = -y_velocity;
            }

            // If the ball hits a block. Remove block, add a point, decrement count and bounce.
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);
                y_velocity = -y_velocity;
                points++;
                bricks--;
            }
        }

        // If the ball hits the right wall.
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            x_velocity = -x_velocity;
        }

        // If the ball hits the left wall.
        if (getX(ball) <= 0)
        {
            x_velocity = -x_velocity;
        }

        // If the ball hits the top wall.
        if (getY(ball) <= 0)
        {
            y_velocity = -y_velocity;
        }

        // If the ball hits the bottom. Remove a life. Start over.
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            lives--;
            //move ball to start
            setLocation(ball, 190, 200);
            //move paddle to start
            setLocation(paddle, 160, 500);
            waitForClick();
        }

    }

    // You Lose Label Message for kicks.
    if (bricks > 0)
    {
        GLabel game_over = newGLabel("YOU LOSE!");
        setFont(game_over, "SansSerif-70");
        setColor(game_over, "RED");
        add(window, game_over);
        setLocation(game_over, 15, 300);
    }
    else
    {
        GLabel game_over = newGLabel("YOU WIN!");
        setFont(game_over, "SansSerif-70");
        setColor(game_over, "GREEN");
        add(window, game_over);
        setLocation(game_over, 15, 300);
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{

    int brick_y = 50; // initial starting Y position of the bricks.
    int row_space = 20; // the space between each line.

    for (int i = 0; i < ROWS; i++) // 5 rows of bricks.
    {
        int brick_x = 2; // initial starting X pos.
        for (int j = 0; j < COLS; j++) //10 bricks per row.
        {
            GRect brick = newGRect(brick_x + 5, brick_y, 35, 10);

            // Setting colors based on row. Not elegant.
            if (i == 0)
            {
                setColor(brick, "RED");
            }
            if (i == 1)
            {
                setColor(brick, "BLACK");
            }
            if (i == 2)
            {
                setColor(brick, "YELLOW");
            }
            if (i == 3)
            {
                setColor(brick, "GREEN");
            }
            if (i == 4)
            {
                setColor(brick, "00FFFF"); //CYAN
            }

            setFilled(brick, true);
            add(window, brick);
            brick_x = brick_x + 39;
        }
        brick_y = brick_y + 15;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(190, 200, 20, 20);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(165, 500, 70, 10);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel(" ");
    setFont(label, "SansSerif-50");
    setColor(label, "C0C0C0");
    add(window, label);
    setLocation(label, 185, 300);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
