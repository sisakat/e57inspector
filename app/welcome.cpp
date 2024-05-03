#include "welcome.h"
#include "./ui_welcome.h"
#include <QFile>

Welcome::Welcome(QWidget* parent) : QWidget(parent), ui(new Ui::Welcome)
{
    ui->setupUi(this);

    QFile welcomeFile(":/text/welcome.html");
    if (welcomeFile.open(QIODevice::ReadOnly))
    {
        auto* document = new QTextDocument(ui->textBrowser);
        document->setHtml(welcomeFile.readAll());
        document->setDocumentMargin(10);
        ui->textBrowser->setDocument(document);
        ui->textBrowser->setReadOnly(true);

        // scroll to top
        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.setPosition(0);
        ui->textBrowser->setTextCursor(cursor);
    }
}

Welcome::~Welcome() = default;
