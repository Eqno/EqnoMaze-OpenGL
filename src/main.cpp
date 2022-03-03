#include <gl/freeglut_std.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <wingdi.h>
#include <winnt.h>
#include <algorithm>
#include <cmath>
#include <ctime>

#include "../include/ball.h"
#include "../include/line.h"
#include "../include/utils.h"
#include "../include/circle.h"
#include "../include/config.h"
#include "../include/transform.h"
#include "../include/texloader.h"
using namespace std;
/*
wasd 移动，按住 shift 跑步，qe 旋转，m 打开地图，z 编辑地图，x 增加墙，
i 清空墙壁，p 生成地图，f 放置墙，c 删除墙，v 切换视角， r 显示朝向, ↑ 推进镜头，↓ 拉远镜头。
n 检测 shift 用。
*/

int groundTexId = 0, ball = 0, wallTexId = 0, 
    skyTopTexId = 0, skyBottomTexId = 0,
    skyFrontTexId = 0, skyBackTexId = 0,
    skyLeftTexId = 0, skyRightTexId = 0;
int W = 1600, H = 900, F = 100;
unsigned int SCENESPEED = 5, SCENEID = 0, BALLACC = 20,
    ACTROTATESPEED = 5, ACTROTATEID = 1,
    ACTORJUMPSPEED = 2, ACTORJUMPID = 2;
double GX = 9000, GY = -200, GZ = 16000, BALLSIZE = 0.3;
double bodyWidth = 3, bodyHeight = 100, armStepWidth = 20,
    legStepWidth = 15, armLongerThanLeg = 10;

double wallTexFactor = 5, groundTexFactor = 50,
    obvRotateX = 0, obvRotateY = 0, genWallFac = 100,
    actStepWalk = 5, actStepRun = 10, actStep = actStepWalk,  // walk speed
    turnLeftStep = 2, turnRightStep = 2, bodyRotateStep = 4,
    obvMoveFactorX = 1.2, obvMoveFactorY = 0.5, obvMoveRadius = 3,
    obvLookRadius = 3, zoomStep = 0.05, obvRadiusMax = 8, obvRadiusMin = 1.3,
    mapViewFac = 0.7, mapViewHeight = 5000, wallHeight = 500,
    skyBottom = -500, skyDistance = 500, siteDistance = 500,
    actMoveX = 0, actMoveZ = 0;
    
bool openMap = false, firstPerson = false, windowFoucus = false,
    jump = false, ifJump = false, followJump = true,
    mouseLeftDown = false, mouseRightDown = false, mouseControlMove = false,
    turnLeft = false, turnRight = false, zoomIn = false, zoomOut = false,
    moveLeft = false, moveRight = false, moveForward = false, moveBack = false,
    editMap = false, drawWall = false, delWall = false, showDre = false;

double armSwingX = 0, 
    armSwingEWalk = 20, armSwingStepWalk = 0.3,
    armSwingERun = 30, armSwingStepRun = 1,
    armSwingEE = armSwingEWalk,
    armSwingE = armSwingEE,
    armSwingStep = armSwingStepWalk,
    actRotateX = obvRotateX,
    bodyRotateX = -actRotateX,
    bodyRotateE = bodyRotateX,
    lastRotateX = obvRotateX,
    lastRotateY = obvRotateY,
    actMoveStartX = 0, actMoveStartZ = 0,
    actMoveEndX = 0, actMoveEndZ = 0;

double actDepth = 0, actJumpVE = -5, actJumpV = 0, actJumpA = 0.1;

vector <Point> actor, body, leftLeg, rightLeg, leftArm, rightArm,
    ground, tmpWall, pointer, sky;
vector <vector <Point>> wall, wallEx;

