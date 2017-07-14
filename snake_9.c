/*
2017.07.02
final !!
Can control up & down & left & right! Add random (OK) !
Add food & eaten food!(OK)
Add random start seat!(OK)
Add speed!(OK)
確認頭部和食物都沒問題!
問題 : 尾巴顯示和移動的問題! (OK)
use 89s51 will happen error : data memory too large! (OK)
grade bug !(OK)
body bug....(body 1 2 OK)
win & lost(OK)
improve the button sensitivity!
author:lypon
*/
#include<AT89X52.H>
#include<stdlib.h>
#include<INTRINS.H>

/////////// function /////////////////

int random(int *seed);                 		  // 取亂數
void initial(unsigned char matrix[]);  		  // 重新初始化陣列
void direction(int *Direction);        		  // 方向
void move(int *Head_start_num,int *Direction);// 移動
void food(void);							  // 產生食物
void food_check(int *Head_start_num);  		  // 確認食物是否重疊到頭部
void match(void);                     	      // 把要顯示的陣列都合起來
void scan(unsigned char k);            		  // 掃描顯示
void delayms(unsigned int time);       		  // 延遲時間

void Direction_matrix_right(void);
void tail_move(void);

///////////////////////////////////////

/////////// display matrix/////////////

unsigned char start_seat[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
unsigned char head_show[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char food_show[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char body_display[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char display[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char win[] = {0x3c,0x42,0x95,0xa1,0xa1,0x95,0x42,0x3c};
unsigned char defeat[] = {0x3c,0x42,0xa5,0x91,0x91,0xa5,0x42,0x3c};
unsigned char go[] = {0x7e,0x81,0x71,0xf6,0x7e,0x81,0x81,0x7e};
unsigned char idata tail[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char idata tail_temp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	  // 尾巴移動前位置
unsigned char idata head_show_temp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // 頭移動前位置
char idata Direction_matrix[64] = {0};

///////////////////////////////////////

//////////// global variable //////////

int set_win_grade = 63;                  // 勝利分數
bit lost = 0;                           // 失敗開關
bit eaten = 0;							// 進食開關
int seed = 3;                           // 亂數種子
int grade = 0;						    // 分數
int grade_temp;							// 分數比較
char food_start = 0x00;					// 食物初始位置
int food_num = 0;						// 食物初始位置


int Direction_num = 1;                  // 尾巴要移動方向的位置
int tail_start_num = 0;					// 尾巴開始位置		
int tail_start_num_temp = 0;			// 尾巴移動前位置數字
int head_start_num_temp = 0;			// 頭移動前位置數字
int speed = 45;  						// 速度
bit start = 0;
///////////////////////////////////////

main()
{
	int Head_start_num = random(&seed)%8; // need find turly random way ! decide start row
	int Direction_key = 1; //default direction down
	int i;
	unsigned char x=0;
	unsigned char t;
	
	P3 = 0x00;                            // P3 不清楚為什麼不能放在前面設定?
	
	///////////////////// 隨機決定起始位置 //////////////////////
	
	while(P3 != 0x80)
	{
		for(t=0;t<15;t++)  // speed
		{
			scan(x);
		}
		seed++;
	}
		seed++;
	
	Head_start_num = random(&seed)%8;
	
	head_show[Head_start_num] = start_seat[Head_start_num];
	
	/////////////////////////////////////////////////////////////
	food();
	
	while(1)
	{
		start = 1;
		initial(display);
		
		if(grade == 0)                   		// 當還沒有身體的時候，要不斷地記錄尾巴的位置(頭移動前的位置等於尾巴的位置)
		{
			tail[Head_start_num] = head_show[Head_start_num];
			tail_start_num = Head_start_num;
		}
			
		
		if(seed > 20000)
			seed = 1;

		food_check(&Head_start_num);
		
		if (grade_temp == grade)  // 如果 grade_temp 等於 grade 就代表現在沒有再進食可以進行有沒有咬到身體的判斷!!
			eaten = 0;
		
		if(grade >= 1)           		      			   // 當分數大於1也就是身體長度大於1的時候，開始記錄頭移動前的位置，
		{					   				   			   // 因為移動前的頭位置和尾巴的位置已經不一樣了!!
			head_show_temp[Head_start_num] = head_show[Head_start_num];
			head_start_num_temp = Head_start_num;
		}
		
		grade_temp = grade;       // 為了保存這次的 grade_temp 所以選擇在判斷完 grade_temp 和 grade是否相等後再刷新!!
		
		move(&Head_start_num,&Direction_key); // snack moving
	
		Direction_matrix[0] = Direction_key;  // 記錄每一次頭移動的方向
		Direction_matrix_right(); 			  // 每移動一次就把方向陣列右移一格
		
		if(grade > 0)
		{
			if(eaten == 1 && grade != grade_temp)
			{
				body_display[tail_start_num] = body_display[tail_start_num] | tail[tail_start_num];  // 進食中，尾巴不向前移動直接變成當前的尾巴
				body_display[head_start_num_temp] = body_display[head_start_num_temp] | head_show_temp[head_start_num_temp]; // 並將移動前的頭加入body_display中
				Direction_num++;			 // 每進食一次就要把尾巴要移動第幾個位置的方向陣列的數字增加1
			}
				
			else if(grade == grade_temp)
			{
				tail_temp[tail_start_num] = tail[tail_start_num];    // 儲存尾巴上一個位置
				tail_start_num_temp = tail_start_num;
				
				tail_move();				// 尾巴移動
				
				body_display[head_start_num_temp] = body_display[head_start_num_temp] | head_show_temp[head_start_num_temp]; // 將移動前的頭加入body_display中
				body_display[tail_start_num] =  body_display[tail_start_num] | tail[tail_start_num] ; 						 // 將移動後的尾巴加入body_display中
				body_display[tail_start_num_temp] = body_display[tail_start_num_temp] ^ tail_temp[tail_start_num_temp];      // 把移動前的尾巴消除
			}
		}

		
		if(eaten == 0)
			for(i=0;i<8;i++)                  // decition lost !!
				if (head_show[Head_start_num] == (body_display[i] & head_show[Head_start_num]) && Head_start_num == i)
				lost = 1;
		
		match();

		
		if(grade >= set_win_grade)
			grade = set_win_grade;
					
		for(t=0;t<speed;t++)  // speed
		{
			direction(&Direction_key); //decide direction
			scan(x);
		}
		
		seed++;
	}

	return 0;	
	
}

///////////////////// random /////////////////

int random(int *seed)
{
	srand(*seed);
	return rand();
}

//////////////////////////////////////////////

///////////////////// initial ////////////////

void initial(unsigned char matrix[])
{
	int i;
	char temp = 0x00;
	for(i=0;i<8;i++)
	{
		matrix[i] = temp;
	}
}

///////////////////////////////////////////////

//////////////////// direction ////////////////

void direction(int *Direction)
{	
	switch(P3)
	{
		case 0x00:
		{
			*Direction = *Direction;
			break; //default
		}
		case 0x01:
		{
			if(*Direction == 1)
				break;
			*Direction = 0; // up
			break;
		}
		case 0x02:
		{
			if(*Direction == 0)
				break;
			*Direction = 1; // down
			break;
		}
		case 0x04:
		{
			if(*Direction == 3)
				break;
			*Direction = 2; // left
			break;
		}
		case 0x08:
		{
			if(*Direction == 2)
				break;
			*Direction = 3; // right
			break;
		}
			
	}
}

///////////////////////////////////////////////

//////////////////// move /////////////////////

void move(int *Head_start_num,int *Direction)
{
	int i = 0;
	int temp = 0x00;
	int Head_start_num_right = (*Head_start_num+1)%8;
	int Head_start_num_left = (*Head_start_num-1)%8;

	if (Head_start_num_left == -1)
		Head_start_num_left = 7;
	if (Head_start_num_right == 8)
		Head_start_num_right = 0;
		
	if (*Direction == 0)
	{
		head_show[*Head_start_num] = _cror_(head_show[*Head_start_num],1); // move 1 up
	}
	
	if (*Direction == 1)
	{
		head_show[*Head_start_num] = _crol_(head_show[*Head_start_num],1); // move 1 down
	}
	
	if (*Direction == 2)
	{
		head_show[Head_start_num_right] = head_show[*Head_start_num]; // move 1 right
		head_show[*Head_start_num] = temp;
		*Head_start_num = Head_start_num_right;
	}
	
	if (*Direction == 3)
	{
		head_show[Head_start_num_left] = head_show[*Head_start_num]; //move 1 left
		head_show[*Head_start_num] = temp;
		*Head_start_num = Head_start_num_left;
	}
	
	if (food_start == head_show[*Head_start_num] && food_num == *Head_start_num )
	{
		grade = grade + 1;
		eaten = 1;
		speed--;
		if(speed < 5)
			speed = 5;
	}
}

///////////////////////////////////////////////

//////////////////// food /////////////////////

void food(void)
{
	initial(food_show);
	seed++;
	food_start = start_seat[random(&seed)%8];
	seed++;
	food_num = random(&seed)%8;
}

///////////////////////////////////////////////

//////////////////// food_check ///////////////

void food_check(int *Head_start_num)
{
	if (food_start == head_show[*Head_start_num] && food_num == *Head_start_num ) // 判斷食物是否與頭重疊
	{
		food();
		food_check(Head_start_num);
	}
	else
		food_show[food_num] = food_start;
		
}

///////////////////////////////////////////////

//////////////////// match ////////////////////

void match(void)
{
	int i;
	for(i=0;i<8;i++)
	{
		display[i] = food_show[i] | head_show[i] | body_display[i] ;
	}

}

///////////////////////////////////////////////

///////////////////// scan ////////////////////

void scan(unsigned char k)
{
	unsigned char m;
	unsigned char com;
	com = 0xfe;
	for(m=0;m<8;m++)
	{
		if (grade >= set_win_grade)
			P1 = win[k];
		else if (lost == 1)
			P1 = defeat[k];
		else if (start == 0)
			P1 = go[k];
		else
			P1 = display[k];
		P2 = com;
		delayms(2);
		P2 = 0xff;
		com = _crol_(com,1);
		k++;
	}
}

////////////////////////////////////////////////

///////////////////// delayms //////////////////

void delayms(unsigned int time)
{
	unsigned int n;
	while(time>0)
	{
		n=120;
		while(n>0) n--;
		time--;
	}
}

/////////////////////////////////////////////////

//////////// Direction_matrix_right ////////////////

void Direction_matrix_right(void)
{
	int i;

	for(i=63;i>0;i--)
	{
		Direction_matrix[i] = Direction_matrix[i-1];
	}
}

/////////////////////////////////////////////////

////////////////// tail_move /////////////////////////

void tail_move(void)
{
	int i = 0;
	int temp = 0x00;
	int tail_start_num_right = (tail_start_num+1)%8;
	int tail_start_num_left = (tail_start_num-1)%8;

	if (tail_start_num_left == -1)
		tail_start_num_left = 7;
	if (tail_start_num_right == 8)
		tail_start_num_right = 0;
		
	if (Direction_matrix[Direction_num] == 0)
	{
		tail[tail_start_num] = _cror_(tail[tail_start_num],1); // move 1 up
	}
	
	if (Direction_matrix[Direction_num] == 1)
	{
		tail[tail_start_num] = _crol_(tail[tail_start_num],1); // move 1 down
	}
	
	if (Direction_matrix[Direction_num] == 2)
	{
		tail[tail_start_num_right] = tail[tail_start_num]; // move 1 right
		tail[tail_start_num] = temp;
		tail_start_num = tail_start_num_right;
	}
	
	if (Direction_matrix[Direction_num] == 3)
	{
		tail[tail_start_num_left] = tail[tail_start_num]; //move 1 left
		tail[tail_start_num] = temp;
		tail_start_num = tail_start_num_left;
	}
	
}

/////////////////////////////////////////////////////




