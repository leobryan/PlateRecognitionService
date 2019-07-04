#include "platerecognition_v3.h"

PlateRecognition_V3::PlateRecognition_V3()
{

}

// 函数1，根据一张图识别出所有包含车牌的车牌信息，调用了函数2
QList<PlateInfo> PlateRecognition_V3::Recognize(cv::Mat matSource)
{
    // 盛放确认为车牌的信息结果
    QList<PlateInfo> *result = new QList<PlateInfo>();
    // 盛放Locator定位出的初步判定为车牌的车牌信息结果
    QList<PlateInfo> plateInfosLocate = PlateLocator_V3::LocatePlates(matSource);
    // 从Locator中的结果选出确认为车牌的结果
    for (int index = 0;index< plateInfosLocate.count();index++)
    {
        PlateInfo plateInfo = plateInfosLocate[index];
        // 判定该切割是否是车牌，是则添加信息，否则设为空
        PlateInfo plateInfoOfHandled = PlateRecognition_V3::GetPlateInfoByMultiMethodAndMultiColor(plateInfo);
        // TODO
        if (&plateInfoOfHandled != nullptr)
        {
            // 确定为车牌，将识别出的车牌的类型添加进去
            plateInfoOfHandled.plateCategory = plateInfo.plateCategory;
            result->append(plateInfoOfHandled);
        }
    }
    // 所有判定是车牌的车牌信息
    return *result;
}

// 函数2，判断切割结果是否为车牌并确认颜色，调用了函数3，4
// 输入一个切割结果PI，若识别出车牌则将切割结果PI类中加入识别出的车牌颜色
// 识别不出则返回空值
PlateInfo PlateRecognition_V3::GetPlateInfoByMultiMethodAndMultiColor(PlateInfo plateInfo)
{

    PlateInfo *result = nullptr;
    // PI的原图片为空，无法识别 TODO
    if (&plateInfo.originalMat == nullptr)
    {
        return *result;
    }
    // 多种方法判断为蓝色车牌，函数3
    PlateInfo plateInfo_Blue = GetPlateInfoByMultiMethod(plateInfo,PlateColor::蓝牌);
    // 判定是不是是车牌，函数4
    if (JudgePlateRightful(plateInfo_Blue))
    {
        plateInfo_Blue.plateColor = PlateColor::蓝牌;
        return plateInfo_Blue;
    }
    // 多种方法判断该车牌为黄色车牌
    PlateInfo plateInfo_Yellow = GetPlateInfoByMultiMethod(plateInfo,PlateColor::黄牌);
    // 是车牌
    if (JudgePlateRightful(plateInfo_Yellow))
    {
        plateInfo_Blue.plateColor = PlateColor::黄牌;
        return plateInfo_Yellow;
    }
    // 不是车牌
    return *result;
}

// 函数3，检验该图是否为车牌
bool PlateRecognition_V3::JudgePlateRightful(PlateInfo plateInfo)
{
    // 车牌的字符信息为空 TODO
    if(plateInfo.charInfos.isEmpty() || plateInfo.charInfos.count()==0) return false;

    // 车牌的颜色未知
    if(plateInfo.plateColor == PlateColor::未知) return false;

    int charCount = 0;
    CharInfo ci;
    foreach(ci, plateInfo.charInfos)
    {
        if (ci.plateChar!=PlateChar::非字符) charCount++;
    }
    // 有多于5个字符时返回车牌
    return (charCount>=5);
}

