#include "charsegment_v3.h"

PlateColor CharSegment_V3::pc = PlateColor::未知;

CharSegment_V3::CharSegment_V3()
{

}

// 函数1.输入图片，抹去每行数值波动数小于7的行，去除钉子，输出
cv::Mat CharSegment_V3::ClearMaoding(cv::Mat threshold)
{
    // QList<float> *jumps = new QList<float>();
    cv::Mat result = threshold.clone();
    cv::Mat jump(result.rows,1,CV_32FC1);
    for (int rowIndex = 0;rowIndex<result.rows-1;rowIndex++)
    {
        int jumpCount = 0;
        for (int colIndex = 0;colIndex <result.cols-1;colIndex++)
        {
            if(result.at<unsigned char>(rowIndex,colIndex)!=
                    result.at<unsigned char>(rowIndex,colIndex+1)) jumpCount++;

        }
        // 记录每一行的数值波动数
        jump.at<int>(rowIndex,0) = jumpCount;
    }
    int x = 8;
    // 对于每一行的波动数小于7次的行置为0
    for (int rowIndex = 0;rowIndex < result.rows ; rowIndex++)
    {
        if (jump.at<int>(rowIndex)<=x)
        {
            for (int colIndex = 0;colIndex<result.cols;colIndex++)
            {
                result.at<unsigned char>(rowIndex,colIndex) = 0;
            }
        }
    }

    jump.release();
    return result;


}

//void clearMaoDing(Mat mask, int &top, int &bottom) {
//    const int x = 7;

//    for (int i = 0; i < mask.rows / 2; i++) {
//      int whiteCount = 0;
//      int jumpCount = 0;
//      for (int j = 0; j < mask.cols - 1; j++) {
//        if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;

//        if ((int) mask.at<uchar>(i, j) == 255) {
//          whiteCount++;
//        }
//      }
//      if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
//          whiteCount < 4) {
//        top = i;
//      }
//    }
//    top -= 1;
//    if (top < 0) {
//      top = 0;
//    }

//    // ok, find top and bottom boudnadry

//    for (int i = mask.rows - 1; i >= mask.rows / 2; i--) {
//      int jumpCount = 0;
//      int whiteCount = 0;
//      for (int j = 0; j < mask.cols - 1; j++) {
//        if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;
//        if (mask.at<uchar>(i, j) == 255) {
//          whiteCount++;
//        }
//      }
//      if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
//          whiteCount < 4) {
//        bottom = i;
//      }
//    }
//    bottom += 1;
//    if (bottom >= mask.rows) {
//      bottom = mask.rows - 1;
//    }

//    if (top >= bottom) {
//      top = 0;
//      bottom = mask.rows - 1;
//    }
//  }

// 函数2，抹去边界处的轮廓
cv::Mat CharSegment_V3::ClearBorder(cv::Mat threshold)
{
    int rows = threshold.rows;
    int cols = threshold.cols;
    // 认定超过此值时为边界
    int noJumpCountThresh = (int)(0.15f*cols);
    // 记录每一行是否为边界
    cv::Mat border(rows,1,CV_8UC1);
    // 每一行有15%的值与前一个点相同时认定为边界
    for (int rowIndex = 0;rowIndex<rows;rowIndex++) {
        int noJumpCount = 0;
        unsigned char isBorder =0;
        // 有超过15%的值相同时认为是边界
        for (int colIndex = 0;colIndex<cols-1;colIndex++) {
            if(threshold.at<unsigned char>(rowIndex,colIndex) ==
                    threshold.at<unsigned char>(rowIndex,colIndex+1)) noJumpCount++;
            // 认定本行为边界
            if (noJumpCount > noJumpCountThresh)
            {
                noJumpCount = 0;
                isBorder = 1;
                break;
            }
        }
        border.at<unsigned char>(rowIndex,0,isBorder);
    }

    int minTop = (int)(0.1f*rows);
    int maxTop = (int)(0.9f*rows);
    cv::Mat result = threshold.clone();
    // 对于0~0.1高度范围内每一行中是边界的行置为0
    for (int rowIndex = 0;rowIndex < minTop ; rowIndex++)
    {
        if (border.at<unsigned char>(rowIndex,0) == 1)
        {
            for (int colIndex = 0;colIndex<cols;colIndex++)
            {
                result.at<unsigned char>(rowIndex,colIndex) = 0;
            }
        }
    }
    // 对于0.9~1高度范围内每一行中是边界的行置为0
    for (int rowIndex = rows -1;rowIndex > maxTop ; rowIndex--)
    {
        if (border.at<unsigned char>(rowIndex,0) == 1)
        {
            for (int colIndex = 0;colIndex<cols;colIndex++)
            {
                result.at<unsigned char>(rowIndex,colIndex) = 0;
            }
        }
    }
    return result;
}

