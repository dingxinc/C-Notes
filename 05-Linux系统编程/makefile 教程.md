
#makefile #make #编译



使用 GCC 的命令行进行程序编译在单个文件下是比较方便的，当工程中的文件逐渐增多，甚至变得十分庞大的时候，使用 GCC 命令编译就会变得力不从心。这种情况下我们需要借助项目构造工具 make 帮助我们完成这个艰巨的任务。 **make 是一个命令工具，是一个解释 makefile 中指令的命令工具**，一般来说，大多数的 IDE 都有这个命令，比如：Visual C++ 的 nmake，QtCreator 的 qmake 等。

make 工具在构造项目的时候需要加载一个叫做 `makefile` 的文件，makefile 关系到了整个工程的编译规则。一个工程中的源文件不计数，其按类型、功能、模块分别放在若干个目录中，<u>makefil 定义了一系列的规则来指定哪些文件需要先编译，哪些文件需要后编译，哪些文件需要重新编译，甚至于进行更复杂的功能操作，</u>因为makefile 就像一个 Shell 脚本一样，其中也可以执行操作系统的命令。

**makefile 带来的好处就是——“自动化编译”，一旦写好，只需要一个 make 命令，整个工程完全自动编译，极大的提高了软件开发的效率。**

makefile 文件有两种命名方式  `makefile` 和 `Makefile`，构建项目的时候在哪个目录下执行构建命令 `make` 这个目录下的 makefile 文件就会被加载，因此在一个项目中可以有多个 makefile 文件，分别位于不同的项目目录中。



# 1. 规则



> Makefile 的框架是由规则构成的。make 命令执行时先在 Makefile 文件中查找各种规则，对各种规则进行解析后运行规则。规则的基本格式为：

```makefile
# 每条规则的语法格式:
target1,target2...: depend1, depend2, ...
	command
	......
	......
```

每条规则由三个部分组成分别是 `目标(target)`, `依赖(depend)`和 `命令(command)`。



* `命令(command)` ：当前这条规则的动作，一般情况下这个动作就是一个 shell 命令
  * 例如：通过某个命令编译文件、生成库文件、进入目录等
  * 动作可以是多个，`每个命令前必须有一个 Tab 缩进并且独占一行`
* `依赖(depend)` ： 规则所必需的依赖条件，在规则的命令中可以使用这些依赖
  * 例如：生成可执行文件的目标文件 `（*.o）`可以作为依赖使用
  * 如果规则的命令中不需要任何依赖，那么规则的依赖可以为空
  * 当前规则中的依赖可以是其他规则中的某个目标，这样就形成了规则之间的嵌套
  * 依赖可以根据要执行的命令的实际需求, 指定很多个
* `目标(target)` ：规则中的目标，这个目标和规则中的命令是对应的
  * 通过执行规则中的命令，可以生成一个和目标同名的文件
  * 规则中可以有多个命令, 因此可以通过这多条命令来生成多个目标, 所有目标也可以有很多个
  * 通过执行规则中的命令，可以只执行一个动作，不生成任何文件，这样的目标被称为 `伪目标`



```makefile
# 举例: 有源文件 a.c b.c c.c head.h, 需要生成可执行程序 app
################# 例1 #################
app:a.c b.c c.c
	gcc a.c b.c c.c -o app

################# 例2 #################
# 有多个目标, 多个依赖, 多个命令
app,app1:a.c b.c c.c d.c
	gcc a.c b.c -o app
	gcc c.c d.c -o app1
	
################# 例3 #################	
# 规则之间的嵌套
app:a.o b.o c.o
	gcc a.o b.o c.o -o app
# a.o 是第一条规则中的依赖
a.o:a.c
	gcc -c a.c
# b.o 是第一条规则中的依赖
b.o:b.c
	gcc -c b.c
# c.o 是第一条规则中的依赖
c.o:c.c
	gcc -c c.c
```



# 2. 工作原理



> 在此主要剖析一下通过提供的 makefile 文件，构建工具 make 什么时候编译项目中的所有文件,  什么时候只选择更新项目中的某几个文件。另外再研究一下如果 makefile 里边有多个规则它们之间是如何配合工作的，我们基于下边的例子：
>



## 2.1 规则的执行



