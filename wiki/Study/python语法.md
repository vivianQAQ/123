1. str.upper():将字符串中的小写字母转为大写字母

2. round(number,num_digits)

number：需要四舍五入的数

digits：需要小数点后保留的位数

3. Python endswith() 方法用于判断字符串是否以指定后缀结尾，如果以指定后缀结尾返回True，否则返回False.

str.endswith(suffix[, start[, end]]); 可选参数"start"与"end"为检索字符串的开始与结束位置

Python startswith() 方法用于检查字符串是否是以指定子字符串开头

4. re.search 扫描整个字符串并返回第一个成功的匹配

re.search(pattern, string, flags=0);

pattern	匹配的正则表达式;

- str.replace(old, new[, max])

old -- 将被替换的子字符串

new -- 新字符串，用于替换old子字符串

max -- 可选字符串, 替换不超过 max 次

\s匹配任何空白字符

"\s+"表示匹配任意多个字符

[\s]表示，只要出现空白就匹配

[\S]表示，非空白就匹配

5. raise手动引发异常

```
except:
    print("\033[1;32m未知错误\033[0m")
    raise
```

assert（断言）用于判断一个表达式，在表达式条件为 false 的时候触发异常

assert expression等价于

```
if not expression:
    raise AssertionError
    ```

6. Python strip() 方法用于移除字符串头尾指定的字符（默认为空格或换行符）或字符序列

str.strip([chars]);

rpstr.rstrip([chars]);

```
# 移除逗号(,)、点号(.)、字母 s、q 或 w，这几个都是尾随字符
txt = "banana,,,,,ssqqqww....."
x = txt.rstrip(",.qsw")
print(x)
banana
```

7. 全局变量和局部变量
可以省略global关键字，如果在局部要对全局变量修改，需要在局部也要先声明该全局变量

nonlocal关键字用来在函数中使用外层(非全局)变量，nonlocal 要绑定一个局部变量

8. r防止字符转义

9. format

```
>>> "{1} {0} {1}".format("hello", "world")  # 设置指定位置
'world hello world'

print("网站名：{name}, 地址 {url}".format(name="菜鸟教程", url="www.runoob.com"))

# 通过字典设置参数
site = {"name": "菜鸟教程", "url": "www.runoob.com"}
print("网站名：{name}, 地址 {url}".format(**site))

# 通过列表索引设置参数
my_list = ['菜鸟教程', 'www.runoob.com']
print("网站名：{0[0]}, 地址 {0[1]}".format(my_list))  # "0" 是必须的
```

10. end=''

```
print ( 'a' ,end = ' ' )
print ( 'a' ,end = '1234' )
>>> a
>>> a1234
```

11. finally

如果存在finally子句，则该finally子句将作为try语句完成之前的最后一项任务执行。

finally无论该try语句是否产生异常，该子句都会运行

12. else

在 Python 的异常处理流程中还可添加一个 else 块，当 try 块没有出现异常时，程序会执行 else 块
```
else:
    pass
```

13. &gt;&amp;  ==>>  >&

14. self不可省略

15. 利用urljoin()方法对绝对url地址与相对url地址进行拼合

Compat 兼容性:提供Python 2和3兼容的数据结构、库和函数

16. os.system



17. quote、unquote、urlencode、urldecode

