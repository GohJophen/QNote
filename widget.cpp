#include "widget.h"
#include "ui_widget.h"





//Widget(QWidget *parent)该构造函数可以接收一个指向父组件（QWidget 类型）的指针，如果没有传递则默认为 nullptr，即没有父组件。
//QWidget(parent), ui(new Ui::Widget)该构造函数的初始化列表。注意：这不是继承父类构造函数，而是显示调用父类的构造函数，来初始化父类的成员。

//QWidget(parent) 表示调用父类 QWidget 的构造函数，将 parent 传递给父类。

//ui(new Ui::Widget) ui是Widget类的成员变量，它是一个是指向Ui::Widget类型的指针；在堆上创建一个 Ui::Widget 类的实例，并将它的指针赋值给 ui。

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget),settings("GohJophen", "QNote"){

    ui->setupUi(this);//将 UI 元素与当前的 Widget 实例关联起来。
    this->setLayout(ui->verticalLayout);//创建的布局跟随ui设计的最大布局，使窗口拉大拉小，不会出现空白布局
    //ui->widgetBottom->setLayout(ui->hLayoutBottom);
    //disconnect(ui->btnSave, SIGNAL(clicked()), this, SLOT(on_btnSave_clicked()));
    // 获取用户选择的编码格式
    //encoding = ui->comboBox->currentText();

    //第4种;函数指针(推荐)
    connect(ui->btnOpen, &QPushButton::clicked, this, &Widget::btnOpen_clicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &Widget::btnSave_clicked);
    connect(ui->btnClear, &QPushButton::clicked, this, &Widget::btnClear_clicked);
    connect(ui->btnClose, &QPushButton::clicked, this, &Widget::btnClose_clicked);
    connect(ui->btnAbout, &QPushButton::clicked, this, &Widget::btnAbout_clicked);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Widget::onCursorPositionChanged);


    //快捷键
    QShortcut* shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O","File|Open")),this);
    QShortcut* shortcutSave = new QShortcut(QKeySequence(tr("Ctrl+S","File|Save")),this);
    QShortcut* shortcutClear = new QShortcut(QKeySequence(tr("Ctrl+D","File|Clear")),this);
    QShortcut* shortcutClose = new QShortcut(QKeySequence(tr("Ctrl+W","File|Close")),this);
    QShortcut* shortcutZoomIn = new QShortcut(QKeySequence(tr("Ctrl+Shift+=","File|ZoomIn")),this);//放大
    QShortcut* shortcutZoomOut = new QShortcut(QKeySequence(tr("Ctrl+Shift+-","File|ZoomOut")),this);//缩小
    //第3种;Lambda 表达式；connect(发送者的对象名, &信号所属的类名::信号名, 接收者的对象名, [捕捉的变量](参数列表){函数体}
    connect(shortcutOpen,&QShortcut::activated,this,[=](){btnOpen_clicked();});
    connect(shortcutSave,&QShortcut::activated,this,[=](){btnSave_clicked();});
    connect(shortcutClear,&QShortcut::activated,this,[=](){btnClear_clicked();});
    connect(shortcutClose,&QShortcut::activated,this,[=](){btnClose_clicked();});
    connect(shortcutZoomIn,&QShortcut::activated,this,[=](){
        //获得textEdit的当前字体信息
        QFont font = ui->textEdit->font();

        //获得当前字体的大小
        int fontSize = font.pointSize();
        if(fontSize == -1) return;

        //改变大小，并设置字体大小
        int newFontSize = fontSize+1;
        font.setPointSize(newFontSize);
        ui->textEdit->setFont(font);
    });
    connect(shortcutZoomOut,&QShortcut::activated,this,[=](){
        QFont font = ui->textEdit->font();

        int fontSize = font.pointSize();
        if(fontSize == -1) return;

        int newFontSize = fontSize-1;
        font.setPointSize(newFontSize);
        ui->textEdit->setFont(font);
    });
}

Widget::~Widget(){
    delete ui;
}

