# 1.变量和基本类型

> C++ 是一门静态类型语言，在编译时会作类型检查。
>
> 在大多数语言中，对象的类型限制了对象可以执行的操作。如果某种类型不支持某种操作，那么这种类型的对象也就不能执行该操作。
>
> 在 C++ 中，操作是否合法是在编译时检查的。随着程序和使用的类型变得越来越复杂，我们将看到静态类型检查能帮助我们更早地发现错误。静态类型检查使得编译器必须能识别程序中的每个实体的类型。因此，程序中使用变量前必须先定义变量的类型。

## 1.1 变量

**左值和右值**   [[右值引用及移动构造]]

1. **左值**：左值可以出现在赋值语句的左边或右边。
2. **右值**：右值只能出现在赋值的右边，不能出现在赋值语句的左边。

变量是左值，因此可以出现在赋值语句的左边。数字字面值是右值，因此不能被赋值。



## 1.2 声明和定义

变量的**定义**用于为变量分配存储空间，还可以为变量指定初始值。在一个程序中，变量有且仅有一个定义。

**声明** 用于向程序表明变量的类型和名字。定义也是声明：当定义变量时我们声明了它的类型和名字。可以通过使用 **extern** 关键字声明变量名而不定义它。

```c++
extern int i;
int i;
```

**extern** 声明不是定义，也不分配存储空间。事实上，它只是说明变量定义在程序的其他地方。程序中变量可以声明多次，但只能定义一次。

> 通常把一个对象定义在它首次使用的地方是一个很好的方法。



## 1.3 引用

**引用（reference)** 就是对象的另一个名字。在实际程序中，引用主要用作函数的形式参数。

### 1.3.1 引用是别名

因为引用只是它绑定的对象的另一个名字，作用在引用上的所有操作事实上都是作用在该引用绑定的对象上

> 当引用初始化后，只要该引用存在，它就保持绑定到初始化时指向的对象。不可能将引用绑定到另一个对象。
>
> 要理解的重要概念是 *引用只是对象的另一个名字。*
>
> 初始化是指明引用指向哪一个对象的唯一方法。

### 1.3.2 const 引用

const 引用是指向 const 对象的引用：

```c++
const int ival = 1024;
const int &refVal = ival;
int &ref2 = ival;     // error:non const reference to a const object
```

可以读取但不能修改 refVal，因此，任何对 refVal 的赋值都是不合法的。

> “const 引用" 的意思是 “ 指向 const 对象的引用 ”。类似地，“非 const 引用” 表示指向非 const 类型的引用。



## 1.4 struct 关键字

C++支持另一个关键字 struct，它也可以定义类类型。struct 关键字是从 C 语言中继承过来的。

**如果使用 class 关键字来定义类，那么定义在第一个访问标号前的任何成员都隐式指定为 private; 如果使用 struct 关键字，那么这些成员都是 public。使用 class 还是 struct 关键字来定义类，仅仅影响默认的初始访问级别。**

---



# 2. 标准库类型

## 2.1 标准库 string 类型

string 类型支持长度可变的字符串，C++ 标准库将负责管理与存储字符相关的内存，以及提供各种有用的操作。

```c++
string s1;            //默认构造函数，s1 为空串
string s2(s1);        //将 s2 初始化为 S1 的副本
string s3("value");   //将 s3 初始化为一个字符串字面值副本
string s4(n, 'c');    //将 s4 初始化为字符 'c' 的 n 个副本
```

> 因为历史原因以及为了与 C 语言兼容，字符串字面值与标准库 string 类型不是同一种类型。C 语言字符串是 const char* 类型，以 '\0' 结尾，而 string 的末尾是没有 '\0' 的。

> 任何存储 string 的 size 操作结果的变量必须为 string::size_type 类型。特别重要的是，不要把 size 的返回值赋给一个 int 变量。其实是一个无符号整型的，因为大小只能是正数。



## 2.2 标准库 vector 类型

vector 是同一种类型的对象的集合，每个对象都有一个对应的整数索引值。我们把 vector 称为**容器**，是因为它可以包含其他对象。一个容器中的所有对象都必须是同一种类型的。

vector 是一个 **类模板**。 使用模板可以编写一个类定义或者函数定义，而用于多个不同的数据类型。

```c++
#include <vector>
using std::vector;

vector<int> ivec;
vector<Sales_item> Sales_vec;
```

定义 vector 对象要指定类型和一个变量的列表。

> vector 对象（以及其他标准库容器对象）的重要属性就在于可以在运行时高效地添加元素。因为 vector 增长的效率高，在元素值已知的情况下，最好是动态地添加元素。

### 2.2.1 vector 对象的操作

| v.empty()      | 如果 v 为空，则返回 true，否则返回 false |
| -------------- | ---------------------------------------- |
| v.size()       | 返回 v 中元素的个数                      |
| v.push_back(t) | 在 v 的末尾增加一个值为 t 的元素         |
| v[n]           | 返回 v 中位置为 n 的元素                 |

使用 size_type 类型时，必须指出该类型是在哪里定义的。vector 类型总是包括 vector 的元素类型：

```C++
vector<int>::size_type    //ok
vector::size_type         //error
```

vector 中的对象是没有命名的，可以按 vector 中对象的位置来访问它们。通常使用下标操作来获取元素。vector 的下标操作类似于 string 类型的下标操作。

vector 的下标操作符接受一个值，并返回 vector 中该对应位置的元素。vector 中元素的位置从 0 开始。

```C++
for(vector<int>::size_type ix = 0; ix != ivec.size(); ++ix){
    ivec[ix] = 0;
}
```

> for 循环的判断条件用 != 而不是用 < 来测试 vector 下标值是否越界。C++ 程序员习惯于优先使用 != 而不是 < 来编写循环判断条件。

### 2.2.2 迭代器简介

迭代器是一种检查容器内元素并遍历元素的数据类型。

标准库为每一种标准容器（包括 vector）定义了一种迭代器类型。迭代器类型提供了比下标操作更通用的方法：所有的标准库容器都定义了相应的迭代器类型，而只有少数的容器支持下标操作。

**由 end() 操作返回的迭代器并不指向 vector 中任何实际的元素，相反，它只是起一个哨兵（sentinel）的作用，表示我们已经处理完了 vector 中的所有元素。**

迭代器实质是一个指针，可以用解引用操作符来访问迭代器所指向的元素。

```C++
for(vector<int>::iterator iter = ivec.begin(); iter != ivec.end(); ++iter){
    *iter = 0;
}
```

**const_iterator**

每种容器类型还定义了一种名为 const_iterator 的类型，该类型只能用于读取容器内元素，但不能改变其值。

```C++
for(vector<int>::const_iterator iter = ivec.begin(); iter != ivec.end(); ++iter){
    cout << *iter << endl;
}
```

```C++
/** 编写程序来创建有 10 个元素的 vector 对象。用迭代器把每个元素值改为当前值的 2 倍。**/
#include <iostream>
#include <vector>
using namespace std;

int main(){

    vector<int> vec(10,3);
    for(vector<int>::iterator iter = vec.begin(); iter != vec.end(); ++iter){
        *iter = *iter * 2;
    }
    //输出
    for(vector<int>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter){
        cout << *iter << endl;
    }
    return 0;
}
[Running] cd "d:\CPlusPlus\vsCode\TEST\DEMO2\" && g++ demo3.cpp -o demo3 && "d:\CPlusPlus\vsCode\TEST\DEMO2\"demo3
6
6
6
6
6
6
6
6
6
6
```

**任何改变 vector 长度的操作都会使已存在的迭代器失效。**

---



# 3. 数组和指针

数组没有获取其容量大小的 size 操作，也不提供 push_back 操作在其中自动添加元素。**如果需要更改数组的长度，程序员只能创建一个更大的新数组，然后把原数组的所有数据复制到新数组空间中去。**

## 3.1 数组操作

数组下标的正确类型是 size_t 类型。

```C++
int main(){
    const size_t array_size = 10;
    int ia[array_size];
    
    for(size_t ix = 0; ix != array_size; ++ix)
        ia[ix] = ix;
    return 0;
}
```



## 3.2 指针

