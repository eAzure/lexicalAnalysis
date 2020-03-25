//���ȳ��㷨��ʵ�ִ�������ʽ���ɺ�׺���ʽ
#include <iostream>
#include <stack>
#include <string>
#include <string.h>
#include <map>
using namespace std;
stack<char>opCode;
/*
���ȼ���
1��*
2��.
3��|
4��(��)
*/
map<char,int>priority;
string shunting_initial(string _string){
    priority['*']=4;
    priority['.']=3;
    priority['|']=2;
    priority['(']=1;
    priority[')']=1;
    string ans;
    //����ʼ������ʽ������.���󷵻����µ�
    for(int i=0;i<_string.length();i++){
        if(i>=1){
            if((_string[i-1]!='|'&&_string[i-1]!='(')&&(_string[i]!='*'&&_string[i]!='|'&&_string[i]!='('&&_string[i]!=')')){
                ans.push_back('.');
            }else if((_string[i-1]=='*'||_string[i-1]==')')&&_string[i]=='('){
                ans.push_back('.');
            }else if((_string[i-1]!='*'&&_string[i-1]!='|'&&_string[i-1]!='('&&_string[i-1]!=')')&&_string[i]=='('){
                ans.push_back('.');
            }
            ans.push_back(_string[i]);
        }else{
            ans.push_back(_string[i]);
        }
    }
    return ans;
}
string ShuntingYard(string _string){
        string ans;
        for(int i=0;i<_string.length();i++){
            if(_string[i]=='*'||_string[i]=='|'||_string[i]=='('||_string[i]==')'||_string[i]=='.'){
                if(opCode.empty()||(_string[i]=='('||opCode.top()=='(')){
                    opCode.push(_string[i]);
                }else if(_string[i]==')'){
                    while(opCode.top()!='('&&!opCode.empty()){
                        ans.push_back(opCode.top());
                        opCode.pop();
                    }
                    opCode.pop();
                }else if(priority[_string[i]]<=priority[opCode.top()]){
                    while(!opCode.empty()&&priority[_string[i]]<=priority[opCode.top()]){
                        ans.push_back(opCode.top());
                        opCode.pop();
                    }
                    opCode.push(_string[i]);
                }else if(priority[_string[i]]>priority[opCode.top()]){
                    opCode.push(_string[i]);
                }
            }else{
                ans.push_back(_string[i]);
//                if(i>=1&&i<_string.length()-1){
//                    int temp=i-1;
//                    int temp1=i+1;
//                    if((_string[temp]!='*'&&_string[temp]!='|'&&_string[temp]!='('&&_string[temp]!=')')&&(_string[temp1]=='*'||_string[temp1]=='|'||_string[temp1]=='('||_string[temp1]==')')){
//                        ans.push_back('.');
//                    }
//                }
            }
        }
        while(!opCode.empty()){
            ans.push_back(opCode.top());
            opCode.pop();
        }
        return ans;
}
//int main(){
////    acd|b*c|(d*e|f)*g
//    string test;
//    cout<<"������������ʽ��"<<endl;
//    cin>>test;
//    //string test="(abcd*|c)*";
//    test=shunting_initial(test);
//    cout<<"��׺���ʽΪ��"<<ShuntingYard(test)<<endl;
//}
