

# 1. POD 类型



POD 是英文中 **Plain Old Data** 的缩写，翻译过来就是**普通的旧数据** 。POD在 C++ 中是非常重要的一个概念，`通常用于说明一个类型的属性，尤其是用户自定义类型的属性。`

POD 属性在 C++11 中往往又是构建其他 C++ 概念的基础，事实上，在 C++11 标准中，POD 出现的概率相当高。因此学习 C++，尤其是在  C++11 中，了解  POD 的概念是非常必要的。

1. `Plain` ：表示是个普通的类型
2. `Old` ：体现了其与 C 的兼容性，支持标准 C 函数

在 C++11 中将  POD 划分为两个基本概念的合集，即∶`平凡的（trivial）` 和`标准布局的（standard layout ）` 。 



# 2. “平凡” 类型



一个平凡的类或者结构体应该符合以下几点要求：

1. **拥有平凡的默认构造函数（trivial constructor）和析构函数（trivial destructor）。**

   平凡的默认构造函数就是说构造函数`什么都不干`。

   * 通常情况下，`不定义类的构造函数`，编译器就会为我们`生成一个平凡的默认构造函数。`

     ```c++
     // 使用默认的构造函数
     class Test {};
     ```

   * `一旦定义了构造函数`，即使构造函数不包含参数，函数体里也没有任何的代码，`那么该构造函数也不再是"平凡"的。`

     ```c++
     class Test1 
     {
         Test1();	// 程序猿定义的构造函数, 非默认构造
     };
     ```

     

