#include "AboutDialog.h"

AboutDialog::AboutDialog()
{
    this->setWindowTitle("About SSH Commander");
    this->setWindowIcon(QIcon(":/images/help-browser.svg"));

    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *caption = new QLabel("SSH Commander");
    QFont font = caption->font();
    font.setPointSize(18);
    font.setBold(true);
    caption->setFont(font);
    caption->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(caption);

    QFormLayout *formLayout = new QFormLayout();
    QLabel *versionLabel = new QLabel(SSHCOMMANDER_VERSION);
    versionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *commitIdLabel = new QLabel(GIT_COMMIT_ID);
    commitIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *qtCompileLabel = new QLabel(QT_VERSION_STR);
    qtCompileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel *qtRuntimeLabel = new QLabel(qVersion());
    qtRuntimeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    formLayout->addRow(tr("Version:"), versionLabel);
    formLayout->addRow(tr("Commit ID:"), commitIdLabel);
    formLayout->addRow(tr("Qt Version (compile time):"), qtCompileLabel);
    formLayout->addRow(tr("Qt Version (runtime):"), qtRuntimeLabel);
    layout->addLayout(formLayout);

    QPushButton *okButton = new QPushButton("OK");
    QObject::connect(okButton, SIGNAL (clicked()), this, SLOT (accept()));
    layout->addWidget(okButton);

    this->setLayout(layout);
}
