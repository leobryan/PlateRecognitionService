#include "platechar_svm.h"
#include "QDebug"

bool PlateChar_SVM::isReady = false;
cv::Size PlateChar_SVM::HOGWinSize = cv::Size(16, 32);
cv::Size PlateChar_SVM::HOGBlockSize = cv::Size(16, 16);
cv::Size PlateChar_SVM::HOGBlockStride = cv::Size(8,8);
cv::Size PlateChar_SVM::HOGCellSize = cv::Size(8, 8);
int PlateChar_SVM::HOGNBits = 9;
cv::Ptr<cv::ml::SVM> PlateChar_SVM::svm;

PlateChar_SVM::PlateChar_SVM()
{

}

std::vector<float> PlateChar_SVM::ComputeHogDescriptors(cv::Mat image)
{
    cv::Mat matToHog;
    cv::resize(image,matToHog,HOGWinSize);
    cv::HOGDescriptor hog = cv::HOGDescriptor(HOGWinSize,
                                              HOGBlockSize,
                                              HOGBlockStride,
                                              HOGCellSize,
                                              HOGNBits);
    std::vector<float> res;
    hog.compute(matToHog,res,cv::Size(1,1),cv::Size(0,0));//?
    return res;
}

bool PlateChar_SVM::Train(cv::Mat samples, cv::Mat responses)
{
    svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::Types::C_SVC);
    svm->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::Type::MAX_ITER,10000,1e-10));
    isReady = true;
    return svm->train(samples,cv::ml::SampleTypes::ROW_SAMPLE,responses);
}

void PlateChar_SVM::Save(QString fileName)
{
    if((isReady == false)||(svm == nullptr))
        return;
    std::string fileNames = fileName.toLocal8Bit().toStdString();
    svm->save(fileNames);
}

void PlateChar_SVM::Load(QString fileName)
{
    std::string fileNames = fileName.toLocal8Bit().toStdString();
    try {
        svm = cv::ml::SVM::load(fileNames);//?
        isReady = true;
    } catch (std::exception) {
        // throw new std::exception("字符识别库加载异常，请检查存放路径");
        qDebug()<< "字符识别库加载异常，请检查存放路径";
    }
}

bool PlateChar_SVM::isCorrectTrainningDirectory(QString path)
{
    bool isCorrect = true;
    for (int i = 0;i < PlateCharString.length();i++) {
        QString plateChar = PlateCharString[i];
        QString charDirectory = path + "/" + plateChar;
        isCorrect = QDir(charDirectory).exists();
        if(isCorrect == false) break;
    }
    return isCorrect;
}

PlateChar PlateChar_SVM::Test(cv::Mat matTest)
{
    if((isReady == false)||(svm == nullptr))
    {
        // throw new std::exception("训练数据为空，请重新训练字符或加载数据");
        qDebug()<< "训练数据为空，请重新训练字符或加载数据";
    }
    PlateChar result = PlateChar::非字符;
    std::vector<float> descriptor = ComputeHogDescriptors(matTest);
    cv::Mat testDescriptor = cv::Mat::zeros(1,descriptor.size(),CV_32FC1);
    for (int i = 0;i < descriptor.size();i++) {
        testDescriptor.at<float>(0,i) = descriptor[i];
    }
    float predict = svm->predict(testDescriptor);
    result = (PlateChar)((int)predict);
    return result;
}

PlateChar PlateChar_SVM::Test(QString fileName)
{
    std::string fileNames = fileName.toLocal8Bit().toStdString();
    cv::Mat matTest = cv::imread(fileNames,cv::ImreadModes::IMREAD_GRAYSCALE);
    return Test(matTest);
}

void PlateChar_SVM::SaveCharSample(CharInfo charInfo, QString libPath)
{
    QTime now = QTime::currentTime();
    QString time = now.toString(Qt::DateFormat::SystemLocaleLongDate) + rand()%100000;
    QString shortFileNameNoExt = charInfo.plateLocateMethod + QString("_") + charInfo.charSplitMethod + QString("_") + time;
    SaveCharSample(charInfo.originalMat,charInfo.plateChar,libPath,shortFileNameNoExt);
}

void PlateChar_SVM::SaveCharSample(cv::Mat charMat, PlateChar plateChar, QString libPath, QString shortFileNameNoExt)
{
    QString fileName = libPath + QString("/chars/") + plateChar + QString("/") + shortFileNameNoExt + QString(".jpg");
    cv::resize(charMat,charMat,HOGWinSize);
    std::string fileNames = fileName.toLocal8Bit().toStdString();
    cv::imwrite(fileNames,charMat);
}

void PlateChar_SVM::SaveCharSample(cv::Mat charMat, PlateChar plateChar, QString libPath)
{
    CharInfo charInfo = CharInfo();
    charInfo.originalMat = charMat;
    charInfo.plateChar = plateChar;
    SaveCharSample(charInfo,libPath);
}

bool PlateChar_SVM::PrepareCharTrainningDirectory(QString path)
{
    bool success = true;
    QDir dir;
    try {
        success = QDir(path).exists();
        QString charsDirectory = path + QString("/chars");
        if (QDir(charsDirectory).exists() == false)
        {
            dir.mkpath(charsDirectory);
        }
        for (int i = 0;i < PlateCharString.length(); i++) {
            QString plateChar = PlateCharString[i];
            QString plateCharDirectory = charsDirectory + QString("/") + plateChar;
            if(QDir(plateCharDirectory).exists() == false)
            {
                dir.mkpath(plateCharDirectory);
            }
        }
    } catch (std::exception) {
        success = false;
    } catch (std::ios_base::failure) {
        success = false;
    }
    return success;
}
