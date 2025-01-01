#include <GL/glut.h>
#include <iostream>
#include <string>
#include <vector>  
#include <cstdlib>
#include <ctime>

float ballX = 0.0f, ballY = 0.0f;
float ballXDir = 0.01f, ballYDir = 0.01f;
float paddleX = 0.0f;
float paddleWidth = 0.3f, paddleHeight = 0.05f;
int score = 0;
int lives = 3;
int level = 1;
bool gameOver = false;
bool replay = false;
std::vector<std::pair<float, float>> obstacles;
const float obstacleSize = 0.1f;
const int MAX_OBSTACLES = 10;

void renderText(float x, float y, std::string text, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawBall() {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(ballX, ballY);
    for (int i = 0; i <= 100; ++i) {
        float angle = i * 2.0f * 3.14159f / 100;
        glVertex2f(ballX + 0.05f * cos(angle), ballY + 0.05f * sin(angle));
    }
    glEnd();
}

void drawPaddle() {
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(paddleX - paddleWidth / 2, -0.9f);
    glVertex2f(paddleX + paddleWidth / 2, -0.9f);
    glVertex2f(paddleX + paddleWidth / 2, -0.9f + paddleHeight);
    glVertex2f(paddleX - paddleWidth / 2, -0.9f + paddleHeight);
    glEnd();
}

void drawObstacles() {
    glColor3f(0.0f, 1.0f, 0.0f);
    for (auto& obstacle : obstacles) {
        glBegin(GL_QUADS);
        glVertex2f(obstacle.first - obstacleSize / 2, obstacle.second - obstacleSize / 2);
        glVertex2f(obstacle.first + obstacleSize / 2, obstacle.second - obstacleSize / 2);
        glVertex2f(obstacle.first + obstacleSize / 2, obstacle.second + obstacleSize / 2);
        glVertex2f(obstacle.first - obstacleSize / 2, obstacle.second + obstacleSize / 2);
        glEnd();
    }
}

void resetGame() {
    ballX = 0.0f;
    ballY = 0.0f;
    ballXDir = 0.01f;
    ballYDir = 0.01f;
    paddleX = 0.0f;
    paddleWidth = 0.3f;
    score = 0;
    lives = 3;
    level = 1;
    gameOver = false;
    replay = false;
    obstacles.clear();
    for (int i = 0; i < 5; ++i) {
        obstacles.push_back({ -0.8f + static_cast<float>(rand()) / (RAND_MAX / 1.6f), 0.5f - i * 0.2f });
    }
}

void nextLevel() {
    level++;
    ballXDir *= 1.2f;
    ballYDir *= 1.2f;
    int numObstacles = std::min(MAX_OBSTACLES, 5 + level);
    for (int i = 0; i < numObstacles; ++i) {
        obstacles.push_back({ -0.8f + static_cast<float>(rand()) / (RAND_MAX / 1.6f), 0.5f - i * 0.2f });
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameOver) {
        glColor3f(1.0f, 0.0f, 0.0f);
        renderText(-0.2f, 0.0f, "GAME OVER", GLUT_BITMAP_HELVETICA_18);
        renderText(-0.3f, -0.1f, "Final Score: " + std::to_string(score), GLUT_BITMAP_HELVETICA_18);
        renderText(-0.3f, -0.2f, "Press SPACE to Replay", GLUT_BITMAP_HELVETICA_18);
    }
    else {
        drawBall();
        drawPaddle();
        drawObstacles();
        renderText(-0.95f, 0.9f, "Score: " + std::to_string(score), GLUT_BITMAP_HELVETICA_18);
        renderText(0.7f, 0.9f, "Lives: " + std::to_string(lives), GLUT_BITMAP_HELVETICA_18);
        renderText(-0.2f, 0.8f, "Level: " + std::to_string(level), GLUT_BITMAP_HELVETICA_18);
    }
    glutSwapBuffers();
}

void update(int value) {
    if (gameOver) return;
    ballX += ballXDir;
    ballY += ballYDir;

    if (ballX + 0.05f > 1.0f || ballX - 0.05f < -1.0f) {
        ballXDir *= -1;
    }
    if (ballY + 0.05f > 1.0f) {
        ballYDir *= -1;
    }
    if (ballY - 0.05f <= -0.9f + paddleHeight &&
        ballY - 0.05f >= -0.9f &&
        ballX >= paddleX - paddleWidth / 2 &&
        ballX <= paddleX + paddleWidth / 2) {
        ballYDir *= -1;
        float hitPosition = (ballX - paddleX) / (paddleWidth / 2);
        ballXDir = hitPosition * 0.02f;
        ballY = -0.9f + paddleHeight + 0.05f;
        score++;

        if (paddleWidth > 0.1f) {
            paddleWidth -= 0.01f;
        }
        ballXDir *= 1.05f;
        ballYDir *= 1.05f;
    }

    for (auto it = obstacles.begin(); it != obstacles.end(); ) {
        if (ballX + 0.05f > it->first - obstacleSize / 2 && ballX - 0.05f < it->first + obstacleSize / 2 &&
            ballY + 0.05f > it->second - obstacleSize / 2 && ballY - 0.05f < it->second + obstacleSize / 2) {
            it = obstacles.erase(it);
            ballYDir *= -1;
        }
        else {
            ++it;
        }
    }

    if (obstacles.empty()) {
        nextLevel();
    }

    if (ballY - 0.05f < -1.0f) {
        lives--;
        if (lives == 0) {
            gameOver = true;
            std::cout << "Game Over! Final Score: " << score << std::endl;
        }
        else {
            ballX = 0.0f;
            ballY = 0.0f;
            ballXDir = 0.01f;
            ballYDir = 0.01f;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKeys(int key, int x, int y) {
    if (!gameOver) {
        if (key == GLUT_KEY_LEFT && paddleX - paddleWidth / 2 > -1.0f) {
            paddleX -= 0.05f;
        }
        if (key == GLUT_KEY_RIGHT && paddleX + paddleWidth / 2 < 1.0f) {
            paddleX += 0.05f;
        }
    }
    glutPostRedisplay();
}

void handleKeyPress(unsigned char key, int x, int y) {
    if (key == 32 && gameOver) {
        replay = true;
        resetGame();
        glutTimerFunc(16, update, 0);
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    resetGame();
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bouncing Ball Game with Levels");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(handleKeys);
    glutKeyboardFunc(handleKeyPress);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