关于析构函数也和上面列举的构造函数类似，一旦被定义就不平凡了。但是这也并非无药可救，使用 [[默认函数控制 =default 与 =delete#2.1 =default | =default]] 关键字可以显式地声明默认的构造函数，从而使得类型恢复 “平凡化”。



2. **拥有平凡的拷贝构造函数（trivial copy constructor）和移动构造函数（trivial move constructor）。**
   * 平凡的拷贝构造函数基本上等同于使用 memcpy 进行类型的构造。
   * 同平凡的默认构造函数一样，不声明拷贝构造函数的话，编译器会帮程序员自动地生成。
   * 可以显式地使用 =default 声明默认拷贝构造函数。 
   * 而平凡移动构造函数跟平凡的拷贝构造函数类似，只不过是用于移动语义。



3. **拥有平凡的拷贝赋值运算符（trivial assignment operator）和移动赋值运算符（trivial move operator）。**

   这基本上与平凡的拷贝构造函数和平凡的移动构造运算符类似。



4. **不包含虚函数以及虚基类。**

   * 类中使用 `virtual` 关键字修饰的函数 叫做虚函数

     ```c++
     class Base 
     {
     public:
         Base() {}
         virtual void print() {}
     };
     ```

   * 虚基类是在`创建子类的时候在继承的基类前加virtual 关键字` 修饰

     ```c++
     语法: class 派生类名：virtual  继承方式  基类名
     ```

   示例代码：

   ```c++
   class Base 
   {
   public:
       Base() {}
   };
   // 子类Child，虚基类：Base
   class Child : virtual public Base 
   {
       Child() {}
   };
   ```

   



# 3. “标准布局” 类型



标准布局类型主要主要指的是 `类` 或者 `结构体` 的结构或者组合方式。

标准布局类型的类应该符合以下五点定义，**最重要的为前两条：**

1. 所有非静态成员有**相同** 的访问权限（public，private，protected）。

   * 类成员拥有不同的访问权限（`非标准布局类型`）

   ```c++
   class Base
   {
   public:
       Base() {}
       int a;
   protected:
       int b;
   private:
       int c;
   };
   ```

   * 类成员拥有相同的访问权限（`标准布局类型`）

   ```c++
   class Base
   {
   public:
       Base() {}
       int a;
       int b;
       int c;
   };
   ```

   

2. **在类或者结构体继承时，满足以下两种情况之一∶**

   *  **派生类中有非静态成员，基类中包含静态成员（或基类没有变量）。**
   * **基类有非静态成员，而派生类没有非静态成员。**

   ```c++
   struct Base { static int a;};
   struct Child: public Base{ int b;};          // ok
   struct Base1 { int a;};
   struct Child1: public Base1{ static int c;}; // ok
   struct Child2:public Base, public Base1 { static int d;); // ok
   struct Child3:public Base1{ int d;};         // error
   struct Child4:public Base1, public Child     // error
   {
       static int num;
   };
   ```

   > 通过上述例子得到的结论：
   >
   > 1. `非静态成员只要同时出现在派生类和基类间，即不属于标准布局。`
   > 2. `对于多重继承，一旦非静态成员出现在多个基类中，即使派生类中没有非静态成员变量，派生类也不属于标准布局。`



3. **子类中第一个非静态成员的类型与其基类不同。** 

   此处基于 G++ 编译器讲解，如果使用 VS 的编译器和 G++ 编译器得到的结果是不一样的。

   ```c++
   struct Parent{};
   struct Child : public Parent
   {
       Parent p;	// 子类的第一个非静态成员
       int foo;
   };
   ```

   上面的例子中 `Child` 不是一个标准布局类型，因为它的第一个非静态成员变量p和父类的类型相同，改成下面这样子类就变成了一个标准布局类型：

   ```c++
   struct Parent{};
   struct Child1 : public Parent
   {
       int foo;   // 子类的第一个非静态成员
       Parent p;	
   };
   ```

   这条规则对于我们来说是比较特别的，这样规定的目的主要是是节约内存，提高数据的读取效率。对于上面的两个子类 `Child`  和  `Child1` 来说它们的内存结构是不一样的，**在基类没有成员的情况下：**

   * C++标准允许`标准布局类型（Child1）`派生类的第一个`成员foo与基类共享地址`，此时基类并没有占据任何的实际空间（可以节省一点数据）
   * 对于子类 `Child` 而言，如果子类的第一个成员仍然是基类类型，C++ 标准要求类型相同的对象它们的地址必须不同（`基类地址不能和子类中的变量 p 类型相同`），此时需要分配额外的地址空间将二者的地址错开。

   ![image-20230918183720468](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309181837542.png)



4. **没有虚函数和虚基类。**
5.  **所有非静态数据成员均符合标准布局类型，其基类也符合标准布局，这是一个递归的定义。**



# 4. 对 POD 类型的判断



如果我们想要判断某个数据类型是不是属于 POD 类型，可以使用 C++11 给我们提供的相关函数：



## 4.1 对 “平凡” 类型判断

C++11 提供的类模板叫做 `is_trivial`，其定义如下：

```c++
template <class T> struct std::is_trivial;
```

`std::is_trivial` 的成员 `value` 可以用于判断T的类型是否是一个平凡的类型（`value 函数返回值为布尔类型`）。除了类和结构体外，`is_trivial` 还可以对内置的标准类型数据（比如int、float都属于平凡类型）及数组类型（元素是平凡类型的数组总是平凡的）进行判断。

关于类型的判断，示例程序如下：

```c++
#include <iostream>
#include <type_traits>
using namespace std;

class A {};
class B { B() {} };
class C : B {};
class D { virtual void fn() {} };
class E : virtual public A { };

int main() 
{
    cout << std::boolalpha;
    cout << "is_trivial:" << std::endl;
    cout << "int: " << is_trivial<int>::value << endl;
    cout << "A: " << is_trivial<A>::value << endl;
    cout << "B: " << is_trivial<B>::value << endl;
    cout << "C: " << is_trivial<C>::value << endl;
    cout << "D: " << is_trivial<D>::value << endl;
    cout << "E: " << is_trivial<E>::value << endl;
    return 0;
}
```

输出的结果：

```c++
is_trivial:
int: true
A: true
B: false
C: false
D: false
E: false
```

* `int` ：内置标准数据类型，属于 trivial 类型
* `A` ：拥有默认的构造和析构函数，属于 trivial 类型
* `B` ：自定义了构造函数，因此不属于 trivial 类型
* `C` ：基类中自定义了构造函数，因此不属于 trivial 类型
* `D` ：类成员函数中有虚函数，因此不属于 trivial 类型
* `E` ：继承关系中有虚基类，因此不属于 trivial 类型



## 4.2 对 “标准布局” 类型的判断

同样，在 C++11 中，我们可以使用模板类来帮助判断类型是否是一个标准布局的类型，其定义如下：

```c++
template <typename T> struct std::is_standard_layout;
```

通过 `is_standard_layout` 模板类的成员 `value（is_standard_layout<T>∶∶value）`，我们可以在代码中打印出类型的标准布局属性，函数返回值为布尔类型。

示例程序

```c++
// pod.cpp
#include <iostream>
#include <type_traits>
using namespace std;

struct A { };
struct B : A { int j; };
struct C
{
public:
    int a;
private:
    int c;
};
struct D1 {  static int i; };
struct D2 {  int i; };
struct E1 { static int i; };
struct E2 { int i; };
struct D : public D1, public E1 { int a; };
struct E : public D1, public E2 { int a; };
struct F : public D2, public E2 { static int a; };
struct G : public A
{
    int foo;
    A a;
};
struct H : public A
{
    A a;
    int foo;
};

int main() 
{
    cout << std::boolalpha;
    cout << "is_standard_layout:" << std::endl;
    cout << "A: " << is_standard_layout<A>::value << endl;
    cout << "B: " << is_standard_layout<B>::value << endl;
    cout << "C: " << is_standard_layout<C>::value << endl;
    cout << "D: " << is_standard_layout<D>::value << endl;
    cout << "D1: " << is_standard_layout<D1>::value << endl;
    cout << "E: " << is_standard_layout<E>::value << endl;
    cout << "F: " << is_standard_layout<F>::value << endl;
    cout << "G: " << is_standard_layout<G>::value << endl;
    cout << "H: " << is_standard_layout<H>::value << endl;
    return 0;
}
```

**VS2019 输出的结果**

```c++
is_standard_layout:
A: true
B: true
C: false
D: true
D1: true
E: false
F: false
G: false
H: false
```

**G++ 编译输出的结果:**

* 编译命令

  ```shell
  $ g++ pod.cpp -std=c++11
  ```

* 输出的结果

  ```c++
  is_standard_layout:
  A: true
  B: true
  C: false
  D: true
  D1: true
  E: false
  F: false
  G: true
  H: false
  ```

  

**关于输出的结果**

* `A` ：没有虚基类和虚函数，属于 standard_layout 类型
* `B` ：没有虚基类和虚函数，属于 standard_layout 类型
* `C` ：所有非静态成员访问权限不一致，不属于 standard_layout 类型
* `D` ：基类和子类没有同时出现非静态成员变量，属于 standard_layout 类型
* `D1` ：没有虚基类和虚函数，属于 standard_layout 类型
* `E` ：基类和子类中同时出现了非静态成员变量，不属于 standard_layout 类型
* `F` ：多重继承中在基类里同时出现了非静态成员变量，不属于 standard_layout 类型
* `G` ：使用的编译器不同，得到的结果也不同。
* `H` ：子类中第一个非静态成员的类型与其基类类型不能相同，不属于 standard_layout 类型



# 5. 总结



事实上，我们使用的很多内置类型默认都是 POD 的。POD 最为复杂的地方还是在类或者结构体的判断。那么，使用POD有什么好处呢？

1. 字节赋值，代码中我们可以安全地使用 memset 和 memcpy 对 POD 类型进行初始化和拷贝等操作。
2.  提供对 C 内存布局兼容。C++ 程序可以与 C 函数进行相互操作，因为 POD 类型的数据在 C 与 C++ 间的操作总是安全的。 
3. 保证了静态初始化的安全有效。静态初始化在很多时候能够提高程序的性能，而POD 类型的对象初始化往往更加简单。 

