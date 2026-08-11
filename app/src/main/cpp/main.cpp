#include "raylib.h"
#include <vector>
#include "raymath.h"
#include <cmath>

// Размер сетки каждой грани куба (10x10)
const int CUBE_SIZE = 10;

// Все 6 граней куба
enum Face {
    FACE_TOP = 0,
    FACE_BOTTOM,
    FACE_FRONT,
    FACE_RIGHT,
    FACE_BACK,
    FACE_LEFT
};

// Направления движения на грани
enum Direction {
    DIR_UP = 0,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
};

// Структура сегмента змейки на кубе
struct SnakeNode {
    int face;
    int u, v;
};

// Перевод координат грани в мировые 3D координаты
Vector3 GetWorldPosition(int face, int u, int v)
{
    float half = CUBE_SIZE / 2.0f;
    float fu = (float)u - half + 0.5f;
    float fv = (float)v - half + 0.5f;
    float bound = half;

    switch (face)
    {
        case FACE_TOP:    return (Vector3){ fu, bound, fv };
        case FACE_BOTTOM: return (Vector3){ fu, -bound, -fv };
        case FACE_FRONT:  return (Vector3){ fu, -fv, -bound };
        case FACE_RIGHT:  return (Vector3){ bound, -fv, fu };
        case FACE_BACK:   return (Vector3){ -fu, -fv, bound };
        case FACE_LEFT:   return (Vector3){ -bound, -fv, -fu };
    }
    return (Vector3){ 0, 0, 0 };
}

// Нормаль грани для позиционирования
Vector3 GetFaceNormal(int face)
{
    switch (face)
    {
        case FACE_TOP:    return (Vector3){ 0.0f, 1.0f, 0.0f };
        case FACE_BOTTOM: return (Vector3){ 0.0f, -1.0f, 0.0f };
        case FACE_FRONT:  return (Vector3){ 0.0f, 0.0f, -1.0f };
        case FACE_RIGHT:  return (Vector3){ 1.0f, 0.0f, 0.0f };
        case FACE_BACK:   return (Vector3){ 0.0f, 0.0f, 1.0f };
        case FACE_LEFT:   return (Vector3){ -1.0f, 0.0f, 0.0f };
    }
    return (Vector3){ 0, 1, 0 };
}

// Получение локальных осей грани (right и down)
void GetFaceAxes(int face, Vector3* right, Vector3* down)
{
    switch (face)
    {
        case FACE_TOP:
            *right = (Vector3){ 1, 0, 0 };
            *down  = (Vector3){ 0, 0, 1 };
            break;
        case FACE_BOTTOM:
            *right = (Vector3){ 1, 0, 0 };
            *down  = (Vector3){ 0, 0, -1 };
            break;
        case FACE_FRONT:
            *right = (Vector3){ 1, 0, 0 };
            *down  = (Vector3){ 0, -1, 0 };
            break;
        case FACE_RIGHT:
            *right = (Vector3){ 0, 0, 1 };
            *down  = (Vector3){ 0, -1, 0 };
            break;
        case FACE_BACK:
            *right = (Vector3){ -1, 0, 0 };
            *down  = (Vector3){ 0, -1, 0 };
            break;
        case FACE_LEFT:
            *right = (Vector3){ 0, 0, -1 };
            *down  = (Vector3){ 0, -1, 0 };
            break;
    }
}

// Генерация еды на любой случайной грани куба
SnakeNode GetRandomFood(const std::vector<SnakeNode>& snake)
{
    SnakeNode food;
    bool valid;
    do {
        valid = true;
        food.face = GetRandomValue(0, 5);
        food.u = GetRandomValue(0, CUBE_SIZE - 1);
        food.v = GetRandomValue(0, CUBE_SIZE - 1);

        for (const auto& seg : snake)
        {
            if (seg.face == food.face && seg.u == food.u && seg.v == food.v)
            {
                valid = false;
                break;
            }
        }
    } while (!valid);
    return food;
}

