//俄罗斯方块
#include <iostream>
#include <graphics.h>
#include <random>
#include <time.h>
#define SPEED 1 //方块速度,数值越大速度越快
#define WIDTH 600
#define HEIGHT 698
#define BL 60 //方块边长
#define Gap 3 //方块间的间隙
#define SCOREFIELD 200
#define XBN (WIDTH - Gap - SCOREFIELD)/(BL + Gap) //x方向上的方块数量
#define YBN (HEIGHT - Gap) / (BL + Gap) //y方向上的方块数量
#define XMAPO 2 //映射原点的x坐标
#define YMAPO 3 //映射原点的y坐标

clock_t startclock;
clock_t loopstart;

enum 
{
	I,J,L,O,S,T,Z
};

struct direct
{
	int xoffset;
	int yoffset;
}Direct;

const direct DOWN = {
	0,1
};

int Panel[YBN][XBN];

struct Block
{
	int xoffset;
	int yoffset;
};

Block blockMap[4][4];

struct Shape
{
	Block blocks[4];//存储方块位置信息
	int numbers;//记录方块数量
	int spawnXoffset;//图形生成点
	int spawnYoffset;
};

Shape Shapes[7];

struct CShape
{
	int x;
	int y;
	Shape shape;
}CurrentShape;

void setKey();//置空按键消息
void getKey();//获取按键消息
void drawBlankBlock(const int x, const int y);
void drawBlock(const int x, const int y);
void initPanel();//初始化画板
void initShape();//初始化形状
void initMap();//初始化映射
void getShape();//获取新的形状
void drawShape(const CShape &);//绘制形状
bool canMove(const CShape &,direct d);//判断是否能移动
bool needFix(const CShape&);//判断形状是否需要固定
void saveBlocks(const CShape &);//保存方块
void moveBlock(CShape& cshape, direct d);//移动形状
bool canRotate(CShape& cshape);//判断能否旋转
void rotateShape(CShape& cshape);//旋转形状
void clearShape(const CShape& cshape);//清除方块
bool needClear(const int y);//判断一行是否需要清除
void clearRow(const int y);//清楚一行
bool endGame(const CShape&);//判断游戏是否结束
void mainLoop();//游戏主循环

int main()
{
	initPanel();
	initShape();
	initMap();
	mainLoop();
	getchar();
}

void setKey()
{
	Direct.xoffset = 0;
	Direct.yoffset = 0;
}

void getKey()
{
	int x;
	ExMessage Message;
	peekmessage(&Message, EX_KEY);
	x = Message.vkcode;
	switch (x)
	{
	case VK_UP:
		if (Message.prevdown)
		{
			std::cout << "Release or Long Press" << std::endl;
			if (canRotate(CurrentShape)) {
				clearShape(CurrentShape);
				rotateShape(CurrentShape);
				drawShape(CurrentShape);
				std::cout << "Can rotate.==============================" << std::endl;
			}

		}
		break;
	case VK_DOWN:
		break;
	case VK_LEFT:
		if (Message.prevdown) 
		{
			std::cout << "Release or Long Press" << std::endl;
			Direct.xoffset = -1;
			Direct.yoffset = 0;
		}
		
		break;
	case VK_RIGHT:
		if (Message.prevdown)
		{
			std::cout << "Release or Long Press" << std::endl;
			Direct.xoffset = 1;
			Direct.yoffset = 0;
		}
		break;
	default:
		break;
	}
}

void drawBlankBlock(const int x, const int y)
{
	if (x >= 0 && x < XBN && y >= 0 && y < YBN) {
		setfillcolor(BLACK);
		fillrectangle(Gap + (BL + Gap) * x, Gap + (BL + Gap) * y, Gap + BL + (BL + Gap) * x, Gap + BL + (BL + Gap) * y);
		setfillcolor(WHITE);
		rectangle(Gap + (BL + Gap) * x, Gap + (BL + Gap) * y, Gap + BL + (BL + Gap) * x, Gap + BL + (BL + Gap) * y);
	}
}

void drawBlock(const int x, const int y)
{
	if (x >= 0 && x < XBN && y >= 0 && y < YBN)
	fillrectangle(Gap + (BL + Gap) * x, Gap + (BL + Gap) * y, Gap + BL + (BL + Gap) * x, Gap + BL + (BL + Gap) * y);
}

