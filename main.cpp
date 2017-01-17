#include <fstream>
#include <vector>
#include <GL/glut.h>
#include<time.h>
#include "Ball.h"
#include "Boundary.h"
#include "Brick.h"
#include "Shape.h"
#include "Stick.h"
#include "Item.h"
#include "vec3.h"

#define CANVAS_WIDTH 600.
#define CANVAS_HEIGHT 600.
#define WINDOW_WIDTH 1000.
#define WINDOW_HEIGHT 700.
#define BALL_RADIUS 10
#define BALL_SPEED 500//500
#define COLUMNS 10
#define ROWS 10
#define PI 3.141592
#define BEZEL 10.
#define DEPTH 50.

// Game CONDITION
#define INITIAL_LIFE 3
#define TIME_LIMIT_FIRE 5
#define TIME_LIMIT_FOG 10
#define TIME_LIMIT_BOOST 3

#define fps 60
const float dt = (1.f / fps); // dt이름을 함수에서 사용하기 위함.

#define sgn(x) ((x)>0?(-1):(1))

using namespace std;

Stick* stick = NULL;
vector<Ball*>balls;
vector<Brick*> bricks;
vector<Item*> items;
Boundary* deadBoundary = NULL; // 닿으면 죽는다.

bool isGamePlaying = false;
bool isGameOver = false;
int score = 0;
int level = 1;
int life = INITIAL_LIFE;

void destroyBrick(Brick* brick);
void destroyBricksAll();

void destroyItem(Item* item);
void destroyItemsAll();

void destroyBall(Ball* ball);
void destroyBallsAll();

void destroyAll();


bool moveBall(Ball* ball);
bool moveItem(Item* item);
bool moveStick(Stick* stick);

void GameOver();
void Idle();

void keySpecial(int key, int x, int y);
void keyUpSpecial(int key, int x, int y);
void keyOrdinary(unsigned char key, int x, int y);

void mouseClick(int button, int state, int x, int y);
void mouseMotion(int x, int y);

void initGames(int _level);
void initBricks(char* path);
void initStick();
void initBalls();

void exitGame();

void applyItem(int type);
void boostMode(bool sw);
void removeItemEffectAll();
void glutPrint(float x, float y, float z, void* font, char* text, float r, float g, float b, float a);

int fogtime;
int firetime;
int boosttime;
float deadtime;
float cam_theta = 0;

int observed_fps = 0;

int collideMode = 0;

// mouse_moment : dt초동안 마우스로입력받은 스틱의 움직임이다. 이를 이용해 속도벡터를 갱신하는데, 직접 스틱을 움직이지 않고
//                이 방법을 쓰는 이유는 스틱을 빠르게 움직일 때 다른 물체와의 충돌을 정상적으로 인식하려면 공의 움직임이 속도
//                벡터로 부터 나와야 한다.(충돌 계산함수는 이를 이용하였기 때문이다.)
Vec3 mouse_moment(0,0,-1);
bool isKeyDown = false;

// destroyBrick Function : 특정 Brick을 파괴한다.
void destroyBrick(Brick* brick){
	// brick리스트에서 제거
	for (std::vector<Brick*>::iterator iter = bricks.begin(); iter != bricks.end();){
		if ((*iter) == brick)
			iter = bricks.erase(iter);
		else
			++iter;
	}
	// 아이템 생성
	if (brick->item != NULL)
		items.push_back(brick->item);

	// 브릭 제거
	delete brick;
}

// destroyItem Function : 특정 Item을 파괴한다.
void destroyItem(Item* item){
	for (std::vector<Item*>::iterator iter = items.begin(); iter != items.end();){
		if ((*iter) == item)
			iter = items.erase(iter);
		else
			++iter;
	}
	delete item;
}

// destroyBall Function : 특정 Ball을 파괴한다.
void destroyBall(Ball* ball){
	for (std::vector<Ball*>::iterator iter = balls.begin(); iter != balls.end();){
		if ((*iter) == ball)
			iter = balls.erase(iter);
		else
			++iter;
	}
	delete ball;
}

// destroyItemsAll Function : 모든 아이템들을 지우고 목록에서 제거한다.
void destroyItemsAll(){
	for (int i = 0; i < items.size(); i++)
		delete items[i];
	items.clear();
}