// 函数3，调用函数1、函数2,将灰度值图像进行二值化，去除杂质提取车牌部分的图片门槛图
cv::Mat CharSegment_V3::ClearMaodingAndBorder(cv::Mat gray, PlateColor plateColor)
{
    cv::Mat threshold;
    gray.type();
    switch(plateColor)
    {
    case PlateColor::蓝牌:
    case PlateColor::黑牌:
        // 源：threshold = gray.Threashold(1,255,ThresholdTypes.THRESH_OTSU | ThresholdTypes.THRESH_BINARY);
        cv::threshold(gray,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);

        break;
    case PlateColor::黄牌:
    case PlateColor::白牌:
    case PlateColor::绿牌:
        cv::threshold(gray,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY_INV);;
        break;
    case PlateColor::未知:
        cv::threshold(gray,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);;
        break;
    }
    cv::Mat matOfClearBorder = ClearBorder(threshold);
    cv::Mat matOfClearMaodingAndBorder = ClearMaoding(matOfClearBorder);

    return matOfClearMaodingAndBorder;
}

// 函数4，调用输入车牌图片、切割好的矩形
QList<CharInfo> CharSegment_V3::SplitCharsInPlateMat(cv::Mat plateMat, QList<cv::Rect> rects)
{
    pc = PlateColor::未知;
    if (PlateChar_SVM::isReady == false)
    {
        //throw  new QException ("字符识别库没有准备好") ;
        qDebug() << "字符识别库没有准备好";
    }
    QList<CharInfo> result;
    for (int index = 0; index < rects.count(); index++) {
        cv::Rect rect = rects[index];
        // 保证矩形在图形内
        rect = GetSafeRect(rect, plateMat);
        CharInfo charInfo;
        cv::Mat originalMat = plateMat(rect);

        charInfo.originalMat = originalMat;
        charInfo.originalRect = rect;
        // 识别
        charInfo.plateChar = PlateChar_SVM::Test(originalMat);
        result.append(charInfo);
    }
    qSort(result.begin(),result.end(),CharInfoLeftCompare);

    return result;

}
QList<CharInfo> CharSegment_V3::SplitPlateYellowThenBlue(cv::Mat plateMat)
{


    QList<CharInfo> charInfos_Original_Yellow = SplitPlateByOriginal(plateMat,plateMat, PlateColor::黄牌);
    QList<CharInfo> charInfos_IndexTransform_Yellow = SplitPlateByIndexTransform(plateMat,PlateColor::黄牌);
    QList<CharInfo> charInfos_GammaTransform_Yellow = SplitPlateByGammaTransform(plateMat,PlateColor::黄牌);
    QList<CharInfo> charInfos_LogTransform_Yellow = SplitPlateByLogTransform(plateMat, PlateColor::黄牌);
    QList<CharInfo> charInfos_Yellow;
    // charInfos_Blue.AddRange(charInfos_Original_Blue.ToArray());
    foreach (CharInfo ci, charInfos_Original_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_IndexTransform_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_GammaTransform_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_LogTransform_Yellow) charInfos_Yellow.append(ci);
    // 用SVM识别图片中字符的个数
    int isCharCount = 0;
    for (int index = 0; index < charInfos_Yellow.count(); index++)
    {
        charInfos_Yellow[index].plateChar  = PlateChar_SVM::Test(charInfos_Yellow.at(index).originalMat);
        // double rate = charInfo.originalMat.cols*1.0/charInfo.originalMat.rows
        if (charInfos_Yellow.at(index).plateChar != PlateChar::非字符 ) isCharCount++;
    }
    for(CharInfo ci : charInfos_Yellow)
    {
        qDebug() << PlateCharString[ci.plateChar];
    }
    if (isCharCount >= 15){
        pc = PlateColor::黄牌;
        return charInfos_Yellow;// 差不多当黄色识别出来了
    }


    //返回长度为零的集合，未识别
    // 先用蓝色识别法识别，不成功再使用黄色
    // 四种方法识别蓝色图像
    QList<CharInfo> charInfos_Original_Blue = SplitPlateByOriginal(plateMat,plateMat, PlateColor::蓝牌);
    QList<CharInfo> charInfos_IndexTransform_Blue = SplitPlateByIndexTransform(plateMat,PlateColor::蓝牌);
     QList<CharInfo> charInfos_GammaTransform_Blue = SplitPlateByGammaTransform(plateMat,PlateColor::蓝牌);
    QList<CharInfo> charInfos_LogTransform_Blue = SplitPlateByLogTransform(plateMat, PlateColor::蓝牌);
    //
    QList<CharInfo> charInfos_Blue;
    // charInfos_Blue.AddRange(charInfos_Original_Blue.ToArray());
    foreach (CharInfo ci, charInfos_Original_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_IndexTransform_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_GammaTransform_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_LogTransform_Blue) charInfos_Blue.append(ci);
    // 用SVM识别图片中字符的个数
    isCharCount = 0;
    for (int index = 0; index < charInfos_Blue.count(); index++)
    {
        charInfos_Blue[index].plateChar  = PlateChar_SVM::Test(charInfos_Blue.at(index).originalMat);
        // double rate = charInfo.originalMat.cols*1.0/charInfo.originalMat.rows
        if (charInfos_Blue.at(index).plateChar != PlateChar::非字符 ) isCharCount++;
    }
    // 如果用黄色切分字符，少于15个，就再用蓝色尝试切分
    if (isCharCount >= 15)
    {
        pc = PlateColor::蓝牌;
        return charInfos_Blue;// 差不多当蓝色识别出来了
    }

    QList<CharInfo> *empty = new QList<CharInfo>();
    return *empty;


}

