#include "AboutDialog.h"

const char *thirdPartyText = "<h3>libssh2</h3>\n"
"Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org><br>"
"Copyright (c) 2005,2006 Mikhail Gusarov <dottedmag@dottedmag.net><br>"
"Copyright (c) 2006-2007 The Written Word, Inc.<br>"
"Copyright (c) 2007 Eli Fant <elifantu@mail.ru><br>"
"Copyright (c) 2009-2014 Daniel Stenberg<br>"
"Copyright (C) 2008, 2009 Simon Josefsson<br>"
"All rights reserved.<br>"
"<br>"
"Redistribution and use in source and binary forms,<br>"
"with or without modification, are permitted provided<br>"
"that the following conditions are met:<br>"
"<br>"
"  Redistributions of source code must retain the above<br>"
"  copyright notice, this list of conditions and the<br>"
"  following disclaimer.<br>"
"<br>"
"  Redistributions in binary form must reproduce the above<br>"
"  copyright notice, this list of conditions and the following<br>"
"  disclaimer in the documentation and/or other materials<br>"
"  provided with the distribution.<br>"
"<br>"
"  Neither the name of the copyright holder nor the names<br>"
"  of any other contributors may be used to endorse or<br>"
"  promote products derived from this software without<br>"
"  specific prior written permission.<br>"
"<br>"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND<br>"
"CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES,<br>"
"INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES<br>"
"OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE<br>"
"ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR<br>"
"CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,<br>"
"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,<br>"
"BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR<br>"
"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS<br>"
"INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,<br>"
"WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING<br>"
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE<br>"
"USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY<br>"
"OF SUCH DAMAGE.";

AboutDialog::AboutDialog()
{
    this->setWindowTitle("About SSH Commander");
    this->setWindowIcon(QIcon(":/images/help-browser.svg"));

    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *captionLabel = new QLabel("SSH Commander", this);
    captionLabel->setMinimumWidth(500);
    QFont font = captionLabel->font();
    font.setPointSize(18);
    font.setBold(true);
    captionLabel->setFont(font);
    captionLabel->setStyleSheet("QLabel { color : grey; }");
    captionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(captionLabel);

    QIcon programIcon = QIcon(":/images/utilities-terminal.svg");
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(programIcon.pixmap(32));
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    QLabel *versionLabel = new QLabel(QString("Version: ") + SSHCOMMANDER_VERSION, this);
    versionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

    QLabel *commitIdLabel = new QLabel(QString("Git commit: ") + GIT_COMMIT_ID, this);
    commitIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    commitIdLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(commitIdLabel);

    QLabel *copyrightLabel = new QLabel("\nCopyright © 2016-2017 Andreas Heck\n\n"
            "This program is free software under the\n"
            "terms of the GNU General Public License Version 2", this);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(copyrightLabel);

    QLabel *versionsLabel = new QLabel("Library Versions", this);
    font = versionsLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    versionsLabel->setFont(font);
    versionsLabel->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(versionsLabel);

    QFormLayout *formLayout = new QFormLayout();
    QLabel *qtCompileLabel = new QLabel(QT_VERSION_STR, this);
    qtCompileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *qtRuntimeLabel = new QLabel(qVersion(), this);
    qtRuntimeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *libssh2CompileLabel = new QLabel(LIBSSH2_VERSION);
    libssh2CompileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    formLayout->addRow(tr("Qt Version (compile time):"), qtCompileLabel);
    formLayout->addRow(tr("Qt Version (runtime):"), qtRuntimeLabel);
    formLayout->addRow(tr("libssh2 (compile time):"), libssh2CompileLabel);
    layout->addLayout(formLayout);

    QLabel *licensesLabel = new QLabel("Third-Party Licenses", this);
    font = licensesLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    licensesLabel->setFont(font);
    licensesLabel->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(licensesLabel);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setText(thirdPartyText);
    layout->addWidget(textEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QObject::connect(okButton, SIGNAL (clicked()), this, SLOT (accept()));
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}
