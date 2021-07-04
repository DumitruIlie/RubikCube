//Ilie Dumitru
#include<graphics.h>
#include<algorithm>
#include<cmath>
#include<ctime>

//If you want a bigger cube, increase this value
const int blockSize=100;
//If you want the cube to be farther away, increase this value
//WARNING: bringing the cube too close could cause undefined behavior
const float cameraDistance=5*blockSize;

inline int min(int a, int b) {return a+(b-a)*(b<a);}
inline int max(int a, int b) {return a+(b-a)*(b>a);}
inline float abs(float x) {return -x+2*x*(x>0);}
const float D2R=M_PI/180;

template<class T>
struct Vec3
{
    T x, y, z;
    Vec3(T _x=0, T _y=0, T _z=0) : x(_x), y(_y), z(_z) {}
    virtual ~Vec3() {}
    friend Vec3 operator+(Vec3 a, Vec3 b) {return Vec3(a.x+b.x, a.y+b.y, a.z+b.z);}
    friend Vec3 operator-(Vec3 a, Vec3 b) {return Vec3(a.x-b.x, a.y-b.y, a.z-b.z);}
    friend Vec3 operator-(Vec3 a) {return Vec3(-a.x, -a.y, -a.z);}
    friend Vec3 operator*(Vec3 a, T s) {return Vec3(a.x*s, a.y*s, a.z*s);}
    friend Vec3 operator*(T s, Vec3 a) {return Vec3(a.x*s, a.y*s, a.z*s);}
    friend Vec3 operator/(Vec3 a, T s) {if(s) return Vec3(a.x/s, a.y/s, a.z/s); return Vec3();}
    friend void operator+=(Vec3 &a, Vec3 b) {a.x+=b.x; a.y+=b.y; a.z+=b.z;}
    friend void operator-=(Vec3 &a, Vec3 b) {a.x-=b.x; a.y-=b.y; a.z-=b.z;}
    friend void operator*=(Vec3 &a, T s) {a.x*=s; a.y*=s; a.z*=s;}
    friend void operator/=(Vec3 &a, T s) {if(s) {a.x/=s; a.y/=s; a.z/=s;} else {a.x=a.y=a.z=0;}}
    friend T operator*(Vec3 a, Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
    friend bool operator==(Vec3 a, Vec3 b) {return a.x==b.x && a.y==b.y && a.z==b.z;}
    friend bool operator!=(Vec3 a, Vec3 b) {return !(a==b);}
};

template<class T>
struct Vec2
{
    T x, y;
    Vec2(T _x=0, T _y=0) : x(_x), y(_y) {}
    virtual ~Vec2() {}
    friend Vec2 operator+(Vec2 a, Vec2 b) {return Vec2(a.x+b.x, a.y+b.y);}
    friend Vec2 operator-(Vec2 a, Vec2 b) {return Vec2(a.x-b.x, a.y-b.y);}
    friend Vec2 operator-(Vec2 a) {return Vec2(-a.x, -a.y);}
    friend Vec2 operator*(Vec2 a, T s) {return Vec2(a.x*s, a.y*s);}
    friend Vec2 operator*(T s, Vec2 a) {return Vec2(a.x*s, a.y*s);}
    friend Vec2 operator/(Vec2 a, T s) {if(s) return Vec2(a.x/s, a.y/s); return Vec2();}
    friend void operator+=(Vec2 &a, Vec2 b) {a.x+=b.x; a.y+=b.y;}
    friend void operator-=(Vec2 &a, Vec2 b) {a.x-=b.x; a.y-=b.y;}
    friend void operator*=(Vec2 &a, T s) {a.x*=s; a.y*=s;}
    friend void operator/=(Vec2 &a, T s) {if(s) {a.x/=s; a.y/=s;} else {a.x=a.y=0;}}
    friend T operator*(Vec2 a, Vec2 b) {return a.x*b.x+a.y*b.y;}
};

#define Vec2i Vec2<int>
#define Vec2f Vec2<float>
#define Vec3i Vec3<int>
#define Vec3f Vec3<float>

inline bool isPressed(int key) {return GetKeyState(key)&0x80000;}

Vec3i raycast(Vec2i v);
void render();
void turning(Vec2i deltaPos);
void rotateFace(int face, bool clockWise);
void randomizeRubik(int limit=5);

const int WINDOWWIDTH=GetSystemMetrics(SM_CXSCREEN), WINDOWHEIGHT=GetSystemMetrics(SM_CYSCREEN);
const int white=0, blue=1, red=2, green=3, orange=4, yellow=5, opposing[6]={yellow, green, orange, blue, red, white};
int faces[6][3][3], leftOf[6]={4, 4, 1, 2, 3, 4}, upOf[6]={1, 5, 5, 5, 5, 3}, closestFaces[6]={0, 1, 2, 3, 4, 5};
Vec3f normals[6]={Vec3f(0, 1, 0), Vec3f(0, 0, 1), Vec3f(1, 0, 0), Vec3f(0, 0, -1), Vec3f(-1, 0, 0), Vec3f(0, -1, 0)};
Vec2i ScreenCenter(GetSystemMetrics(SM_CXSCREEN)>>1, GetSystemMetrics(SM_CYSCREEN)>>1);
int colours[7]={COLOR(255, 255, 255), COLOR(0, 0, 255), COLOR(255, 0, 0), COLOR(0, 255, 0), COLOR(255, 128, 0), COLOR(255, 255, 0), BLACK};
int rotation[6][4]={{blue, orange, green, red}, {yellow, orange, white, red}, {yellow, blue, white, green}, {yellow, red, white, orange}, {yellow, green, white, blue}, {green, orange, blue, red}};
int rotationIndexes[6][4][2]={{0, -1, 0, -1, 0, -1, 0, -1}, {0, -2, -1, 0, 2, -1, -2, 2}, {-1, 0, -1, 0, -1, 0, -2, 2}, {2, -2, -2, 0, 0, -1, -1, 2}, {-1, 2, -2, 0, -1, 2, -1, 2}, {2, -1, 2, -1, 2, -1, 2, -1}};

void turnFace(Vec3i current, Vec3i last);

int main()
{
    initwindow(WINDOWWIDTH, WINDOWHEIGHT, "Rubik's cube by Ilie Dumitru", -3, -26, true);
    bool appRunning=true;
    for(int k=0;k<6;++k)
        for(int i=0;i<3;++i)
            for(int j=0;j<3;++j)
                faces[k][i][j]=k;
    bool pressedLeft=false, pressedRand=false;
    Vec2i lastMousePos(mousex(), mousey()), currentMousePos;
    Vec3i lastIndices(-1, -1, -1);
    srand(time(0));
    while(appRunning)
    {
        if(isPressed(VK_ESCAPE))
            appRunning=false;
        bool l=isPressed(VK_LBUTTON), r=isPressed(VK_RBUTTON);
        currentMousePos=Vec2i(mousex(), mousey());
        if(l && !pressedLeft && !r)
            lastIndices=raycast(currentMousePos);
        else if(l && pressedLeft && !r)
        {
            Vec3i indices=raycast(currentMousePos);
            turnFace(indices, lastIndices);
            lastIndices=indices;
        }
        else if(!l && r)
            turning(currentMousePos-lastMousePos);
        {
            //Accessibility feature(WASD can be used to rotate the cube around)
            //Increase this value if the cube is rotating too slow
            int factor=3;
            turning(factor*Vec2i(isPressed('D')-isPressed('A'), isPressed('S')-isPressed('W')));
        }
        //Press 'R' to randomize the cube once
        //Hold 'left shift' while pressing 'r' to randomize the cube 5 times
        //Hold 'left control' while pressing 'r' to randomize the cube 5000 times
        r=isPressed('R');
        if(r && !pressedRand)
        {
            if(isPressed(VK_LCONTROL))
                randomizeRubik(5000);
            else if(isPressed(VK_LSHIFT))
                randomizeRubik(5);
            else
                randomizeRubik(1);
        }
        pressedLeft=l;
        pressedRand=r;
        lastMousePos=currentMousePos;
        render();
        swapbuffers();
        Sleep(1);
    }
    closegraph();
    return 0;
}

bool cmp(int i, int j) {return normals[i]*Vec3f(0, 0, 1)>normals[j]*Vec3f(0, 0, 1);}

Vec2i posOnScreen(Vec3f v)
{
    /*float cameraDist=5*blockSize;
    Vec3f delta=Vec3(v.x, v.y, cameraDist-v.z);
    float theta=atan2(delta.x, delta.z), phi=atan2(delta.y, delta.z);
    return Vec2i(theta/M_PI_2*3*blockSize+ScreenCenter.x, phi/M_PI_2*3*blockSize+ScreenCenter.y);*/
    return Vec2i(atan2(v.x, cameraDistance-v.z)/M_PI_2*3*blockSize+ScreenCenter.x, atan2(v.y, cameraDistance-v.z)/M_PI_2*3*blockSize+ScreenCenter.y);
}

void drawFace(int face, Vec3f center, Vec3f left, Vec3f up)
{
    Vec3f cornerPos=center-left-up;
    Vec3f iAdd=up*2/3, jAdd=left*2/3;
    Vec2i aux;
    int i, j, polig[8];
    for(i=0;i<3;++i)
        for(j=0;j<3;++j)
        {
            setfillstyle(1, colours[faces[face][i][j]]);
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

Vec3i raycast(Vec2i v)
{
    Vec2i polig[4], AABBm, AABBM;
    int i, j, k, h, linesIntersected;
    for(h=0;h<3;++h)
    {
        Vec3f center=normals[closestFaces[h]]*1.5f*blockSize;
        Vec3f left=(normals[leftOf[closestFaces[h]]]+normals[closestFaces[h]])*1.5f*blockSize-center;
        Vec3f up=(normals[upOf[closestFaces[h]]]+normals[closestFaces[h]])*1.5f*blockSize-center;
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
                if(v.x>AABBm.x && v.x<AABBM.x && v.y>AABBm.y && v.y<AABBM.y)
                {
                    for(k=linesIntersected=0;k<4;++k)
                    {
                        AABBm.y=min(polig[k].y, polig[(k+1)&3].y);
                        AABBM.y=max(polig[k].y, polig[(k+1)&3].y);
                        if(v.y>=AABBm.y && v.y<=AABBM.y)
                            //Here we make use of the cross product of two 2D vectors to check if a line from the
                            //cursor to the right side of the screen intersects the polygon line
                            linesIntersected^=(((v.x-polig[k].x)*(polig[(k+1)&3].y-polig[k].y)-(polig[(k+1)&3].x-polig[k].x)*(v.y-polig[k].y))*((GetSystemMetrics(SM_CXSCREEN))*(polig[(k+1)&3].y-polig[k].y)-(polig[(k+1)&3].x-polig[k].x)*(v.y-polig[k].y))<0);
                    }
                    if(linesIntersected)
                        return Vec3i(closestFaces[h], 2-i, 2-j);
                }
            }
    }
    return Vec3i(-1, -1, -1);
}

void render()
{
    int i;
    cleardevice();
    std::sort(closestFaces, closestFaces+6, cmp);
    Vec3f center, left, up;
    setcolor(BLACK);
    for(i=2;i>-1;--i)
    {
        center=normals[closestFaces[i]]*1.5f*blockSize;
        left=(normals[leftOf[closestFaces[i]]]+normals[closestFaces[i]])*1.5f*blockSize-center;
        up=(normals[upOf[closestFaces[i]]]+normals[closestFaces[i]])*1.5f*blockSize-center;
        drawFace(closestFaces[i], center, left, up);
    }
}

#define getter(x) (x==-1)?i:(x==-2?2-i:x)
void rotateFace(int face, bool clockwise)
{
    int wh[3][3], aux[3], i, j, x, y, z, t;
    if(clockwise)
    {
        wh[0][0]=faces[face][2][0];
        wh[0][1]=faces[face][1][0];
        wh[0][2]=faces[face][0][0];
        wh[1][2]=faces[face][0][1];
        wh[2][2]=faces[face][0][2];
        wh[2][1]=faces[face][1][2];
        wh[2][0]=faces[face][2][2];
        wh[1][0]=faces[face][2][1];
        wh[1][1]=face;
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                faces[face][i][j]=wh[i][j];
        for(i=0;i<3;++i)
        {
            x=rotationIndexes[face][0][0], y=rotationIndexes[face][0][1];
            aux[i]=faces[rotation[face][0]][getter(x)][getter(y)];
            z=rotationIndexes[face][1][0], t=rotationIndexes[face][1][1];
            faces[rotation[face][0]][getter(x)][getter(y)]=faces[rotation[face][1]][getter(z)][getter(t)];
            x=z;
            y=t;
            z=rotationIndexes[face][2][0], t=rotationIndexes[face][2][1];
            faces[rotation[face][1]][getter(x)][getter(y)]=faces[rotation[face][2]][getter(z)][getter(t)];
            x=z;
            y=t;
            z=rotationIndexes[face][3][0], t=rotationIndexes[face][3][1];
            faces[rotation[face][2]][getter(x)][getter(y)]=faces[rotation[face][3]][getter(z)][getter(t)];
            x=z;
            y=t;
            faces[rotation[face][3]][getter(x)][getter(y)]=aux[i];
        }
    }
    else
    {
        wh[0][0]=faces[face][0][2];
        wh[0][1]=faces[face][1][2];
        wh[0][2]=faces[face][2][2];
        wh[1][2]=faces[face][2][1];
        wh[2][2]=faces[face][2][0];
        wh[2][1]=faces[face][1][0];
        wh[2][0]=faces[face][0][0];
        wh[1][0]=faces[face][0][1];
        wh[1][1]=face;
        for(i=0;i<3;++i)
            for(j=0;j<3;++j)
                faces[face][i][j]=wh[i][j];
        for(i=0;i<3;++i)
        {
            x=rotationIndexes[face][0][0], y=rotationIndexes[face][0][1];
            aux[i]=faces[rotation[face][0]][getter(x)][getter(y)];
            z=rotationIndexes[face][3][0], t=rotationIndexes[face][3][1];
            faces[rotation[face][0]][getter(x)][getter(y)]=faces[rotation[face][3]][getter(z)][getter(t)];
            x=z;
            y=t;
            z=rotationIndexes[face][2][0], t=rotationIndexes[face][2][1];
            faces[rotation[face][3]][getter(x)][getter(y)]=faces[rotation[face][2]][getter(z)][getter(t)];
            x=z;
            y=t;
            z=rotationIndexes[face][1][0], t=rotationIndexes[face][1][1];
            faces[rotation[face][2]][getter(x)][getter(y)]=faces[rotation[face][1]][getter(z)][getter(t)];
            x=z;
            y=t;
            faces[rotation[face][1]][getter(x)][getter(y)]=aux[i];
        }
    }
}
#undef getter

void turning(Vec2i deltaPos)
{
    float SIN=sin(deltaPos.x*D2R), COS=cos(deltaPos.x*D2R), aux;
    int i;
    if(deltaPos.x)
    {
        for(i=0;i<6;++i)
        {
            aux=normals[i].x;
            normals[i].x=SIN*normals[i].z+COS*aux;
            normals[i].z=COS*normals[i].z-SIN*aux;
        }
    }
    SIN=sin(deltaPos.y*D2R);
    COS=cos(deltaPos.y*D2R);
    if(deltaPos.y)
    {
        for(i=0;i<6;++i)
        {
            aux=normals[i].y;
            normals[i].y=SIN*normals[i].z+COS*aux;
            normals[i].z=COS*normals[i].z-SIN*aux;
        }
    }
}

void randomizeRubik(int limit) {while(limit--) rotateFace(rand()%6, rand()&1);}

void turnFace(Vec3i current, Vec3i last)
{
    if(last.x!=-1 && current.x!=-1 && current!=last)
    {
        if(current.x!=last.x)
        {
            if(current.x==upOf[last.x])
            {
                if(!last.z)
                    rotateFace(leftOf[last.x], false);
                else if(last.z==2)
                    rotateFace(opposing[leftOf[last.x]], true);
            }
            else if(current.x==leftOf[last.x])
            {
                if(!last.y)
                    rotateFace(upOf[last.x], true);
                else if(last.y==2)
                    rotateFace(opposing[upOf[last.x]], false);
            }
            else if(current.x==opposing[upOf[last.x]])
            {
                if(!last.z)
                    rotateFace(leftOf[last.x], true);
                else if(last.z==2)
                    rotateFace(opposing[leftOf[last.x]], false);
            }
            else if(current.x==opposing[leftOf[last.x]])
            {
                if(!last.y)
                    rotateFace(upOf[last.x], false);
                else if(last.y==2)
                    rotateFace(opposing[upOf[last.x]], true);
            }
        }
        else if(current.y!=last.y)
        {
            if(!last.z)
                rotateFace(leftOf[current.x], current.y>last.y);
            else if(last.z==2)
                rotateFace(opposing[leftOf[current.x]], current.y<last.y);
        }
        else
        {
            if(!last.y)
                rotateFace(upOf[current.x], current.z<last.z);
            else if(last.y==2)
                rotateFace(opposing[upOf[current.x]], current.z>last.z);
        }
    }
}