QList<CharInfo> CharSegment_V3::SplitPlateBlueThenYellow(cv::Mat plateMat)
{
    // 先用蓝色识别法识别，不成功再使用黄色
    // 四种方法识别蓝色图像
    QList<CharInfo> charInfos_Original_Blue = SplitPlateByOriginal(plateMat,plateMat, PlateColor::蓝牌);
    QList<CharInfo> charInfos_IndexTransform_Blue = SplitPlateByIndexTransform(plateMat,PlateColor::蓝牌);
     QList<CharInfo> charInfos_GammaTransform_Blue = SplitPlateByGammaTransform(plateMat,PlateColor::蓝牌);
    QList<CharInfo> charInfos_LogTransform_Blue = SplitPlateByLogTransform(plateMat, PlateColor::蓝牌);
    //
    QList<CharInfo> charInfos_Blue;
    // charInfos_Blue.AddRange(charInfos_Original_Blue.ToArray());
    foreach (CharInfo ci, charInfos_Original_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_IndexTransform_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_GammaTransform_Blue) charInfos_Blue.append(ci);
    foreach (CharInfo ci, charInfos_LogTransform_Blue) charInfos_Blue.append(ci);
    // 用SVM识别图片中字符的个数
    int isCharCount = 0;
    for (int index = 0; index < charInfos_Blue.count(); index++)
    {
        charInfos_Blue[index].plateChar  = PlateChar_SVM::Test(charInfos_Blue.at(index).originalMat);
        // double rate = charInfo.originalMat.cols*1.0/charInfo.originalMat.rows
        if (charInfos_Blue.at(index).plateChar != PlateChar::非字符 ) isCharCount++;
    }
    // 如果用蓝色切分字符，少于15个，就再用⻩色尝试切分
    if (isCharCount >= 15)
    {
        pc = PlateColor::蓝牌;
        return charInfos_Blue;// 差不多当蓝色识别出来了
    }



    QList<CharInfo> charInfos_Original_Yellow = SplitPlateByOriginal(plateMat,plateMat, PlateColor::黄牌);
    QList<CharInfo> charInfos_IndexTransform_Yellow = SplitPlateByIndexTransform(plateMat,PlateColor::黄牌);
    QList<CharInfo> charInfos_GammaTransform_Yellow = SplitPlateByGammaTransform(plateMat,PlateColor::黄牌);
    QList<CharInfo> charInfos_LogTransform_Yellow = SplitPlateByLogTransform(plateMat, PlateColor::黄牌);
    QList<CharInfo> charInfos_Yellow;
    // charInfos_Blue.AddRange(charInfos_Original_Blue.ToArray());
    foreach (CharInfo ci, charInfos_Original_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_IndexTransform_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_GammaTransform_Yellow) charInfos_Yellow.append(ci);
    foreach (CharInfo ci, charInfos_LogTransform_Yellow) charInfos_Yellow.append(ci);
    // 用SVM识别图片中字符的个数
    isCharCount = 0;
    for (int index = 0; index < charInfos_Yellow.count(); index++)
    {
        charInfos_Yellow[index].plateChar  = PlateChar_SVM::Test(charInfos_Yellow.at(index).originalMat);
        // double rate = charInfo.originalMat.cols*1.0/charInfo.originalMat.rows
        if (charInfos_Yellow.at(index).plateChar != PlateChar::非字符 ) isCharCount++;
    }
    for(CharInfo ci : charInfos_Yellow)
    {
        qDebug() << PlateCharString[ci.plateChar];
    }
    if (isCharCount >= 15){
        pc = PlateColor::黄牌;
        return charInfos_Yellow;// 差不多当黄色识别出来了
    }

    QList<CharInfo> *empty = new QList<CharInfo>();
    return *empty;
    //返回长度为零的集合，未识别
}

