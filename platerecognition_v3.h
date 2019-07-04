#ifndef PLATERECOGNITION_V3_H
#define PLATERECOGNITION_V3_H

#include <opencv2/ml/ml.hpp>

#include "platecategory.h"
#include "platelocator_v3.h"
#include "platechar_svm.h"
#include "charsegment_v3.h"

class PlateRecognition_V3
{
private:



public:
    PlateRecognition_V3();

    static PlateInfo CheckLeftAndRightToRemove(PlateInfo plateInfo);

    static void CheckPlateColor(PlateInfo plateInfo);

    static QList<PlateInfo> Recognize(cv::Mat matSource);

    static PlateInfo GetPlateInfoByMultiMethodAndMultiColor(PlateInfo plateInfo);

    static bool JudgePlateRightful(PlateInfo plateInfo);

    static PlateInfo GetPlateInfoByMultiMethod(PlateInfo plateInfo, PlateColor plateColor);

    static PlateInfo GetPlateInfo(PlateInfo plateInfo, PlateColor plateColor, CharSplitMethod splitMethod);



};

#endif // PLATERECOGNITION_V3_H
