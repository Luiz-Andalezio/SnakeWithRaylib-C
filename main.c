#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define SNAKE_LENGTH   256
#define SQUARE_SIZE     31

typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
} Food;

static const int screenWidth = 1280;
static const int screenHeight = 720;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

static Food fruit = { 0 };
static Snake snake[SNAKE_LENGTH] = { 0 };
static Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
static bool allowMove = false;
static Vector2 offset = { 0 };
static int counterTail = 0;
static int score = 0;
static Color groundColor = GREEN;

static void InitGame(void);         
static void UpdateGame(void);       
static void DrawGame(void);         
static void UnloadGame(void);       
static void UpdateDrawFrame(void);  

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Cobrinha Colorida");

    InitAudioDevice();
    Music music = LoadMusicStream("assets/snake.mp3");
    PlayMusicStream(music);

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())    
    {
        UpdateMusicStream(music);
        UpdateDrawFrame();
    }
#endif

    UnloadGame();         
    
UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();    

    return 0;
}

void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;
    score = 0;
    groundColor = DARKGREEN;

    counterTail = 1;
    allowMove = false;

    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;
    
    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ offset.x / 2, offset.y / 2 };
        snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

        if (i > 0 && i < counterTail) {
            snake[i].size = (Vector2){ SQUARE_SIZE - (i * 2), SQUARE_SIZE - (i * 2) };
        }

        if (i == 0) snake[i].color = BLUE;
        else snake[i].color = BLUE;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruit.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
    fruit.color = RED;
    fruit.active = false;
}

void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
            }

            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter % 5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i - 1];
                }
            }

            if (((snake[0].position.x) > (screenWidth - offset.x)) ||
                ((snake[0].position.y) > (screenHeight - offset.y)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                gameOver = true;
            }

            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
            }

            if (!fruit.active)
            {
                fruit.active = true;
                fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };

                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
                    {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                        i = 0;
                    }
                }
            }

            if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
                (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                score += 1;
                
Color newColor = (Color){ GetRandomValue(50, 255), GetRandomValue(50, 255), GetRandomValue(50, 255), 255 };
for (int i = 0; i < counterTail; i++) {
    snake[i].color = newColor;
}

fruit.active = false;
            }

            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

void DrawGame(void)
{
    BeginDrawing();

    ClearBackground(groundColor);

    if (!gameOver)
    {
        DrawRectangleLines(offset.x / 2, offset.y / 2, screenWidth - offset.x, screenHeight - offset.y, WHITE);

        for (int i = 0; i < counterTail; i++)
            DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

        DrawRectangleV(fruit.position, fruit.size, fruit.color);

        DrawText(TextFormat("Pontuação: %d", score), 10, 10, 20, WHITE);

        if (pause)
            DrawText("JOGO PAUSADO", screenWidth / 2 - MeasureText("JOGO PAUSADO", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
    }
    else
        DrawText("SUA COBRA BATEU!\n\nPRESSIONE [ENTER] PARA JOGAR NOVAMENTE", GetScreenWidth() / 2 - MeasureText("SUA COBRA BATEU!\n\nPRESSIONE [ENTER] PARA JOGAR NOVAMENTE", 20) / 2, GetScreenHeight() / 2 - 50, 20, RED);

    EndDrawing();
}


void UnloadGame(void)
{
   
}

void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
    