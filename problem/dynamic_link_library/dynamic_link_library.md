# 动态库编译连接方法
## 讲解动态库的一些参数

    -fpic  在编译的过程中，告诉编译器产生位置无关代码
    -shared 生成一个共享目标文件, 他可以和其他目标文件连接产生可执行文件.
## 把单目标或者文件编译成动态库
```
 gcc -fPIC -shared xxx.c -o xxx.so
 gcc -fPIC -shared xxx.o -o libxxx.so
```

