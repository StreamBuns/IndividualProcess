//
//  main.cpp
//  IndividualProcess
//
//  Created by 赵桐 on 2021/3/13.
//

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;
int main(int argc, char *const *argv) {
    // insert code here...
    int ch;
    opterr = 0; //选项错误时不让报错
    string bamFile,header,dupFile,sex,referenceDBFile,process,correct,normalized;
    const char *pathForRaw="../RD_raw";
    const char *pathForAbsolute="../RD_absolute";
    const char *pathForNormalize="../RD_normalized";

    int windowsSize;
    while ((ch = getopt(argc, argv, "b:h:d:s:r:")) != -1) {
        switch (ch) {
            case 'b':bamFile=string(optarg);break;
            case 'h':header=string(optarg);break;
            case 'd':dupFile=string(optarg);break;
            case 's':sex=string(optarg);break;
            case 'r':referenceDBFile=string(optarg);break;
        }
    }
    windowsSize=stoi(referenceDBFile.substr(referenceDBFile.find_last_of(".")+1));
    process="./1-1CNVprogress -r "+referenceDBFile+" -b "+bamFile+" -w " +to_string(windowsSize)+" -h "+header;
    correct="./1-2CNVcorrect -r ../RD_raw/"+header+"_raw -d "+dupFile+ " -h "+header;
    normalized="./1-3CNVnormalize -h ../RD_absolute/"+header+" -w "+to_string(windowsSize)+" -s "+sex;
    int isCreate = mkdir(pathForRaw,S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if( !isCreate )
        printf("create path:%s\n",pathForRaw);
    else
        printf("create  %s failed!  \n",pathForRaw);
    system(process.data());


    isCreate = mkdir(pathForAbsolute,S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if( !isCreate )
        printf("create path:%s\n",pathForRaw);
    else
        printf("create  %s failed!  \n",pathForRaw);
    system(correct.data());

    isCreate = mkdir(pathForNormalize,S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if( !isCreate )
        printf("create path:%s\n",pathForRaw);
    else
        printf("create  %s failed!  \n",pathForRaw);
    system(normalized.data());
    return 0;
}
