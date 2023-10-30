

JSON(`JavaScrip Object Notation`)是`一种轻量级的数据交换格式`。它基于 ECMAScript (欧洲计算机协会制定的js规范)的一个子集，`采用完全独立于编程语言的文本格式来存储和表示数据`。简洁和清晰的层次结构使得 JSON 成为理想的数据交换语言。 易于人阅读和编写，同时也易于机器解析和生成，并有效地提升网络传输效率。

关于上面的描述可以精简为一句话：**Json 是一种数据格式，和语言无关，在什么语言中都可以使用 Json。**基于这种通用的数据格式，一般处理两方面的任务：

1. 组织数据（数据序列化），用于数据的网络传输
2. 组织数据（数据序列化），写磁盘文件实现数据的持久化存储（一般以 `.json` 作为文件后缀）

Json 中主要有两种数据格式：Json 数组和 Json 对象，并且这两种格式可以交叉嵌套使用，下面依次介绍下这两种数据格式：



# 1. Json 数组



Json 数组使用 [] 表示，[] 里边是元素，元素和元素之间使用逗号间隔，`最后一个元素后边没有逗号`，一个 Json 数组中支持同时存在多种不同类型的成员，包括：`整形、 浮点、 字符串、 布尔类型、 json数组、 json对象、 空值-null`。由此可见 Json 数组比起 C/C++ 数组要灵活很多。

* Json 数组中的元素数据类型一致

```json
// 整形
[1,2,3,4,5]
// 字符串
["luffy", "sanji", "zoro", "nami", "robin"]
```

* Json 数组中的元素数据类型不一致

```json
[12, 13.34, true, false, "hello,world", null]
```

* Json 数组中的数组嵌套使用

```json
[
    ["cat", "dog", "panda", "beer", "rabbit"],
    ["北京", "上海", "天津", "重庆"],
    ["luffy", "boy", 19]
]
```

* Json数组和对象嵌套使用

```json
[
    {
        "luffy":{
            "age":19,
            "father":"Monkey·D·Dragon",
            "grandpa":"Monkey D Garp",
            "brother1":"Portgas D Ace",
            "brother2":"Sabo"
        }
    }
]
```



# 2. Json 对象



Json 对象使用 {} 来描述，每个 Json 对象中可以存储若干个元素，每一个元素对应一个键值对（key：value 结构），元素和元素之间使用逗号间隔，最后一个元素后边没有逗号。对于每个元素中的键值对有以下细节需要注意：

1. 键值（key）必须是字符串，位于同一层级的键值不要重复（因为是通过键值取出对应的 value 值）

2. value 值的类型是可选的，可根据实际需求指定，可用类型包括：`整形、 浮点、 字符串、 布尔类型、 json数组、 json对象、 空值-null。`

**使用 Json 对象描述一个人的信息:**

```json
{
    "Name":"Ace",
    "Sex":"man",
    "Age":20,
    "Family":{
        "Father":"Gol·D·Roger",
        "Mother":"Portgas·D·Rouge",
        "Brother":["Sabo", "Monkey D. Luffy"]
    },
    "IsAlive":false,
    "Comment":"yyds"
}
```



# 3. 注意事项



通过上面的介绍可用看到，Json 的结构虽然简单，但是进行嵌套之后就可以描述很复杂的事情，在项目开发过程中往往需要我们根据实际需求自己定义 Json 格式用来存储项目数据。

另外，如果需要将 Json 数据持久化到磁盘文件中，需要注意一个问题：**在一个 Json 文件中只能有一个 Json 数组或者Json 对象的根节点，不允许同时存储多个并列的根节点**。下面举例说明：

* **错误的写法**

```json
// test.json
{
    "name":"luffy",
    "age":19
}
{
    "user":"ace",
    "passwd":"123456"
}
```

错误原因：**在一个 Json 文件中有两个并列的 Json 根节点（并列包含 Json 对象和 Json 对象、Json 对象和Json数组、Json 数组和 Json 数组），根节点只能有一个。**

* **正确的写法**

```json
// test.json
{
    "Name":"Ace",
    "Sex":"man",
    "Age":20,
    "Family":{
        "Father":"Gol·D·Roger",
        "Mother":"Portgas·D·Rouge",
        "Brother":["Sabo", "Monkey D. Luffy"]
    },
    "IsAlive":false,
    "Comment":"yyds"
}
```

`在上面的例子中通过 Json 对象以及 Json 数组的嵌套描述了一个人的身份信息，并且根节点只有一个就是 Json对象，如果还需要使用 Json 数组或者 Json 对象描述其他信息，需要将这些信息写入到其他文件中，不要和这个 Json 对象并列写入到同一个文件里边，切记！！！`