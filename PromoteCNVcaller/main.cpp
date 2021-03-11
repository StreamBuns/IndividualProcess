//
//  main.cpp
//  CNVcaller
//
//  Created by 赵桐 on 2020/11/4.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <getopt.h>
#include <map>
#include <string.h>
#include <vector>
#include <regex>
#include <fstream>
#include <unordered_map>
using namespace std;
#define usage2 "Program: count the number of reads map to each window based on BWA mapped BAM files, need to install samtools software\nUsage: "+programName+" <refdb> <BAM>\n\t\t-w\t\twindow\tsize (bp)\n\t\tdefault = 800, keep the same with reference database"
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}
/*
 *模版功能：提供一个hashcode用于unorder_map
 */
template <class T1,class T2> struct hash<pair<T1,T2>>{
    size_t operator()(const pair<T1, T2>&p)const{
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};
void loadingBamFile(unordered_map<pair<string,int>,int> &mul,unordered_map<pair<string, int>, int>& uniq);
void CompareOutput(unordered_map<pair<string, int>, int>& mul,unordered_map<pair<string, int>, int>& uniq);
int i=0;
int windowSize=1000;
string header;
string bamFile;
string refFile;
int stepSize=windowSize/2;
int main(int argc, char *const *argv) {
    // insert code here...

/*
 *定义变量 windowsSize窗口大小，bamFile bam，refFile
 */

    unordered_map<pair<string,int>,int> mul;
    unordered_map<pair<string,int>,int> uniq;

    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
/*
 ***********************************************
 */
    
/*
 取出命令行参数
 */
    int ch;
    opterr = 0; //选项错误时不让报错
    while ((ch = getopt(argc, argv, "r:b:w:")) != -1) {
        switch (ch) {
            case 'r':refFile=string(optarg);break;
            case 'b':bamFile=string(optarg);break;
            case 'w': windowSize = stoi(string(optarg)); break;
        }
    }

    
    cout<<"stats read depth per window start!\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;

//    vector<char*> out;
    loadingBamFile(mul,uniq);
//    cout<<"uniq:"<<endl;
//        for(auto i:uniq){
//            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second<<endl;
//        }
//        cout<<"mul:"<<endl;
//        for(auto i:mul){
//            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second<<endl;
//        }
//     cout<<argv[1]<<" file loading done!";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    CompareOutput(mul,uniq);
    cout<<"completed bam parsing!\n";
//    cout<<__DATE__<<" "<<__TIME__<<"\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}


//-----------------------------------------------------------------------------
/*
*
*/


void loadingBamFile(unordered_map<pair<string, int>, int> &mul,unordered_map<pair<string, int>, int> &uniq){
    FILE *fp;
    chdir("/Users/zhaotong/SVsDemo/CNVcaller/");
    bamFile="ERR340328.bam";
    bamFile="samtools view -F 0x504 "+bamFile;
//    if ((fp = popen(bamFile.data(), "r")) == NULL) {
    if ((fp = popen("cat testBam", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    vector<char *> stringArray;
    string date;
//    用于临时存储读取的段
    char tempChar[500];
    char *token;
    const char spliteStr[2] = "\t";
    int start,end;
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        date=tempChar;
        token = strtok(tempChar, spliteStr);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, spliteStr);
        }
        if(stringArray.size()<=10){
            stringArray.clear();
            memset(tempChar, 0, sizeof(char) * 500);
            continue;
        }
    
//        cout <<stringArray[3]<<"\t"<<i++<<endl;
        start=int((stoi(stringArray[3])+float(strlen(stringArray[9])/2))/windowSize)*windowSize+1;
        end=int((stoi(stringArray[3])+float(strlen(stringArray[9])/2))/windowSize+0.5)*windowSize-stepSize+1;
//此处如果是按照原来的程序是上面的end，不过我感觉end应该是下面那个，即开始加上一步。
//        end =start+windowSize/2;
        if(date.find("XA:Z")){
            auto  iterator=mul.find(pair<string,int>(stringArray[2],start));
             if(iterator==mul.end())
                 mul.insert(pair<pair<string ,int>,int>(pair<string,int>(stringArray[2],start),1));
             else iterator->second++;

             iterator=mul.find(pair<string,int>(stringArray[2],end));
             if(iterator==mul.end())
                 mul.insert(pair<pair<string ,int>,int>(pair<string,int>(stringArray[2],end),1));
             else iterator->second++;
         }else{
             auto  iterator=uniq.find(pair<string,int>(stringArray[2],start));
             if(iterator==uniq.end())
                 uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(stringArray[2],start),1));
             else iterator->second++;
             iterator=uniq.find(pair<string,int>(stringArray[2],end));
             if(iterator==uniq.end())
                 uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(stringArray[2],end),1));
             else iterator->second++;
        }
        
    
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
}

}
/*
 *
 */
void CompareOutput(unordered_map<pair<string, int>, int>& mul,unordered_map<pair<string,int>,int> &uniq){
    FILE *fp;
    
    fstream outFile("/Users/zhaotong/SVsDemo/CNVcaller/coutRaw4",ios::out);
    refFile = "cat "+refFile;
//    if ((fp = popen(refFile.date(), "r")) ==
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/referenceDB.1000", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    vector<char *> stringArray;
//    用于临时存储读取的段
    char tempChar[500];
    char *token;
    const char spliteStr[2] = "\t";
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        token = strtok(tempChar, spliteStr);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, spliteStr);
        }

        auto  mulIterator=mul.find(pair<string,int>(stringArray[0],stoi(stringArray[2])));
        auto  uniqIterator=uniq.find(pair<string,int>(stringArray[0],stoi(stringArray[2])));
        if(mul.end()!=mulIterator&&uniq.end()!=uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<uniqIterator->second<<"\t"<<mulIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()==mulIterator&&uniq.end()!=uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<uniqIterator->second<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()!=mulIterator&&uniq.end()==uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<mulIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else{
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }
    
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
}
    outFile.close();
}