`在调用 make 命令编译程序的时候，make 会首先找到 Makefile 文件中的第 1 个规则，分析并执行相关的动作。`但是需要注意的是，好多时候要执行的动作（命令）中使用的依赖是不存在的，如果使用的依赖不存在，这个动作也就不会被执行。

对应的解决方案是先将需要的依赖生成出来，我们就可以在 makefile 中添加新的规则，将不存在的依赖作为这个新的规则中的目标，当这条新的规则对应的命令执行完毕，对应的目标就被生成了，同时另一条规则中需要的依赖也就存在了。

这样，makefile 中的某一条规则在需要的时候，就会被其他的规则调用，直到 makefile 中的第一条规则中的所有的依赖全部被生成，第一条规则中的命令就可以基于这些依赖生成对应的目标，make 的任务也就完成了。

```makefile
# makefile
# 规则之间的嵌套
# 规则1
app:a.o b.o c.o
	gcc a.o b.o c.o -o app
# 规则2
a.o:a.c
	gcc -c a.c
# 规则3
b.o:b.c
	gcc -c b.c
# 规则4
c.o:c.c
	gcc -c c.c
```



> 在这个例子中，如果执行 make 命令就会根据这个 makefile 中的 4 条规则编译这三个源文件。**在解析第一条规则的时候发现里边的三个依赖都是不存在的，因此规则对应的命令也就不能被执行。**
>
> 当依赖不存在的时候，make 就是查找其他的规则，看哪一条规则是用来生成需要的这个依赖的，找到之后就会执行这条规则中的命令。因此规则 2， 规则 3， 规则 4 里的命令会相继被执行，当规则 1 中依赖全部被生成之后对应的命令也就被执行了，因此规则 1 的目标被生成，make 工作结束。
>

> 知识点拓展：
>
> 如果想要执行 makefile 中非第一条规则对应的命令,  那么就不能直接 `make`,  需要将那条规则的目标也写到 make 的后边,  比如只需要执行规则 3 中的命令,  就需要:  `make b.o`。



## 2.2 文件的时间戳



**make 命令执行的时候会根据文件的时间戳判定是否执行 makefile 文件中相关规则中的命令。**

* 目标是通过依赖生成的，因此 `正常情况下：目标时间戳 > 所有依赖的时间戳`, 如果执行 make 命令的时候检测到规则中的目标和依赖满足这个条件, 那么规则中的命令就不会被执行。
* 当依赖文件被更新了,  文件时间戳也会随之被更新,  这时候 `目标时间戳 < 某些依赖的时间戳`,  在这种情况下目标文件会通过规则中的命令被重新生成。
* 如果规则中的目标对应的文件根本就不存在， 那么规则中的命令肯定会被执行。



```makefile
# makefile
# 规则之间的嵌套
# 规则1
app:a.o b.o c.o
	gcc a.o b.o c.o -o app
# 规则2
a.o:a.c
	gcc -c a.c
# 规则3
b.o:b.c
	gcc -c b.c
# 规则4
c.o:c.c
	gcc -c c.c
```

> 根据上文的描述,  先执行 make 命令，基于这个 makefile 编译这几个源文件生成对应的目标文件。然后再修改例子中的 `a.c`,  再次通过 `make` 编译这几个源文件，那么这个时候先执行规则 2 更新目标文件 `a.o`， 然后再执行规则 1 更新目标文件 `app`，其余的规则是不会被执行的。
>



## 2.3 自动推导

make 是一个功能强大的构建工具，虽然 make 需要根据 makefile 中指定的规则来完成源文件的编译。我们编写makefile 的时候难免写的不是那么严谨从而漏写一些构建规则，但是我们会发现程序还是会被编译成功。这是因为 make 有自动推导的能力，不会完全依赖 makefile。

比如:  使用命令 make 编译扩展名为 .c 的 C 语言文件的时候，源文件的编译规则不用明确给出。这是因为 make 进行编译的时候会使用一个默认的编译规则，按照默认规则完成对 .c 文件的编译，生成对应的 .o 文件。它使用命令 `cc -c` 来编译 .c  源文件。在 Makefile 中只要给出需要构建的目标文件名（一个.o 文件），make 会自动为这个 .o 文件寻找合适的依赖文件（对应的 .c 文件），并且使用默认的命令来构建这个目标文件。