// 函数4，检验该车牌是否为该颜色的车牌，是则加入其中,调用函数5
PlateInfo PlateRecognition_V3::GetPlateInfoByMultiMethod(PlateInfo plateInfo, PlateColor plateColor)
{
    PlateInfo plateInfoByOriginal = GetPlateInfo(plateInfo,plateColor,CharSplitMethod::原图);
    PlateInfo plateInfoByGamma = GetPlateInfo(plateInfo,plateColor,CharSplitMethod::伽马);
    PlateInfo plateInfoByIndex = GetPlateInfo(plateInfo,plateColor,CharSplitMethod::指数);
    PlateInfo plateInfoByLog = GetPlateInfo(plateInfo,plateColor,CharSplitMethod::对数);

    QList<PlateInfo> *plateInfos = new QList<PlateInfo>();
    // if (!plateInfoByOriginal.charInfos.isEmpty())
    // if (plateInfoByOriginal.charInfos != null)
    if(plateInfoByOriginal.charInfos.count()!=0 && &plateInfoByOriginal!=nullptr) plateInfos->append(plateInfoByOriginal);
    if(plateInfoByGamma.charInfos.count()!=0 && &plateInfoByGamma!=nullptr) plateInfos->append(plateInfoByGamma);
    if(plateInfoByIndex.charInfos.count()!=0 && &plateInfoByIndex!=nullptr) plateInfos->append(plateInfoByIndex);
    if(plateInfoByLog.charInfos.count()!=0 && &plateInfoByLog!=nullptr) plateInfos->append(plateInfoByLog);

    PlateInfo *pl = new PlateInfo();
    if (plateInfos->count()==0) return *pl;
    // incomplete TODO
    PlateInfo result = plateInfos->first();
    int max = plateInfos->length();
    for (int index = 1;index<max;index++) {
        PlateInfo compare = plateInfos->at(index);
        if(result.charInfos.count()<compare.charInfos.count()) result = compare;
    }

    return result;
}

// 函数5，调用分割字符分割，输入车牌的颜色以及现有的车牌信息，设定分割方法，将分割了字符的信息加入到车牌信息中返回，调用函数6、函数7
PlateInfo PlateRecognition_V3::GetPlateInfo(PlateInfo plateInfo, PlateColor plateColor, CharSplitMethod splitMethod)
{
    PlateInfo *result = new PlateInfo();
    result->plateCategory = plateInfo.plateCategory;
    result->originalMat = plateInfo.originalMat;
    result->originalRect = plateInfo.originalRect;
    result->plateLocateMethod = plateInfo.plateLocateMethod;
    result->plateColor = plateColor;

    QList<CharInfo> *charInfos = new QList <CharInfo>();

    switch (splitMethod) {

    case CharSplitMethod::伽马:
        *charInfos = CharSegment_V3::SplitPlateByGammaTransform(plateInfo.originalMat,plateColor);
        break;
    case CharSplitMethod::指数:
        *charInfos = CharSegment_V3::SplitPlateByIndexTransform(plateInfo.originalMat,plateColor);
        break;
    case CharSplitMethod::对数:
        *charInfos = CharSegment_V3::SplitPlateByLogTransform(plateInfo.originalMat,plateColor);
        break;
    case CharSplitMethod::原图:
        *charInfos = CharSegment_V3::SplitPlateByOriginal(plateInfo.originalMat,plateInfo.originalMat,plateColor);
        break;
    }

    for (int index = charInfos->count()-1;index>=0;index--) {
        CharInfo charInfo = charInfos->at(index);
        PlateChar plateChar = PlateChar_SVM::Test(charInfo.originalMat);
        if (plateChar == PlateChar::非字符) charInfos->removeAt(index);
        charInfo.plateChar = plateChar;
    }
    // ?TODO
    result->charInfos = *charInfos;

    CheckLeftAndRightToRemove(*result);
    CheckPlateColor(*result);
}

