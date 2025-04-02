#include<iostream>  // 包含输入输出流库
#include<cstdlib>   // 包含标准库函数，如rand()、malloc()等
#include <time.h>   // 包含时间相关函数，用于随机数生成
#include<math.h>    // 包含数学函数，如sqrt()、log()等
int maxsearch=9999; // 定义最大搜索次数为9999次
int c=2;            // UCT算法中的探索参数，控制探索与利用的平衡
using namespace std; // 使用标准命名空间

// 棋盘类定义
class chessboard{
	private:
		int board[9][9];  // 9x9的棋盘数组，0表示空，1表示玩家1的棋子，2表示玩家2的棋子
		int turn;	      // 当前轮到哪个玩家，1或2
		bool visited[9][9]; // 用于记录搜索时已访问的位置
	public:
		chessboard();     // 构造函数
		bool gameover();  // 判断游戏是否结束
		bool drop(int x,int y); // 在位置(x,y)落子
		void printboard(); // 打印棋盘状态
		bool hasair(int x,int y,int val); // 检查位置(x,y)的棋子是否有气（围棋术语）
		void refresh();    // 重置visited数组
		int getturn(){     // 获取当前轮到的玩家
			return turn;
		}
};

// 棋盘类构造函数，初始化棋盘为空
chessboard::chessboard(){
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			board[i][j]=0;  // 初始化棋盘所有位置为0（空）
		}
	}
	turn=1;  // 初始化为玩家1先行
}

// 重置visited数组，用于hasair函数的递归调用前
void chessboard::refresh(){
	for(int x=0;x<9;x++){
		for(int y=0;y<9;y++){
			visited[x][y]=false;  // 将所有位置标记为未访问
		}
	}
}

// 检查位置(x,y)的棋子是否有气（围棋规则）
bool chessboard::hasair(int x,int y,int val){
	if(x==-1||x==9||y==-1||y==9){  // 如果坐标超出棋盘范围
		return false;
	}
	if(visited[x][y]){  // 如果该位置已被访问过
		return false;
	}
	if(board[x][y]==0){  // 如果该位置为空，则有气
		return true;
	}
	if(board[x][y]!=val){  // 如果该位置的棋子不是当前检查的颜色
		return false;
	}
	visited[x][y]=true;  // 标记该位置已访问
	// 递归检查上下左右四个相邻位置，有一个有气就返回true
	if(hasair(x-1,y,val)||hasair(x+1,y,val)||hasair(x,y+1,val)||hasair(x,y-1,val)){
		return true;
	}
	return false;  // 所有相邻位置都没有气
}

// 判断游戏是否结束
bool chessboard::gameover(){
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			refresh();  // 重置visited数组
			if(!hasair(i,j,board[i][j])){  // 如果某个位置的棋子没有气了
				turn=3-turn;  // 切换到另一个玩家（胜利者）
				return true;  // 游戏结束
			}
		}
	}
	return false;  // 游戏未结束
}

// 在位置(x,y)落子
bool chessboard::drop(int x,int y){
	if(board[x][y]==0&&x>=0&&x<=8&&y>=0&&y<=8){  // 如果位置为空且在棋盘范围内
		board[x][y]=turn;  // 放置当前玩家的棋子
		turn=3-turn;       // 切换到另一个玩家（1变2，2变1）
		return true;       // 落子成功
	}
	else{
		return false;      // 落子失败
	}
}

// 打印棋盘状态
void chessboard::printboard(){
	cout<<"turn:"<<turn<<endl;  // 打印当前轮到的玩家
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			cout<<board[i][j]<<" ";  // 打印棋盘上的每个位置
		}
		cout<<endl;  // 每行结束后换行
	}
}

// 随机落子，并模拟到游戏结束，返回游戏结果
int randomdrop(chessboard c,int t){
	int x,y;
	srand((unsigned)time(NULL));  // 设置随机数种子
	x = rand() % 9;  // 随机生成x坐标（0-8）
	y = rand() % 9;  // 随机生成y坐标（0-8）
	while(!c.drop(x,y)){  // 如果当前位置不能落子，重新生成位置
		x = rand() % 9;
		y = rand() % 9;
	}
	if(c.gameover()){  // 如果游戏结束
		if(c.getturn()==t){  // 如果当前玩家输了
			return 0;  // 返回0表示失败
		}
		return 1;  // 返回1表示胜利
	}
	return randomdrop(c,t);  // 递归直到游戏结束
}

// 定义蒙特卡洛树节点结构
typedef struct treenode{
	chessboard c;  // 当前棋盘状态
	double wingames;  // 从该节点出发获胜的次数
	double allgames;  // 从该节点出发模拟的总次数
	int newx,newy;    // 到达该节点的最后一步棋的坐标
	struct treenode *parent,*firstchild,*nextbrother;  // 父节点、第一个子节点、下一个兄弟节点
}treenode,*tree;

// 计算UCT值（Upper Confidence Bound for Trees）
double caluct(tree t){
	if(t->allgames==0){  // 如果未曾模拟过该节点
		return 9999;     // 返回一个很大的值，确保未探索的节点会被选中
	}
	double res;
	tree a;
	for(a=t;a->parent;a=a->parent){  // 找到根节点
	}
	// UCT公式：胜率 + c * sqrt(ln(总模拟次数) / 当前节点模拟次数)
	res=t->wingames/t->allgames+sqrt(c*log(a->allgames)/t->allgames);
	//res=t->wingames/t->allgames+sqrt(c*log(t->parent->allgames)/t->allgames);
	return res;
}