// 函数5，调用函数6，7，8，9， 主函数，调用这个函数可以将车牌图片传入，将切好并识别的字符信息类传出
QList<CharInfo> CharSegment_V3::SplitPlateForAutoSample(cv::Mat plateMat)
{
    cv::Mat gray;
    // 转为灰色图像
    cv::cvtColor(plateMat,gray,cv::COLOR_BGR2GRAY);
            //cvtColor(ColorConversionCodes.BGR2GRAY);
    // 去除杂质
    cv::Mat regardBlue = ClearMaodingAndBorder(gray, PlateColor::蓝牌);
    cv::Mat regardYellow = ClearMaodingAndBorder(gray, PlateColor::黄牌);
    // blue<yellow:blue
    int countBlue = 0;
    int countYellow = 0;
    for(int i = 0;i<gray.rows;i++)
    {
        for (int j = 0;j<gray.cols;j++) {
            if(regardBlue.at<int>(i,j)!=0) countBlue++;
        }
    }
    for(int i = 0;i<gray.rows;i++)
    {
        for (int j = 0;j<gray.cols;j++) {
            if(regardYellow.at<int>(i,j)!=0) countYellow++;
        }
    }
    if(countBlue<countYellow)
    {
        return SplitPlateBlueThenYellow(plateMat);
    }
    else {
        return SplitPlateYellowThenBlue(plateMat);
    }
}

// 函数6，调用函数9、23，用指数变换结果分割字符
QList<CharInfo> CharSegment_V3::SplitPlateByIndexTransform(cv::Mat originalMat, PlateColor plateColor, int leftLimit, int rightLimit, int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    cv::Mat plateMat = IndexTransform(originalMat);
    return SplitPlateByOriginal(originalMat,plateMat, plateColor, CharSplitMethod::指数,
    leftLimit, rightLimit, topLimit, bottomLimit, minWidth, maxWidth, minHeight, maxHeight, minRatio, maxRatio);
}

// 函数7，调用函数9、24，用对数变换结果分割字符
QList<CharInfo> CharSegment_V3::SplitPlateByLogTransform(cv::Mat originalMat, PlateColor plateColor, int leftLimit, int rightLimit, int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    cv::Mat plateMat = LogTransform(originalMat);
    return SplitPlateByOriginal(originalMat,plateMat, plateColor, CharSplitMethod::对数,
    leftLimit, rightLimit, topLimit, bottomLimit, minWidth, maxWidth, minHeight, maxHeight, minRatio, maxRatio);
}