void Widget::wheelEvent(QWheelEvent *event){
    // 检查是否按下了鼠标的中键（滚轮）
    //event->buttons()返回一个 Qt::MouseButtons 类型的值；获取当前按下的所有鼠标按钮的组合
    //Qt::MiddleButtonQt 中的枚举值，表示鼠标的中键（通常就是滚轮键）
    //&（按位与操作）；如果从event->buttons()获取到的鼠标事件含有Qt::MiddleButton鼠标中键，则该条件为真。
    if (event->buttons() & Qt::MiddleButton)
    {
        // 获得textEdit的当前字体信息
        QFont font = ui->textEdit->font();

        // 获得当前字体的大小
        int fontSize = font.pointSize();
        if (fontSize == -1) return; // 如果字体大小无效，直接返回

        // 根据鼠标滚轮的滚动方向来决定是放大还是缩小字体
        if (event->angleDelta().y() > 0) {
            // 滚轮向上滚动，放大字体
            font.setPointSize(fontSize + 1);
        } else if (event->angleDelta().y() < 0) {
            // 滚轮向下滚动，缩小字体
            font.setPointSize(fontSize - 1);
        }

        // 应用新的字体大小到 textEdit
        ui->textEdit->setFont(font);

        // 更新事件处理状态，表示该事件已被处理
        event->accept();
    }else {
        // 如果没有按下中键，则执行默认的滚动操作
        QWidget::wheelEvent(event);
    }
}

void Widget::closeEvent(QCloseEvent *event){
    if(!file.isOpen() && !ui->textEdit->toPlainText().isEmpty()){
        int ret = QMessageBox::warning(
            this,
            tr("QNote"),
            tr("是否保存当前文件？"),
            QMessageBox::Ok | QMessageBox::No
            );

        switch(ret){
        case QMessageBox::Ok:
            btnSave_clicked();//调用保存功能
            originalText = ui->textEdit->toPlainText();//文件保存好后继续存储当前的文本内容，用以检查接下来是否有继续修改
            event->ignore();//忽略关闭事件；确认保存 或者 取消保存 后不关闭应用窗口；保存对话框中的取消键和关闭应用窗口弹出的询问对话框的取消键 是相同的事件。
            break;
        case QMessageBox::No:
            event->accept();//应用关闭窗口
            break;
        }
    }
    else if(file.isOpen() && ui->textEdit->toPlainText() != originalText){
        int ret = QMessageBox::warning(
            this,
            tr("QNote"),
            tr("是否保存当前文件？"),
            QMessageBox::Ok | QMessageBox::No
            );

        switch(ret){
        case QMessageBox::Ok:
            btnSave_clicked();//调用保存功能
            originalText = ui->textEdit->toPlainText();//文件保存好后继续存储当前的文本内容，用以检查接下来是否有继续修改
            event->ignore();//忽略关闭事件；确认保存 或者 取消保存 后不关闭应用窗口；保存对话框中的取消键和关闭应用窗口弹出的询问对话框的取消键 是相同的事件。
            break;
        case QMessageBox::No:
            event->accept();//应用关闭窗口
            break;
        }
    }
}


//第1种；ui自动连接方式，在ui界面上右键对象，选择点击“转到槽”，再选择点击信号，会自动在头文件里声明槽函数，并自动在源文件里定义槽，函数体为空，待自定义
void Widget::btnOpen_clicked()//“on_对象名_信号名”是信号与槽自动连接方式的专用函数名格式，其他连接方式使用此格式命名，会导致槽函数多调用一次。
{

        QString defaultOpenDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString openDirectory = QFileInfo(settings.value("lastOpenDirectory", defaultOpenDirectory).toString()).absolutePath();
        QString pathFileName = QFileDialog::getOpenFileName(
                                                        this,
                                                        tr("Open File"),
                                                        openDirectory,
                                                        tr("Text Files (*.txt);;All Files (*)")
                                                        );

        // 存储用户选择的保存目录到 QSettings 中，便于下次使用
        if (!pathFileName.isEmpty()) {
            if(file.isOpen()){
                file.close();
            }
            settings.setValue("lastOpenDirectory", pathFileName);
            ui->textEdit->clear();
            file.setFileName(pathFileName);

            if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
                qDebug() << "无法打开文件！";
                return; // 如果某个文件无法打开，则提前返回，终止函数的执行
            }

            QFileInfo fileInfo(pathFileName);
            QString fileName = fileInfo.fileName();
            this->setWindowTitle(fileName);


            QTextStream in(&file);//读取指定文件

            // 指定以utf-8编码
            in.setEncoding(QStringConverter::Utf8);

            //in.setEncoding(QStringConverter::Utf8); // 设置编码为 UTF-8（如果需要）

            /*
            //一次性读取整个文件，并直接塞入文本框部件里。该方式适合文本内容较小的情况
            QString content = in.readAll();// 读取整个文件的内容，默认使用 UTF-8 编码
            //qDebug() << "文件内容：" << content;
            ui->textEdit->setText(content); //一次性塞入所有内容到文本框部件里
            */

            //QString fileContent;
            originalText.clear(); // 确保 originalText 被清空

            //逐行读取，逐行追加。该方式适合读取大文件
            while(!in.atEnd()) {
                QString line = in.readLine();          // 逐行读取
                ui->textEdit->append(line);            // 将每行追加到 textEdit 部件中
            }

            originalText = ui->textEdit->toPlainText(); // 保存原始文本
        }
}

