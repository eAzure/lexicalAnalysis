#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<string.h>
#include<stack>
#include"nfa.cpp"
using namespace std;
//������ʵ�������Ĵʷ�����

//����token�ṹ
struct token{
    string content;
    string kind;
};
//����ؼ��ֵ�������ʽ
string keyWord="i(nt|f)|else|while|c(har|onst|lass|out)|double|f(or|loat)|break|long|static|new|void|return|public";
//��������������ʽ
string delimiter="{|}|[|]|=|;|\"";
//���������
string operat="+($|+)|-($|-)|~|/|=|>|<|&|!|>|<";//~����˷�
//��������
string number="0|(1|2|3|4|5|6|7|8|9|(0|1|2|3|4|5|6|7|8|9)*)";
//�����ʶ��������ĸ��ͷ
string identifier="(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9|_)*";

//������ַ�����NFA
stack<Fragment*>keyWordTable;
stack<Fragment*>delimiterTable;
stack<Fragment*>operatorTable;
stack<Fragment*>numberTable;
stack<Fragment*>identifierTable;

//����token��
stack<token>tokenTable;
//����ϲ����token��
stack<token>mergeTokenTable;

//����������ʽ���ɸ��ַ�����NFA
void generateNfa(){
    //���ɹؼ��ֵ�NFA
    keyWord=shunting_initial(keyWord);
    keyWord=ShuntingYard(keyWord);
    keyWordTable=transformToNfa(keyWord);
    //������ֹ����
    keyWordTable.top()->endState->type="end";
    //���ɽ����NFA
    delimiter=shunting_initial(delimiter);
    delimiter=ShuntingYard(delimiter);
    delimiterTable=transformToNfa(delimiter);
    //������ֹ����
    delimiterTable.top()->endState->type="end";
    //�����������NFA
    operat=shunting_initial(operat);
    operat=ShuntingYard(operat);
    operatorTable=transformToNfa(operat);
    //������ֹ����
    operatorTable.top()->endState->type="end";
    //�������ֵ�NFA
    number=shunting_initial(number);
    number=ShuntingYard(number);
    numberTable=transformToNfa(number);
    //������ֹ����
    numberTable.top()->endState->type="end";
    //���ɱ�ʶ����NFA
    identifier=shunting_initial(identifier);
    identifier=ShuntingYard(identifier);
    identifierTable=transformToNfa(identifier);
    //������ֹ����
    identifierTable.top()->endState->type="end";
    cout<<"����NFA��ɣ�"<<endl;
}

//���д�������ȡ
//��������Խ��д����жϣ���Ϊ���������жϱ�ʶ��ʱ��û���жϣ������жϺ󣬾Ϳ����������жϣ��ж��Ƿ���ϱ�ʶ���Ķ���
void lexicalAnalysis(string _string){
    //���ַ���NFA�е�Fragment��ȡ����
    //����ȡ�ؼ��ֵ�������ʽ��Fragment
    Fragment *keyWordFrag=keyWordTable.top();
    state *keyWordState=keyWordFrag->startState;
    //��ȡ���ֵ�������ʽ��Fragment
    Fragment *numberFrag=numberTable.top();
    state *numberState=numberFrag->startState;
    //��ȡ��ʶ����������ʽ��Fragment
    Fragment *identifierFrag=identifierTable.top();
    state *identifierState=identifierFrag->startState;
    //��ȡ�������������ʽ��Fragment
    Fragment *operatorFrag=operatorTable.top();
    state *operatorState=operatorFrag->startState;
    //��ȡ�����������ʽ��Fragment
    Fragment *delimiterFrag=delimiterTable.top();
    state *delimiterState=delimiterFrag->startState;

    string temp;
    for(int i=0;i<_string.length();i++){
        if((_string[i]>=48&&_string[i]<=57)||(_string[i]>=97&&_string[i]<=122)){
            temp.push_back(_string[i]);
        }else if(_string[i]==' '){
            if(!temp.empty()){
                if(testS(keyWordState,temp)){
                    tokenTable.push(token{temp,"keyWord"});
                    temp.clear();
                }else if(testS(numberState,temp)){
                    tokenTable.push(token{temp,"number"});
                    temp.clear();
                }else{
                    tokenTable.push(token{temp,"identifier"});
                    temp.clear();
                }
            }
        }else if(_string[i]=='('||_string[i]==')'){
            if(!temp.empty()){
                if(testS(keyWordState,temp)){
                    tokenTable.push(token{temp,"keyWord"});
                    temp.clear();
                }else if(testS(numberState,temp)){
                    tokenTable.push(token{temp,"number"});
                    temp.clear();
                }else{
                    tokenTable.push(token{temp,"identifier"});
                    temp.clear();
                }
            }
            temp.push_back(_string[i]);
            tokenTable.push(token{temp,"delimiter"});
            temp.clear();
        }else{
            //�жϷ���++��--
            //++��--��ô�����жϴ����أ��ҵķ������ڶ�ջ�н��кϲ������������Ƶ�cout<<
            if(!temp.empty()){
                if(testS(keyWordState,temp)){
                    tokenTable.push(token{temp,"keyWord"});
                    temp.clear();
                }else if(testS(numberState,temp)){
                    tokenTable.push(token{temp,"number"});
                    temp.clear();
                }else{
                    tokenTable.push(token{temp,"identifier"});
                    temp.clear();
                }
            }
            temp.push_back(_string[i]);
            if(testS(operatorState,temp)){
                tokenTable.push(token{temp,"operator"});
                temp.clear();
            }else if(testS(delimiterState,temp)){
                tokenTable.push(token{temp,"delimiter"});
                temp.clear();
            }
        }
    }
}


