#include <Arduino.h>
#include <random>
#include <iostream>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

using namespace std;
enum eDir {
    STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6
};

class cBall {
private:
    int x, y;
    int originalX, originalY;
    int speed;
    int width;
    int height;
    eDir direction;
    static std::random_device rd;  // Seed for the random number engine
    static std::mt19937 gen; // Mersenne Twister engine
    static std::uniform_int_distribution<> dis; // Distribution in range [1, 6]
public:
    cBall(int posX, int posY, int w, int h, int speed_) {

        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
        direction = STOP;
        width = w;
        height = h;
        speed = speed_;
    }

    void Reset() {
        x = originalX;
        y = originalY;
        direction = STOP;
        speed = 5;
    }

    void changeDirection(eDir d) {
        direction = d;
    }


    void randomDirection() {
        direction = static_cast<eDir>(dis(gen));
    }

    inline int getX() const { return x; }

    inline int getY() const { return y; }

    inline eDir getDirection() { return direction; }

    void Move() {
        switch (direction) {
            case STOP:
                break;
            case LEFT:
                DecrementX();
                break;
            case RIGHT:
                IncrementX();
                break;
            case UPLEFT:
                DecrementX();
                DecrementY();
                break;
            case DOWNLEFT:
                DecrementX();
                IncrementY();
                break;
            case UPRIGHT:
                IncrementX();
                DecrementY();
                break;
            case DOWNRIGHT:
                IncrementX();
                IncrementY();
                break;
            default:
                break;
        }
    }

    void IncrementX() {
        x+=speed;
        if (x > (width-1))
            x = width-1;
    }

    void DecrementX() {
        x-=speed;
        if (x < 1)
            x = 1;
    }

    void IncrementY() {
        y+=speed;
        if (y > (height -1))
            y = height - 1;
    }

    void DecrementY() {
      int yspeed = speed/2;
      if(yspeed < 1)
          yspeed = 1;
        y-=yspeed;
        if (y < 1)
            y = 1;
    }

    void IncreaseSpeed(int s) { speed += s; }

    friend ostream &operator<<(ostream &o, cBall c) {
        o << "Ball [" << c.x << "," << c.y << "][" << c.direction << "]";
        return o;
    }


    int GetSpeed() const {
        return speed;
    }
};

class cPaddle {
private:
    int x{}, y{};
    int originalX;
    int paddle_size;
    int speed;
    int original_speed;
    int field_height;
public:


    cPaddle(int posX, int field_height_, int paddle_size_, int speed_) {
        originalX = posX;
        paddle_size = paddle_size_;
        speed = speed_;
        original_speed = speed_;
        field_height = field_height_;
        Reset();
    }

    inline void Reset() {
        x = originalX;
        y = field_height / 2 - (paddle_size / 2);
        speed = original_speed;
    }

    inline int getX() const { return x; }

    inline int getY() const { return y; }

    inline void moveUp() {
        y += speed;
        if (y > field_height - paddle_size)
            y = field_height - paddle_size;
    }

    inline void moveDown() {
        y -= speed;
        if (y < 0)
            y = 0;
    }

    inline void IncreaseSpeed(int s) { speed += s; }

    friend ostream &operator<<(ostream &o, cPaddle c) {
        o << "Paddle [" << c.x << "," << c.y << "]";
        return o;
    }


};

class cGameManager {
private:
    int width, height, ball_size, paddle_size;
    int score1, score2;
    bool quit;
    bool state_started = false;
    bool state_show_score = false;
    bool state_show_new_game = true;
    int cycles = 0;
    cBall *ball;
    cPaddle *player1;
    cPaddle *player2;
public:
    cGameManager(int w, int h) {
        srand(time(NULL));
        quit = false;
        score1 = score2 = 0;
        width = w;
        height = h;
        ball_size = 2;
        paddle_size = 10;
        ball = new cBall(w / 2, h / 2, w, h, 2);
        player1 = new cPaddle(1, height, paddle_size, 4);
        player2 = new cPaddle(w - 2, height, paddle_size, 4);
    }

    ~cGameManager() {
        delete ball, player1, player2;
    }

    void ScoreUp(cPaddle *player) {
        if (player == player1)
            score1++;
        else if (player == player2)
            score2++;

        if (score1 > 3 || score2 > 3) {
            state_started = false;
            state_show_score = true;
            state_show_new_game = false;

        }

        ball->Reset();
        player1->Reset();
        player2->Reset();
        cycles = 0;
    }