假设本地项目目录中有以下几个源文件:

```shell
$ tree
.
├── add.c
├── div.c
├── head.h
├── main.c
├── makefile
├── mult.c
└── sub.c
```

目录中 makefile 文件类容如下：

```makefile
# 这是一个完整的 makefile 文件
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
```

通过 make 构建项目：

```shell
$ make
cc    -c -o add.o add.c
cc    -c -o div.o div.c
cc    -c -o main.o main.c
cc    -c -o mult.o mult.c
cc    -c -o sub.o sub.c
gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
```

>我们可以发现上边的 makefile 文件中只有一条规则,  依赖中所有的  `.o ` 文件在本地项目目录中是不存在的,  并且也没有其他的规则用来生成这些依赖文件,  这时候 make 会使用内部默认的构造规则先将这些依赖文件生成出来,  然后在执行规则中的命令,  最后生成目标文件 calc。
>



# 3. 变量



> 使用 Makefile 进行规则定义的时候，为了写起来更加灵活，我们可以在里边使用变量。makefile 中的变量分为三种：`自定义变量，预定义变量` 和 `自动变量`。



## 3.1 自定义变量



> 用 Makefile 进行规则定义的时候，用户可以定义自己的变量，称为用户自定义变量。makefile 中的变量是没有类型的，直接创建变量然后给其赋值就可以了。

```makefile
# 错误, 只创建了变量名, 没有赋值
变量名 
# 正确, 创建一个变量名并且给其赋值
变量名=变量值
```

在给 makefile 中的变量赋值之后,  如何在需要的时候将变量值取出来呢 ?

```makefile
# 如果将变量的值取出?
$(变量的名字)

# 举例 add.o  div.o  main.o  mult.o  sub.o
# 定义变量并赋值
obj=add.o  div.o  main.o  mult.o  sub.o
# 取变量的值
$(obj)
```

自定义变量使用举例：

```makefile
# 这是一个规则，普通写法
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
        
# 这是一个规则，里边使用了自定义变量
obj=add.o  div.o  main.o  mult.o  sub.o
target=calc
$(target):$(obj)
        gcc  $(obj) -o $(target)
```



## 3.2 预定义变量



> 在 Makefile 中有一些已经定义的变量，用户可以直接使用这些变量，不用进行定义。在进行编译的时候，某些条件下 Makefile 会使用这些预定义变量的值进行编译。这些预定义变量的名字一般都是大写的，经常采用的预定义变量如下表所示：
>

|  变量名  |             含义             |  默认值  |
| :------: | :--------------------------: | :------: |
|    AR    |  生成静态库库文件的程序名称  |    ar    |
|    AS    |       汇编编译器的名称       |    as    |
|    CC    |      C 语言编译器的名称      |    cc    |
|   CPP    |     C 语言预编译器的名称     | $(CC) -E |
|   CXX    |     C++语言编译器的名称      |   g++    |
|    FC    |   FORTRAN 语言编译器的名称   |   f77    |
|    RM    |      删除文件程序的名称      |  rm -f   |
| ARFLAGS  |  生成静态库库文件程序的选项  | 无默认值 |
| ASFLAGS  |   汇编语言编译器的编译选项   | 无默认值 |
|  CFLAGS  |    C 语言编译器的编译选项    | 无默认值 |
| CPPFLAGS |    C 语言预编译的编译选项    | 无默认值 |
| CXXFLAGS |   C++语言编译器的编译选项    | 无默认值 |
|  FFLAGS  | FORTRAN 语言编译器的编译选项 | 无默认值 |

```makefile
# 这是一个规则，普通写法
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
        
# 这是一个规则，里边使用了自定义变量和预定义变量
obj=add.o  div.o  main.o  mult.o  sub.o
target=calc
CFLAGS=-O3 # 代码优化
$(target):$(obj)
        $(CC)  $(obj) -o $(target) $(CFLAGS)
```



## 3.3 自动变量



> Makefile 中的变量除了用户自定义变量和预定义变量外，还有一类自动变量。Makefile 中的规则语句中经常会出现目标文件和依赖文件，`自动变量用来代表这些规则中的目标文件和依赖文件，并且它们只能在规则的命令中使用。`
>
> 下表中是一些常见的自动变量。
>