//ȥ��ע��
string getRidOfSpacesAndComments(string _string){
    string temp;
    for(int i=0;i<_string.length();i++){
        if(_string[i]=='/'&&i<_string.length()-1&&_string[i+1]=='/'){
            break;
        }
        temp.push_back(_string[i]);

    }
    return temp;
}

//tokenջ�ĺϲ������++,--,<<,<=,==,!=���Ƶ�
void mergeStack(){
    while(!tokenTable.empty()){
        mergeTokenTable.push(tokenTable.top());
        tokenTable.pop();
        if(!tokenTable.empty()){
            if(tokenTable.top().content=="<"&&mergeTokenTable.top().content=="<"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"<<","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="+"&&mergeTokenTable.top().content=="+"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"++","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="-"&&mergeTokenTable.top().content=="-"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"--","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="="&&mergeTokenTable.top().content=="<"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"<=","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="="&&mergeTokenTable.top().content=="="){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"==","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="="&&mergeTokenTable.top().content==">"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{">=","operator"});
                tokenTable.pop();
            }
            if(tokenTable.top().content=="="&&mergeTokenTable.top().content=="!"){
                mergeTokenTable.pop();
                mergeTokenTable.push(token{"!=","operator"});
                tokenTable.pop();
            }
        }
    }
}


void readThefile(char filename[]){
    ifstream in(filename);
    string line;//ÿһ�е��ı�
    if(in){
        while(getline(in,line)){
            //ȥ��ע�ͣ��ո��ڽ��дʷ�����ʱ���д���
            line=getRidOfSpacesAndComments(line);
            //���дʷ�����
            lexicalAnalysis(line);
        }
    }else{
        cout<<"�ļ��򲻿���"<<endl;
    }
}


int main(){
    //����NFA��
    generateNfa();
    //��ȡ�ļ�
    //ʾ���ļ�·����D:\lexicalAnalysis\code.txt
    cout<<"�������ļ�·����"<<endl;
    char filename[50];
    cin>>filename;
    readThefile(filename);
    mergeStack();
//    while(!tokenTable.empty()){
//        cout<<"<"<<tokenTable.top().content<<">"<<"  "<<"<"<<tokenTable.top().kind<<">"<<endl;
//        tokenTable.pop();
//    }
    while(!mergeTokenTable.empty()){
        cout<<"<"<<mergeTokenTable.top().content<<">"<<" "<<"<"<<mergeTokenTable.top().kind<<">"<<endl;
        mergeTokenTable.pop();
    }
    return 0;
}
