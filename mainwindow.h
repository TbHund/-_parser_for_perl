#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QVector>
#include <cmath>
#include <QStack>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Tokenize(QString line);
    void FindOperands();
    void FindOperators();
    int findFirstNonSpace(const QString &str);

private slots:
    void on_openFile_clicked();

private:
    Ui::MainWindow *ui;
    QStringList stringlist;
    QStringList linelist;

    QStringList wordslist;
    bool insideString;
    bool longComment;

    // для метрики Джилба
    int NestingLevel = 0;
    int helperForNesting = 0;
    int secondhelper = 0;
    int switchHelper = 0;
    bool When = false;
    int whenCounter = 0;
    bool If_Else_Switch_is_met = false;
    int forIfCounter = 0;
    //when это свитч кейс в perl

    struct Operand {
        QString name;
        int amount;
    };
    QVector<Operand> vector;

    struct Operator {
        QString name;
        int amount;
    };
    QVector<Operator> Opvector;
    QVector<QString> operators = { ";","+", "-", "*", "/", "==", "!=",
                                 "**", "!=", "%", "<", ">", "<=",
                                 ">=", "&&", "||", "!", "=", "+=",
                                 "-=", "*=", "/=", "%=", "=>", "if",
                                 "for", "while", "last", "next", "redo",
                                  "..", "print", "elsif", "when"
                                 };
};
#endif // MAINWINDOW_H
