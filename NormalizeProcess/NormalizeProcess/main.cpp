//
//  main.cpp
//  程序名称：NormalizeProcess
//  程序功能：对之前的拷贝数的统计出的窗口片断进行修正。
//  程序使用方法：先用gcc进行编译，然后再结合参数进行编译
//  程序调用的参数：-h absoluteFilePath -w windowsSize -s sex -p p -l l
//              其中-h -w -s必填
//  Created by 赵桐 on 2021/1/24.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <zconf.h>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;


/*
 *功能定义输入的初始变量，主要是窗口大小windowsSize，可重复的上线l与下限p，还有染色体号s
 *其实后面也会进行变量的赋值，这里只是给予一个初始化的值
 */
int windowSize=800;
double p=0.05,l=0.05;
string absoluteFile;
string head;
string s="X";
int repeatMax=windowSize*0.3;
/*
 *函数名称：itoa
 *函数功能：把int转换为char*
 */
char* itoa(int val, int base);
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
 *函数名称：printGenomeCopy
 *函数功能：打印GenomeCopy,map里面的变量属于辅助函数
 */
void printGenomeCopy(unordered_map<pair<string, int>, int> genomeCopy);
/*
 *函数名称：topPercent
 *函数功能：取出数组特定百分比的数
 */
float topPercent(float p,vector<float> globalArray);

