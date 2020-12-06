//
//  main.cpp
//  CorrectProcess
//
//  Created by 赵桐 on 2020/12/1.
//

#include <iostream>
#include <map>
#include <vector>

using namespace std;
#define usage2
void loadingDupfile(map<pair<string,int>,vector<char*> *>& link);
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}
vector<char*>*&  deepCopyStringArray(vector<char*>& stringArray);
int main(int argc, char *const *argv) {
    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
    map<pair<string,int>,vector<char*> *> link;
    loadingDupfile(link);
//    for(auto i:link){
//        for(auto j:*i.second){
//            cout<<j<<"\t";
//        }
//        cout<<endl;
//    }
    cout<<"duplicated window record file loading finished!\n";
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
/*
 *负责深度拷贝函数
 */
//vector<char*>* & deepCopyStringArray(vector<char*>& stringArray){
//    void * temp;
//    (temp)=(vector<char*>*)malloc(sizeof(stringArray));
//    for(auto i=stringArray.begin();i!=stringArray.end();i++){
//
////        temp.push_back();
//    }
//    return (vector<char*>*)temp;
//}
void loadingDupfile(map<pair<string,int>,vector<char*> *>& link) {
//    声明文件指针
    FILE *fp;
//打开文件
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/dupfile2", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
//    用于存储临时每行的数据
//    29:10068501    29:10068501    29:37307501    29:30795001
    vector<char*> stringArray;
//    用于临时存储读取的段
    char tempChar[500];
    char *token;
    const char s[2] = "\t";
    const char s1[2]=":";
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar)-1]=0;
        token = strtok(tempChar, s);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, s);
        }
//        cout<<"\nstringArray----------\n";
//        for(auto i:stringArray){
//            cout<<i<<"\t";
//        }
//        read_inf[1] = strtok(read_inf[0], s1);
        stringArray[0]=strtok(stringArray[0], s1);
        token=strtok(NULL, s1);
//        cout<<stringArray[0]<<"\t"<<token<<endl;
        vector<char*> temp;
//        memset(tempChar, 0, sizeof(tempChar));
//        for(auto i:stringArray){
//            cout<<i;
////            sprintf(tempChar, "%s",i);
//
//        }
        vector<int > intArray={1,2,3,4};
        vector<int > intArray2;
        intArray2.assign(intArray.begin()+1, intArray.end());

//        int intArray3[10];
        link.insert(pair<pair<string ,int>,vector<char*>*>(pair<string,int>(stringArray[0],atoi(token)),new vector<char*>(stringArray.begin()+1, stringArray.end())));
//        link.insert(pair<pair<string,int>(stringArray[0],atoi(token)),vector<char*>>())
        stringArray.clear();
        memset(tempChar, 0, sizeof(tempChar));
//        cout<<"-----------\n";
//        cout<<token<<endl;
//        cout<<read_inf[0]<<endl;
//        cout<<"-----------\n";
//        link.insert(pair<pair<string,int>,int*>(read_inf[1],));
    }
}
