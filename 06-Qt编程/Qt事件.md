> Qt 中所有组件都可以当作一个窗口，如 QPushButton 也是一个窗口



# 1. Qt 的事件系统

**窗口系统是由事件驱动的，Qt 为事件处理编程提供了完善的支持。** QWidget 类是所有界面组件类的基类，QWidget 类定义了大量与事件处理相关的数据类型和接口函数。



## 1.1 事件的产生和派发

### 1.1.1 事件的产生

在 Qt 中，事件是对象， 是 QEvent 类或其派生类的**实例**。例如 QKeyEvent 是按键事件类，QMouseEvent 是鼠标事件类，QPaintEvent 是绘制事件类，QTimerEvent 是定时器事件类。

* 自生事件：窗口系统产生的事件。会进入**系统队列**。 然后被应用程序的事件循环逐个处理。
* 发布事件：Qt 或应用程序产生的事件。例如，QTimer 定时器发生定时溢出时 Qt 会自动发布 QTimerEvent 事件。应用程序使用静态函数 `QCoreApplication::postEvent()` 产生发布事件，发布事件会进入 Qt 事件队列，由应用程序的事件循环进行处理。
* 发送事件：Qt 或应用程序定向发送给某个对象的事件。应用程序使用静态函数 `QCoreApplication::sendEvent()` 产生发送事件，由对象的 event() 函数直接处理。

自生事件和发布事件的处理是**异步的**。事件进入队列后由系统去处理，程序不会再产生事件的地方停止进行等待。发送事件是的处理是**同步的**。



### 1.1.2 事件的派发

```c++
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();  // 开始应用程序的事件循环
}
```

**函数 `QApplication::exec()` 就是不断检查系统队列和 Qt 事件队列里是否由未处理的自生事件和发布事件，如果有事件就派发给接受事件的对象去处理。**



## 1.2 事件的处理

[Qt 中的事件系统 - 有什么难的呢？ - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/50053079)

[Qt事件之事件处理器 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/event_handler/)



## 1.3 典型事件处理示例

### 1.3.1 程序界面

![image-20230908231148946](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309082311999.png)



### 1.3.2 Widget 头文件

```c++
#include <QWidget>

#include <QResizeEvent>

#include <QCloseEvent>

#include <QKeyEvent>

#include <QPaintEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Widget : public QWidget
{
   Q_OBJECT

protected:
   void paintEvent(QPaintEvent *event);

   void closeEvent(QCloseEvent *event);

//   void keyReleaseEvent(QKeyEvent *event);

   void keyPressEvent(QKeyEvent *event);

   void showEvent(QShowEvent *event);

   void hideEvent(QHideEvent *event);

   void mousePressEvent(QMouseEvent *event);

public:
    Widget(QWidget *parent = 0);
   ~Widget();

private:
   Ui::Widget *ui;
};
```



### 1.3.3 事件处理函数 paintEvent()

在窗口需要重绘时，应用程序会向窗口发送 QEvent::Paint 类型的事件，窗口对象会自动运行事件处理函数 paintEvent() 。

```c++
void Widget::paintEvent(QPaintEvent *event)
{
   Q_UNUSED(event);
   QPainter painter(this);
   painter.drawPixmap(0,0,this->width(), this->height(),
                      QPixmap(":/pics/images/background.jpg"));
//   QWidget::paintEvent(event);
}
```



### 1.3.4 事件处理函数 closeEvent()

```c++
void Widget::closeEvent(QCloseEvent *event)
{
   QString dlgTitle="消息框";
   QString strInfo="确定要退出吗？";

   QMessageBox::StandardButton result=QMessageBox::question(this, dlgTitle, strInfo,
                                QMessageBox::Yes|QMessageBox::No |QMessageBox::Cancel);
   if (result==QMessageBox::Yes)
      event->accept();  //窗口可关闭
   else
      event->ignore();  //窗口不能被关闭
}
```



### 1.3.5 事件处理函数 mousePressEvent()