// 函数8，调用函数9、25，用伽马变换结果分割字符
QList<CharInfo> CharSegment_V3::SplitPlateByGammaTransform(cv::Mat originalMat, PlateColor plateColor, float gammaFactor, int leftLimit, int rightLimit, int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    cv::Mat plateMat = GammaTransform(originalMat);
    return SplitPlateByOriginal(originalMat,plateMat, plateColor, CharSplitMethod::伽马,
    leftLimit, rightLimit, topLimit, bottomLimit, minWidth, maxWidth, minHeight, maxHeight, minRatio, maxRatio);
}

// 函数9，用原图形分割字符
QList<CharInfo> CharSegment_V3::SplitPlateByOriginal(cv::Mat originalMat, cv::Mat plateMat, PlateColor plateColor, CharSplitMethod charSplitMethod, int leftLimit, int rightLimit, int topLimit, int bottomLimit, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    QList<CharInfo> result;
    cv::Mat gray;
    // 转为灰色图像

    cv::cvtColor(plateMat,gray,cv::COLOR_BGR2GRAY);

            //cvtColor(ColorConversionCodes.BGR2GRAY);
    // 去除杂质
    cv::Mat matOfClearMaodingAndBorder = ClearMaodingAndBorder(gray, plateColor);
    // 尝试开操作
//    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2,2));
//    cv::dilate(matOfClearMaodingAndBorder,matOfClearMaodingAndBorder,element);
//    cv::erode(matOfClearMaodingAndBorder,matOfClearMaodingAndBorder,element);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(1,3));
    cv::dilate(matOfClearMaodingAndBorder,matOfClearMaodingAndBorder,element);
    cv::erode(matOfClearMaodingAndBorder,matOfClearMaodingAndBorder,element);

    // 寻找门槛图像(为1的部分)的连通区域
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchyIndices;
    cv::findContours(matOfClearMaodingAndBorder,contours,
                     hierarchyIndices,
                     cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE);

    // 将抓取的字符矩形放入数组
    QList<cv::Rect> rects;
    qDebug()<< "切出的矩形数："<< contours.size();
    for (int index = 0; index < contours.size(); index++)
    {
        // 计算包含轮廓的最小矩形
        cv::Rect rect = cv::boundingRect(contours.at(index));
        rects.append(rect);

    }
    QList<cv::Rect> resultRects;
    // rects = MergeRects(rects);

    for (int index = 0; index < rects.size(); index++)
    {
        cv::Rect rect = rects.at(index);
         if (NotOnBorder(rect,plateMat.size(),leftLimit, rightLimit, topLimit, bottomLimit)
                 && VerifyRect(rect, minWidth, maxWidth, minHeight, maxHeight, minRatio, maxRatio))
            resultRects.append(rect);
    }
    // 删除矩形中的小矩形
    resultRects = RejectInnerRectFromRects(resultRects);
    // 调整大小
    resultRects = AdjustRects(resultRects);
    // 没找到 返回空
    qDebug()<<"矩形剩余个数："<< resultRects.count();
    if (resultRects.count() == 0) return result;
    // 将矩阵信息加入字符信息类CharInfo中
    for (int index = 0; index < resultRects.count(); index++) {
        CharInfo plateCharInfo;
        resultRects.replace(index,GetSafeRect(resultRects.at(index), originalMat));
        cv::Rect rectROI = resultRects.at(index);
        cv::Mat matROI = originalMat(rectROI);
        plateCharInfo.originalMat = matROI;
        plateCharInfo.originalRect = rectROI;
        plateCharInfo.charSplitMethod = charSplitMethod;
        plateCharInfo.plateChar = PlateChar::未识别字符;
        plateCharInfo.plateLocateMethod = PlateLocateMethod::颜色法;

        result.append(plateCharInfo);
    }
    // 从左到右排序
    qSort(result.begin(),result.end(),CharInfoLeftCompare);
    return result;
}

