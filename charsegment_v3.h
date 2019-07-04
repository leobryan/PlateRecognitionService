#ifndef CHARSEGMENT_V3_H
#define CHARSEGMENT_V3_H

#include <opencv2/opencv.hpp>
#include "platecategory.h"
#include "platechar_svm.h"
#include "QDebug"


class CharSegment_V3
{
public:
    CharSegment_V3();
    static cv::Mat ClearMaoding(cv::Mat threshold);
    static cv::Mat ClearBorder(cv::Mat threshold);
    static cv::Mat ClearMaodingAndBorder(cv::Mat gray, PlateColor plateColor);
    static QList<CharInfo> SplitCharsInPlateMat(cv::Mat plateMat, QList<cv::Rect> rects);
    static QList<CharInfo> SplitPlateForAutoSample(cv::Mat plateMat);
    static QList<CharInfo> SplitPlateYellowThenBlue(cv::Mat plateMat);
    static QList<CharInfo> SplitPlateBlueThenYellow(cv::Mat plateMat);
    static QList<CharInfo> SplitPlateByIndexTransform(cv::Mat originalMat,PlateColor plateColor,
                                                       int leftLimit = 0,int rightLimit = 0,
                                                       int topLimit = 0,int bottomLimit = 0,
                                                       int minWidth = 2,int maxWidth = 30,
                                                       int minHeight = 10,int maxHeight = 80,
                                                       float minRatio = 0.1f,float maxRatio = 0.7f);

    static QList<CharInfo> SplitPlateByLogTransform(cv::Mat originalMat,PlateColor plateColor,
                                                       int leftLimit = 0,int rightLimit = 0,
                                                       int topLimit = 0,int bottomLimit = 0,
                                                       int minWidth = 2,int maxWidth = 30,
                                                       int minHeight = 10,int maxHeight = 80,
                                                       float minRatio = 0.1f,float maxRatio = 0.7f);

    static QList<CharInfo> SplitPlateByGammaTransform(cv::Mat originalMat,PlateColor plateColor,
                                                       float gammaFactor = 0.40f,int leftLimit = 0,
                                                       int rightLimit = 0,int topLimit = 0,
                                                       int bottomLimit = 0,int minWidth = 2,
                                                       int maxWidth = 30,int minHeight = 10,
                                                       int maxHeight = 80,float minRatio = 0.1f,float maxRatio = 0.7f);
    static QList<CharInfo> SplitPlateByOriginal(cv::Mat originalMat,cv::Mat plateMat,PlateColor plateColor,
                                                 CharSplitMethod charSplitMethod = CharSplitMethod::原图,
                                                 int leftLimit = 0,int rightLimit = 0,
                                                 int topLimit = 0,int bottomLimit = 0,
                                                 int minWidth = 2,int maxWidth = 30,
                                                 int minHeight = 10,int maxHeight = 80,
                                                 float minRatio = 0.1f,float maxRatio = 0.7f);
    static bool VerifyRect(cv::Rect rect,
                           int minWidth = 2,int maxWidth = 30,
                           int minHeight = 10,int maxHeight = 80,
                           float minRatio = 0.1f,float maxRatio = 0.7f);
    static bool NotOnBorder(cv::Rect rectToJudge,cv::Size borderSize,
                            int leftLimit = 0,int rightLimit = 0,
                            int topLimit = 0,int bottomLimit = 0);
    static void SortRectsByLeft_ASC(QList<cv::Rect> rects);
    static void SortRectsByHeight_ASC(QList<cv::Rect> rects);
    static cv::Rect MergeRect(cv::Rect A,cv::Rect B);
    static QList<cv::Rect> AdjustRects(QList<cv::Rect> rects);
    static QList<cv::Rect> MergeRects(QList<cv::Rect> rects);
    static QList<cv::Rect> RejectInnerRectFromRects(QList<cv::Rect> rects);
    static float GetRectsAverageHeight(QList<cv::Rect> rects);
    static int GetRectsMaxHeight(QList<cv::Rect> rects);
    static int GetMedianRectsTop(QList<cv::Rect> rects);
    static int GetMedianRectsBottom(QList<cv::Rect> rects);
    // orginally from Utilities class
    static cv::Rect GetSafeRect(cv::Rect rect, cv::Mat plateMat);
    static cv::Mat IndexTransform(cv::Mat originalMap);
    static cv::Mat LogTransform(cv::Mat originalMap);
    static cv::Mat GammaTransform(cv::Mat originalMap);
    static PlateColor pc;


private:

    static bool RectTopCompare(cv::Rect x,cv::Rect y);

    static bool RectBottomCompare(cv::Rect x,cv::Rect y);

    static bool RectHeightCompare(cv::Rect x,cv::Rect y);

    static bool RectLeftCompare(cv::Rect x,cv::Rect y);

    static bool CharInfoLeftCompare(CharInfo x,CharInfo y);

};

#endif // CHARSEGMENT_V3_H
