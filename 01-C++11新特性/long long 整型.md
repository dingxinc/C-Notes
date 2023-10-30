

相比于 C++98 标准，C++11 整型的最大改变就是多了 `long long`。但事实上，long long  整型本来就离 C++ 标准很近，早在 1995 年，long long 就被提议写入 C++98 标准，却被 C++ 标准委员会拒绝了。而后来，long long 类型却进入了 C99 标准，而且也事实上也被很多编译器支持。于是辗转地，C++ 标准委员会又掉头决定将 long long 纳入 C++11 标准。



# 1. long long 类型



C++11 标准要求 long long 整型可以在不同平台上有不同的长度，但`至少有 64 位`。long long 整型有两种∶

* `long long - 对应类型的数值可以使用 LL (大写) 或者 ll (小写) 后缀`

  ```c++
  long long num1 = 123456789LL;
  long long num2 = 123456789ll;
  ```

* `unsigned long long - 对应类型的数值可以使用 ULL (大写) 或者 ull (小写) 或者 Ull、uLL (等大小写混合)后缀`

  ```c++
  unsigned long long num1 = 123456789ULL;
  unsigned long long num2 = 123456789ull;
  unsigned long long num3 = 123456789uLL;
  unsigned long long num4 = 123456789Ull;
  ```



事实上在 C++11 中还有一些类型与以上两种类型是等价的:

* 对于有符号类型的 long long 和以下三种类型等价
  * `long long int`
  * `signed long long`
  * `signed long long int`
* 对于无符号类型的 `unsigned long long` 和 `unsigned long long int` 是等价的

同其他的整型一样，要了解平台上 long long 大小的方法就是查看 `<climits>（或<limits. h> ）`中的宏与 long long 整 型相关的一共有 3 个：

* `LLONG_MIN` - 最小的 long long 值
* `LLONG_MAX` - 最大的 long long 值
* `ULLONG MAX` - 最大的 unsigned long long 值



测试代码如下:

```c++
#include <iostream>
using namespace std;

int main()
{
    long long max = LLONG_MAX;
    long long min = LLONG_MIN;
    unsigned long long ullMax = ULLONG_MAX;

    cout << "Max Long Long value: " << max << endl
        << "Min Long Long value: " << min << endl
        << "Max unsigned Long Long value: " << ullMax << endl;
    return 0;
}
```

程序输出的结果:

```c++
Max Long Long value: 9223372036854775807
Min Long Long value: -9223372036854775808
Max unsigned Long Long value: 18446744073709551615
```

**可以看到  long long 类型能够存储的最大/最小值还是非常大/小的，但是这个值根据平台不同会有所变化，原因是因为 C++11 标准规定该类型至少占 8 字节，它占的字节数越多，对应能够存储的数值也就越大。**



# 2. 扩展的整型



在 C++11 中一共只定义了以下 5 种标准的有符号整型：

*  `signed char`
* `short int`
* `int`
* `long int`
* `long long int`

标准同时规定，每一种有符号整型都有一种对应的无符号整数版本，且有符号整型与其对应的无符号整型具有相同的存储空间大小。比如与 signed int 对应的无符号版本的整型是 unsigned int。

当我们在 C++ 中处理数据的时候，如果参与运算的数据或者传递的参数类型不匹配，整型间会发生隐式的转换，这种过程通常被称为整型的提升。比如如下表达式∶

```c++
(int)num1 + (long long)num2
```

关于这种整形提升的隐式转换遵循如下原则:

* 长度越大的整型等级越高，比如 `long long int` 的等级会高于 `int`。
* 长度相同的情况下，标准整型的等级高于扩展类型，比如 `long long int` 和 `int64` 如果 都是 64 位长度，则 `long long int` 类型的等级更高。 
* 相同大小的有符号类型和无符号类型的等级相同，`long long int` 和 `unsigned longlong  int` 的等级就相同。
* **转换过程中，低等级整型需要转换为高等级整型，有符号的需要转换为无符号整形。**