void Widget::btnSave_clicked()
{
    if(!file.isOpen()){
        QString defaultSaveDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

        // 从 QSettings 中获取上一次保存的目录，排除文件名
        QString saveDirectory = QFileInfo(settings.value("lastSaveDirectory", defaultSaveDirectory).toString()).absolutePath();

        QString pathFileName = QFileDialog::getSaveFileName(
                                                    this,
                                                    tr("Save File"),
                                                    saveDirectory,
                                                    tr("Text Files (*.txt);;All Files (*)")
                                                    );

        // 存储用户选择的保存目录到 QSettings 中，便于下次使用
        if (!pathFileName.isEmpty()) {
            settings.setValue("lastSaveDirectory", pathFileName);

            file.setFileName(pathFileName);//将用户选择的文件路径设置给类中的 QFile 成员变量 file

            // WriteOnly 模式以只写方式打开文件，Truncate 会清空文件中的现有内容
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                qDebug() << "无法保存文件！";
                return; // 如果文件无法打开，提前返回
            }

            // 提取文件名（不包括路径）
            QFileInfo fileInfo(pathFileName);
            QString fileName = fileInfo.fileName(); // 仅获取文件名（如 123.txt）
            this->setWindowTitle(fileName); // 设置窗体标题为 "123.txt"
        }

    }else{
        file.close();
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << "无法保存文件！";
            return; // 如果文件无法打开，提前返回
        }
    }


    QTextStream out(&file); // 用 QTextStream 处理文件输出


    // 指定以utf-8编码
    out.setEncoding(QStringConverter::Utf8);

    // 从文本部件中获取内容并写入文件
    QString content = ui->textEdit->toPlainText(); // 获取 QTextEdit 中的内容
    out << content; // 将内容写入文件
}

void Widget::btnClear_clicked(){
    ui->textEdit->clear();
}

void Widget::btnClose_clicked(){
    ui->textEdit->clear();
    file.close();
    this->setWindowTitle("QNote");
}
//第4种；
void Widget::btnAbout_clicked()
{
    QMessageBox::about(
        this,
        tr("关于"),
        tr("QNote 1.0.0\n"
           "© 2024 GohJophen. All rights reserved.\n"
           "gohjophen@outlook.com\n\n"
           "Description:\n"
           "This is a very simple text editor.")
        );

}

void Widget::onCursorPositionChanged(){
    //qDebug() << "onCursorPositionChanged() triggered";
    ui->labelPosition->setText("");
    QTextCursor cursor = ui->textEdit->textCursor();
    //qDebug() << cursor.columnNumber();
    // 打印当前行号和列号，便于调试
    //qDebug() << "Cursor column:" << cursor.columnNumber() << ", line:" << cursor.blockNumber();


    // 获取当前行号和列号（行号和列号都是从 0 开始的）
    int lineNumber = cursor.blockNumber() + 1;  // 行号从 1 开始
    int columnNumber = cursor.columnNumber()+1; // 列号从 1 开始

    //qDebug() << columnNumber;

    // 设置标签内容为 "第 x 行 第 y 列"
    QString positionText = QString("第 %1 行, 第 %2 列").arg(lineNumber).arg(columnNumber);
    ui->labelPosition->setText(positionText);

    //设置当前行高亮
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection ext;

    //把光标所在的行传入ext
    ext.cursor = ui->textEdit->textCursor();

    //设置颜色
    QBrush qBrush(Qt::yellow);
    ext.format.setBackground(qBrush);
    ext.format.setProperty(QTextFormat::FullWidthSelection,true);

    //设置
    extraSelections.append(ext);
    ui->textEdit->setExtraSelections(extraSelections);

}