指针用于指向对象。与迭代器一样，指针提供对其所指对象的间接访问，只是指针结构更通用一些。与迭代器不同的是，指针用于指向单个对象，而迭代器只能用于访问容器内的元素。

> **尽量避免使用指针和数组**
>
> 指针和数组容易产生不可预料的错误。指针用于低级操作，容易产生与繁琐细节相关的错误。其他错误则源于使用指针的语法规则，特别是声明指针的语法。
>
> 现代 C++ 程序采用 vector 类型和迭代器取代一般的数组和指针、采用 string 类型取代 C 风格字符串。

**注意：将符号 * 紧贴着指针变量名放置，是良好的编程习惯。**

>除非所指向的对象已经存在，否则不要先定义指针，这样可避免定义一个未初始化的指针。
>
>如果必须分开定义指针和所指向的对象，则将指针初始化为 0。因为编译器可以检测出 0 值的指针，程序可判断该指针并未指向一个对象。
>
>把指针初始化为 NULL 等效于初始化为 0 值。

### 3.2.1 void* 指针

C++ 提供了一种特殊的指针类型 void*，它可以保存任何类型对象的地址：

```C++
double obj = 3.14;
double *pd = &obj;

void *pv = &obj;
pv = pd;
```

### 3.2.2 指针操作

**a. 生成左值的解引用操作**

解引用操作符返回指定对象的左值，利用这个功能可修改指针所指对象的值：

```C++
string s("hello world");
string *sp = &s;
cout << *sp;
*sp = "goodbye";   // 因为 sp 指向 s，所以给 *sp 赋值也就修改了 s 的值。
```

也可以修改指针 sp 本身的值，使 sp 指向另外一个新对象：

```C++
string s2 = "some value";
sp = &s2;
```

**b. 指针和引用的比较**

引用总是指向某个对象：定义引用时没有初始化是错误的。给引用赋值修改的是该引用所关联的对象的值，而并不是使引用和另一个对象关联。

```C++
#include <iostream>
using namespace std;

int main(){
    int ival = 1024, ival2 = 2048;
    int *pi = &ival, *pi2 = &ival2;
    pi = pi2;         // 让指针指向 ival2 ，此时指针指向改变，两个指针指向同一个对象
    cout << ival << " " << ival2 << endl;
    cout << *pi << " " << *pi2 << endl;
    cout << pi << " " << pi2 << endl;
    return 0;
}

/*
1024 2048
2048 2048
0x61fe08 0x61fe08 **/
```

```C++
#include <iostream>
using namespace std;

int main(){
    int ival = 1024, ival2 = 2048;
    int &ref = ival, &ref2 = ival2;
    ref = ref2;
    cout << ival << " " << ival2 << endl;
    cout << ref << " " << ref2 << endl;
    return 0;
}
/* 
2048 2048
2048 2048 **/
```

这个赋值操作修改了 ref 引用的值 ival 对象，而非引用本身。赋值后，这两个引用还是分别指向原来关联的对象。此时两个对象的值相等。

### 3.2.3 使用指针访问数组元素

指针和数组密切相关。特别是在表达式中使用数组名时，该名字会自动转换为指向数组第一个元素的指针：

```C++
int ia[] = {0, 2, 4, 6, 8};
int *ip = ia;
```

如果希望使指针指向数组中的另一个元素，则可使用下标操作符给某个元素定位，然后用取地址操作符 & 获取该元素的存储地址：

```C++
ip = &ia[4];
```

> C++ 允许计算数组或对象的超出末端的地址，但不允许对此地址进行解引用操作。而计算数组超出末端位置之后或数组首地址之前的地址都是不合法的。
>
> 类似于 vector 的 end() 函数。

### 3.2.4 输出数组元素

```C++
const size_t arr_sz = 5;
int int_arr[arr_sz] = {0, 1, 2, 3, 4 };
for(int *pbegin = int_arr, *pend = int_arr + arr_sz; pbegin != pend; ++pbegin){
    cout << *pbegin << " ";
}
/** 此时 pend 指向的是最后一个元素的下一个地址，就是超出末端的地址 **/
```



## 3.3 指针和 const 限定符

### 3.3.1 指向 const 对象的指针

我们可以使用指针来修改其所指对象的值。但是如果指针指向 const 对象，则不允许用指针来改变其所指的 const 值。**为了保证这个特性，C++ 语言强制要求指向 const 对象的指针也必须具有 const 特性： **

```C++
const double *cptr;
```

这里的 cptr 是一个指向 double 类型 const 对象的指针，const 限定了 cptr 指针所指向的对象类型，，而并非 cptr 本身。也就是说，cptr 本身并不是 const。在定义时不需要对它进行初始化，如果需要的话，允许给 cptr 重新赋值，使其指向另一个 const 对象。但不能通过 cptr 修改其所指对象的值。

**不能使用 void* 指针保存 const 对象的地址，而必须使用 const void* 类型的指针保存 const 对象的地址。**

```C++
const int universe = 42;
const void *cpv = &universe;//ok
void *pv = &universe;       //error
```

### 3.3.2 const 指针

除指向 const 对象的指针外，C++ 语言还提供了 const 指针——本身的值不能修改：

```C++
int errNumb = 0;
int *const curErr = &errNumb;
```

const 指针的值不能修改，这就意味着不能使 curErr 指向其他对象，任何给 const 对象赋值的行为，都会导致编译时的错误，**与任何 const 量一样，const 指针也必须在定义时初始化。**

### 3.3.3 指向 const 对象的 const 指针

```C++
const double pi = 3.14;
const double *const pi_ptr = &pi;
```

既不能修改 pi_ptr 所指向对象的值，也不允许修改该指针的指向（即 pi_ptr 中存放的地址值）。



## 3.4 C 风格字符串

> 尽管 C++ 支持 C 风格字符串，但不应该在 C++ 程序中使用这个类型。

**字符串字面值的类型就是 const char 类型的数组。**C++ 从 C 语言继承下来的一种通用结构是 C 风格字符串，而字符串字面值就是该类型的实例。实际上，C 风格字符串既不能确切地归结为 C 语言的类型，也不能归结为 C++ 语言的类型，而是以空字符 null 结束的字符数组。

C++ 语言通过 char* 类型的指针来操纵 C 风格字符串。**永远不要忘记字符串结束符 null **。



## 3.5 创建动态数组

**new 表达式返回指向新分配数组的第一个元素的指针：**

```C++
int *pia = new int[10];
```

此 new 表达式分配了一个含有10个 int 型元素的数组，并返回指向该数组第一个元素的指针，此返回值初始化了指针 pia 。

---



# 4. 表达式和语句

## 4.1 自增和自减操作符

前自增操作使其操作数加一，操作结果使修改后的值。后自增操作符操作后产生操作原来的、未修改的值作为表达式的结果：

```C++
int i = 0, j;
j = ++i;      // j=1, i=1;
j = i++;      // j=1, i=2;
```

**因为前置操作符返回加一后的值，所以返回对象本身，这是左值。**而后置操作返回的则是右值。

> 只有在必要时才使用后置操作符，尽量使用前置操作符。

*iter++ 等价于 *(iter++)，我们应该多的使用 *iter++ 简洁即是美。



## 4.2 sizeof 操作符

sizeof 操作符的作用是返回一个对象或类型名的长度，返回值的类型为 size_t 长度的单位是字节。sizeof 表达式的结果是编译时常量。

指针的长度 32 位操作系统为 4，64 位操作系统为 8。

```C++
#include <iostream>
using namespace std;

int main(){
    int a = 10;
    int *p = &a;

    cout << sizeof(p) << endl;
    return 0;
}
/**   8    **/
```



## 4.3 assert 

预处理宏 assert 是在 cassert 头文件中定义的，所有使用 assert 的文件都必须包含这个头文件。**预处理宏有点像函数调用。**assert 宏需要一个表达式作为它的条件：

```C++
assert(expr)
```

只要 NDEBUG 未定义，assert 宏就求解条件表达式 expr，**如果结果为 false ，assert 输出信息并终止程序的执行。如果该表达式有一个非零值，则 assert 不做任何操作。**