// destroyBallsAll Function : 모든 공들을 지우고 목록에서 제거한다.
void destroyBallsAll(){
	for (int i = 0; i < balls.size(); i++)
		delete balls[i];
	balls.clear();
}

// destroyBricksAll Function : 스틱과 테두리를 포함한 모든 브릭들을 지우고 목록에서 제거한다.
void destroyBricksAll(){
	for (int i = 0; i < bricks.size(); i++) // stick객체와 boundary객체도 같이 제거된다.
		delete bricks[i];
	bricks.clear();
	stick = NULL;
	deadBoundary = NULL;
}

// destroyAll Function : 모든 공, 아이템, 브릭들을 제거한다.
void destroyAll(){
	destroyBallsAll();
	destroyItemsAll();
	destroyBricksAll();
}

//removeItemEffectAll Function : 모든 아이템 표과를 지운다.
void removeItemEffectAll(){
	if (stick != NULL)	stick->setType(0);
	for (int i = 0; i < balls.size(); i++)
		balls[i]->setType(0);
	boostMode(false);
	firetime += TIME_LIMIT_FIRE * 10000;
	fogtime += TIME_LIMIT_FOG * 10000;
	boosttime += TIME_LIMIT_BOOST * 10000;
}


// 0~dt초 사이 행동을 예측하고 공을 움직인다.
bool moveBall(Ball* ball){

	// collision detection
	float restTime = dt;

	while (restTime>0){

		float minTime = restTime;
		Brick* minBrick = NULL;

		// 충돌예측시간이 가장 짧은 Brick을 찾는다.
		for (int i = 0; i < bricks.size(); i++){

			if (ball->isCollision(bricks[i], restTime)){
				float t = ball->getCollisionTime(bricks[i], restTime);
				if (minTime > t)	minTime = t, minBrick = bricks[i];
			}
		}

		// 충돌처리한다.
		Vec3 normal = ball->getCollisionNormalVector(minBrick, restTime); // 계산과정에서 좌표가 쓰여, move를 쓰기 전에 계산해야한다.

		ball->move(minTime * ball->velocity);
		// 불공이면 Stick, Boundary이외에 방향을 바꾸지 않는다.
		if ( ( minBrick != NULL && minBrick->getLife() < 0 ) || !ball->hasType(STATE_BALL_FIRE)){
			if (minBrick != NULL && minBrick->getLife() == -2 && normal.y != 0){ // 스틱 윗면과 충돌한 경우
				Vec3 col_pos = ball->center + minTime * ball->velocity - Vec3(0,ball->radius,0);
				float delta_rate = (col_pos.x - stick->center.x) / stick->width; // 왼쪽끝일때 -0.5, 오른쪽끝일때 0.5
				float vel = ball->velocity.size();
				float theta = (PI / 4) * delta_rate / 0.5;
				ball->velocity = Vec3(vel*sin(theta), vel*cos(theta),0);
			}else {
				ball->velocity = ball->velocity - 2 * ball->velocity.projTo(normal);
			}
		}
		restTime -= minTime;

		//블록 파괴
		if (minBrick != NULL){
			// 불공이면 Stick, Boundary이외에 무조건 파괴한다.
			if (ball->hasType(STATE_BALL_FIRE)){
				if (minBrick->getLife() > 0)
					destroyBrick(minBrick);
			}
			else{
				if (minBrick->getLife() > 0){
					minBrick->hit();
					score += 10;
				}
				if (minBrick->getLife() == 0)
					destroyBrick(minBrick);
			}
		}
		
		//공 파괴
		if (minBrick == deadBoundary){
			destroyBall(ball);
			return false;
		}
	}
	return true;
}

bool moveStick(Stick* stick){

	stick->move(stick->velocity * (dt));

	if (stick->center[0] - stick->width / 2 < 0){
		stick->center[0] = stick->width / 2;
		stick->velocity = Vec3(0, 0, 0);
	}
	if (stick->center[0] + stick->width / 2 - CANVAS_WIDTH > 0){
		stick->center[0] = CANVAS_WIDTH - stick->width / 2;
		stick->velocity = Vec3(0, 0, 0);
	}

	return true;
}


