#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<fstream>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<vector>
#include<bitset>
#include <iomanip>
#include <sstream>
#include <cstring>

using namespace cv;
using namespace std;
Mat img;
int len=60*4, dir=2, posx=400, posy=300, *arrx, *arry, scoreold=0,
fx,fy,fc=1,leninc=20,score=0,oldvalue=10,slim=0,level=4,count=0,
showbonus=0,bx,by,bc=0,levelstat=0;

String sha256hash="";               //Hash generated from passcode
                                    //Also key for AES to encrypt or decrypt
                                    //high score file

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

/* Implementation of SHA-256
   This block of code simply creates a hash of given password
   Password should be less than 56 characters
   This hash will be used as AES key to encrypt and decrypt the highscore file
   This implementation uses the following C++ libraries extensively
   1)Vectors library - Vectors are essentially dynamic arrays which resize
                       automatically.
                       For further info read:http://www.geeksforgeeks.org/vector-in-cpp-stl/
                       Its used since SHA deals with modifying contents of an array
                       a lotalong with its size.This library saves time and
                       reduces code length
   2)Bitset library -  Essentially makes a boolean array with some optimization
                       to space savings.
                       It helps in visualizing what bits are getting shifted where
                       without the hassle of constant conversions between ascii
                       integer and binary.
                       For further info read:http://www.cplusplus.com/reference/bitset/bitset/
  */

  // defining a function which converts a given block to hexadecimal (only used for final HASH)
  string convert_to_hex(unsigned long input)
  {
    //uses sstream of c++ to make it easier
    bitset<32> bs(input);
    unsigned n = bs.to_ulong();
    stringstream sstream;
    sstream << std::hex << std::setw(8) << std::setfill('0') << n;
    string temp;
    sstream >> temp;
  	return temp;
  }

  //preprocessing starts

  #define ROTRIGHT(word,bits) (((word) >> (bits)) | ((word) << (32-(bits))))
	#define SSIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
	#define SSIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))
	#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
	#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

  //modified operations since official were wrong implementations
  #define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
  #define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))


  //1st phase: convert Given Password into its binary (using its ASCII values)
  vector<unsigned long> String_to_binary(string pass)
  {
    //Vector library funtions used are : .push_back()
    //bitset library functions used are: .to_ulong()
    vector<unsigned long> block;         //final array
	   for (int i = 0; i < pass.size(); ++i)
	    {
		      bitset<8> b(pass.c_str()[i]);     //stores 8BIT pattern of ASCII value
                                            //of charater at i
		      block.push_back(b.to_ulong());    // Add that 8 bit pattern into the block.
	     }
	return block;
  }

  //2nd phase : Padding on block to actually make it a multiple of 512 bits
  vector<unsigned long> pad_to_512bits(vector<unsigned long> block) //uses block generated in above function
  {
    // Variable names as per NIST.

    // l will represent the length of the message in terms of bits, with each
    // block representing one 8 bit ascii character.
  int l = block.size() * 8;
  int k = 447 - l; //simplification of NIST equation to find k

  // First add in another 8 bit block with the first bit set to 1
      unsigned long t1 = 0x80;
      block.push_back(t1);
  // We just added in 7 zero's, so subtract 7 from k.
      k = k - 7;
  // Add 8 bit blocks containing zero's
      for(int i = 0; i < k / 8; i++)
      block.push_back(0x00000000);

  // We are now at 488 bits out of 512. We need to add l in the binary
  // form of eight 8 bit blocks.
    bitset<64> big_64bit_blob(l);

  // Split up that 64 bit blob into 8 bit sections.
    string big_64bit_string = big_64bit_blob.to_string();

  // Push the first block into the 56th position.
    bitset<8> temp_string_holder1(big_64bit_string.substr(0,8));
    block.push_back(temp_string_holder1.to_ulong());

  // Push all the rest of the 8 bit blocks in.
    for(int i = 8; i < 63; i=i+8)
      {
        bitset<8> temp_string_holder2(big_64bit_string.substr(i,8));
        block.push_back(temp_string_holder2.to_ulong());
      }
    return block;
  }
  //phase3 resize block from 64 8 bits to 16 32 bits
  vector<unsigned long> resize_block(vector<unsigned long> input) //uses padded block from above functions
    {
  	   vector<unsigned long> output(16);
       // Loop through the 64 sections by 4 steps and merge those 4 sections.
  	    for(int i = 0; i < 64; i = i + 4)
  	     {
  		       bitset<32> temp(0);
             // Shift the blocks to their assigned spots and OR them with the original
  		       // to combine them.
  		       temp = (unsigned long) input[i] << 24;
  		       temp |= (unsigned long) input[i + 1] << 16;
  		       temp |= (unsigned long) input[i + 2] << 8;
  		       temp |= (unsigned long) input[i + 3];
             // Puts the new 32 bit word into the correct output array location.
  		       output[i/4] = temp.to_ulong();
  	      }
  	       return output;
    }
  //preprocessing ends