```c++
void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)   //鼠标左键
    {
        QPoint  pt=event->pos();                 //点击点在窗口上的相对坐标
        QPointF relaPt=event->position();        //相对坐标
        QPointF winPt=event->scenePosition();    //相对坐标
        QPointF globPt=event->globalPosition();  //屏幕上的绝对坐标

        QString str=QString::asprintf("pos()=(%d,%d)", pt.x(),pt.y());
        str= str+QString::asprintf("\nposition()=(%.0f,%.0f)", relaPt.x(),relaPt.y());
        str= str+QString::asprintf("\nscenePosition()=(%.0f,%.0f)", winPt.x(),winPt.y());
        str= str+QString::asprintf("\nglobalPosition()=(%.0f,%.0f)", globPt.x(),globPt.y());

        ui->labMove->setText(str);
        ui->labMove->adjustSize();    //自动调整组件大小
        ui->labMove->move(event->pos());  //标签移动到鼠标光标处
    }

    QWidget::mousePressEvent(event);
}
```



### 1.3.6 事件处理函数 keyPressEvent() 或 keyReleaseEvent()

```c++
void Widget::keyPressEvent(QKeyEvent *event)
{
    QPoint  pt=ui->btnMove->pos();

    if ((event->key()==Qt::Key_A) || (event->key()==Qt::Key_Left))
        ui->btnMove->move(pt.x()-20, pt.y());
    else if((event->key()==Qt::Key_D) || (event->key()==Qt::Key_Right))
        ui->btnMove->move(pt.x()+20, pt.y());
    else if((event->key()==Qt::Key_W)  || (event->key()==Qt::Key_Up))
        ui->btnMove->move(pt.x(),    pt.y()-20);
    else if((event->key()==Qt::Key_S) || (event->key()==Qt::Key_Down))
        ui->btnMove->move(pt.x(),    pt.y()+20);

    event->accept(); //被处理,不会再传播到父窗体
    //    QWidget::keyPressEvent(event);
}
```



### 1.3.7 事件处理函数 showEvent() 和 hideEvent()

```c++
void Widget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    qDebug("showEvent()函数被触发");
}

void Widget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    qDebug("hideEvent()函数被触发");
}
```



# 2. 事件与信号

事件和信号的区别在于，事件通常是由窗口系统或应用程序产生的，信号则是 Qt 定义或用户自定义的。Qt 为界面组件定义的信号通常是对**事件的封装** ，例如 QPushButton 的 clicked() 信号可以看作对 QEvent::MouseButtonRelease 类型事件的封装。

## 2.1 函数 event() 的作用

应用程序派发给界面组件的事件首先会由其函数 event() 处理，如果函数 event() 不做任何处理，组件就会自动调用 QWidget 中与事件类型对应的默认事件处理函数。从 QWidget 派生的界面组件类一般不需要重新实现函数 event()，如果如果要对某种类型事件进行处理，可以重新实现对应的事件处理函数。



## 2.2 事件与信号编程实例

```C++
class TMyLabel : public QLabel
{
    Q_OBJECT
public:
    TMyLabel(QWidget *parent = nullptr);  // 界面组件必须有一个父容器

    bool event(QEvent *e);  //重新实现event()函数

protected:
    void mouseDoubleClickEvent(QMouseEvent *event); //重新实现鼠标双击事件的默认处理函数

signals:
    void doubleClicked();   //自定义信号
};
```

```C++
TMyLabel::TMyLabel(QWidget *parent):QLabel(parent)
{
    this->setAttribute(Qt::WA_Hover,true);  //必须设置这个属性，才能产生hover事件
}

bool TMyLabel::event(QEvent *e)
{
    if(e->type() ==QEvent::HoverEnter)      //鼠标移入
    {
        QPalette plet=this->palette();
        plet.setColor(QPalette::WindowText, Qt::red);
        this->setPalette(plet);
    }
    else if (e->type()==QEvent::HoverLeave) //鼠标移出
    {
        QPalette plet=this->palette();
        plet.setColor(QPalette::WindowText, Qt::black);
        this->setPalette(plet);
    }

    return QLabel::event(e);    //执行父类的event()，处理其他类型事件
}

void TMyLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubleClicked();   //发射信号
}
```



