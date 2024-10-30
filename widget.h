#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTextBlock>
#include <QShortcut>



QT_BEGIN_NAMESPACE
namespace Ui {class Widget;}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


public:
    void wheelEvent(QWheelEvent *event) override;//处理鼠标滚轮事件
    void closeEvent(QCloseEvent *event) override;//处理关闭控件的事件


private slots:
    void btnOpen_clicked();
    void btnSave_clicked();
    void btnClear_clicked();
    void btnClose_clicked();
    void btnAbout_clicked();
    void onCursorPositionChanged();


private:
    Ui::Widget *ui;
    QFile file;
    QSettings settings;
    QString originalText; // 用于存储打开文件时的原始文本
};
#endif // WIDGET_H