//HASHING starts
string compute_hash(vector<unsigned long> block)    //uses resized block from above function
{
  //initializing NIST defined constants using NIST defined variables
  unsigned long k[64] =
  {
	  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
		0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
		0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
		0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
		0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
		0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};
  unsigned long static H0 = 0x6a09e667;
	unsigned long static H1 = 0xbb67ae85;
	unsigned long static H2 = 0x3c6ef372;
	unsigned long static H3 = 0xa54ff53a;
	unsigned long static H4 = 0x510e527f;
	unsigned long static H5 = 0x9b05688c;
	unsigned long static H6 = 0x1f83d9ab;
	unsigned long static H7 = 0x5be0cd19;

  unsigned long temp1;
	unsigned long temp2;
	unsigned long a = H0;
	unsigned long b = H1;
	unsigned long c = H2;
	unsigned long d = H3;
	unsigned long e = H4;
	unsigned long f = H5;
	unsigned long g = H6;
	unsigned long h = H7;

  unsigned long W[64];
  //NIST defined algorithm
  for(int t = 0; t <= 15; t++)
	{
		W[t] = block[t] & 0xFFFFFFFF;
	}
  for(int t = 16; t <= 63; t++)
  {
    W[t] = SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
    W[t] = W[t] & 0xFFFFFFFF;
  }
  for(int t=0;t<64;t++)
  {
    temp1=h+EP1(e)+CH(e,f,g)+k[t]+W[t];
    temp2=EP0(a)+MAJ(a,b,c);
  }
  h = g;
	g = f;
	f = e;
	e = (d + temp1) & 0xFFFFFFFF;     // Makes sure that we are still using 32 bits.
	d = c;
	c = b;
	b = a;
	a = (temp1 + temp2) & 0xFFFFFFFF; // Makes sure that we are still using 32 bits.

  H0 = (H0 + a) & 0xFFFFFFFF;
  H1 = (H1 + b) & 0xFFFFFFFF;
  H2 = (H2 + c) & 0xFFFFFFFF;
  H3 = (H3 + d) & 0xFFFFFFFF;
  H4 = (H4 + e) & 0xFFFFFFFF;
  H5 = (H5 + f) & 0xFFFFFFFF;
  H6 = (H6 + g) & 0xFFFFFFFF;
  H7 = (H7 + h) & 0xFFFFFFFF;

  return convert_to_hex(H0)+convert_to_hex(H1)+convert_to_hex(H2)+
         convert_to_hex(H3)+convert_to_hex(H4)+convert_to_hex(H5)+
         convert_to_hex(H6)+convert_to_hex(H7);

}

void generateHash()
  {
    cout<<"Enter Passcode";
    string pass;
    cin>>pass;

    vector<unsigned long> block;
    block=String_to_binary(pass);
    block=pad_to_512bits(block);
    block=resize_block(block);

    sha256hash=compute_hash(block);
  }

int main()
{
    //cout<<"Enter passcode to play";
    //get input compare with previous sha to encrypt with aes
    if(sha256hash==null)
    generateHash();
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
