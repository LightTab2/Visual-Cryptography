#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionLoad,             &QAction::triggered,            this,   &MainWindow::loadFile);
    connect(ui->actionLoad1,            &QAction::triggered,            this,   &MainWindow::loadPart1File);
    connect(ui->actionLoad2,            &QAction::triggered,            this,   &MainWindow::loadPart2File);
    connect(ui->actionSave1,            &QAction::triggered,            this,   &MainWindow::savePart1File);
    connect(ui->actionSave2,            &QAction::triggered,            this,   &MainWindow::savePart2File);
    connect(ui->actionSaveAs1,          &QAction::triggered,            this,   &MainWindow::saveAsPart1File);
    connect(ui->actionSaveAs2,          &QAction::triggered,            this,   &MainWindow::saveAsPart2File);
    connect(ui->actionSave3,            &QAction::triggered,            this,   &MainWindow::saveDiffFile);
    connect(ui->actionSaveAs3,          &QAction::triggered,            this,   &MainWindow::saveAsDiffFile);
    connect(ui->regenerateButton,       &QPushButton::pressed,          this,   &MainWindow::split);
    connect(ui->simplifiedCheck,        &QCheckBox::toggled,            this,   [this](bool bChecked){ bSimplified = bChecked; });
    connect(ui->sumCheck,               &QCheckBox::toggled,            this,   [this](bool bChecked){ bSum  = bChecked;
        if (bSum)
        {
            ui->simplifiedCheck->setEnabled(false);
            ui->diffLabel->setText("Suma");
        }
        else
        {
            ui->simplifiedCheck->setEnabled(true);
            ui->diffLabel->setText("Różnica");
        }
        });
}

void MainWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Wczytaj plik"), "",
                                                    tr("Bezstratne obrazy (*.png *.bmp)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Nie udało się otworzyć pliku"),
            file.errorString());

        return;
    }

    QPixmap pixmap(fileName);//, fileName.right(3).toUpper().toLatin1());
    QImage img = pixmap.toImage();
    img = img.convertToFormat(QImage::Format_RGB32);
    ui->originalImage->setPixmap(QPixmap::fromImage(img));
    original.resize(img.sizeInBytes());
    memcpy(original.data(), img.bits(), img.sizeInBytes());
    split();
}

void MainWindow::loadPartFile(quint16 partID)
{
    QLabel      *part           = ((partID == 1) ? ui->part1Image        :       ui->part2Image),
                *part_r         = ((partID == 1) ? ui->rPart1Image       :       ui->rPart2Image),
                *part_g         = ((partID == 1) ? ui->gPart1Image       :       ui->gPart2Image),
                *part_b         = ((partID == 1) ? ui->bPart1Image       :       ui->bPart2Image);

    QByteArray  *full           = ((partID == 1) ? &part1                :       &part2),
                *r              = ((partID == 1) ? &part1_r              :       &part2_r),
                *g              = ((partID == 1) ? &part1_g              :       &part2_g),
                *b              = ((partID == 1) ? &part1_b              :       &part2_b);

    QString     *savedFileName  = ((partID == 1) ? &part1SavedFileName   : &part2SavedFileName);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Wczytaj plik"), "",
                                                    tr("Bezstratne obrazy (*.png *.bmp)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Nie udało się otworzyć pliku"),
            file.errorString());

        return;
    }
    *savedFileName = fileName;
    QPixmap pixmap(*savedFileName);//, fileName.right(3).toUpper().toLatin1());
    QImage img = pixmap.toImage();
    img = img.convertToFormat(QImage::Format_RGB32);
    part->setPixmap(QPixmap::fromImage(img));
    full->resize(img.sizeInBytes());
    memcpy(full->data(), img.bits(), img.sizeInBytes());

    r->resize(full->size());
    g->resize(full->size());
    b->resize(full->size());
    memset(r->data(), 0, full->size());
    memset(g->data(), 0, full->size());
    memset(b->data(), 0, full->size());

    for (quint32 i = 0u; i != part1.size(); ++i)
    {
        //Not read alpha channel
        if ((i % 4u) == 3u)
            continue;
        switch (i % 4u)
        {
            case 1u:
                (*r)[i]    = (*full)[i];
            break;
            case 2u:
                (*g)[i]    = (*full)[i];
            break;
            default:
                (*b)[i]    = (*full)[i];
                break;
        }
    }

    quint32 width = part->pixmap().width(), height = part->pixmap().height();

    QImage  image(reinterpret_cast<const unsigned char*>(r->constData()), width, height, QImage::Format_RGB32);
    QPixmap pixie = QPixmap::fromImage(image);
    part_r->setPixmap(pixie);

    image = QImage(reinterpret_cast<const unsigned char*>(g->constData()), width, height, QImage::Format_RGB32);
    pixie = QPixmap::fromImage(image);
    part_g->setPixmap(pixie);

    image = QImage(reinterpret_cast<const unsigned char*>(b->constData()), width, height, QImage::Format_RGB32);
    pixie = QPixmap::fromImage(image);
    part_b->setPixmap(pixie);

    diffParts();
}

void MainWindow::savePart1File()
{
    if (part1SavedFileName == "")
        return;

    if (!ui->part1Image->pixmap().save(part1SavedFileName))
    {
        QMessageBox::information(this, tr("Błąd"), "Nie udało się zapisać obrazu!");
        return;
    }
}

void MainWindow::saveAsPart1File()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Zapisz plik jako"), "",
                                                    tr("Bezstratne obrazy (*.png *.bmp)"));
    if (fileName.isEmpty())
        return;

    part1SavedFileName = fileName;

    savePart1File();
}

void MainWindow::loadPart1File()
{
    loadPartFile(1);
}


void MainWindow::savePart2File()
{
    if (part2SavedFileName == "")
        return;

    if (!ui->part2Image->pixmap().save(part2SavedFileName))
    {
        QMessageBox::information(this, tr("Błąd"), "Nie udało się zapisać obrazu!");
        return;
    }
}

void MainWindow::saveAsPart2File()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Zapisz plik jako"), "",
                                                    tr("Bezstratne obrazy (*.png *.bmp)"));
    if (fileName.isEmpty())
        return;

    part2SavedFileName = fileName;

    savePart2File();
}

void MainWindow::loadPart2File()
{
    loadPartFile(2);
}

void MainWindow::saveDiffFile()
{
    if (diffSavedFileName == "")
        return;

    if (!ui->diffImage->pixmap().save(diffSavedFileName))
    {
        QMessageBox::information(this, tr("Błąd"), "Nie udało się zapisać obrazu!");
        return;
    }
}

void MainWindow::saveAsDiffFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Zapisz plik jako"), "",
                                                    tr("Bezstratne obrazy (*.png *.bmp)"));
    if (fileName.isEmpty())
        return;

    diffSavedFileName = fileName;

    saveDiffFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}
