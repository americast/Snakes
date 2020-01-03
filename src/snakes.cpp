#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<fstream>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sstream> //library used for integer to string conversion

cv::Mat img;
int len=60*4, dir=2, posx=400, posy=300, *arrx, *arry, scoreold=0,
fx,fy,fc=1,leninc=20,score=0,oldvalue=10,slim=0,level=4,count=0,
showbonus=0,bx,by,bc=0,levelstat=0;
std::string sha_score; // to store the hash of score
std::ostringstream sha_temp; // a temporary variable to convert int to string

/* A class for implementing SHA-256
   having some functions which are declared later.

   The function which calculates the SHA of a 
   string => sha256(std::string)[declared later]
   uses these functions as utility functions

   Apart from these some definitions are also there.   	
*/
class SHA256
{
protected:
    typedef unsigned char uint8;
    typedef unsigned int uint32;
    typedef unsigned long long uint64;
 
    const static uint32 sha256_k[];
    static const unsigned int SHA224_256_BLOCK_SIZE = (512/8);
public:
    void init();
    void update(const unsigned char *message, unsigned int len);
    void final(unsigned char *digest);
    static const unsigned int DIGEST_SIZE = ( 256 / 8);
 
protected:
    void transform(const unsigned char *message, unsigned int block_nb);
    unsigned int m_tot_len;
    unsigned int m_len;
    unsigned char m_block[2*SHA224_256_BLOCK_SIZE];
    uint32 m_h[8];
};

//Some definitions used 
#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))
#define SHA2_UNPACK32(x, str)                 \
{                                             \
    *((str) + 3) = (uint8) ((x)      );       \
    *((str) + 2) = (uint8) ((x) >>  8);       \
    *((str) + 1) = (uint8) ((x) >> 16);       \
    *((str) + 0) = (uint8) ((x) >> 24);       \
}
#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((uint32) *((str) + 3)      )    \
           | ((uint32) *((str) + 2) <<  8)    \
           | ((uint32) *((str) + 1) << 16)    \
           | ((uint32) *((str) + 0) << 24);   \
}

//Initializing the array
const unsigned int SHA256::sha256_k[64] = //UL = uint32
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void SHA256::transform(const unsigned char *message, unsigned int block_nb)
{
    uint32 w[64];
    uint32 wv[8];
    uint32 t1, t2;
    const unsigned char *sub_block;
    int i;
    int j;
    for (i = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);
        for (j = 0; j < 16; j++) {
            SHA2_PACK32(&sub_block[j << 2], &w[j]);
        }
        for (j = 16; j < 64; j++) {
            w[j] =  SHA256_F4(w[j -  2]) + w[j -  7] + SHA256_F3(w[j - 15]) + w[j - 16];
        }
        for (j = 0; j < 8; j++) {
            wv[j] = m_h[j];
        }
        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }
        for (j = 0; j < 8; j++) {
            m_h[j] += wv[j];
        }
    }
}
 
void SHA256::init()
{
    m_h[0] = 0x6a09e667;
    m_h[1] = 0xbb67ae85;
    m_h[2] = 0x3c6ef372;
    m_h[3] = 0xa54ff53a;
    m_h[4] = 0x510e527f;
    m_h[5] = 0x9b05688c;
    m_h[6] = 0x1f83d9ab;
    m_h[7] = 0x5be0cd19;
    m_len = 0;
    m_tot_len = 0;
}
 
void SHA256::update(const unsigned char *message, unsigned int len)
{
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;
    tmp_len = SHA224_256_BLOCK_SIZE - m_len;
    rem_len = len < tmp_len ? len : tmp_len;
    memcpy(&m_block[m_len], message, rem_len);
    if (m_len + len < SHA224_256_BLOCK_SIZE) {
        m_len += len;
        return;
    }
    new_len = len - rem_len;
    block_nb = new_len / SHA224_256_BLOCK_SIZE;
    shifted_message = message + rem_len;
    transform(m_block, 1);
    transform(shifted_message, block_nb);
    rem_len = new_len % SHA224_256_BLOCK_SIZE;
    memcpy(m_block, &shifted_message[block_nb << 6], rem_len);
    m_len = rem_len;
    m_tot_len += (block_nb + 1) << 6;
}
 
