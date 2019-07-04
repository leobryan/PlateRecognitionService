#include "platelocator_v3.h"

PlateLocator_V3::PlateLocator_V3()
{

}

bool PlateLocator_V3::VerifyPlateSize(cv::Size size, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    int width = size.width;
    int height = size.height;
    if((width == 0)||(height == 0))
        return false;
    float ratio = (float)height/width;
    bool result = ((width>minWidth)&&(width<maxWidth))&&((height>minHeight)&&(height<maxHeight))&&((ratio>minRatio)&&(ratio<maxRatio));
    return result;
}

QList<PlateInfo> PlateLocator_V3::LocatePlatesForCameraAdjust(cv::Mat matSource, cv::Mat *matProcess, int blur_Size, int sobel_Scale, int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<PlateInfo> plateInfos = LocatePlatesForAutoSample(matSource,matProcess,blur_Size,sobel_Scale,sobel_Delta,sobel_X_Weight,sobel_Y_Weight,morph_Size_Width,morph_Size_Height,minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio);
    for (int i = plateInfos.count()-1;i>=0;i--) {
        if (plateInfos[i].plateCategory == PlateCategory::非车牌)
            plateInfos.removeAt(i);
    }
    return plateInfos;
}

QList<PlateInfo> PlateLocator_V3::LocatePlatesForAutoSample(cv::Mat matSource, cv::Mat *matProcess, int blur_Size, int sobel_Scale, int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<PlateInfo> plateInfos = QList<PlateInfo>();
    cv::Mat gray;
    cv::Mat blur;
    if (matSource.empty()||(matSource.rows == 0)||(matSource.cols == 0))
    {
        *matProcess = cv::Mat(0,0,CV_8UC1);
        return plateInfos;
    }

    cv::cvtColor(matSource,gray,cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray,blur,cv::Size(blur_Size,blur_Size),0,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat hsv;
    // 1.orig to hsv
    cv::cvtColor(matSource,hsv,cv::COLOR_BGR2HSV);
    // 2.hsv with v equalized
    cv::Mat* hsvSplits = new cv::Mat[3];
    cv::split(hsv,hsvSplits);

    cv::equalizeHist(hsvSplits[2],hsvSplits[2]);
    cv::Mat hsvEqualizeHist = cv::Mat();
    cv::merge(hsvSplits,3,hsvEqualizeHist);
    cv::Scalar blueStart = cv::Scalar(100,70,70);
    cv::Scalar blueEnd = cv::Scalar(140,255,255);
    cv::Mat blue;
    cv::inRange(hsvEqualizeHist,blueStart,blueEnd,blue);
    cv::Scalar yellowStart = cv::Scalar(15,70,70);
    cv::Scalar yellowEnd = cv::Scalar(40,255,255);
    cv::Mat yellow;
    cv::inRange(hsvEqualizeHist,yellowStart,yellowEnd,yellow);
    // 3.blue+yellow
    cv::Mat add = blue + yellow;
    // 4.threshold
    cv::Mat threshold;
    cv::threshold(add,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU|cv::ThresholdTypes::THRESH_BINARY);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    // 5.threshold close+erode
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchys;
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    int isPlateCount = 0;
    for (int i = 0;i < contours.size();i++) {
        cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if(VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            if(plateCategory != PlateCategory::非车牌) isPlateCount++;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.rotatedRect = rotatedRect;
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::颜色法;

            plateInfos.append(plateInfo);
        }
    }
    if(isPlateCount>0) return plateInfos;
    // 1.orig
    // 2.Gaussian blur
    cv::GaussianBlur(matSource,blur,cv::Size(blur_Size,blur_Size),0,0,cv::BorderTypes::BORDER_DEFAULT);
    // 3. blur
    cv::cvtColor(blur,gray,cv::COLOR_BGR2GRAY);
    cv::Mat grad_x;
    cv::Sobel(gray,grad_x,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_x;
    cv::convertScaleAbs(grad_x,abs_grad_x);
    cv::Mat grad_y;
    cv::Sobel(gray,grad_y,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_y;
    cv::convertScaleAbs(grad_y,abs_grad_y);
    // 4.grad
    cv::Mat grad;
    cv::addWeighted(abs_grad_x,sobel_X_Weight,abs_grad_y,sobel_Y_Weight,0,grad);
    // 5.threshold
    cv::threshold(grad,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);
    element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    // 6.threshold close+erode
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));

    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    *matProcess = threshold_Erode.clone();
    contours.clear();
    hierarchys.clear();
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    for (int i = 0;i < contours.size();i++) {
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if (VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            PlateInfo plateInfo = PlateInfo();
            plateInfo.rotatedRect = rotatedRect;
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::Sobel法;
            plateInfos.append(plateInfo);
        }
    }
    return plateInfos;
}

QList<PlateInfo> PlateLocator_V3::LocatePlatesForAutoSampleWithAllPara(cv::Mat matSource, cv::Mat *matProcess, int blur_Size, int sobel_Scale, int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio, int bluelow_H, int bluelow_S, int bluelow_V, int blueup_H, int blueup_S, int blueup_V, int yellowlow_H, int yellowlow_S, int yellowlow_V, int yellowup_H, int yellowup_S, int yellowup_V)
{
    QList<PlateInfo> plateInfos = QList<PlateInfo>();
    cv::Mat gray;
    cv::Mat blur;
    if (matSource.empty()||(matSource.rows == 0)||(matSource.cols == 0))
    {
        *matProcess = cv::Mat(0,0,CV_8UC1);
        return plateInfos;
    }
    cv::cvtColor(matSource,gray,cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray,blur,cv::Size(blur_Size,blur_Size),0,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat hsv;
    cv::cvtColor(matSource,hsv,cv::COLOR_BGR2HSV);
    cv::Mat* hsvSplits = new cv::Mat[3];
    cv::split(hsv,hsvSplits);
    cv::equalizeHist(hsvSplits[2],hsvSplits[2]);
    cv::Mat hsvEqualizeHist = cv::Mat();
    cv::merge(hsvSplits,3,hsvEqualizeHist);
    cv::Scalar blueStart = cv::Scalar(bluelow_H,bluelow_S,bluelow_V);
    cv::Scalar blueEnd = cv::Scalar(blueup_H,blueup_S,blueup_V);
    cv::Mat blue;
    cv::inRange(hsvEqualizeHist,blueStart,blueEnd,blue);
    cv::Scalar yellowStart = cv::Scalar(yellowlow_H,yellowlow_S,yellowlow_V);
    cv::Scalar yellowEnd = cv::Scalar(yellowup_H,yellowup_S,yellowup_V);
    cv::Mat yellow;
    cv::inRange(hsvEqualizeHist,yellowStart,yellowEnd,yellow);
    cv::Mat add = blue + yellow;
    cv::Mat threshold;
    cv::threshold(add,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU|cv::ThresholdTypes::THRESH_BINARY);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchys;
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    int isPlateCount = 0;
    for (int i = 0;i < contours.size();i++) {
        cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if(VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            if(plateCategory != PlateCategory::非车牌) isPlateCount++;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.rotatedRect = rotatedRect;
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::颜色法;

            plateInfos.append(plateInfo);
        }
    }
    if(isPlateCount>0) return plateInfos;
    cv::GaussianBlur(matSource,blur,cv::Size(blur_Size,blur_Size),0,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::cvtColor(blur,gray,cv::COLOR_BGR2GRAY);
    cv::Mat grad_x;
    cv::Sobel(gray,grad_x,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_x;
    cv::convertScaleAbs(grad_x,abs_grad_x);
    cv::Mat grad_y;
    cv::Sobel(gray,grad_y,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_y;
    cv::convertScaleAbs(grad_y,abs_grad_y);
    cv::Mat grad;
    cv::addWeighted(abs_grad_x,sobel_X_Weight,abs_grad_y,sobel_Y_Weight,0,grad);
    cv::threshold(grad,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);
    element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    *matProcess = threshold_Erode.clone();
    contours.clear();
    hierarchys.clear();
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    for (int i = 0;i < contours.size();i++) {
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if (VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            PlateInfo plateInfo = PlateInfo();
            plateInfo.rotatedRect = rotatedRect;
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::Sobel法;
            plateInfos.append(plateInfo);
        }
    }
    return plateInfos;
}

QList<PlateInfo> PlateLocator_V3::LocatePlates(cv::Mat matSource, int blur_Size, int sobel_Scale, int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<PlateInfo> plateInfos = LocatePlatesByColor(matSource,blur_Size,morph_Size_Width,morph_Size_Height,minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio);
    if(plateInfos.count() > 0)
        return plateInfos;
    plateInfos = LocatePlatesBySobel(matSource,blur_Size,sobel_Scale,sobel_Delta,sobel_X_Weight,sobel_Y_Weight,morph_Size_Width,morph_Size_Height,minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio);
    return plateInfos;
}

QList<PlateInfo> PlateLocator_V3::LocatePlatesByColor(cv::Mat matSource, int blur_Size, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<PlateInfo> plateInfos = QList<PlateInfo>();
    if(matSource.empty())
        return plateInfos;
    cv::Mat hsv;
    cv::cvtColor(matSource,hsv,cv::COLOR_BGR2HSV);
    cv::Mat* hsvSplits = new cv::Mat[3];
    cv::split(hsv,hsvSplits);
    cv::equalizeHist(hsvSplits[2],hsvSplits[2]);
    cv::Mat hsvEqualizeHist;
    cv::merge(hsvSplits,3,hsvEqualizeHist);
    cv::Scalar blueStart = cv::Scalar(100,70,70);
    cv::Scalar blueEnd = cv::Scalar(140,255,255);
    cv::Mat blue;
    cv::inRange(hsvEqualizeHist,blueStart,blueEnd,blue);
    cv::Scalar yellowStart = cv::Scalar(15,70,70);
    cv::Scalar yellowEnd = cv::Scalar(40,255,255);
    cv::Mat yellow;
    cv::inRange(hsvEqualizeHist,yellowStart,yellowEnd,yellow);
    cv::Mat add = blue + yellow;
    cv::Mat threshold;
    cv::threshold(add,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchys;
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    for (int i = 0;i < contours.size();i++) {
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if(VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            if(plateCategory != PlateCategory::非车牌) continue;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::颜色法;

            plateInfos.append(plateInfo);
        }
    }
    return plateInfos;
}

QList<PlateInfo> PlateLocator_V3::LocatePlatesBySobel(cv::Mat matSource, int blur_Size, int sobel_Scale, int sobel_Delta, int sobel_X_Weight, int sobel_Y_Weight, int morph_Size_Width, int morph_Size_Height, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<PlateInfo> plateInfos = QList<PlateInfo>();
    if(matSource.empty())
        return plateInfos;
    cv::Mat gray;
    cv::Mat blur;
    cv::GaussianBlur(matSource,blur,cv::Size(blur_Size,blur_Size),0,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::cvtColor(blur,gray,cv::COLOR_BGR2GRAY);
    cv::Mat grad_x;
    cv::Sobel(gray,grad_x,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_x;
    cv::convertScaleAbs(grad_x,abs_grad_x);
    cv::Mat grad_y;
    cv::Sobel(gray,grad_y,CV_16S,1,0,3,sobel_Scale,sobel_Delta,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_y;
    cv::convertScaleAbs(grad_y,abs_grad_y);
    cv::Mat grad;
    cv::addWeighted(abs_grad_x,sobel_X_Weight,abs_grad_y,sobel_Y_Weight,0,grad);
    cv::Mat threshold;
    cv::threshold(grad,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(morph_Size_Width,morph_Size_Height));
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchys;
    cv::findContours(threshold_Erode,contours,hierarchys,cv::RetrievalModes::RETR_EXTERNAL,cv::ContourApproximationModes::CHAIN_APPROX_NONE);
    for (int i = 0;i < contours.size();i++) {
        cv::Rect rectROI = cv::boundingRect(contours[i]);
        if (VerifyPlateSize(rectROI.size(),minWidth,maxWidth,minHeight,maxHeight,minRatio,maxRatio))
        {
            cv::Mat matROI = matSource(rectROI);
            PlateCategory plateCategory = PlateCategory_SVM::Test(matROI);
            if(plateCategory == PlateCategory::非车牌) continue;
            PlateInfo plateInfo = PlateInfo();
            plateInfo.originalRect = rectROI;
            plateInfo.originalMat = matROI;
            plateInfo.plateCategory = plateCategory;
            plateInfo.plateLocateMethod = PlateLocateMethod::Sobel法;
            plateInfos.append(plateInfo);
        }
    }
    return plateInfos;
}