int main(int argc, char *const *argv) {
/*
 *定义变量，genomeCopy存储片段，GCreginAverage是存储特定位置的片段信息，Xarray存储性染色体的片段信息
 *globalArray存储常染色体片段信息，globalArray2存储矫正后的片段信息
 *
 */
    unordered_map<pair<string, int>, int> genomeCopy;
    map<int,vector<int>*> GCRegionAverage;
    vector<int> XArray;
    vector<int> globalArray;
    vector<float> globalArray2;
/*
 *功能获取程序名称
 */
    
    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
//-----------------------------------------------------------------------------
    
/*
 *此区域功能提供获取命令行参数，一般是-h ../RD_absolute/$header，-w 800 -s X -p 0.05 -l 0.05
 */
    
    int ch;
    opterr = 0; //选项错误时不让报错
    while ((ch = getopt(argc, argv, "h:w:s:pl")) != -1) {
        switch (ch) {
            case 'h':absoluteFile=string(optarg);break;
            case 'w':windowSize=stoi(string(optarg));break;
            case 's': s = string(optarg); break;
            case 'p': p = stod(string(optarg)) ; break;
            case 'l': l = stod(string(optarg)); break;
        }
    }
    repeatMax=windowSize*0.3;

    programName=absoluteFile.substr(absoluteFile.find_last_of('/')+1);
    cout<<"parsing "<<programName<<" starting!\n";
    
//-----------------------------------------------------------------------------
    
/*
 *加载片段信息
 */
    
    FILE *fp;
    cout<<absoluteFile<<endl;
//打开文件
    absoluteFile="/Users/zhaotong/SVsDemo/CNVcaller/RD_absolute/ERR340328";
    absoluteFile="cat "+absoluteFile;
    
    if ((fp = popen(absoluteFile.data(), "r")) == NULL) {
//        if ((fp = popen("cat "+absoluteFile, "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }

//    用于存储临时每行的数据
//   29    2    56    440    504    1
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
        
        genomeCopy.insert(pair<pair<string, int>, int>(pair<string, int>(stringArray[0],stoi(stringArray[1])),stoi(stringArray[stringArray.size()-1])));
        
        if(1 == s.compare(stringArray[0])){
            if(stoi(stringArray[4])<=repeatMax&&stoi(stringArray[stringArray.size()-1])==1){
                XArray.push_back(stoi(stringArray[2]));
            }
        }else{
            if(stoi(stringArray[4])<=repeatMax&&stoi(stringArray[stringArray.size()-1])==1){
                globalArray.push_back(stoi(stringArray[2]));
                
                auto iter=GCRegionAverage.find(stoi(stringArray[3]));
                if(iter!= GCRegionAverage.end()){
                    iter->second->push_back(stoi(stringArray[2]));
                }else{
                    GCRegionAverage.insert(pair<int,vector<int> *>(stoi(stringArray[3]),new vector<int>{stoi(stringArray[2])}));
                }

            }
        }

    
        stringArray.clear();
        memset(tempChar, 0, sizeof(char) * 500);
}
    cout<<"RD_absolute file loading finished.\n";
    
//-----------------------------------------------------------------------------

/*
 *找常染色体不同百分比下的中位数
 */
    sort(globalArray.begin(), globalArray.end());
    
    int median90=globalArray[(int)((0.9)*globalArray.size())];
    int median50=globalArray[(int)((0.5)*globalArray.size())];
    int median10=globalArray[(int)((0.1)*globalArray.size())];
    int median8=globalArray[(int)((0.08)*globalArray.size())];
    int median6=globalArray[(int)((0.06)*globalArray.size())];
    int median4=globalArray[(int)((0.04)*globalArray.size())];
    int median2=globalArray[(int)((0.02)*globalArray.size())];
   
    cout<<"tail 2 percent window is lower than "<<median2<<endl;
    cout<<"tail 4 percent window is lower than "<<median4<<endl;
    cout<<"tail 6 percent window is lower than "<<median6<<endl;
    cout<<"tail 8 percent window is lower than "<<median8<<endl;
    cout<<"tail 10 percent window is lower than "<<median10<<endl;
    cout<<"tail 50 percent window is lower than "<<median50<<endl;
    cout<<"tail 90 percent window is lower than "<<median90<<endl;
    
/*
 *找到性染色体的不同百分比的中位数，然后判定性染色体性染色体与常染色体的关系？
 */
    
    int Xmedian50=0;
    if(XArray.size()>=1){
        sort(XArray.begin(), XArray.end());
        int Xmedian90 =XArray[(int)((0.9)*XArray.size())];
        Xmedian50=XArray[(int)((0.5)*XArray.size())];
        int Xmedian10=XArray[(int)((0.1)*XArray.size())];
        cout<<"tail 10 percent "<<s<<" window is lower than "<<Xmedian10<<endl;
        cout<<"tail 50 percent "<<s<<" window is lower than "<<Xmedian50<<endl;
        cout<<"tail 90 percent "<<s<<" window is lower than "<<Xmedian90<<endl;
    }
    int sexCorrectFold=0;
    if(Xmedian50>median50*0.75&&Xmedian50<median50*1.5){
        sexCorrectFold=1;
        cout<<"sex chromosome "<<s<<" show similar coverage of autosome!\n";
    }else if(Xmedian50>median50*0.75&&Xmedian50<median50*0.25){
        sexCorrectFold=2;
        cout<<"sex chromosome "<<s<<" show ~ half coverage of autosome!\n";
    }else{
        sexCorrectFold=1;
        cout<<"sex chromosome "<<s<<" show UNKOWN relationship with autosome?\n";
    }
//-----------------------------------------------------------------------------

    
/*
 *计算每个GC含量区域的平均值
 */
    globalArray.clear();
    XArray.clear();
    int sum=0;
    int sumWindows=0;
    float temp;
    map<int,float> regionAverageSd;
    for(auto i:GCRegionAverage){
        sort(i.second->begin(), i.second->end());
        for(int j=(int)(i.second->size()*l);j<=(int)((1-p)*i.second->size());j++){
            sumWindows++;
            sum+=i.second->at(j);
        }
        
        temp=sumWindows>0?sum/sumWindows:0;
        regionAverageSd.insert(pair<int, float>(i.first,temp));
        cout<<i.first<<"\t"<<temp<<"\t"<<sumWindows<<"\t"<<i.second->size()-1<<"\n";
        
        sumWindows=0;
        sum=0;
    }
    
        GCRegionAverage.clear();
        cout<<"calculate average value for each GC content region done!\n";
        
//-----------------------------------------------------------------------------
    
/*
 *计算矫正的百分比
 */
    
        temp=(int)windowSize*0.4;
        auto iter= regionAverageSd.find(temp);
        while (iter==regionAverageSd.end()) {
            temp++;
            iter = regionAverageSd.find(temp);
        }
        int standardAverage = iter->second;
        
        for(auto &i:regionAverageSd){
            i.second=i.second>0?(float)((float)standardAverage/i.second):0;
        }
        
//-----------------------------------------------------------------------------
    
/*
 *根据矫正比例矫正GC的读取片段
 */
        
        if ((fp = popen(absoluteFile.data(), "r")) == NULL) {
            perror("Fail to popen\n");
            exit(1);
        }
        
        //    用于存储临时每行的数据
        //   29    2    56    440    504    1

        map<pair<string, int>,float> cleanRecord;
        while (fgets(tempChar, sizeof(tempChar), fp) != NULL) {
    //    去掉最后面的回车
            tempChar[strlen(tempChar) - 1] = 0;
            token = strtok(tempChar, spliteStr);
            while (token != NULL) {
                stringArray.push_back(token);
                token = strtok(NULL, spliteStr);
            }
            iter=regionAverageSd.find(stoi(stringArray[3]));
            if(iter==regionAverageSd.end()){
                regionAverageSd.insert(pair<int, float>(stoi(stringArray[3]),0));
                
                if(s.compare(stringArray[0])==1){
                    cleanRecord.insert(pair<pair<string, int>, float>(pair<string, int>(stringArray[0],stoi(stringArray[1])),0));
                }else{
                    globalArray2.push_back(0);
                    cleanRecord.insert(pair<pair<string, int>, float>(pair<string, int>(stringArray[0],stoi(stringArray[1])),0));
                }
                
                stringArray.clear();
                memset(tempChar, 0, sizeof(char) * 500);
                
                continue;

            }
            if(s.compare(stringArray[0])==1){
                cleanRecord.insert(pair<pair<string, int>, float>(pair<string, int>(stringArray[0],stoi(stringArray[1])),sexCorrectFold*stoi(stringArray[2])*iter->second));
            }else{
                globalArray2.push_back(stoi(stringArray[2])*iter->second);
                cleanRecord.insert(pair<pair<string, int>, float>(pair<string, int>(stringArray[0],stoi(stringArray[1])),stoi(stringArray[2])*iter->second));
            }
            

            stringArray.clear();
            memset(tempChar, 0, sizeof(char) * 500);
            
        }
        

        cout<<"correct read count according to GC content done!\n";
//-----------------------------------------------------------------------------
    
/*
 *计算全部的平均值和SD
 */
        sort(globalArray2.begin(), globalArray2.end());
        
        float correctMedian50 =globalArray2[(int)((0.5)*globalArray2.size())];
        
        cout<<"global median50 "<<correctMedian50<<"\n";
        
        int globalMax=topPercent(1-p, globalArray2);
        int globalMin=topPercent(l, globalArray2);
    
        globalArray2.clear();
        for(auto i:cleanRecord){
            if(i.second>globalMax||i.second<globalMin){
                continue;
            }
            globalArray2.push_back(i.second);
        }
    
   float sumForGlobalArray=0;
    for(auto i:globalArray2){
        sumForGlobalArray+=i;
    }
    
    float globalAverage = sumForGlobalArray/globalArray2.size();
    
    sumForGlobalArray=0;
    for(auto i:globalArray2){
        sumForGlobalArray+=(i-globalAverage)*(i-globalAverage);
    }
    
    float globalSd=sqrt(sumForGlobalArray/(globalArray2.size()-1));
    cout<<"global average: "<<globalAverage<<"\tglobal SD: "<<globalSd<<"\n";
    cout<<"the "<<1-p<<" th percentile absolute reads count:"<<globalMax<<"\n";
    cout<<"the "<<l<<" th percentile absolute reads count:"<<globalMin<<"\n";
    
    
//-----------------------------------------------------------------------------

    
/*
 * 输出矫正的数据
 */
//    absoluteFile=_mean_$correct_median50\_SD_$global_sd\_sex_$sex_correct_fold
    programName="ERR303";
    programName = programName+"/"+to_string(correctMedian50)+"/_SD_"+to_string(globalSd)+"/"+s+to_string(sexCorrectFold);
    programName ="/Users/zhaotong/SVsDemo/CNVcaller/"+programName;
    fstream outFile(programName.data(),ios::out);
    
    float copyNumber=0;
    for(auto i:cleanRecord){
        copyNumber=i.second/correctMedian50;
        outFile<<i.first.first<<"\t"<<i.first.second<<"\t"<<copyNumber<<"\t"<<i.second<<"\t"<<endl;
    }
    outFile.close();
    cout<<"succeed！\n";
    return 0;
}
//-----------------------------------------------------------------------------
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}
//-----------------------------------------------------------------------------
void printGenomeCopy(unordered_map<pair<string, int>, int> genomeCopy){
    for(auto i:genomeCopy){
        cout<<i.second<<endl;
    }
}
//-----------------------------------------------------------------------------
float topPercent(float p,vector<float> globalArray){
    int intCutoff=(int)(p*(globalArray.size()-1));
    return globalArray.at(intCutoff);
}
//-----------------------------------------------------------------------------