> 如果默认窗口部件没有我们想要的功能，那么我们只能自定义类去实现，让其继承 Qt 中已有的组件类，然后将此类提升为组件类，就能使用我们实现的功能。

![image-20230910105522300](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309101055375.png)

```C++
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event); //在窗口上双击时的响应

private slots:
    void do_doubleClick();  //与lab的doubleClicked()信号关联

private:
    Ui::Widget *ui;
};
```

```C++
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->lab,SIGNAL(doubleClicked()),
            this,SLOT(do_doubleClick()));
}

Widget::~Widget()
{
    delete ui;
}

//双击窗口时的响应
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ui->lab->setText("窗口被双击了");
    ui->lab->adjustSize();
}

//双击标签时的响应
void Widget::do_doubleClick()
{
    ui->lab->setText("标签被双击了，信号的槽函数响应");
    ui->lab->adjustSize();
}
```



[Qt事件之事件分发器 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/event/)



# 3. 事件过滤器

事件过滤器是 QObject 提供的一种处理事件的方法，它可以**将一个对象的事件委托给另一个对象来监视并处理。** 一个窗口可以作为其界面上的 QLable 组件的事件过滤器，派发给 QLabel 组将的事件由窗口去处理，这样，就不需要为了处理某种事件而新定义一个标签类。

1. 被监视的对象使用函数 installEventFilter() 将自己注册给监视对象，监视对象就是事件过滤器。
2. 监视对象重新实现 eventFilter() ，对监视到的事件进行处理。



## 3.1 事件过滤器编程实例

```C++
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

   bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::Widget *ui;
};
```

```C++
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->labHover->installEventFilter(this);     //安装事件滤波器
    ui->labDBClick->installEventFilter(this);   //安装事件滤波器
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==ui->labHover) //上面的QLabel组件
    {
        if (event->type()==QEvent::Enter)       //鼠标移入
            ui->labHover->setStyleSheet("background-color: rgb(170, 255, 255);");
        else if (event->type()==QEvent::Leave)  //鼠标离开
        {
            ui->labHover->setStyleSheet("");
            ui->labHover->setText("靠近我，点击我");
        }
        else if (event->type()==QEvent::MouseButtonPress)   //鼠标键按下
            ui->labHover->setText("button pressed");
        else if (event->type()==QEvent::MouseButtonRelease) //鼠标键释放
            ui->labHover->setText("button released");
    }

    if (watched==ui->labDBClick)    //下面的QLabel组件
    {
        if (event->type()==QEvent::Enter)       //鼠标移入
            ui->labDBClick->setStyleSheet("background-color: rgb(85, 255, 127);");
        else if (event->type()==QEvent::Leave)  //鼠标离开
        {
            ui->labDBClick->setStyleSheet("");
            ui->labDBClick->setText("可双击的标签");
        }
        else if (event->type()==QEvent::MouseButtonDblClick)    //鼠标双击
            ui->labDBClick->setText("double clicked");
    }

    return QWidget::eventFilter(watched,event);     //执行父类的eventFilter()函数
    //    return true;      //有问题，不能直接返回true
}
```



