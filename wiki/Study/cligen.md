`https://github.com/clicon/cligen`

# 基本框架

CLIgen提供交互命令行界面和api函数，设计者制定命令行语法并编写c回调函数来实现命令，用户输入定制语法实现相应的功能

```
//回调函数的cvv  - CLIgen命令行的变量列表;argv - 命令语法中声明的CLIgen变量
static int hello_cb(cligen_handle h, cvec *cvv, cvec *argv)
{
    //获取变量值
    //1. 循环获取单个变量
    //cg_var * cvec_each(cvec *cvv,cg_var *prev);
    cg_var *cv = NULL;
    while ((cv = cvec_each(cvv, cv)) != NULL) {
        ...
    }

    //2. 如果知道顺序，可以单独访问变量
    //cg_var * cvec_i(cvec *cvv,int i);
    //当cvec_i的参数是argv时，0、1代表第1,2个参数
    //当cvec_i的参数是cvv时，0代表整个命令行参数(cmd)，1、2代表第1,2个参数
    cg_var *cv = cvec_i(argv, 1);

    //3. 使用变量名查找变量的方法
    //cg_var * cvec_find(cvec *cvv,char *name);
    cg_var *cv = cvec_find(cvv, "addr");

    //4. 通过get/set 运算符来访问和修改值
    //ip:struct in_addr *addr = cv_ipv4addr_get(cv);
    printf("%s\n", cv_string_get(cvec_i(argv, 0)));

#if 0
    char *name = cv_string_get(cvec_i(argv, 0));
    if (strcmp(name, "ssh") == 0) {
        ssh();
    }else if (strcmp(name, "shell") == 0) {
        shell();
#endif
}

cgv_fnstype_t * str2fn(char *name, void *arg, char **error)
{
    //再根据回调函数名调用不同的回调函数
    if (strcmp(name, "hello") == 0) {
        return hello_cb;
    }else if (strcmp(name, "shell") == 0) {
        return shell_cb;
    }
    return unknown;
}

int main(int argc, char *argv[])
{
    cligen_handle h = cligen_init();
    //1. 解析字符串语法
    cligen_parse_str(h, hello_syntax, "hello world", NULL, NULL);

#if 0
    //2. 解析文件
    //解析文件需要在执行文件时在后面加上-f filename
    FILE *f = fopen(filename, "r"))
    cligen_parse_file(h, f, filename, NULL, globals);
#endif

    ph = cligen_ph_i(h, 0));
    pt = cligen_ph_parsetree_get(ph);
    //调用回调函数str2fn
    cligen_callbackv_str2fn(pt, str2fn, NULL);
    //命令循环
    cligen_loop(h);
    //终止函数
    cligen_exit(h);
}
```


# 命令行语法

```
shell("shell") <addr:ipv4addr> <port:int32>, shell("shell");
ssh("ssh") <addr:ipv4addr> <port:int32> [cmd1 <args1:args1_type>], shell("ssh");
hello("Greet the world") world, hello("Hello","world!");
```

- 第一个shell变量是顶层命令，shell后面括号里的是提示文本;

- 关键字也可以使用变量来指定<port:int32>

- TAB可以补全和提示下一层的命令列表

- 输入？：将提供下一层的命令列表和提示文本

- 中括号[]表示参数或者命令项是可选的，敲命令时可以敲也可以不敲

- <addr:string length[8:12]>string 可以限制长度范围

<x:int64 range[1000]>integers 可以限制数值范围

- 参数上还可以通过choice来限制参数的可选择范围

<interface:string choice:s1mme|s10|s11_c>这个参数就只能在s1mme,s10, s11_c这三个选项里面选择

- 逗号后面的shell是回调函数，后面括号里的变量可以用回调函数的cvec *argv参数来获取

因为ssh和shell都需要ip和port，它们可以调用相同的回调函数shell，再在回调函数里根据cvec *argv参数的值来区分


# api函数

1. 应用程序调用 cligen_handle h = cligen_init() 函数来初始化 CLIgen 库。该函数返回 CLIgen API 函数中使用的句柄

2. 两种获取cligen语法的方法：文件-cligen_parse_file（解析文件）、字符串-cligen_parse_str（解析字符串语法）

- int cligen_parse_file(cligen_handle h, FILE *f, char *name, parse_tree *obsolete, cvec *globals)

h:cligen句柄，f:文件指针

- int cligen_parse_str(cligen_handle h, char *str, char *name, parse_tree *pt, cvec *globals);

h:cligen句柄，str:字符串语法

- cligen_prompt_set、cligen_comment_set：设置提示符和注释

3. cligen_callbackv_str2fn绑定第一个回调函数str2fn;str2fn函数根据命令语法里的函数名，调用不同的回调函数

int cligen_callbackv_str2fn(parse_tree *pt, cgv_str2fn_t *str2fn, void *arg);

4. 获取变量值

| type     | Accessor        | C-type         |
| :---     | :------         | :-----         |
| ipv4addr | cv_ipv4addr_get | struct in addr |
| uint32   | cv_uint32_get   | uint32_t       |
| string   | cv_string_get   | char*          |

int cv2str(cg_var *cv, char *str, size_t size)可以将值转化成char *存放到str里


# 函数指针

```
typedef void (*common_cb_t)(cligen_handle h, int sock, cvec *cvv);

typedef struct cb_common_entry {
    char *name;
    common_cb_t cb;
} cb_common_t;

/* Common Callback look-up table */
static cb_common_t cb_common[] = {
    {"run", run_shell},
    {"exit", exit_shell},
    {"shell", exec_shell},
    {"reboot", reboot},
    {"poweroff", poweroff},
    {"upload", put_file},
    {"download", get_file},
    {"load_conf", load_conf},
    {"show_config", show_configuration},
};

int common_cb(cligen_handle h, cvec *cvv, cvec *argv)
{
    size_t i;
    char *name = cv_string_get(cvec_i(argv, 0));
    for (i = 0; i < sizeof(cb_common) / sizeof(cb_common_t); i++) {
        if (strcmp(name, cb_common[i].name)) {
            continue;
        }

        cb_common[i].cb(h, conf_info.sockfd, cvv);
        break;
    }

    return 0;
}
```
