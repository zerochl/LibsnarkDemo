# 开发环境准备
## 镜像基础环境
Ubuntu 16.04
## 如何使用
```
docker run -it -v 本机目录:要隐射的镜像内目录 libsnark bash
```
本机目录内存放零知识证明项目，要隐射的镜像内目录随便即可，例如/root/zkSnarksDemo
# libsnark demo
实现了默克尔树、x^3 + x + 5 == out方程、渔船位置上报的零知识证明demo，具体参考libsnarkdemo目录下readme说明。
# ethsnark demo
## 为什么需要ethsnark
ethsnark的诞生源自libsnark的缺陷，从以太坊拜占庭硬分叉之后，以太链引入了基于ALT_BIN128的配对函数计算预编译合约。
如果直接使用libsnark的验证等式，以太坊上的预编译合约无法实现，在不影响Groth16安全性的前提下修改了Groth16的验证等式。
vk.alpha_beta = e(A, B) e(-x, vk.gamma) e(-C, vk.delta) ==> e(A, B) e(-x, vk.gamma) e(-C, vk.delta) * e(-alpha, beta) = 1

## demo说明
本库使用loopring v3的电路来作为demo基础，在原有电路上做了一些修改与代码结构优化，配上丰富的代码注释，可以更方便的学习ethsnark。

具体请参考ethsnarkdemo目录下readme说明