void SHA256::final(unsigned char *digest)
{
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;
    int i;
    block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9)
                     < (m_len % SHA224_256_BLOCK_SIZE)));
    len_b = (m_tot_len + m_len) << 3;
    pm_len = block_nb << 6;
    memset(m_block + m_len, 0, pm_len - m_len);
    m_block[m_len] = 0x80;
    SHA2_UNPACK32(len_b, m_block + pm_len - 4);
    transform(m_block, block_nb);
    for (i = 0 ; i < 8; i++) {
        SHA2_UNPACK32(m_h[i], &digest[i << 2]);
    }
}
 
//Function for calculating SHA-256 of a string
std::string sha256(std::string input)
{
    unsigned char digest[SHA256::DIGEST_SIZE];
    memset(digest,0,SHA256::DIGEST_SIZE);
 
    SHA256 ctx = SHA256();
    ctx.init();
    ctx.update( (unsigned char*)input.c_str(), input.length());
    ctx.final(digest);
 
    char buf[2*SHA256::DIGEST_SIZE+1];
    buf[2*SHA256::DIGEST_SIZE] = 0;
    for (unsigned int i = 0; i < SHA256::DIGEST_SIZE; i++)
        sprintf(buf+i*2, "%02x", digest[i]);
    return std::string(buf);
} 

void exitnow() //to exit the game
{
  std::cout<<"Game Over!\nScore: "<<score<<std::endl;

  if (score>scoreold)// if current score grater than last high score
  {
    std::cout<<"You have got a new high score!"<<std::endl;
    std::ofstream fout;
    fout.open("snake.txt",std::ios::out);
    sha_temp.str("");
    sha_temp << score;
    fout<<sha256(sha_temp.str());
    fout.close();
  }
  std::cout<<"Gramercy..."<<std::endl;
  exit(0);
}

void setDir(char a) //to set direction
{
    if (a=='a' && dir!=2) //when 'a' is pressed and snake is not moving towards right
      dir=1; //left
    if (a=='d' && dir!=1) //when 'd' is pressed and snake is not moving towards left
      dir=2; //right
    if (a=='w' && dir!=4) //when 'w' is pressed and snake is not moving in the back(bottom) direction
      dir=3; //front
    if (a=='s' && dir!=3) //when 's' is pressed and direction is not moving in the front(up) direction
      dir=4; //back
}

void setPos() //to set postion when direction keys are pressed
{
 
  if (dir==1) //left
  {
    for (int i=len-1;i>0;i--)//moving x-coordinate of snake towards left
      arrx[i]=arrx[i-1];
    for (int i=len-1;i>0;i--)//moving y-coordinate of snake towards left
      arry[i]=arry[i-1];
    arrx[0]-=1;
    if(arrx[0]<14) exitnow();//terminate game if snake touches the wall
    
    if(arrx[0]<500 && arrx[0]>300)
    {
  	 if((arry[0]>250&&arry[0]<265) ||(arry[0]>335 && arry[0]<350)) exitnow();}
    }
  if (dir==2) //right
  {
    for (int i=len-1;i>0;i--)//moving x-coordinate of snake towards right
      arrx[i]=arrx[i-1];
    for (int i=len-1;i>0;i--)//moving y-coordinate of snake towards right
      arry[i]=arry[i-1];
    arrx[0]+=1;
    if (arrx[0]>786) exitnow();//terminate game if snake touches the wall
     if(arrx[0]<500&& arrx[0]>300)
     {
      if((arry[0]>250&&arry[0]<265) ||(arry[0]>335 && arry[0]<350)) exitnow(); }
     }
  if (dir==3) //front(up)
  {
    for (int i=len-1;i>0;i--)//moving x-coordinate of snake towards front(up)
      arry[i]=arry[i-1];
    for (int i=len-1;i>0;i--)//moving y-coordinate of snake towards front(up)
      arrx[i]=arrx[i-1];
    arry[0]-=1;
    if (arry[0]<14) exitnow();//terminate game if snake touches the wall

    if((arry[0]>=250&&arry[0]<=265) ||(arry[0]<350 && arry[0]>335))
     {
      if(arrx[0]<500&& arrx[0]>300) exitnow(); 
     }
  }
  if (dir==4) //down(bottom)
  {
    for (int i=len-1;i>0;i--)//moving x-coordinate of snake towards down(bottom)
      arry[i]=arry[i-1];
    for (int i=len-1;i>0;i--)//moving y-coordinate of snake towards down(bottom)
      arrx[i]=arrx[i-1];
    arry[0]+=1;
    if (arry[0]>586) exitnow();//terminate game if snake touches the wall
     if((arry[0]>250&&arry[0]<265) ||(arry[0]<350 && arry[0]>335))
     {
      if(arrx[0]<500&& arrx[0]>300) exitnow(); 
   }
  }
}