void initPanel()
{
	startclock = clock();
	initgraph(WIDTH, HEIGHT);
	rectangle(0, 0, XBN * (BL + Gap) + Gap, YBN * (BL + Gap) + Gap);
	for (int y = 0; y < YBN; y++)
	{
		for (int x = 0; x < XBN; x++)
		{
			drawBlankBlock(x, y);
		}
	}
}

void initShape()
{
	Shapes[I].blocks[0] = { 0,0 };
	Shapes[I].blocks[1] = { 0,-1 };
	Shapes[I].blocks[2] = { 0,-2 };
	Shapes[I].blocks[3] = { 0,-3 };
	Shapes[I].numbers = 4;
	Shapes[I].spawnXoffset = 0;
	Shapes[I].spawnYoffset = 0;

	Shapes[J].blocks[0] = { 0,0 };
	Shapes[J].blocks[1] = { -1,0 };
	Shapes[J].blocks[2] = { 0,-1 };
	Shapes[J].blocks[3] = { 0,-2 };
	Shapes[J].numbers = 4;
	Shapes[J].spawnXoffset = 0;
	Shapes[J].spawnYoffset = 0;

	Shapes[L].blocks[0] = { -1,0 };
	Shapes[L].blocks[1] = { 0,0 };
	Shapes[L].blocks[2] = { -1,-1 };
	Shapes[L].blocks[3] = { -1,-2 };
	Shapes[L].numbers = 4;
	Shapes[L].spawnXoffset = -1;
	Shapes[L].spawnXoffset = 0;

	Shapes[O].blocks[0] = { 0,-1 };
	Shapes[O].blocks[1] = { -1,-1 };
	Shapes[O].blocks[2] = { 0,-2 };
	Shapes[O].blocks[3] = { -1,-2 };
	Shapes[O].numbers = 4;
	Shapes[O].spawnXoffset = 0;
	Shapes[O].spawnYoffset = 1;

	Shapes[S].blocks[0] = { -1,-1 };
	Shapes[S].blocks[1] = { -2,-1 };
	Shapes[S].blocks[2] = { -1,-2 };
	Shapes[S].blocks[3] = { 0,-2 };
	Shapes[S].numbers = 4;
	Shapes[S].spawnXoffset = 1;
	Shapes[S].spawnYoffset = 1;

	Shapes[T].blocks[0] = { 0,-1 };
	Shapes[T].blocks[1] = { -1,-1 };
	Shapes[T].blocks[2] = { 1,-1 };
	Shapes[T].blocks[3] = { 0,-2 };
	Shapes[T].numbers = 4;
	Shapes[T].spawnXoffset = 0;
	Shapes[T].spawnYoffset = 1;

	Shapes[Z].blocks[0] = { -1,-1 };
	Shapes[Z].blocks[1] = { 0,-1 };
	Shapes[Z].blocks[2] = { -1,-2 };
	Shapes[Z].blocks[3] = { -2,-2 };
	Shapes[Z].numbers = 4;
	Shapes[Z].spawnXoffset = 1;
	Shapes[Z].spawnYoffset = 1;
	
	srand((unsigned int)time(NULL));
	CurrentShape.shape = Shapes[rand()%7];
	CurrentShape.x = XBN / 2 + CurrentShape.shape.spawnXoffset;
	CurrentShape.y = -1 + CurrentShape.shape.spawnYoffset;
	std::cout << "X : " << CurrentShape.x << " y : " << CurrentShape.y << std::endl;
}

void initMap()
{
	Block tempBlockMap[4][4];
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			blockMap[y][x].xoffset = x - XMAPO;
			blockMap[y][x].yoffset = y - YMAPO;
		}
	}
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			tempBlockMap[y][x] = blockMap[x][3-y];
		}
	}
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			blockMap[y][x] = tempBlockMap[y][x];
		}
	}
}

void getShape()
{
	CurrentShape.shape = Shapes[rand() % 7];
	CurrentShape.x = XBN / 2 + CurrentShape.shape.spawnXoffset;
	CurrentShape.y = -1 + CurrentShape.shape.spawnYoffset;
}

void drawShape(const CShape &cshape)
{
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		drawBlock(cshape.x + cshape.shape.blocks[i].xoffset, cshape.y + cshape.shape.blocks[i].yoffset);
	}
}

