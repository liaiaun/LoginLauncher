/********************************************************************************
** Form generated from reading UI file 'LoginLauncher.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINLAUNCHER_H
#define UI_LOGINLAUNCHER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginLauncherClass
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *HBoxLayout1;
    QLabel *userLabel;
    QLineEdit *userLineEdit;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *HBoxLayout2;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QPushButton *pushButton;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *LoginLauncherClass)
    {
        if (LoginLauncherClass->objectName().isEmpty())
            LoginLauncherClass->setObjectName(QStringLiteral("LoginLauncherClass"));
        LoginLauncherClass->resize(600, 400);
        horizontalLayoutWidget = new QWidget(LoginLauncherClass);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(130, 20, 331, 71));
        HBoxLayout1 = new QHBoxLayout(horizontalLayoutWidget);
        HBoxLayout1->setSpacing(6);
        HBoxLayout1->setContentsMargins(11, 11, 11, 11);
        HBoxLayout1->setObjectName(QStringLiteral("HBoxLayout1"));
        HBoxLayout1->setContentsMargins(0, 0, 0, 0);
        userLabel = new QLabel(horizontalLayoutWidget);
        userLabel->setObjectName(QStringLiteral("userLabel"));

        HBoxLayout1->addWidget(userLabel);

        userLineEdit = new QLineEdit(horizontalLayoutWidget);
        userLineEdit->setObjectName(QStringLiteral("userLineEdit"));

        HBoxLayout1->addWidget(userLineEdit);

        horizontalLayoutWidget_2 = new QWidget(LoginLauncherClass);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(130, 100, 331, 61));
        HBoxLayout2 = new QHBoxLayout(horizontalLayoutWidget_2);
        HBoxLayout2->setSpacing(6);
        HBoxLayout2->setContentsMargins(11, 11, 11, 11);
        HBoxLayout2->setObjectName(QStringLiteral("HBoxLayout2"));
        HBoxLayout2->setContentsMargins(0, 0, 0, 0);
        passwordLabel = new QLabel(horizontalLayoutWidget_2);
        passwordLabel->setObjectName(QStringLiteral("passwordLabel"));

        HBoxLayout2->addWidget(passwordLabel);

        passwordLineEdit = new QLineEdit(horizontalLayoutWidget_2);
        passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));

        HBoxLayout2->addWidget(passwordLineEdit);

        pushButton = new QPushButton(LoginLauncherClass);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(380, 190, 81, 41));
        textBrowser = new QTextBrowser(LoginLauncherClass);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(130, 170, 241, 151));

        retranslateUi(LoginLauncherClass);

        QMetaObject::connectSlotsByName(LoginLauncherClass);
    } // setupUi

    void retranslateUi(QWidget *LoginLauncherClass)
    {
        LoginLauncherClass->setWindowTitle(QApplication::translate("LoginLauncherClass", "LoginLauncher", 0));
        userLabel->setText(QApplication::translate("LoginLauncherClass", "\347\224\250\346\210\267\345\220\215\357\274\232", 0));
        passwordLabel->setText(QApplication::translate("LoginLauncherClass", "\345\257\206  \347\240\201\357\274\232", 0));
        pushButton->setText(QApplication::translate("LoginLauncherClass", "\347\231\273\345\275\225", 0));
    } // retranslateUi

};

namespace Ui {
    class LoginLauncherClass: public Ui_LoginLauncherClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINLAUNCHER_H