> 在成品代码中，assert 不做任何工作，因此也没有任何运行时代价。当然，也不会引起任何运行时检查。assert 仅用于检查确定不可能的条件，这只对程序的调试有帮助，但不能用来代替运行时的逻辑检查，也不能代替对程序可能产生的错误的检测。

---



# 5.函数

> C++ 是一种静态强类型语言，对于每一次的函数调用，编译时都会检查其实参。

## 5.1 参数传递

每次调用函数时，都会重新创建该函数多有的参数，此时所传递的实参将会初始化对应的形参。

**形参的初始化与变量的初始化一样：如果形参具有非引用类型，则复制实参的值；如果形参为引用类型，则它只是实参的别名。**

### 5.1.1 非引用形参

普通的非引用类型的参数通过复制对应的实参实现初始化。当用实参副本初始化形参时，函数并没有访问调用所传递的实参本身，因此不会修改实参的值。

### 5.1.2 引用形参

考虑下面不适宜复制实参的例子，该函数希望交换两个实参的值：

```C++
#include <iostream>
using namespace std;

void swap(int v1, int v2){
    int temp = v2;
    v2 = v1;
    v1 = temp;
}

int main(){
    int a = 10;
    int b = 20;
    cout << a << " " << b << endl;
    swap(a, b);
    cout << a << " " << b << endl;
    return 0;
}
/** 
10 20
10 20   **/
```

这个例子期望改变实参本身的值。单对于上述的函数定义，swap 无法影响实参本省。执行 swap 时，只交换了其实参的局部副本，而传递给 swap 的实参并没有改变。

为了使 swap 函数以期望的方式工作，交换实参的值，需要将形参定义为引用类型：

```C++
void swap(int &v1, int &v2){
    int temp = v1;
    v1 = v2; 
    v2 = temp;
}
```

**C 语言中习惯用指针来实现对实参的访问。在 C++ 中，使用引用形参则更安全和更自然。**

---

**1.使用引用形参返回额外的信息**

函数只能返回一个值，但有时候，函数有不止一个的内容需要返回。例如，定义一个 find_val 函数，在一个整型 vector 对象的元素中搜索某个特定值。如果找不到满足要求的元素，则返回指向该元素的迭代器，如果该值出现不止一次，我们还希望可以返回其出现的次数。

```C++
#include <iostream>
#include <vector>
using namespace std;

vector<int>::const_iterator find_val(vector<int>::const_iterator beg, vector<int>::const_iterator end, int value, vector<int>::size_type &occurs){
    vector<int>::const_iterator res_iter = end;
    occurs = 0;
    for( ; beg != end; ++beg){
        if(*beg == value){
            if(res_iter == end)
                res_iter = beg;
            ++occurs;
        }
    }
    cout << occurs << endl;
    return res_iter;
}

int main(){
    vector<int> list = {1, 2, 2, 4, 6, 4, 8, 2, 2, 4};
    vector<int>::size_type ctr;
    vector<int>::const_iterator it = find_val(list.begin(), list.end(), 2, ctr);
    cout << *it << endl;
}
/**  
4
2   **/
```

**2. 利用 const 引用避免复制**

使用引用形参，函数可以直接访问实参对象，而无需复制它。

编写一个比较两个 string 对象长度的函数作为例子。这个函数需要访问每个 string 对象的 size，但不必修改这些对象。由于 string 对象可能相当长，所以我们希望避免复制操作。使用 const 引用就可避免复制：

```C++
bool isShorter(const string &s1, const string &s2){
    return s1.size() < s2.size();
}
```

其每一个形参都是 const string 类型的引用。因为形参是引用，所以不复制实参。**又因为形参是 const 引用，所以 isShorter 函数不能使用该引用来修改实参。**

> 如果使用引用形参的唯一目的是避免复制实参，则应将形参定义为 const 引用。

**3. 传递指向指针的引用**

假设我们想编写一个与前面交换两个整数的 swap 类似的函数，实现两个指针的交换。

```C++
void ptrswap(int *&v1, int *&v2){
    int *tmp = v2;
    v2 = v1;
    v1 = tmp;
}
```

v1 是一个引用，与指向 int 型对象的指针相关联。也就是说，v1 只是传递进 ptrswap 函数的任意指针的别名。



## 5.2 数组形参

**数组有两个特殊的性质，影响我们定义和使用作用在数组上的函数：一是不能复制数组；二是使用数组名字时，数组名会自动转化为指向其第一个元素的指针。因为数组不能复制，所以无法编写使用数组类型形参的函数。因为数组会被转化为指针，所以处理数组的函数通常通过操纵指向数组中的元素的指针来处理数组。**



> 理解返回引用至关重要的是：千万不能返回局部变量的引用。

当函数执行完毕时，将释放分配给局部变量的存储空间。此时，对局部对象的引用就会指向不确定的内存。

```C++
const string &manip(const string& s){
    string ret = s;
    return ret;
}
/** 这个函数会在运行时出错，因为它返回了局部对象的引用。当函数执行完毕，字符串 ret 占用的储存空间被释放，函数返回值指向了对于这个程序来说不再有效的空间。
```

### 5.2.1 引用返回左值

返回引用的函数返回一个左值。因此，这样的函数可用于任何要求使用左值的地方：

```C++
char &get_val(string &str, string::size_type ix){
    return str[ix];
}
int main(){
    string s("a value");
    cout << s << endl;
    get_val(s, 0) = 'A';
    cout << s <<endl;
    return 0;
}
/**   
a value
A value     **/
```

### 5.2.2 静态局部变量

**static 局部对象**确保不迟于在程序执行流程第一次经过该对象的定义语句时进行初始化。这种对象一旦被创建，在程序结束前都不会被销毁。当定义静态局部对象的函数结束时，静态局部对象不会撤销。在该函数被多次调用的过程中，静态局部对象会持续存在并保持它的值。

```C++
#include <iostream>
using namespace std;

size_t count_calls(){
    static size_t ctr = 0;    // 静态变量只会初始化一次，且在程序结束时被销毁。
    return ++ctr;
}

int main(){
    for(size_t i = 0; i != 10; ++i){
        cout << count_calls() << " ";
    }
    cout << endl;
    return 0;
}
/**
1 2 3 4 5 6 7 8 9 10 
**/
```

在第一次调用函数 count_calls 之前，ctr 就以创建并赋予初值 0。每次函数调用都使 ctr 加1，并且返回其当前值。在执行函数 count_calls 时，变量 ctr 就已经存在并且保留上次调用该函数时的值。因此，第二次调用时，ctr 的值为 1，第三次为 2，以此类推。



## 5.3 内联函数

### 5.3.1 内联函数避免函数调用的开销

将函数指定为内联函数，就是将它在程序中每个调用点上 “内联地” 展开。

在函数返回类型前加上关键字 inline 就可以把函数指定为内联函数：

```C++
inline const string &shorterString(const string &s1, const string &s2){
    return s1.size() < s2.size() ? s1 : s2;
}
```

> 内联说明对编译器来说就是个建议，编译器可以选择忽略这个建议。

一般来说，内联机制适用于优化小的、只有几行的而且经常被调用的函数。大多数的编译器都不支持递归函数的内联。一个1200行的函数也不太可能在调用点内联展开。

### 5.3.2 把内联函数放入头文件

> 内联函数应该在头文件中定义，这一点不同于其他函数。

内联函数的定义对编译器而言必须是可见的，以便编译器能够在调用点内联展开该函数的代码。此时，仅有函数原型是不够的。

内联函数可能可能要在程序中定义不止一次，只要内联函数的定义在某个源文件中只出现一次，而且在所有源文件中，其定义必须是完全相同的。把内联函数的定义放在头文件中，可以确保在调用函数时所使用的定义是相同的，并且保证在调用点该函数的定义对编译器可见。

> 在头文件中加入或修改内联函数时，使用了该头文件的所有源文件都必须重新编译。



## 5.4 类的成员函数

### 5.4.1 this 指针

每个成员函数（除 static 成员函数外）都有一个额外的、隐含的形参 **this**。在调用成员函数时，形参 this 初始化为调用函数的对象的地址。

在成员函数中，不必显式地使用 this 指针来访问被调用函数所属对象的成员。对这个类的成员在任何没有前缀的引用，都被假定为通过指针 this 实现的引用。

