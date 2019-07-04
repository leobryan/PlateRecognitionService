#ifndef PLATECHAR_SVM_H
#define PLATECHAR_SVM_H

#include <opencv2/ml/ml.hpp>
#include <stdlib.h>
#include "platecategory.h"
#include <QDir>
#include <QTime>

#define random(x) (rand()%x)

class PlateChar_SVM
{
private:
    static cv::Ptr<cv::ml::SVM> svm;

public:
    static bool isReady;
    static cv::Size HOGWinSize;
    static cv::Size HOGBlockSize;
    static cv::Size HOGBlockStride;
    static cv::Size HOGCellSize;
    static int HOGNBits;

    PlateChar_SVM();

    static std::vector<float> ComputeHogDescriptors(cv::Mat image);

    static bool Train(cv::Mat samples, cv::Mat responses);

    static void Save(QString fileName);

    static void Load(QString fileName);

    static bool isCorrectTrainningDirectory(QString path);

    static PlateChar Test(cv::Mat matTest);

    static PlateChar Test(QString fileName);

    static void SaveCharSample(CharInfo charInfo,QString libPath);

    static void SaveCharSample(cv::Mat charMat, PlateChar plateChar, QString libPath,QString shortFileNameNoExt);

    static void SaveCharSample(cv::Mat charMat, PlateChar plateChar, QString libPath);

    static bool PrepareCharTrainningDirectory(QString path);

};

#endif // PLATECHAR_SVM_H
