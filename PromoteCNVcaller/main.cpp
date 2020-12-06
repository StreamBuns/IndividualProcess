//
//  main.cpp
//  CNVcaller
//
//  Created by 赵桐 on 2020/11/4.
//

#include <iostream>
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string>
#include <getopt.h>
#include <map>
#include <string.h>
#include <vector>
#include <regex>
#include <fstream>

using namespace std;
#define MaxPath_64 18446744073709551615
#define usage2 "Program: count the number of reads map to each window based on BWA mapped BAM files, need to install samtools software\nUsage: "+programName+" <refdb> <BAM>\n\t\t-w\t\twindow\tsize (bp)\n\t\tdefault = 800, keep the same with reference database"
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}
void loadingBamFile(map<pair<string,int>,int>& mul,map<pair<string,int>,int>& uniq);
void CompareOutput(map<pair<string,int>,int>& mul,map<pair<string,int>,int>& uniq);
int i=0;
int main(int argc, char *const *argv) {
    // insert code here...

    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
    int ch;
    while((ch = getopt(argc,argv, "abc:")) != -1){
        if(optarg==NULL)cout<<"null"<<endl;
        switch (ch) {
            case 'a':
                ch = getopt(argc,argv, "abc:");
                cout<<"opt is w\n"<<optopt;
                printf("%s\n",optarg);
//                cout<<optarg;
                break;
            case 'b':
                cout<<"opt is b\n";
                break;
            case 'c':
                cout<<"opt is c\n";
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }
    int windowSize=1000;
    int stepSize=windowSize/2;
    map<pair<string,int>,int> mul;
    map<pair<string,int>,int> uniq;

    cout<<"stats read depth per window start!\n";
    std::cout << "The run time is: " <<(double)clock() / CLOCKS_PER_SEC << "s" << std::endl;
    string header;
    vector<char*> out;
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



void loadingBamFile(map<pair<string,int>,int> &mul,map<pair<string,int>,int> &uniq){
    FILE *fp;
    chdir("/Users/zhaotong/SVsDemo/CNVcaller/");
//    system("samtools view -F 0x504 ERR340328.bam >>loadingBamFile2");
//    fp=fopen("/Users/zhaotong/SVsDemo/CNVcaller/loadingBamFile2","r");
//    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/loadingBamFile2", "r")) == NULL) {
    if ((fp = popen("samtools view -F 0x504 ERR340328.bam", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    char tempChar[1024*8];
    char **read_inf= (char**)(malloc(20 * sizeof(char *)));
    char *token;
    const char s[2] = "\t";
    string tempString;
    int i=0;
    int  count=0;
    while (fgets(tempChar,sizeof(tempChar), fp)!=NULL){
        tempString=tempChar;
        tempChar[strlen(tempChar)-1]=0;
        token = strtok(tempChar, s);
        while( token != NULL ) {
//            stringArray[i++]=token;
            read_inf[i++]=token;
//            printf( "%s\n", token );
            token = strtok(NULL, s);
        }

        read_inf[3]=itoa((int)(strlen(read_inf[9])/2)+atoi(read_inf[3]),10);
       int  windowSize=1000;
        int start= int(atoi(read_inf[3])/windowSize)*windowSize+1;
        int end=start+windowSize/2;
//        if(regex_search(tempString,regex("XA:Z"))){
        if(tempString.find("XA:Z")==MaxPath_64){
//        if(strStr(tempString,"XA:Z")==0){
//           count = mul.count(pair<string,int>(read_inf[2],start));
           auto  iterator=mul.find(pair<string,int>(read_inf[2],start));
//            if(count==0)
            if(iterator==mul.end())
                mul.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],start),1));
            else iterator->second++;
//            else mul[pair<string,int>(read_inf[2],start)]++;
//map[1]=1;

            iterator=mul.find(pair<string,int>(read_inf[2],end));
//            count = mul.count(pair<string,int>(read_inf[2],end));
//            if(0==count)
//                mul.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],end),1));
//            else mul[pair<string,int>(read_inf[2],end)]++;
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
//            count = uniq.count(pair<string,int>(read_inf[2],start));
////           auto  iterator=mul.find(pair<string,int>(read_inf[2],start));
//            if(count==0)
//                uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],start),1));
////            else iterator->second++;
//            else mul[pair<string,int>(read_inf[2],start)]++;
////            iterator=mul.find(pair<string,int>(read_inf[2],end));
//            count = uniq.count(pair<string,int>(read_inf[2],end));
//            if(0==count)
//                uniq.insert(pair<pair<string ,int>,int>(pair<string,int>(read_inf[2],end),1));
//            else uniq[pair<string,int>(read_inf[2],end)]++;
        }
        i=0;
        memset(tempChar,0,sizeof(tempChar));
    }
}
/*
 *
 */
void CompareOutput(map<pair<string,int>,int>& mul,map<pair<string,int>,int>& uniq){
    FILE *fp;
    fstream outFile("/Users/zhaotong/SVsDemo/CNVcaller/coutRaw",ios::out);
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/referenceDB.1000", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    char tempChar[1024*8];
//    vector<char*> stringArray;
//    char **read_inf= (char**)(malloc(20 * sizeof(char *)));
        char**stringArray=(char**)malloc(sizeof(char*)*20);
    char *token;
    const char s[2] = "\t";
    int i=0;
    while (fgets(tempChar,sizeof(tempChar), fp)!=NULL){

        token = strtok(tempChar, s);
        while( token != NULL ) {
            stringArray[i++]=token;
            token = strtok(NULL, s);
        }
        auto  mulIterator=mul.find(pair<string,int>(stringArray[0],atoi(stringArray[2])));
        auto  uniqIterator=uniq.find(pair<string,int>(stringArray[0],atoi(stringArray[2])));
        if(mul.end()!=mulIterator&&uniq.end()!=uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<uniqIterator->second<<"\t"<<mulIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()==mulIterator&&uniq.end()!=uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<uniqIterator->second<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else if(mul.end()!=mulIterator&&uniq.end()==uniqIterator){
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<mulIterator->second<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }else{
            outFile<<stringArray[0]<<"\t"<<stringArray[1]<<"\t"<<stringArray[2]<<"\t"<<0<<"\t"<<0<<"\t"<<stringArray[3]<<"\t"<<stringArray[4]<<"\n";
        }
        i=0;
    }
    outFile.close();
}
