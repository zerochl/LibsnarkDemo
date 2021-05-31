# 开发环境准备
## 镜像基础环境
Ubuntu 16.04
## 如何使用
```
docker run -it -v 本机目录:要隐射的镜像内目录 libsnark bash
```
本机目录内存放零知识证明项目，要隐射的镜像内目录随便即可，例如/root/libsnarkdemo

1、 init 

进入项目根目录
 ```
 git submodule update --init --recursive
 ```
2、 compile
 ```
 mkdir build; cd build; cmake ..; make
 ```
 You can find the "merkle" binary under the merkle folder.
 
 在build文件夹下面会出现merkle文件夹，文件夹里面有名为merkle的binary文件，这个就是可执行文件

## 默克尔树证明

depth = 3

1、setup
```
./merkle setup
```

2、prove
```
./merkle prove [data1] [data2] [data3] [data4] [data5] [data6] [data7] [data8] [index]
./merkle prove 1 2 3 4 5 6 7 8 3
```
data1 - data8：叶子节点的数据。本demo默克尔树为三层，叶子节点就为8个。
index：叶子节点的index，当前取值范围：0-7
输出proof文件和root：f171e00bb40c83de1f09c64e2cc4558e3c327aa9e8525a467c83576071bc1045

Record down the root information, which is used on verify.

3、verify
```
./merkle verify [root]
./merkle verify f171e00bb40c83de1f09c64e2cc4558e3c327aa9e8525a467c83576071bc1045
```
## x^3 + x + 5 == out方程式证明

1、setup
```
./merkle equationSetup
```
2、prove

初始化值硬编码在代码中了：x=3
```
./merkle equationProve
```
3、verify

初始化值硬编码在代码中了：out=35
```
./merkle equationVerify
```
## 渔船位置零知识证明
渔船 无需透露自己的具体位置，就可以向监管机构证明自己处于划定的 合法捕捞区内
文章描述https://flur.ee/2020/02/05/using-zero-knowledge-proofs-with-fluree/
snarkjs实现：https://github.com/fluree/example-zero-knowledge/blob/main/src/circuits/InRange.circom

1、setup
```
./merkle fishingBoatSetup
```
2、prove

初始化值硬编码在代码中了：

维度限制：30-60

经度限制：40-60

渔船位置（维度，经度）：40，50

多定义了一个out，表明每一个判别式的句式成立，即less为true，int类型表示为1，四个句式加一起out就为4.
```
./merkle fishingBoatProve
```
3、verify


```
./merkle fishingBoatVerify
```