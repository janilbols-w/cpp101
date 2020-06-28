# Tutorial for `Class` usage
## 0 Source
https://bbs.csdn.net/topics/300237086  
https://www.cnblogs.com/gentle-min-601/p/9556920.html  
https://www.cnblogs.com/theda/p/11907088.html  

## 1 单继承
由基类得到派生类。

派生方式：默认为私有继承

public 公有继承

private 私有继承

protected 保护继承

 

注意！！

派生类不能继承基类的构造函数和析构函数

 

单继承示例代码
```cpp
#include <iostream>
#include<string>
#include<cstring>
using namespace std;
class Student
{
private:
    int number;
    string name;
public:
    Student() { number = 0; name= ""; }
    void SetValue(int n, string s1)
    {
        number = n;
        name = s1;
    }
    void Print()
    {
        cout << "Numver:" << number << endl;
        cout << "Name:" << name << endl;
    }
};
class UGStudent :public Student
{
private:
    int age; int grade;
public:
    UGStudent() { SetValue(0, ""); age = 0; grade = 0; }
    UGStudent(int n, string s1, int a, int g)
    {
        SetValue(n,s1);
        age = a; grade = g;
    }
    void PrintExtra()
    {
        cout << "Age:" << age << endl;
        cout << "Grade:" << grade << endl;
    }
};
int main()
{
    UGStudent st1(100, "wang", 18, 1);
    st1.Print(); //调用基类的函数
    st1.PrintExtra(); //调用派生类的新定义的函数
    return 0;
};
```

## 2 单继承的访问权限控制
### 2.1 公有继承的访问权限控制
基类的public和protected成员的访问属性在派生 类中保持不变，但基类的private成员不可访问。  
派生类中的成员函数可以直接访问基类中的 public和protected成员，但不能访问基类的 private成员。  
通过派生类的对象只能访问基类的public成员。  
```cpp
#include <iostream>
#include<string>
#include<cstring>
using namespace std;
class Person //定义基类Person
{
public: //外部接口
    Person(const string Name, int Age, char Sex); //基类构造函数
    string GetName()
    {
        return name;
    }
    int GetAge(); //基类成员函数的声明
    char GetSex();
    void Display();
private:
    string name;
    char sex;
protected: //保护成员
    int age;
};
Person::Person(const string Name, int Age, char Sex)
//基类构造函数的实现
{
    name = Name;
    age = Age;
    sex = Sex;
}
//基类成员函数的实现
int Person::GetAge()
{
    return(age);
}
char Person::GetSex()
{
    return(sex);
}
//基类成员函数的实现
void Person::Display()
{
    //直接访问本类私有成员
    cout << "name:" << name << '\t'; cout << "age:" << age << '\t';
    cout << "sex:" << sex << endl;
}
class Student :public Person //定义公有继承的学生类
{
public: //外部接口
    Student(string pName, int Age, char Sex, string pId, float Score) :
        Person(pName, Age, Sex)
        //调用基类的构造函数初始化基类的数据成员
    {
        id=pId; //学生类的数据初始化
        score = Score;
    }
    string GetId(string pId) //派生类的新成员
    {
        return(id);
    }
    float GetScore() //派生类的新成员
    {
        return score;
    }
    void Display(); //派生类的新成员
private:
    string id;
    float score;
};
void Student::Display() //派生类的成员函数的实现
{
    cout << "id:" << id << '\t'; //直接访问本类私有成员
    cout << "age:" << age << '\t'; //访问基类的保护成员
    cout << "score:" << score << endl;
}
int main()
{
    string name;
    cout << "Enter a person′s name:";
    cin >> name;
    Person p1(name, 29, 'm'); //基类对象
    p1.Display(); //基类对象访问基类公有成员函数
    string pId;
    cout << "Enter a student′s name:";
    cin >> name;
    Student s1(name, 19, 'f', "03410101", 95); //派生类对象
    //派生类对象访问基类成员函数
    cout << "name:" << s1.GetName() << '\t';
    cout << "id:" << s1.GetId(pId) << '\t';
    cout << "age:" << s1.GetAge() << '\t';
    cout << "sex:" << s1.GetSex() << '\t';
    cout << "score:" << s1.GetScore() << endl;
    return 0;
}
```
### 2.2 私有继承的访问权限控制
基类的public和protected成员都以private身份出现 在派生类中，但基类的private成员不可访问。  
派生类中的成员函数可以直接访问基类中的public 和protected成员，但不能访问基类的private成员。  
通过派生类的对象不能访问基类中的任何成员。  
私有继承之后，基类的成员再也无法在以后的派生 类中发挥作用，出于这种原因，一般不使用私有继 承方式。  