[Qt事件之事件过滤器 | 爱编程的大丙 (subingwen.cn)](https://subingwen.cn/qt/event_filter/)



# 4. 拖放事件与拖放操作

## 4.1 拖放操作相关事件

拖放由两个操作组成：拖动（drag）和放置（drop）。被拖动的组件称为拖动点（drag site），接受拖放操作的组件称为放置点（drop site）。拖动点与放置点可以是不同的组件，甚至是不同的应用程序。

1. 被拖动的组将通过 mousePressEvent() 和 mouseMoveEvent() 这两个事件处理函数处理，检测到鼠标左键按下并移动就可以启动拖动操作。启动拖动操作需要创建一个 `QDrag` 对象描述拖动操作，以及创建一个 `QMimeData` 类的对象用于存储拖动操作的格式信息和数据，并将其赋值给 QDrag 对象的 mimeData 属性。
2. 当拖动操作移动到放置点范围内时，首先触发 `dragEnterEvent()` 事件处理函数，通过 QDrag 对象的 mimeData 数据判断拖动操作的来源和操作，以决定是否接受此拖动操作，只有被接受的拖动操作才可以被放置，并触发 `dropEvent()` 事件处理函数。此函数用来 **处理放置时的具体操作。**

> QWidget 类有一个属性 acceptDrops，如果设置为 true，那么对应的这个组件就可以作为一个放置点。默认为 false。QListWidget、QTreeWidget、QTableWiget 等组件既可以作为拖动点，也可以作为放置点。



## 4.2 外部文件拖放操作示例

程序界面

![image-20230910112321726](https://mater-1312713760.cos.ap-guangzhou.myqcloud.com/img/202309101123771.png)

```C++
class Widget : public QWidget
{
    Q_OBJECT

protected:
    void dragEnterEvent(QDragEnterEvent *event); // 拖动文件进入窗口时触发的事件处理函数

    //   void dragMoveEvent(QDragMoveEvent *event);
    void resizeEvent(QResizeEvent *event); // 窗口改变大小时触发的事件处理函数

    void dropEvent(QDropEvent *event);  // 拖动文件在窗口上放置时触发的事件处理函数

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:

private:
    Ui::Widget *ui;
};
```

```C++
#include <QMimeData>

#include <QFileInfo>

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    //显示MIME信息
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("dragEnterEvent事件 mimeData()->formats()");
    for(int i=0;i<event->mimeData()->formats().size();i++)
        ui->plainTextEdit->appendPlainText(event->mimeData()->formats().at(i));

    ui->plainTextEdit->appendPlainText("\n dragEnterEvent事件 mimeData()->urls()");
    for(int i=0;i<event->mimeData()->urls().size();i++)
    {
        QUrl url=event->mimeData()->urls().at(i);//带路径文件名
        ui->plainTextEdit->appendPlainText(url.path());
    }

    if (event->mimeData()->hasUrls())
    {
        QString filename=event->mimeData()->urls().at(0).fileName();//获取文件名
        QFileInfo fileInfo(filename);       //获取文件信息
        QString ext=fileInfo.suffix().toUpper();    //获取文件后缀
        if (ext =="JPG")
            event->acceptProposedAction();  //接受拖动操作
        else
            event->ignore();        //忽略事件
    }
    else
        event->ignore();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QSize sz=ui->plainTextEdit->size();
    ui->plainTextEdit->resize(this->width()-10,sz.height());    //只改变宽度
    ui->labPic->resize(this->width()-10,this->height()-sz.height()-20); //改变宽度和高度
    event->accept();
}

//void Widget::dragMoveEvent(QDragMoveEvent *event)
//{//不是必须的
//   if (event->mimeData()->hasUrls())
//      event->acceptProposedAction();
//}

void Widget::dropEvent(QDropEvent *event)
{
    QString filename=event->mimeData()->urls().at(0).path(); //完整文件名
    filename=filename.right(filename.length()-1); //去掉最左边的“/”
    QPixmap pixmap(filename);
    ui->labPic->setPixmap(pixmap);
    event->accept();
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->labPic->setScaledContents(true);     //图片适应组件大小

    this->setAcceptDrops(true);  //由窗口接受drop操作
    ui->plainTextEdit->setAcceptDrops(false);    //不接受drop，由窗口去处理
    ui->labPic->setAcceptDrops(false);           //不接受drop,由窗口去处理
}

Widget::~Widget()
{
    delete ui;
}
```