// 函数6，检查车牌的字符信息是否有效、是否符合车牌字符规矩
PlateInfo PlateRecognition_V3::CheckLeftAndRightToRemove(PlateInfo plateInfo)
{
    if (plateInfo.plateCategory == PlateCategory::非车牌) return plateInfo;
    if (&plateInfo.charInfos == nullptr) return plateInfo;
    if (plateInfo.charInfos.count()<4) return plateInfo;

    int charCount = plateInfo.charInfos.count();

    for (int index = charCount-2; index>1; index--)
    {
        CharInfo charInfo = plateInfo.charInfos[index];
        int charInfoValue = (int)charInfo.plateChar;
        // 去除汉字
        if (charInfoValue>=(int)PlateChar::京&&charInfoValue<=(int)PlateChar::警)
        {
            plateInfo.charInfos.removeAt(index);
        }
    }

    charCount = plateInfo.charInfos.count();
    CharInfo second = plateInfo.charInfos.at(1);
    int secondValue = (int) second.plateChar;
    CharInfo lastSecond = plateInfo.charInfos.at(charCount-2);
    int lastSecondValue = (int) lastSecond.plateChar;
    switch (plateInfo.plateCategory) {
    case PlateCategory::普通车牌:
    case PlateCategory::普通车牌_两行:
    case PlateCategory::澳门车牌_内:
    case PlateCategory::香港车牌_内:
    case PlateCategory::警牌:
        // 若第二个是符合规则的汉字则删除第一个
        if (secondValue >= (int)PlateChar::京 && secondValue <= (int)PlateChar::警)
        {
            plateInfo.charInfos.removeAt(0);

        }
        charCount = plateInfo.charInfos.count();
        if(charCount>7) plateInfo.charInfos.removeAt(charCount-1);
        break;
    case PlateCategory::澳门车牌:
        break;
    case PlateCategory::澳门车牌_两行:
        break;
    case PlateCategory::香港车牌:
        break;
    case PlateCategory::香港车牌_两行:
        break;
    case PlateCategory::军牌:
        break;
    case PlateCategory::军牌_两行:
        break;
    case PlateCategory::新能源:
        break;

    }
    charCount = plateInfo.charInfos.count();
    if(charCount<7) return plateInfo;

    CharInfo first = plateInfo.charInfos.at(0);
    int firstValue = (int) first.plateChar;
    second = plateInfo.charInfos.at(1);
    secondValue = (int) second.plateChar;
    CharInfo lastFirst = plateInfo.charInfos.at(charCount-1);
    int lastFirstValue = (int) lastFirst.plateChar;

    switch (plateInfo.plateCategory) {
    case PlateCategory::普通车牌:
        // 若最后一个是符合规则的汉字则删除
        if (lastFirstValue >= (int)PlateChar::京 && lastFirstValue <= (int)PlateChar::警)
        {
            plateInfo.charInfos.removeAt(charCount -1);

        }
        // 若第一个是字母数字则删除
        if (firstValue <= (int)PlateChar::点)
        {
            plateInfo.charInfos.removeAt(0);
        }
        // 若第二个是数字则删除
        if (secondValue >= (int)PlateChar::_0 && secondValue <= (int)(int)PlateChar::_9)
        {
            plateInfo.charInfos.removeAt(1);
        }
        break;
//    case PlateCategory::普通车牌_两行:
//    case PlateCategory::澳门车牌_内:
//    case PlateCategory::香港车牌_内:
//    case PlateCategory::警牌:
//        break;
//    case PlateCategory::澳门车牌:
//        break;
//    case PlateCategory::澳门车牌_两行:
//        break;
//    case PlateCategory::香港车牌:
//        break;
//    case PlateCategory::香港车牌_两行:
//        break;
//    case PlateCategory::军牌:
//        break;
//    case PlateCategory::军牌_两行:
//        break;
//    case PlateCategory::新能源:
//        break;

    }
    return plateInfo;
}

// 函数7，根据车牌类型确定车牌颜色，加入车牌信息中
void PlateRecognition_V3::CheckPlateColor(PlateInfo plateInfo)
{
    if(plateInfo.plateCategory == PlateCategory::非车牌) return;
    switch (plateInfo.plateCategory) {
    case PlateCategory::普通车牌:
        break;
    case PlateCategory::普通车牌_两行:
        plateInfo.plateColor = PlateColor::黄牌;
        break;
    case PlateCategory::澳门车牌_内:
    case PlateCategory::香港车牌_内:
        plateInfo.plateColor = PlateColor::黑牌;
    case PlateCategory::警牌:
    case PlateCategory::香港车牌:
    case PlateCategory::香港车牌_两行:
        plateInfo.plateColor = PlateColor::白牌;
        break;
    case PlateCategory::澳门车牌:
        break;
    case PlateCategory::澳门车牌_两行:
        break;
    case PlateCategory::军牌:
        break;
    case PlateCategory::军牌_两行:
        break;
    case PlateCategory::新能源:
        plateInfo.plateColor = PlateColor::绿牌;
        break;

    }

}