### 5.4.2 构造函数

```C++
Sales_item():units_sold(0), revenue(0.0) { }
```

在冒号和花括号之间的代码称为**构造函数的初始化列表**。构造函数的初始化列表为类的一个或多个数据成员指定初值。它跟在构造函数的形参表之后，以冒号开头。构造函数的初始化是一系列成员名，每个成员后面是括在圆括号中的初始值。多个成员的初始化用逗号分隔。

上述代码的初始化列表表明 units_sold 和 revenue 成员都应初始化为 0。每当创建 Sales_item 对象时，它的这两个成员都以初值 0 出现。



## 5.5 指向函数的指针

函数指针是指指向函数而非对象的指针。与其他指针一样，函数指针也指向某个特定的类型。函数类型由其返回类型以及形参表确定，而与函数名无关：

```C++
bool (*pf)(const string &, const string &);
```

这个语句将 pf 声明为指向函数的指针，它所指向的函数带有两个 const string& 类型的形参和 bool 类型的返回值。

> *pf 两侧的圆括号是必须的
>
> bool *pf(const string &, const string &);    返回的是一个 bool *

### 5.5.1 通过指针调用函数

指向函数的指针可用于调用它所指向的函数。可以不需要使用解引用操作符，直接通过指针调用函数：

```C++
cmpFcn pf = lengthCompare;
lengthCompare("hi", "bye");
pf("hi", "bye");
(*pf)("hi", "bye");
```

### 5.5.2 函数指针形参

函数的形参可以是指向函数的指针。这种形参可以用以下两种形式编写：

```C++
void useBigger(const string &, const string &, bool(const string &, const string &));

void useBigger(const string &, const string &, bool (*)(const string &, const string &));
```

### 5.5.3 返回指向函数的指针

函数可以返回指向函数的指针，但是，正确写出这种返回类型相当不容易：

```C++
int (*ff(int))(int*, int)
```

ff(int) ，将 ff 声明为一个函数，它带有一个 int 型的形参。该函数返回 int (*)(int *, int);

它是一个指向函数的指针，所指向的函数返回 int 型并带有两个分别是 int* 型和 int 型的形参。

---



# 6. 顺序容器

| **顺序容器**       |                      |
| ------------------ | -------------------- |
| vector             | 支持快速随机访问     |
| list               | 支持快速插入/删除    |
| deque              | 双端队列             |
| **顺序容器适配器** |                      |
| stack              | 后进先出（LIFO）栈   |
| queue              | 先进先出（FIFO）队列 |
| priority_queue     | 有优先级管理的队列   |

容器只定义了少量操作。大多数额外的操作则由算法库提供。标准库为由容器类型定义的操作强加了公共的接口。这些容器类型的差别在于他们提供哪些操作，但是如果两个容器提供了相同的操作，则它们的接口（函数名字和参数个数）应该相同。容器类型的操作集合形成了以下层次结构：

* 一些操作适用于所有容器类型。
* 另外一些操作则只适用于顺序或关联容器类型。
* 还有一些操作只适用于顺序或关联容器类型的一个子集。

> 使用迭代器编写程序时，必须留意哪些操作会使迭代器失效。使用无效迭代器会导致严重的运行时错误。

```C++
list<string> lst;
list<string>::iterator iter = lst.begin();
while(cin >> word)
    iter = lst.insert(iter, word);
```

上述代码等效于调用 push_front 函数。循环前，将 iter 初始化为 lst.begin()。此时，**由于该 llist 对象是空的，因此 lst.begin() 与 lst.end() 相等，于是 iter 指向该（空）容器的超出末端的下一个位置。**第一次调用 insert 函数时，将刚读入的元素插入到 iter 所指向位置的前面，容器 lst 得到第一个也是唯一的元素。**然后 insert 函数返回指向这个新元素的迭代器，并赋给 iter，接着重复 while 循环，读入下一个单词。**

> 任何 insert 或 push 操作都可能导致迭代器失效。当编写循环将元素插入到 vector 或 deque 容器中时，程序必须确保迭代器在每次循环后都得到更新。

## 6.1 容器赋值

| c1 = c2       | 删除容器 c1 的所有元素，然后将 c2 的元素复制给 c1。c1 和 c2 的类型必须相同 |
| ------------- | ------------------------------------------------------------ |
| c1.swap(c2)   | 交换内容：调用完该函数后，c1 中存放的是 c2 原来的元素，c2 中存放的则是 c1 原来的元素。c1 和 c2 的类型必须相同。该函数的执行速度通常比将 c2 的元素复制到 c1 的操作快 |
| c.assign(b,e) | 重新设置 c 的元素：将迭代器 b 和 e 标记的范围内所有的元素复制到 c 中。b 和 e 必须不是指向 c 中元素的迭代器 |
| c.assign(n,t) | 将容器 c 重新设置为存储 n 个值为 t 的元素                    |

> 赋值和 assign 操作使左操作数容器的所有迭代器失效。swap 操作则不会使迭代器失效。



## 6.2 vector 与 string 的使用

### 6.2.1 字符串分割

将 string 对象中的字符以 . 截取放进一个 vector 容器中。

```C++
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

void myFun(const string &str){
    vector<string> v;
    int start = 0;
    while(true){
        int pos = str.find(".", start);
        if(pos == -1){
            // 将 . 最后一个元素放入到容器中
            string tmp = str.substr(start, str.size() - start);
            v.push_back(tmp);
            break;
        }
        // 将字符串存放到容器中，以 . 分割
        string tmp = str.substr(start, pos - start);
        v.push_back(tmp);
        start = pos + 1;
    }
    
    //输出容器中的元素
    for(vector<string>::const_iterator it = v.begin(); it != v.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;
}

int main(){
    string str = "www.hisense.mail.com";
    myFun(str);
    return 0;
}
/**
www hisense mail com **/
```

### 6.2.2 评委打分

5名选手，姓名为ABCDE，10个评委打分，去除最低分和最高分，输出平均分。

```C++
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

class Person{
    public:
        Person(string name, int score): m_Name(name), m_Score(score){}
        string m_Name;    // 姓名
        int m_Score;      // 分数
};

void createPerson(vector<Person> &v){
    string strSeed = "ABCDE";
    int score = 0;
    for(int i = 0; i < 5; i++){
        string name = "选手：";
        name = name + strSeed[i];

        // 用构造函数初始化对象
        Person p(name, score);
        v.push_back(p);
    }
}

void setScore(vector<Person> &v){
    // 对容器中的每一个对象设置分数
    for(vector<Person>::iterator it = v.begin(); it != v.end(); ++it){
        // 存放10个评委的分数
        deque<int> d;
        for(int i = 0; i < 10; i++){
            int score = rand() % 41 + 60;   // 60~100
            d.push_back(score);
        }
        // 排序
        sort(d.begin(), d.end());
        // 去除最低和最低分
        d.pop_back();
        d.pop_front();
        // 得出总分
        int sum = 0;
        for(deque<int>::const_iterator dit = d.begin(); dit != d.end(); ++dit){
            sum = sum + *dit;
        }
        // 计算平均分
        int avg = sum / d.size();
        // 赋值分数，将 avg 赋值给m_Score
        it->m_Score = avg;
    }
}

//显示分数
void showScore(vector<Person> &v){
    for(vector<Person>::const_iterator it = v.begin(); it != v.end(); ++it){
        cout << "姓名：" << it->m_Name << "分数为：" << it->m_Score << endl;
    }
}


int main(){
    // 创建容器
    vector<Person> v;
    // 创建对象
    createPerson(v);
    // 设置分数
    setScore(v);
    // 显示分数
    showScore(v);
}
/**
姓名：选手：A分数为：78
姓名：选手：B分数为：82
姓名：选手：C分数为：74
姓名：选手：D分数为：80
姓名：选手：E分数为：77
**/
```

---



# 7.关联容器

关联容器支持通过键来高效地查找和读取元素。

| map      | 关联数组：元素通过键来存储和读取         |
| -------- | ---------------------------------------- |
| set      | 大小可变的集合，支持通过键实现的快速读取 |
| multimap | 支持同一个键多次出现的 map 类型          |
| multiset | 支持同一个键多次出现的 set 类型          |