| 变量 | 含义                                                         |
| ---- | ------------------------------------------------------------ |
| $*   | 表示目标文件的名称，不包含目标文件的扩展名                   |
| $+   | 表示所有的依赖文件，这些依赖文件之间以空格分开，按照出现的先后为顺序，其中可能 包含重复的依赖文件 |
| $<   | 表示依赖项中第一个依赖文件的名称                             |
| $?   | 依赖项中，所有比目标文件时间戳晚的依赖文件，依赖文件之间以空格分开 |
| $@   | 表示目标文件的名称，包含文件扩展名                           |
| $^   | 依赖项中，所有不重复的依赖文件，这些文件之间以空格分开       |

```makefile
# 这是一个规则，普通写法
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
        
# 这是一个规则，里边使用了自定义变量
# 使用自动变量, 替换相关的内容
calc:add.o  div.o  main.o  mult.o  sub.o
	gcc $^ -o $@ 			# 自动变量只能在规则的命令中使用
```



# 4. 模式匹配



在介绍概念之前, 先读一下下面的这个 makefile 文件:

```makefile
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc
# 语法格式重复的规则, 将 .c -> .o, 使用的命令都是一样的 gcc *.c -c
add.o:add.c
        gcc add.c -c

div.o:div.c
        gcc div.c -c

main.o:main.c
        gcc main.c -c

sub.o:sub.c
        gcc sub.c -c

mult.o:mult.c
        gcc mult.c -c
```

在阅读过程中能够发现从第二个规则开始到第六个规则做的是相同的事情,  但是由于文件名不同不得不在文件中写出多个规则，这就让 makefile 文件看起来非常的冗余，我们可以将这一系列的相同操作整理成一个模板，所有类似的操作都通过模板去匹配 makefile 会因此而精简不少，只是可读性会有所下降。

这个规则模板可以写成下边的样子，这种操作就称之为模式匹配。

```makefile
# 模式匹配 -> 通过一个公式, 代表若干个满足条件的规则
# 依赖有一个, 后缀为.c, 生成的目标是一个 .o 的文件, % 是一个通配符, 匹配的是文件名
%.o:%.c
	gcc $< -c
```



![image-20230911155017021](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309111550082.png)



# 5. 函数


> makefile 中有很多函数并且 `所有的函数都是有返回值的`。makefile 中函数的格式和 C/C++ 中函数也不同，其写法是这样的： `$(函数名 参数1, 参数2, 参数3, ...)`，主要目的是让我们能够快速方便的得到函数的返回值。
>
> 这里介绍两个 makefile 中使用频率比较高的函数：`wildcard`和 `patsubst`。
>



## 5.1 wildcard

这个函数的主要作用是获取指定目录下指定类型的文件名，其返回值是以空格分割的、指定目录下的所有符合条件的文件名列表。函数原型如下：

```makefile
# 该函数的参数只有一个, 但是这个参数可以分成若干个部分, 通过空格间隔
$(wildcard PATTERN...)
	参数:	指定某个目录, 搜索这个路径下指定类型的文件，比如： *.c
```

* 参数功能：
  * PATTERN 指的是某个或多个目录下的对应的某种类型的文件, 比如 `当前目录下的.c` 文件可以写成  `*.c`
  * 可以指定多个目录，每个路径之间使用空格间隔
* 返回值：
  * 得到的若干个文件的文件列表， 文件名之间使用空格间隔
  * 示例：`$(wildcard *.c ./sub/*.c)`
    * 返回值格式: **a.c b.c c.c d.c e.c f.c ./sub/aa.c ./sub/bb.c**

```makefile
# 使用举例: 分别搜索三个不同目录下的 .c 格式的源文件
src = $(wildcard /home/robin/a/*.c /home/robin/b/*.c *.c)  # *.c == ./*.c
# 返回值: 得到一个大的字符串, 里边有若干个满足条件的文件名, 文件名之间使用空格间隔
/home/robin/a/a.c /home/robin/a/b.c /home/robin/b/c.c /home/robin/b/d.c e.c f.c
```



## 5.2 patsubst

这个函数的功能是按照指定的模式替换指定的文件名的后缀, 函数原型如下:

```makefile
# 有三个参数, 参数之间使用 逗号间隔
$(patsubst <pattern>,<replacement>,<text>)
```