void applyItem(int type){
	float fogColor[4] = { 1, 1, 1, 1 };

	switch (type){

		// 전체 지우기
	case ITEM_ERASE:
		removeItemEffectAll();
		break;

		// 환경관련 아이템
	case ITEM_ENV_FOG:
		fogtime = clock();
		glFogf(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_START, 600.0f);
		glFogf(GL_FOG_END, 750.0f);
		glEnable(GL_FOG);
		break;

	case ITEM_ENV_BOOST:
		boosttime = clock();
		boostMode(true);
		break;

		// 공관련 아이템
	case ITEM_BALL_FAST:
	case ITEM_BALL_SLOW:
	case ITEM_BALL_FIRE:
		firetime = clock();
		for (int i = 0; i < balls.size(); i++)
			balls[i]->applyType(type);
		break;

	case ITEM_BALL_MULTIPLE:
		int size = balls.size(); // 안에서 사이즈가 바뀌기 때문
		for (int i = 0; i < size; i++){
			//공1번째 추가
			Ball* new_ball = new Ball(*(balls[i]));
			float vx = new_ball->velocity.x, vy = new_ball->velocity.y, vz = new_ball->velocity.z;
			float theta = atan(vy / vx);
			float cos = cosf(PI / 12.5);
			float sin = sinf(PI / 12.5);
			new_ball->setVel(Vec3(cos*vx - sin*vy, sin*vx + cos*vy, vz));
			new_ball->setCollideMode(collideMode);
			balls.push_back(new_ball);
			//공 2번째 추가
			new_ball = new Ball(*(balls[i]));
			vx = new_ball->velocity.x, vy = new_ball->velocity.y, vz = new_ball->velocity.z;
			new_ball->setVel(Vec3(cos * vx + sin * vy, -sin * vx + cos * vy, vz));
			new_ball->setCollideMode(collideMode);
			balls.push_back(new_ball);
		}
		break;

	}

}


bool moveItem(Item* item){

	item->move(item->velocity * (dt));

	// 아이템 적용
	if (item->isCollision(stick, dt)){
		score += 30;
		applyItem(item->type);
		destroyItem(item);
	}

	// 아이템 제거
	if (item->isCollision(deadBoundary, dt)){
		destroyItem(item);
		return false;
	}

	return true;
}

void boostMode(bool sw){
	static bool boostMode = false;
	static float* bv = NULL;
	static float stick_width = 0;
	static Vec3 stick_ctr;
	if (sw == boostMode)	return;
	boostMode = sw;
	if (boostMode){
		// boost on
		bv = new float[balls.size()];
		for (int i = 0; i < balls.size(); i++){
			bv[i] = balls[i]->velocity.size();
			balls[i]->velocity.normalize();
			balls[i]->velocity *= 8000;
		}
		if (stick != NULL){
			stick_ctr = stick->center;
			stick_width = stick->width;
			stick->center.x = CANVAS_WIDTH / 2;
			stick->width = CANVAS_WIDTH;
		}
	}
	else{
		// boost off
		for (int i = 0; i < balls.size(); i++){
			balls[i]->velocity.normalize();
			balls[i]->velocity *= bv[i];
		}
		if (stick != NULL){
			stick->center = stick_ctr;
			stick->width = stick_width;
		}
	}
}

void GameOver(){
	isGamePlaying = false;
	isGameOver = true;
	removeItemEffectAll();
	deadtime = 0;
}

void exitGame(){
	// 메모리 해제
	destroyAll();
	// 종료
	exit(0);
}