一般来说，如果希望有效的存储不同值的集合，那么使用 set 容器比较合适，而 map 容器则更适用于需要存储（乃至修改）每个键所关联的值的情况。

## 7.1 map 类型

map 是键-值对的集合。map 类型通常可理解为**关联数组**：可使用键作为下标来获取一个值，正如内置数组类型一样。而关联的本质在于元素的值与某个特定的键相关联，而非通过元素在数组中的位置来获取。

> **键类型的约束**
>
> 在使用关联容器时，它的键不但有一个类型，而且还有一个相关的比较函数。默认情况下，标准库使用键类型定义的 < 操作符来实现键的比较。
>
> 所用的比较函数必须在键类型上定义**严格弱排序**。所谓严格弱排序可理解为键类型数据上的 "小于" 关系，虽然实际上可以选择将比较函数设计得更复杂。但无论这样的函数如何定义，当用于一个键与自身的比较时，肯定会导致 false 结果。此外，在比较两个键时，不能出现相互 ”小于“ 的情况。
>
> 在实际应用中，键类型必须定义 < 操作符，而且该操作符应能 “正确地工作”，这一点很重要。

| map<K,V>::key_type    | 在map容器中，用作索引的键的类型                              |
| --------------------- | ------------------------------------------------------------ |
| map<K,V>::mapped_type | 在map容器中，键所关联的值的类型                              |
| map<K,V>::value_type  | 一个pair类型，它的first元素具有map<K,V>::key_type类型，而second元素具有map<K,V>::mapped_type类型 |

**需谨记 value_type 是 pair 类型，它的值成员可以修改，但键成员不能修改。键为 const。**

### 7.1.1 map 迭代器进行解引用将产生 pair 对象

对迭代器进行解引用时，将获得一个引用，指向容器中一个 value_type 类型的值。对于 map 容器，其 value_type 是 pair 类型：

```C++
map<string, int> word_count;
map<string, int>::iterator map_it = word_count.begin();
cout << map_it->first;
cout << " " << map_it->second;
map_it->first = "new key";      // error, key is const;
++map_it->second;               // ok, we can change value through an iterator 
```

**对迭代器进行解引用将获得一个 pair 对象，它的 first 成员存放键，为 const，而 second 成员存放值。**

### 7.1.2 下标行为的编程意义

对于 map 容器，如果下标所表示的键在容器中不存在，则添加新元素，这一特性可使程序惊人地简练：

```C++
map<string, int> word_count;
string word;
while(cin >> word)
    ++word_count(word);
```

这段程序创建一个 map 对象，用来记录每个单词出现的次数。while 循环每次从标准输入读取一个单词。如果这是一个新的单词，则在 word_count 中添加该单词为索引的新元素。如果读入的单词已在 map 对象中，则将它所对应的值加1。



## 7.2 set 类型

在 set 中添加元素 insert 函数。在 set 中获取元素 find 和 count 函数。

**容器的综合查询：文本查询程序**

我们的程序将读取用户指定的任意文本文件，然后允许用户从该文件中查找单词。查询的结果是该单词出现的次数，并列出每次出现所在的行。如果某单词在同一行中多次出现，程序将只显示该行一次。行号按升序显示。

设计程序的一个良好习惯是首先将程序所设计的操作列出来。明确需要提供的操作有助于建立需要的数据结构和实现这些行为：

1. 它必须允许用户指明要处理的文件名字，程序将存储该文件的内容，以便输出每个单词所在的原始行。
2. 它必须将每一行分解为各个单词，并记录每个单词所在的所有行。在输出行号时，应保证以升序输出，并且不重复。
3. 对特定单词的查询将返回出现该单词的所有行的行号。
4. 输出某单词所在的行文本时，程序必须能根据给定的行号从输入文件中获取相应的行。

```C++
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
using namespace std;

class TextQuery{
public:
    /* 该容器用于存储文件行 */
    typedef vector<string>::size_type line_no;
    /*
     * interface
     * read_file 形参为一个 ifstream& 类对象。该函数每次从文件中读入一行，并将它保存在 vector 容器中。
     *           输入完毕后，read_file 将创建关联每个单词及所在行号的 map 容器。
     * run_query 其形参为一个 string 类型队象, 返回一个 set 对象，该 set 对象包含出现该 string 对象的
     *           所有行的行号。
     * text_line 其形参为一个行号，返回输入文本中该行号队应的文本行。
     * 无论 run_query 还是 text_line 都不会修改调用此函数的对象，因此，可将这两个操作定义为 const 成员函数。
     *
     * 为实现 read_file 功能，还需要定义两个 private 函数来读取输入文本和创建 map 容器：
     * store_file 函数读入文件，并将文件内容存储在 vector 容器对象中。
     * build_map 函数将每一行分解为各个单词，创建 map 容器对象，同时记录每个单词出现的行号。
     * */
    void read_file(ifstream &is){
        store_file(is);
        build_map();
    }
    set<line_no> run_query(const string&) const;
    string text_line(line_no) const;
private:
    void store_file(ifstream&);
    void build_map();
    vector<string> lines_of_text;
    map<string, set<line_no>> word_map;
};

void TextQuery::store_file(ifstream &is){
    string textline;
    while(getline(is, textline))
        lines_of_text.push_back(textline);
}

void TextQuery::build_map(){
    for(line_no line_num = 0; line_num != lines_of_text.size(); ++ line_num){
        istringstream line(lines_of_text[line_num]);
        string word;
        while(line >> word)
            word_map[word].insert(line_num);
    }
}

set<TextQuery::line_no> TextQuery::run_query(const string &query_word) const {
    map<string, set<line_no>>::const_iterator loc = word_map.find(query_word);
    if(loc == word_map.end())
        return set<line_no>();
    else
        return loc->second;
}

string TextQuery::text_line(line_no line) const {
    if(line < lines_of_text.size()){
        return lines_of_text[line];
    }
    throw out_of_range("line number out of range");
}

void print_results(const set<TextQuery::line_no>& locs, const string& sought, const TextQuery &file){
    typedef set<TextQuery::line_no> line_nums;
    line_nums::size_type size = locs.size();
    cout << "\n" << sought << "occurs" << size << " " << make_plural(size, "time", "s") << endl;
    line_nums::const_iterator it = locs.begin();
    for( ; it != locs.end(); ++it){
        cout << "\t(line " << (*it) + 1 << ") " <<file.text_line(*it) << endl;
    }
}

int main(int argc, char **argv){
    ifstream infile;
    if(argc < 2 || !open_file(infile, argv[1])){
        cerr << "No input file!" << endl;
        return EXIT_FAILURE;
    }
    TextQuery tq;
    tq.read_file(infile);
    while(true){
        cout << "enter word to look for , or q to quit: ";
        string s;
        cin >> s;
        if(!cin || s == 'q') break;
        set<TextQuery::lin_no> locs = tq.run_query(s);
        print_results(locs, s, tq);
    }
    return 0;
}

```





# 8.泛型算法

> **算法永不执行容器提供的操作**
>
> 泛型算法本身从不执行容器操作，只是单独依赖迭代器和迭代器操作实现。算法基于迭代器及其操作实现，而并非基于容器操作。算法也许会改变存储在容器中的元素的值，也许会在容器内移动元素，但是，算法从不直接添加或删除元素。

## 8.1 只读算法

许多算法只会读取其输入范围内的元素，而不会写这些元素。find 就是一个这样的算法。另一个简单的只读算法是 accumulate，该算法在 numeric 头文件中定义。

```C++
int sum = accumulate(vec.begin(), vec.end(), 42);
/**   vec 是一个 int 型的 vector 容器 **/
```

将 sum 设置为 vec 的元素之和再加上 42。accumulate 带有三个形参。头两个形参指定要累加的元素范围。第三个形参则是累加的初值。accumulate 函数将它的一个内部变量设置为指定的初值，然后在此初值上累加输入范围内所有元素的值。**用于指定累加初始值的第三个实参是必要的，因为 accumulate 对将要累加的元素类型一无所知。**

```C++
string sum = accumulate(v.begin(), v.end(), string(" "));
```