void giveFood()//to generate food
{
  if (fc==1)
  {
    do
    {
       srand(::count);
       fx=50+rand()%700;fy=50+rand()%500;//random postion for food b/w 50 to 750 horizontal pixels and 50 to 550 vertical pixels
    }while((fx>300&&fx<500)&&(fy>250&&fy<265));//to regenerate food's coordinate if they are on the mid-walls
    fc=0;
  }
  for (int i=fx-(12-level);i<=fx+(12-level);i++)//determining food's size according to the level(larger for low level and vice versa) 
    for (int j=fy-(12-level);j<=fy+(12-level);j++)//used 12 to make sure food is visble in the last level(10)
     img.at<uchar>(j, i) =200;//food's pixel colour(200) lighter than the snake(255)
  ::count++;
}

void sprint()//to print the map and snake
{
  for (int i=0;i<img.rows;i++)//to print map with walls
    for (int j=0; j<img.cols;j++)
      if((j<=14)||(j>=img.cols-15)||(i<=14)||(i>=img.rows-15))//to print the bounday wall
        img.at<uchar>(i,j) = 200;
      else if(j>=300&&j<=500&&i>=img.rows/2-50&&i<=img.rows/2-35)//to print the upper mid-wall
      	img.at<uchar>(i,j) = 200;
      else if(j>=300&&j<=500&&i>=img.rows/2+35&&i<=img.rows/2+50)//to print the lower mid wall
      	img.at<uchar>(i,j) = 200;
      else     
        img.at<uchar>(i, j) = 0;//to print map except wall

  for (int i=0;i<len;i++)//loop to print snake
  {
    int x=arrx[i],y=arry[i];
    img.at<uchar>(y, x) = 255;//setting snake's pixel to white
  }
}

