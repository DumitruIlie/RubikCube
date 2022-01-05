//Ilie Dumitru
#include<graphics.h>
#include<algorithm>
#include<cmath>
#include<ctime>
#include<cstdio>

const int blockSize=100;

const int cameraDistanceSliderLength=200, cameraDistanceSliderWidth=10, cameraDistanceSliderLeft=10, cameraDistanceSliderTop=50;
int cameraDistanceSliderPosition=75;
float cameraDistanceSliderPercent=cameraDistanceSliderPosition/(float)cameraDistanceSliderLength;
bool usingSlider=0;
const float minCameraDistance=4*blockSize, maxCameraDistance=8*blockSize;
float cameraDistance=cameraDistanceSliderPercent*(minCameraDistance-maxCameraDistance)+maxCameraDistance;

const float M_PI=3.1415926535897, M_PI_2=M_PI*0.5f;

inline bool isPressed(int key) {return GetKeyState(key)&0x80000;}
inline int min(int a, int b) {return a+(b-a)*(b<a);}
inline int max(int a, int b) {return a+(b-a)*(b>a);}
inline float abs(float x) {return -x+2*x*(x>0);}
const float D2R=M_PI/180;

template<class T>
class Vec3
{
public:
    T x, y, z;
    Vec3(T _x=0, T _y=0, T _z=0) : x(_x), y(_y), z(_z) {}
    virtual ~Vec3() {}
    friend Vec3 operator+(Vec3 a, Vec3 b) {return Vec3(a.x+b.x, a.y+b.y, a.z+b.z);}
    friend Vec3 operator-(Vec3 a, Vec3 b) {return Vec3(a.x-b.x, a.y-b.y, a.z-b.z);}
    friend Vec3 operator-(Vec3 a) {return Vec3(-a.x, -a.y, -a.z);}
    friend Vec3 operator*(Vec3 a, T s) {return Vec3(a.x*s, a.y*s, a.z*s);}
    friend Vec3 operator*(T s, Vec3 a) {return Vec3(a.x*s, a.y*s, a.z*s);}
    friend Vec3 operator/(Vec3 a, T s) {if(s) return Vec3(a.x/s, a.y/s, a.z/s); return Vec3();}
    friend Vec3& operator+=(Vec3 &a, Vec3 b) {a.x+=b.x; a.y+=b.y; a.z+=b.z; return a;}
    friend Vec3& operator-=(Vec3 &a, Vec3 b) {a.x-=b.x; a.y-=b.y; a.z-=b.z; return a;}
    friend Vec3& operator*=(Vec3 &a, T s) {a.x*=s; a.y*=s; a.z*=s; return a;}
    friend Vec3& operator/=(Vec3 &a, T s) {if(s) {a.x/=s; a.y/=s; a.z/=s;} else {a.x=a.y=a.z=0;} return a;}
    friend T operator*(Vec3 a, Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
    friend bool operator==(Vec3 a, Vec3 b) {return a.x==b.x && a.y==b.y && a.z==b.z;}
    friend bool operator!=(Vec3 a, Vec3 b) {return !(a==b);}
};

template<class T>
class Vec2
{
public:
    T x, y;
    Vec2(T _x=0, T _y=0) : x(_x), y(_y) {}
    virtual ~Vec2() {}
    friend Vec2 operator+(Vec2 a, Vec2 b) {return Vec2(a.x+b.x, a.y+b.y);}
    friend Vec2 operator-(Vec2 a, Vec2 b) {return Vec2(a.x-b.x, a.y-b.y);}
    friend Vec2 operator-(Vec2 a) {return Vec2(-a.x, -a.y);}
    friend Vec2 operator*(Vec2 a, T s) {return Vec2(a.x*s, a.y*s);}
    friend Vec2 operator*(T s, Vec2 a) {return Vec2(a.x*s, a.y*s);}
    friend Vec2 operator/(Vec2 a, T s) {if(s) return Vec2(a.x/s, a.y/s); return Vec2();}
    friend Vec2& operator+=(Vec2 &a, Vec2 b) {a.x+=b.x; a.y+=b.y; return a;}
    friend Vec2& operator-=(Vec2 &a, Vec2 b) {a.x-=b.x; a.y-=b.y; return a;}
    friend Vec2& operator*=(Vec2 &a, T s) {a.x*=s; a.y*=s; return a;}
    friend Vec2& operator/=(Vec2 &a, T s) {if(s) {a.x/=s; a.y/=s;} else {a.x=a.y=0;} return a;}
    friend T operator*(Vec2 a, Vec2 b) {return a.x*b.x+a.y*b.y;}
    friend bool operator==(Vec2 a, Vec2 b) {return a.x==b.x && a.y==b.y;}
    friend bool operator!=(Vec2 a, Vec2 b) {return !(a==b);}
};

#define Vec2i Vec2<int>
#define Vec2f Vec2<float>
#define Vec3i Vec3<int>
#define Vec3f Vec3<float>

Vec3i mouseRaycast(Vec2i v);
void renderRubikCube();
void rotateRubikCube(Vec2i deltaPos);
void turnFace(int face, bool clockWise);
void randomizeRubikCube(int limit=1);
void tryTurnFace(Vec3i current, Vec3i last);
void renderSlider();

const int WINDOWWIDTH=GetSystemMetrics(SM_CXSCREEN), WINDOWHEIGHT=GetSystemMetrics(SM_CYSCREEN);
const int white=0, blue=1, red=2, green=3, orange=4, yellow=5, opposing[6]={yellow, green, orange, blue, red, white};
int faces[6][3][3], leftFaceOf[6]={4, 4, 1, 2, 3, 4}, aboveFaceOf[6]={1, 5, 5, 5, 5, 3}, sortedClosestFaces[6]={0, 1, 2, 3, 4, 5};
Vec3f normals[6]={Vec3f(0, 1, 0), Vec3f(0, 0, 1), Vec3f(1, 0, 0), Vec3f(0, 0, -1), Vec3f(-1, 0, 0), Vec3f(0, -1, 0)};
Vec2i ScreenCenter(GetSystemMetrics(SM_CXSCREEN)>>1, GetSystemMetrics(SM_CYSCREEN)>>1);
int paintColours[7]={COLOR(255, 255, 255), COLOR(0, 0, 255), COLOR(255, 0, 0), COLOR(0, 255, 0), COLOR(255, 128, 0), COLOR(255, 255, 0), BLACK};
int turnFacesNeighbours[6][4]={{blue, orange, green, red}, {yellow, orange, white, red}, {yellow, blue, white, green}, {yellow, red, white, orange}, {yellow, green, white, blue}, {green, orange, blue, red}};
int unmappedRotationIndices[6][4][2]={{0, -1, 0, -1, 0, -1, 0, -1}, {0, -2, -1, 0, 2, -1, -2, 2}, {-1, 0, -1, 0, -1, 0, -2, 2}, {2, -2, -2, 0, 0, -1, -1, 2}, {-1, 2, -2, 0, -1, 2, -1, 2}, {2, -1, 2, -1, 2, -1, 2, -1}};

int main()
{
    initwindow(WINDOWWIDTH, WINDOWHEIGHT, "Rubik's cube by Ilie Dumitru", -3, -26, true);
    bool appRunning=true, debugDraw=true;
    for(int k=0;k<6;++k)
        for(int i=0;i<3;++i)
            for(int j=0;j<3;++j)
                faces[k][i][j]=k;
    bool pressedLeft=false, pressedRand=false;
    Vec2i lastMousePos(mousex(), mousey()), currentMousePos;
    Vec3i lastIndices(-1, -1, -1), currentMouseIndices(-1, -1, -1);
    srand(time(0));
    while(appRunning)
    {
        cleardevice();
        if(isPressed(VK_ESCAPE))
            appRunning=false;
        bool l=isPressed(VK_LBUTTON), r=isPressed(VK_RBUTTON);
        currentMousePos=Vec2i(mousex(), mousey());
        currentMouseIndices=mouseRaycast(currentMousePos);

        if(l && pressedLeft && !r)
        {
            if(currentMouseIndices.x==-1 || usingSlider)
            {
                //slider for camera distance
                usingSlider=(currentMousePos.y>=cameraDistanceSliderTop && currentMousePos.y<=cameraDistanceSliderTop+cameraDistanceSliderWidth && currentMousePos.x>=cameraDistanceSliderLeft && currentMousePos.x<=cameraDistanceSliderLeft+cameraDistanceSliderLength);
                if(usingSlider)
                {
                    cameraDistanceSliderPosition=currentMousePos.x-cameraDistanceSliderLeft;
                    cameraDistanceSliderPercent=cameraDistanceSliderPosition/(float)cameraDistanceSliderLength;
                }
                cameraDistance=cameraDistanceSliderPercent*(minCameraDistance-maxCameraDistance)+maxCameraDistance;
                if(cameraDistance<minCameraDistance)
                    cameraDistance=minCameraDistance;
                if(cameraDistance>maxCameraDistance)
                    cameraDistance=maxCameraDistance;
            }
            else
                tryTurnFace(currentMouseIndices, lastIndices);
        }
        else if(!l && r)
            rotateRubikCube(currentMousePos-lastMousePos);
        lastIndices=currentMouseIndices;

        {
            //Accessibility feature(WASD can be used to rotate the cube around)
            //Increase this value if the cube is rotating too slow
            int factor=3;
            rotateRubikCube(factor*Vec2i(isPressed('D')-isPressed('A'), isPressed('S')-isPressed('W')));
        }
        //Press 'R' to randomize the cube once
        //Hold 'left shift' while pressing 'r' to randomize the cube 5 times
        //Hold 'left control' while pressing 'r' to randomize the cube 5001 times
        r=isPressed('R');
        if(r && !pressedRand)
        {
            if(isPressed(VK_LCONTROL))
                randomizeRubikCube(5001);
            else if(isPressed(VK_LSHIFT))
                randomizeRubikCube(5);
            else
                randomizeRubikCube();
        }
        pressedLeft=l;
        pressedRand=r;
        lastMousePos=currentMousePos;
        renderRubikCube();
        renderSlider();
        if(debugDraw)
        {
            char textToDisplay[100], colsNames[7][7]={"null", "white", "blue", "red", "green", "orange", "yellow"};
            sprintf(textToDisplay, "Mouse over face %s at position (%d, %d)", colsNames[currentMouseIndices.x+1], currentMouseIndices.y, currentMouseIndices.z);
            setcolor(paintColours[green]);
            outtextxy(0, 0, textToDisplay);
        }
        swapbuffers();
        Sleep(1);
    }
    closegraph();
    return 0;
}

void renderSlider()
{
    setfillstyle(1, GREEN);
    setcolor(WHITE);
    bar(cameraDistanceSliderLeft, cameraDistanceSliderTop, cameraDistanceSliderLeft+cameraDistanceSliderLength, cameraDistanceSliderTop+cameraDistanceSliderWidth);
    setfillstyle(1, paintColours[green]);
    bar(cameraDistanceSliderLeft, cameraDistanceSliderTop, cameraDistanceSliderLeft+cameraDistanceSliderPosition, cameraDistanceSliderTop+cameraDistanceSliderWidth);
}

bool cmp(int i, int j)
{
    //scalar product
    //return normals[i]*Vec3f(0, 0, 1)>normals[j]*Vec3f(0, 0, 1);
    //what it really means
    return normals[i].z>normals[j].z;
}

Vec2i posOnScreen(Vec3f v) {return Vec2i(atan2(v.x, cameraDistance-v.z)/M_PI_2*3*blockSize+ScreenCenter.x, atan2(v.y, cameraDistance-v.z)/M_PI_2*3*blockSize+ScreenCenter.y);}

void drawRubikCubeFace(int face, Vec3f center, Vec3f left, Vec3f up)
{
    Vec3f cornerPos=center-left-up;
    Vec3f iAdd=up*2/3, jAdd=left*2/3;
    Vec2i aux;
    int i, j, polig[8];
    for(i=0;i<3;++i)
        for(j=0;j<3;++j)
        {
            setfillstyle(1, paintColours[faces[face][i][j]]);
            aux=posOnScreen(cornerPos+iAdd*i+jAdd*j);
            polig[0]=aux.x;
            polig[1]=aux.y;
            aux=posOnScreen(cornerPos+iAdd*(i+1)+jAdd*j);
            polig[2]=aux.x;
            polig[3]=aux.y;
            aux=posOnScreen(cornerPos+iAdd*(i+1)+jAdd*(j+1));
            polig[4]=aux.x;
            polig[5]=aux.y;
            aux=posOnScreen(cornerPos+iAdd*i+jAdd*(j+1));
            polig[6]=aux.x;
            polig[7]=aux.y;
            fillpoly(4, polig);
        }
}

Vec3i mouseRaycast(Vec2i v)
{
    Vec2i polig[4], AABBm, AABBM;
    int i, j, k, h, linesIntersected;
    for(h=0;h<3;++h)
    {
        Vec3f center=normals[sortedClosestFaces[h]]*1.5f*blockSize;
        Vec3f left=(normals[leftFaceOf[sortedClosestFaces[h]]]+normals[sortedClosestFaces[h]])*1.5f*blockSize-center;
        Vec3f up=(normals[aboveFaceOf[sortedClosestFaces[h]]]+normals[sortedClosestFaces[h]])*1.5f*blockSize-center;
        Vec3f cornerPos=center-left-up;
        Vec3f iAdd=up*2/3, jAdd=left*2/3;
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
            {
                polig[0]=posOnScreen(cornerPos+iAdd*i+jAdd*j);
                polig[1]=posOnScreen(cornerPos+iAdd*(i+1)+jAdd*j);
                polig[2]=posOnScreen(cornerPos+iAdd*(i+1)+jAdd*(j+1));
                polig[3]=posOnScreen(cornerPos+iAdd*i+jAdd*(j+1));
                AABBm=AABBM=polig[0];
                for(k=1;k<4;++k)
                    AABBm.x=min(AABBm.x, polig[k].x), AABBm.y=min(AABBm.y, polig[k].y), AABBM.x=max(AABBM.x, polig[k].x), AABBM.y=max(AABBM.y, polig[k].y);
                //we are in the bounding box of the cell
                if(v.x>=AABBm.x && v.x<=AABBM.x && v.y>=AABBm.y && v.y<=AABBM.y)
                {
                    for(k=linesIntersected=0;k<4;++k)
                    {
                        AABBm.x=min(polig[k].x, polig[(k+1)&3].x);
                        AABBm.y=min(polig[k].y, polig[(k+1)&3].y);
                        AABBM.x=max(polig[k].x, polig[(k+1)&3].x);
                        AABBM.y=max(polig[k].y, polig[(k+1)&3].y);
                        if(v.y>=AABBm.y && v.y<AABBM.y)
                        {
                            if(v.x<=AABBm.x)
                                linesIntersected^=1;
                            else if(v.x<=AABBM.x)
                                //Here we make use of the cross product of two 2D vectors to check if a line from the
                                //cursor to the right side of the screen intersects the polygon line
                                linesIntersected^=(((v.x-polig[k].x)*(polig[(k+1)&3].y-polig[k].y)-(polig[(k+1)&3].x-polig[k].x)*(v.y-polig[k].y))*((GetSystemMetrics(SM_CXSCREEN))*(polig[(k+1)&3].y-polig[k].y)-(polig[(k+1)&3].x-polig[k].x)*(v.y-polig[k].y))<=0);
                        }
                    }
                    if(linesIntersected)
                        return Vec3i(sortedClosestFaces[h], 2-i, 2-j);
                }
            }
    }
    return Vec3i(-1, -1, -1);
}

void renderRubikCube()
{
    int i;
    std::sort(sortedClosestFaces, sortedClosestFaces+6, cmp);
    setcolor(BLACK);
    for(i=2;i>-1;--i) {drawRubikCubeFace(sortedClosestFaces[i], normals[sortedClosestFaces[i]]*1.5f*blockSize, normals[leftFaceOf[sortedClosestFaces[i]]]*1.5f*blockSize, normals[aboveFaceOf[sortedClosestFaces[i]]]*1.5f*blockSize);}
}

void turnFace(int face, bool clockwise)
{
    #define mapFunction(x) x*(x>-1)+i*(x==-1)+(2-i)*(x==-2)
    int wh[3][3], aux[3], i, j, x, y, z, t;
    if(clockwise)
    {
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                wh[i][j]=faces[face][2-j][i];
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                faces[face][i][j]=wh[i][j];
        for(i=0;i<3;++i)
        {
            x=unmappedRotationIndices[face][0][0];
            y=unmappedRotationIndices[face][0][1];
            aux[i]=faces[turnFacesNeighbours[face][0]][mapFunction(x)][mapFunction(y)];
            for(j=1;j<4;++j)
            {
                z=unmappedRotationIndices[face][j][0];
                t=unmappedRotationIndices[face][j][1];
                faces[turnFacesNeighbours[face][j-1]][mapFunction(x)][mapFunction(y)]=faces[turnFacesNeighbours[face][j]][mapFunction(z)][mapFunction(t)];
                x=z;
                y=t;
            }
            faces[turnFacesNeighbours[face][3]][mapFunction(x)][mapFunction(y)]=aux[i];
        }
    }
    else
    {
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                wh[i][j]=faces[face][j][2-i];
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                faces[face][i][j]=wh[i][j];
        for(i=0;i<3;++i)
        {
            x=unmappedRotationIndices[face][0][0];
            y=unmappedRotationIndices[face][0][1];
            aux[i]=faces[turnFacesNeighbours[face][0]][mapFunction(x)][mapFunction(y)];
            for(j=3;j>0;--j)
            {
                z=unmappedRotationIndices[face][j][0];
                t=unmappedRotationIndices[face][j][1];
                faces[turnFacesNeighbours[face][(j+1)&3]][mapFunction(x)][mapFunction(y)]=faces[turnFacesNeighbours[face][j]][mapFunction(z)][mapFunction(t)];
                x=z;
                y=t;
            }
            faces[turnFacesNeighbours[face][1]][mapFunction(x)][mapFunction(y)]=aux[i];
        }
    }
    #undef mapFunction
}

void rotateRubikCube(Vec2i deltaPos)
{
    float SIN, COS, aux;
    int i;
    if(deltaPos.x)
    {
        SIN=sin(deltaPos.x*D2R);
        COS=cos(deltaPos.x*D2R);
        for(i=0;i<6;++i)
        {
            //rotation along the y axis
            aux=normals[i].x;
            normals[i].x=SIN*normals[i].z+COS*aux;
            normals[i].z=COS*normals[i].z-SIN*aux;
        }
    }
    if(deltaPos.y)
    {
        SIN=sin(deltaPos.y*D2R);
        COS=cos(deltaPos.y*D2R);
        for(i=0;i<6;++i)
        {
            //rotation along the x axis
            aux=normals[i].y;
            normals[i].y=SIN*normals[i].z+COS*aux;
            normals[i].z=COS*normals[i].z-SIN*aux;
        }
    }
}

void randomizeRubikCube(int limit) {while(limit-->0) turnFace(rand()%6, rand()&1);}

void tryTurnFace(Vec3i current, Vec3i last)
{
    if(last.x!=-1 && current.x!=-1 && current!=last)
    {
        if(current.x!=last.x)
        {
            //different face
            if(current.x==aboveFaceOf[last.x])
            {
                if(!last.z)
                    turnFace(leftFaceOf[last.x], false);
                else if(last.z==2)
                    turnFace(opposing[leftFaceOf[last.x]], true);
            }
            else if(current.x==leftFaceOf[last.x])
            {
                if(!last.y)
                    turnFace(aboveFaceOf[last.x], true);
                else if(last.y==2)
                    turnFace(opposing[aboveFaceOf[last.x]], false);
            }
            else if(current.x==opposing[aboveFaceOf[last.x]])
            {
                if(!last.z)
                    turnFace(leftFaceOf[last.x], true);
                else if(last.z==2)
                    turnFace(opposing[leftFaceOf[last.x]], false);
            }
            else if(current.x==opposing[leftFaceOf[last.x]])
            {
                if(!last.y)
                    turnFace(aboveFaceOf[last.x], false);
                else if(last.y==2)
                    turnFace(opposing[aboveFaceOf[last.x]], true);
            }
        }
        else if(current.y!=last.y)
        {
            //different line
            if(!last.z)
                turnFace(leftFaceOf[current.x], current.y>last.y);
            else if(last.z==2)
                turnFace(opposing[leftFaceOf[current.x]], current.y<last.y);
        }
        else
        {
            //different column
            if(!last.y)
                turnFace(aboveFaceOf[current.x], current.z<last.z);
            else if(last.y==2)
                turnFace(opposing[aboveFaceOf[current.x]], current.z>last.z);
        }
    }
}