// 函数10，保证矩形符合参数要求的规范
bool CharSegment_V3::VerifyRect(cv::Rect rect, int minWidth, int maxWidth, int minHeight, int maxHeight, float minRatio, float maxRatio)
{
    int width = rect.width;
    int height = rect.height;
    if (width == 0 || height == 0) return false;
    float ratio = (float)width / height;
    return ((width > minWidth && width < maxWidth) &&
            (height > minHeight && height < maxHeight) &&
            (ratio > minRatio && ratio < maxRatio));
}

// 函数11，保证内容不在边界
bool CharSegment_V3::NotOnBorder(cv::Rect rectToJudge, cv::Size borderSize, int leftLimit, int rightLimit, int topLimit, int bottomLimit)
{
    float leftPercent = leftLimit / 100;
    float rightPercent = rightLimit / 100;
    float topPercent = topLimit / 100;
    float bottomPercent = bottomLimit / 100;
    float widthPercent = 1 - leftPercent - rightPercent;
    float heightPercent = 1 - topPercent - bottomPercent;
    int xLimit = (int)(borderSize.width * leftPercent);
    int yLimit = (int)(borderSize.height * topPercent);
    int widthLimit = (int)(borderSize.width * widthPercent);
    int heightLimit = (int)(borderSize.height * heightPercent);
    // TODO
    return (xLimit <= rectToJudge.x && yLimit <= rectToJudge.y
            && widthLimit >= rectToJudge.width && heightLimit >= rectToJudge.height);
}
// 函数12 按左到右排序
void CharSegment_V3::SortRectsByLeft_ASC(QList<cv::Rect> rects)
{
    qSort(rects.begin(),rects.end(),RectLeftCompare);
}
// 函数13 按高度排序
void CharSegment_V3::SortRectsByHeight_ASC(QList<cv::Rect> rects)
{
    qSort(rects.begin(),rects.end(),RectHeightCompare);

}
// 函数14，合并两个矩形
cv::Rect CharSegment_V3::MergeRect(cv::Rect A, cv::Rect B)
{
    cv::Rect result;
    int minX = (A.x<=B.x)? A.x:B.x;
    int minY = (A.y<=B.y)? A.y:B.y;
    int maxX = (A.width+A.x >= B.width+B.x)? A.width+A.x:B.width+B.x;
    int maxY = (A.height+A.y >= B.height+B.y)? A.height+A.y:B.height+B.y;
    result.x = minX;
    result.y = minY;
    result.width = maxX-minX;
    result.height = maxY-minY;

    return result;
}
// 函数15，调整字符大小使得所有矩形的大小相似，并包含字符
QList<cv::Rect> CharSegment_V3::AdjustRects(QList<cv::Rect> rects)
{
    float averageHeight = GetRectsAverageHeight(rects);
    float heightLimit = averageHeight * 0.5f;
    int medianTop = GetMedianRectsTop(rects);
    int medianBottom = GetMedianRectsBottom(rects);
    for (int index = rects.count() - 1; index >= 0; index--)
    {
        cv::Rect rect = rects[index];
        // rect = rects.at(index);
        if (rect.height >= heightLimit && rect.height < averageHeight) {
            int offsetTop = (rect.y - medianTop>0) ? rect.y - medianTop: medianTop - rect.y;
            int offsetBottom = (rect.y +rect.height - medianBottom >0) ?
                        rect.y +rect.height - medianBottom:-(rect.y +rect.height - medianBottom);
            if (offsetTop > offsetBottom) {
                rect.y = (int)(rect.y+rect.height - averageHeight);
            }
            rect.height = (int)averageHeight + 5;
            rects.replace(index,rect);
            // rects[index] = rect;
        }
    }
    return rects;
}

