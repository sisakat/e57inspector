#include "about.h"
#include "./ui_about.h"

#include "version.h"

#include <QDirIterator>
#include <QFile>
#include <QTextEdit>
#include <sstream>

About::About(QWidget* parent) : QDialog(parent), ui(new Ui::About)
{
    ui->setupUi(this);

    std::stringstream ss;
    ss << "# E57 Inspector\n";
    ss << "Version " << INFO_FILEVERSION_STRING << " (" << APP_GIT_HASH
       << "), built on " << APP_BUILD_DATE;

    ui->headerLabel->setText(QString::fromStdString(ss.str()));
    ui->headerLabel->setTextFormat(Qt::TextFormat::MarkdownText);

    QFile aboutMarkdown(":/text/about.md");
    if (aboutMarkdown.open(QIODevice::ReadOnly))
    {
        QString aboutText(aboutMarkdown.readAll());
        aboutText.append("---\n");
        aboutText.append(INFO_LEGALCOPYRIGHT_STRING);
        ui->aboutLabel->setTextFormat(Qt::TextFormat::MarkdownText);
        ui->aboutLabel->setText(aboutText);
    }

    QDirIterator licenseIterator(":/text/licenses",
                                 QDirIterator::NoIteratorFlags);
    while (licenseIterator.hasNext())
    {
        licenseIterator.next();
        auto fileInfo = licenseIterator.fileInfo();
        QFile licenseFile(fileInfo.absoluteFilePath());
        if (licenseFile.open(QIODevice::ReadOnly))
        {
            auto* document = new QTextDocument(ui->licenseTabWidget);
            document->setMarkdown(licenseFile.readAll());
            auto* textEdit = new QTextEdit(ui->licenseTabWidget);
            textEdit->setDocument(document);
            textEdit->setReadOnly(true);
            ui->licenseTabWidget->addTab(textEdit, fileInfo.baseName());
        }
    }

    ui->tabWidget->setCurrentIndex(0);
}

About::~About() = default;
