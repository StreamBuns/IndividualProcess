//
//  main.cpp
//  CorrectProcess
//
//  Created by 赵桐 on 2020/12/1.
//

#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
//#include <hash_map>

using namespace std;
#define usage2
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};
template <class T1,class T2> struct hash<pair<T1,T2>>{
    size_t operator()(const pair<T1, T2>&p)const{
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};
void correct(unordered_map<pair<string , int>,int> &raw,unordered_map<pair<string, int>, string *> &link);
void loadingRawFile(unordered_map<pair<string , int>,int> &raw);
void loadingDupfile(unordered_map<pair<string, int>, string *> &link);
char *itoa(int val, int base) {
    static char buf[32] = {0};
    int i = 30;
    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i + 1];
}

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型
    char * strs = new char[str.length() + 1] ; //不要忘了
    strcpy(strs, str.c_str());
 
    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());
 
    char *p = strtok(strs, d);
    while(p) {
        string s = p; //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }
 
    return res;
}


/*
 *函数名：void printRaw(unordered_map<pair<string , int>,int> raw)
 *输入：unordered_map<pair<string , int>,int> raw
 *功能：打印raw里面的内容。
 *备注：raw是上一步处理得到的初步的readdeepth，是个键值对，key是染色体序号和窗口位置，值是两个深度相加之和。
 */
void printRaw(unordered_map<pair<string , int>,int> raw){
    for(auto i:raw){
        cout << i.second<<endl;
    }
}
void printLink(unordered_map<pair<string, int>, string *>link){
    for (auto i:link) {
        int size = stoi(*i.second);
        for (int m = 0; m < size; m++) {
            cout << i.second[m]<<endl;
        }
    }
}
int main(int argc, char *const *argv) {
    string programName = argv[0];
    programName = programName.substr(programName.find_last_of('/') + 1);
    cout << programName << endl;
    unordered_map<pair<string, int>,int> raw;
    unordered_map<pair<string, int>,string *> link;
    loadingRawFile(raw);
//    printRaw(raw);
    loadingDupfile(link);
//    printLink(link);
    correct(raw, link);
    cout << "absolute reads counts correction finished!\n" ;
    return 0;
}
void loadingRawFile(unordered_map<pair<string , int>,int> &raw){
    FILE *fp;

//打开文件
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/RD_raw/ERR340328_raw2", "r")) == NULL) {
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
/*
 *加载重复窗口文件。dupfile
 */

void loadingDupfile(unordered_map<pair<string, int>, string *> &link){
//    声明文件指针
    FILE *fp;


//打开文件
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/dupfile2", "r")) == NULL) {
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
void correct(unordered_map<pair<string , int>,int> &raw,unordered_map<pair<string, int>, string *> &link){
    FILE *fp;

//打开文件
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/RD_raw/ERR340328_raw2", "r")) == NULL) {
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
    const char s1[2] = ":";
    string chr;
    string position;
    int pos;
    int absoluteCp=0,genome_copy = 0,correctNumber=0;
    while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
//    去掉最后面的回车
        tempChar[strlen(tempChar) - 1] = 0;
        token = strtok(tempChar, s);
        while (token != NULL) {
            stringArray.push_back(token);
            token = strtok(NULL, s);
        }
        
        auto iterator=link.find(pair<string,int>(stringArray[0],stoi(stringArray[2])));
        if (iterator!=link.end()){
            correctNumber++;
            
            int size = stoi(*iterator->second);
            //因为那个size是加上了第一个所以大小大1.
            for (int i=1; i<size; i++) {
                cout << iterator->second[i]<<endl;
                
                chr=iterator->second[i];
                
               pos= (int)chr.find(s1);
                
                position=chr.substr(pos+1,chr.length());
                chr=chr.substr(0,pos);
                
                auto iteratorForRaw=raw.find(pair<string,int>(chr,stoi(position)));
                if (iteratorForRaw!=raw.end()) {
                    absoluteCp+=iteratorForRaw->second;
                }
            }
            cout<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<absoluteCp<<"\t"<<stringArray[5]<<"\t"<<stringArray[6]<<"\t"<<genome_copy<<"\n";
            
            
        }else             cout<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stoi(stringArray[3])+stoi(stringArray[4])<<"\t"<<stringArray[5]<<"\t"<<stringArray[6]<<"\t"<<1<<"\n";;
        
        
        
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
    }
    cout<< "corrected window number: "<<correctNumber<<endl;
}
