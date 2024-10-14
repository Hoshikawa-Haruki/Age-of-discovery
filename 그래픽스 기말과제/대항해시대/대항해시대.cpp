#include <stdio.h>
#include <glut.h>
#include <glu.h>
#include <gl.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")
#define Caribbean "C:\\Users\\이승환\\Desktop\\그래픽스 기말과제\\Caribbean.wav"
#define Caribbean "C:\\Users\\이승환\\Desktop\\그래픽스 기말과제\\Caribbean.wav"
#define Win "C:\\Users\\이승환\\Desktop\\그래픽스 기말과제\\Win.wav"
#define Win "C:\\Users\\이승환\\Desktop\\그래픽스 기말과제\\Win.wav"
using namespace std;

GLfloat OW;
GLfloat OH;
GLfloat aspect;

static int Time = 0;
static int gameTime = 180; // 1분 = 60초
static int minimapcount = 5; // 미니맵 횟수
static int boostercount = 5; // 부스터 횟수
static GLfloat shipX= 175.0f;
static GLfloat shipZ = 195.0f;
static GLfloat xRot = -135.0f;
static GLfloat speed = 0.0f; // 기본 속도
static GLfloat roundspeed = 0.5; // 기본 회전 속도
static GLfloat cammove = 0.0f; // 항해 시 카메라 위아래 움직임
static GLfloat booster = 1.0f; // 부스터 기본속도
static GLfloat downmove = 1.0f; // 위아래 움직임 음양 조건
static GLfloat horizon = 1.0f;; // 좌우 움직임 음양 조건

float CheckX = 170;
float CheckY = 0;
float CheckZ = 170;

GLboolean moveForward = false; // 직진 on off
GLboolean moveBackward = false; // 후진 on off
GLboolean moveRot = false; // 회전 on off
GLboolean view = true; // 시점 변환 1인칭 or 3인칭
GLboolean dKey = false; // 미니맵 on off
GLboolean win = false; // 승리 확인

bool music = true;
bool complete = false;
bool sandil = false; // 섬 체크 유무 확인
bool treeil = false;
bool snowil = false;
bool cityil = false;
bool shipil = false;

bool downkey = true; // 부스터 사용시 후진 키 비활성화용
bool attach = false; // 충돌 범위 on off

static GLUquadricObj* pObj = gluNewQuadric(); // 눈사람 객체

//------------------------------------------------------------------------------------------------------------ 변수
void Ocean(); // 바다
void SandIsland(); // 모래섬
void TreeIsland(); // 숲
void SnowIsland(); // 북극
void CityIsland(); // 도시
void objShip(); // 난파선 디자인
void SnowMan(); // 눈사람
void Tree(); // 나무
void SnowTree(); // 눈나무
void Building(); // 빌딩
void Ship(); // 선박 디자인

void setRC(); // 조명
void Camera(); // 1인칭 카메라 - 미니맵 카메라
void mukae(); // 배 이동 함수
void viewchange(); // 1인칭 - 미니맵 전환
void moveRestrict(); // 이동 가능범위
void DrawUI(); // UI
void Music(); // bgm
void EndingMusic(); // 승리 효과음

void Restart(); // 재시작
void master(); // 강제 승리 트리거 활성화
void lose(); // 강제 패배 트리거 활성화
//------------------------------------------------------------------------------------------------------------함수

