

`friend` 关键字在 C++ 中是一个比较特别的存在。因为在大多数编程语言中是没有提供 `friend` 关键字的，比如Java。friend 关键字用于声明类的友元，友元可以无视类中成员的属性（ public、protected 或是 private ），友元类或友元函数都可以访问，这就`完全破坏了面向对象编程中封装性的概念`。但有的时候，friend 关键字确实会让程序猿少写很多代码，因此 friend 还是在很多程序中被使用到。



# 1. 语法改进



在 C++11 标准中对 friend 关键字进行了一些改进，以保证其更加好用：

**声明一个类为另外一个类的友元时，不再需要使用 class 关键字，并且还可以使用类的别名（使用 typedef 或者 using 定义）。**

我们可以看看下面的例子：

```c++
#include <iostream>
using namespace std;

// 类声明
class Tom;
// 定义别名
using Honey = Tom;

// 定义两个测试类
class Jack
{
    // 声明友元
    // friend class Tom;    // C++98 标准语法
    friend Tom;             // C++11 标准语法 
    string name = "jack";   // 默认私有
    void print()            // 默认私有
    {
        cout << "my name is " << name << endl;
    }
};

class Lucy
{
protected:
    // 声明友元
    // friend class Tom;    // C++98 标准语法
    friend Honey;           // C++11 标准语法 
    string name = "lucy";
    void print()
    {
        cout << "my name is " << name << endl;
    }
};

class Tom
{
public:
    void print()
    {
        // 通过类成员对象访问其私有成员
        cout << "invoke Jack private member: " << jObj.name << endl;
        cout << "invoke Jack private function: " << endl;
        jObj.print();

        cout << "invoke Lucy private member: " << lObj.name << endl;
        cout << "invoke Lucy private function: " << endl;
        lObj.print();
    }
private:
    string name = "tom";
    Jack jObj;
    Lucy lObj;
};

int main()
{
    Tom t;
    t.print();
    return 0;
}
```

在上面的例子中 `Tom 类` 分别作为了`Jack类` 和 `Lucy类` 的友元类，然后在 Tom类 中定义了Jack类 和Lucy类 的对象 `jObj` 和 `lObj`，这样我们就可以在 Tom 类中通过这两个类对象直接访问它们各自的私有或者受保护的成员变量或者成员函数了。



# 2. 为类模板声明友元



虽然在 C++11 标准中对友元的改进不大，却会带来应用的变化——程序员可以为类模板声明友元了，这在 C++98中是无法做到的。使用方法如下：

```c++
class Tom;

template<typename T>  
class Person
{
    friend T;
};

int main()
{
    Person<Tom> p;
    Person<int> pp;
    return 0;
}
```

* 第11行：`Tom` 类是 `Person` 类的友元
* 第12行：对于 `int` 类型的模板参数，友元声明被忽略（第6行）

这样一来，我们就可以在模板实例化时才确定一个模板类是否有友元，以及谁是这个模板类的友元。

下面基于一个实际场景来讲解一下如何给模板类指定友元：

假设有一个矩形类，一个圆形类，我们在对其进行了一系列的操作之后，需要验证一下矩形的宽度和高度、圆形的半径是否满足要求，并且要求这个校验操作要在另一个类中完成。

```c++
template<typename T>  
class Rectangle
{
public:
    friend T;
    Rectangle(int w, int h) : width(w), height(h) {}
private:
    int width;
    int height;
};

template<typename T> 
class Circle
{
public:
    friend T;
    Circle(int r) : radius(r) {}
private:
    int radius;
};

// 校验类
class Verify
{
public:
    void verifyRectangle(int w, int h, Rectangle<Verify> &r)
    {
        if (r.width >= w && r.height >= h)
        {
            cout << "矩形的宽度和高度满足条件!" << endl;
        }
        else
        {
            cout << "矩形的宽度和高度不满足条件!" << endl;
        }
    }

    void verifyCircle(int r, Circle<Verify> &c)
    {
        if (r >= c.radius)
        {
            cout << "圆形的半径满足条件!" << endl;
        }
        else
        {
            cout << "圆形的半径不满足条件!" << endl;
        }
    }
};

int main()
{
    Verify v;
    Circle<Verify> circle(30);
    Rectangle<Verify> rect(90, 100);
    v.verifyCircle(60, circle);
    v.verifyRectangle(100, 100, rect);
    return 0;
}
```

* 第28行：在 `Verify 类`中 访问了 `Rectangle 类` 的私有成员变量
* 第40行：在 `Verify类` 中 访问了 `Circle类` 的私有成员变量

程序输出的结果：

```c++
圆形的半径满足条件!
矩形的宽度和高度不满足条件!
```

在上面的例子中我们定义了两个类模板 `Rectangle和Circle` 并且将其模板类型定义为了它们的友元（如果是模板类型是基础类型友元的定义就被忽略了）。在 `main()函数` 中测试的时候将 `Verify类` 指定为了两个模板类的实际友元类型。这样我们在 `Verify类` 中就可以通过 `Rectangle类和Circle类` 的实例对象访问它们内部的私有成员变量了。

补充说明：

1. 在上面的测试程序中 `Rectangle类和Circle类` 我们没有提供对应的 `set方法` 来设置私有成员的值，为了简化程序直接通过构造函数的初始化列表完成了它们的初始化。
2. 在上面的程序中也没有给 `Rectangle类和Circle类` 提供 `get 方法`，这样如果想要在类外部访问私有（或受保护）成员就只能使用友元了（此处这样处理完全了为了测试的需要）。