
[[Json]]

[cJSON源码及解析流程详解-CSDN博客](https://blog.csdn.net/qq_38289815/article/details/103307262)

[【万字详解】cJSON解析-CSDN博客](https://blog.csdn.net/HouGOD/article/details/124038350)

cJSON 是一个超轻巧，携带方便，单文件，简单的可以作为 ANSI-C 标准的 JSON 解析器。

cJSON 是一个开源项目，github下载地址：https://github.com/DaveGamble/cJSON

cJSON，目前来说，主要的文件有两个，一个 `cJSON.c`  一个 `cJSON.h`。使用的时候，将头文件 include 进去即可。

如果是在 Linux 操作系统中使用，编译 到时候需要添加数学库 `libm.so`，如下所示：

```shell
gcc *.c cJSON.c -lm
```



# 1. cJSON 结构体



在 `cJSON.h` 中定义了一个非常重要的结构体 `cJSON`，想要熟悉使用 `cJSON` 库函数可从 `cJSON` 结构体入手，`cJSON` 结构体如下所示：

```c
typedef struct cJSON {  
     struct cJSON *next,*prev;   
     struct cJSON *child;   
     int type;   
     char *valuestring;        // value值是字符串类型
     int valueint;  
     double valuedouble;   
     char *string;             // 对象中的key
} cJSON; 
```

关于这个结构体做如下几点的说明:

1. `cJOSN` 结构体是一个双向链表，并且可通过 `child` 指针访问下一层。
2. 结构体成员 `type` 变量用于描述数据元素的类型（如果是键值对表示 `value` 值的类型），数据元素可以是字符串可以是整形，也可以是浮点型。

   * 如果是整形值的话可通过 `valueint` 将值取出
   * 如果是浮点型的话可通过 `valuedouble` 将值取出
   * 如果是字符串类型的话可通过 `valuestring` 将值取出
3. 结构体成员 `string` 表示键值对中键值的名称。

`cJSON` 作为 `Json` 格式的解析库，其主要功能就是构建和解析 `Json` 格式了，比如要发送数据：用途就是发送端将要发送的数据以 `json` 形式封装，然后发送，接收端收到此数据后，还是按 `json` 形式解析，就得到想要的数据了。



# 2. cJSON API



`Json` 格式的数据无外乎有两种 `Json 对象`和 `Json 数组`，创建的 Json 数据串可能是二者中 的一种，也可能是二者的组合，不管哪一种通过调用相关的 API 函数都可以轻松的做到这一点。



## 2.1 数据的封装

在 `cJSON.h` 头文件中可以看到一些函数声明，通过调用这些创建函数就可以将 Json 支持的数据类型封装为`cJSON` 结构体类型：

```c
// 空值类型
extern cJSON *cJSON_CreateNull(void);
// 布尔类型
extern cJSON *cJSON_CreateTrue(void);
extern cJSON *cJSON_CreateFalse(void);
extern cJSON *cJSON_CreateBool(int b);
// 数值类型
extern cJSON *cJSON_CreateNumber(double num);
// 字符串类型
extern cJSON *cJSON_CreateString(const char *string);
// json数组(创建空数组)
extern cJSON *cJSON_CreateArray(void);
// json对象(创建空对象)
extern cJSON *cJSON_CreateObject(void);
```

另外，cJson 库中还给我我们提供了一些更为简便的操作函数，在创建数组的同时还可以进行初始化

```c
// 创建一个Json数组, 元素为整形
extern cJSON *cJSON_CreateIntArray(const int *numbers,int count);
// 创建一个Json数组, 元素为浮点
extern cJSON *cJSON_CreateFloatArray(const float *numbers,int count);
extern cJSON *cJSON_CreateDoubleArray(const double *numbers,int count);
// 创建一个Json数组, 元素为字符串类型
extern cJSON *cJSON_CreateStringArray(const char **strings,int count);
```



## 2.2 Json 对象操作

当得到一个 Json 对象之后，就可以往对象中添加键值对了，可以使用 `cJSON_AddItemToObject()`

```c
extern void cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item);
```

在 `cJSON` 库中节点的从属关系是通过树来维护的，每一层节点都是通过链表来维护的，这样就能分析出该函数参数的含义：

- object：要添加的键值对从属于那个节点
- string：添加的键值对的键值
- item：添加的键值对的value值（需要先将其封装为cJSON类型的结构体）

为了让我的操作更加方便，cJson 库还给我们提供了一些宏函数，方便我们快速的往 Json 对象中添加键值对

```c
#define cJSON_AddNullToObject(object,name)      cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)      cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)     cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object,name,b)    cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object,name,n)  cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)  cJSON_AddItemToObject(object, name, cJSON_CreateString(s))
```

我们还可以根据 Json 对象中的键值取出相应的 value 值，API 函数原型如下:

```c
extern cJSON *cJSON_GetObjectItem(cJSON *object,const char *string);
```



## 2.3 Json 数组操作

添加数据到 Json 数组中（原始数据需要先转换为 cJSON 结构体类型）

```c
extern void cJSON_AddItemToArray(cJSON *array, cJSON *item);
```

得到 Json 数组中元素的个数:

```c
extern int cJSON_GetArraySize(cJSON *array);
```

得到 Json 数组中指定位置的原素，如果返回 `NULL` 表示取值失败了。

```c
extern cJSON *cJSON_GetArrayItem(cJSON *array,int item);
```



## 2.4 序列化

序列化就是将 Json 格式的数据转换为字符串的过程，cJson 库中给我们提供了 3 个转换函数，具体如下：

第一个参数 `item` 表示 Json 数据块的根节点。

```c
extern char  *cJSON_Print(cJSON *item);
extern char  *cJSON_PrintUnformatted(cJSON *item);
extern char *cJSON_PrintBuffered(cJSON *item,int prebuffer,int fmt);
```

- 调用 `cJSON_Print()` 函数我们可以得到一个带格式的 Json 字符串（有换行，看起来更直观）

- 调用 `cJSON_PrintUnformatted()` 函数会得到一个没有格式的 Json 字符串（没有换行，所有的数据都在同一行）。
- 调用 `cJSON_PrintBuffered()` 函数使用缓冲策略将 Json 实体转换为字符串，参数 `prebuffer` 是指定缓冲区的大小，参数 `fmt==0` 表示未格式化，`fmt==1` 表示格式化。

我们在编码过程中可以根据自己的实际需求调用相关的操作函数得到对应格式的 Json 字符串。



## 2.5 Json 字符串解析

如果我们得到了一个 Json 格式的字符串，想要读出里边的数据，就需要对这个字符串进行解析，处理方式就是将字符串转换为 cJSON 结构体，然后再基于这个结构体读里边的原始数据，转换函数的函数原型如下：

```c
extern cJSON *cJSON_Parse(const char *value);
```



## 2.6 内存释放

当我们将数据封装为 `cJSON` 结构类型的节点之后都会得到一块堆内存，当我们释放某个节点的时候可以调用cJson 库提供的删除函数 `cJSON_Delete()`，函数原型如下：

```c
extern void   cJSON_Delete(cJSON *c);
```

该函数的参数为要释放的节点的地址，在此强调一点：`在进行内存地址释放的时候，当前节点以及其子节点都会被删除。`



# 3. Json 数据的封装



## 3.1 Json 对象操作举例

> 创建一个对象，并向这个对象里添加字符串和整型键值：

```c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"cJSON.h"
 
int main()
{
    cJSON * root;
    cJSON *arry;

    root=cJSON_CreateObject();                     // 创建根数据对象
    cJSON_AddStringToObject(root,"name","luffy");  // 添加键值对
    cJSON_AddStringToObject(root,"sex","man");     // 添加键值对
    cJSON_AddNumberToObject(root,"age",19);        // 添加键值对

    char *out = cJSON_Print(root);   // 将json形式转换成字符串
    printf("%s\n",out);

    // 释放内存  
    cJSON_Delete(root);  
    free(out);        
}
```

运行结果

```json
{
	"name":	"luffy",
	"sex":	"man",
	"age":	19
}
```

若干说明:

1. `cJSON_CreateObject` 函数可创建一个根对象，返回的是一个 `cJSON` 指针，在这个指针用完了以后，需要手动调用 `cJSON_Delete(root)` 进行内存回收。
2. 函数 `cJSON_Print()` 内部封装了 `malloc` 函数，所以需要使用 `free()` 函数释放被 `out` 占用的内存空间。



## 3.2 Json 数组操作举例

> 创建一个数组，并向数组添加一个字符串和一个数字

```c
int main(int argc, char **argv)
{
    cJSON *root;
    root = cJSON_CreateArray();
    cJSON_AddItemToArray(root, cJSON_CreateString("Hello world"));
    cJSON_AddItemToArray(root, cJSON_CreateNumber(10)); 
    // char *s = cJSON_Print(root);
    char *s = cJSON_PrintUnformatted(root);
    if(s)
    {
        printf(" %s \n",s);
        free(s);
    }
    cJSON_Delete(root);
    return 0;
}
```

运行结果:

```json
["Hello world",10]
```



## 3.3 Json 对象、数组嵌套使用

> 对象里面包括一个数组，数组里面包括对象，对象里面再添加一个字符串和一个数字

```json
{
    "person":[{
        "name":"luffy",
        "age":19
    }]
}
```

示例代码:

```c
int main(int argc, char **argv)
{
    cJSON *root, *body, *list;
    // josn 对象 root
    root = cJSON_CreateObject();
    // root 添加键值对 person:json数组A
    cJSON_AddItemToObject(root,"person", body = cJSON_CreateArray());
    // json数组A 添加Json对象B
    cJSON_AddItemToArray(body, list = cJSON_CreateObject());
    // 在json对象B中添加键值对: "name":"luffy"
    cJSON_AddStringToObject(list,"name","luffy");
    // 在json对象B中添加键值对: "age":19
    cJSON_AddNumberToObject(list,"age",19);
 
    // char *s = cJSON_Print(root);
    char *s = cJSON_PrintUnformatted(root);
    if(s)
    {
        printf(" %s \n",s);
        free(s);
    }
    if(root)
    {
        cJSON_Delete(root); 
    }
    return 0;
}
```

运行结果:

```json
{"person":[{"name":"luffy","age":19}]}
```



# 4. 解析 Json 字符串



## 4.1 解析 Json 对象

Json 字符串的解析流程和数据的封装流程相反，假设我们有这样一个 Json 字符串（字符串中的双引号需要通过转义字符将其转译为普通字符）：

```json
{\"name\":\"luffy\",\"sex\":\"man\",\"age\":19}
```

示例代码如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
 
int main()
{
    cJSON *json, *name, *sex, *age;
    char* out="{\"name\":\"luffy\",\"sex\":\"man\",\"age\":19}";
 
    json = cJSON_Parse(out); //解析成json形式
    name = cJSON_GetObjectItem(json, "name");  //获取键值内容
    sex = cJSON_GetObjectItem(json, "sex");
    age = cJSON_GetObjectItem(json, "age");
 
    printf("name:%s,sex:%s,age:%d\n", name->valuestring, sex->valuestring, age->valueint);
 
    cJSON_Delete(json);  //释放内存 
}
```

输出的结果:

```c
name:luffy,sex:man,age:19
```

> **如果是在严格的场所，应该先判定每个 item 的 type，然后再考虑去取值。**



## 4.2 解析嵌套的 Json 对象

加大一点难度，下面我们解析一个嵌套的 Json 对象，数据如下：

```c
int main()
{
    char *s = "{\"list\":{\"name\":\"luffy\",\"age\":19},\"other\":{\"name\":\"ace\"}}";
    cJSON *root = cJSON_Parse(s);
    if(!root) 
    {
        printf("get root faild !\n");
        return -1;
    }

    cJSON *js_list = cJSON_GetObjectItem(root, "list");
    if(!js_list) 
    {
        printf("no list!\n");
        return -1;
    }
    printf("list type is %d\n",js_list->type);

    cJSON *name = cJSON_GetObjectItem(js_list, "name");
    if(!name) 
    {
        printf("No name !\n");
        return -1;
    }
    printf("name type is %d\n",name->type);
    printf("name is %s\n",name->valuestring);

    cJSON *age = cJSON_GetObjectItem(js_list, "age");
    if(!age) 
    {
        printf("no age!\n");
        return -1;
    }
    printf("age type is %d\n", age->type);
    printf("age is %d\n",age->valueint);

    cJSON *js_other = cJSON_GetObjectItem(root, "other");
    if(!js_other) 
    {
        printf("no list!\n");
        return -1;
    }
    printf("list type is %d\n",js_other->type);

    cJSON *js_name = cJSON_GetObjectItem(js_other, "name");
    if(!js_name) 
    {
        printf("No name !\n");
        return -1;
    }
    printf("name type is %d\n",js_name->type);
    printf("name is %s\n",js_name->valuestring);

    if(root)
    {
        cJSON_Delete(root);
    }
    return 0;
}
```

打印结果:

```shell
list type is 6
name type is 4
name is luffy
age type is 3
age is 19
list type is 6
name type is 4
name is ace
```



## 4.3 解析 Json 数组

如果我们遇到的 Json 字符串是一个 Json 数组格式，处理方式和 Json 对象差不多，比如我们要解析如下字符串：

```json
{\"names\":[\"luffy\",\"robin\"]}
```

```c

int main(int argc, char **argv)
{
    char *s = "{\"names\":[\"luffy\",\"robin\"]}";
    cJSON *root = cJSON_Parse(s);
    if(!root) 
    {
        printf("get root faild !\n");
        return -1;
    }
    cJSON *js_list = cJSON_GetObjectItem(root, "names");
    if(!js_list)
    {
        printf("no list!\n");
        return -1;
    }
    int array_size = cJSON_GetArraySize(js_list);
    printf("array size is %d\n",array_size);
    for(int i=0; i< array_size; i++) 
    {
        cJSON *item = cJSON_GetArrayItem(js_list, i);
        printf("item type is %d\n",item->type);
        printf("%s\n",item->valuestring);
    }

    if(root)
    {
        cJSON_Delete(root);
    }
    return 0;
}
```



## 4.4 解析嵌套的 Json 对象和数组

对于 Json 字符串最复杂的个数莫过于 Json 对象和 Json 数组嵌套的形式，下面通过一个例子演示一下应该如何解析，字符串格式如下：

```json
{\"list\":[{\"name\":\"luffy\",\"age\":19},{\"name\":\"sabo\",\"age\":21}]}
```

在解析的时候，我们只需要按照从属关系，一层层解析即可：

1. 根节点是一个Json 对象，基于根节点中的 key 值取出对应的 value 值，得到一个 Json 数组

2. 读出 Json 数组的大小，遍历里边的各个元素，每个元素都是一个 Json 对象
3. 将 Json 对象中的键值对根据 key 值取出对应的 value 值
4. 从取出的 Value 值中读出实际类型对应的数值

**示例代码如下：**

```c
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *s = "{\"list\":[{\"name\":\"luffy\",\"age\":19},{\"name\":\"sabo\",\"age\":21}]}";
    cJSON *root = cJSON_Parse(s);
    if(!root) 
    {
        printf("get root faild !\n");
        return -1;
    }
    cJSON *list = cJSON_GetObjectItem(root, "list");
    if(!list)
    {
        printf("no list!\n");
        return -1;
    }
    int array_size = cJSON_GetArraySize(list);
    printf("array size is %d\n",array_size);
    
    for(int i=0; i< array_size; i++) 
    {
        cJSON* item = cJSON_GetArrayItem(list, i);
        cJSON* name = cJSON_GetObjectItem(item, "name");
        printf("name is %s\n",name->valuestring);
        cJSON* age = cJSON_GetObjectItem(item, "age");
        printf("age is %d\n",age->valueint);
    }

    if(root)
    {
        cJSON_Delete(root);
    }
    return 0;
}
```

