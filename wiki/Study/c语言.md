1. int getopt(int argc, char * const argv[], const char *optstring);
```
   const char *optstring = "abc:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
   while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 'a':
                printf("opt is a, oprarg is: %s\n", optarg);
                break;
        }
   }
```

原文链接：https://blog.csdn.net/afei__/article/details/81261879

2. patch
diff -u a.c b.c > test.patch

option:

-u 显示有差异行的前后几行(上下文), 默认是前后各 3 行, 这样, patch 中带有更多的信息

-p 显示代码所在的 c 函数的信息

-N 选项确保补丁文件将正确地处理已经创建或删除文件的情况

-a 逐行比较文本文件

-r 比较子目录中的文件

patch a.c < test.patch

patch -RE < test.patch 取消 patch 对源文件的修改

https://www.51cto.com/article/699332.html

3. shell

ctrl+a   ctrl+e   分别代表把管标移动到最前和最后

ctrl+u   ctrl+k   分别代表光标处往前和光标处往后删除

pavucontrol:ubuntu音量调节
