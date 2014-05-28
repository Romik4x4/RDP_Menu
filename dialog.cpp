/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QSettings>
#include <QNetworkInterface>
#include <QtNetwork>
#include <QDebug>

#include "dialog.h"

QString base64_encode(QString string);
QString base64_decode(QString string);


Dialog::Dialog()
{

    createFormGroupBox();
    createXrandr();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(romik()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(caseCombo, SIGNAL(activated(int)), this, SLOT(changeCase(int)));

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(formGroupBox); //! Ввод данных
    mainLayout->addWidget(caseCombo);    //! Выбор разрешения
    mainLayout->addWidget(buttonBox);    //! Кноки

    setLayout(mainLayout);

    setWindowTitle(tr("Remote connection"));

}

void Dialog::changeCase(int comboIndex) {

    //! qDebug() <<  caseCombo->itemData(comboIndex).toString().split(" ")[0];
    QProcess process;
    process.startDetached("/usr/bin/xrandr -s "+caseCombo->itemData(comboIndex).toString().split(" ")[0]);
    process.waitForFinished(-1);

}

void Dialog::romik() {

    QSettings *settings = new QSettings("settings.conf",QSettings::NativeFormat);

    settings->setValue("section/login",Username->text());
    settings->setValue("section/server", ipAddress->text());
    settings->setValue("section/password", base64_encode(Pass->text()));
    settings->sync();

    QProcess process;
    process.start("/usr/bin/xfreerdp -p "+Pass->text()+" -u "+Username->text()+" "+ipAddress->text());
    process.waitForFinished(-1);
    accept();

}


void Dialog::createXrandr() {

    QLabel *caseLabel = new QLabel;

    caseLabel->setText(tr("Display"));

    QProcess process;

    process.start("/usr/bin/xrandr");
    process.waitForReadyRead();
    process.waitForFinished();
    QByteArray xmode = process.readAllStandardOutput();

    QList<QByteArray> list;

    list = xmode.split('\n');

    caseCombo = new QComboBox;

    int pos=0,p=0;

    for (int i = 0; i < list.size(); ++i) {
        if (list.at(i).startsWith(" ")) {
            if (list.at(i).indexOf("*") > 0) pos = p;
            caseCombo->addItem(list.at(i).trimmed(),QVariant(list.at(i).trimmed()));
            p++;
        }
        }

     caseCombo->setCurrentIndex(pos);

}

void Dialog::createFormGroupBox()
{

    QString xipAddress;

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); i++)
    {
      if (ipAddressesList.at(i).toIPv4Address() && ipAddressesList.at(i).toString() != "127.0.0.1")
             xipAddress = ipAddressesList.at(i).toString();
    }


    formGroupBox = new QGroupBox(tr("Enter remote desktop parameters"));

    QFormLayout *layout = new QFormLayout;

    QSettings *settings = new QSettings("settings.conf",QSettings::NativeFormat);

    Pass = new QLineEdit ;
    ipAddress = new QLineEdit;
    Username = new QLineEdit;

    Username->setText(settings->value("section/login").toString());
    ipAddress->setText(settings->value("section/server").toString());
    Pass->setText(base64_decode(settings->value("section/password").toString()));

    Pass->setEchoMode(QLineEdit::Password);

    layout->addRow(new QLabel(tr("Your IP Address: ")),new QLabel(xipAddress));

    layout->addRow(new QLabel(tr("IP Address:")), ipAddress);
    layout->addRow(new QLabel(tr("Username:")),  Username);
    layout->addRow(new QLabel(tr("Password:")), Pass);

    formGroupBox->setLayout(layout);
}
