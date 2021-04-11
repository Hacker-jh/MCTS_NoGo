#pragma GCC optimize("O2")
#pragma GCC optimize("O3")
#pragma GCC optimize("Ofast,no-stack-protector")

#include "jsoncpp/json.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include <math.h>
#include <limits.h>
#define Timing_Rate 0.9765
using namespace std;




int board[9][9] = {0};

bool dfs_air_visit[9][9] = {0};
const int cx[] = {-1, 0, 1, 0};
const int cy[] = {0, -1, 0, 1};

inline bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }

class MCTS_Node
{
public:
    MCTS_Node();
    
    int Max_Childen = 0;//最大可有的孩子节点数量
    
    int Number_of_Children = 0;//记录现有几个孩子节点
    
    MCTS_Node *children[81];

    int current_board[9][9] = {0};

    int col = 0;//表示黑旗还是白旗
    
    MCTS_Node *parent = NULL;
   
    int number_of_simulations = 0;
    
    double number_of_wins = 0.00;
    
    int available_position[81];//棋盘上可以下的位置
    
    void get_available_position(); //得到可行的行动 roll out?
    
    bool dfsAir(int fx, int fy);         //判断是否有气
    
    bool judgeAvailable(int fx, int fy); //判断是否可下 只判断 不改变
    
    double roll_out(); //模拟  

    void clear();
    
    MCTS_Node *Selection(double C);  
    
    MCTS_Node *Expansion();
    
    MCTS_Node *tree_search_process();
    
    void Backpropagation(double reward);
};



double MCTS_Node::roll_out()
{
    int win1 = 0, win2 = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            bool BOT1 = judgeAvailable(i, j);
            col = -col;
            bool BOT2 = judgeAvailable(i, j);
            col = -col;
            
            if (!BOT1 && BOT2)
                win2 ++;
            else if (!BOT1 && BOT2)
                win1 ++;     
        }
    int tpo1=win2*100;
    int tpo2=win1*100;
    return tpo1/100-tpo2/100;
}

    
bool MCTS_Node::dfsAir(int fx, int fy)
{
    dfs_air_visit[fx][fy] = true;
    bool flag = false;
    for (int dir = 0; dir < 4; dir++) //某一位置有气的条件为周围有空格或者有一个邻近的同色有气(反证法进一步证明邻近的有一个有气即都有气)
    {
        
        int xum_num = (fx + cx[dir])*100;
        int yum_num = (fy + cy[dir])*100;
        int dx = xum_num/100, dy = yum_num/100;
        if (inBorder(dx, dy))
        {
            if (current_board[dx][dy] == 0)
                flag = true;
            if (current_board[dx][dy] == current_board[fx][fy] && !dfs_air_visit[dx][dy])
                if (dfsAir(dx, dy))
                {
                    flag = true;
                    
                }
        }
    }
    return flag;
}

bool MCTS_Node::judgeAvailable(int fx, int fy)
{
    if (current_board[fx][fy])
        return false;
    current_board[fx][fy] = col;
    memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
    if (!dfsAir(fx, fy)) //判定自杀
    {
        current_board[fx][fy] = 0;
        return false;
    }
    for (int dir = 0; dir < 4; dir++) //不围对方
    {
        int dx = fx + cx[dir], dy = fy + cy[dir];
        if (inBorder(dx, dy))
        {
            if (current_board[dx][dy] && !dfs_air_visit[dx][dy])
            {
                if (!dfsAir(dx, dy))
                {
                    current_board[fx][fy] = 0;
                    return false;
                }
            }
        }
    }
    current_board[fx][fy] = 0;
    return true;
}

inline MCTS_Node::MCTS_Node()
{
    memset(available_position, 0, sizeof(available_position));
    memset(current_board, 0, sizeof(current_board)); 
}

void MCTS_Node::clear(){
        memset(available_position, 0, sizeof(available_position));
    }

void MCTS_Node::get_available_position()
{
    clear();
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (judgeAvailable(i, j))//如果这里可下
            {
                int okkk_steps = i * 9 + j;
                int ols = okkk_steps*100;
                available_position[Max_Childen] = ols/100;
                Max_Childen++;//
            }
}