好处：基类原有的外部接口被派生类封闭和隐藏起来  
```cpp
#include <iostream>
#include<string>
#include<cstring>
using namespace std;
class Parent //定义基类Parent
{
protected:
    int x;
public:
    void setx()
    {
        x = 0;
    }
    void display()
    {
        cout << "x=" << x << endl;
    }
};
class Son :private Parent //定义派生类Son
{
public:
    void increase() //新增成员函数
    {
        x++; //有效
    }
};
int main()
{
    Son s;
    //s.setx(); //错误：不能访问基类的任何成员
    s.increase(); //有效
    //s.display(); //错误：不能访问基类的任何成员
    return 0;
}
```
```cpp
#include <iostream>
#include<string>
#include<cstring>
using namespace std;
class Parent //定义基类Parent
{
public:
    int pubD;
protected:
    int protD;
private:
    int privD;
};
class Son :public Parent //定义公有派生类Son
{
public:
    void fn()
    {
        int x;
        x = pubD; //有效
        x = protD; //有效
        //x = privD; //错误：不能访问
    }
};
class Daughter :private Parent //定义私有派生类
{
public:
    void fn()
    {
        int y;
        y = pubD; //有效，但pubD被降为私有
        y = protD; //有效，但pubD被降为私有
        //y = privD; //错误
    }
};
int main()
{
    Parent p; //基类对象
    p.pubD = 10; //有效
    //p.protD = 10; //错误
    //p.privD = 10; //错误
    Son s; //公有派生类对象
    s.pubD = 20; //有效
    //s.protD = 20; //错误
    //s.privD = 20; //错误
    Daughter d; //私有派生类对象
    //d.pubD = 30; //错误：不能访问，pubD已降为私有
    //d.protD = 30; //错误
    //d.privD = 30; //错误
    return 0;
}
```
### 2.3 保护继承的访问权限控制
基类的public和protected成员都以protected身份出 现在派生类中，但基类的private成员不可访问  
派生类中的成员函数可以直接访问基类中的public 和protected成员，但不能访问基类的private成员。  
通过派生类的对象不能访问基类中的任何成员。  
保护继承与私有继承的区别在于能否进一步将基类 成员传递给派生类的派生类，保护继承可以传递部 分基类成员，但私有继承不可以。  
 ```cpp
 #include <iostream>
using namespace std;
class demo //定义基类demo
{
protected:
int j;
public:
demo( ) { j = 0; }
void add(int i) { j += i; }
void sub(int i) { j -= i ; }
void display() {cout<<"current value of j is "<<j<<endl; }
};
class child: protected demo
{ public:
void sub(int i) { j -= i ;} //有效：访问基类中的保护数据成员
};
int main( )
{
chlid object;
object.display();
object.add(10);
object.display();
object.sub(5);
object.display();
return 0;
}
 ```
 ## 3 protected 成员的特点与作用  
对建立其所在类对象的模块来说（水平访问时）， 它与 private 成员的性质相同。  
对于其派生类来说（垂直访问时），它与 public 成 员的性质相同。  
既实现了数据隐藏，又方便继承，实现代码重用。  

