#ifndef PLATECATEGORY_H
#define PLATECATEGORY_H

#pragma execution_character_set("utf-8")

#include <QString>
#include <opencv2/opencv.hpp>
#include <qtextcodec.h>
#include <QDebug>

enum PlateCategory
{
    非车牌 = 0,
    普通车牌 = 1,
    普通车牌_两行 = 2,
    内集车牌 = 3,
    澳门车牌_内 = 4,
    澳门车牌 = 5,
    澳门车牌_两行=6,
    香港车牌_内 = 7,
    香港车牌 = 8,
    香港车牌_两行 = 9,
    警牌 = 10,
    军牌 = 11,
    军牌_两行 = 12,
    新能源 = 13,
    未识别 = 14
};

const QList<QString> PlateCategoryString =
{
    "非车牌",
    "普通车牌",
    "普通车牌_两行",
    "內集车牌",
    "澳门车牌_内",
    "澳门车牌",
    "澳门车牌_两行",
    "香港车牌_内",
    "香港车牌",
    "香港车牌_两行",
    "警牌",
    "军牌",
    "军牌_两行",
    "新能源",
    "未识别"
};

enum PlateColor
{
    蓝牌 = 0,
    黄牌 = 1,
    白牌 = 2,
    黑牌 = 4,
    绿牌 = 5,
    未知 = 6
};

const QList<QString> PlateColorString =
{
    "蓝牌",
    "黄牌",
    "白牌",
    "黑牌",
    "绿牌",
    "未知",
    "未识别"
};

enum PlateChar
{
    非字符 = 0,
    穗 = 1,
    A = 2,
    B = 3,
    C = 4,
    D = 5,
    E = 6,
    F = 7,
    G = 8,
    H = 9,
    I = 10,
    J = 11,
    K = 12,
    L = 13,
    M = 14,
    N = 15,
    O = 16,
    P = 17,
    Q = 18,
    R = 19,
    S = 20,
    T = 21,
    U = 22,
    V = 23,
    W = 24,
    X = 25,
    Y = 26,
    Z = 27,
    _0 = 28,
    _1 = 29,
    _2 = 30,
    _3 = 31,
    _4 = 32,
    _5 = 33,
    _6 = 34,
    _7 = 35,
    _8 = 36,
    _9 = 37,
    点 = 38,
    京 = 39,
    津 = 40,
    沪 = 41,
    渝 = 42,
    蒙 = 43,
    新 = 44,
    藏 = 45,
    宁 = 46,
    桂 = 47,
    港 = 48,
    澳 = 49,
    黑 = 50,
    吉 = 51,
    辽 = 52,
    晋 = 53,
    冀 = 54,
    青 = 55,
    鲁 = 56,
    豫 = 57,
    苏 = 58,
    皖 = 59,
    浙 = 60,
    闽 = 61,
    赣 = 62,
    湘 = 63,
    鄂 = 64,
    粤 = 65,
    琼 = 66,
    甘 = 67,
    陕 = 68,
    贵 = 69,
    云 = 70,
    川 = 71,
    警 = 72,
    未识别字符 = 73
};

const QList<QString> PlateCharString
{
    "非字符",
    "穗",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "_0",
    "_1",
    "_2",
    "_3",
    "_4",
    "_5",
    "_6",
    "_7",
    "_8",
    "_9",
    "点",
    "京",
    "津",
    "沪",
    "渝",
    "蒙",
    "新",
    "藏",
    "宁",
    "桂",
    "港",
    "澳",
    "黑",
    "吉",
    "辽",
    "晋",
    "冀",
    "青",
    "鲁",
    "豫",
    "苏",
    "皖",
    "浙",
    "闽",
    "赣",
    "湘",
    "鄂",
    "粤",
    "琼",
    "甘",
    "陕",
    "贵",
    "云",
    "川",
    "警",
    "未识别字符"
};

enum PlateLocateMethod
{
    未知方法 = 0,
    颜色法_蓝黑 = 1,
    颜色法_黄白 = 2,
    颜色法 = 3,
    Sobel法 = 4,
    MSER法 = 5
};

const QList<QString> PlateLocateMethodString =
{
    "未知方法",
    "颜色法_蓝黑",
    "颜色法_黄白",
    "颜色法",
    "Sobel法",
    "MSER法"
};

enum CharSplitMethod
{
    未知字符方法 = 0,
    原图 = 1,
    伽马 = 2,
    指数 = 3,
    对数 = 4
};

const QList<QString> CharSplitMethodString =
{
    "未知字符方法",
    "原图",
    "伽马",
    "指数",
    "对数"
};

