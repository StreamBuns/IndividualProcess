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
int main(int argc, char *const *argv) {
    // insert code here...

    string programName=argv[0];
    programName=programName.substr(programName.find_last_of('/')+1);
    cout<<programName<<endl;
    int ch;
//    const char *optstring = ;
//    while((ch = getopt(argc,argv, "abc:")) != -1){
////        opta
//        if(optarg==NULL)cout<<"null"<<endl;
//        switch (ch) {
//            case 'a':
//                ch = getopt(argc,argv, "abc:");
//                cout<<"opt is a\n"<<optopt;
//                break;
//            case 'b':
//                cout<<"opt is b\n";
//                break;
//            case 'c':
//                cout<<"opt is c\n";
//                break;
//            case '?':
//                printf("error optopt: %c\n", optopt);
//                printf("error opterr: %d\n", opterr);
//                break;
//        }
//    }
    int windowSize=1000;
    int stepSize=windowSize/2;
    map<pair<string,int>,int> mul;
    map<pair<string,int>,int> uniq;
    cout<<"stats read depth per window start!\n";
    cout<<__DATE__<<" "<<__TIME__<<"\n";
    string header;
    vector<char*> out;
    loadingBamFile(mul,uniq);
    cout<<"uniq:"<<endl;
        for(auto i:uniq){
            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second<<endl;
        }
        cout<<"mul:"<<endl;
        for(auto i:mul){
            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second<<endl;
        }
//     cout<<argv[1]<<" file loading done!";
    cout<<__DATE__<<" "<<__TIME__<<"\n";
    CompareOutput(mul,uniq);
    cout<<"completed bam parsing!\n";
//    cout<<out[0];
//    int fds[2];
//    if(pipe(fds)==-1){
//        cerr<<"创建匿名管道失败\n";
//    }
//    pid_t pid=fork();
//    if(pid==-1)cerr<<"创立子进程失败\n";
//    if(pid==0){
////        子进程
//        close(fds[0]);
//        write(fds[1], "abc", 3);
//        close(fds[1]);
//        exit(0);
//    }else{
////        夫进程
//        close(fds[1]);
//        char buf[100]={};
//
//        int r = (int)read(fds[0], buf, 100);
//        if(r>0){
//            cout<<"读取成功:"<<buf;
//        }else{
//            cerr<<"读取失败\n";
//        }
//        close(fds[0]);
//    }
    return 0;
}
//-----------------------------------------------------------------------------



void loadingBamFile(map<pair<string,int>,int> &mul,map<pair<string,int>,int> &uniq){
    FILE *fp;
//    map<pair<string,int>,int> mul;
//    map<pair<string,int>,int> uniq;
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/loadingBamFile", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    char tempChar[500];
//    int i=0;
    while (fgets(tempChar,sizeof(tempChar), fp)!=NULL){
        string tempString=tempChar;
        tempChar[strlen(tempChar)-1]=0;
        char**read_inf=(char**)malloc(sizeof(char*)*20);
        char* token;
        const char s[2] = "\t";
        int i=0;
        token = strtok(tempChar, s);
        while( token != NULL ) {
            read_inf[i++]=token;
            printf( "%s\n", token );
            token = strtok(NULL, s);
        }

        read_inf[3]=itoa((int)(strlen(read_inf[9])/2)+atoi(read_inf[3]),10);
       int  windowSize=1000;

        int start= int(atoi(read_inf[3])/windowSize)*windowSize+1;
        int end=start+windowSize/2;
//        if(regex_search(tempString,regex("XA:Z"))){
        if(tempString.find("XA:Z")==MaxPath_64){
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
//        for(auto i:uniq){
//            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second;
//        }
//        for(auto i:mul){
//            cout<<i.first.first<<"->"<<i.first.second<<":"<<i.second;
//        }
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
//    map<pair<string,int>,int> mul;
//    map<pair<string,int>,int> uniq;
    if ((fp = popen("cat /Users/zhaotong/SVsDemo/CNVcaller/referenceDB.1000", "r")) == NULL) {
        perror("Fail to popen\n");
        exit(1);
    }
    char tempChar[500];
    while (fgets(tempChar,sizeof(tempChar), fp)!=NULL){
        string tempString=tempChar;
        char**read_inf=(char**)malloc(sizeof(char*)*20);
        char* token;
        const char s[2] = "\t";
        int i=0;
        token = strtok(tempChar, s);
        while( token != NULL ) {
            read_inf[i++]=token;
            printf( "%s\n", token );
            token = strtok(NULL, s);
        }
        auto  mulIterator=mul.find(pair<string,int>(read_inf[0],atoi(read_inf[2])));
        auto  uniqIterator=uniq.find(pair<string,int>(read_inf[0],atoi(read_inf[2])));
        if(mul.end()!=mulIterator&&uniq.end()!=uniqIterator){
            outFile<<read_inf[0]<<"\t"<<read_inf[1]<<"\t"<<read_inf[2]<<"\t"<<uniqIterator->second<<"\t"<<mulIterator->second<<"\t"<<read_inf[3]<<"\t"<<read_inf[4]<<"\n";
        }else if(mul.end()==mulIterator&&uniq.end()!=uniqIterator){
            outFile<<read_inf[0]<<"\t"<<read_inf[1]<<"\t"<<read_inf[2]<<"\t"<<uniqIterator->second<<"\t"<<0<<"\t"<<read_inf[3]<<"\t"<<read_inf[4]<<"\n";
        }else if(mul.end()!=mulIterator&&uniq.end()==uniqIterator){
            outFile<<read_inf[0]<<"\t"<<read_inf[1]<<"\t"<<read_inf[2]<<"\t"<<0<<"\t"<<mulIterator->second<<"\t"<<read_inf[3]<<"\t"<<read_inf[4]<<"\n";
        }else{
            outFile<<read_inf[0]<<"\t"<<read_inf[1]<<"\t"<<read_inf[2]<<"\t"<<0<<"\t"<<0<<"\t"<<read_inf[3]<<"\t"<<read_inf[4]<<"\n";
        }
    }
}
