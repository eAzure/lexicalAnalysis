#include<iostream>
#include<stack>
#include<string>
#include<string.h>
#include"ShuntingYard.cpp"
using namespace std;
//本部分实现后缀表达式转NFA

class state{
    public:
    state(int id,string type,char symbol){
        this->id=id;
        this->type=type;
        this->symbol=symbol;
    }
    int id;//代表该状态的序号
    string type;// 'n'-normal 'e'-epsilon 'end'-end
    char symbol;//对应的输入的字符
    //下一个状态最多两个
    state *out1=0;//允许的下一个状态
    state *out2=0;//允许的另外的下一个状态
};

class Fragment{
    public:
    Fragment(state *startState,state *endState){
        this->startState=startState;
        this->endState=endState;
    }
    state *startState;
    state *endState;
};

stack<Fragment*> transformToNfa(string _string){
    stack<Fragment*>stateTable;
    int count=0;//记录状态编号
    //对处理后形成的后缀表达式进行处理，生成NFA
    for(int i=0;i<_string.length();i++){
        if(_string[i]=='*'){
            Fragment *temp1=stateTable.top();
            stateTable.pop();
            temp1->endState->out1=temp1->startState;
            state *st1=new state(count++,"e",'$');
            state *st2=new state(count++,"e",'$');
            st1->out1=temp1->startState;
            st1->out2=st2;
            temp1->endState->out2=st2;
            stateTable.push(new Fragment(st1,temp1->endState->out2));
        }else if(_string[i]=='|'){
            Fragment *temp1=stateTable.top();
            stateTable.pop();
            Fragment *temp2=stateTable.top();
            stateTable.pop();
            state *st1=new state(count++,"e",'$');
            state *st2=new state(count++,"e",'$');
            st1->out1=temp2->startState;
            st1->out2=temp1->startState;
            temp2->endState->out1=st2;
            temp1->endState->out1=st2;
            stateTable.push(new Fragment(st1,st2));

        }else if(_string[i]=='.'){
            Fragment *temp1=stateTable.top();
            //temp1->startState->id--;
            count--;
            stateTable.pop();
            Fragment *temp2=stateTable.top();
            stateTable.pop();
            temp2->endState->out1=temp1->startState;
            stateTable.push(new Fragment(temp2->startState,temp1->endState));
        }else{
            state *temp=new state(count++,"n",_string[i]);
            state *temp1=new state(count++,"e",'$');
            temp1->out1=temp;
            stateTable.push(new Fragment(temp1,temp));
        }
    }
    return stateTable;
}

bool judgeIsConnect(state *temp){
    stack <state*>tool;
    tool.push(temp);
    while(!tool.empty()){
        state *te=tool.top();
        tool.pop();
        if(te->type=="end"){
            return true;
        }
        if(te->out1&&te->out1->symbol=='$'){
            tool.push(te->out1);
        }
        if(te->out2&&te->out1->symbol=='$'){
            tool.push(te->out2);
        }
    }
    return false;
}

bool testS(state *temp,string str){
    //使用一个状态栈就可以解决递归问题了
    stack<state*>stateStack;
    stateStack.push(temp);
    int len=0;
    while(!stateStack.empty()&&len<str.length()){
        state *st=stateStack.top();
        stateStack.pop();
        if(st->type=="e"){
            if(st->out1){
                stateStack.push(st->out1);
            }
            if(st->out2){
                stateStack.push(st->out2);
            }
        }
        if(st->type=="n"||st->type=="end"){
            if(st->symbol==str[len]){
                if(len==str.length()-1&&judgeIsConnect(st)){
                    //判断是否是连接的状态
                    //需要写一个方法
                    return true;
                }
                len++;
                if(st->out1){
                    stateStack.push(st->out1);
                }
                if(st->out2){
                    stateStack.push(st->out2);
                }
            }
        }
    }
    return false;
}

//int main(){
//    cout<<"请输入正则表达式："<<endl;
//    string test;
//    cin>>test;
//    //string test="a($|b)";
//    test=shunting_initial(test);
//    test=ShuntingYard(test);
//    cout<<test<<endl;
//    stack<Fragment *>stateTable=transformToNfa(test);
//    stateTable.top()->endState->type="end";//设置终结条件
//    //cout<<stateTable.top()->startState->out1->out2->out1->symbol<<"    "<<stateTable.top()->startState->out1->out2->out1->type<<endl;
//    //测试部分
//    Fragment *temp=stateTable.top();
//    state *tempState=temp->startState;
//    cout<<"请输入你要判断的字符串："<<endl;
//    string testStr;
//    cin>>testStr;
//    //string testStr="ab";
//    //用一个递归函数来进行求解
//    if(testS(tempState,testStr)){
//        cout<<"正则表达式接受该字符"<<endl;
//    }else{
//        cout<<"正则表达式不接受该字符"<<endl;
//    }
//
////有问题的类型：
///*
//1、a*b*最后没有把两状态连起来
//2、(a|b)ab aab
//问题都是出在连接上
//目前的解决办法是自己添加点。。。。得进行改进，修改了shuntingyard中添加.的判断，以适应自己手动添加的
//比如这样的：(a|b)*a，解决办法是(a|b)*.a
//
//想出了另一种解决办法：
//考虑所有需要加.的情况，在shuntingyard代码中进行改正
//
//这个也行啦：((a|b)(c|d))*
//
//int|if会出现问题，因为入口有两种选择，因此会产生问题，因此
//解决办法为：i(nt|f)，此时就可以啦
//
//a|ab这种应该以a($|b)这种形式就可以啦
//*/
//}