此函数调用的效果是：从空字符串开始，把 vec 里的每个元素连接成一个字符串。注意：程序显示地创建了一个 string 对象，用作该函数调用的第三个实参。

**因为此时，累加和的类型将是 const char *，而 string 的加法操作符所使用的操作数则分别是 string 和 const char * 类型，加法的结果将产生一个 string 对象，而不是 const char * 指针。**



## 8.2 写容器元素的算法

### 8.2.1 写入输入序列的元素

```C++
fill(vec.begin(), vec.end(), 0);
fill(vec.begin(), vec.begin() + vec.size()/2, 10);
```

fill 带有一对迭代器形参，用于指定要写入的范围，而所写的值则是它的第三个形参的副本。

### 8.2.2 不检查写入操作的算法

fill_n 函数带有的参数包括：一个迭代器、一个计数器以及一个值。该函数从迭代器指向的元素开始，将指定数量的元素设置为给定的值。fill_n 函数假定对指定数量的元素做写操作是安全的。

```C++
vector<int> vec;
fill_n(vec.begin(), 10, 0);
```

这个 fill_n 函数的调用将带来灾难性的后果。我们指定要写入 10 个元素，但这些元素却不存在----vec 是空的。

### 8.2.3 引入 back_inserter

确保算法有足够的元素存储输出数据的一种方法是使用**插入迭代器**。插入迭代器是可以给基础容器添加元素的迭代器。使用 back_inserter 的程序必须包含 iterator 头文件。

**back_inserter 函数是迭代器适配器。**

```C++
vector<int> vec;
fill_n(back_inserter(vec), 10, 0);
```

现在，fill_n 函数每写入一个值，都会通过 back_inserter 生成的插入迭代器实现。效果相当于在 vec 上调用 push_back，在 vec 末尾添加 10 个元素，每个元素的值都是 0。

### 8.2.4 写入到目标迭代器的算法

copy 带有三个迭代器参数：头两个指定输入范围，第三个则指向目标序列的一个元素。传递给 copy 的目标序列必须至少要与输入范围一样大。

```C++
vector<int> ivec;
copy(ilst.begin(), ilst.end(), back_inserter(ivec));
```



## 8.3 对容器元素重新排序的算法

### 8.3.1 unique 的使用

![image-20230403202213457](C:\Users\75488\AppData\Roaming\Typora\typora-user-images\image-20230403202213457.png)

容器的大小并没有改变，依然保存10个元素；只是这些元素的顺序改变了。调用 unique “删除了” 相邻的重复值。实际上并没有删除，而是将无重复的元素复制到序列的前端，从而覆盖相邻的重复元素。unique 返回的迭代器指向超出无重复的元素范围的末端的下一个位置。

**如果要删除重复的项，必须使用容器操作，例如 erase 函数。算法不直接修改容器的大小。如果需要添加或删除元素，则必须使用容器操作。**

**谓词**是做某些检测的函数，返回用于条件判断的类型，指出条件是否成立。

```C++
bool isShorter(const string &s1, const string &s2){
    return s1.size() < s2.size();
}

bool GT6(const string &s){
    return s.size() > 6;
}
```

### 8.3.2 排序算法

stable_sort 保留相等元素的原始相对位置。对于长度相同的元素，将保留其字典顺序。

count_if 函数返回使谓词函数条件成立的元素个数。

```C++
vector<string>::size_type wc = count_if(word.begin(), word.end(), GT6);
```



## 8.4 再谈迭代器

| 插入迭代器     | 这类迭代器与容器绑定在一起，实现在容器中插入元素的功能       |
| -------------- | ------------------------------------------------------------ |
| iostream迭代器 | 这类迭代器可与输入或输出流绑定在一起，用于迭代遍历所关联的 IO 流 |
| 反向迭代器     | 这类迭代器实现向后遍历，而不是向前遍历。所有容器类型都定义了自己的 reverse_iterator 由 rbegin 和 rend 成员函数返回。 |

上述迭代器类型都在 iterator 头文件定义。

### 8.4.1 插入迭代器

| back_inserter  | 创建使用 push_back 实现插入的迭代器                          |
| -------------- | ------------------------------------------------------------ |
| front_inserter | 使用 push_front 实现插入                                     |
| interter       | 使用 insert 实现插入操作。第二个参数指向插入起始位置的迭代器 |

### 8.4.2 反向迭代器

```C++
#include <vector>

using namespace std;

int main(){
    vector<int> v;
    for(int i = 0; i < 10; i++){
        v.push_back(i);
    }

    // 反向输出
    vector<int>::reverse_iterator r_iter;
    for(r_iter = v.rbegin(); r_iter != v.rend(); ++r_iter){
        cout << *r_iter << " ";
    }
    cout << endl;
    return 0;
}
/** 9 8 7 6 5 4 3 2 1 0 **/
```

---



# 9.类

## 9.1 隐含的 this 指针

成员函数具有一个附加的隐含形参，即指向该类对象的一个指针。这个隐含形参命名为 this，与调用成员函数的对象绑定在一起。成员函数不能定义 this 形参，而是由编译器隐含地定义。成员函数的函数体可以显示使用 this 指针，但不是必须这么做。如果对类成员的引用没有限定，编译器会将这种引用处理通过 this 指针的引用。

### 9.1.1 返回 *this

在单个表达式中调用成员函数时，每个操作必须返回一个引用，该引用指向执行操作的那个对象：

```C++
class Screen{
    public:
    	typedef string::size_type index;
    	Screen& move(index r, index c);
    	Screen& set(char);
    	Screen& set(index, index, char);
};

Screen& Screen::set(char c){
    contents[currsor] = c;
    return *this;
}

Screena& Screen::move(index r, index c){
    index row = r * width;
    cursor = row + c;
    return *this;
}
```

每个函数都返回调用自己的那个对象。使用 this 指针来访问该对象。this 是一个指向非常量 Screen 的指针。如同任意的指针一样，可以通过对 this 指针解引用来访问 this 指向的对象。

### 9.1.2 从 const  成员函数返回 *this

**在普通的非 const 成员函数中，this 的类型是一个指向类类型的 const 指针。可以改变 this 所指向的值，但不能改变 this 所保存的地址。在 const 成员函数中，this 的类型是一个指向 const 类类型对象的 const 指针。既不能改变 this 所指向的对象，也不能改变 this 所保存的地址。**

> 不能从 const 成员函数返回指向类对象的普通引用。const 成员函数只能返回 *this 作为一个 const 引用



## 9.2 可变数据类型

有时，我们希望类额数据成员（甚至在 const 成员函数内）可以修改。这可以通过将它们声明为 mutable 来实现。

**可变数据成员**永远都不能为 const，甚至当它是 const 对象的成员时也如此。因此，const 成员函数可以改变 mutable 成员。要将数据成员声明为可变的，必须将关键字 mutable 放在成员声明之前：

```C++
class Screen{
    public:
    private:
    	mutable size_t access_ctr;
};
```

我们给 Screen 添加了一个新的可变数据成员 access_ctr。使用 access_ctr 来跟踪调用 Screen 成员函数的频繁程度：

```C++
void Screen::do_display(std::ostream& os) const {
	++access_ctr;
    os << contents;
}
```

尽管 do_display 是 const，它也可以增加 access_ctr。该成员是可变成员，所以，任意成员函数，包括 const 函数，都可以改变 access_ctr 的值。



## 9.3 友元

**友元**机制允许一个类将对其非公有成员的访问授予指定的函数或类。友元的声明以关键字 friend 开始。它只能出现在类定义的内部。友元声明可以出现在类中的任何地方：友元不是授予友元关系的那个类的成员，所以它们不受其声明出现部分的访问控制影响。

> 通常，将友元声明成组地放在类定义的开始或结尾是个好主意。

```C++
class Screen {
	friend class Window_Mgr;  
};
```

Window_Mgr 的成员可以直接引用 Screen 的私有成员。

### 9.3.1 使其它类的成员函数成为友元

如果不是将整个 Window_Mgr 类设为友元，Screen 就可以指定只允许 relocate 成员访问：

```C++
class Screen {
    friend Window_Mgr& Window_Mgr::relocate(Window_Mgr::index, Window_Mgr::index, Screen&);
};
```