// 函数16，将需要合并的字符小矩形合并为大矩形
QList<cv::Rect> CharSegment_V3::MergeRects(QList<cv::Rect> rects)
{
    QList<int> *indexesOfMerge = new QList<int>();
    QList<int> *indexesBeMerged = new QList<int>();
    // int maxHeight = GetRectsMaxHeight(rects);
    float averageHeight = GetRectsAverageHeight(rects);
    float hightLimit = averageHeight * 0.5f;
    for (int index = rects.count() - 1; index >= 0; index--)
    {
        if (indexesBeMerged->contains(index)) continue;
        if (indexesOfMerge->contains(index)) continue;
        cv::Rect A = rects[index];
        if (A.height < hightLimit) continue;
        for (int i = rects.count() - 1; i >= 0; i--)
        {
            if (i == index) continue;
            cv::Rect B = rects[i];
            if (B.height > hightLimit) continue;
            if ((A.x >= B.x && A.x+A.width >= B.x+B.width) || (A.x <= B.x && A.x+A.width <= B.x+B.width))
            {
                cv::Rect rectMerge = MergeRect(A, B);
                if (VerifyRect(rectMerge))
                {
                    indexesBeMerged->append(i);
                    rects[index] = rectMerge;
                    indexesOfMerge->append(index);
                }
            }
        }
    }

    QList<cv::Rect> *result = new QList<cv::Rect>();
    for (int index = 0; index < rects.count(); index++)
    {
        if (indexesBeMerged->contains(index) == false)
            result->append(rects[index]);
    }
    return *result;
}

// 函数17 去除矩形中的小矩形
QList<cv::Rect> CharSegment_V3::RejectInnerRectFromRects(QList<cv::Rect> rects)
{
    for (int index = rects.count() - 1; index >= 0; index--)
    {
        cv::Rect rect = rects[index];
        //
        for (int i = 0; i < rects.count(); i++)
        {
            cv::Rect rectTemp = rects[i];
            // rect左上角在temp下面和右侧
            if ((rect.x >= rectTemp.x && rect.y >= rectTemp.y &&
            // rect右下角在temp上面和左侧
            rect.x+rect.width <= rectTemp.x+rectTemp.width &&
            rect.y+rect.height <= rectTemp.y+rectTemp.height) &&
            // rect宽或高小于temp ？
            (rect.width < rectTemp.width || rect.height < rectTemp.height))
            {
                rects.removeAt(index);
                break;
            }
        }
    }
    return rects;
}

// 函数18 取高度平均值
float CharSegment_V3::GetRectsAverageHeight(QList<cv::Rect> rects)
{
    float aver = 0;
    if(rects.count()==0) return aver;
    foreach(cv::Rect rect, rects)
    {
        aver += rect.height;
    }

    return aver/rects.count();
}
// 函数19 取高度最大值
int CharSegment_V3::GetRectsMaxHeight(QList<cv::Rect> rects)
{
    int max = 0;
    if(rects.count()==0) return max;
    foreach(cv::Rect rect, rects)
    {
        if(max<rect.height) max= rect.height;
    }

    return max;
}

// 函数20 取上沿中间值
int CharSegment_V3::GetMedianRectsTop(QList<cv::Rect> rects)
{
    if (rects.count() == 0) return 0;
    qSort(rects.begin(),rects.end(),RectTopCompare);
    int midianIndex = rects.count() / 2;
    return rects[midianIndex].y;
}

// 函数21 取下沿中间值
int CharSegment_V3::GetMedianRectsBottom(QList<cv::Rect> rects)
{
    if (rects.count() == 0) return 0;
    qSort(rects.begin(),rects.end(),RectBottomCompare);
    int midianIndex = rects.count() / 2;
    return rects[midianIndex].y+rects[midianIndex].height;
}

// 函数22 保证矩形在图像内部
cv::Rect CharSegment_V3::GetSafeRect(cv::Rect rect, cv::Mat plateMat)
{
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x+rect.width > plateMat.cols)
        rect.width = plateMat.cols-rect.x;
    if (rect.y+rect.height > plateMat.rows)
        rect.height= plateMat.rows-rect.y;
    return rect;
}
// 函数23 图像指数变换，将图像取平方除以255
cv::Mat CharSegment_V3::IndexTransform(cv::Mat originalMap)
{
    cv::Mat *result = new cv::Mat(originalMap.size(), originalMap.type());
    int rows = originalMap.rows;
    int cols = originalMap.cols;
    double k = 1.0 / 255.0;
    qDebug()<< k;
    for (int rowIndex = 0; rowIndex < rows; rowIndex++)
    {
        for (int colIndex = 0; colIndex < cols; colIndex++)
        {
            cv::Vec3b color = originalMap.at<cv::Vec3b>(rowIndex, colIndex);
            unsigned char B = color[0];
            unsigned char G = color[1];
            unsigned char R = color[2];
            B = (unsigned char)(k * B * B);
            G = (unsigned char)(k * G * G);
            R = (unsigned char)(k * R * R);

            color = cv::Vec3b(B, G, R);
            result->at<cv::Vec3b>(rowIndex,colIndex) = color;
            // result[rowIndex] =color;
            //qDebug()<< B << G  << R;
        }

    }

    return *result;
}