比如这样，访问就是错误的
```cpp
// 例 protected 成员举例
class A
{
protected:
int x;
}
int main（ ）
{
A a;
a.X=5; //错误
}
```
正确
```cpp
class A {
protected:
int x;
}
class B: public A{
public:
void Function（ ）;
};
void B:Function（ ）
{
x =5; //正确
}
```
错误
```cpp
class A {
private:
int x;
}
class B: public A{
public:
void Function（ ）;
};
void B:Function（ ）
{
x =5; //错误：不能访问基类中的私有成员
}
```
## 4 派生类的构造 函数和析构函数
### 4.1 派生类的构造函数
基类的构造函数不被继承，需要在派生类 中自行声明。  
声明构造函数时，只需要对本类中新增成 员进行初始化，对继承来的基类成员的初 始化由基类完成。  
也就是各自的构造函数各自干自己的活  
重要 构造函数的调用顺序  
```cpp
#include <iostream>
using namespace std;
class A
{
public:
A( ){cout<<"A Constructor"<<endl;}
};
class B:public A
{
public:
B( ){cout<<"B Constructor"<<endl;}
};
int main()
{ B b; reurn 0;
}
```
运行结果为 先A后B  
定义派生类对象时，构造函数的调用顺序：  
首先调用基类的构造函数；  
再调用对象成员类的构造函数；  
最后调用派生类的构造函数  
  
注意别忘了对象成员  
```cpp
#include <iostream>
using namespace std;

class component
{
public:
    component() {cout << "construct component" << endl;};
    ~component() {cout<< "destruct component" << endl;};
};

class base
{
public:
    base() {cout << "construct base" << endl;};
    ~base() {cout << "destruct base" << endl;};
};

class derived
    :base
{
public:
    derived() {cout << "construct derived" << endl;};
    ~derived() {cout << "destruct derived" << endl;};
    component c;
};

int main()
{
    derived d;
    return 0;
}

/*运行结果为
construct base
construct component
construct derived
destruct derived
destruct component
destruct base
*/
```
### 4.2 派生类构造函数的构造规则
4.2.1 在基类中定义了默认构造函数，且该默认构造函 数可以完成派生类对象中 基类成员的构造，则派生 类构造函数的定义可以省略对基类构造函数的显式 调用，而采用隐含调用。
准确的说，默认构造函数就是在调用时不需要显示地传入实参的构造函数。
可以不带形参，也可以有默认形参

隐含调用就是看起来没有调用基类的构造函数。
```cpp
#include <iostream>
using namespace std;
class Base
{ public:
Base() { a=0; }
Base( int i) { a=i; }
protected:
int a;
};
class Derived:public Base
{ public:
Derived() { b=0; }
Derived( int i) { b=i; }
void Print() { cout<<"a="<<a<<",b="<<b<<endl; }
private:
int b;
};
int main( )
{ Derived d1; Derived d2(12);
d1.Print(); d2.Print();
cin.ignore(); return 0;
}
/*result
00 和0 12
*/
```

