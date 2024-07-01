// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <ncurses.h>

typedef struct {
	int x;
	int y;
} pos;

#define snake_speed 1
#define max_snake_length 30
#define max_height 12
#define max_width 16
#define num_apple 2
#define num_snake 1
#define num_air 0
#define stop 0
#define left 1
#define up 2
#define right 3
#define down 4
#define direction_num 4
#define half_direction_num 2
#define board_num ((max_height) * (max_width))
#define apple_init_x 5
#define apple_init_y 3
#define apple_move_x 7
#define apple_move_y 8
#define delay 1000

#define move_direction \
	(*(volatile int *)(0x000)) //直接将方向值映射到内存中的0x000位置
#define v_sync_finished (*(volatile int *)(0x004))
//board以0x008为起点 , board[1]-board[192] , *(board[1])=0x00c , *(board[trans(x,y)])=0x008+((x-1)*16+y)*4
#define board ((volatile int *)(0x008))

//用于蛇数据结构
#define sub1_mod(x)                                               \
	((((x) - 1 + max_snake_length) % max_snake_length) == 0 ? \
		 max_snake_length :                               \
		 (((x) - 1 + max_snake_length) % max_snake_length))
#define add1_mod(x)                                               \
	((((x) + 1) % max_snake_length) == 0 ? max_snake_length : \
					       ((x) + 1) % max_snake_length)
//判断是否直接反向导致死亡
#define is_inverse(direction1, direction2)                                \
	((((direction1) + half_direction_num) % direction_num == 0 ?      \
		  direction_num :                                         \
		  ((direction1) + half_direction_num) % direction_num) == \
			 direction2 ?                                     \
		 1 :                                                      \
		 0)

//用于棋盘数据结构
#define add_mod_x(x, y)                                 \
	((((x) + (y)) % max_height) == 0 ? max_height : \
					   (((x) + (y)) % max_height))
#define add_mod_y(x, y) \
	((((x) + (y)) % max_width) == 0 ? max_width : (((x) + (y)) % max_width))
#define trans(x, y) (((x) - 1) * (max_width) + (y)) //坐标转化为一维数组里的位置

// void printboard(int* board,pos* snake,int snake_length,int head,int tail,int apple_x,int apple_y){
//     for (int i=1; i<=max_height; i++) {
//         for (int j=1; j<=max_width; j++) {
//             mvprintw(i,j,"%d ",board[trans(i, j)]);
//         }
//     }
//     refresh();
//     return;
// }

int main()
{
	// 初始化IO库
	// initscr();  // 初始化ncurses库
	// cbreak();   // 立即捕获输入字符，不需要回车
	// nodelay(stdscr, TRUE); // 设置getch为非阻塞模式

	// int move_direction;
	// int* board = (int*)malloc(sizeof(int)*(board_num+1));
	pos snake[max_snake_length + 1];

	int head;
	int tail;
	int snake_length;

	int apple_x;
	int apple_y;
	int apple_eaten;
	int snake_dead;

	int dis[5][2];

	dis[0][0] = 0;
	dis[0][1] = 0;
	dis[1][0] = 0;
	dis[1][1] = -1;
	dis[2][0] = -1;
	dis[2][1] = 0;
	dis[3][0] = 0;
	dis[3][1] = 1;
	dis[4][0] = 1;
	dis[4][1] = 0;

	while (1) {
		//归零初始化
		for (int i = 1; i <= max_snake_length; i++) {
			snake[i].x = 0;
			snake[i].y = 0;
		}

		//游戏初始化
		apple_x = apple_init_x;
		apple_y = apple_init_y;
		apple_eaten = 0;
		snake_dead = 0;

		move_direction = stop;

		//蛇初始化
		head = 1;
		tail = 1;
		snake_length = 1;
		snake[head].x = max_height / 2;
		snake[head].y = max_width / 2;

		//屏幕初始化
		for (int i = 1; i <= board_num; i++) {
			board[i] = num_air;
		};
		board[trans(apple_x, apple_y)] = num_apple;
		board[trans(snake[head].x, snake[head].y)] = num_snake;

		v_sync_finished = 0;

		//初始状态打印
		// printboard(board,snake,snake_length,head,tail,apple_x,apple_y);

		while (1) {
			//PC输入
			// int ch = getch();
			// if(ch != ERR){
			//     switch (ch){
			//         case 'a': move_direction = 1; break;  // 上
			//         case 'w': move_direction = 2; break;  // 右
			//         case 'd': move_direction = 3; break;  // 下
			//         case 's': move_direction = 4; break;  // 左
			//     }
			// }
			if (v_sync_finished == 1) {
				int newx =
					snake[head].x + dis[move_direction][0];
				int newy =
					snake[head].y + dis[move_direction][1];
				head = sub1_mod(head);
				snake[head].x = newx;
				snake[head].y = newy;

				board[trans(snake[tail].x, snake[tail].y)] =
					num_air;
				snake[tail].x = 0;
				snake[tail].y = 0;
				tail = sub1_mod(tail);

				int nowx = snake[head].x;
				int nowy = snake[head].y;

				//检查是否咬死
				if (snake_length > 4) {
					for (int i = add1_mod(head);
					     i != add1_mod(tail);
					     i = add1_mod(i)) {
						if (nowx == snake[i].x &&
						    nowy == snake[i].y) {
							snake_dead = 1;
							break;
						}
					}
				}
				if (snake_dead) {
					break;
				}

				//检查是否出界
				if (nowx < 1 || nowx > max_height || nowy < 1 ||
				    nowy > max_width) {
					break;
				}

				//检查是否吃到苹果
				if (nowx == apple_x && nowy == apple_y) {
					apple_eaten = 1;
					snake_length++;
					if (snake_length == max_snake_length) {
						break;
					} else {
						int offset_x;
						int offset_y;
						if (snake_length == 2) {
							offset_x =
								0 -
								dis[move_direction]
								   [0];
							offset_y =
								0 -
								dis[move_direction]
								   [1];
						} else {
							offset_x =
								snake[tail].x -
								snake[sub1_mod(
									      tail)]
									.x;
							offset_y =
								snake[tail].y -
								snake[sub1_mod(
									      tail)]
									.y;
						}
						tail = add1_mod(tail);
						snake[tail].x = add_mod_x(
							snake[sub1_mod(tail)].x,
							offset_x);
						snake[tail].y = add_mod_y(
							snake[sub1_mod(tail)].y,
							offset_y);
					}
				}

				//更新屏幕
				if (apple_eaten == 1) {
					board[trans(snake[tail].x,
						    snake[tail].y)] = num_snake;
				}
				board[trans(snake[head].x, snake[head].y)] =
					num_snake;

				//更新苹果
				if (apple_eaten == 1) {
					apple_x = add_mod_x(apple_x,
							    apple_move_x);
					apple_y = add_mod_y(apple_y,
							    apple_move_y);
					while (board[trans(apple_x, apple_y)] ==
					       num_snake) {
						apple_x = add_mod_x(
							apple_x, apple_move_x);
						apple_y = add_mod_y(
							apple_y, apple_move_y);
					}

					board[trans(apple_x, apple_y)] =
						num_apple;
					apple_eaten = 0;
				}
				v_sync_finished = 0;
			}
			// printboard(board,snake,snake_length,head,tail,apple_x,apple_y);
			// usleep(400000); //sleep 1s
		}
	}
};