// Idle Function : 언제나 실행되며 dt초마다 변화를 수행한다.
void Idle(){
	static int time = clock();

	if (clock() - time >= dt*1000){
		//시간 갱신
		time = clock();
		if (isGameOver){
			deadtime += 8*dt;
			cam_theta += (PI / 2)*(deadtime*exp(-deadtime))*8*dt;
		}
		else if (cam_theta > 0){
			cam_theta -= PI*dt;
			if (cam_theta < 0)	cam_theta = 0;
		}

		//속도 갱신
		if (!isKeyDown)
			stick->velocity.x = mouse_moment.x / dt;
		mouse_moment *= 0;

		// 스틱을 움직인다.
		moveStick(stick);

		// 모든 아이템을 움직인다
		for (int i = 0; i < items.size(); i++){
			if (!moveItem(items[i]))
				i--; // 아이템을 제거해 개수가 변하는 경우
		}

		if (isGamePlaying){
			// 모든 공을 움직인다
			for (int i = 0; i < balls.size(); i++){
				if (!moveBall(balls[i]))
					i--; // 공을 제거해 개수가 변하는 경우
			}

			// 공이 하나도 남지 않으면 재시작
			if (balls.size() == 0){
				life--;
				removeItemEffectAll();
				destroyBrick(stick);

				// 게임종료
				if (life < 0){
					GameOver();
					glutPostRedisplay();
					return;
				}

				initStick();
				initBalls();
				isGamePlaying = false;
			}
		}
		else{
			// 모든 공은 초기에 스틱 위에있다.
			for (int i = 0; i < balls.size(); i++){
				balls[i]->setPos(stick->center + Vec3(0, stick->height / 2 + balls[i]->radius, 0));
			}
		}

		//변경사항 디스플레이
		glutPostRedisplay();
	}

	// 안개가 옅어진다.
	if (glIsEnabled(GL_FOG)){
		float t_delta = (float)(clock() - fogtime) / 1000;
		glFogf(GL_FOG_END, 750.0f + 70*t_delta);
		if (t_delta > TIME_LIMIT_FOG){
			glDisable(GL_FOG);
		}
	}
	// 불공 아이템 효과가 사라진다.
	for (int i = 0; i < balls.size(); i++){
		float t_delta = (float)(clock() - firetime) / 1000;
		if (t_delta > TIME_LIMIT_FIRE && balls[i]->hasType(STATE_BALL_FIRE))
			balls[i]->removeType(STATE_BALL_FIRE);
	}
	// 부스트모드를 끈다.
	if ((float)(clock() - boosttime) / 1000 > TIME_LIMIT_BOOST)
		boostMode(false);

	// 모든 블록을 깬경우.
	if (bricks.size() == 5){ // 상하좌우 벽, 스틱
		initGames(++level);
	}
}


// keySpecial Function : 키 입력을 받아 스틱의 위치를 조정한다.
void keySpecial(int key, int x, int y){

	if (isGameOver){
		score = 0;
		life = INITIAL_LIFE;
		initGames(1);
		return;
	}

	isKeyDown = true;

	switch (key){
	case GLUT_KEY_LEFT:
		if (stick != NULL)
			stick->velocity = Vec3(-300, 0, 0);
		break;
	case GLUT_KEY_RIGHT:
		if (stick != NULL)
			stick->velocity = Vec3(300, 0, 0);
		break;
	}
	glutPostRedisplay();
}

// keyUpSpecial Function : 키를 떼면 멈춘다.
void keyUpSpecial(int key, int x, int y){
	stick->setVel(Vec3(0, 0, 0));
	isKeyDown = false;
	mouse_moment.z = -1;

}


// keyOrdinary Function : 기본 아스키 키입력을 받는다.
void keyOrdinary(unsigned char key, int x, int y){

	if (isGameOver){
		score = 0;
		life = INITIAL_LIFE;
		initGames(1);
		return;
	}

	isKeyDown = true;

	switch (key){
	case 32: // spacebar
		isGamePlaying = true;
		break;
	case 27: // ESC
		// 게임 종료
		exitGame();
		break;
	case '1':
		// Fire
		applyItem(ITEM_BALL_FIRE);
		break;
	case '2':
		// Fast
		applyItem(ITEM_BALL_FAST);
		break;
	case '3':
		// Slow
		applyItem(ITEM_BALL_SLOW);
		break;
	case '4':
		// Multiple
		applyItem(ITEM_BALL_MULTIPLE);
		break;
	case '5':
		// Fog
		applyItem(ITEM_ENV_FOG);
		break;
	case '6':
		// Boost
		applyItem(ITEM_ENV_BOOST);
		break;
	case '7':
		//Erase
		applyItem(ITEM_ERASE);
		break;
	}
	glutPostRedisplay();
}

void keyUpOrdinary(unsigned char key, int x, int y){
	isKeyDown = false;
	mouse_moment.z = -1;
}

// mouseClick Function : 마우스 입력.
void mouseClick(int button, int state,int x, int y){

	if (isGameOver){
		score = 0;
		life = INITIAL_LIFE;
		initGames(1);
		return;
	}

	switch (state){
	case GLUT_LEFT_BUTTON:
		isGamePlaying = true;
		break;
	}
}