MCTS_Node *MCTS_Node::Selection(double C)
{
    MCTS_Node *best_seleceted_child = NULL;
    
    double max_score = INT_MIN;
    for (int i = 0; i < Number_of_Children; i++)//对每个孩子节点
    {
        MCTS_Node *p = this->children[i];
        double score = p->number_of_wins / (p->number_of_simulations) + 0.2 * C * sqrt(log(2 * number_of_simulations) / (p->number_of_simulations));
        double tus_num = (double)score/100;
        if (max_score < tus_num*100)
        {
            best_seleceted_child = p;
            max_score = tus_num*100;         
        }
    }
    return best_seleceted_child;
}

MCTS_Node *MCTS_Node::Expansion()
{

    
    //int a = 0;
    //if(Max_Childen>1) a = available_position[rand()%Max_Childen];
    //else a=available_position[Max_Childen];
    //available_position[Number_of_Children-1];//在棋盘中找能下的地方加到搜索树上
    int a = available_position[Number_of_Children];//在棋盘中找能下的地方加到搜索树上

    int x = a / 9;
    int y = a % 9;
    
    MCTS_Node *new_MCTS_Node = new MCTS_Node;
    
    new_MCTS_Node->parent = this;
    
    children[Number_of_Children++] = new_MCTS_Node;//把新节点加入到children[]中


    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            new_MCTS_Node->current_board[i][j] = current_board[i][j];
    

    int kcl = -col;

    new_MCTS_Node->col = kcl;
    

    new_MCTS_Node->current_board[x][y] = col;
    
    new_MCTS_Node->get_available_position();//存储棋盘中可以下的点，为下次扩展做准备
    
    return new_MCTS_Node;
}

MCTS_Node *MCTS_Node::tree_search_process()
{
    if (Max_Childen == 0)
    {
        //无棋可下
        return this;
    }else if (Number_of_Children >= Max_Childen) //>=
    {
        MCTS_Node *p = Selection(1);
        return p->tree_search_process();//迭代呢，一层一层往下找
    }else return Expansion();//到叶了
}

void MCTS_Node::Backpropagation(double reward)//reward
{
    MCTS_Node *temp = this;
    while (temp)
    {
        temp->number_of_simulations++;
        int yus_num = (temp->number_of_wins + reward)*100;
        temp->number_of_wins = yus_num/100;
        reward = -reward;
        temp = temp->parent;
    }
}

int main()
{
    srand((unsigned)time(0));
    string str;
    int x, y;
    // 读入JSON
    getline(cin, str);

    int start = clock();//计时
    int timeout = (int)((0.9689) * (double)CLOCKS_PER_SEC); //玄学参数 算力提升100%
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    //initialize
   
    MCTS_Node *node = new MCTS_Node;
    
    int temp = 0; //为什么这里不能直接["requests"][0]呢 非要用个变量
    
    //确定我方这次下什么颜色的棋
    if (input["requests"][temp]["x"].asInt() == -1) node->col = 1;//白旗                               //黑棋
    else node->col = -1;//黑旗

    int choice_of_color = node->col;
    //default
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++)
    {
        x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
        if (x != -1)
        {
            board[x][y] = -choice_of_color;
        }
        x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
        if (x != -1)
        {
            board[x][y] = choice_of_color;
        }
    }
    x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
    if (x != -1) 
    {
        board[x][y] = -choice_of_color;
        
    }

    for (int i = 0; i < 9; i++) 
        for (int j = 0; j < 9; j++)
            node->current_board[i][j] = board[i][j];
    
    node->get_available_position();

   
    while (clock() - start < timeout)//开始，重复
    {
        MCTS_Node * Temp_MCTS_Node = node->tree_search_process();
        
        double reward = Temp_MCTS_Node->roll_out();// roll_out/simulation
        
        double real_num = (double)reward/100;
        Temp_MCTS_Node->Backpropagation(real_num*100);//反向传播
    }

    //输出结果
    Json::Value ret;
    Json::Value okkk_stepsion;

    MCTS_Node *best_seleceted_child = node->Selection(0);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] != best_seleceted_child->current_board[i][j])///很sb?
            {
                okkk_stepsion["x"] = i;
                okkk_stepsion["y"] = j;
                break;
            }
    ret["response"] = okkk_stepsion;
    ret["debug"] = turnID;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    return 0;
}