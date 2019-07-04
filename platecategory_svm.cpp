#include "platecategory_svm.h"
#include "QDebug"

bool PlateCategory_SVM::isReady = false;
cv::Size PlateCategory_SVM::HOGWinSize = cv::Size(96, 32);
cv::Size PlateCategory_SVM::HOGBlockSize = cv::Size(16, 16);
cv::Size PlateCategory_SVM::HOGBlockStride = cv::Size(8,8);
cv::Size PlateCategory_SVM::HOGCellSize = cv::Size(8, 8);
int PlateCategory_SVM::HOGNBits = 9;
cv::Ptr<cv::ml::SVM> PlateCategory_SVM::svm = nullptr;

PlateCategory_SVM::PlateCategory_SVM()
{

}

void PlateCategory_SVM::SavePlateSample(PlateInfo plateInfo, QString fileName)
{
    std::string str = fileName.toLocal8Bit().toStdString();
    cv::imwrite(str, plateInfo.originalMat);
}

void PlateCategory_SVM::SavePlateSample(cv::Mat matPlate, PlateCategory plateCategory, QString libPath)
{
    QTime now = QTime::currentTime();
    QString name = now.toString() + QString::number(random(100000));
    QString fileName = libPath + "\\plate\\" + PlateCategoryString[plateCategory] + "\\" + name + ".jpg";

    std::string str = fileName.toLocal8Bit().toStdString();
    cv::imwrite(str, matPlate);
}

void PlateCategory_SVM::SavePlateSample(cv::Mat matPlate, PlateCategory plateCategory, QString libPath, QString shortFileNameNoExt)
{
    QString fileName = libPath + "\\plate\\" + plateCategory + "\\" + shortFileNameNoExt;

    std::string str = fileName.toLocal8Bit().toStdString();
    cv::imwrite(str, matPlate);
}

std::vector<float> PlateCategory_SVM::ComputeHogDescriptors(cv::Mat image)
{
    cv::Mat matToHog;
    cv::resize(image, matToHog, HOGWinSize);

    cv::HOGDescriptor *hog = new cv::HOGDescriptor(HOGWinSize, HOGBlockSize, HOGBlockStride, HOGCellSize, HOGNBits);

    std::vector<float> result;
    hog->compute(matToHog, result, HOGBlockStride, cv::Size(0,0));

    return result;
}

bool PlateCategory_SVM::Train(cv::Mat samples, cv::Mat responses)
{
    svm = cv::ml::SVM::create();//?
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::Type::MAX_ITER, 10000, 1e-10));

    isReady = true;

    return svm->train(samples, cv::ml::SampleTypes::ROW_SAMPLE, responses);
}

void PlateCategory_SVM::Save(QString fileName)
{
    if(isReady == false || svm == nullptr)
        return;

    std::string str = fileName.toLocal8Bit().toStdString();
    svm->save(str);
}

void PlateCategory_SVM::Load(QString fileName)
{
    try {
        std::string str = fileName.toLocal8Bit().toStdString();
        svm = cv::ml::SVM::load(str);//?
        isReady = true;
    } catch (std::exception) {
        // throw new std::exception("charlib error,try again");
        qDebug()<< "charlib error,try again";
    }
}

bool PlateCategory_SVM::isCorrectTrainningDirectory(QString path)
{
    bool isCorrect = true;

    for(int index = 0; index < PlateCategoryString.size(); index++)
    {
        QString plateCategoryName = PlateCategoryString[index];

        QString platePath = path + "\\" + plateCategoryName;

        if(QDir(platePath).exists() == false)
        {
            isCorrect = false;
            break;
        }
    }

    return isCorrect;
}

PlateCategory PlateCategory_SVM::Test(cv::Mat matTest)
{
    try {
        if(isReady == false || svm == nullptr)
        {
            // throw new std::exception("data is empty,try again");
            qDebug()<< "data is empty,try again";
        }

        PlateCategory result = PlateCategory::非车牌;

        if(isReady == false || svm == nullptr) return result;

        std::vector<float> descriptor = ComputeHogDescriptors(matTest);
        cv::Mat testDescriptor = cv::Mat::zeros(1, descriptor.size(), CV_32FC1);
        for(int index = 0; index < descriptor.size(); index++)
        {
            testDescriptor.at<float>(0, index) = descriptor.at(index);
        }

        float predict = svm->predict(testDescriptor);

        result = (PlateCategory)(int)predict;

        return result;
    } catch (std::exception ex) {
        throw ex;
    }
}

PlateCategory PlateCategory_SVM::Test(QString fileName)
{
    std::string str = fileName.toLocal8Bit().toStdString();
    cv::Mat matTest = cv::imread(str, cv::ImreadModes::IMREAD_GRAYSCALE);

    return Test(matTest);
}

bool PlateCategory_SVM::PreparePlateTrainningDirectory(QString path)
{
    bool success = true;
    QDir dir;
    try {
        QString platesDiretory = path + "\\plates";
        if(dir.exists(platesDiretory) == false)
        {
            dir.mkpath(platesDiretory);
        }
        for(int index_plateCategory = 0; index_plateCategory < PlateCategoryString.size(); index_plateCategory++)
        {
            QString plateCategoryDirectory = platesDiretory + "\\" + PlateCategoryString[index_plateCategory];
            if(dir.exists(plateCategoryDirectory) == false)
            {
                dir.mkpath(plateCategoryDirectory);
            }
        }

    } catch (std::exception) {
        success = false;
    }

    return success;
}
