#include <iostream>
#include <raylib.h>
#include <deque>
#include <string>
#include <raymath.h>
// Better updated system to read user's directional inputs in an event queue
// Let players start by moving to the left as well
// Initialization
//--------------------------------------------------------------------------------------
Color green = { 125, 183, 59, 255 };

const int cellSize = 28;
const int cellWidthCount = 40;
const int cellHeightCount = 30;
const int borderOffset = 84;

double INTERVAL = 0.20;
double lastUpdateTime = 0;

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    else {
        return false;
    }
};

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(element, deque[i])) {
            return true;
        }
    }
    return false;
}

class Food {
public:
    Texture2D texture;
    Vector2 position;

    Food(std::deque<Vector2> snakeBody) {
        Image image = LoadImage("Graphics/food.png"); //loads successfully 
        texture = LoadTextureFromImage(image); // Access violation executing location error unless Intialized after InitWindow()
        UnloadImage(image);
        position = Move(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        //DrawRectangle(position.x * cellSize, position.y * cellSize,cellSize, cellSize, RAYWHITE);
        DrawTexture(texture, (float)borderOffset + position.x * (float)cellSize, (float)borderOffset + position.y * (float)cellSize, RAYWHITE);
    }

    Vector2 GenerateRandomPos() {
        float x = GetRandomValue(0, (float)cellWidthCount - 1);
        float y = GetRandomValue(0, (float)cellHeightCount - 1);
        return Vector2{ x, y };
    }

    Vector2 Move(std::deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomPos();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomPos();
        }
        return position;
    }
};

class Snake {
public:
    std::deque<Vector2> body = { Vector2{ 9,10 }, Vector2{ 8,10 }, Vector2{ 7,10 } };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            DrawRectangleRounded(Rectangle{ borderOffset + x * cellSize, borderOffset + y * cellSize, cellSize, cellSize}, 0.75, 10, BLACK);
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment) {
            addSegment = false;
        }
        else {
            body.pop_back();
        }
    }

    void Reset() {
        body = { Vector2{ 9,10 }, Vector2{ 8,10 }, Vector2{ 7,10 } };
        direction = { 0, 0 };
    }

};

class Game {
public:
    Snake snake = Snake();
    //pass in snake body to make sure initially the food does not spawn on the snake
    Food food = Food(snake.body);
    bool running = false;
    int score = 0;

    void Draw() {
        snake.Draw();
        food.Draw();
    }

    void Update() {
        if (running) {
            snake.Update();
            // CHECKING FOR COLLISIONS, in here since snake is updated by the interval 
            // and thus only collides during the interval update
            CheckFoodCollision();
            CheckEdgeCollision();
            CheckSelfCollision();
        }
    }

    void CheckDirection() {
        if (IsKeyPressed(KEY_UP) && snake.direction.y != 1) {
            snake.direction = { 0, -1 };
            running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && snake.direction.y != -1) {
            snake.direction = { 0, 1 };
            running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && snake.direction.x != -1) {
            snake.direction = { 1, 0 };
            running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && snake.direction.x != 1) {
            snake.direction = { -1, 0 };
            running = true;
        }
    }

    void CheckFoodCollision() {
        if (Vector2Equals(food.position, snake.body[0])) {
            //std::cout << "collided" << std::endl;
            food.position = food.Move(snake.body);
            if (INTERVAL > 0.05) {
                INTERVAL -= 0.005; //Speeds up!
            }
            snake.addSegment = true;
            score++;
        }
    }

    void CheckEdgeCollision() {
        if (snake.body[0].x < 0 || snake.body[0].x == cellWidthCount ||
            snake.body[0].y < 0 || snake.body[0].y == cellHeightCount) {
            GameOver();
        }
    }

    void CheckSelfCollision() {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }

    void GameOver() {
        snake.Reset();
        food.position = food.Move(snake.body);
        running = false;
        score = 0;
    }
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    //const char* WorkingDirectory = GetWorkingDirectory();
    //std::cout << WorkingDirectory << std::endl;
    InitWindow(2 * borderOffset + cellSize * cellWidthCount, 2 * borderOffset + cellSize * cellHeightCount, "Retro Snake");
    SetTargetFPS(60);
    Game game = Game();
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        // UPDATING
        game.CheckDirection();
        if (eventTriggered(INTERVAL)) {
            game.Update();
        }
        
        // DRAWING
        ClearBackground(green);
        DrawText("RETRO SNAKE", borderOffset - 5, 20, 40, DARKGREEN);
        DrawText(TextFormat("%i", game.score), cellSize * cellWidthCount, 20, 40, DARKGREEN);
        DrawRectangleLinesEx(Rectangle{ (float)borderOffset - 5, (float)borderOffset - 5, (float)cellSize * cellWidthCount + 10, (float)cellSize * cellHeightCount + 10 }, 5, DARKGREEN);
        //----------------------------------------------------------------------------------
        game.Draw();
        //OBJECTS
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