* 参数功能:
  * pattern： 这是一个模式字符串, 需要指定出要被替换的文件名中的后缀是什么
    * 文件名和路径不需要关心, 因此使用 % 表示即可 [通配符是 %]
    * 在通配符后边指定出要被替换的后缀,  比如:  %.c, 意味着 .c 的后缀要被替换掉
  * replacement：这是一个模式字符串, 指定参数 pattern 中的后缀最终要被替换为什么
    * 还是使用 % 来表示参数pattern 中文件的路径和名字
    * 在通配符 % 后边指定出新的后缀名, 比如:  %.o 这表示原来的后缀被替换为 .o
  * text：该参数中存储这要被替换的原始数据
  * 返回值：
    * 函数返回被替换过后的字符串

```makefile
src = a.cpp b.cpp c.cpp e.cpp
# 把变量 src 中的所有文件名的后缀从 .cpp 替换为 .o
obj = $(patsubst %.cpp, %.o, $(src)) 
# obj 的值为: a.o b.o c.o e.o
```



# 6. makefile 的编写

```shell
# 项目目录结构
.
├── add.c
├── div.c
├── head.h
├── main.c
├── mult.c
└── sub.c
# 需要编写makefile对该项目进行自动化编译
```



## 6.1 版本 1

```makefile
calc:add.c  div.c  main.c  mult.c  sub.c
        gcc add.c  div.c  main.c  mult.c  sub.c -o calc
```

> 这个版本的优点：书写简单
>
> 这版本的缺点：只要依赖中的某一个源文件被修改，所有的源文件都需要被重新编译，太耗时、效率低
>
> 改进方式：提高效率，修改哪一个源文件, 哪个源文件被重新编译, 不修改就不重新编译
>



## 6.2 版本 2

```makefile
# 默认所有的依赖都不存在, 需要使用其他规则生成这些依赖
# 因为 add.o 被更新, 需要使用最新的依赖, 生成最新的目标
calc:add.o  div.o  main.o  mult.o  sub.o
        gcc  add.o  div.o  main.o  mult.o  sub.o -o calc

# 如果修改了add.c, add.o 被重新生成
add.o:add.c
        gcc add.c -c

div.o:div.c
        gcc div.c -c

main.o:main.c
        gcc main.c -c

sub.o:sub.c
        gcc sub.c -c

mult.o:mult.c
        gcc mult.c -c
```

>这个版本的优点：相较于版本 1 效率提升了
>
>这个版本的缺点：规则比较冗余, 需要精简
>
>改进方式：在 makefile 中使用**变量** 和 **模式匹配**



## 6.3 版本 3

```makefile
# 添加自定义变量 -> makefile中注释前 使用 # 
obj=add.o  div.o  main.o  mult.o  sub.o
target=calc

$(target):$(obj)
        gcc $(obj)  -o $(target)

%.o:%.c
        gcc $< -c
```

> 这个版本的优点：文件精简不少，变得简洁了
>
> 这个版本的缺点：变量 obj 的值需要手动的写出来, 如果需要编译的项目文件很多，都用手写出来不现实
>
> 改进方式：在makefile中使用**函数**



## 6.4 版本 4

```makefile
# 添加自定义变量 -> makefile中注释前 使用 # 
# 使用函数搜索当前目录下的源文件 .c
src=$(wildcard *.c)
# 将源文件的后缀替换为 .o
# % 匹配的内容是不能被替换的, 需要替换的是第一个参数中的后缀, 替换为第二个参数中指定的后缀
# obj=$(patsubst %.cpp, %.o, $(src)) 将src中的关键字 .cpp 替换为 .o
obj=$(patsubst %.c, %.o, $(src))
target=calc

$(target):$(obj)
        gcc $(obj)  -o $(target)

%.o:%.c
        gcc $< -c
```

> 这个版本的优点：解决了自动加载项目文件的问题，解放了双手
>
> 这个版本的缺点：没有文件删除的功能，不能删除项目编译过程中生成的目标文件（*.o）和可执行程序
>
> 改进方式: 在 makefile 文件中添加新的规则用于删除生成的目标文件（*.o）和可执行程序
>



## 6.5 版本 5