// 函数24 图像对数变换，将RGB色取对数乘系数
cv::Mat CharSegment_V3::LogTransform(cv::Mat originalMap)
{
    cv::Mat *result = new cv::Mat(originalMap.size(), originalMap.type());
    int rows = originalMap.rows;
    int cols = originalMap.cols;
    double k = 255 / log10(256.0);
    for (int rowIndex = 0; rowIndex < rows; rowIndex++)
    {
        for (int colIndex = 0; colIndex < cols; colIndex++)
        {
            cv::Vec3b color = originalMap.at<cv::Vec3b>(rowIndex, colIndex);
            unsigned char B = color[0];
            unsigned char G = color[1];
            unsigned char R = color[2];
            B = (unsigned char)(k * log10(B+1));
            G = (unsigned char)(k * log10(G+1));
            R = (unsigned char)(k * log10(R+1));
            color = cv::Vec3b(B, G, R);
            result->at<cv::Vec3b>(rowIndex,colIndex) = color;
        }
    }
    return *result;

}

// 函数25 图像伽马变换
cv::Mat CharSegment_V3::GammaTransform(cv::Mat originalMap)
{
    int lut[256];
    float gammaFactor = 0.4f;
    for (int index = 0; index < 256; index++)
    {
        float f = (index + 0.5f) / 255;
        f = (float) pow(f,gammaFactor);
        lut[index] = (int)(f * 255.0f - 0.5f);
        if (lut[index] > 255) lut[index] = 255;
    }

    cv::Mat result = originalMap.clone();
    if (originalMap.channels() == 1)
    {
        for (int rowIndex = 0; rowIndex < result.rows; rowIndex++)
        {
            for (int colIndex = 0; colIndex < result.cols; colIndex++)
            {
                int temp = result.at<int>(rowIndex, colIndex);
                result.at<int>(rowIndex, colIndex) = lut[temp];
            }
        }
    }
    else
    {
        for (int rowIndex = 0; rowIndex < result.rows; rowIndex++)
        {
            for (int colIndex = 0; colIndex < result.cols; colIndex++)
            {
                cv::Vec3b temp = result.at<cv::Vec3b>(rowIndex, colIndex);
                cv::Vec3b gamma((unsigned char)lut[temp[0]],
                        (unsigned char)lut[temp[1]],
                        (unsigned char)lut[temp[2]]);
                result.at<cv::Vec3b>(rowIndex, colIndex) =  gamma;
            }
        }
    }

    return result;
}

// 用于比较排序
bool CharSegment_V3::RectTopCompare(cv::Rect x, cv::Rect y)
{
    if(x.y < y.y)
    {
        return true;
    }
    return  false;
}

bool CharSegment_V3::RectBottomCompare(cv::Rect x, cv::Rect y)
{
    if(x.y+x.height < y.y+y.height)
    {
        return true;
    }
    return  false;
}

bool CharSegment_V3::RectHeightCompare(cv::Rect x, cv::Rect y)
{
    if(x.height < y.height)
    {
        return true;
    }
    return  false;
}

bool CharSegment_V3::RectLeftCompare(cv::Rect x, cv::Rect y)
{
    if(x.x < y.x)
    {
        return true;
    }
    return  false;
}



bool CharSegment_V3::CharInfoLeftCompare(CharInfo x, CharInfo y)
{
    if(x.originalRect.x < y.originalRect.x)
    {
        return true;
    }
    return  false;
}