当我们将成员函数声明为友元时，函数名必须用该函数所属的类名字加以限定。



## 9.4 static 类成员

非 static 数据成员存在于类类型的每个对象中。不像普通的数据成员，static 数据成员独立于该类的任意对象而存在；每个 static 数据成员是与类关联的对象，而不与该类的对象关联。

正如类可以定义共享的 static 数据成员一样，类也可以定义 static 成员函数。**static 成员函数没有 this 形参，它可以直接访问所属类的 static 成员，但不能直接使用非 static 成员。**

static 成员不是类对象的组成部分。

---



# 10.复制控制
**复制构造函数**是一种特殊构造函数，具有单个形参，该形参（常用 const 修饰）是对该类类型的引用。当定义一个新对象并用一个同类型的对象对它进行初始化时，将显式使用复制构造函数。当将该类型的对象传递给函数或从函数返回该类型的对象时，将隐式使用复制构造函数。
**析构函数**是构造函数的互补：当对象超出作用域或动态分配的对象被删除时，将自动应用析构函数。不管类是否定义了自己的析构函数，编译器都自动执行类中非 static 数据成员的析构函数。
复制构造函数、赋值操作符和析构函数总称为**复制控制**。编译器自动实现这些操作，但类也可以定义自己的版本。
> 复制控制是定义任意 C++ 类必不可少的部分。编译器合成的复制控制函数是非常精炼的---它只做必须的工作。但对于某些类而言，依赖于默认定义会导致灾难。
## 10.1 复制构造函数
 * 根据另一个同类型的对象显式或隐式初始化一个对象。
 * 复制一个对象，将它作为实参传给函数。
 * 从函数返回时复制一个对象。
 * 初始化顺序容器中的元素。
 * 根据元素初始化式列表初始化数组元素。

**合成复制构造函数**的行为是，执行**逐个成员初始化**，将对新对象初始化为原对象的副本。所谓“逐个成员”，指的是编译器将现有对象的非 static 成员，依次复制到正创建的对象。只有一个例外，每个成员的类型决定了复制该成员的含义。



## 10.2 析构函数
构造函数的一个用途是自动获取资源。例如，构造函数可以分配一个缓冲区或打开一个文件，在构造函数中分配了资源以后，需要一个对应操作自动回收或释放资源。析构函数就是这样的一个特殊函数，它可以完成所需的资源回收，作为类构造函数的补充。
**撤销类对象时会自动调用析构函数。**



## 10.3 管理指针成员
包含指针的类需要特别注意复制控制，原因是复制指针时只复制指针中的地址，而不会复制指针指向的对象。

 1. 指针成员采取常规的指针型行为。这样的类具有指针的所有缺陷但无需特殊的复制控制。
 2. 类可以实现所谓的“智能指针”行为。指针所指向的对象是共享的，但类能够防止悬垂指针。
 3. 类采取值型行为。指针所指向的对象是唯一的，由每个类对象独立管理。

### 10.3.1 一个带指针成员的简单类
```C++
class HasPtr {
public:
	HasPtr(int *p, int i) : ptr(p), val(i) { }
	int* get_ptr() const { return ptr; }
	int get_int() const { return val; }
	void set_ptr(int* p) { ptr = p; }
	void set_int(int i) { val = i; }
	int get_ptr_val() const { return *ptr };
	void set_ptr_val(int val) const { *ptr = val; }
private:
	int* ptr;
	int val;
};
```
### 10.3.2 可能出现悬垂指针
```C++
int *ip = new int(42);
HasPtr ptr(ip, 10);
delete ip;
ptr.set_ptr_val(0);    // error
```
这里的问题是 ip 和 ptr 中的指针指向同一对象。删除了该对象时，ptr 中的指针不再指向有效对象。然而，没有办法得知对象已经不存在了。
### 10.3.3 定义智能指针
定义智能指针的通用技术是采用一个**使用计数**。智能指针类将一个计数器与类指向的对象相关联。使用计数跟踪该类有多少个对象共享同一指针。使用计数为0时，删除对象。使用计数有时也称为**引用计数**。
每次创建类的新对象时，初始化指针并将使用计数置为1.
```C++
class U_Ptr {
    friend class HasPtr;
    int *ip;
    size_t use;
    U_Ptr(int *ip) : ip(p), use(1) { }
    ~U_Ptr() { delete ip; }
};

class HasPtr {
public:
	HasPtr(int *p, int i): ptr(new U_ptr(p)), val(i) { }
	HasPtr(const HasPtr& orig) : ptr(orig.ptr), val(orig.val) { ++ptr->use; }
	HasPtr& operator=(const HasPtr&);
	~HasPtr() { if (--ptr->use == 0) delete ptr; }
private:
	U_ptr* ptr;
	int val;
};

HasPtr& HasPtr::operator=(const HasPtr& rhs) {
	++rhs.ptr->use;
	if (--ptr->use == 0)
		delete ptr;
	ptr = rhs.ptr;
	val = rhs.val;
	return *this;
}
```
这个类的所有成员均为 private。我们不希望普通用户使用 U_Ptr 类，所以它没有任何 public 成员。将 HasPtr 类设置为友元，使其成员可以访问 U_Ptr 的成员。

### 10.3.4 定义值型类
处理指针成员的另一个完全不同的方法，是给指针成员提供值语义。具有值语义的类所定义的对象，其行为很像算术类型的对象：复制值型对象时，会得到一个不同的新副本。对副本所做的改变不会反映在原有对象上，反之亦然。string 类是值类型的一个例子。
**要使指针成员表现得像一个值，复制 HasPtr 对象时必须复制指针所指向的对象：**
```C++
class HasPtr {
public:
	HasPtr(const int &p, int i): ptr(new int(p)), val(i) { }
	HasPtr(const HasPtr &orig): ptr(new int (*orig.ptr)), val(orig.val) { }
	HasPtr& operator=(const HasPtr&);
	~HasPtr() { delete ptr; }
	int get_ptr_val() const { return *ptr; }
	int get_int() const { return val; }
	void set_ptr(int* p) { ptr = p; }
	void set_int(int i) { val = i; }
	int* get_ptr() const { return ptr; }
	void set_ptr_val(int p) const { *ptr = p; }
private:
	int* ptr;
	int val;
};

HasPtr& HasPtr::operator=(const HasPtr& rhs) {
	*ptr = *rhs.ptr;
	val = rhs.val;
	return *this;
}
```
赋值操作符不需要分配新对象，它只是必须记得给其指针所指向的对象赋新值，而不是给指针本身赋值，换句话说，改变的是指针所指向的值，而不是指针。即使要将一个对象赋值给它本身，赋值操作符也必须总是保证正确。

> 分配内存或其他资源的类几乎总是需要定义复制控制成员来管理所分配的资源。如果一个类需要析构函数，则它几乎也总是需要定义复制构造函数和赋值操作符。

---



# 11.面向对象编程
> 通过**动态绑定**我们能编写程序使用继承层次中任意类型的对象，无须关心对象的具体类型。使用这些类的程序无须区分函数是在基类还是在派生类中定义的。

> **在 C++ 中，通过基类的引用（或指针）调用虚函数时，发生动态绑定。**引用（或指针）既可以指向基类对象也可以指向派生类对象，这一事实是动态绑定的关键。用引用（或指针）调用的虚函数在运行时确定，被调用的函数是引用（或指针）所指对象的实际类型所定义的。

>基类通常应将派生类需要重定义的任意函数定义为虚函数。

```C++
#include <iostream>

class Item_base {
public:
	Item_base(const std::string &book = "", double sales_price = 0.0): isbn(book), price(sales_price) { }
	std::string book() const { return isbn; }
	virtual double net_price(std::size_t n) const { return n * price; }    // 启用动态绑定
	virtual ~Item_base() { }
private:
	std::string isbn;
protected:
	double price;
};
```

> * 像 private 成员一样，protected 成员不能被类的用户访问。
> * 像 public 成员一样，protected 成员可被该类的派生类访问。
> * 派生类智能通过派生类对象访问其基类的 protected 成员，派生类对其基类类型对象的 protected 成员没有特殊访问权限。