```makefile
# 添加自定义变量 -> makefile中注释前 使用 # 
# 使用函数搜索当前目录下的源文件 .c
src=$(wildcard *.c)
# 将源文件的后缀替换为 .o
obj=$(patsubst %.c, %.o, $(src))
target=calc
# obj 的值 xxx.o xxx.o xxx.o xx.o
$(target):$(obj)
        gcc $(obj)  -o $(target)

%.o:%.c
        gcc $< -c

# 添加规则, 删除生成文件 *.o 可执行程序
# 这个规则比较特殊, clean根本不会生成, 这是一个伪目标
clean:
        rm $(obj) $(target)
```

> 这个版本的优点: 添加了新的规则（16行）用于文件的删除, 直接 `make clean` 就可以执行规则中的删除命令了
>
> 这个版本的缺点: 在下面有具体的问题演示和分析
>
> 改进方式: 在 makefile 文件中声明 `clean` 是一个伪目标，让 make 放弃对它的时间戳检测
>



正常情况下这个版本的 makefile 是可以正常工作的，但是我们如果在这个项目目录中添加一个叫做 `clean` 的文件（和规则中的目标名称相同），再进行 `make clean` 发现这个规则就不能正常工作了。

```makefile
# 在项目目录中添加一个叫 clean的文件, 然后在 make clean 这个规则中的命令就不工作了
$ ls
add.c  calc   div.c  head.h  main.o    mult.c  sub.c
add.o  div.o  main.c  makefile  mult.o  sub.o  clean  ---> 新添加的

# 使用 makefile 中的规则删除生成的目标文件和可执行程序
$ make clean
make: 'clean' is up to date. 

# 查看目录, 发现相关文件并没有被删除, make clean 失败了
$ ls
add.c  calc   div.c  head.h  main.o    mult.c  sub.c
add.o  clean  div.o  main.c  makefile  mult.o  sub.o
```

> 这个问题的关键点在于 `clean` 是一个伪目标,  不对应任何实体文件,  在前边讲 `关于文件时间戳更新 `问题的时候说过，如果目标不存在规则的命令肯定被执行， 如果目标文件存在了就需要比较规则中目标文件和依赖文件的时间戳，满足条件才执行规则的命令，否则不执行。
>
> 解决这个问题需要在 makefile 中声明 `clean` 是一个伪目标，这样 make 就不会对文件的时间戳进行检测，规则中的命令也就每次都会被执行了。
>
> 在 makefile 中声明一个伪目标需要使用  `.PHONY` 关键字,  声明方式为: ` .PHONY:伪文件名称`
>



## 6.6 最终版

```makefile
# 添加自定义变量 -> makefile中注释前 使用 # 
# 使用函数搜索当前目录下的源文件 .c
src=$(wildcard *.c)
# 将源文件的后缀替换为 .o
obj=$(patsubst %.c, %.o, $(src))
target=calc

$(target):$(obj)
        gcc $(obj)  -o $(target)

%.o:%.c
        gcc $< -c

# 添加规则, 删除生成文件 *.o 可执行程序
# 声明clean为伪文件
.PHONY:clean
clean:
        # shell命令前的 - 表示强制这个指令执行, 如果执行失败也不会终止
        -rm $(obj) $(target) 
        echo "hello, 我是测试字符串"
```



# 7. makefile 项目练习



```shell
# 目录结构
.
├── include
│   └── head.h	==> 头文件, 声明了加减乘除四个函数
├── main.c		==> 测试程序, 调用了head.h中的函数
└── src
    ├── add.c	==> 加法运算
    ├── div.c	==> 除法运算
    ├── mult.c  ==> 乘法运算
    └── sub.c   ==> 减法运算
```



```makefile
# 最终的目标名 app
target = app
# 搜索当前项目目录下的源文件
src=$(wildcard *.c ./src/*.c)
# 将文件的后缀替换掉 .c -> .o
obj=$(patsubst %.c, %.o, $(src))
# 头文件目录
include=./include

# 第一条规则
# 依赖中都是 xx.o yy.o zz.o
# gcc命令执行的是链接操作
$(target):$(obj)
        gcc $^ -o $@

# 模式匹配规则
# 执行汇编操作, 前两步: 预处理, 编译是自动完成
%.o:%.c
        gcc $< -c -I $(include) -o $@

# 添加一个清除文件的规则
.PHONY:clean

clean:
        -rm $(obj) $(target) -f
```