bool canMove(const CShape &cshape,direct d)
{
	int cx, cy;
	if (d.xoffset == 0 && d.yoffset == 0)
		return false;
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		cx = cshape.x + cshape.shape.blocks[i].xoffset + d.xoffset;
		cy = cshape.y + cshape.shape.blocks[i].yoffset + d.yoffset;
		if(cx >= 0 && cx <XBN && cy < YBN)
		{
			if (cy >= 0) {
				if (Panel[cy][cx] == 1) {
					return false;
				}
			}
			
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool needFix(const CShape& cshape)
{
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		std::cout << "YBN = " << YBN << " #YOFF :" << cshape.y + cshape.shape.blocks[i].yoffset +1 << std::endl;
		if (Panel[cshape.y + cshape.shape.blocks[i].yoffset + 1][cshape.x + cshape.shape.blocks[i].xoffset] == 1 || cshape.y + cshape.shape.blocks[i].yoffset + 1 > YBN-1)
		{
			std::cout << "Need Fix!!!!" << std::endl;
			return true;
		}
	}
	return false;
}

void saveBlocks(const CShape &cshape)
{
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		Panel[cshape.y + cshape.shape.blocks[i].yoffset][cshape.x + cshape.shape.blocks[i].xoffset] = 1;
	}
}

void moveBlock(CShape& cshape, direct d)
{
	cshape.x += d.xoffset;
	cshape.y += d.yoffset;
}

bool canRotate(CShape& cshape)
{
	int cx, cy;
	CShape tempShape;
	tempShape = cshape;
	rotateShape(tempShape);
	for (int i = 0; i < tempShape.shape.numbers; i++)
	{
		cx = tempShape.x + tempShape.shape.blocks[i].xoffset;
		cy = tempShape.y + tempShape.shape.blocks[i].yoffset;
		if (cx >= 0 && cx < XBN && cy < YBN)
		{
			if (cy >= 0) {
				if (Panel[cy][cx] == 1) {
					return false;
				}
			}

		}
		else
		{
			return false;
		}
	}
	return true;
}

void rotateShape(CShape& cshape)
{
	for(int i = 0; i < cshape.shape.numbers; i++)
	cshape.shape.blocks[i] = blockMap[YMAPO+cshape.shape.blocks[i].yoffset][XMAPO+cshape.shape.blocks[i].xoffset];
}

void clearShape(const CShape& cshape)
{
	std::cout << "Clear" << std::endl;
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		drawBlankBlock(cshape.x + cshape.shape.blocks[i].xoffset, cshape.y + cshape.shape.blocks[i].yoffset);
	}
}

bool needClear(const int y)
{
	for (int x = 0; x < XBN; x++)
	{
		if (Panel[y][x] == 0)
		{
			return false;
		}
	}
	return true;
}

void clearRow(int y)
{
	for (int x = 0; x < XBN; x++)
	{
		Panel[y][x] = 0;
	}
	for (; y >= 0; y--)
	{
		for (int x = 0; x < XBN; x++)
		{
			Panel[y][x] = Panel[y - 1][x];
		}
	}
	for (int y = 0; y < YBN; y++)//清空屏幕
	{
		for (int x = 0; x < XBN; x++)
		{
			drawBlankBlock(x, y);
		}
	}
	for (int y = 0; y < YBN; y++)//重绘
	{
		for (int x = 0; x < XBN; x++)
		{
			if (Panel[y][x] == 1)
			{
				drawBlock(x, y);
			}
		}
	}
}

bool endGame(const CShape& cshape)
{
	for (int i = 0; i < cshape.shape.numbers; i++)
	{
		if (cshape.y + cshape.shape.blocks[i].yoffset < 0)
		{
			std::cout << "GameEnd" << std::endl;
			return true;
		}
	}
	return false;
}

void mainLoop()
{
	bool endgame = 0;
	while (!endgame) {
		if (clock() - startclock > CLOCKS_PER_SEC/SPEED)
		{
			startclock = clock();
			if (needFix(CurrentShape))
			{
				endgame = endGame(CurrentShape);
				saveBlocks(CurrentShape);
				if (!endgame)
				{
					getShape();
					std::cout << "New Shape" << std::endl;
				}
				for (int y = 0; y < YBN; y++)
				{
					if (needClear(y))
						clearRow(y);
				}
			}
			std::cout << clock() - startclock << std::endl;
			if (canMove(CurrentShape, DOWN)) {
				std::cout << "Can move down"<< std::endl;
				clearShape(CurrentShape);
				moveBlock(CurrentShape, DOWN);
				drawShape(CurrentShape);
			}
		}
		setKey();
		getKey();
		if (canMove(CurrentShape, Direct))
		{
			clearShape(CurrentShape);
			moveBlock(CurrentShape, Direct);
			drawShape(CurrentShape);
		}
	}
}
