

# 1. 类与默认函数



在 C++ 中声明自定义的类，编译器会默认帮助程序员生成一些他们未自定义的成员函数。这样的函数版本被称为”默认函数”。这样的函数一共有六个，我们一起来看一下：

1. `无参构造函数`：创建类对象
2. `拷贝构造函数`：拷贝类对象
3. `移动构造函数`：拷贝类对象
4. `拷贝赋值函数`：类对象赋值
5. `移动赋值函数`：类对象赋值
6. `析构函数` ：销毁类对象

**在 C++ 语法规则中，一旦程序员实现了这些函数的自定义版本，则编译器不会再为该类自动生成默认版本。**

有时程序员会忘记上面提到的规则，最常见的是声明了带参数的构造，如果还需要无参构造函数，这时候必须定义出不带参数的版本。不过通过编译器的提示，这样的问题通常会得到更正。但更为严重的问题是，一旦声明了自定义版本的构造函数，则有可能导致我们定义的类型不再是 [[POD 类型]]，我们便不再能够享受 POD 类型为我们带来的便利。

对于上面提到的这些，我们无需过度担心，因为 C++11 非常贴心地为我们提供了解决方案，就是使用 `=default` 。



# 2. =default 和 =delete



在 C++11 标准中称 `= default` 修饰的函数为`显式默认【缺省】（explicit defaulted）函数`，而 `=delete` 修饰的函数为 `删除（deleted）函数或者显示删除函数`。

 C++11 引入显式默认和显式删除是为了增强对类默认函数的控制，让程序员能够更加精细地控制默认版本的函数。



## 2.1 =default

我们可以在类内部修饰满足条件的类函数为显示默认函数，也可以在类定义之外修饰成员函数为默认函数。下面举例说明：



**在类内部指定函数为默认函数**

一般情况下，我们可以在定义类的时候直接在类内部指定默认函数，如下所示：

```c++
class Base
{
public:
    Base() = default;
    Base(const Base& obj) = default;
    Base(Base&& obj) = default;
    Base& operator= (const Base& obj) = default;
    Base& operator= (Base&& obj) = default;
    ~Base() = default;
};
```

* 第4行：指定无参构造为默认函数
* 第5行：指定拷贝构造函数为默认函数
* 第6行：指定移动构造函数为默认函数
* 第7行：指定复制赋值操作符重载函数为默认函数
* 第8行：指定移动赋值操作符重载函数为默认函数
* 第9行：指定析构函数为默认函数

**使用 =defaut 指定的默认函数和类提供的默认函数是等价的**



**在类外部指定函数为默认函数**

默认函数除了在类定义的内部指定，也可以在类的外部指定，如下所示：

```c++
// 类定义
class Base
{
public:
    Base();
    Base(const Base& obj);
    Base(Base&& obj);
    Base& operator= (const Base& obj);
    Base& operator= (Base&& obj);
    ~Base();
};
// 在类定义之外指定成员函数为默认函数
Base::Base() = default;
Base::Base(const Base& obj) = default;
Base::Base(Base&& obj) = default;
Base& Base::operator= (const Base& obj) = default;
Base& Base::operator= (Base&& obj) = default;
Base::~Base() = default;
```



**定义默认函数的注意事项:**

> **如果程序猿对 C++ 类提供的默认函数（上面提到的六个函数）进行了实现，那么可以通过 =default 将他们再次指定为默认函数，不能使用 =default 修饰这六个函数以外的函数。**

```c++
class Base
{
public:
    Base() = default;
    Base(const Base& obj) = default;
    Base(Base&& obj) = default;
    Base& operator= (const Base& obj) = default;
    Base& operator= (Base&& obj) = default;
    ~Base() = default;

    // 以下写法全部都是错误的
    Base(int a = 0) = default;
    Base(int a, int b) = default;
    void print() = default;
    bool operator== (const Base& obj) = default;
    bool operator>=(const Base& obj) = default;
};
```

* 第12行：自定义带参构造，不允许使用 `=default` 修饰（即使有默认参数也不行）
* 第13行：自定义带参构造，不允许使用 `=default` 修饰
* 第14行：自定义函数，不允许使用 `=default` 修饰
* 第15、16行：不是移动、复制赋值运算符重载，不允许使用 `=default` 修饰



## 2.2 =delete

`=delete` 表示显示删除，`显式删除可以避免用户使用一些不应该使用的类的成员函数`，使用这种方式可以有效的防止某些类型之间自动进行隐式类型转换产生的错误。下面举例说明：

**禁止使用默认生成的函数**

```c++
class Base
{
public:
    Base() = default;
    Base(const Base& obj) = delete;
    Base& operator= (const Base& obj) = delete;
};

int main()
{
    Base b;
    Base tmp1(b);    // error
    Base tmp = b;    // error
    return 0;
}
```

* 第5行：禁用拷贝构造函数
* 第6行：禁用 = 进行对象复制
* 第12行：拷贝构造函数已被显示删除，无法拷贝对象
* 第13行：复制对象的赋值操作符重载函数已被显示删除，无法复制对象



**禁止使用自定义函数**

```c++
class Base
{
public:
    Base(int num) : m_num(num) {}
    Base(char c) = delete;
    void print(char c) = delete;
    void print()
    {
        cout << "num: " << m_num << endl;
    }
    void print(int num)
    {
        cout << "num: " << num << endl;
    }
private:
    int m_num;
};

int main()
{
    Base b(97);       // 'a' 对应的 acscii 值为97
    Base b1('a');     // error
    b.print();
    b.print(97);
    b.print('a');     // error
    return 0;
}
```

* 第5行：禁用带 `char`类型参数的构造函数，防止隐式类型转换（char转int)
* 第6行：禁止使用带 `char `类型的自定义函数，防止隐式类型转换（char转int)
* 第22行：对应的构造函数被禁用，因此无法使用该构造函数构造对象
* 第25行：对应的打印函数被禁用，因此无法给函数传递 `char` 类型参数

