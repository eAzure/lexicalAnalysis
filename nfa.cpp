#include<iostream>
#include<stack>
#include<string>
#include<string.h>
#include"ShuntingYard.cpp"
using namespace std;
//������ʵ�ֺ�׺���ʽתNFA

class state{
    public:
    state(int id,string type,char symbol){
        this->id=id;
        this->type=type;
        this->symbol=symbol;
    }
    int id;//�����״̬�����
    string type;// 'n'-normal 'e'-epsilon 'end'-end
    char symbol;//��Ӧ��������ַ�
    //��һ��״̬�������
    state *out1=0;//�������һ��״̬
    state *out2=0;//������������һ��״̬
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
    int count=0;//��¼״̬���
    //�Դ�����γɵĺ�׺���ʽ���д�������NFA
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
    //ʹ��һ��״̬ջ�Ϳ��Խ���ݹ�������
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
                    //�ж��Ƿ������ӵ�״̬
                    //��Ҫдһ������
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
//    cout<<"������������ʽ��"<<endl;
//    string test;
//    cin>>test;
//    //string test="a($|b)";
//    test=shunting_initial(test);
//    test=ShuntingYard(test);
//    cout<<test<<endl;
//    stack<Fragment *>stateTable=transformToNfa(test);
//    stateTable.top()->endState->type="end";//�����ս�����
//    //cout<<stateTable.top()->startState->out1->out2->out1->symbol<<"    "<<stateTable.top()->startState->out1->out2->out1->type<<endl;
//    //���Բ���
//    Fragment *temp=stateTable.top();
//    state *tempState=temp->startState;
//    cout<<"��������Ҫ�жϵ��ַ�����"<<endl;
//    string testStr;
//    cin>>testStr;
//    //string testStr="ab";
//    //��һ���ݹ麯�����������
//    if(testS(tempState,testStr)){
//        cout<<"������ʽ���ܸ��ַ�"<<endl;
//    }else{
//        cout<<"������ʽ�����ܸ��ַ�"<<endl;
//    }
//
////����������ͣ�
///*
//1��a*b*���û�а���״̬������
//2��(a|b)ab aab
//���ⶼ�ǳ���������
//Ŀǰ�Ľ���취���Լ���ӵ㡣�������ý��иĽ����޸���shuntingyard�����.���жϣ�����Ӧ�Լ��ֶ���ӵ�
//���������ģ�(a|b)*a������취��(a|b)*.a
//
//�������һ�ֽ���취��
//����������Ҫ��.���������shuntingyard�����н��и���
//
//���Ҳ������((a|b)(c|d))*
//
//int|if��������⣬��Ϊ���������ѡ����˻�������⣬���
//����취Ϊ��i(nt|f)����ʱ�Ϳ�����
//
//a|ab����Ӧ����a($|b)������ʽ�Ϳ�����
//*/
//}
