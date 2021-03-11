//
//  main.cpp
//  程序名称：CorrectProcess
//  程序功能：对之前的窗口文件进行矫正，得到绝对的窗口片段读数
//  程序使用方法：先用gcc进行编译，然后再结合参数进行编译
//  程序调用的参数：-r rawFile -d dupFile -c saveFile
//
//  Created by 赵桐 on 2020/12/1.
//

#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <zconf.h>
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
/*
 *函数名称：loadingRawFile
 *函数功能：加载1-1生成的raw文件，选取有效的数据存入raw数据结构中
 */
void loadingRawFile(unordered_map<pair<string , int>,int> &raw);
/*
 *函数名称：loadingDupfile
 *函数功能：加载dupFile重复窗口记录文件，需拿去有效的数据存入link数据结构中
 */
void loadingDupfile(unordered_map<pair<string, int>, string *> &link);
/*
 *函数名称：correct
 *函数功能：将加载的重复窗口记录与原始窗口数据进行矫正。
 */
void correct(unordered_map<pair<string , int>,int> &raw,unordered_map<pair<string, int>, string *> &link);
/*
 *函数名称：itoa
 *函数功能：把int转换为char*
 */
char *itoa(int val, int base) {
    static char buf[32] = {0};
    int i = 30;
    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i + 1];
}
//-----------------------------------------------------------------------------
string rawFile,dupfile,savaFile;
int main(int argc, char *const *argv) {
    string programName = argv[0];
    programName = programName.substr(programName.find_last_of('/') + 1);
    cout << programName << endl;
/*
* 此区域功能提供获取命令行参数，一般是-h ../RD_absolute/$header，-w 800 -s X -p 0.05 -l 0.05
*/
        
        int ch;
        opterr = 0; //选项错误时不让报错
        while ((ch = getopt(argc, argv, "r:d:c:")) != -1) {
            switch (ch) {
                case 'r':rawFile=string(optarg);break;
                case 'd':dupfile=string(optarg);break;
                case 'c':savaFile = string(optarg); break;
            }
        }
    
    unordered_map<pair<string, int>,int> raw;
    unordered_map<pair<string, int>,string *> link;
    loadingRawFile(raw);
    loadingDupfile(link);
    correct(raw, link);
    cout << "absolute reads counts correction finished!\n" ;
    return 0;
}
//-----------------------------------------------------------------------------
  
void loadingRawFile(unordered_map<pair<string , int>,int> &raw){
    FILE *fp;
    rawFile="cat "+rawFile;
//打开文件
    if ((fp = popen(rawFile.data(), "r")) ==
//    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/RD_raw/ERR340328_raw3", "r")) ==
        NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    //    用于存储临时每行的数据
    //    29    1    1    50    0    482    129
    vector<char *> stringArray;
//    用于临时存储读取的段
    char tempChar[500];
    char *token;
    const char s[2] = "\t";

    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        token = strtok(tempChar, s);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, s);
        }
        
        raw.insert(pair<pair<string, int>, int>(pair<string, int>(stringArray[0], atoi(stringArray[2])), atoi(stringArray[3])+atoi(stringArray[4])));
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
        
        
        
}
    cout<<"raw reads count file loading finished!\n";
}
//-----------------------------------------------------------------------------
  
void loadingDupfile(unordered_map<pair<string, int>, string *> &link){
//    声明文件指针
    FILE *fp;

    dupfile="cat "+dupfile;
//    打开文件
    if ((fp = popen(dupfile.data(), "r")) == NULL) {

//    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/dupfile", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
//    用于存储临时每行的数据
//    29:10068501    29:10068501    29:37307501    29:30795001
    vector<char *> stringArray;
//    用于临时存储读取的段
    char tempChar[500];
    char *token;
    const char s[2] = "\t";
    const char s1[2] = ":";
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        token = strtok(tempChar, s);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, s);
        }

        stringArray[0] = strtok(stringArray[0], s1);
        token = strtok(NULL, s1);

        string *strArray2 = new string[stringArray.size()];

        for (int i = 0; i < stringArray.size(); i++) {
            strArray2[i] = stringArray[i];
        }

        strArray2[0] = to_string(stringArray.size());
        link.insert(pair<pair<string, int>, string *>(pair<string, int>(stringArray[0], atoi(token)), strArray2));



        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);

    }
    cout << "duplicated window record file loading finished!\n";
}
//-----------------------------------------------------------------------------
  
void correct(unordered_map<pair<string , int>,int> &raw,unordered_map<pair<string, int>, string *> &link){
    FILE *fp;
    fstream outFile(savaFile.data(),ios::out);
//     打开文件
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/RD_raw/ERR340328_raw3", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
//        用于存储临时每行的数据
//        29    1    1    50    0    482    129
    vector<char *> stringArray;
//    用于临时存储读取的段
     
    char tempChar[500];
    char *token;
    const char s[2] = "\t";
    const char s1[2] = ":";
    string chr;
    string position;
    int pos;
    int absoluteCp=0,genome_copy = 0,correctNumber=0,size=0;
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        token = strtok(tempChar, s);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, s);
        }
        auto iterator=link.find(pair<string,int>(stringArray[0],stoi(stringArray[2])));
//               如果存在这个窗口则把这个基因的拷贝片段的读段相加
        if (iterator!=link.end()){
            correctNumber++;
            size = stoi(*iterator->second);
//              因为那个size是加上了第一个所以大小大1.
            for (int i=1; i<size; i++) {
                
                chr=iterator->second[i];
                
               pos= (int)chr.find(s1);
                
                position=chr.substr(pos+1,chr.length());
                chr=chr.substr(0,pos);
                
                auto iteratorForRaw=raw.find(pair<string,int>(chr,stoi(position)));
                if (iteratorForRaw!=raw.end()) {
                    absoluteCp+=iteratorForRaw->second;
                }
            }
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<absoluteCp<<"\t"<<stringArray[5]<<"\t"<<stringArray[6]<<"\t"<<genome_copy<<"\n";
        }
//        没有记录的窗口那么就直接把多重索引与单个的索引相加。
        else outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stoi(stringArray[3])+stoi(stringArray[4])<<"\t"<<stringArray[5]<<"\t"<<stringArray[6]<<"\t"<<1<<"\n";;
        
        
        
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
    }
    cout<< "corrected window number: "<<correctNumber<<endl;
}
//-----------------------------------------------------------------------------
  