void viewchange() {
    glViewport(0, 0, OW, OH);
    if (view == false) {    //3인칭 
        glViewport(0, 0, OW, OH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        {
            if (aspect >= 1.0) {
                float width = 300.0f * aspect;
                glOrtho(-width, width, -300.0f, 300.0f, -300.0f, 300.0f); // 직교 투영 설정
            }
            else {
                float height = 300.0f / aspect;
                glOrtho(-300.0f, 300.0f, -height, height, -300.0f, 300.0f); // 직교 투영 설정
            }
        }
    }

    else {      // 1인칭
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70.0f, aspect, 0.1f, 1000.0f); // 원근 투영 설정
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}//완료
void designCam() {
    gluLookAt(CheckX, CheckY+20, CheckZ+50, // 카메라 위치
        CheckX, CheckY, CheckZ,   // 카메라가 바라보는 지점
        0.0, 1, 0.0);  // 카메라의 위쪽 방향
}
void Camera() {
    GLfloat cameraX = shipX; // 뱃머리 위치에서 약간 뒤로 이동
    GLfloat cameraY = 7.0f; // 선박 상부에서 약간 높게 시점을 조정
    GLfloat cameraZ = shipZ; // 뱃머리 위치에서 약간 위로 이동
    //1인칭
    if (view == true) {
        gluLookAt(cameraX, cameraY + (cammove), cameraZ, // 카메라 위치
            cameraX + sin(xRot * 3.141592f / 180.0f),
            cameraY + (cammove), cameraZ + cos(xRot * 3.141592f / 180.0f),// 카메라가 바라보는 지점
            0.0, 1, 0.0);  // 카메라의 위쪽 방향
    }
    else if (view == false) {
        gluLookAt(0, 60, 0,
            0, 0, 0,
            1, 0, 0);
    }
}
void mukae() {
    glPushMatrix();
    glTranslatef(shipX, 0.0f, shipZ);
    glRotatef(xRot, 0.0f, 1.0f, 0.0f);
    Ship();
    glPopMatrix();

    if (moveForward) {  
        float angleRad = xRot * (3.141592f / 180.0f); // xRot 각도를 라디안으로 변환
        shipX += sin(angleRad) * speed * booster; // x 방향으로 이동
        shipZ += cos(angleRad) * speed * booster; // z 방향으로 이동
    }
    else if (moveBackward) {
        float angleRad = xRot * (3.141592f / 180.0f); // xRot 각도를 라디안으로 변환
        shipX += sin(angleRad) * speed; // x 방향으로 이동
        shipZ += cos(angleRad) * speed; // z 방향으로 이동
    }
}
void Restart() {
    shipX = 175.0f;
    shipZ = 195.0f;
    xRot = -135.0f;
    speed = 0.0f; // 기본 속도
    roundspeed = 0.5f; // 기본 회전 속도
    cammove = 0.0f; // 항해 시 카메라 위아래 움직임
    moveForward = false;
    moveBackward = false;
    minimapcount = 10;
    boostercount = 5;
    Time = 0;

    win = false;
    sandil = false;
    snowil = false;
    treeil = false;
    cityil = false;
    shipil = false;
    complete = false;
    
    printf("게임 재시작\n");
}
void setRC() {
    GLfloat whiteLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat sourceLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat listPos[] = { -50.0f, 60.0f, -50.0f, 1.0f }; // 조명의 위치

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, listPos);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.80f, 0.80f, 0.9f, 1.0f); // 배경색
    
}
void moveRestrict() {
    if (150.0f >= shipX && shipX >= 50.0f && 150.0f >= shipZ && shipZ >= 50.0f) {
        moveForward = false; // 모래섬 정지 좌표
        attach = true;
    }

    else if (shipX > 290.0f || shipX < -290.0f || shipZ > 290.0f || shipZ < -290.0f) {
        moveForward = false;
        printf("세상의 끝입니다!\n");
        Restart();
    }

    else if (250.0f >= shipX && shipX >= 150.0f && -150.0f >= shipZ && shipZ >= -250.0f) {
        moveForward = false; // 눈섬 정지 좌표
        attach = true;
    }

    else if (250.0f >= shipZ && shipZ >= 150.0f && -150.0f >= shipX && shipX >= -250.0f) {
        moveForward = false; // 나무섬 정지 좌표
        attach = true;
    }

    else if (-203.0f >= shipX && 0.0f >= shipZ && shipZ >= -100.0f) {
        moveForward = false; // 도시섬 정지 좌표
        attach = true;
    }
    else if (40.0f>=shipX && shipX >= -35.0f && shipZ <= -200.0f) {
        moveForward = false; // 난파선 정지좌표
        attach = true;
    }

    else attach = false;
}
void Music() {
    if (music == true) {
        PlaySound(TEXT(Caribbean), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
    else {
        PlaySound(NULL, NULL, SND_PURGE);
    }
}
void EndingMusic() {
    if (complete == true) {
        PlaySound(TEXT(Win), NULL, SND_FILENAME | SND_ASYNC);
    }
}
void master() {
    sandil = true;
    treeil = true;
    snowil = true;
    cityil = true;
    shipil = true;
}
void lose() {
    Time = 175;
}

void Ocean() {
    glPushMatrix();
    glColor3f(0.429f, 0.707f, 0.98f);
    glBegin(GL_QUADS); // 바다
    glVertex3f(-290.0f, 0.0f, -290.0f);
    glVertex3f(-290.0f, 0.0f, 290.0f);
    glVertex3f(290.0f, 0.0f, 290.0f);
    glVertex3f(290.0f, 0.0f, -290.0f);
    glEnd();
    glPopMatrix();
}
void Ship() {
    glPushMatrix(); // 객체 푸시
    GLUquadric* quadric;// = gluNewQuadric(); // GLUquadric 객체 생성
    glTranslatef(0, 0, 2);

    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);

    glPushMatrix(); // 배부분 전부
    glDisable(GL_CULL_FACE);
    glColor3f(0.87, 0.72, 0.52); // 벌리우드(베이지색)
    glBegin(GL_POLYGON); // 선저
    glVertex3f(4, 0, 4);
    glVertex3f(4, 0, -4);
    glVertex3f(-4, 0, -4);
    glVertex3f(-4, 0, 4);
    glEnd();

    glBegin(GL_POLYGON); // 선저 2(선수 세모 바닥)
    glVertex3f(3, 0, 6);
    glVertex3f(-3, 0, 6);
    glVertex3f(0, 2, 10);
    glEnd();

    glBegin(GL_POLYGON); // 선상(갑판)
    glVertex3f(4, 3, 4);
    glVertex3f(4, 3, -4);
    glVertex3f(-4, 3, -4);
    glVertex3f(-4, 3, 4);
    glEnd();

    glBegin(GL_POLYGON); // 선수(선수갑판)
    glVertex3f(4, 4, 6);
    glVertex3f(-4, 4, 6);
    glVertex3f(0, 5, 12);
    glEnd();

    glBegin(GL_POLYGON); // 선미(선미갑판)
    glVertex3f(4, 4, -6);
    glVertex3f(-4, 4, -6);
    glVertex3f(-4, 4, -10);
    glVertex3f(4, 4, -10);
    glEnd();

    glBegin(GL_POLYGON); // 선미(선미갑판바닥)
    glVertex3f(3, 1, -6);
    glVertex3f(-3, 1, -6);
    glVertex3f(-3, 1, -8);
    glVertex3f(3, 1, -8);
    glEnd();

    glColor3f(0, 1, 1);
    glBegin(GL_POLYGON); // 선미(선미마감)
    glVertex3f(-3, 1, -8);
    glVertex3f(3, 1, -8);
    glVertex3f(4, 4, -10);
    glVertex3f(-4, 4, -10);
    glEnd();

    glColor3f(1, 0, 1);
    glBegin(GL_POLYGON);
    glVertex3f(3, 1, -6);
    glVertex3f(3, 1, -8);
    glVertex3f(4, 4, -10);
    glVertex3f(4, 4, -6);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(-3, 1, -6);
    glVertex3f(-3, 1, -8);
    glVertex3f(-4, 4, -10);
    glVertex3f(-4, 4, -6);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON); // 선미 옆부분
    glVertex3f(-4, 0, -4);
    glVertex3f(-3, 1, -6);
    glVertex3f(-4, 4, -6);
    glVertex3f(-4, 3, -4);
    glEnd();
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON); // 선미 옆부분
    glVertex3f(4, 3, -4);
    glVertex3f(4, 4, -6);
    glVertex3f(3, 1, -6);
    glVertex3f(4, 0, -4);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON); // 선미바닥
    glVertex3f(4, 0, -4);
    glVertex3f(-4, 0, -4);
    glVertex3f(-3, 1, -6);
    glVertex3f(3, 1, -6);
    glEnd();


    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 1
    glVertex3f(4, 3, 4);
    glVertex3f(4, 4, 6);
    glVertex3f(3, 0, 6);
    glVertex3f(4, 0, 4);
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 1-2
    glVertex3f(4, 0, 4);
    glVertex3f(4, 0, -4);
    glVertex3f(4, 3, -4);
    glVertex3f(4, 3, 4);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 2
    glVertex3f(-4, 0, 4);
    glVertex3f(-3, 0, 6);
    glVertex3f(-4, 4, 6);
    glVertex3f(-4, 3, 4);

    glEnd();
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 2-2
    glVertex3f(-4, 0, -4);
    glVertex3f(-4, 0, 4);
    glVertex3f(-4, 3, 4);
    glVertex3f(-4, 3, -4);
    glEnd();

    glColor3f(0.54, 0.27, 0.07); // 진한브라운(새들브라운)
    glBegin(GL_POLYGON); // 선수 옆부분 1
    glVertex3f(-3, 0, 6);
    glVertex3f(0, 2, 10);
    glVertex3f(0, 5, 12);
    glVertex3f(-4, 4, 6);
    glEnd();

    glBegin(GL_POLYGON); // 선수 옆부분 2
    glVertex3f(0, 2, 10);
    glVertex3f(3, 0, 6);
    glVertex3f(4, 4, 6);
    glVertex3f(0, 5, 12);
    glEnd();

    glBegin(GL_POLYGON); // 선상-선수 연결부
    glVertex3f(4, 3, 4);
    glVertex3f(-4, 3, 4);
    glVertex3f(-4, 4, 6);
    glVertex3f(4, 4, 6);
    glEnd();

    glBegin(GL_POLYGON); // 선상-선수 연결부2
    glVertex3f(4, 3, -4);
    glVertex3f(-4, 3, -4);
    glVertex3f(-4, 4, -6);
    glVertex3f(4, 4, -6);
    glEnd();
    glPopMatrix(); // 배 부분 전부 팝

    glPushMatrix();
    glTranslatef(0, 12, 5);
    glRotatef(90.0, 1, 0, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 10, 16, 16); // 돛대
    glTranslatef(-4, 0, 2);
    glRotatef(90.0, 0, 1, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 8, 16, 16); // 돛대
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 15, -5);
    glRotatef(90.0, 1, 0, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 13, 16, 16); // 돛대2
    glTranslatef(-4, 0, 2);
    glRotatef(90.0, 0, 1, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 8, 16, 16); // 돛대2
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 12, 5);
    glutSolidTorus(0.2, 1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 5);
    glColor3f(1, 1, 1);
    if (booster == 2.5f) { // 부스터 중 돛 색이 바뀜
        glColor3f(0.9, 0, 0);
    }

    if (attach == true) {
        glColor3f(1, 0.8, 1);
    }
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();
    
    if (speed >= 0.039f) {
        glTranslatef(0, -0.85, 0);
        if (booster == 2.5f) { // 부스터 중
            glColor3f(1, 0.647, 0);
        }
        if (attach == true) {
            glColor3f(1, 0.8, 1);
        }
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.059f) {
        glTranslatef(0, -0.85, 0);
        if (booster == 2.5f) { // 부스터 중
            glColor3f(1, 1, 0.5);
        }
        if (attach == true) {
            glColor3f(1, 0.8, 1);
        }
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.079f) {
        glTranslatef(0, -0.85, 0);
        if (booster == 2.5f) { // 부스터 중
            glColor3f(0, 1, 0.2);
        }
        if (attach == true) {
            glColor3f(1, 0.8, 1);
        }
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.099f) {
        glTranslatef(0, -0.85, 0);
        if (booster == 2.5f) { // 부스터 중
            glColor3f(0.2, 0.2, 0.803);
        }
        if (attach == true) {
            glColor3f(1, 0.8, 1);
        }
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    glPopMatrix();


    glPushMatrix(); // 돛 222
    glTranslatef(0, 15, -5);
    glColor3f(0.54, 0.27, 0.07);
    glutSolidTorus(0.2, 1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 3, -5);
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON); // 돛(접은거)2
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)2
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();

    if (speed >= 0.19f) {
        glTranslatef(0, -0.85, 0);
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.039f) {
        glTranslatef(0, -0.85, 0);
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.059f) {
        glTranslatef(0, -0.85, 0);
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    if (speed >= 0.079f) {
        glTranslatef(0, -0.85, 0);
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }

    if (speed >= 0.099f) {
        glTranslatef(0, -0.85, 0);
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(4, 9, 0.1);
        glEnd();
        glBegin(GL_POLYGON); // 돛(접은거)
        glVertex3f(-4, 9.7, 0.1);
        glVertex3f(0, 9.7, 0.3);
        glVertex3f(0, 9, 0.3);
        glVertex3f(-4, 9, 0.1);
        glEnd();
    }
    glPopMatrix();

    glPopMatrix(); // 객체 팝
}
void objShip() {
    glPushMatrix(); // 객체 푸시
    GLUquadric* quadric;// = gluNewQuadric(); // GLUquadric 객체 생성
    glTranslatef(0, 0, 2);

    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);

    glPushMatrix(); // 배부분 전부
    glDisable(GL_CULL_FACE);
    glColor3f(0.87, 0.72, 0.52); // 벌리우드(베이지색)
    glBegin(GL_POLYGON); // 선저
    glVertex3f(4, 0, 4);
    glVertex3f(4, 0, -4);
    glVertex3f(-4, 0, -4);
    glVertex3f(-4, 0, 4);
    glEnd();

    glBegin(GL_POLYGON); // 선저 2(선수 세모 바닥)
    glVertex3f(3, 0, 6);
    glVertex3f(-3, 0, 6);
    glVertex3f(0, 2, 10);
    glEnd();

    glBegin(GL_POLYGON); // 선상(갑판)
    glVertex3f(4, 3, 4);
    glVertex3f(4, 3, -4);
    glVertex3f(-4, 3, -4);
    glVertex3f(-4, 3, 4);
    glEnd();

    glBegin(GL_POLYGON); // 선수(선수갑판)
    glVertex3f(4, 4, 6);
    glVertex3f(-4, 4, 6);
    glVertex3f(0, 5, 12);
    glEnd();

    glBegin(GL_POLYGON); // 선미(선미갑판)
    glVertex3f(4, 4, -6);
    glVertex3f(-4, 4, -6);
    glVertex3f(-4, 4, -10);
    glVertex3f(4, 4, -10);
    glEnd();

    glBegin(GL_POLYGON); // 선미(선미갑판바닥)
    glVertex3f(3, 1, -6);
    glVertex3f(-3, 1, -6);
    glVertex3f(-3, 1, -8);
    glVertex3f(3, 1, -8);
    glEnd();

    glColor3f(0, 1, 1);
    glBegin(GL_POLYGON); // 선미(선미마감)
    glVertex3f(-3, 1, -8);
    glVertex3f(3, 1, -8);
    glVertex3f(4, 4, -10);
    glVertex3f(-4, 4, -10);
    glEnd();

    glColor3f(1, 0, 1);
    glBegin(GL_POLYGON);
    glVertex3f(3, 1, -6);
    glVertex3f(3, 1, -8);
    glVertex3f(4, 4, -10);
    glVertex3f(4, 4, -6);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(-3, 1, -6);
    glVertex3f(-3, 1, -8);
    glVertex3f(-4, 4, -10);
    glVertex3f(-4, 4, -6);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON); // 선미 옆부분
    glVertex3f(-4, 0, -4);
    glVertex3f(-3, 1, -6);
    glVertex3f(-4, 4, -6);
    glVertex3f(-4, 3, -4);
    glEnd();
    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON); // 선미 옆부분
    glVertex3f(4, 3, -4);
    glVertex3f(4, 4, -6);
    glVertex3f(3, 1, -6);
    glVertex3f(4, 0, -4);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON); // 선미바닥
    glVertex3f(4, 0, -4);
    glVertex3f(-4, 0, -4);
    glVertex3f(-3, 1, -6);
    glVertex3f(3, 1, -6);
    glEnd();


    glColor3f(1, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 1
    glVertex3f(4, 3, 4);
    glVertex3f(4, 4, 6);
    glVertex3f(3, 0, 6);
    glVertex3f(4, 0, 4);
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 1-2
    glVertex3f(4, 0, 4);
    glVertex3f(4, 0, -4);
    glVertex3f(4, 3, -4);
    glVertex3f(4, 3, 4);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 2
    glVertex3f(-4, 0, 4);
    glVertex3f(-3, 0, 6);
    glVertex3f(-4, 4, 6);
    glVertex3f(-4, 3, 4);

    glEnd();
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON); // 선체 옆부분 2-2
    glVertex3f(-4, 0, -4);
    glVertex3f(-4, 0, 4);
    glVertex3f(-4, 3, 4);
    glVertex3f(-4, 3, -4);
    glEnd();

    glColor3f(0.54, 0.27, 0.07); // 진한브라운(새들브라운)
    glBegin(GL_POLYGON); // 선수 옆부분 1
    glVertex3f(-3, 0, 6);
    glVertex3f(0, 2, 10);
    glVertex3f(0, 5, 12);
    glVertex3f(-4, 4, 6);
    glEnd();

    glBegin(GL_POLYGON); // 선수 옆부분 2
    glVertex3f(0, 2, 10);
    glVertex3f(3, 0, 6);
    glVertex3f(4, 4, 6);
    glVertex3f(0, 5, 12);
    glEnd();

    glBegin(GL_POLYGON); // 선상-선수 연결부
    glVertex3f(4, 3, 4);
    glVertex3f(-4, 3, 4);
    glVertex3f(-4, 4, 6);
    glVertex3f(4, 4, 6);
    glEnd();

    glBegin(GL_POLYGON); // 선상-선수 연결부2
    glVertex3f(4, 3, -4);
    glVertex3f(-4, 3, -4);
    glVertex3f(-4, 4, -6);
    glVertex3f(4, 4, -6);
    glEnd();
    glPopMatrix(); // 배 부분 전부 팝

    glPushMatrix();
    glTranslatef(0, 12, 5);
    glRotatef(90.0, 1, 0, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 10, 16, 16); // 돛대
    glTranslatef(-4, 0, 2);
    glRotatef(90.0, 0, 1, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 8, 16, 16); // 돛대
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 15, -5);
    glRotatef(90.0, 1, 0, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 13, 16, 16); // 돛대2
    glTranslatef(-4, 0, 2);
    glRotatef(90.0, 0, 1, 0);
    gluCylinder(quadric, 0.3f, 0.3f, 8, 16, 16); // 돛대2
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 12, 5);
    glutSolidTorus(0.2, 1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 5);
    glColor3f(1, 1, 1);

    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();

    glTranslatef(0, -0.85, 0);

    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();

    glPopMatrix();


    glPushMatrix(); // 돛 222
    glTranslatef(0, 15, -5);
    glColor3f(0.54, 0.27, 0.07);
    glutSolidTorus(0.2, 1, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -5);
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON); // 돛(접은거)2
    glVertex3f(4, 12.7, 0.1);
    glVertex3f(0, 12.7, 0.3);
    glVertex3f(0, 12, 0.3);
    glVertex3f(4, 12, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)2
    glVertex3f(-4, 12.7, 0.1);
    glVertex3f(0, 12.7, 0.3);
    glVertex3f(0, 12, 0.3);
    glVertex3f(-4, 12, 0.1);
    glEnd();


    glTranslatef(0, -0.85 + 3, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();


    glTranslatef(0, -0.85, 0);
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(4, 9, 0.1);
    glEnd();
    glBegin(GL_POLYGON); // 돛(접은거)
    glVertex3f(-4, 9.7, 0.1);
    glVertex3f(0, 9.7, 0.3);
    glVertex3f(0, 9, 0.3);
    glVertex3f(-4, 9, 0.1);
    glEnd();

    glPopMatrix();

    glPopMatrix(); // 객체 팝
}
void Tree() {
    GLUquadric* tree;
    tree = gluNewQuadric();
    gluQuadricNormals(tree, GLU_SMOOTH);

    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.54, 0.27, 0.07); // 브라운
    gluCylinder(tree, 4, 4, 50, 8, 8);

    glTranslatef(0, 0, 30);
    glColor3f(0.1, 0.6, 0.1); // 초록
    gluCylinder(tree, 20, 4, 16, 30, 30); // 높이 15 나무
    gluDisk(tree, 0, 20, 30, 30);
    glTranslatef(0, 0, 12);
    glColor3f(0.1, 0.6, 0.1); // 초록
    gluCylinder(tree, 15, 4, 12, 30, 30); 
    gluDisk(tree, 0, 15, 30, 30);
    glTranslatef(0, 0, 10);
    glColor3f(0.1, 0.6, 0.1); // 초록
    gluCylinder(tree, 10, 0, 9, 30, 30); 
    gluDisk(tree, 0, 10, 30, 30);
    glPopMatrix();

    glPopMatrix();
}
void SnowTree() {
    GLUquadric* snowtree;
    snowtree = gluNewQuadric();
    gluQuadricNormals(snowtree, GLU_SMOOTH);

    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);

    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.54, 0.27, 0.07); // 브라운
    gluCylinder(snowtree, 4, 4, 50, 8, 8);

    glTranslatef(0, 0, 30);
    glColor3f(0,0.3,0); // 초록
    gluCylinder(snowtree, 20, 4, 16, 30, 30); // 높이 15 나무
    gluDisk(snowtree, 0, 20, 30, 30);

    glTranslatef(0, 0, 4);
    glColor3f(0.9, 0.9, 0.9); //흰색
    gluCylinder(snowtree, 20, 4, 16, 30, 30);

    glTranslatef(0, 0, 8);
    glColor3f(0, 0.3, 0); // 초록
    gluCylinder(snowtree, 15, 4, 12, 30, 30);
    gluDisk(snowtree, 0, 15, 30, 30);

    glTranslatef(0, 0, 3);
    glColor3f(0.9, 0.9, 0.9); //흰색
    gluCylinder(snowtree, 15, 4, 12, 30, 30);

    glTranslatef(0, 0, 10);
    glColor3f(0, 0.3, 0); // 초록
    gluCylinder(snowtree, 10, 0, 9, 30, 30);
    gluDisk(snowtree, 0, 10, 30, 30);

    glTranslatef(0, 0, 2);
    glColor3f(0.9, 0.9, 0.9); //흰색
    gluCylinder(snowtree, 10, 0, 9, 30, 30);
    glPopMatrix();

    glPopMatrix();
}
void SnowMan(){
    glTranslatef(0, 0.75, 0);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    gluSphere(pObj, .40f, 26, 13); // 다리
    glTranslated(0.0f, .550f, 0.0f); // 중심 이동 y축 0.55 만큼 이동 - 겹쳐짐
    gluSphere(pObj, .3f, 26, 13); // 몸통
    glTranslatef(0.0f, 0.45f, 0.0f); // 중심 이동
    gluSphere(pObj, 0.24f, 26, 13); // 머리
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.1f, 0.1f, 0.21f); // 중심 이동
    gluSphere(pObj, 0.02f, 26, 13); // 눈 1
    glTranslatef(-0.2f, 0.0f, 0.0f); // 중심 이동
    gluSphere(pObj, 0.02f, 26, 13); // 눈 2
    glColor3f(1.0f, 0.3f, 0.3f);
    glTranslatef(0.1f, -0.12f, 0.0f); // 중심 이동
    gluCylinder(pObj, 0.04f, 0.0f, 0.3f, 26, 13); // 꼬깔
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 1.17f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 0.17f, 0.17f, 0.4f, 26, 13); // 기둥
    glDisable(GL_CULL_FACE);  // 안보여지는 영역 까지 그리기 위해서 disable 활성화 (뒷면)
    gluDisk(pObj, 0.17f, 0.28f, 26, 13); // 챙
    glEnable(GL_CULL_FACE);
    glTranslatef(0.0f, 0.0f, 0.40f);
    gluDisk(pObj, 0.0f, 0.17f, 26, 13); // 뚜껑
    glPopMatrix();
} // 눈사람
void SnowIsland() {
    glPushMatrix();
    glTranslatef(200.0, -5.0f, -200.0); // 섬 중심좌표

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glColor3f(0.933, 0.911f, 0.911f); // 스노우
    glutSolidTorus(10,37,20,30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 9.9f, 0);
    glRotatef(-90, 1, 0, 0);
    gluDisk(pObj, 0.0f, 37.0f, 26, 26); // 섬바닥
    glPopMatrix();


    // 큰 눈사람
    glPushMatrix();
    glScalef(20, 20, 20);
    glRotatef(-15, 0, 1, 0);
    glTranslatef(-0.2, 0.1, 0.5);
    SnowMan();
    glPopMatrix();

    // 애기 눈사람
    glPushMatrix();
    glScalef(15, 15, 15);
    glRotatef(-30, 0, 1, 0);
    glTranslatef(1.5, 0.1, 0.5);
    SnowMan();
    glPopMatrix();

    // 눈맞은나무
    glPushMatrix();
    glScalef(3, 3, 3);
    glTranslatef(5, 2, -7);
    SnowTree();
    glPopMatrix();

    glPushMatrix();
    glScalef(2, 2, 2);
    glTranslatef(-5, 2, -7);
    SnowTree();
    glPopMatrix();



    glPopMatrix();
}
void SandIsland() {
    glPushMatrix();

    glPushMatrix();
    glTranslatef(100.0, 4, 100.0); // 섬 중심좌표
    glColor3f(0.933f, 0.86f, 0.53f); // 아이보리
    glPushMatrix();
    glTranslatef(0, -43, 0);
    glutSolidCube(85);
    glPopMatrix();

    glRotatef(180, 0, 1, 0);

    glTranslatef(10, 0, 10);
    glRotatef(90, 1, 0, 0); // 피라미드
    glScalef(1.1, 1.1, 1.7);
    glColor3f(0.933f, 0.86f, 0.53f); // 아이보리
    glutSolidTorus(4, 20, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 16, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 12, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 8, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 4, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(3, 1, 5, 4);

    glTranslatef(-27, -22, 20);
    glColor3f(0.80f, 0.80f, 0.73f); // 아2보리
    glutSolidTorus(4, 12, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 8, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(4, 4, 5, 4);
    glTranslatef(0, 0, -4);
    glutSolidTorus(3, 1, 5, 4);
    glPopMatrix();

    glPopMatrix();
}
void TreeIsland() {
    glPushMatrix();
    glTranslatef(-200.0, -5.0f, 200.0); // 섬 중심좌표

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glColor3f(0.85, 0.72f, 0.52f); // 흙
    glutSolidTorus(10, 38, 20, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, 0);
    glRotatef(-90, 1, 0, 0);
    gluDisk(pObj, 0.0f, 38.0f, 26, 26); // 섬바닥
    glPopMatrix();
    
   
    glPushMatrix();
    glTranslatef(0, 3, 0);
    glScalef(0.95, 0.8, 0.95);
    glRotatef(90, 1, 0, 0);
    glColor3f(0.133,0.545,0.133); // 잔디
    glutSolidTorus(10, 37, 20, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 11, 0);
    glRotatef(-90, 1, 0, 0);
    gluDisk(pObj, 0.0f, 37.0f, 26, 26); // 섬바닥
    glPopMatrix();

    glTranslatef(0, 2, 0);
    glPushMatrix();
    glTranslatef(0, 0, 10);
    glScalef(1.8, 2.5, 1.8);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glScalef(1.5, 2, 1.5);
    glTranslatef(-7, 0, -7);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5, 0, -15);
    glScalef(1, 1.5, 1);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20, 0, 0);
    glScalef(1, 1.5, 1);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20, 0, -20);
    glScalef(0.6, 1, 0.6);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -25);
    glScalef(0.6, 1, 0.6);
    Tree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-24, 0, -14);
    glScalef(0.6, 1, 0.6);
    Tree();
    glPopMatrix();

    glPopMatrix();
}
void Building() {
    glPushMatrix();
    GLUquadric* Building;
    Building = gluNewQuadric();
   
    glPushMatrix();
    
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glRotatef(45, 0, 0, 1);
    glColor3f(0.41f, 0.41f, 0.41f); // 회색(건물 색)
    gluCylinder(Building, 6, 6, 40, 4, 4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 38, 0);
    glColor3f(0.80f, 0.80f, 0.73f); // 아2보리
    glutSolidCube(8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 6);
    glRotatef(-90, 1, 0, 0);
    glRotatef(45, 0, 0, 1);
    glColor3f(0.41f, 0.41f, 0.41f); // 회색
    gluCylinder(Building, 6, 6, 40, 4, 4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 38, 6);
    glColor3f(0.80f, 0.80f, 0.73f); // 아2보리
    glutSolidCube(8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -6);
    glRotatef(-90, 1, 0, 0);
    glRotatef(45, 0, 0, 1);
    glColor3f(0.41f, 0.41f, 0.41f); // 회색
    gluCylinder(Building, 6, 6, 40, 4, 4);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 38, -6);
    glColor3f(0.80f, 0.80f, 0.73f); // 아2보리
    glutSolidCube(8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5, 35, 5);
    glScalef(1, 0.8, 1);
    glColor3f(0.67f, 0.84f, 0.90f); // 창문
    glutSolidCube(5);
    glTranslatef(0, 0, -10);
    glutSolidCube(5);
    glTranslatef(0, -8, 0);
    glutSolidCube(5);
    glTranslatef(0, 0, 10);
    glutSolidCube(5);
    glTranslatef(0, -8, 0);
    glutSolidCube(5);
    glTranslatef(0, 0, -10);
    glutSolidCube(5);
    glTranslatef(0, -8, 0);
    glutSolidCube(5);
    glTranslatef(0, 0, 10);
    glutSolidCube(5);
    glTranslatef(0, -8, 0);
    glutSolidCube(5);
    glTranslatef(0, 0, -10);
    glutSolidCube(5);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    glPopMatrix();


}
void CityIsland() {
    glPushMatrix(); // (1)
    glTranslatef(-248, 2, -48.0); // 섬 중심좌표
    
    glPushMatrix(); //(3)
    glColor3f(0.80f, 0.80f, 0.73f); // 아2보리
    glTranslatef(0, -38, 0); // 윗부분만 보이기 위해 가라앉힘
    glutSolidCube(80);
    glPopMatrix(); // 섬 바닥 (3)

    glPushMatrix(); // (2)
    //Building();
    glTranslatef(-25, 0, 25);
    Building(); // 아파트 1
    glTranslatef(-25, 0, -5);
    Building(); // 아파트 2
    
    glPushMatrix();// (4)
    glTranslatef(-250, 4, -50.0);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(-25, 0, 5);
    Building(); // 아파트 3
    glPopMatrix(); //4

    glPushMatrix();//5
    glTranslatef(-250, 4, -50.0);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(-25, 0, -25);
    Building(); // 아파트 4;
    glPopMatrix();//5

    glPushMatrix();//6
    glTranslatef(-280, 4, -50.0);
    glTranslatef(30, 0, 0);
    glScalef(0.5, 0.5, 0.5);
    Tree();
    glTranslatef(45, 0, 0);
    Tree();
    glTranslatef(0, 0, 45);
    Tree();
    glTranslatef(-45, 0, 0);
    Tree();
    glPopMatrix();//7
    
    glPopMatrix(); // (2)
 
    
    glPopMatrix(); // (1)
}
void DrawUI() {
    if (view == true) { // 섬 체크 유무판
        glViewport(0, 0, OW / 6, OH / 4);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 2.2, -1.0, 1.0);

        glPushMatrix();
        glColor3f(1, 1, 0.2); // 모래섬
        if (sandil == true) {
            glColor3f(0.2, 0.2, 1);
        }
        glTranslatef(0.55, 0.55, 0);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.2, 1, 0); // 나무섬
        if (treeil == true) {
            glColor3f(0.2, 0.2, 1);
        }
        glTranslatef(-0.55, 0.55, 0);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.54, 0.27, 0.07); // 난파선 시그니처 컬러
        if (shipil == true) {
            glColor3f(0.2, 0.2, 1);
        }
        glTranslatef(-0.55, 1.7, 0);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glColor3f(1, 1, 1); // 눈섬
        if (snowil == true) {
            glColor3f(0.2, 0.2, 1);
        }
        glTranslatef(0.55, -0.55, 0);
        glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.5, 0.5, 0.5); // 도시섬
        if (cityil == true) {
            glColor3f(0.2, 0.2, 1);
        }
        glTranslatef(-0.55, -0.55, 0);
        glutSolidCube(1);
        glPopMatrix();

        glViewport(0, 0, OW, OH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70.0f, aspect, 0.1f, 500.0f); // 원근 투영 설정
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (view == true) {
        glViewport(OW/1.5, OH/1.1, OW / 3, OH / 12);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, gameTime/10 , -1, 1, -1.0, 1.0);
        glColor3f(0, 1, 0);

        glColor3f(0.9, 0, 0.1);
        glBegin(GL_QUADS); // 시간제한표 지난시간
        glVertex3f(0,-0.5,0);
        glVertex3f(0, 0.5, 0);
        glVertex3f(0+Time*0.1, 0.5, 0);
        glVertex3f(0+Time*0.1, -0.5, 0);
        glEnd();

        glColor3f(0, 0, 0);
        glBegin(GL_QUADS); // 시간제한표 남은시간
        glVertex3f(0 + Time * 0.1, 0.5, 0);
        glVertex3f(gameTime/10, 0.5, 0);
        glVertex3f(gameTime/10, -0.5, 0);
        glVertex3f(0 + Time * 0.1, -0.5, 0);
        glEnd();

        glViewport(0, 0, OW, OH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70.0f, aspect, 0.1f, 500.0f); // 원근 투영 설정
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}
void SinkShip() {
    glPushMatrix();
    glTranslatef(0, 0, -255);
    glTranslatef(0, 10, 0);
    glRotatef(45, 1, 0, -1);
    glScalef(4, 4, 4);
    objShip();
    glPopMatrix();
}


