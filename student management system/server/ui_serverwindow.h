/********************************************************************************
** Form generated from reading UI file 'serverwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWINDOW_H
#define UI_SERVERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerWindow
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *serverControlGroup;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *portSpinBox;
    QPushButton *startServerBtn;
    QPushButton *stopServerBtn;
    QSpacerItem *horizontalSpacer;
    QLabel *statusLabel;
    QGroupBox *clientInfoGroup;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *clientTable;
    QGroupBox *logGroup;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *logTextEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *clearLogBtn;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *ServerWindow)
    {
        if (ServerWindow->objectName().isEmpty())
            ServerWindow->setObjectName(QStringLiteral("ServerWindow"));
        ServerWindow->resize(900, 600);
        verticalLayout = new QVBoxLayout(ServerWindow);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        serverControlGroup = new QGroupBox(ServerWindow);
        serverControlGroup->setObjectName(QStringLiteral("serverControlGroup"));
        horizontalLayout = new QHBoxLayout(serverControlGroup);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(serverControlGroup);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        portSpinBox = new QSpinBox(serverControlGroup);
        portSpinBox->setObjectName(QStringLiteral("portSpinBox"));
        portSpinBox->setMinimum(1024);
        portSpinBox->setMaximum(65535);
        portSpinBox->setValue(8888);

        horizontalLayout->addWidget(portSpinBox);

        startServerBtn = new QPushButton(serverControlGroup);
        startServerBtn->setObjectName(QStringLiteral("startServerBtn"));

        horizontalLayout->addWidget(startServerBtn);

        stopServerBtn = new QPushButton(serverControlGroup);
        stopServerBtn->setObjectName(QStringLiteral("stopServerBtn"));
        stopServerBtn->setEnabled(false);

        horizontalLayout->addWidget(stopServerBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        statusLabel = new QLabel(serverControlGroup);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));

        horizontalLayout->addWidget(statusLabel);


        verticalLayout->addWidget(serverControlGroup);

        clientInfoGroup = new QGroupBox(ServerWindow);
        clientInfoGroup->setObjectName(QStringLiteral("clientInfoGroup"));
        verticalLayout_2 = new QVBoxLayout(clientInfoGroup);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        clientTable = new QTableWidget(clientInfoGroup);
        if (clientTable->columnCount() < 4)
            clientTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        clientTable->setObjectName(QStringLiteral("clientTable"));

        verticalLayout_2->addWidget(clientTable);


        verticalLayout->addWidget(clientInfoGroup);

        logGroup = new QGroupBox(ServerWindow);
        logGroup->setObjectName(QStringLiteral("logGroup"));
        verticalLayout_3 = new QVBoxLayout(logGroup);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        logTextEdit = new QTextEdit(logGroup);
        logTextEdit->setObjectName(QStringLiteral("logTextEdit"));
        logTextEdit->setReadOnly(true);

        verticalLayout_3->addWidget(logTextEdit);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        clearLogBtn = new QPushButton(logGroup);
        clearLogBtn->setObjectName(QStringLiteral("clearLogBtn"));

        horizontalLayout_2->addWidget(clearLogBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(logGroup);


        retranslateUi(ServerWindow);

        QMetaObject::connectSlotsByName(ServerWindow);
    } // setupUi

    void retranslateUi(QWidget *ServerWindow)
    {
        ServerWindow->setWindowTitle(QApplication::translate("ServerWindow", "\345\255\246\347\224\237\347\256\241\347\220\206\347\263\273\347\273\237 - \346\234\215\345\212\241\347\253\257", Q_NULLPTR));
        serverControlGroup->setTitle(QApplication::translate("ServerWindow", "\346\234\215\345\212\241\345\231\250\346\216\247\345\210\266", Q_NULLPTR));
        label->setText(QApplication::translate("ServerWindow", "\347\233\221\345\220\254\347\253\257\345\217\243\357\274\232", Q_NULLPTR));
        startServerBtn->setText(QApplication::translate("ServerWindow", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", Q_NULLPTR));
        stopServerBtn->setText(QApplication::translate("ServerWindow", "\345\201\234\346\255\242\346\234\215\345\212\241\345\231\250", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("ServerWindow", "\347\212\266\346\200\201\357\274\232\346\234\252\345\220\257\345\212\250", Q_NULLPTR));
        statusLabel->setStyleSheet(QApplication::translate("ServerWindow", "color: red; font-weight: bold;", Q_NULLPTR));
        clientInfoGroup->setTitle(QApplication::translate("ServerWindow", "\350\277\236\346\216\245\345\256\242\346\210\267\347\253\257", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem = clientTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("ServerWindow", "\345\256\242\346\210\267\347\253\257ID", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = clientTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("ServerWindow", "IP\345\234\260\345\235\200", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = clientTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("ServerWindow", "\350\277\236\346\216\245\346\227\266\351\227\264", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem3 = clientTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("ServerWindow", "\347\212\266\346\200\201", Q_NULLPTR));
        logGroup->setTitle(QApplication::translate("ServerWindow", "\350\277\220\350\241\214\346\227\245\345\277\227", Q_NULLPTR));
        clearLogBtn->setText(QApplication::translate("ServerWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ServerWindow: public Ui_ServerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWINDOW_H
