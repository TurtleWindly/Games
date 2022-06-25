#include "raylib.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#define TWIN_USB_GAMEPAD "Twin USB Gamepad      "

struct Pipe
{
    Rectangle top;
    Rectangle bottom;
};

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 1366;
    const int screenHeight = 768;

    const int GRAVITY = 4;

    int scores = 0;

    // Pipe
    const int pipe_width     = 70;
    const int pipes_length = 7;
    const int pipe_space = floor((double)screenHeight * 1.0/3.0);
    const int pipe_speed = 4;
    const int pipe_gaps = 4;
    const int start_position = screenWidth;
    const int end_position = -pipe_width;

    struct Pipe pipes[pipes_length];
    for (int iii = 0; iii < pipes_length; iii++)
    {
        Rectangle top = {
            screenWidth + (iii * pipe_width * pipe_gaps),
            0,
            pipe_width,
            GetRandomValue(0, pipe_space),
        };

        Rectangle bottom = {
            screenWidth + (iii * pipe_width * pipe_gaps),
            top.height + pipe_space,
            pipe_width,
            screenHeight - top.height - pipe_space,
        };

        struct Pipe new_pipe = {
            top,
            bottom,
        };

        pipes[iii] = new_pipe;
    }
    

    // Game state
    bool isEnd = false;

    // Player
    Rectangle player = {
        80,
        (float)screenHeight / 2.0,
        50,
        50,
    };

    const int max_jump_time = 20;
    const int jump_height = 5;
    int jump_time = 0;
    bool is_not_jumping = true;

    // Grass
    Rectangle grass = {
        0,
        screenHeight * 6/7,
        screenWidth,
        screenHeight * 1/7 + 10,
    };

    InitWindow(screenWidth, screenHeight, "flappy bird");


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (isEnd) {
            BeginDrawing();
                DrawText("You lose", screenWidth / 2, 150, 24, BLACK);
                DrawText("Press R to restart", screenWidth / 2, 250, 32, BLACK);
            EndDrawing();

            if (IsKeyPressed(KEY_R)) {
                isEnd = false;
            }

            if (IsGamepadAvailable(0)) {
                if (TextIsEqual(GetGamepadName(0), TWIN_USB_GAMEPAD)) {
                    if (GetGamepadButtonPressed() == 8 ) {
                        isEnd = false;
                    }
                }
            }

            if (!isEnd) {
                player.y = (float)screenHeight / 2.0;
                for (int iii = 0; iii < pipes_length; iii++)
                {
                    Rectangle top = {
                        screenWidth + (iii * pipe_width * pipe_gaps),
                        0,
                        pipe_width,
                        GetRandomValue(0, pipe_space),
                    };

                    Rectangle bottom = {
                        screenWidth + (iii * pipe_width * pipe_gaps),
                        top.height + pipe_space,
                        pipe_width,
                        screenHeight - top.height - pipe_space,
                    };

                    struct Pipe new_pipe = {
                        top,
                        bottom,
                    };

                    pipes[iii] = new_pipe;
                }
            }

            continue;
        }
        // Update
        //----------------------------------------------------------------------------------

        // Check player with grass
        if (CheckCollisionRecs(player, grass)) {
            isEnd = true;
        }

        // Gamepad
        if (IsGamepadAvailable(0)) {
            if (TextIsEqual(GetGamepadName(0), TWIN_USB_GAMEPAD)) {
                if (GetGamepadButtonPressed() == 5 ) {
                    is_not_jumping = false;
                }
            }
        }

        // Jump
        if (IsKeyPressed(KEY_SPACE)) {
            is_not_jumping = false;
        }

        if (!is_not_jumping) {
            jump_time++;
            player.y -= jump_height;
        }

        if (jump_time == max_jump_time) {
            is_not_jumping = true;
            jump_time = 0;
        }

        // Apply physic to player
        if (is_not_jumping) {
            player.y += GRAVITY;
        }

        // Move Pipe
        for (int iii = 0; iii < pipes_length; iii++)
        {
            pipes[iii].top.x    -= pipe_speed;
            pipes[iii].bottom.x -= pipe_speed;
        }

        // Reset pipe
        for (int iii = 0; iii < pipes_length; iii++) {

            // check pipe x is less than end position
            if (pipes[iii].top.x < end_position) {

                int farest_position = 0;

                // get the farest position of the pipes
                for (int uuu = 0; uuu < pipes_length; uuu++) {

                    if (pipes[uuu].top.x > farest_position) {
                        farest_position = pipes[uuu].top.x;
                    }
                }

                // set pipes position relative to farest pipe
                pipes[iii].top.x    = farest_position + (pipe_width * pipe_gaps);
                pipes[iii].bottom.x = farest_position + (pipe_width * pipe_gaps);

                // set new pipes heigth
                pipes[iii].top.height = GetRandomValue(0, pipe_space);

                pipes[iii].bottom.height = screenHeight - pipe_space - pipes[iii].top.height;
                pipes[iii].bottom.y = pipes[iii].top.height + pipe_space;
            }

            // Increase scores when pipe at player position
            if ((pipes[iii].top.x <= player.x) && (pipes[iii].top.x > player.x - 4)) {
                scores++;
            }

            // Pipes collion
            if (CheckCollisionRecs(player, pipes[iii].top) || CheckCollisionRecs(player, pipes[iii].bottom)) {
                isEnd = true;
            }


        }
        

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLUE);

            DrawFPS(0, 0);
            DrawRectangleRec(grass, GREEN);

            // Draw pipe
            for (int iii = 0; iii < pipes_length; iii++)
            {
                DrawRectangleRec(pipes[iii].top, DARKGREEN);
                DrawRectangleRec(pipes[iii].bottom, DARKGREEN);
            }

            // Draw player
            DrawRectangleRec(player, YELLOW);

            // Draw scores
            DrawText(TextFormat("%i", scores), screenWidth / 2, 100, 32, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