void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // 모델뷰 변환 설정
    glLoadIdentity(); // 초기화
    
    Camera();
    //designCam();
    SnowIsland();
    SandIsland();
    Ocean();
    SinkShip();
    TreeIsland();
    CityIsland();
    mukae();
  
    viewchange();   
    DrawUI();
    moveRestrict(); // 장애물함수
    setRC(); // 조명
    
    glutPostRedisplay();
    glutSwapBuffers();
}
void MyReshape(int NewWidth, int NewHeight) {
    aspect = (float)NewWidth / (float)NewHeight;
    OW = NewWidth;
    OH = NewHeight;

    glViewport(0, 0, NewWidth, NewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, aspect, 0.1f, 500.0f); // 원근 투영 설정
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//aspect 변수를 통해 윈도우 창의 가로 세로 비율을 계산
//가로가 더 긴 경우(1.0 이상)에는 width 변수를 계산하여 x 좌표 범위를 윈도우 창에 맞게 설정
//세로가 더 긴 경우(1.0 미만)에는 height 변수를 계산하여 y 좌표 범위를 윈도우 창에 맞게 설정
//설정된 좌표 범위를 glOrtho 함수를 통해 전달하여 출력합니다.

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
    if (KeyPressed == 'f' && moveForward == true && speed > 0 && speed < 0.099f) {
        speed += 0.02f;
        if (speed >= 0.1f) {
            speed = 0.1f;
        }
        roundspeed += 0.25f;
        if (roundspeed >= 1.5f) {
            roundspeed = 1.5f;
        }
        printf("스피드 : %.2f Knot 회전 속도: %.2f Km \n", speed * 50, roundspeed * 10);
    }
    else if (KeyPressed == 'g' && moveForward == true && speed >= 0.021) {
        speed -= 0.02f;
        if (speed <= 0.005f) {
            speed = 0.02f;
        }
        roundspeed -= 0.25f;
        if (roundspeed <= 0.5f)
            roundspeed = 0.5f;
        printf("스피드 : %.2f Knot 회전 속도: %.2f Km \n", speed * 50, roundspeed * 10);
    }

    if (KeyPressed == 'v') {
        printf("현재 X좌표 : %f 현재 Z좌표 : %f\n", shipX, shipZ);
    }

    if (KeyPressed == 32) { // 스페이스바
        moveForward = false;
        moveBackward = false;
        speed = 0.0f;
        roundspeed = 0.5f;
        printf("정지\n");
    }

    if (KeyPressed == 'm') {
        master();
    }
    if (KeyPressed == 'l') {
        lose();
    }
    if (KeyPressed == 'r') {
        Restart();
    }

    if (KeyPressed == 'q') {
        CheckX++;
    }
    if (KeyPressed == 'w') {
        CheckY++;
    }
    if (KeyPressed == 'e') {
        CheckZ++;
    }
    if (KeyPressed == 'a') {
        CheckX--;
    }
    if (KeyPressed == 's') {
        CheckY--;
    }
    if (KeyPressed == 'd') {
        CheckZ--;
    }
    glutPostRedisplay();
}
void MySpecial(int key, int X, int Y) {
    if (key == GLUT_KEY_LEFT)
        xRot += (0.5f * roundspeed);
    else if (key == GLUT_KEY_RIGHT)
        xRot -= (0.5f * roundspeed);

    if (key == GLUT_KEY_UP && speed <= 0.01) {
        moveForward = true;
        moveBackward = false;
        speed = 0.02f;
        roundspeed = 0.5f;
        printf("스피드 : %.2f Knot 회전 속도: %.2f Km \n", speed * 50, roundspeed * 10);

    }
    else if (key == GLUT_KEY_DOWN && downkey == true && moveBackward == false) {
        moveBackward = true;
        moveForward = false;
        speed = -0.04f;
        roundspeed = 1.0f;
        printf("후진속도 : %.2f Knot 회전 속도: %.2f Km \n", speed * -50, roundspeed * 10);
    }
}
void MyTimer(int Value) {
    if (moveForward == true && speed >= 0) {
        cammove = cammove + 0.002 * speed * 0.9 * 200 * downmove; // 위아래로 움직이는 속도 speed 단계에 따라 달라짐
        if (cammove >= 1.5) {
            downmove = -1.0;
        }
        else if (cammove < -0.5) {
            downmove = 1.0;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(10, MyTimer, true);
}
void MyTimer2(int Value) {
    if (win == false) {
        Time++;
        if (Time >= gameTime) {
            printf("타임 오버 하였습니다.\n");
            Restart();
        }
        glutTimerFunc(1000, MyTimer2, 0); // 1초마다 타이머 콜백 함수 호출, 1초씩 time에 누적
    }
}
void MenuProc(int entryID) {
    if (entryID == 0) {
        printf("==================== How To Play =========================\n");
        printf("| 위 방향키 입력 시 배가 직진합니다.                   \n");
        printf("| 아래 방향키 입력 시 배가 후진합니다.                  \n");
        printf("| 좌우 방향키 입력 시 배가 회전합니다.                  \n");
        printf("| 스페이스바 입력 시 배가 정지합니다.                   \n");
        printf("==========================================================\n");
        printf("| f키 입력시 배의 속도가 증가합니다.                   \n");
        printf("| g키 입력시 배의 속도가 감소합니다.                   \n");
        printf("==========================================================\n");
        printf("| 메뉴를 통해 부스터를 2초간 사용할 수 있습니다.        \n");
        printf("| 메뉴를 통해 미니맵을 3초간 확인할 수 있습니다.        \n");
        printf("| 메뉴를 통해 각 섬을 방문체크 할 수 있습니다.          \n");
        printf("| 메뉴를 통해 게임을 재시작 할 수 있습니다.             \n");
        printf("| 메뉴를 통해 배경음악을 켜고 끌 수 있습니다.           \n");
        printf("==========================================================\n");
        printf("| ※ 임무목표 ※                                       \n");
        printf("| 1. 각 섬을 순서에 관계없이 방문하여 체크합니다.        \n");
        printf("|    체크 시 왼쪽하단 섬의 시그니쳐 컬러가 파랗게 변합니다.\n");
        printf("| 2. 모든 섬 체크 후, 골인 지점에서 완료버튼을 누르면 승리!\n");
        printf("| 3. 부스터와 미니맵의 사용 횟수에는 제한이 있습니다.      \n");
        printf("| 4. 시간제한에 유의하여 플레이 하셔야 합니다!  \n");
        printf("==========================================================\n");
        printf("| *주의사항*\n");
        printf("| 1. 지형에 충돌 시 배가 정지하고 돛이 분홍색으로 변합니다.\n");
        printf("| 2. 후진을 통해 빠져나온 뒤 이어서 진행합니다.\n");
        printf("| 3. 시간초과 시  패배합니다.\n");
        printf("| 4. 미니맵을 통해 자신의 위치를 파악해보세요.\n");
        printf("| 5. 부스터는 최대 속도(5노트) 도달 시 사용가능합니다.\n");
        printf("==========================================================\n");
    }
    if (entryID == 1 && minimapcount > 0) {
        view = false;
        dKey = true;
        glutTimerFunc(3000, [](int) { view = true; dKey = false; }, 0); // 3초 후 미니맵 꺼짐
        minimapcount -= 1;
        printf("남은 미니맵 횟수 : %d\n", minimapcount);
    }
    else if (entryID == 1 && minimapcount <= 0) {
        printf("남은 미니맵이 없습니다\n");
    }

    if (entryID == 2 && speed >= 0.099f && boostercount > 0) {
        booster = 2.5f;
        speed = 0.175f;
        downkey = false;
        glutTimerFunc(2000, [](int) {  booster = 1.0f; }, 0); // 2초 간 5배 부스터 ( 이동 속도 )
        glutTimerFunc(2000, [](int) {  speed = 0.1f; }, 0); // 2초 간 5배 부스터 ( 화면 흔들림)
        glutTimerFunc(2000, [](int) {  downkey = true; }, 0); // 2초 간 후진 비활성화
        printf("BOOSTER!!\n");
        boostercount -= 1;
        printf("남은 부스터 횟수 : %d\n", boostercount);
    }
    else if (entryID == 2 && speed >= 0.05 && boostercount <= 0) {
        printf("남은 부스터가 없습니다\n");
    }
    else if (entryID == 2 && speed <= 0.05 && boostercount > 0) {
        printf("아직 부스터를 사용할 수 없습니다\n");
    }

    if (entryID == 31) {
        if (150.0f >= shipX && shipX >= 50.0f && 150.0f >= shipZ && shipZ >= 50.0f && sandil == false) {
            sandil = true;
            printf("  모래섬을 체크했습니다!\n");
        }
    }
    else if (entryID == 32) {
        if (250.0f >= shipX && shipX >= 150.0f && -150.0f >= shipZ && shipZ >= -250.0f && snowil == false) {
            snowil = true;
            printf("  눈 섬을 체크했습니다!\n");
        }
    }
    else if (entryID == 33) {
        if (250.0f >= shipZ && shipZ >= 150.0f && -150.0f >= shipX && shipX >= -250.0f && treeil == false) {
            treeil = true;
            printf("  나무섬을 체크했습니다!\n");
        }
    }
    else if (entryID == 34) {
        if (-203.0f >= shipX && 0.0f >= shipZ && shipZ >= -100.0f && cityil == false) {
            cityil = true;
            printf("  도시섬을 체크했습니다!\n");
        }
    }
    else if (entryID == 35) {
        if (40.0f >= shipX && shipX >= -35.0f && shipZ <= -200.0f && shipil == false) {
            shipil = true;
            printf(" 난파선을 체크했습니다.\n");
        }
    }

    if (entryID == 41) {
        music = true;
        Music();
        printf("BGM ON\n");
    }
    else if (entryID == 42) {
        music = false;
        Music();
        printf("BGM OFF\n");
    }

    if (entryID == 5) {
        if (sandil == true && snowil == true && treeil == true && cityil == true && shipil == true) {
            printf("게임에서 승리하셨습니다 !!!\n");
            win = true;
            complete = true;
            EndingMusic();
            printf("잠시 후 게임이 종료 됩니다 !!!\n");
            glutTimerFunc(5000, [](int) { exit(0); }, 0); // 5초 후 종료
        }
    }
    if (entryID == 6) {
        Restart();
    }

    if (entryID == 7) {
        exit(0);
    }

    glutPostRedisplay();
}
void MenuFunc() {
    GLint MySubMenuCheck = glutCreateMenu(MenuProc); // 서브메뉴 기능 활성화
    glutAddMenuEntry("SandIsland", 31);
    glutAddMenuEntry("SnowIsland", 32);
    glutAddMenuEntry("TreeIsland", 33);
    glutAddMenuEntry("CityIsland", 34);
    glutAddMenuEntry("SinkShip", 35);

    GLint MySubMenuMusic = glutCreateMenu(MenuProc);
    glutAddMenuEntry("ON", 41);
    glutAddMenuEntry("OFF", 42);

    // 위는 전부 서브메뉴, 메인메뉴 이전에 선언 해 놓아야함
     // 다단메뉴의 엔트리 아이디는 구분 가능하게 할당해야 편함

    GLint MyMainMenu = glutCreateMenu(MenuProc); // 메인메뉴 기능활성화
    
    glutAddMenuEntry("How to play", 0);
    glutAddMenuEntry("Minimap", 1);
    glutAddMenuEntry("Booster", 2);
    glutAddSubMenu("Check", MySubMenuCheck);
    glutAddSubMenu("Music", MySubMenuMusic);
    glutAddMenuEntry("Complete", 5);
    glutAddMenuEntry("Restart", 6);
    glutAddMenuEntry("Exit", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON); // Menu를 붙이기 위한 버튼(현재 코드에서는 우클릭 시 메뉴 호출)
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("대항해시대");
    glClearColor(0.93, 0.93, 0.871, 1.0);

    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutKeyboardFunc(MyKeyboard);
    glutSpecialFunc(MySpecial);
    glutTimerFunc(40, MyTimer, 0);
    glutTimerFunc(1000, MyTimer2, 0);
    MenuFunc();

    glutMainLoop();
    return 0;
}