double actLastAngX = 0;
void adjustBodyRotateE()
{
    if (bodyRotateE > 180) bodyRotateE -= 360;
    if (bodyRotateE < -180) bodyRotateE += 360;
}
void updateObverse()
{   
    if (openMap)
    {
        gluLookAt((ground[0].x+GX)/F, (ground[0].y+mapViewHeight)/F, (ground[0].z+GZ)/F,
            (ground[0].x+GX)/F, ground[0].y/F, (ground[0].z+GZ)/F, 1, 0, 0);
    }
    else
    {
        if (firstPerson) glTranslated(0, 0, 0);
        else glTranslated(0, 0, -obvLookRadius);
        glTranslated(0, -actDepth/F*cos(obvRotateY/180*pi), 0);
        glTranslated(0, 0, -actDepth/F*sin(obvRotateY/180*pi));
        if (obvRotateX > 180) obvRotateX = -180;
        if (obvRotateX < -180) obvRotateX = 180;
        if (obvRotateY > 180) obvRotateY = -180;
        if (obvRotateY < -180) obvRotateY = 180;
        if (actRotateX > 180) actRotateX = -180;
        if (actRotateX < -180) actRotateX = 180;
        glRotated(obvRotateY, 1, 0, 0);
        glRotated(obvRotateX, 0, 1, 0);
        if (mouseRightDown)
        {
            bodyRotateE = -obvRotateX;
            adjustBodyRotateE();
            rotateDY(actor, -obvRotateX+actLastAngX);
            rotateDY(pointer, -obvRotateX+actLastAngX);
            actLastAngX = obvRotateX;
            actRotateX = obvRotateX;
        }
    }
}
void getGround()
{
    ground.push_back((Point) {-GX, GY, -GZ});
    ground.push_back((Point) {-GX, GY, GZ});
    ground.push_back((Point) {GX, GY, GZ});
    ground.push_back((Point) {GX, GY, -GZ});
}
void addGround()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    if (drawWall) glColor3dv(DARKBLUE);
    else if (delWall) glColor3dv(DARKRED);
    else if (editMap) glColor3dv(DARKGREEN);
    else glBindTexture(GL_TEXTURE_2D, groundTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3d(ground[0].x/F, ground[0].y/F, ground[0].z/F);
    glTexCoord2d(0, groundTexFactor); glVertex3d(ground[1].x/F, ground[1].y/F, ground[1].z/F);
    glTexCoord2d(groundTexFactor, groundTexFactor); glVertex3d(ground[2].x/F, ground[2].y/F, ground[2].z/F);
    glTexCoord2d(groundTexFactor, 0); glVertex3d(ground[3].x/F, ground[3].y/F, ground[3].z/F);
    glEnd();
}
void addSky()
{
    glBindTexture(GL_TEXTURE_2D, skyTopTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3d(sky[0].x/F, sky[0].y/F, sky[0].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[1].x/F, sky[1].y/F, sky[1].z/F);
    glTexCoord2d(1, 1); glVertex3d(sky[2].x/F, sky[2].y/F, sky[2].z/F);
    glTexCoord2d(1, 0); glVertex3d(sky[3].x/F, sky[3].y/F, sky[3].z/F);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skyLeftTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(1, 0); glVertex3d(sky[4].x/F, sky[4].y/F, sky[4].z/F);
    glTexCoord2d(0, 0); glVertex3d(sky[5].x/F, sky[5].y/F, sky[5].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[6].x/F, sky[6].y/F, sky[6].z/F);
    glTexCoord2d(1, 1); glVertex3d(sky[7].x/F, sky[7].y/F, sky[7].z/F);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skyRightTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(1, 0); glVertex3d(sky[8].x/F, sky[8].y/F, sky[8].z/F);
    glTexCoord2d(0, 0); glVertex3d(sky[9].x/F, sky[9].y/F, sky[9].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[10].x/F, sky[10].y/F, sky[10].z/F);
    glTexCoord2d(1, 1); glVertex3d(sky[11].x/F, sky[11].y/F, sky[11].z/F);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skyBackTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3d(sky[12].x/F, sky[12].y/F, sky[12].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[13].x/F, sky[13].y/F, sky[13].z/F);
    glTexCoord2d(1, 1); glVertex3d(sky[14].x/F, sky[14].y/F, sky[14].z/F);
    glTexCoord2d(1, 0); glVertex3d(sky[15].x/F, sky[15].y/F, sky[15].z/F);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skyFrontTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3d(sky[16].x/F, sky[16].y/F, sky[16].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[17].x/F, sky[17].y/F, sky[17].z/F);
    glTexCoord2d(1, 1); glVertex3d(sky[18].x/F, sky[18].y/F, sky[18].z/F);
    glTexCoord2d(1, 0); glVertex3d(sky[19].x/F, sky[19].y/F, sky[19].z/F);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, skyBottomTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(1, 1); glVertex3d(sky[20].x/F, sky[20].y/F, sky[20].z/F);
    glTexCoord2d(1, 0); glVertex3d(sky[21].x/F, sky[21].y/F, sky[21].z/F);
    glTexCoord2d(0, 0); glVertex3d(sky[22].x/F, sky[22].y/F, sky[22].z/F);
    glTexCoord2d(0, 1); glVertex3d(sky[23].x/F, sky[23].y/F, sky[23].z/F);
    glEnd();
}
void drawLine(double x0, double y0, double z0,
    double x1, double y1, double z1, const double color[3])
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3dv(color);
    glBegin(GL_LINE_LOOP);
    glVertex3d(x0/F, y0/F, z0/F);
    glVertex3d(x1/F, y1/F, z1/F);
    glEnd();
}
void addWall()
{
    if (openMap && tmpWall.size())
    {
        drawLine(tmpWall[1].x, tmpWall[1].y, tmpWall[1].z,
            tmpWall[2].x, tmpWall[2].y, tmpWall[2].z, WHITE);
    }
    for (auto i: wall)
    {
        if (openMap) drawLine(i[1].x, i[1].y, i[1].z, i[2].x, i[2].y, i[2].z, BLACK);
        glBindTexture(GL_TEXTURE_2D, wallTexId);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex3d(i[0].x/F, i[0].y/F, i[0].z/F);
        glTexCoord2d(0, F*wallTexFactor/wallHeight); glVertex3d(i[1].x/F, i[1].y/F, i[1].z/F);
        double wallLen = sqrt((i[2].x-i[1].x)*(i[2].x-i[1].x)+(i[2].z-i[1].z)*(i[2].z-i[1].z));
        glTexCoord2d(wallLen/wallTexFactor/F, F*wallTexFactor/wallHeight); glVertex3d(i[2].x/F, i[2].y/F, i[2].z/F);
        glTexCoord2d(wallLen/wallTexFactor/F, 0); glVertex3d(i[3].x/F, i[3].y/F, i[3].z/F);
        glEnd();
    }
}
void addMatch(const vector <Point> &match)
{
    glBegin(GL_LINE_LOOP);
    for (auto i: match) glVertex3d(i.x/F, i.y/F, i.z/F);
    glEnd();
}
void addActor()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3dv(BLACK);
    if ((!firstPerson) || openMap)
    {
        // for (int i=3; i<actor.size(); i+=4)
        // {
            // glBindTexture(GL_TEXTURE_2D, ball);
            // glBegin(GL_LINE_LOOP);
            // for (int j=0; j<3; j++)
                // glVertex3d(actor[i-j].x, actor[i-j].y, actor[i-j].z);
            // glEnd();
        // }
        glBegin(GL_QUADS);
        for (auto i: actor) glVertex3d(i.x, i.y, i.z);
        glEnd();
    }
    addMatch(body), addMatch(leftLeg), addMatch(rightLeg), addMatch(leftArm), addMatch(rightArm);
    if (openMap || showDre)
    {
        glColor3dv(WHITE);
        glBegin(GL_QUADS);
        glVertex3d(pointer[3].x, pointer[3].y, pointer[3].z);
        glVertex3d(pointer[1].x, pointer[1].y, pointer[1].z);
        glVertex3d(pointer[0].x, pointer[0].y, pointer[0].z);
        glVertex3d(pointer[2].x, pointer[2].y, pointer[2].z);
        glEnd();
    }
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    glRotated(obvRotateY, 1, 0, 0);
    glRotated(obvRotateX, 0, 1, 0);
    addSky();

    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    glLineWidth(3);
    updateObverse();
    addGround();
    addWall();
	addActor();

    // drawLine(-300, 0, 0, 300, 0, 0, WHITE);
    // drawLine(0, -300, 0, 0, 300, 0, WHITE);
    // drawLine(0, 0, -300, 0, 0, 300, WHITE);
    glutSwapBuffers();
}
void reshape (int w, int h)
{
    glViewport(0, 0, w, h); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (openMap) glOrtho(-GZ*mapViewFac*w/h/F, GZ*mapViewFac*w/h/F,
        -GZ*mapViewFac/F, GZ*mapViewFac/F, 0.1, siteDistance);
    else gluPerspective(60, (double)w/h, 0.1, siteDistance);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    W = w, H = h;
}
void move(double delX, double delZ)
{
    for (size_t i=0; i<ground.size(); i++)
    {
        ground[i].x += delX;
        ground[i].z += delZ;
    }
    actMoveEndX = ground[0].x + GX - actMoveStartX;
    actMoveEndZ = ground[0].z + GZ - actMoveStartZ;
    if (drawWall && tmpWall.size())
    {
        tmpWall[0] = (Point) {actMoveEndX, GY, actMoveEndZ};
        tmpWall[1] = (Point) {actMoveEndX, GY+wallHeight, actMoveEndZ};
    }
    for (size_t i=0; i<wall.size(); i++)
        for (size_t j=0; j<wall[i].size(); j++)
        {
            wall[i][j].x += delX;
            wall[i][j].z += delZ;
        }
    for (size_t i=0; i<wallEx.size(); i++)
        for (size_t j=0; j<wallEx[i].size(); j++)
        {
            wallEx[i][j].x += delX;
            wallEx[i][j].z += delZ;
        }
    actMoveX += delX, actMoveZ += delZ;
}
void mouseClick(int button, int state, int x, int y)
{
    if (! openMap)
    {
        if (state == GLUT_DOWN)
        {
            lastRotateX = x, lastRotateY = y;
            if (button == GLUT_LEFT_BUTTON) mouseLeftDown = true;
            if (button == GLUT_RIGHT_BUTTON) mouseRightDown = true;
        }
        if (state == GLUT_UP)
        {
            if (button == GLUT_LEFT_BUTTON) mouseLeftDown = false;
            if (button == GLUT_RIGHT_BUTTON) mouseRightDown = false;
        }
        if (mouseLeftDown && mouseRightDown) mouseControlMove = true;
        else mouseControlMove = false;
    }    
}
void obverseChange(double &obvRotate, double coord, double &lastRotate, double obvMoveFac)
{
    obvRotate += obvMoveFac * (coord-lastRotate) / obvMoveRadius;
    lastRotate = coord;
}
void mouseClickMove(int x, int y)
{
    if (! openMap)
    {
        obverseChange(obvRotateX, x, lastRotateX, obvMoveFactorX);
        obverseChange(obvRotateY, y, lastRotateY, obvMoveFactorY);
    }
}
struct Edge { int u, v, z; };
vector <Edge> edge, ans;
const int MAXN = 1e5+10;
int f[MAXN];
int getf(int x) { return x==f[x] ? x : (f[x]=getf(f[x])); }
bool merge(int x, int y)
{
    x=getf(x), y=getf(y);
    if (x != y)
    {
        f[x] = y;
        return true;
    }
    else return false;
}
bool genWallCmp(const Edge &i, const Edge &j) { return i.z < j.z; }
void genWall()
{
    openMap = editMap = true;
    zoomIn = zoomOut = false;
    reshape(W, H);
    glutReshapeWindow(W, H);

    srand(time(NULL));
    int numX = GX/genWallFac, numZ = GZ/genWallFac;
    for (int i=0; i<numX; i++)
        for (int j=0; j<numZ; j++)
            {
                f[i*numZ+j] = i*numZ+j;
                if (i) edge.push_back({i*numZ+j, (i-1)*numZ+j, abs(rand())%65535});
                if (j) edge.push_back({i*numZ+j, i*numZ+(j-1), abs(rand())%65535});
            }
    sort(edge.begin(), edge.end(), genWallCmp);
    for (auto i: edge)
        if (merge(i.u, i.v)) ans.push_back(i);
    for (auto p: ans)
    {
        int i1 = p.u/numZ, j1 = p.u%numZ;
        int i2 = p.v/numZ, j2 = p.v%numZ;
        Point m = {actMoveX-GX+i1*genWallFac*2+genWallFac, GY+wallHeight, actMoveZ-GZ+j1*genWallFac*2+genWallFac},
            n = {actMoveX-GX+i2*genWallFac*2+genWallFac, GY+wallHeight, actMoveZ-GZ+j2*genWallFac*2+genWallFac};
        wall.push_back({
            {actMoveX-GX+i1*genWallFac*2+genWallFac, GY, actMoveZ-GZ+j1*genWallFac*2+genWallFac}, m, n,
            {actMoveX-GX+i2*genWallFac*2+genWallFac, GY, actMoveZ-GZ+j2*genWallFac*2+genWallFac},
        });
        vector <Point> v;
        getLine(m, n, v);
        wallEx.push_back(v);
    }
    
}
void keyboardListener(unsigned char cmd, int x, int y)
{
    switch (cmd)
    {
        case 'w': moveForward = true; break;
        case 's': moveBack = true; break;
        case 'a': moveLeft = true; break;
        case 'd': moveRight = true; break;
        case 'q': turnLeft = true; break;
        case 'e': turnRight = true; break;
        case 'r': showDre = !showDre; break;
        case 'p': if (wall.size() <= 4) genWall(); break;
        case 'i':
            while (wall.size() > 4) wall.pop_back();
            while (wallEx.size() > 4) wallEx.pop_back();
            break;
        case ' ':
            if (! jump)
            {
                ifJump = true;
                jump = true;
                actJumpV = actJumpVE;
            }
            break;
        case 'n':
            actStep = actStepWalk;
            armSwingEE = armSwingEWalk;
            armSwingStep = armSwingStepWalk;
            break;
        case 'N':
            actStep = actStepRun;
            armSwingEE = armSwingERun;
            armSwingStep = armSwingStepRun;
        break;
        default: break;
    }
    if (openMap)
    {
        if (editMap)
        {
            if (delWall)
            {
                if (cmd == 'c')
                {
                    delWall = false;
                }
                else
                {

                }
            }
            else if (drawWall)
            {
                if (cmd=='x' || cmd=='f')
                {
                    const Point &p = tmpWall[1];
                    const Point &q = tmpWall[2];
                    vector <Point> v;
                    getLine(p, q, v);
                    wallEx.push_back(v);
                    wall.push_back(tmpWall);
                    if (cmd == 'x') drawWall = false, tmpWall.clear();
                    else
                    {
                        tmpWall = vector <Point> ({
                            (Point) {0, GY, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY, 0},
                        });
                        actMoveStartX = ground[0].x+GX;
                        actMoveStartZ = ground[0].z+GZ;
                    }
                }
                else
                {

                }
            }
            else
            {
                switch (cmd)
                {
                    case 'x':
                        drawWall = true;
                        tmpWall = vector <Point> ({
                            (Point) {0, GY, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY, 0},
                        });
                        actMoveStartX = ground[0].x+GX;
                        actMoveStartZ = ground[0].z+GZ;
                    break;
                    case 'c': delWall = true; break;
                    case 'z': editMap = false; break;
                    default: break;
                }
            }
        }
        else
        {
            switch (cmd)
            {
                case 'z': editMap = true; break;
                case 'm':
                    openMap = false;
                    zoomIn = zoomOut = false;
                    reshape(W, H);
                    glutReshapeWindow(W, H);
                break;
                default: break;
            }
        }
    }
    else
    {
        switch (cmd)
        {
            case 'v': firstPerson = !firstPerson; zoomIn = zoomOut = false; break;
            case 'm':
                openMap = true;
                zoomIn = zoomOut = false;
                reshape(W, H);
                glutReshapeWindow(W, H);
            break;
            default: break;
        }
    }
}
void keyboardUpListener(unsigned char cmd, int x, int y)
{
    switch (cmd)
    {
        case 'w': moveForward = false; break;
        case 's': moveBack = false; break;
        case 'a': moveLeft = false; break;
        case 'd': moveRight = false; break;
        case 'q': turnLeft = false; break;
        case 'e': turnRight = false; break;
        default: break;
    }
}
void specialListener(int cmd, int x, int y)
{
    if (firstPerson || openMap)
    {
        switch (cmd)
        {
            case GLUT_KEY_LEFT: turnLeft = true; break;
            case GLUT_KEY_RIGHT: turnRight = true; break;
            default: break;
        }
    }
    else
    {
        switch (cmd)
        {
            case GLUT_KEY_UP: zoomIn = true; break;
            case GLUT_KEY_DOWN: zoomOut = true; break;
            case GLUT_KEY_LEFT: turnLeft = true; break;
            case GLUT_KEY_RIGHT: turnRight = true; break;
            default: break;
        }
    }
}
void specialUpListener(int cmd, int x, int y)
{
    if (firstPerson || openMap)
    {
        switch (cmd)
        {
            case GLUT_KEY_LEFT: turnLeft = false; break;
            case GLUT_KEY_RIGHT: turnRight = false; break;
            default: break;
        }
    }
    else
    {
        switch (cmd)
        {
            case GLUT_KEY_UP: zoomIn = false; break;
            case GLUT_KEY_DOWN: zoomOut = false; break;
            case GLUT_KEY_LEFT: turnLeft = false; break;
            case GLUT_KEY_RIGHT: turnRight = false; break;
            default: break;
        }
    }
}
void windowFoucusListener(int state)
{
    if (state == GLUT_LEFT) windowFoucus = false;
    if (state == GLUT_ENTERED) windowFoucus = true;
}
bool _checkMovable(double delX, double delZ, size_t wall)
{
    static const double EPS = 5;
    for (size_t i=0; i<wallEx.size(); i++)
        for (size_t j=0; j<wallEx[i].size(); j++)
            if (i!=wall && sqrt((wallEx[i][j].x+delX)*(wallEx[i][j].x+delX)
                +(wallEx[i][j].z+delX)*(wallEx[i][j].z+delZ)) <= F*BALLSIZE+EPS)
                return false;
    return true;
}
void moveAlongWall(double delX, double delZ, double u, double v, int wall)
{
    double r = sqrt(u*u+v*v);
    double s = u/r;
    double c = v/r;
    double l = 0;
    const double EPS = 1e-5;
    if (abs(delX)>=EPS && abs(u)>=EPS)
        l = cos(atan(delZ/delX)-atan(v/u))*sqrt(delX*delX+delZ*delZ);
    else if (abs(delX) < EPS)
        l = cos(pi/2-atan(v/u))*delZ;
    else if (abs(u) < EPS)
        l = cos(pi/2-atan(-delZ/delX))*sqrt(delX*delX+delZ*delZ);
    l = abs(l);
    if (delX*u+delZ*v > 0)
    {
        double dx = s*l, dz = c*l;
        if (_checkMovable(dx, dz, wall))
            move(dx, dz);
    }
    else
    {
        double dx = -s*l, dz = -c*l;
        if (_checkMovable(dx, dz, wall))
            move(dx, dz);
    }
}
bool checkMovable(double delX, double delZ)
{
    static const int EPS = 5;
    static const double EPS2 = 5;
    for (size_t i=0; i<wallEx.size(); i++)
        for (size_t j=0; j<wallEx[i].size(); j++)
            if (sqrt((wallEx[i][j].x+delX)*(wallEx[i][j].x+delX)
                +(wallEx[i][j].z+delX)*(wallEx[i][j].z+delZ)) <= F*BALLSIZE+EPS2)
            {
                if (delWall && i>3)
                {
                    wall.erase(wall.begin()+i);
                    wallEx.erase(wallEx.begin()+i);
                    return true;
                }
                else if (editMap && i>3) return true;
                else
                {
                    double a = wall[i][2].x-wall[i][1].x, b = wall[i][2].z-wall[i][1].z;
                    if (j>EPS && j<wallEx[i].size()-1-EPS)
                        moveAlongWall(delX, delZ, a, b, i);
                    return false;
                }
            }
    return true;
}
void sceneMoveLoop(int id)
{
    if(windowFoucus) keybd_event('N', 0, 0, 0);  // 模拟按下 n 键，检测 shift。
    double delX = 0, delZ = 0;
    if (moveLeft)
    {
        // adjustBodyDre(-actRotateX+60);
        bodyRotateE = -actRotateX+60;
        adjustBodyRotateE();
        delX = actStep * cos(actRotateX/180*pi);
        delZ = actStep * sin(actRotateX/180*pi);
    }
    if (moveRight)
    {
        // adjustBodyDre(-actRotateX-60);
        bodyRotateE = -actRotateX-60;
        adjustBodyRotateE();
        delX = -actStep * cos(actRotateX/180*pi);
        delZ = -actStep * sin(actRotateX/180*pi);
    }
    if (moveForward || mouseControlMove) 
    {
        // adjustBodyDre(-actRotateX);
        bodyRotateE = -actRotateX;
        adjustBodyRotateE();
        delX = -actStep * sin(actRotateX/180*pi);
        delZ = actStep * cos(actRotateX/180*pi);
    }
    if (moveBack) 
    {
        // adjustBodyDre(-actRotateX+180);
        bodyRotateE = -actRotateX+180;
        adjustBodyRotateE();
        delX = actStep * sin(actRotateX/180*pi);
        delZ = -actStep * cos(actRotateX/180*pi);
    }
    if (turnLeft)
    {
        actRotateX -= obvMoveFactorX * turnLeftStep / obvMoveRadius;
        obvRotateX = actRotateX;

        rotateDY(actor, -obvRotateX+actLastAngX);
        rotateDY(pointer, -obvRotateX+actLastAngX);
        actLastAngX = obvRotateX;
    }
    if (turnRight)
    {
        actRotateX += obvMoveFactorX * turnLeftStep / obvMoveRadius;
        obvRotateX = actRotateX;

        rotateDY(actor, -obvRotateX+actLastAngX);
        rotateDY(pointer, -obvRotateX+actLastAngX);
        actLastAngX = obvRotateX;
    }
    if (zoomIn && obvLookRadius>obvRadiusMin)
        obvLookRadius -= zoomStep;
    if (zoomOut && obvLookRadius<obvRadiusMax)
        obvLookRadius += zoomStep;
    if (checkMovable(delX, delZ)) move(delX, delZ);
    glutPostRedisplay();
    glutTimerFunc(SCENESPEED, sceneMoveLoop, id);
}
void rotateLeft()
{
    bodyRotateX -= bodyRotateStep;
    if (bodyRotateX < -180) bodyRotateX += 360;
}
void rotateRight()
{
    bodyRotateX += bodyRotateStep;
    if (bodyRotateX > 180) bodyRotateX -= 360;
}
void swingArm()
{
    static const double EPS = 1;
    if (moveForward || mouseControlMove || moveLeft || moveRight || moveBack)
    {
        if (armSwingE > 0)
        {
            if (armSwingX < armSwingE)
            {
                armSwingX += armSwingStep;
                swingIndArm(armSwingStep, F*BALLSIZE-actDepth);
                swingIndLeg(-armSwingStep, bodyHeight-actDepth);
            }
            else armSwingE = -armSwingEE;
        }
        else
        {
            if (armSwingX > armSwingE)
            {
                armSwingX -= armSwingStep;
                swingIndArm(-armSwingStep, F*BALLSIZE-actDepth);
                swingIndLeg(armSwingStep, bodyHeight-actDepth);
            }
            else armSwingE = armSwingEE;
        }
    }
    else
    {
        if (armSwingX < -EPS)
        {
            armSwingX += armSwingStep;
            swingIndArm(armSwingStep, F*BALLSIZE);
            swingIndLeg(-armSwingStep, bodyHeight);
        }
        else if (armSwingX > EPS)
        {
            armSwingX -= armSwingStep;
            swingIndArm(-armSwingStep, F*BALLSIZE);
            swingIndLeg(armSwingStep, bodyHeight);
        }
    }
}
void actRotateLoop(int id)
{
    const static double EPS = 4;
    if (! jump) swingArm();
    double delta = bodyRotateE - bodyRotateX;
    if (abs(delta) > EPS)
    {
        if ((delta<0&&delta>-180) || (delta>0&&delta>180)) rotateLeft();
        if ((delta<0&&delta<-180) || (delta>0&&delta<180)) rotateRight();
    }
    rotateIndLeftLeg(leftLeg, bodyRotateX);
    rotateIndRightLeg(rightLeg, bodyRotateX);
    rotateIndLeftArm(leftArm, bodyRotateX);
    rotateIndRightArm(rightArm, bodyRotateX);
    glutPostRedisplay();
    glutTimerFunc(ACTROTATESPEED, actRotateLoop, id);
}
void actJumpLoop(int id)
{
    // double v2m2ax = actJumpVE*actJumpVE-2*actJumpA*actDepth;
    // if (actJumpV > 0)
    // {
    //     jump = true;
    //     if (v2m2ax > 0)
    //     {
    //         actJumpV = sqrt(v2m2ax);
    //         bodyTranslateY(actJumpV);
    //         translateY(actor, actJumpV/F);
    //         translateY(body, actJumpV);
    //         actDepth += actJumpV;
    //     }
    //     else actJumpV = 0, actDepth -= 0.01;
    // }
    // else if (actDepth > 0) 
    // {
    //     if (v2m2ax > 0)
    //     {
    //         actJumpV = -sqrt(v2m2ax);
    //         bodyTranslateY(actJumpV);
    //         translateY(actor, actJumpV/F);
    //         translateY(body, actJumpV);
    //         actDepth += actJumpV;
    //     }
    // }
    // else jump = false;

    if (ifJump)
    {
        actDepth -= actJumpV;
        ifJump = false;
        goto nextLoop;
    }
    if (actDepth-actJumpV <= 0)
    {
        actDepth = 0;
        actJumpV = 0;
        jump = false;
        goto nextLoop;
    }
    bodyTranslateY(-actJumpV);
    translateY(actor, -actJumpV/F);
    translateY(body, -actJumpV);
    actDepth -= actJumpV;
    actJumpV += actJumpA;
    printf("%.2f\n", actDepth);
nextLoop:
    glutPostRedisplay();
    glutTimerFunc(ACTORJUMPSPEED, actJumpLoop, id);
}
void makeMatch(double topx, double topy, double bottomx, double bottomy,
    double matchWidth, double topd, double bottomd, vector <Point> &res)
{
    vector <Point> top, bottom;
    getCircle(topx, topy, matchWidth, topd, top);
    getCircle(bottomx, bottomy, matchWidth, bottomd, bottom);
    for (size_t i=0; i<top.size(); i++)
    {
        res.push_back({top[i].x, top[i].y, top[i].z});
        res.push_back({bottom[i].x, bottom[i].y, bottom[i].z});
    }
}
void makeBody()
{
    makeMatch(0, 0, 0, 0, bodyWidth, -F*BALLSIZE, -bodyHeight-armLongerThanLeg, body);
    makeMatch(0, 0, -legStepWidth, 0, bodyWidth, -bodyHeight, GY, leftLeg);
    makeMatch(0, 0, legStepWidth, 0, bodyWidth, -bodyHeight, GY, rightLeg);
    makeMatch(0, 0, -armStepWidth, 0, bodyWidth, -F*BALLSIZE, -bodyHeight-armLongerThanLeg, leftArm);
    makeMatch(0, 0, armStepWidth, 0, bodyWidth, -F*BALLSIZE, -bodyHeight-armLongerThanLeg, rightArm);
    initInd(leftLeg, rightLeg, leftArm, rightArm);
}
void makeSky()
{
    sky = {
            {-skyDistance, skyBottom+2*skyDistance, -skyDistance},
            {-skyDistance, skyBottom+2*skyDistance, skyDistance},
            {skyDistance, skyBottom+2*skyDistance, skyDistance},
            {skyDistance, skyBottom+2*skyDistance, -skyDistance},

            {skyDistance, skyBottom, -skyDistance},
            {skyDistance, skyBottom, skyDistance},
            {skyDistance, skyBottom+2*skyDistance, skyDistance},
            {skyDistance, skyBottom+2*skyDistance, -skyDistance},

            {-skyDistance, skyBottom, -skyDistance},
            {-skyDistance, skyBottom, skyDistance},
            {-skyDistance, skyBottom+2*skyDistance, skyDistance},
            {-skyDistance, skyBottom+2*skyDistance, -skyDistance},

            {-skyDistance, skyBottom, skyDistance},
            {-skyDistance, skyBottom+2*skyDistance, skyDistance},
            {skyDistance, skyBottom+2*skyDistance, skyDistance},
            {skyDistance, skyBottom, skyDistance},

            {-skyDistance, skyBottom, -skyDistance},
            {-skyDistance, skyBottom+2*skyDistance, -skyDistance},
            {skyDistance, skyBottom+2*skyDistance, -skyDistance},
            {skyDistance, skyBottom, -skyDistance},

            {-skyDistance, skyBottom, -skyDistance},
            {-skyDistance, skyBottom, skyDistance},
            {skyDistance, skyBottom, skyDistance},
            {skyDistance, skyBottom, -skyDistance}
        };
}
void makeBorder()
{
    wall = {
        {{-GX, GY, -GZ}, {-GX, GY+wallHeight, -GZ}, {GX, GY+wallHeight, -GZ}, {GX, GY, -GZ}},
        {{-GX, GY, GZ}, {-GX, GY+wallHeight, GZ}, {GX, GY+wallHeight, GZ}, {GX, GY, GZ}},
        {{-GX, GY, -GZ}, {-GX, GY+wallHeight, -GZ}, {-GX, GY+wallHeight, GZ}, {-GX, GY, GZ}},
        {{GX, GY, -GZ}, {GX, GY+wallHeight, -GZ}, {GX, GY+wallHeight, GZ}, {GX, GY, GZ}},
    };
    vector <Point> a, b, c, d;
    getLine({-GX, GY+wallHeight, -GZ}, {GX, GY+wallHeight, -GZ}, a);
    getLine({-GX, GY+wallHeight, GZ}, {GX, GY+wallHeight, GZ}, b);
    getLine({-GX, GY+wallHeight, -GZ}, {-GX, GY+wallHeight, GZ}, c);
    getLine({GX, GY+wallHeight, -GZ}, {GX, GY+wallHeight, GZ}, d);
    wallEx = { a, b, c, d};
}
int main(int argc, char** argv)
{
    // window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H); 
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Maze");

	// init
    getGround();
	getBall(0, 0, 0, BALLSIZE, actor, BALLACC);
    makeBody();
    makeSky();
    pointer = {{0, 0, -300./F}, {-50./F, 0, -200./F}, {50./F, 0, -200./F}, {0, 0, 0}};
    makeBorder();
	groundTexId = loadTexture("tex/cement.bmp");
    wallTexId = loadTexture("tex/redbrick.bmp");
    skyTopTexId = loadTexture("tex/skybox/top.bmp");
    skyBottomTexId = loadTexture("tex/skybox/bottom.bmp");
    skyFrontTexId = loadTexture("tex/skybox/front.bmp");
    skyBackTexId = loadTexture("tex/skybox/back.bmp");
    skyLeftTexId = loadTexture("tex/skybox/left.bmp");
    skyRightTexId = loadTexture("tex/skybox/right.bmp");

    // rendering
    glEnable(GL_TEXTURE_2D);
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);

    // listener
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseClickMove);
    // glutPassiveMotionFunc(mouseInMove);
    glutKeyboardFunc(keyboardListener);
    glutKeyboardUpFunc(keyboardUpListener);
    glutSpecialFunc(specialListener);
    glutSpecialUpFunc(specialUpListener);
    glutEntryFunc(windowFoucusListener);

    // loop
    glutTimerFunc(SCENESPEED, sceneMoveLoop, SCENEID);
    glutTimerFunc(ACTROTATESPEED, actRotateLoop, ACTROTATEID);
    glutTimerFunc(ACTORJUMPSPEED, actJumpLoop, ACTORJUMPID);
    glutMainLoop();
    return 0;
}