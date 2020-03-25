#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<string.h>
#include<stack>
#include"nfa.cpp"
using namespace std;
//本部分实现完整的词法分析

//定义token结构
struct token{
    string content;
    string kind;
};
//定义关键字的正则表达式
string keyWord="i(nt|f)|else|while|c(har|onst|lass|out)|double|f(or|loat)|break|long|static|new|void|return|public";
//定义界符的正则表达式
string delimiter="{|}|[|]|=|;|\"";
//定义运算符
string operat="+($|+)|-($|-)|~|/|=|>|<|&|!|>|<";//~代表乘法
//定义数字
string number="0|(1|2|3|4|5|6|7|8|9|(0|1|2|3|4|5|6|7|8|9)*)";
//定义标识符，以字母开头
string identifier="(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9|_)*";

//定义各字符集的NFA
stack<Fragment*>keyWordTable;
stack<Fragment*>delimiterTable;
stack<Fragment*>operatorTable;
stack<Fragment*>numberTable;
stack<Fragment*>identifierTable;

//定义token集
stack<token>tokenTable;
//定义合并后的token集
stack<token>mergeTokenTable;

//根据正则表达式生成各字符集的NFA
void generateNfa(){
    //生成关键字的NFA
    keyWord=shunting_initial(keyWord);
    keyWord=ShuntingYard(keyWord);
    keyWordTable=transformToNfa(keyWord);
    //设置终止条件
    keyWordTable.top()->endState->type="end";
    //生成界符的NFA
    delimiter=shunting_initial(delimiter);
    delimiter=ShuntingYard(delimiter);
    delimiterTable=transformToNfa(delimiter);
    //设置终止条件
    delimiterTable.top()->endState->type="end";
    //生成运算符的NFA
    operat=shunting_initial(operat);
    operat=ShuntingYard(operat);
    operatorTable=transformToNfa(operat);
    //设置终止条件
    operatorTable.top()->endState->type="end";
    //生成数字的NFA
    number=shunting_initial(number);
    number=ShuntingYard(number);
    numberTable=transformToNfa(number);
    //设置终止条件
    numberTable.top()->endState->type="end";
    //生成标识符的NFA
    identifier=shunting_initial(identifier);
    identifier=ShuntingYard(identifier);
    identifierTable=transformToNfa(identifier);
    //设置终止条件
    identifierTable.top()->endState->type="end";
    cout<<"构造NFA完成！"<<endl;
}

//进行词向量提取
//在这里可以进行错误判断，因为我这里在判断标识符时并没有判断，加上判断后，就可以做错误判断，判断是否符合标识符的定义
void lexicalAnalysis(string _string){
    //将字符集NFA中的Fragment提取出来
    //先提取关键字的正则表达式的Fragment
    Fragment *keyWordFrag=keyWordTable.top();
    state *keyWordState=keyWordFrag->startState;
    //提取数字的正则表达式的Fragment
    Fragment *numberFrag=numberTable.top();
    state *numberState=numberFrag->startState;
    //提取标识符的正则表达式的Fragment
    Fragment *identifierFrag=identifierTable.top();
    state *identifierState=identifierFrag->startState;
    //提取运算符的正则表达式的Fragment
    Fragment *operatorFrag=operatorTable.top();
    state *operatorState=operatorFrag->startState;
    //提取界符的正则表达式的Fragment
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
            //判断符号++和--
            //++和--怎么进行判断处理呢？我的方法是在堆栈中进行合并处理，还有类似的cout<<
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


//去掉注释
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

//token栈的合并，针对++,--,<<,<=,==,!=类似的
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
    string line;//每一行的文本
    if(in){
        while(getline(in,line)){
            //去掉注释，空格在进行词法分析时进行处理
            line=getRidOfSpacesAndComments(line);
            //进行词法分析
            lexicalAnalysis(line);
        }
    }else{
        cout<<"文件打不开！"<<endl;
    }
}


int main(){
    //创建NFA集
    generateNfa();
    //读取文件
    //示例文件路径：D:\lexicalAnalysis\code.txt
    cout<<"请输入文件路径："<<endl;
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