4.2.2 当基类的构造函数使用一个或多个参数时，或者 基类的默认构造函数不能完成基类成员的构造时， 派生类必须定义构造函数，
显式调用基类构造函数， 提供将参数传递给基类构造函数的途径。这时，派 生类构造函数的函数体可能为空，仅起到参数传递 作用。 这时需要用到“成员初始化列表”
```cpp
#include <iostream>
using namespace std;
class A
{
public:
    A() { cout << "A Constructor1" << endl; }
    A(int i) { x1 = i; cout << "A Constructor2" << endl; }
    void dispa() { cout << "x1=" << x1 << endl; }
private:
    int x1;
};
class B :public A
{
public:
    B() { cout << "B Constructor1" << endl; }
    B(int i) :A(i + 10)//没加这个东西也不会报错
        //定义派生类构造函数时，传递参数到基类构造函数
    {
        x2 = i; cout << "B Constructor2" << endl;
    }
    void dispb()
    {
        dispa(); //调用基类成员函数
        cout << "x2=" << x2 << endl;
    }
private:
    int x2;
};
int main()
{
    B b(2);
    b.dispb();//12 2



    B b1;
    b1.dispb();//乱码
    return 0;
}
```
注意派生类中构造函数的写法。
```cpp
//派生类构造函数显式调用基类构造函数
#include <iostream>
#include <cstring>
using namespace std;
//日期类
class date
{
private:
    int year, mon, day;
public:
    date(int y = 2011, int m = 10, int d = 1)
    {
        year = y; mon = m; day = d;
    }
    void Print()
    {
        cout << year << "-" << mon << "-" << day << endl;
    }
};
//学生基类
class Student
{
protected:
    int number; char name[20]; char sex;
public:
    Student()
    {
        number = 0;
        strcpy(name, "No name "); sex = 'M';
    }
    Student(int n, char *s, char x)
    {
        number = n;
        strcpy(name, s); sex = x;
    }
};
//大学生派生类
class UGStudent :public Student
{
public:
    UGStudent(int n, char *s, char x, int a, int y, int m, int d)
        :Student(n, s, x), birth(y, m, d)
    {
        age = a;
    }
    UGStudent()
    {
        age = 0;
    }
    void Print()
    {
        cout << "number:" << number << endl;
        cout << "name:" << name << endl;
        cout << "sex:" << sex << endl;
        cout << "age:" << age << endl;
        cout << "birthday:";
        birth.Print();
    }
private:
    int age;
    date birth;
};
int main()
{
    UGStudent st1;
    UGStudent st2(1001, "Zhang", 'F', 20, 1991, 6, 11);
    st1.Print();
    st2.Print();
    cin.ignore();
    return 0;
}
/*
number:0
name:No name
sex:M
age:0
birthday:2011-10-1
number:1001
name:Zhang
sex:F
age:20
birthday:1991-6-11
*/

## 5 多继承
```cpp
class 派生类名:继承方式1 基类名1,…,继承方式n 基类名n { 派生类新定义成员 };
```
### 5.1 多继承下构造函数调用顺序
（1）先调用所有基类的构造函数，再调用派生类的构造函数。  
（2）处于同一层次的各基类构造函数的调用顺序取决于定义 派生类时所指定的基类顺序，与派生类构造函数中所定义的成 员函数初始化列表顺序无关。
```cpp
// 示例多继承方式下构造函数和析构函数的调用顺序。
#include <iostream>
using namespace std;
class A //定义基类A
{
public:
    A(int i) { a = i; cout << "A Constructor" << endl; }
    void disp() { cout << "a=" << a << endl; }
    ~A() { cout << "A Destructor" << endl; }
private:
    int a;
};
class B //定义基类B
{
public:
    B(int j) { b = j; cout << "B Constructor" << endl; }
    void disp() { cout << "b=" << b << endl; }
    ~B() { cout << "B Destructor" << endl; }
private:
    int b;
};
class C :public B, public A //定义A和B的派生类C。B在前，A在后
{
public:
    C(int k) :A(k + 2), B(k - 2) //包含基类成员初始化列表
    {
        c = k;
        cout << "C Constructor" << endl;
    }
    void disp()
    {
        //用类名加作用域运算符限定调用某个基类的同名成员
        A::disp();
        B::disp();
        cout << "c=" << c << endl;
    }
    ~C() { cout << "C Destructor" << endl; }
private:
    int c;
};
int main()
{
    C obj(10);
    //调用类C的成员函数disp
    obj.disp();
    cin.ignore();
    return 0;
}
/*
B Constructor
A Constructor
C Constructor
a=12
b=8
c=10

C Destructor
A Destructor
B Destructor

*/
```
### 5.2 多继承且有内嵌对象时的构造函数
1.调用基类构造函数，调用顺序按照它们 被继承时声明的顺序（从左向右）。

2.调用成员对象的构造函数，调用顺序按 照它们在类中声明的顺序。

3.派生类的构造函数体中的内容。
```cpp
// 例 派生类构造函数举例（多继承，含有内嵌对象） 。
#include <iostream>
using namespace std;
class B1 //基类B1，构造函数有参数
{
public:
    B1(int i) { cout << "constructing B1 " << i << endl; }
};
class B2 //基类B2，构造函数有参数
{
public:
    B2(int j) { cout << "constructing B2 " << j << endl; }
};
class B3 //基类B3，构造函数无参数
{
public:
    B3(){ cout << "constructing B3 *" << endl; }
};
class C : public B2, public B1, public B3
{
public: //派生类的公有成员
//注意基类名的个数与顺序
//注意成员对象名的个数与顺序
    C(int a, int b, int c, int d) :
        B1(a), memberB2(d), memberB1(c), B2(b) { }
private: //派生类的私有对象成员
    B1 memberB1;
    B2 memberB2;
    B3 memberB3;
};
int main()
{
C obj(1,2,3,4);
cin.ignore();
return 0;
}
/*
constructing B2 2
constructing B1 1
constructing B3 *
constructing B1 3
constructing B2 4
constructing B3 *
*/
```
```cpp
// 例题5.6,多继承范例
#include <iostream>
#include <cstring>
using namespace std;
//定义研究生基类
class GStudent
{
protected:
    int number; char name[20]; char sex;
public:
    GStudent(int n, char *s, char x)
    {
        number = n; strcpy(name, s); sex = x;
        cout << "Construct GStudent. " << endl;
    }
    ~GStudent()
    {
        cout << "Destruct GStudent. " << endl;
    }
};
//定义职员基类
class Emplyee
{
protected:
    char ename[20];
    char jobname[20];
public:
    Emplyee(char *sn, char *sj)
    {
        strcpy(ename, sn);
        strcpy(jobname, sj);
        cout << "Construct Employee. " << endl;
    }
    ~Emplyee()
    {
        cout << "Destruct Employee. " << endl;
    }
};
//定义在职研究生类，从两个基类派生
class GStudentHasJob :public GStudent, public Emplyee
{
public:
    GStudentHasJob(int n, char *s, char x, char *sj) :
        GStudent(n, s, x), Emplyee(s, sj)
    {
        cout << "Construct GStudentHasJob. " << endl;
    }
    ~GStudentHasJob()
    {
        cout << "Destruct GStudentHasJob. " << endl;
    }
    void Print()
    {
        cout << "number:" << number << endl;
        cout << "name:" << name << endl;
        cout << "sex:" << sex << endl;
        cout << "job:" << jobname << endl;
    }
};
int main()
{
    GStudentHasJob st(1001, "zhang", 'F', "teacher");
    st.Print();
    cin.ignore();
    return 0;
}
/*
Construct GStudent.
Construct Employee.
Construct GStudentHasJob.
number:1001
name:zhang
sex:F
job:teacher

Destruct GStudentHasJob.
Destruct Employee.
Destruct GStudent.
*/
```
## 6 基类成员访问 和赋值兼容性
### 6.1 基类成员名的限定访问 和同名覆盖
在多继承中出现？

当派生类的多个基类存在同名成员时： 则派生类对些同名成员的访问就可能存在冲 突
解决办法：显式调用要访问的成员，成员名 限定访问法 基类名：：成员名

### 6.2 同名覆盖原则
当派生类与基类中有相同成员时： 若未强行指名，则通过派生类对象使用的 是派生类中的同名成员。  
如要通过派生类对象访问基类中被覆盖的 同名成员，应使用基类名限定//同上
```cpp
// 多继承同名覆盖举例
#include <iostream>
using namespace std;
class B1 //声明基类B1
{
public: //外部接口
    int nV;
    void fun(){ cout << "Member of B1" << endl; }
};
class B2 //声明基类B2
{
public: //外部接口
    int nV;
    void fun(){ cout << "Member of B2" << endl; }
};
class D1 : public B1, public B2 //定义派生类
{
public:
    int nV; //同名数据成员
    void fun(){ cout << "Member of D1" << endl; }
        //同名函数成员
};
int main()
{ D1 d1;
d1.nV = 1; //对象名.成员名标识, 访问D1类成员
d1.fun();
d1.B1::nV = 2; //作用域分辨符标识, 访问基类B1成员
d1.B1::fun();
d1.B2::nV = 3; //作用域分辨符标识, 访问基类B2成员
d1.B2::fun();
return 0;
}
/*
Member of D1
Member of B1
Member of B2
*/
```
### 6.3 二义性问题
同名覆盖，无声明就二义性  
在多继承时，基类与派生类之间，或基类之间出现同 名成员时，将出现访问时的二义性（不确定性）—— 采用虚函数或支配（同名覆盖）原则来解决。  
当派生类从多个基类派生，而这些基类又从同一个基 类派生，则在访问此共同基类中的成员时，将产生二 义性——采用虚基类来解决。  
```cpp
// 二义性问题举例
class A
{
public:
void f（ ）;
};
class B
{
public:
void f（ ）;
void g（ ）;
};
class C: public A, piblic B
{ public:
void g（ ）;
void h（ ）;
};
如果声明：C c1;
则 c1.f（ ）; 具有二义性
而 c1.g（ ）; 无二义性（同名覆
盖）
```
决方法一：用类名来限定 c1.A::f（ ） 或 c1.B::f（ ）  
解决方法二：同名覆盖 在类C 中声明一个同名成员函数f（ ），f（ ） 再根据需要调用 A::f（ ） 或 B::f（ ）  

再来举一个例子  
这个例子还是比较特别的  
```cpp
class B
{ public:
int b;
}
class B1 : public B
{
private:
int b1;
}
class B2 : public B
{
private:
int b2;
};
class C : public B1,public B2
{
public:
int f（ ）;
private:
int d;
}
/*
下面的访问是二义性的:  
          C c;  
          c.b c.B::b  

下面是正确的： 
          c.B1::b
          c.B2::b
*/
```
### 6.4 赋值兼容规则
派生类对象可以看作是基类对象：派生类包含了基类的 全部成员；  
基类对象不能被看作是派生类对象：派生类新增了成员  
基类对象可以赋值给基类对象，也可以把派生类对象 赋值给基类对象  
基类指针可以指向基类对象，也可以指向派生类对象  
基类引用可以指向基类对象，也可以指向派生类对象   
  
一个公有派生类的对象在使用上可以被当作基类的 对象，反之则禁止。  
具体表现在：  
派生类的对象可以被赋值给基类对象。  
派生类的对象可以初始化基类的引用。  
指向基类的指针也可以指向派生类。  
  
疑问：为什么是公有派生类？ 因为公有派生类和基类之间可以使用相同的函数，而 没有必要为每一个类设计单独的模块  
```cpp
#include <iostream>
using namespace std;
class B0 //基类B0声明
{
public:
    void display() { cout << "B0::display（ ）" << endl; }
    //公有成员函数
};
class B1 : public B0
{
public:
    void display() { cout << "B1::display（ ）" << endl; }
};
class D1 : public B1
{
public:
    void display() { cout << "D1::display（ ）" << endl; }
};
void fun(B0 *ptr)
{
    ptr->display();
} //"对象指针->成员名"
int main() //主函数
{
B0 b0; //声明B0类对象B1 b1; //声明B1类对象D1 d1; //声明D1类对象B0 *p; //声明B0类指针p = &b0; //B0类指针指向B0类对象fun(p);p = &b1; //B0类指针指向B1类对象p->display();cout << 1 << endl;fun(p);p = &d1; //B0类指针指向D1类对象p->display();cout << 1 << endl;fun(p);return 0;

}
/*
B0::display（ ）B0::display（ ）1B0::display（ ）B0::display（ ）1B0::display（ ）
*/
```
同样的例子
```cpp
// 例题5.10，赋值兼容实例
#include <iostream>
using namespace std;
class Base
{
protected:
    int member;
public:
    Base()
    {
        member = 0;
    }
    void Show()
    {
        cout << "Base::Show():" << member << endl;
    }
};
class Derived1 :public Base
{
public:
    Derived1(int a)
    {
        member = a;
    }
    void Show()
    {
        cout << "Derived1::Show():" << member << endl;
    }
};
class Derived2 :public Derived1
{
public:
    Derived2(int a) :Derived1(a)
    { }
    void Show()
    {
        cout << "Derived2::Show():" << member << endl;
    }
};
void Test(Base *pb)
{
    pb->Show();
}
void Test(Base &br)
{
    br.Show();
}

