#pragma once
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QtGlobal>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow  *ui;

    void            loadFile            ();

    void            loadPartFile        (quint16 bPart);
    void            savePart1File       ();
    void            saveAsPart1File     ();
    void            loadPart1File       ();

    void            savePart2File       ();
    void            saveAsPart2File     ();
    void            loadPart2File       ();

    void            saveDiffFile        ();
    void            saveAsDiffFile      ();

    void            split               ();
    void            diffParts           ();
    void            merge               ();

    QString         part1SavedFileName,  part2SavedFileName, diffSavedFileName;

    QByteArray      original, part1, part1_r, part1_g, part1_b, part2, part2_r, part2_g, part2_b, diff, diff_r, diff_g, diff_b, joined;

    quint16         downScaleExponent = 4u;
    bool            bSimplified = true, bSum = true;
};
