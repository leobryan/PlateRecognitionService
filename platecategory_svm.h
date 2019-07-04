#ifndef PLATECATEGORY_SVM_H
#define PLATECATEGORY_SVM_H

#include <opencv2/ml/ml.hpp>
#include <stdlib.h>
#include <QTime>
#include <QDir>

#include "platecategory.h"

#define random(x) (rand()%x)

class PlateCategory_SVM
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

    PlateCategory_SVM();

    static void SavePlateSample(PlateInfo plateInfo, QString fileName);

    static void SavePlateSample(cv::Mat matPlate, PlateCategory plateCategory, QString libPath);

    static void SavePlateSample(cv::Mat matPlate, PlateCategory plateCategory, QString libPath, QString shortFileNameNoExt);

    static std::vector<float> ComputeHogDescriptors(cv::Mat image);

    static bool Train(cv::Mat samples, cv::Mat responses);

    static void Save(QString fileName);

    static void Load(QString fileName);

    static bool isCorrectTrainningDirectory(QString path);

    static PlateCategory Test(cv::Mat matTest);

    static PlateCategory Test(QString fileName);

    static bool PreparePlateTrainningDirectory(QString path);

};

#endif // PLATECATEGORY_SVM_H