// mouseMotion Function : 마우스의 클릭과 상관없이 좌표를 얻음
void mouseMotion(int x, int y){
	float gx = x - 60;
	static float pgx = gx;
	// 변환
	if (stick == NULL)
		return;

	{
		// 초기화
		if (mouse_moment.z == -1){
			stick->center.x = gx;
			mouse_moment.z = 0;
		}
		// 마우스가 CANVAS 밖에 있는경우(or 밖에서 시작하는 경우) 보정해준다.
		if (gx < stick->width / 2){
			pgx = gx = stick->width / 2;
			stick->center.x = gx;
		}
		if (gx > CANVAS_WIDTH - stick->width / 2){
			pgx = gx = CANVAS_WIDTH - stick->width / 2;
			stick->center.x = gx;
		}

		// 스틱의 모멘트를 누적한다.
		mouse_moment.x += gx - pgx;
	}
	pgx = gx;
}


void* glutFonts[7] = {
	GLUT_BITMAP_9_BY_15,
	GLUT_BITMAP_8_BY_13,
	GLUT_BITMAP_TIMES_ROMAN_10,
	GLUT_BITMAP_TIMES_ROMAN_24,
	GLUT_BITMAP_HELVETICA_10,
	GLUT_BITMAP_HELVETICA_12,
	GLUT_BITMAP_HELVETICA_18
};

void glutPrint(float x, float y, float z, void* font, char* text, float r, float g, float b, float a)
{
	if (!text || !strlen(text)) return;
	bool blending = false;
	glDisable(GL_LIGHTING);

	if (glIsEnabled(GL_BLEND)) blending = true;
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(r, g, b, a);
	glRasterPos3f(x, y, z);

	
	while (*text) {
		glutBitmapCharacter(font, *text);
		text++;
	}

	glEnable(GL_LIGHTING);
	if (!blending) glDisable(GL_BLEND);
}

