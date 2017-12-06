#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<fstream>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>

using namespace cv;
using namespace std;
Mat img;
int len=60*4, dir=2, posx=400, posy=300, *arrx, *arry, scoreold=0,
fx,fy,fc=1,leninc=20,score=0,oldvalue=10,slim=0,level=4,count=0,
showbonus=0,bx,by,bc=0,levelstat=0;

void exitnow()
{
  cout<<"Game Over!\nScore: "<<score<<endl;
  if (score>scoreold)
  {
    cout<<"You have got a new high score!"<<endl;
    ofstream fout;
    fout.open("snake.txt",ios::out| ios::binary);
    fout<<score;
    fout.close();
  }
  cout<<"Gramercy..."<<endl;
  exit(0);
}

void setDir(char a)
{
    if (a=='a' && dir!=2)
      dir=1; //left
    if (a=='d'  && dir!=1)
      dir=2; //right
    if (a=='w' && dir!=4)
      dir=3; //front
    if (a=='s' && dir!=3)
      dir=4; //back
}

void setPos()
{
  int arx,ary;
  if (dir==1)
  {
    for (int i=len-1;i>0;i--)
      arrx[i]=arrx[i-1];
    for (int i=len-1;i>0;i--)
      arry[i]=arry[i-1];
    arrx[0]-=1;
    if (arrx[0]<0) arrx[0]=799;
  }
  if (dir==2)
  {
    for (int i=len-1;i>0;i--)
      arrx[i]=arrx[i-1];
    for (int i=len-1;i>0;i--)
      arry[i]=arry[i-1];
    arrx[0]+=1;
    if (arrx[0]>799) arrx[0]=0;
  }
  if (dir==3)
  {
    for (int i=len-1;i>0;i--)
      arry[i]=arry[i-1];
    for (int i=len-1;i>0;i--)
      arrx[i]=arrx[i-1];
    arry[0]-=1;
    if (arry[0]<0) arry[0]=599;
  }
  if (dir==4)
  {
    for (int i=len-1;i>0;i--)
      arry[i]=arry[i-1];
    for (int i=len-1;i>0;i--)
      arrx[i]=arrx[i-1];
    arry[0]+=1;
    if (arry[0]>599) arry[0]=0;
  }
}

void giveFood()
{
  if (fc==1)
  {
    srand(::count);
    fx=50+rand()%700;fy=50+rand()%500;
    fc=0;
  }
  for (int i=fx-(12-level);i<=fx+(12-level);i++)
    for (int j=fy-(12-level);j<=fy+(12-level);j++)
     img.at<uchar>(j, i) =200;
  ::count++;
}

void sprint()
{
  for (int i=0;i<img.rows;i++)
    for (int j=0; j<img.cols;j++)
      img.at<uchar>(i, j) = 0;
  for (int i=0;i<len;i++)
  {
    int x=arrx[i],y=arry[i];
    img.at<uchar>(y, x) = 255;
  }
}

void incLen()
{
  int xd=arrx[len-2]-arrx[len-1];
  int yd=arry[len-2]-arry[len-1];
  arrx=(int*)realloc(arrx,(len+leninc)*sizeof(int));
  arry=(int*)realloc(arry,(len+leninc)*sizeof(int));
  if (xd>0)
  {
    for (int i=len;i<len+leninc;i++)
      {
        arrx[i]=arrx[i-1]-1;
        if (arrx[i]<0) arrx[i]+=800;
      }
    for (int i=len;i<len+leninc;i++)
      arry[i]=arry[i-1];
  }
  if (xd<0)
  {
    for (int i=len;i<len+leninc;i++)
      {
        arrx[i]=arrx[i-1]+1;
        if (arrx[i]>799) arrx[i]-=800;
      }
    for (int i=len;i<len+leninc;i++)
      arry[i]=arry[i-1];
  }
  if (yd>0)
  {
    for (int i=len;i<len+leninc;i++)
      {
        arry[i]=arry[i-1]-1;
        if (arry[i]<0) arry[i]+=800;
      }
    for (int i=len;i<len+leninc;i++)
      arrx[i]=arrx[i-1];
  }
  if (yd<0)
  {
    for (int i=len;i<len+leninc;i++)
      {
        arry[i]=arry[i-1]+1;
        if (arry[i]>799) arry[i]-=800;
      }
    for (int i=len;i<len+leninc;i++)
      arrx[i]=arrx[i-1];
  }
  len+=leninc;
  slim=0;
}

