/********************************************************************************
** Form generated from reading UI file 'adipfinal.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADIPFINAL_H
#define UI_ADIPFINAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_adipfinalClass
{
public:
    QWidget *centralWidget;
    QPushButton *importButton;
    QPushButton *saveButton;
    QLabel *imgshowLabel;
    QPushButton *executionButton;
    QPushButton *object_slicButton;
    QCheckBox *redcheckBox;
    QCheckBox *greencheckBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QCheckBox *blackcheckBox;
    QCheckBox *yellowcheckBox;
    QPushButton *clearButton;
    QLabel *label_2;
    QLabel *timelabel;
    QLineEdit *savenamelineEdit;
    QLabel *label_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *adipfinalClass)
    {
        if (adipfinalClass->objectName().isEmpty())
            adipfinalClass->setObjectName(QString::fromUtf8("adipfinalClass"));
        adipfinalClass->resize(1014, 696);
        centralWidget = new QWidget(adipfinalClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        importButton = new QPushButton(centralWidget);
        importButton->setObjectName(QString::fromUtf8("importButton"));
        importButton->setGeometry(QRect(10, 30, 151, 41));
        QFont font;
        font.setPointSize(16);
        importButton->setFont(font);
        saveButton = new QPushButton(centralWidget);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setGeometry(QRect(10, 150, 151, 41));
        saveButton->setFont(font);
        imgshowLabel = new QLabel(centralWidget);
        imgshowLabel->setObjectName(QString::fromUtf8("imgshowLabel"));
        imgshowLabel->setGeometry(QRect(190, 70, 451, 231));
        executionButton = new QPushButton(centralWidget);
        executionButton->setObjectName(QString::fromUtf8("executionButton"));
        executionButton->setGeometry(QRect(10, 270, 151, 41));
        executionButton->setFont(font);
        object_slicButton = new QPushButton(centralWidget);
        object_slicButton->setObjectName(QString::fromUtf8("object_slicButton"));
        object_slicButton->setGeometry(QRect(10, 210, 151, 41));
        QFont font1;
        font1.setPointSize(12);
        object_slicButton->setFont(font1);
        redcheckBox = new QCheckBox(centralWidget);
        redcheckBox->setObjectName(QString::fromUtf8("redcheckBox"));
        redcheckBox->setGeometry(QRect(10, 410, 141, 21));
        QFont font2;
        font2.setPointSize(14);
        redcheckBox->setFont(font2);
        greencheckBox = new QCheckBox(centralWidget);
        greencheckBox->setObjectName(QString::fromUtf8("greencheckBox"));
        greencheckBox->setGeometry(QRect(10, 350, 141, 21));
        greencheckBox->setFont(font2);
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(30, 520, 71, 31));
        lineEdit->setFont(font);
        lineEdit->setContextMenuPolicy(Qt::DefaultContextMenu);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 481, 101, 31));
        label->setFont(font);
        blackcheckBox = new QCheckBox(centralWidget);
        blackcheckBox->setObjectName(QString::fromUtf8("blackcheckBox"));
        blackcheckBox->setGeometry(QRect(10, 440, 161, 21));
        blackcheckBox->setFont(font2);
        yellowcheckBox = new QCheckBox(centralWidget);
        yellowcheckBox->setObjectName(QString::fromUtf8("yellowcheckBox"));
        yellowcheckBox->setGeometry(QRect(10, 380, 141, 21));
        yellowcheckBox->setFont(font2);
        clearButton = new QPushButton(centralWidget);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setGeometry(QRect(10, 90, 151, 41));
        clearButton->setFont(font);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(230, 20, 161, 31));
        label_2->setFont(font);
        timelabel = new QLabel(centralWidget);
        timelabel->setObjectName(QString::fromUtf8("timelabel"));
        timelabel->setGeometry(QRect(400, 20, 141, 31));
        timelabel->setFont(font);
        savenamelineEdit = new QLineEdit(centralWidget);
        savenamelineEdit->setObjectName(QString::fromUtf8("savenamelineEdit"));
        savenamelineEdit->setGeometry(QRect(10, 590, 113, 20));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 550, 121, 31));
        label_3->setFont(font);
        adipfinalClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(adipfinalClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1014, 21));
        adipfinalClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(adipfinalClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        adipfinalClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(adipfinalClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        adipfinalClass->setStatusBar(statusBar);

        retranslateUi(adipfinalClass);

        QMetaObject::connectSlotsByName(adipfinalClass);
    } // setupUi

    void retranslateUi(QMainWindow *adipfinalClass)
    {
        adipfinalClass->setWindowTitle(QApplication::translate("adipfinalClass", "adipfinal", nullptr));
        importButton->setText(QApplication::translate("adipfinalClass", "import", nullptr));
        saveButton->setText(QApplication::translate("adipfinalClass", "save", nullptr));
        imgshowLabel->setText(QString());
        executionButton->setText(QApplication::translate("adipfinalClass", "execution", nullptr));
        object_slicButton->setText(QApplication::translate("adipfinalClass", "object_SLIC", nullptr));
        redcheckBox->setText(QApplication::translate("adipfinalClass", "background", nullptr));
        greencheckBox->setText(QApplication::translate("adipfinalClass", "object", nullptr));
        lineEdit->setText(QApplication::translate("adipfinalClass", "10", nullptr));
        label->setText(QApplication::translate("adipfinalClass", "object speed", nullptr));
        blackcheckBox->setText(QApplication::translate("adipfinalClass", "background_move", nullptr));
        yellowcheckBox->setText(QApplication::translate("adipfinalClass", "object_move", nullptr));
        clearButton->setText(QApplication::translate("adipfinalClass", "clear", nullptr));
        label_2->setText(QApplication::translate("adipfinalClass", "program execution: ", nullptr));
        timelabel->setText(QString());
        savenamelineEdit->setText(QApplication::translate("adipfinalClass", "test.avi", nullptr));
        label_3->setText(QApplication::translate("adipfinalClass", "save fileName", nullptr));
    } // retranslateUi

};

namespace Ui {
    class adipfinalClass: public Ui_adipfinalClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADIPFINAL_H