```C++
void Bulk_item::memfcn(const Bulk_item& d, const Item_base& b) {
	double ret = price;           // ok: uses this->price
	ret = d.price;                // ok: uses price from a Bulk_item object
	ret = b.price;                // error: no access to price from an Item_base
}
```
d.price 的使用正确，因为是通过 Bulk_item 类型对象引用 price；b.price 的使用非法，因为对 Base_item 类型的对象没有特殊访问权限。



## 11.1 派生类

```C++
class Bulk_item : public Item_base {
public:
	double net_price(std::size_t) const;
private:
	std::size_t min_qty;
	double discount;
};

double Bulk_item::net_price(size_t cnt) const {
	if (cnt >= min_qty)
		return cnt * (1 - discount) * price;
	else
		return cnt * price;
}
```
派生类一般会重定义所继承的虚函数。如果派生类没有重定义某个虚函数，则使用基类中定义的版本。一旦函数在基类中声明为虚函数，它就一直为虚函数，派生类无法改变该函数为虚函数这一事实。



## 11.2 virtual 与其他成员函数

> C++ 中函数调用默认不使用动态绑定。要触发动态绑定，必须满足两个条件：第一，只有指定为虚函数的成员函数才能进行动态绑定，成员函数默认为非虚函数，非虚函数不进行动态绑定；第二，必须通过基类类型的引用或指针进行函数调用。

> 基类类型引用和指针的关键点在于**静态类型**(static type，在编译时可知的引用类型或指针类型）和**动态类型**（dynamic type，指针或引用所绑定的对象的类型，这是仅在运行时可知的）可能不同。

```C++
class Base {
public:
	void basemem();
protected:
	int i;
};

struct Public_derived : public Base {
	int use_base() { return i; }
};

struct Private_derived : private Base {
	int use_base() { return i; }
};

int main() {
	Base b;
	Public_derived d1;
	Private_derived d2;
	b.basemem();              // ok
	d1.basemem();             // ok
	d2.basemem();             // error
}
```

> 所有继承 Base 的类对 Base 中的成员具有相同的访问。派生访问标号将控制派生类的用户对从 Base 继承而来的成员的访问。



## 11.3 转换与继承

每个派生类对象包含一个基类的部分，这意味着可以像使用基类对象一样在派生类对象上执行操作。因为派生类对象也是基类对象，所以存在从派生类型引用到基类类型引用的自动转换，即，可以将派生类对象的引用转换为基类对象的引用，对指针也类似。

基类类型对象既可以作为独立对象存在，也可以作为派生类对象的一部分而存在，因此，一个基类对象可能是也可能不是一个派生类对象的部分，结果，没有从基类引用（或基类指针）到派生类引用（或派生类指针）的（自动转换）。

### 11.3.1 引用转换不同于转换对象
**可以将派生类型的对象传给希望接受基类引用的函数。此时，引用直接绑定到该对象，虽然看起来在传递对象，实际上实参是该对象的引用，对象本身未被复制，并且，转换不会在任何方面改变派生类型对象，该对象仍是派生类型对象。**

**将派生类对象传给希望接受基类类型对象（而不是引用）的函数时，情况完全不同。在这种情况下，形参的类型是固定的---在编译时和运行时形参都是基类类型对象。如果用派生类型对象调用这样的函数，则该派生类对象的基类部分被复制到形参。**

**一个是将派生类对象转换为基类类型的引用，一个是用派生类对象对基类对象进行初始化或赋值。**
### 11.3.2 用派生类对象对基类对象进行初始化或赋值
对基类对象进行初始化或赋值，实际上是在调用函数：初始化时调用构造函数，赋值时调用赋值操作符。



## 11.4 构造函数和复制控制

构造函数和复制控制成员不能继承，每个类定义自己的构造函数和复制控制成员。像任何类一样，如果类不定义自己的默认构造函数和复制控制成员，就将使用合成版本。

派生类的构造函数受继承关系的影响，每个派生类构造函数除了初始化自己的数据成员之外，还要初始化基类。

**即使析构函数没有工作要做，继承层次的根类也应该定义一个虚析构函数。**



## 11.5 句柄类与继承

C++中面向对象编程的一个颇具讽刺意味的地方是，不能使用对象支持面向对象编程，相反，必须使用指针或引用。

句柄类存储和管理基类指针。指针所指对象的类型可以变化，它既可以指向基类类型对象又可以指向派生类型对象。用户通过句柄类访问继承层次的操作。因为句柄类使用指针执行操作，虚成员的行为将在运行时根据句柄实际绑定的对象的类型而变化。因此，句柄的用户可以获得动态行为但无须操心指针的管理。

包装了继承层次的句柄有两个重要的设计考虑因素：

 - 像对任何保存指针的类一样，必须确定对复制控制做些什么。包装了继承层次的句柄通常表现得像一个智能指针或者像一个值。
 - 句柄类决定句柄接口屏蔽还是不屏蔽继承层次，如果不屏蔽继承层次，用户必须了解和使用基本层次中的对象。

```C++
class Sales_item {
public:
	Sales_item(): p(0), use(new std::size_t(1)) { }
	Sales_item(const Item_base&);
	Sales_item(const Sales_item& i) : p(i.p), use(i.use) { ++* use; }
	~Sales_item() { decr_use(); }
	Sales_item& operator=(const Sales_item&);
	const Item_base* operator->() const { if (p) return p; else throw std::logic_error("unbound Sales_item"); }
	const Item_base& operator*() const { if (p) return *p; else throw std::logic_error("unbound Sales_item"); }
private:
	Item_base* p;
	std::size_t* use;
	void decr_use() { if (-- * use == 0) { delete p; delete use; } }
};

class Item_base : public Sales_item {

};
```

---



# 12.模板与泛型编程

泛型编程与面向对象编程一样，都依赖于某种形式的多态性（面向对象所依赖的多态性称为运行时的多态性，泛型编程所依赖的多态性称为编译时多态性或参数式多态性。）

## 12.1 模板定义
### 12.1.1 函数模板
```c++
#include <iostream>
using namespace std;

template <typename T> int compare(const T& v1, const T& v2) {
	if (v1 > v2) return -1;
	if (v2 > v1) return 1;
	return 0;
}

int main() {
	cout << compare(1, 0) << endl;
	string s1 = "hi", s2 = "world";
	cout << compare(s1, s2) << endl;
	return 0;
}
```
### 12.1.2 类模板
```C++
#include <iostream>
using namespace std;

template <class Type> class QueueItem {
    friend class Queue<Type>;
	QueueItem(const Type &t): item(t), next(0) { }
	Type item;
	QueueItem* next;
};

template <class Type> class Queue {
public:
	Queue(): head(0), tail(0) { }
	Queue(const Queue& Q) : head(0), tail(0) { copy_elems(Q); }
	Queue& operator=(const Queue&);
	~Queue() { destroy(); }

	Type& front() { return head->item; }
	const Type& front() const { return head->item; }
	void push(const Type&);
	void pop();
	bool empty() const { return head == 0; }
private:
	QueueItem<Type>* head;
	QueueItem<Type>* tail;
	void destroy();
	void copy_elems(const Queue&);
};

template <class Type> void Queue<Type>::destroy() {
	while (!empty())
		pop();
}

template <class Type> void Queue<Type>::pop() {
	QueueItem<Type>* p = head;
	head = head->next;
	delete p;
}

template <class Type> void Queue<Type>::push(const Type& val) {
	QueueItem<Type>* pt = new QueueItem<Type>(val);
	if (empty())
		head = tail = pt;
	else {
		tail->next = pt;
		tail = pt;
	}
}

template <class Type> void Queue<Type>::copy_elems(const Queue& orig) {
	for (QueueItem<Type>* pt = orig.head; pt; pt = pt->next)
		push(pt->item);
}
```



### 12.1.3 非类型模板形参

模板形参不必都是类型，在调用函数时非类型形参将用值代替，值的类型在模板形参表中指定。例如，下面的函数模板声明了 array_init 是一个含有一个类型模板形参和非类型模板形参的函数模板。函数本身接受一个形参，该形参是数组的引用。

```c++
template <class T, size_t N> void array_init(T(&pram)[N]) {
    for (size_t i = 0; i < N; ++i) {
        pram[i] = 0;
    }
}
```

