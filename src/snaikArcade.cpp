#include<stdlib.h>
#include<conio.h>
#include<iostream>
#include<stdio.h>
#include<windows.h>
#define width 70
#define height 70

using namespace std;

//The global map

char ma[width][height];
int w=35,h=35;
char ch = 'd';//initially the snake's direction
int initialize=0;
int frow;
int fcol;
int score = 0;
int stuck = 0;
int found;//to check if fruit was tasted


//body
struct node{
     char data;
     int rindex;
     int cindex;
     struct node* next;
}*root=NULL;

void append(char a='o',int r=2,int c=2)
{
	//gene created
	struct node* temp;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->data = a;
	temp->rindex = r;
	temp->cindex = c;
	temp->next = NULL;
	//cout<<"Location recievded in node:"<<&(*b);
	if(root==NULL)
	{
		//snake's head is being created;
		root=temp;
		root->data='0';
		
	}
	else
	{
		struct node* ptr;
		ptr=root;
		while(ptr->next!=NULL)
		{
			ptr=ptr->next; //getting to last node
		}
		ptr->next=temp;//conecting the two nodes
		temp->data='o';
		}
}

void doChange(int r,int c)
{
	struct node* ptr;
	int tempr,tempc;
	//a for holding the new location of the head while b is a temporary variable
	//cout<<"In change";
	tempr = root->rindex;
	tempc = root->cindex; //point to address stored at root->idexes
	root->rindex = r;
	root->cindex = c;
	ptr = root->next;
	while(ptr!=NULL)
	{
		r = ptr->rindex;
		c = ptr->cindex;
		
		ptr->rindex = tempr;
		ptr->cindex = tempc;
		
		ptr=ptr->next;
	 } 
	 
	 //to check
	 //approach to void the location of the last node
	 if(root->next!=NULL)
	 ma[r][c]=' ';
	 else
	 ma[tempr][tempc]=' ';
	 //cout<<"Did change";
}

void applyChange()
{
	struct node*ptr;
	ptr=root;
	//cout<<"Applying change";
	while(ptr!=NULL)
	{
		ma[ptr->rindex][ptr->cindex]=ptr->data;
		ptr=ptr->next; //applying the change on the respective nodes
	}
}

void frame(){

for(int i=0;i<width;i++)
{
	for(int j=0;j<height;j++)
	{
		if(i==0)//for the top row
		ma[i][j]='-';
		else if(i==height-1)//for the bottom row
		ma[i][j]='-';
		else if(j==0)//for the left margin
		ma[i][j]='|';
		else if(j==width-1)//for the right margin
		ma[i][j]='|';
		else
		ma[i][j]=' ';
	}
}
}


//displaying the finished frame
void display()
{
	for(int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			cout<<ma[i][j];
		}
	cout<<endl;
	}
	
cout<<"___ SCORE____: "<<score;
}

//Getting the fruit in
void placeFruit(){
	
   int rindex=1+(rand()%(width-2));
   int cindex=1+(rand()%(height-2));
   frow=rindex;
   fcol=cindex;
   ma[rindex][cindex]='@';
   
}

//fruit mechanism

void checkDiet(){
	
	if(frow==w&&fcol==h)
	{
		score+=100;
		found=1;
	}
	
	if(found==1)
	{
		placeFruit();
		append();
		found=0;
	}
}

//to check whether the snake's head is touching it's body or not
void findCor(){
	struct node* ptr;
	ptr=root->next;//as root will always be equal to it
	while(ptr!=NULL)
	{
		if(ptr->rindex==w&&ptr->cindex==h)
		{
			stuck=1;
		}
		
		ptr=ptr->next;
	}
}
checkDeath()
{
	if((w>68||h>68)||(w<2||h<2))
	{
		initialize=-1;
		system("CLS");
		cout<<"\n\n\n\t\t\t\t  ||GAME _ _ OVER ||";
		cout<<"\n\n\n\t\t\t\t___SCORE:"<<score;
		getch();
		exit(1);
	}
	else if(root->next!=NULL)
	{
		struct node* ptr;
		ptr = root->next;
		if(ptr!=NULL)
		{
		
		 findCor();
		 if(stuck)
		 {
		 	initialize=-1;
		   system("CLS");
		   cout<<"\n\n\n\t\t\t\t  ||GAME _ _ OVER ||";
		   cout<<"\n\n\n\t\t\t\t___SCORE:"<<score;
		   getch();
		   exit(1);
		  }
		
	    }
	
	}	
}

//Moving the snake via head
void move(char ch)
{
	switch(ch)
	{
		case 'w':w--;
		         checkDiet();
		         checkDeath();
		         doChange(w,h);
		         applyChange();
		         break;
		case 's':w++;
		         //implement it on ma
		         checkDiet();
		         checkDeath();
				 doChange(w,h);
		         applyChange();
		         break;
		case 'a':h--;
		         checkDiet();
		         checkDeath();
		         doChange(w,h);
		         applyChange();
				 break;
		case 'd':h++;
		         checkDiet();
		         checkDeath();
		         doChange(w,h);
		         applyChange();
				 break;		          
	}
}


void setGame(){
if(initialize==0)
{
	system("CLS");
	frame();
	//appendbody();
	append('0',w,h);
	placeFruit();
	display();
	initialize++;
	}else{
		system("CLS");
	if(_kbhit())
	{
		ch=(_getch());
		switch(ch)
		{
			case 'w':move('w');
			         break;
			case 's':move('s');
			         break;
			case 'a':move('a');
			         break;
			case 'd':move('d');
			         break;
			case 'x':initialize=-1;		 		          
			         break;
		}
	}else
	{
		move(ch);
	}
		display();
	}	
}



int main()
{
	
	while(initialize!=-1)
	{
		setGame();
	    Sleep(500);
	}
	return 0;

}