    void Draw() {
        if (state_started) {
            //TODO Draw the game on Display
            //Draw ball (ball->getX(), ball->getY(), ball_size);
            display.fillCircle(ball->getX(), ball->getY(), ball_size, SSD1306_WHITE);
            //Draw paddles
            //Player 1
            //player1->getX(), player1->getY()
            display.fillRect(player1->getX(), player1->getY(), 3, paddle_size, SSD1306_WHITE);
            //Player 2
            //player2->getX(), player2->getY()
            display.fillRect(player2->getX(), player2->getY(), 3, paddle_size, SSD1306_WHITE);

            //Draw Score Player 1: score1
            //Draw Score Player 2: score2
            display.setCursor(0, 0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.print("Player 1: ");
            display.print(score1);
            display.print("  Player 2: ");
            display.print(score2);
            display.display();
        }

        if (state_show_score) {
            //TODO Draw the score on Display
            //Show score
            display.setCursor(0, 0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.print("Score:");
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.print("Player 1: ");
            display.print(score1);
            display.setCursor(0, 30);
            display.print("Player 2: ");
            display.print(score2);
            display.display();
        }

        if (state_show_new_game) {
            //TODO Draw the new game on Display
            //show new game
            display.setCursor(0, 0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.print("New Game!");
            display.setTextSize(1);
            display.setCursor(0, 20);
            display.print("Player 1: ");
            display.print(score1);
            display.setCursor(0, 30);
            display.print("Player 2: ");
            display.print(score2);
            display.setCursor(0, 50);
            display.setTextSize(1);
            display.print("Press any button to start");
            display.display();
        }


    }

    void Input() {
             

            int player1_up, player1_down, player2_up, player2_down;
            //TODO Set the input for the paddles - they should be LOW
            player1_up = digitalRead(10);
            player1_down = digitalRead(11);
            player2_up = digitalRead(13);
            player2_down = digitalRead(12);

        if (state_started) {
            ball->Move();

            int ballx = ball->getX();
            int bally = ball->getY();
            int player1x = player1->getX();
            int player2x = player2->getX();
            int player1y = player1->getY();
            int player2y = player2->getY();

     

            if (player1_up != player1_down) {
                if (player1_up == LOW)
                    player1->moveUp();
                else if (player1_down == LOW)
                    player1->moveDown();
            }
            if (player2_up != player2_down) {
                if (player2_up == LOW)
                    player2->moveUp();
                else if (player2_down == LOW)
                    player2->moveDown();
            }

            if (ball->getDirection() == STOP)
                ball->randomDirection();
        }
        if (state_show_new_game) {
            if (player1_up == LOW || player1_down == LOW || player2_up == LOW || player2_down == LOW) {
                state_started = true;
                state_show_score = false;
                state_show_new_game = false;
            }
        }
        if (state_show_score) {
            if (player1_up == LOW || player1_down == LOW || player2_up == LOW || player2_down == LOW) {
                score1 = 0;
                score2 = 0;
                cycles = 0;
                //reset ball and paddles
                ball->Reset();
                player1->Reset();
                player2->Reset();
                state_started = false;
                state_show_score = false;
                state_show_new_game = true;
            }
        }


    }

    void Logic() {
        if (state_started) {
            cycles++;
            if(cycles > 50)
            {
                ball->IncreaseSpeed(1);
                cycles = 0;
            }
            int ballx = ball->getX();
            int bally = ball->getY();
            int player1x = player1->getX();
            int player2x = player2->getX();
            int player1y = player1->getY();
            int player2y = player2->getY();

            //bottom wall
            if (bally >= height - 1)
                ball->changeDirection(ball->getDirection() == DOWNRIGHT ? UPRIGHT : UPLEFT);

            //top wall
            if (bally <= 1)
                ball->changeDirection(ball->getDirection() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);

            //left paddle
            for (int i = 0; i < paddle_size; i++)
                if (ballx <= player1x + 1)
                    if (bally == player1y + i)
                    {
                        ball->randomDirection();
                        ball->changeDirection((eDir) ((ball->getDirection() % 3) + 4));
                        return;
                    }



            //right paddle
            for (int i = 0; i < paddle_size; i++)
                if (ballx >= player2x - 2)
                    if (bally == player2y + i)
                    {
                        ball->randomDirection();
                        ball->changeDirection((eDir) ((ball->getDirection() % 3) + 1));
                        return;
                    }




            //right wall
            if (ballx >= width - 1)
                ScoreUp(player1);
            //left wall
            if (ballx <= 1)
                ScoreUp(player2);
        }
    }

    void Run() {
        Draw();
        Input();
        Logic();
    }



};

cGameManager c(SCREEN_WIDTH, SCREEN_HEIGHT);

void setupGFX() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C));
      Serial.println(F("SSD1306 allocation failed"));
  display.clearDisplay();
}

void setup() {
    // print the result to the serial monitor
    //Setup GPIO4 and 5 for I2C communication
    Wire.begin();
    Serial.begin(9600);
    setupGFX();

    //setup GP10 bis GP13 as input with pullup
    for (int i = 10; i < 14; i++) {
        pinMode(i, INPUT_PULLUP);
    }


}


void loop() {
    c.Run();
    delay(60);
}