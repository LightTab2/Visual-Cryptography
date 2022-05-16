#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::split()
{
        quint32 width = ui->originalImage->pixmap().width(), height = ui->originalImage->pixmap().height();

        part1.resize(original.size() * 4u);
        part2.resize(original.size() * 4u);

        memset(part1.data(), 0, original.size() * 4u);
        memset(part2.data(), 0, original.size() * 4u);

        width *= 2u;
        height *= 2u;

        for (quint32 y = 0u; y != height; y += 2u)
            for (quint32 x = 0u; x != width; x += 2u)
            {
                //Not read alpha channel
                for (quint32 j = 0u; j != 3; ++j)
                {
                    //losowanie indeksów
                    std::vector<quint32> ids;
                    ids.push_back(j + y * width * 4u + x * 4u);
                    ids.push_back(j + y * width * 4u + (x + 1u) * 4u);
                    ids.push_back(j + (y + 1u) * width * 4u + x * 4u);
                    ids.push_back(j + (y + 1u) * width * 4u + (x + 1u) * 4u);

                    quint16 rand = QRandomGenerator::global()->bounded(4);
                    quint32 id1 = ids[rand], id2, id3, id4;
                    ids.erase(ids.begin() + rand);
                    rand = QRandomGenerator::global()->bounded(3);
                    id2 = ids[rand];
                    ids.erase(ids.begin() + rand);
                    rand = QRandomGenerator::global()->bounded(2);
                    id3 = ids[rand];
                    ids.erase(ids.begin() + rand);
                    id4 = ids[0];

                    if (bSimplified)
                    {
                        uchar orig = original[j + y * width + x * 2u]/128u;
                        if (orig)
                        {
                            part1[id1] = static_cast<uchar>(255u);
                            part2[id1] = 0u;
                            part1[id2] = static_cast<uchar>(255u);
                            part2[id2] = 0u;
                            part2[id3] = static_cast<uchar>(255u);
                            part1[id3] = 0u;
                            part2[id4] = static_cast<uchar>(255u);
                            part1[id4] = 0u;
                        }
                        else
                        {
                            part1[id1] = static_cast<uchar>(255u);
                            part2[id1] = static_cast<uchar>(255u);
                            part1[id2] = static_cast<uchar>(255u);
                            part2[id2] = static_cast<uchar>(255u);

                            part1[id3] = 0u;
                            part2[id3] = 0u;
                            part1[id4] = 0u;
                            part2[id4] = 0u;
                        }
                    }
                    //To jest chyba jednak połączenie dziwnej steganografii z kryptografią wizualną
                    else
                    {
                        quint16 max = 256u >> downScaleExponent;
                        quint16 halfMax = max/2u;
                        quint16 quarterMax = halfMax/2u;

                        quint16 orig = (static_cast<quint16>(original[j + y * width + x * 2u])) >> downScaleExponent;
                        if (orig < halfMax)
                        {
                            quint32 toShift = orig << 2u;
                            quint32 firstShift = QRandomGenerator::global()->bounded(qMin(max, toShift + 1u)), secondShift = toShift - firstShift;
                            if (secondShift > max)
                            {
                                firstShift += secondShift - max + 1u;
                                secondShift = max - 1u;
                            }
                            quint32 firstLift = QRandomGenerator::global()->bounded(128u - quarterMax - firstShift), secondLift = QRandomGenerator::global()->bounded(128u - quarterMax - secondShift);
                            //Powinienem wymyslic ,,sprawiedliwszy'' algorytm rozdzielania toShift, zebym nie musial robic tego, co jest ponizej (+ to jest przeciek informacji)
                            if (QRandomGenerator::global()->bounded(2))
                            {
                                part1[id1] = static_cast<uchar>(127u + quarterMax + firstShift + firstLift);
                                part2[id1] = static_cast<uchar>(127u + quarterMax + firstLift);
                                part1[id2] = static_cast<uchar>(127u + quarterMax + secondLift);
                                part2[id2] = static_cast<uchar>(127u + quarterMax + secondShift + secondLift);
                            }
                            else
                            {
                                part2[id1] = static_cast<uchar>(127u + quarterMax + firstShift + firstLift);
                                part1[id1] = static_cast<uchar>(127u + quarterMax + firstLift);
                                part2[id2] = static_cast<uchar>(127u + quarterMax + secondLift);
                                part1[id2] = static_cast<uchar>(127u + quarterMax + secondShift + secondLift);
                            }
                            //todo: zamienic na jakas wartosc statystyczna zamiast losowego pixela
                            quint16 rand = QRandomGenerator::global()->bounded(128u - quarterMax);
                            rand = 127u - quarterMax + rand;
                            part1[id3] = static_cast<uchar>(rand);
                            part2[id3] = static_cast<uchar>(rand);
                            rand = QRandomGenerator::global()->bounded(128u - quarterMax);
                            rand = 127u - quarterMax + rand;
                            part1[id4] = static_cast<uchar>(rand);
                            part2[id4] = static_cast<uchar>(rand);
                        }
                        else
                        {
                            quint32 toShift = (orig - halfMax) << 2u;
                            quint32 firstShift = QRandomGenerator::global()->bounded(qMin(max, 1u + toShift)), secondShift = QRandomGenerator::global()->bounded(qMin(max, 1u + toShift - firstShift)),
                                    thirdShift = QRandomGenerator::global()->bounded(qMin(max, 1u + toShift - firstShift - secondShift)),
                                    fourthShift = toShift - firstShift - secondShift - thirdShift;
                            if (fourthShift > max)
                            {
                                thirdShift += fourthShift - max + 1u;
                                fourthShift = max - 1u;
                            }
                            if (thirdShift > max)
                            {
                                secondShift += thirdShift - max + 1u;
                                thirdShift = max - 1u;
                            }
                            if (secondShift > max)
                            {
                                firstShift += secondShift - max + 1u;
                                secondShift = max - 1u;
                            }
                            quint32 firstLift = QRandomGenerator::global()->bounded(128u - quarterMax - firstShift),     secondLift = QRandomGenerator::global()->bounded(128u - quarterMax - secondShift),
                                    thirdLift = QRandomGenerator::global()->bounded(128u - quarterMax - thirdShift),     fourthLift = QRandomGenerator::global()->bounded(128u - quarterMax - fourthShift);
                            if (QRandomGenerator::global()->bounded(2u))
                            {
                                part1[id1] = static_cast<uchar>(127u + quarterMax + firstShift + firstLift);
                                part2[id1] = static_cast<uchar>(127u - quarterMax + firstLift);
                                part1[id2] = static_cast<uchar>(127u - quarterMax + secondLift);
                                part2[id2] = static_cast<uchar>(127u + quarterMax + secondShift + secondLift);
                                part1[id3] = static_cast<uchar>(127u + quarterMax + thirdShift + thirdLift);
                                part2[id3] = static_cast<uchar>(127u - quarterMax + thirdLift);
                                part1[id4] = static_cast<uchar>(127u - quarterMax + fourthLift);
                                part2[id4] = static_cast<uchar>(127u + quarterMax + fourthShift + fourthLift);

                            }
                            else
                            {
                                part2[id1] = static_cast<uchar>(127u + quarterMax + firstShift + firstLift);
                                part1[id1] = static_cast<uchar>(127u - quarterMax + firstLift);
                                part2[id2] = static_cast<uchar>(127u - quarterMax + secondLift);
                                part1[id2] = static_cast<uchar>(127u + quarterMax + secondShift + secondLift);
                                part2[id3] = static_cast<uchar>(127u + quarterMax + thirdShift + thirdLift);
                                part1[id3] = static_cast<uchar>(127u - quarterMax + thirdLift);
                                part2[id4] = static_cast<uchar>(127u - quarterMax + fourthLift);
                                part1[id4] = static_cast<uchar>(127u + quarterMax + fourthShift + fourthLift);
                            }
                        }
                    }
                }
            }

        QImage  image(reinterpret_cast<const unsigned char*>(part1.constData()), width, height, QImage::Format_RGB32);
        QPixmap pixie = QPixmap::fromImage(image);
        ui->part1Image->setPixmap(QPixmap::fromImage(image));

        part1_r.resize(part1.size());
        part1_g.resize(part1.size());
        part1_b.resize(part1.size());
        memset(part1_r.data(), 0, part1.size());
        memset(part1_g.data(), 0, part1.size());
        memset(part1_b.data(), 0, part1.size());

        for (quint32 i = 0u; i != part1.size(); ++i)
        {
            //Not read alpha channel
            if ((i % 4u) == 3u)
                continue;
            switch (i % 4u)
            {
                case 1u:
                    part1_g[i]    = part1[i];
                break;
                case 2u:
                    part1_r[i]    = part1[i];
                break;
                default:
                    part1_b[i]    = part1[i];
                    break;
            }
        }

        image = QImage(reinterpret_cast<const unsigned char*>(part1_r.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->rPart1Image->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(part1_g.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->gPart1Image->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(part1_b.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->bPart1Image->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(part2.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->part2Image->setPixmap(QPixmap::fromImage(image));

        part2_r.resize(part2.size());
        part2_g.resize(part2.size());
        part2_b.resize(part2.size());
        memset(part2_r.data(), 0, part2.size());
        memset(part2_g.data(), 0, part2.size());
        memset(part2_b.data(), 0, part2.size());

        for (quint32 i = 0u; i != part2.size(); ++i)
        {
            //Not read alpha channel
            if ((i % 4u) == 3u)
                continue;
            switch (i % 4u)
            {
                case 1u:
                    part2_g[i]    = part2[i];
                break;
                case 2u:
                    part2_r[i]    = part2[i];
                break;
                default:
                    part2_b[i]    = part2[i];
                    break;
            }
        }

        image = QImage(reinterpret_cast<const unsigned char*>(part2_r.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->rPart2Image->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(part2_g.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->gPart2Image->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(part2_b.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->bPart2Image->setPixmap(pixie);

        diffParts();
}

void MainWindow::diffParts()
{
    ui->diffImage->setPixmap(QPixmap());
    ui->rDiffImage->setPixmap(QPixmap());
    ui->gDiffImage->setPixmap(QPixmap());
    ui->bDiffImage->setPixmap(QPixmap());
    ui->mergedImage->setPixmap(QPixmap());
    if (part1.size() != 0u && part1.size() == part2.size())
    {
        diff.resize(part1.size());
        diff_r.resize(diff.size());
        diff_g.resize(diff.size());
        diff_b.resize(diff.size());
        memset(diff_r.data(), 0, diff.size());
        memset(diff_g.data(), 0, diff.size());
        memset(diff_b.data(), 0, diff.size());

        for (quint32 i = 0u; i != part1.size(); ++i)
        {
            //Not read alpha channel
            if ((i % 4u) == 3u)
                continue;
            if (bSum)
            {
                quint16 sum = static_cast<quint16>(part1[i]) + static_cast<quint16>(part2[i]);
                if (sum > 255u)
                    diff[i] = 255u;
                else diff[i] = sum;
            }
            else diff[i] = static_cast<uchar>(std::abs((static_cast<quint16>(part1[i]) - static_cast<quint16>(part2[i])) << downScaleExponent) + (1 << (downScaleExponent - 1u)));
            switch (i % 4u)
            {
                case 1u:
                    diff_g[i]    = diff[i];
                break;
                case 2u:
                    diff_r[i]    = diff[i];
                break;
                default:
                    diff_b[i]    = diff[i];
                    break;
            }
        }

        quint32 width = ui->part1Image->pixmap().width(), height = ui->part1Image->pixmap().height();
        QImage  image(reinterpret_cast<const unsigned char*>(diff.constData()), width, height, QImage::Format_RGB32);
        QPixmap pixie = QPixmap::fromImage(image);
        ui->diffImage->setPixmap(pixie);

        image = QImage(reinterpret_cast<const unsigned char*>(diff_r.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->rDiffImage->setPixmap(pixie);
        image = QImage(reinterpret_cast<const unsigned char*>(diff_g.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->gDiffImage->setPixmap(pixie);
        image = QImage(reinterpret_cast<const unsigned char*>(diff_b.constData()), width, height, QImage::Format_RGB32);
        pixie = QPixmap::fromImage(image);
        ui->bDiffImage->setPixmap(pixie);

        merge();
    }
}

void MainWindow::merge()
{
    quint32 width = ui->diffImage->pixmap().width(), height = ui->diffImage->pixmap().height();
    QImage  image(reinterpret_cast<const unsigned char*>(diff.constData()), width, height, QImage::Format_RGB32);
    image = image.scaledToHeight(height/2u, Qt::SmoothTransformation);
    QPixmap pixie = QPixmap::fromImage(image);
    ui->mergedImage->setPixmap(pixie);
}
