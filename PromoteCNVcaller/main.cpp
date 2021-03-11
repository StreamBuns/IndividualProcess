//
//  main.cpp
//  程序名称：CNVprogress
//  程序功能：加载bam源文件，然后与对照文件比对，输出单独映射与多映射记录表
//  程序使用方法：先用gcc进行编译，然后再结合参数进行编译
//  程序调用的参数：-r referenceDBFile -b bamFile -w windowsSize
//  Created by 赵桐 on 2020/11/4.
//

#include <iostream>
#include <unistd.h>
#include <string>
#include <getopt.h>
#include <map>
#include <vector>
#include <fstream>
#include <unordered_map>
using namespace std;

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
char* itoa(int val, int base);
void loadingBamFile(unordered_map<pair<string,int>,int> &mul,unordered_map<pair<string, int>, int>& uniq);
void CompareOutput(unordered_map<pair<string, int>, int>& mul,unordered_map<pair<string, int>, int>& uniq);
/*
 *定义变量 windowsSize窗口大小，bamFile bam，refFile
 */
int windowSize=1000;
string bamFile;
string refFile;
int stepSize=windowSize/2;
int main(int argc, char *const *argv) {
    // insert code here...



    unordered_map<pair<string,int>,int> mul;
    unordered_map<pair<string,int>,int> uniq;

    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
//-----------------------------------------------------------------------------

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

    stepSize=windowSize/2;
    
    cout<<"stats read depth per window start!\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    loadingBamFile(mul,uniq);
    cout<<"bam file loading done!\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    CompareOutput(mul,uniq);
    cout<<"completed bam parsing!\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *函数名称：itoa
 *函数功能：把int转换为char*
 */
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}
//-----------------------------------------------------------------------------
/*
 *函数名称：loadingBamFile
 *函数功能：将bam文件读出将有用的数据提取出来存放到 mul跟uniq中
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
    char tempChar[1024*8];
    char **read_inf= (char**)(malloc(20 * sizeof(char *)));
    char *token;
    const char s[2] = "\t";
    string tempString;
    int i=0;
    int start,end;
    while (fgets(tempChar,sizeof(tempChar), fp)!=NULL){
        tempString=tempChar;
        tempChar[strlen(tempChar)-1]=0;
        token = strtok(tempChar, s);
        while( token != NULL ) {
            read_inf[i++]=token;
            token = strtok(NULL, s);
        }

        start=int((atoi(read_inf[3])+float(strlen(read_inf[9])/2))/windowSize)*windowSize+1;
        end=int((atoi(read_inf[3])+float(strlen(read_inf[9])/2))/windowSize+0.5)*windowSize-stepSize+1;
        if(tempString.find("XA:Z")==string::npos){
            auto  iterator=mul.find(pair<string,int>(read_inf[2],start));
            if(iterator==mul.end())
                mul.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],start),1));
            else iterator->second++;
            iterator=mul.find(pair<string,int>(read_inf[2],end));
            if(iterator==mul.end())
                mul.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],end),1));
            else iterator->second++;
        }else{
            auto  iterator=uniq.find(pair<string,int>(read_inf[2],start));
            if(iterator==uniq.end())
                uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],start),1));
            else iterator->second++;
            iterator=uniq.find(pair<string,int>(read_inf[2],end));
            if(iterator==uniq.end())
                uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],end),1));
            else iterator->second++;
        }
        i=0;
        memset(tempChar,0,sizeof(tempChar));
    }

}
//-----------------------------------------------------------------------------
/*
 *函数名称：CompareOutput
 *函数功能：将bam文件与比对文件进行比对，然后进行输出得到文件Raw
 */
void CompareOutput(unordered_map<pair<string, int>, int>& mul,unordered_map<pair<string,int>,int> &uniq){
    FILE *fp;

    fstream outFile("/Users/zhaotong/SVsDemo/CNVcaller/coutRaw7",ios::out);
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
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<mulIterator->second<<"\t"<<uniqIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()==mulIterator&&uniq.end()!=uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<uniqIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()!=mulIterator&&uniq.end()==uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<mulIterator->second<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else{
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }

        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
    }
    outFile.close();
}
//-------------------------------------------------------------------------------