int main()
{
    const int width = 1280;
    const int height = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, "SNAKE 3D CUBE");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound eatSound = LoadSound("eat.mp3");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 20.0f };
    camera.target   = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 50.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::vector<SnakeNode> snake;
    snake.push_back({ FACE_TOP, 5, 5 });
    snake.push_back({ FACE_TOP, 5, 4 });
    snake.push_back({ FACE_TOP, 5, 3 });

    Direction dir = DIR_DOWN;
    Direction nextDir = DIR_DOWN;

    SnakeNode food = GetRandomFood(snake);

    float moveTimer = 0.0f;
    float baseMoveInterval = 0.18f;
    float moveInterval = baseMoveInterval;

    int score = 0;
    int highScore = 0;
    bool gameOver = false;

    while (!WindowShouldClose())
    {
        // Управление (Клавиатура + Свайпы)
        int gesture = GetGestureDetected();

        if (!gameOver)
        {
            if ((IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W) || gesture == GESTURE_SWIPE_UP)    && dir != DIR_DOWN)  nextDir = DIR_UP;
            if ((IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S) || gesture == GESTURE_SWIPE_DOWN)  && dir != DIR_UP)    nextDir = DIR_DOWN;
            if ((IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A) || gesture == GESTURE_SWIPE_LEFT)  && dir != DIR_RIGHT) nextDir = DIR_LEFT;
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || gesture == GESTURE_SWIPE_RIGHT) && dir != DIR_LEFT)  nextDir = DIR_RIGHT;
        }
        else
        {
            if (IsKeyPressed(KEY_SPACE) || gesture == GESTURE_TAP)
            {
                if (score > highScore) {
                    highScore = score;
                }

                snake.clear();
                snake.push_back({ FACE_TOP, 5, 5 });
                snake.push_back({ FACE_TOP, 5, 4 });
                snake.push_back({ FACE_TOP, 5, 3 });

                dir = DIR_DOWN;
                nextDir = DIR_DOWN;
                food = GetRandomFood(snake);
                score = 0;
                moveInterval = baseMoveInterval;
                gameOver = false;
                moveTimer = 0.0f;
            }
        }

        // Движение
        if (!gameOver)
        {
            moveTimer += GetFrameTime();

            if (moveTimer >= moveInterval)
            {
                moveTimer = 0.0f;
                dir = nextDir;

                SnakeNode head = snake.front();
                int next_u = head.u;
                int next_v = head.v;

                if (dir == DIR_UP)    next_v--;
                if (dir == DIR_DOWN)  next_v++;
                if (dir == DIR_LEFT)  next_u--;
                if (dir == DIR_RIGHT) next_u++;

                SnakeNode newHead = head;
                Direction newDir = dir;

                // Проверка выхода за границы грани
                if (next_u >= 0 && next_u < CUBE_SIZE && next_v >= 0 && next_v < CUBE_SIZE)
                {
                    newHead.u = next_u;
                    newHead.v = next_v;
                }
                else
                {
                    // Геометрический переход через ребро в 3D пространстве
                    Vector3 right, down;
                    GetFaceAxes(head.face, &right, &down);

                    Vector3 stepDir = {0, 0, 0};
                    if (dir == DIR_UP)    stepDir = Vector3Scale(down, -1.0f);
                    if (dir == DIR_DOWN)  stepDir = down;
                    if (dir == DIR_LEFT)  stepDir = Vector3Scale(right, -1.0f);
                    if (dir == DIR_RIGHT) stepDir = right;

                    Vector3 currentPos = GetWorldPosition(head.face, head.u, head.v);
                    Vector3 nextWorldPos = Vector3Add(currentPos, stepDir);

                    float half = CUBE_SIZE / 2.0f;
                    int newFace = head.face;

                    float ax = fabsf(nextWorldPos.x);
                    float ay = fabsf(nextWorldPos.y);
                    float az = fabsf(nextWorldPos.z);

                    if (ax >= ay && ax >= az) {
                        newFace = (nextWorldPos.x > 0) ? FACE_RIGHT : FACE_LEFT;
                    } else if (ay >= ax && ay >= az) {
                        newFace = (nextWorldPos.y > 0) ? FACE_TOP : FACE_BOTTOM;
                    } else {
                        newFace = (nextWorldPos.z > 0) ? FACE_BACK : FACE_FRONT;
                    }

                    float fu = 0, fv = 0;
                    switch (newFace)
                    {
                        case FACE_TOP:    fu = nextWorldPos.x; fv = nextWorldPos.z; break;
                        case FACE_BOTTOM: fu = nextWorldPos.x; fv = -nextWorldPos.z; break;
                        case FACE_FRONT:  fu = nextWorldPos.x; fv = -nextWorldPos.y; break;
                        case FACE_RIGHT:  fu = nextWorldPos.z; fv = -nextWorldPos.y; break;
                        case FACE_BACK:   fu = -nextWorldPos.x; fv = -nextWorldPos.y; break;
                        case FACE_LEFT:   fu = -nextWorldPos.z; fv = -nextWorldPos.y; break;
                    }

                    int new_u = (int)roundf(fu + half - 0.5f);
                    int new_v = (int)roundf(fv + half - 0.5f);

                    if (new_u < 0) new_u = 0;
                    if (new_u >= CUBE_SIZE) new_u = CUBE_SIZE - 1;
                    if (new_v < 0) new_v = 0;
                    if (new_v >= CUBE_SIZE) new_v = CUBE_SIZE - 1;

                    newHead.face = newFace;
                    newHead.u = new_u;
                    newHead.v = new_v;

                    // Универсальное определение нового направления движения вглубь новой грани
                    if (newHead.v == 0) {
                        if (dir == DIR_UP || dir == DIR_DOWN) newDir = DIR_DOWN;
                        else newDir = dir;
                    }
                    else if (newHead.v == CUBE_SIZE - 1) {
                        if (dir == DIR_UP || dir == DIR_DOWN) newDir = DIR_UP;
                        else newDir = dir;
                    }
                    else if (newHead.u == 0) {
                        if (dir == DIR_LEFT || dir == DIR_RIGHT) newDir = DIR_RIGHT;
                        else newDir = dir;
                    }
                    else if (newHead.u == CUBE_SIZE - 1) {
                        if (dir == DIR_LEFT || dir == DIR_RIGHT) newDir = DIR_LEFT;
                        else newDir = dir;
                    }
                    else {
                        newDir = dir;
                    }
                }

                dir = newDir;
                nextDir = newDir;

                // Столкновение с собой (начиная с 2-го сегмента)
                for (size_t i = 2; i < snake.size(); i++)
                {
                    if (snake[i].face == newHead.face && snake[i].u == newHead.u && snake[i].v == newHead.v)
                    {
                        gameOver = true;
                        if (score > highScore) {
                            highScore = score;
                        }
                        break;
                    }
                }

                if (!gameOver)
                {
                    snake.insert(snake.begin(), newHead);

                    if (newHead.face == food.face && newHead.u == food.u && newHead.v == food.v)
                    {
                        score += 10;
                        PlaySound(eatSound);
                        food = GetRandomFood(snake);

                        // Увеличение скорости каждые 50 очков
                        if (score % 50 == 0 && moveInterval > 0.08f) {
                            moveInterval -= 0.015f;
                        }
                    }
                    else
                    {
                        snake.pop_back();
                    }
                }
            }
        }

        // Плавная камера за головой
        Vector3 headPos = GetWorldPosition(snake.front().face, snake.front().u, snake.front().v);
        Vector3 faceNormal = GetFaceNormal(snake.front().face);

        Vector3 targetCamPos = Vector3Add(headPos, Vector3Scale(faceNormal, 9.0f));
        targetCamPos = Vector3Add(targetCamPos, (Vector3){ 0.0f, 7.0f, 7.0f });

        camera.position = Vector3Lerp(camera.position, targetCamPos, 0.1f);
        camera.target   = Vector3Lerp(camera.target, headPos, 0.1f);

        // Отрисовка
        BeginDrawing();
            ClearBackground((Color){ 200, 185, 205, 255 });

            BeginMode3D(camera);

                for (int f = 0; f < 6; f++)
                {
                    for (int u = 0; u < CUBE_SIZE; u++)
                    {
                        for (int v = 0; v < CUBE_SIZE; v++)
                        {
                            Vector3 cellPos = GetWorldPosition(f, u, v);
                            Vector3 normal = GetFaceNormal(f);

                            if (food.face == f && food.u == u && food.v == v)
                            {
                                Vector3 foodPos = Vector3Add(cellPos, Vector3Scale(normal, 0.02f));
                                DrawCube(foodPos, 0.90f, 0.90f, 0.90f, (Color){ 235, 80, 80, 255 });
                                DrawCubeWires(foodPos, 0.90f, 0.90f, 0.90f, (Color){ 180, 40, 40, 255 });
                            }
                            else
                            {
                                bool isSnake = false;
                                size_t snakeIndex = 0;
                                for (size_t i = 0; i < snake.size(); i++)
                                {
                                    if (snake[i].face == f && snake[i].u == u && snake[i].v == v)
                                    {
                                        isSnake = true;
                                        snakeIndex = i;
                                        break;
                                    }
                                }

                                if (isSnake)
                                {
                                    Vector3 snakePos = Vector3Add(cellPos, Vector3Scale(normal, 0.03f));
                                    Color color = (snakeIndex == 0) ? (Color){ 60, 110, 180, 255 } : (Color){ 100, 150, 210, 255 };
                                    DrawCube(snakePos, 0.90f, 0.90f, 0.90f, color);
                                    DrawCubeWires(snakePos, 0.90f, 0.90f, 0.90f, (Color){ 40, 80, 140, 255 });
                                }
                                else
                                {
                                    DrawCube(cellPos, 0.92f, 0.92f, 0.92f, (Color){ 245, 243, 248, 255 });
                                    DrawCubeWires(cellPos, 0.92f, 0.92f, 0.92f, (Color){ 200, 190, 205, 255 });
                                }
                            }
                        }
                    }
                }

            EndMode3D();

            DrawText(TextFormat("SCORE: %d", score), 20, 20, 30, (Color){ 80, 70, 90, 255 });
            DrawText(TextFormat("BEST: %d", highScore), 20, 60, 20, (Color){ 120, 110, 130, 255 });

            if (gameOver)
            {
                DrawRectangle(0, 0, width, height, (Color){ 0, 0, 0, 150 });
                DrawText("GAME OVER", width / 2 - 140, height / 2 - 70, 50, (Color){ 230, 120, 100, 255 });
                DrawText(TextFormat("Final Score: %d", score), width / 2 - 90, height / 2 - 10, 25, WHITE);
                DrawText("Press SPACE to restart", width / 2 - 130, height / 2 + 30, 20, (Color){ 200, 200, 200, 255 });
            }

        EndDrawing();
    }

    UnloadSound(eatSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