void checkFood()
{
  int flag=0;
  for (int i=fx-(12-level);i<=fx+(12-level);i++)
    for (int j=fy-(12-level);j<=fy+(12-level);j++)
      if (arrx[0]==i && arry[0]==j)
       {flag=1; break;}
  if (flag==1)
    {incLen(); score++; if (score%4==0) {showbonus=1;bc=1;} fc=1;}
}

void gameOver()
{
  int lennow=len;
  if (slim>10)
    lennow-=5;
  int x=arrx[0],y=arry[0];
  for (int i=1;i<lennow;i++)
    if (arrx[i]==x && arry[i]==y)
    exitnow();
}
void a(int value, void*)
{
    printf("Level set to: %2d\n",value);
    if (value<oldvalue)
    {
    arrx=(int*)realloc(arrx,(len-leninc)*sizeof(int));
    arry=(int*)realloc(arry,(len-leninc)*sizeof(int));
    len-=leninc;
    }
    else
      incLen();
     oldvalue=value;
}

void printBonus()
{
  if (bc==1)
  {bx=50+rand()%700;by=50+rand()%500;}
  if ((bc/10)%2==0)
    for (int i=bx-6;i<=bx+6;i++)
    for (int j=by-6;j<=by+6;j++)
       img.at<uchar>(j, i) =200;
  bc++;
}

void checkBonus()
{
  int flag=0;
  for (int i=bx-6;i<=bx+6;i++)
    for (int j=by-6;j<=by+6;j++)
      if (arrx[0]==i && arry[0]==j)
       {flag=1; break;}
  if (flag==1)
    {incLen(); incLen();score+=2; if (score%4==0) {showbonus=1;bc=1;} showbonus=0; bc=0;}
  if (bc>650) {showbonus=0;bc=0;}
}

void itoa(int num, char ch[])
{
    if (num==0)  {ch[0]='0'; ch[1]='\0'; return;}
    int c=0;
    while (num!=0)
    {
        ch[c++]=(num%10)+'0';
        num/=10;
    }
    ch[c]='\0';
    for (int i=0;i<c/2;i++)
    {
        char temp=ch[i];
        ch[i]=ch[c-i-1];
        ch[c-i-1]=temp;
    }
}

int main()
{
    ifstream fin;
    fin.open("snake.txt",ios::in|ios::binary);
    fin>>scoreold;
    fin.close();
    arrx=(int *)malloc(len*sizeof(int));
    arry=(int *)malloc(len*sizeof(int));
    for (int i=0;i<len;i++)
    {
      arrx[i]=posx-i;
      arry[i]=posy;
    }
    img = Mat::zeros(600,800, CV_8U);
    namedWindow("Snakes",1);
    createTrackbar( "Level: ", "Snakes", &level, 10,a);
    while(1)
    {
      if ((score%10) == 0 && level !=10 && score!=0 && levelstat==0) {level++;a(level,NULL);levelstat=1;}
      if (score%10==1 || score %10==2) levelstat=0;
      slim++;
      char ch='d';
      ch=waitKey((101-level*10));
      if (ch=='p') score++;
      if (ch=='\n') exitnow();
      if (ch==' ') fc=1;
      setDir(ch);
      for (int i=1; i<=8+(1.5*level);i++)
        setPos();
      gameOver();
      sprint();
      if (showbonus==1)
        {printBonus(); checkBonus();}
      giveFood();
      checkFood();
      char adder[4]; itoa(score,adder);
      char s[15]="Score: \0"; strcat(s,adder);//s[7]=(char)(score+'0'); s[8]='\0';
      char sold[17]="Highest score: \0";
       itoa(scoreold,adder);
        strcat(sold,adder);// sold[15]=(char)(scoreold+'0'); sold[16]='\0';
      Point p; p.x=0; p.y=20;
      putText(img,s,p,0,0.5,255);
      p.x=630; putText(img,sold,p,0,0.5,255);
      imshow("Snakes", img);
    }
}