int main()
{
    Base b0;
    Derived1 d1(5);
    Derived2 d2(10);
    Base *pb0;
    pb0 = &b0;
    Test(pb0);
    b0 = d1;
    Test(pb0);
    pb0 = &d1;
    Test(pb0);
    Test(d2);
    cin.ignore(); return 0;
}
/*
Base::Show():0
Base::Show():5
Base::Show():5
Base::Show():10

*/
```
## 7 虚基类
### 7.1 虚基类的定义
class 派生类名:virtual 继承方式 共同基类名  
在派生类的对象中，这些同名成员在内存中 同时拥有多个副本，如果将直接基类的共同 基类设置为虚基类，  
那么从不同的路径继承 过来的该类成员（包括数据成员和函数成员） 在内存中只拥有一个副本（数据成员只  
有一 个副本，函数成员只有一个映射），从而解 决了同名成员的唯一标识问题  

代码示例
```cpp
// 示例虚基类。
#include <iostream>
using namespace std;
class A
{
public:
    A() { a = 10; }
protected:
    int a;
};
class A1 :virtual public A
    //定义虚基类
{
public:
    A1() { cout << a << endl; }
};
class A2 :virtual public A
    //定义虚基类
{
public:
    A2() { cout << a << endl; }
};
class B :A1, A2 //私有继承
{
public:
    B() { cout << a << endl; }
};
int main()
{
    B obj;
    return 0;
}
/*
10
10
10*/
```
### 7.2 虚基类的初始化
构造函数的调用顺序  
先调用虚基类的构造函数，再调用非虚基类的 构造函数。  
若同一层次中包含多个虚基类，其调用顺序按定 义时顺序。  
若虚基类由非虚基类派生而来，则仍按先调用基 类构造函数，再调用派生类构造函数的顺序  
  
代码示例  
```cpp
// 示例引入虚基类后构造函数的调用顺序。
#include <iostream>
using namespace std;
class Base1
{
public:
    Base1() { cout << "class Base1" << endl; }
};
class Base2
{
public:
    Base2() { cout << "class Base2" << endl; }
};
class Level1 :public Base2, virtual public Base1 //定义虚基类
{
public:
    Level1() { cout << "class Level1" << endl; }
};
class Level2 :public Base2, virtual public Base1
{
public:
    Level2() { cout << "class Level2" << endl; }
};
class TopLevel :public Level1, virtual public Level2
{
public:
    TopLevel() { cout << "class TopLevel" << endl; }
};
int main()
{
    TopLevel obj;
    return 0;
}
/*
class Base1
class Base2
class Level2
class Base2
class Level1
class TopLevel
*/
```
这里就是先调用Toplevel的构造函数，先虚基类level2，再level1

 

level2中先虚基类base1，输出class base1，再base2构造函数，输出class base2

然后level2的本身的calss level2

 

level1中(先虚基类base1，输出class base1)  错错错，没了，虚基类只有一次，再base2，输出class base2

然后level1的本身的class level1

最后本身的构造函数

 

 

换个角度讲，虚派生只影响从指定了虚基类的派生类中进一步派生出来的类，它不会影响派生类本身。

 

如果加上

Level1 a;
Level2 b;

结果会是

class Base1
class Base2
class Level1
class Base1
class Base2
class Level2

所以说，虚基类就是保证在多继承的类似的菱形路线上，后代的后代的身体（类的内存结构上），一个虚基类只会最多生成一次

emmm，感觉就是这样的，


注意：定义虚基类的构造函数注意：

（1）一般情况下虚基类只允许定义不带参数的或带 缺省参数的构造函数

（2）如果多继承不牵扯到对同一基类的派生，就没 必要定义虚基类。

 

虚基类的初始化与一般多继承的初始化规则如下：

1.所有从虚基类直接或间接派生的类，都必须在该类 构造函数的成员初始化列表中列出对虚基类构造函数 的调用。

   但是只有实际构造对象的类的构造函数才会 引发对虚基类构造函数的调用，而其他基类在成员初 始化列表中对

   虚基类构造函数的调用都会被忽略，这 样将确保派生类对象中虚基类成员只被初始化一次。

2.如果某类构造函数的成员初始化列表中同时列出对 虚基类构造函数和非虚基类构造函数的调用，则会优 先执行

   虚基类构造函数。
```cpp
// 示例虚基类的应用。
#include <iostream>
#include <string.h>
using namespace std;
class Data_rec
    //定义基类Data_rec
{
public:
    Data_rec()
    {
        name = NULL;
    }
    ~Data_rec()
    {
        delete[] name;
    }
    void insert_name(char* pname)
    {
        delete[] name;
        name = new char[strlen(pname) + 1];
        strcpy(name, pname);
    }
    void print()
    {
        cout << "Name:" << name << endl;
    }
private:
    char* name;
};
class Student :virtual public Data_rec //定义虚基类
{
public:
    Student() :Data_rec() { id = NULL; }
    ~Student() { delete[] id; }
    void insert_id(char* pid)
    {
        delete[] id;
        id = new char[strlen(pid) + 1];
        strcpy(id, pid);
    }
    void print()
    {
        Data_rec::print(); //访问基类的成员函数
        cout << "Id:" << id << endl;
    }
private:
    char* id;
};
class Teacher :virtual public Data_rec //定义虚基类
{
public:
    Teacher() :Data_rec() { sal = 0; }
    void insert_sal(float psal) { sal = psal; }
    void print()
    {
        Data_rec::print(); //访问基类的成员函数
        cout << "Sal:" << sal << endl;
    }
private:
    float sal;
};
class Postgrad :public Student //定义派生类Postgrad
{
public:
    Postgrad() :Student() { dn = NULL; }
    void insert_dn(char* p)
    {
        delete[] dn;
        dn = new char[strlen(p) + 1];
        strcpy(dn, p);
    }
    void print()
    {
        Student::print(); //访问基类的成员函数
        cout << "Dept Name:" << dn << endl;
    }
private:
    char* dn;
};
class Tpost :public Teacher, public Postgrad //定义多继承派生类Tpost
{
public:
    Tpost() :Teacher(), Postgrad() {}
    void print()
    {
        Teacher::print();
        Postgrad::print();
    }
};
int main()
{
    Teacher tobj;
    Tpost tpobj;
    tobj.insert_name("Li Min");
    tobj.insert_sal(2000);
    tpobj.insert_name("Zhang Hua");
    tpobj.insert_sal(1500);
    tpobj.insert_id("03410101");
    tpobj.insert_dn("Computer");
    tobj.print();
    tpobj.print();
    cin.ignore();
    return 0;
}
/*
Name:Li Min
Sal:2000
Name:Zhang Hua
Sal:1500
Name:Zhang Hua
Id:03410101
Dept Name:Computer
*/
```
```cpp
#include <iostream>
using namespace std;

class transportation
{
public:
    transportation(int i)
    {
        cout<<"transportation "<<i<<" born"<<endl;
    }
};
class car:virtual public transportation
{
public:
    car(int i):transportation(i)
    {
        cout<<"car "<<i<<" born"<<endl;
    }
};
class plane:virtual public transportation
{
public:
    plane(int i):transportation(i)
    {
        cout<<"plane "<<i<<" born"<<endl;
    }
};
class carplane:public car,public plane
{
public:
    carplane(int i ):car(i+2),plane(i+3)
    {
        cout<<"carplane "<<i<<" born"<<endl;
    }
};
int main()
{
    carplane t(0);
    return 0;
}
/*程序输出
transportation 1 born
car 2 born
plane 3 born
carplane 0 born
*/
/*如果没有使用虚继承的方式，则程序输入为：
transportation 2 born
car 2 born
transportation 3 born
plane 3 born
carplane 0 born
*/
```
### 
