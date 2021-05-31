# 开发环境准备
## 镜像基础环境
Ubuntu 16.04
## 如何使用
```
docker run -it -v 本机目录:要隐射的镜像内目录 libsnark bash
```
本机目录内存放零知识证明项目，要隐射的镜像内目录随便即可

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

## 路印V3证明

###1、setup
dex_circuit同级目录下创建keys文件夹
```
./dex_circuit zero setup
```
###2、prove
block.json文件copy到dex_circuit同级目录下

block.json包含8条记录，会构成loopring的默克尔树
```
./dex_circuit zero prove block.json
```
###3、verify
proof.raw是一个json文件，包含了proof和public input
```
./dex_circuit zero verify keys/all_8_vk.json proof.raw
```