void incLen()// to increase length
{
  int xd=arrx[len-2]-arrx[len-1];// determining the change in length
  int yd=arry[len-2]-arry[len-1];// determining the change in width
  arrx=(int*)realloc(arrx,(len+leninc)*sizeof(int));
  arry=(int*)realloc(arry,(len+leninc)*sizeof(int));
  if (xd>0)//to increase the length by 1 pixel when snake is moving towards left
  {
    for (int i=len;i<len+leninc;i++)
      {
        arrx[i]=arrx[i-1]-1;
        if (arrx[i]<0) arrx[i]+=800;
      }
    for (int i=len;i<len+leninc;i++)
      arry[i]=arry[i-1];
  }
  if (xd<0)//to increase the length by 1 pixel when snake is moving towards right
  {
    for (int i=len;i<len+leninc;i++)
      {
        arrx[i]=arrx[i-1]+1;
        if (arrx[i]>799) arrx[i]-=800;
      }
    for (int i=len;i<len+leninc;i++)
      arry[i]=arry[i-1];
  }
  if (yd>0)//to increase the length by 1 pixel when snake is moving upwards
  {
    for (int i=len;i<len+leninc;i++)
      {
        arry[i]=arry[i-1]-1;
        if (arry[i]<0) arry[i]+=800;
      }
    for (int i=len;i<len+leninc;i++)
      arrx[i]=arrx[i-1];
  }
  if (yd<0)//to increase the length by 1 pixel when snake is moving downwards
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

void checkFood()// eating food and generaing bonus
{
  int flag=0, k;
  for (int i=fx-(12-level);i<=fx+(12-level);i++)
    for (int j=fy-(12-level);j<=fy+(12-level);j++)
      for(k=0;k<len;k++)
        if (arrx[k]==i && arry[k]==j)// check if snake's and food's coordinate are same
         {flag=1; break;}
  if (flag==1)
    {
      incLen(); score++; //increase length and score
     if (score%4==0) {showbonus=1;bc=1;} fc=1;}// print bonus when score is divisible by 4
}

void gameOver()// to exit game
{
  int lennow=len;
  if (slim>10)
    lennow-=5;
  int x=arrx[0],y=arry[0];
  for (int i=1;i<lennow;i++)
    if (arrx[i]==x && arry[i]==y)//if snakes's head touches any part of its body 
    exitnow();// game terminates
}
void a(int value, void*)
{
    printf("Level set to: %2d\n",value);//to print the level on which game is being played
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

void printBonus() // to print bonus 
{
  if (bc==1)
  {bx=50+rand()%700;by=50+rand()%500;} //genrating random position for bonus 
  if ((bc/10)%2==0)
    for (int i=bx-6;i<=bx+6;i++) // bx: x-cordinate of bonus
    for (int j=by-6;j<=by+6;j++) // by: y-cordinate of bonus
       img.at<uchar>(j, i) =200;// printing bonus 
  bc++;
}

void checkBonus()//to eat bonus
{
  int flag=0, k;
  for (int i=bx-6;i<=bx+6;i++)
    for (int j=by-6;j<=by+6;j++)
      for (k=0;k<len;k++)
      if (arrx[k]==i && arry[k]==j)// if snake's and bonus' coordinte are same
       {flag=1; break;}
  if (flag==1)
    {incLen(); incLen();// increase length by two
      score+=2; // increase score by two
      if (score%4==0) //if score divisible by 4
       {showbonus=1;bc=1;} showbonus=0; bc=0;}
  if (bc>80) {showbonus=0;bc=0;}
}

void itoa(int num, char ch[])// converting integer to alphabets
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
    std::ifstream fin;
    fin.open("snake.txt",std::ios::in);
    fin>>sha_score;
    fin.close();

    /*
     while loop is used to find the old score
     from the hash stored in the file
    */
    int i = 0;
    while(true and !sha_score.empty())
    {
    	sha_temp.str("");
    	sha_temp << i;
    	if(sha_score == sha256(sha_temp.str()))
    		break;  // value found
    	i++;
    }
    scoreold = i; //assign the scoreold
    arrx=(int *)malloc(len*sizeof(int));
    arry=(int *)malloc(len*sizeof(int));
    for (int i=0;i<len;i++)
    {
      arrx[i]=posx-i;
      arry[i]=posy;
    }
    img = cv::Mat::zeros(600,800, CV_8U); // creating the map
    cv::namedWindow("Snakes",1);// creating display window
    cv::createTrackbar( "Level: ", "Snakes", &level, 10,a);
    while(1)
    {
      if ((score%10) == 0 && level !=10 && score!=0 && levelstat==0) {level++;a(level,NULL);levelstat=1;}
      if (score%10==1 || score %10==2) levelstat=0;
      slim++;
      char ch='d';
      ch=cv::waitKey((101-level*10));
      if (ch=='p') score++; // if p is pressed increase score by 1
      if (ch=='\n') exitnow(); // enter key for instant exit
      if (ch==' ') fc=1;// space key for producing food at different location  
      setDir(ch);
      for (int i=1; i<=8+(1.5*level);i++)//loop to decide speed depending on level
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
      cv::Point p; p.x=0; p.y=20;
      putText(img,s,p,0,0.5,255);
      p.x=630; putText(img,sold,p,0,0.5,255);
      cv::imshow("Snakes", img);
    }
}