// renderScene Function : 화면에 도형들을 출력한다.
void renderScene()
{
	static int fps_time = clock();
	static int fps_temp = 0;
	fps_temp++;
	if (clock() - fps_time >= 1000){
		observed_fps = fps_temp;
		fps_temp = 0;
		fps_time = clock();
	}


	// Set ViewPort1
	double rate = WINDOW_HEIGHT / (CANVAS_HEIGHT + BEZEL * 2);
	glViewport((-(CANVAS_HEIGHT - CANVAS_WIDTH) / 2)*rate, -CANVAS_HEIGHT*sin(cam_theta) / 5, WINDOW_HEIGHT, WINDOW_HEIGHT);

	// Clear Color and Depth Buffers
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float z = ((CANVAS_HEIGHT / 2 + BEZEL) * (2.41421356) + 50);
	gluLookAt(CANVAS_WIDTH / 2, z*sin(cam_theta) - CANVAS_HEIGHT / 3, z*cos(cam_theta), // tan(67.5 degree)
	CANVAS_WIDTH / 2, -CANVAS_HEIGHT / 2, 0.0f,
	0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 0.1f, 2000.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE); // glScalef에서 normal vector도 scale되는 것을 방지

	float lightPos[4] = { .5, .7, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//draw bricks with boundaries, stick
	for (std::vector<Brick*>::iterator iter = bricks.begin(); iter != bricks.end();iter++)
		(*iter)->draw();

	//life를 위한 stick 재출력
	if(stick!=NULL)	stick->draw(life);
	if (!isGameOver && !isGamePlaying)
	glutPrint(CANVAS_WIDTH/2 - 150, -CANVAS_HEIGHT/2 - 150, 0, glutFonts[3], "PRESS SPACE or CLICK TO START", 0, 0, 0, exp(-5*cam_theta));
		
	//draw balls
	for (std::vector<Ball*>::iterator iter = balls.begin(); iter != balls.end(); iter++)
		(*iter)->draw();

	//draw items
	for (std::vector<Item*>::iterator iter = items.begin(); iter != items.end(); iter++){
		char *name = (*iter)->getItemName();
		glutPrint((*iter)->center.x - 10, (*iter)->center.y + 10, (*iter)->center.z + (*iter)->depth + 2, glutFonts[3], name, 0, 0, 0, 1);
		if (name != NULL)	delete name;
		(*iter)->draw();
	}

	if (isGameOver){
		glRotatef(90.0f, 1.f, 0.0f, 0.0f);
		glutPrint(200, -150, 0, glutFonts[3], "Game Over!", 1, 0, 0, 1);
		char clevel[20];
		sprintf_s(clevel, "Level : %d\n", level);
		glutPrint(300, -100, 0, glutFonts[3], clevel, 0, 1, 0, 1);
		char cscore[20];
		sprintf_s(cscore, "Score : %d\n", score);
		glutPrint(200, -100, 0, glutFonts[3], cscore, 0, 1, 0, 1);
		glutPrint(200, -50, 0, glutFonts[3], "Press Any Key or Click to Restart!", 0, 0, 1, 1);
		glRotatef(-90.0f, 1.f, 0.0f, 0.0f);
	}


	// Set ViewPort2
	glViewport((CANVAS_WIDTH + BEZEL * 2)*rate, 0, WINDOW_WIDTH - (CANVAS_WIDTH + BEZEL * 2)*rate, WINDOW_HEIGHT);
	
	char cfps[10];
	_itoa_s(observed_fps, cfps, 10);
	glutPrint(300, -WINDOW_HEIGHT / 2 + 300, 0, glutFonts[3], "FPS", 0, 0, 0, cos(cam_theta));
	glutPrint(300, -WINDOW_HEIGHT / 2 + 250, 0, glutFonts[3], cfps, 0, 1, 0, cos(cam_theta));

	char clife[10];
	_itoa_s(life, clife, 10);
	glutPrint(50, -WINDOW_HEIGHT / 2 + 300, 0, glutFonts[3], "Life", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT / 2 + 250, 0, glutFonts[3], clife, 0, 1, 0, cos(cam_theta));

	char clevel[10];
	_itoa_s(level, clevel, 10);
	glutPrint(50, -WINDOW_HEIGHT / 2 + 200, 0, glutFonts[3], "Level", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT / 2 + 150, 0, glutFonts[3], clevel, 0, 0, 1, cos(cam_theta));

	char cscore[10];
	_itoa_s(score, cscore, 10);
	glutPrint(50, -WINDOW_HEIGHT / 2 + 100, 0, glutFonts[3], "Score", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT / 2 + 50, 0, glutFonts[3], cscore, 1, 0, 0, cos(cam_theta));

	int const ITEM_ERASE = 1;	// 모든 효과를 무시함

	int const ITEM_BALL_FIRE = 1 << 1;	// 공이 모두 뚫음v
	int const ITEM_BALL_FAST = 1 << 2;	// 공속도 rate배v
	int const ITEM_BALL_SLOW = 1 << 3;	// 공속도 1/rate배v
	int const ITEM_BALL_MULTIPLE = 1 << 4;	// 공이 3개가 됨v

	glutPrint(50, -WINDOW_HEIGHT + 300, 0, glutFonts[2], "-Debug System-", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT + 280, 0, glutFonts[2], "Key1 : Fire", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT + 260, 0, glutFonts[2], "Key2 : Fast", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT + 240, 0, glutFonts[2], "Key3 : Slow", 0, 0, 0, cos(cam_theta));
	glutPrint(50, -WINDOW_HEIGHT + 220, 0, glutFonts[2], "Key4 : Multiple", 0, 0, 0, cos(cam_theta));
	glutPrint(250, -WINDOW_HEIGHT + 280, 0, glutFonts[2], "Key5 : Fog", 0, 0, 0, cos(cam_theta));
	glutPrint(250, -WINDOW_HEIGHT + 260, 0, glutFonts[2], "Key6 : Boost", 0, 0, 0, cos(cam_theta));
	glutPrint(250, -WINDOW_HEIGHT + 240, 0, glutFonts[2], "Key7 : Erase", 0, 0, 0, cos(cam_theta));
	glutPrint(250, -WINDOW_HEIGHT + 220, 0, glutFonts[2], "Esc : Exit", 0, 0, 0, cos(cam_theta));
	glutPrint(0, -WINDOW_HEIGHT + 120, 0, glutFonts[2], collideMode == 1 ? " Brick Collide Mode" : "Sphere Collide Mode", 1, 0, 0, cos(cam_theta));
	glutPrint(0, -WINDOW_HEIGHT + 100, 0, glutFonts[2], "KC1, KHS, PHK", 0, 0, 0, cos(cam_theta));

	glutSwapBuffers();

}

void initBalls(){
	// Ball들을 초기화한다.
	Ball* new_ball;
	new_ball = new Ball(BALL_RADIUS);
	new_ball->setPos(Vec3(CANVAS_WIDTH / 2, -CANVAS_HEIGHT + 35, 0));
	new_ball->setVel(Vec3(0, BALL_SPEED, 0));
	new_ball->setCollideMode(collideMode);
	balls.push_back(new_ball);
}

void initStick(){
	// stick을 초기화한다. 관리는 Brick과 같이 관리한다.(bricks에 넣음)
	Stick* new_stick = new Stick(80, 10, DEPTH);
	new_stick->setPos(Vec3(CANVAS_WIDTH / 2, -CANVAS_HEIGHT + 20, 0));

	stick = new_stick;
	bricks.push_back(new_stick);
}


// initBricks Function : path 경로에 있는 파일을 읽어 블록들을 생성하고, 그 외 도형들을 초기화한다.
void initBricks(char* path){
	ifstream in(path);
	float row, colm, strength;

	if (in.fail()){
		// 더이상 play할 map이 없음
		exitGame();
		return;
	}
	// Brick들을 읽어와 초기화한다.
	while (in >> row >> colm >> strength , row != -1 && !in.eof()){
		float width = CANVAS_WIDTH / COLUMNS;
		float height = (CANVAS_HEIGHT / 2) / ROWS;

		Brick *new_brick = new Brick(width - 1, height - 1, DEPTH); // 1칸씩 줄여 공간을 만든다.

		new_brick->setPos(Vec3(width * (colm + 0.5f) + 1, -height*(row + 0.5f) + 1, 0));
		new_brick->setStrength(strength);
		new_brick->setColor(Vec3(row / ROWS, 1, colm / COLUMNS));

		if (rand()%7 <= 0){
			Item *new_item = new Item(20,20,20,0);
			new_item->setTypeRandom();
			new_item->setPos(new_brick->center);

			new_brick->setItem(new_item);
		}

		bricks.push_back(new_brick);

	}

	// 상하좌우 Boundary를 초기화한다. 관리는 Brick과 같이 관리한다.(bricks에 넣음)
	Boundary* new_boundary;
	//상
	new_boundary = new Boundary(CANVAS_WIDTH + 2 * BEZEL, BEZEL, DEPTH);
	new_boundary->setPos(Vec3(CANVAS_WIDTH / 2, BEZEL / 2, 0));
	bricks.push_back(new_boundary);
	//하
	new_boundary = new Boundary(CANVAS_WIDTH + 2 * BEZEL, BEZEL, DEPTH);
	new_boundary->setPos(Vec3(CANVAS_WIDTH / 2, -CANVAS_HEIGHT - BEZEL / 2, 0));
	bricks.push_back(new_boundary);
	deadBoundary = new_boundary;
	//좌
	new_boundary = new Boundary(BEZEL, CANVAS_HEIGHT + 2 * BEZEL, DEPTH);
	new_boundary->setPos(Vec3(-BEZEL / 2, -CANVAS_HEIGHT / 2, 0));
	bricks.push_back(new_boundary);
	//우
	new_boundary = new Boundary(BEZEL, CANVAS_HEIGHT + 2 * BEZEL, DEPTH);
	new_boundary->setPos(Vec3(CANVAS_WIDTH + BEZEL / 2, -CANVAS_HEIGHT / 2, 0));
	bricks.push_back(new_boundary);

	in.close();
}

void initGames(int _level){
	level = _level;

	// 모든 오브젝트, 효과 제거
	{
		destroyAll();
		removeItemEffectAll();
		mouse_moment.z = -1;
	}

	// 게임 중지 및 준비
	isGamePlaying = false;
	isGameOver = false;

	char map_s[10];
	sprintf_s(map_s, "map%d.txt", level);

	initBricks(map_s); // level에 맞는 맵
	initBalls();
	initStick();
}


void main(int argc, char **argv) 
{
	srand(time(NULL));
	// 초기화
	score = 0;
	cam_theta = PI/2;
	isGamePlaying = false;
	life = INITIAL_LIFE;
	while (printf("충돌 모드를 선택해주세요\n  1:공을 brick으로 충돌 처리(default)\n  2:공을 sphere로 충돌 처리(추가구현)\n 선택 : "), scanf_s("%d", &collideMode), collideMode!=1 && collideMode!=2);

	initGames(1);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300,0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Practice 10");
	glutIdleFunc(Idle);
	glutSpecialFunc(keySpecial);
	glutSpecialUpFunc(keyUpSpecial);
	glutKeyboardFunc(keyOrdinary);
	glutKeyboardUpFunc(keyUpOrdinary);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMotion);
	glutDisplayFunc(renderScene);
	glutMainLoop();
}
