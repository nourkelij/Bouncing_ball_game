#include <GL/glut.h>
#include <iostream>
#include <string>

float ballX = 0.0f, ballY = 0.0f;
float ballXDir = 0.01f, ballYDir = 0.01f;
float paddleX = 0.0f;
const float paddleWidth = 0.3f, paddleHeight = 0.05f;
int score = 0;
bool gameOver = false;

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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameOver) {
        glColor3f(1.0f, 0.0f, 0.0f);
        renderText(-0.2f, 0.0f, "GAME OVER", GLUT_BITMAP_HELVETICA_18);
        renderText(-0.3f, -0.1f, "Final Score: " + std::to_string(score), GLUT_BITMAP_HELVETICA_18);
    }
    else {
        drawBall();
        drawPaddle();
        renderText(-0.95f, 0.9f, "Score: " + std::to_string(score), GLUT_BITMAP_HELVETICA_18);
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
    if (ballY - 0.05f < -0.9f + paddleHeight && ballX > paddleX - paddleWidth / 2 && ballX < paddleX + paddleWidth / 2) {
        ballYDir *= -1;
        score++;
    }
    if (ballY - 0.05f < -1.0f) {
        gameOver = true;
        std::cout << "Game Over! Final Score: " << score << std::endl;
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

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bouncing Ball Game");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(handleKeys);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}