struct CharImage
{
    QString fileName;
    QString name;

    PlateChar plateChar;

    cv::Size matSize;

    CharImage(QString fileName_, QString name_, PlateChar plateChar_, cv::Size matSize_)
    {
        this->fileName = fileName_;
        this->name = name_;
        this->plateChar = plateChar_;
        this->matSize = matSize_;
    }
};

struct PlateImage
{
    QString fileName;
    QString name;

    PlateCategory plateCategory;

    cv::Size matSize;

    PlateImage(QString fileName_, QString name_, PlateCategory plateCategory_, cv::Size matSize_)
    {
        this->fileName = fileName_;
        this->name = name_;
        this->plateCategory = plateCategory_;
        this->matSize = matSize_;
    }
};

class CharInfo
{
public:
    PlateChar plateChar;
    cv::Mat originalMat;
    cv::Rect originalRect;
    PlateLocateMethod plateLocateMethod;
    CharSplitMethod charSplitMethod;

    QString info()
    {
        return "字符:" + PlateCharString[plateChar] + "\r\n"
                + "宽:" + QString::number(this->originalRect.width) + "\r\n"
                + "高:" + QString::number(this->originalRect.height) + "\r\n"
                + "宽高比:" + QString("%2").arg((float)this->originalRect.width/this->originalRect.height) + "\r\n"
                + "左:" + QString::number(this->originalRect.x) + "\r\n"
                + "右:" + QString::number(this->originalRect.x + this->originalRect.width) + "\r\n"
                + "上:" + QString::number(this->originalRect.y) + "\r\n"
                + "下:" + QString::number(this->originalRect.y + this->originalRect.height) + "\r\n"
                + "车牌定位:" + PlateLocateMethodString[this->plateLocateMethod] + "\r\n"
                + "字符切分:" + CharSplitMethodString[this->charSplitMethod] + "\r\n";
    }

    CharInfo(){}

    CharInfo(PlateChar plateChar_, cv::Mat originalMat_, cv::Rect originalRect_, PlateLocateMethod plateLocatMethod_, CharSplitMethod charSplitMethod_)
    {
        this->plateChar = plateChar_;
        this->originalMat = originalMat_;
        this->originalRect = originalRect_;
        this->plateLocateMethod = plateLocatMethod_;
        this->charSplitMethod = charSplitMethod_;
    }

    QString ToString()
    {
        QString title = PlateCharString[this->plateChar];
        return title.replace("_","");
    }
};

class PlateInfo
{
public:
    PlateCategory plateCategory;
    PlateColor plateColor = PlateColor::未知;

    cv::RotatedRect rotatedRect;
    cv::Rect originalRect;
    cv::Mat originalMat;

    PlateLocateMethod plateLocateMethod;

    QList<CharInfo> charInfos;

    QString Info()
    {
        return "类型:" + PlateCategoryString[this->plateCategory] + "\r\n"
                + "颜色:" + PlateColorString[this->plateColor] + "\r\n"
                + "字符:" + this->ToString() + "\r\n"
                + "宽:" + QString::number(this->originalRect.width) + "\r\n"
                + "高:" + QString::number(this->originalRect.height) + "\r\n"
                + "宽高比:" + QString("%2").arg((float)this->originalRect.width/this->originalRect.height) + "\r\n"
                + "左:" + QString::number(this->originalRect.x) + "\r\n"
                + "右:" + QString::number(this->originalRect.x + this->originalRect.width) + "\r\n"
                + "上:" + QString::number(this->originalRect.y) + "\r\n"
                + "下:" + QString::number(this->originalRect.y + this->originalRect.height) + "\r\n"
                + "车牌定位:" + PlateLocateMethodString[this->plateLocateMethod] + "\r\n";
    }

    PlateInfo(){}

    PlateInfo(PlateCategory plateCategory_, cv::Rect originalRect_, cv::Mat originalMat_, QList<CharInfo> charInfos_, PlateLocateMethod plateLocateMethod_)
    {
        this->plateCategory = plateCategory_;
        this->originalRect = originalRect_;
        this->originalMat = originalMat_;
        this->charInfos = charInfos_;
        this->plateLocateMethod = plateLocateMethod_;
    }

    QString ToString()
    {
        QString str = "";

        if(this->charInfos.isEmpty())
        {
            return str;
        }

        for(CharInfo ci : this->charInfos)
        {
            str+=ci.ToString();
        }
        str.replace("非字符","");
        return str;
    }
};

#endif // PLATECATEGORY_H