// 为节点生成所有可能的子节点
void generatechild(tree &t){
	chessboard cb=t->c;  // 复制当前棋盘状态
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			if(cb.drop(i,j)){  // 尝试在位置(i,j)落子
				if(!cb.gameover()){  // 如果游戏未结束
					treenode *tn=(treenode*)malloc(sizeof(treenode));  // 创建新节点
					tn->c=cb;         // 设置新节点的棋盘状态
					tn->wingames=0;   // 初始化胜利次数为0
					tn->allgames=0;   // 初始化总模拟次数为0
					tn->newx=i;       // 记录落子的x坐标
					tn->newy=j;       // 记录落子的y坐标
					tn->parent=t;     // 设置父节点
					tn->firstchild=NULL;  // 初始化第一个子节点为空
					tn->nextbrother=t->firstchild;  // 插入到子节点链表的头部
					t->firstchild=tn;  // 更新父节点的第一个子节点
				}
			}
			cb=t->c;  // 重置棋盘状态，准备下一次尝试
		}
	}
}

// 根据模拟结果更新树中节点的信息
void traceback(tree &t,int i){
	for(tree temp=t;temp;temp=temp->parent){  // 从当前节点一直回溯到根节点
		temp->allgames++;  // 增加总模拟次数
		temp->wingames+=i;  // 增加胜利次数（如果i=1）
		i=1-i;  // 切换结果（在树中相邻层的玩家是对手关系）
	}
}

// 找到UCT值最高的子节点
treenode* findchild(treenode *t){
	treenode* res=t->firstchild;  // 默认选择第一个子节点
	for(treenode* temp=t->firstchild;temp;temp=temp->nextbrother){  // 遍历所有子节点
		if(caluct(temp)>caluct(res)){  // 如果找到UCT值更高的节点
			res=temp;  // 更新结果
		}
	}
	return res;  // 返回UCT值最高的子节点
}

// 蒙特卡洛树搜索的核心函数
void search(treenode *t){
	if(t->allgames==0){  // 如果是新节点
		int i=randomdrop(t->c,t->c.getturn());  // 随机模拟到游戏结束
		traceback(t,i);  // 更新路径上的节点信息
	}
	else{
		if(!t->firstchild){  // 如果没有子节点
			generatechild(t);  // 生成所有可能的子节点
			
		}
		if(t->firstchild){  // 如果有子节点
			search(findchild(t));  // 递归搜索UCT值最高的子节点
		}
		
	}
}

// 蒙特卡洛算法的主函数，用于决定下一步走法
bool montecarlo(chessboard cb,int &x,int &y){
	treenode *t=new treenode;  // 创建根节点
	t->c=cb;         // 设置根节点的棋盘状态
	t->wingames=0;   // 初始化胜利次数为0
	t->allgames=0;   // 初始化总模拟次数为0
	t->newx=0;       // 初始化坐标为0
	t->newy=0;       // 初始化坐标为0
	t->parent=NULL;  // 根节点没有父节点
	t->firstchild=NULL;  // 初始化第一个子节点为空
	t->nextbrother=NULL;  // 初始化下一个兄弟节点为空
	for(int i=0;i<maxsearch;i++){  // 执行maxsearch次搜索
		search(t);  // 调用搜索函数
	}
	if(t->firstchild){  // 如果有可行的走法
		x=findchild(t)->newx;  // 选择UCT值最高的子节点的x坐标
		y=findchild(t)->newy;  // 选择UCT值最高的子节点的y坐标
		return true;  // 返回true表示找到了走法
	}
	return false;  // 返回false表示没有可行的走法
}

// 主函数
int main(){
	int x,y;  // 用于存储走法的坐标
	char entry;  // 未使用的变量
	chessboard c=chessboard();  // 创建新棋盘
	while(!c.gameover()){  // 当游戏未结束时循环
		system("cls");  // 清屏（Windows系统命令）
		c.printboard();  // 打印棋盘状态
		/* 
		srand((unsigned)time(NULL));
		x = rand() % 9;
		y = rand() % 9;
		while(!c.drop(x,y)){
			x = rand() % 9;
			y = rand() % 9;
		}
		*/
		/*
		cin>>x>>y;
		while(!c.drop(x,y)){
			cin>>x>>y;
		}
		*/
		
		if(!montecarlo(c,x,y)){  // 使用蒙特卡洛算法为玩家1决定走法
			cout<<"player 1 lose"<<endl;  // 如果没有可行走法，玩家1输
			break;
		}
		cout<<x<<" "<<y<<endl;  // 打印选择的坐标
		c.drop(x,y);  // 执行走法
			
		if(c.gameover()){  // 如果游戏结束
			break;
		}
		
/**************************************/		
		c.printboard();  // 打印棋盘状态
		
		if(!montecarlo(c,x,y)){  // 使用蒙特卡洛算法为玩家2决定走法
			cout<<"player 2 lose"<<endl;  // 如果没有可行走法，玩家2输
			break;
		}
		cout<<x<<" "<<y<<endl;  // 打印选择的坐标
		c.drop(x,y);  // 执行走法
		
		/*
		cin>>x>>y;
		while(!c.drop(x,y)){
			cin>>x>>y;
		}
		*/
	}
	//cout<<"game over! player "<<c.getturn()<<" has stepped onto a wrong position!"<<endl;
	c.printboard();  // 游戏结束后打印最终棋盘状态
	return 0;  // 程序结束
}

