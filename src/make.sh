#!//bin/bash
for file in *.cpp
do
g++ -o  ${file%%.*} $file -std=c++11;
mv ${file%%.*} ../bin
done
echo "编译成功";

