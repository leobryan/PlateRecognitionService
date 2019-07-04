#include "service.h"
#include "ui_service.h"






Service::Service(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Service)
{
    ui->setupUi(this);
    this->setWindowTitle("车牌自动识别工具");

}

Service::~Service()
{
    delete ui;
}

void Service::showCharSegment(PlateInfo pi)
{
    cv::Mat mat = pi.originalMat;

    QList<CharInfo> boul = CharSegment_V3::SplitPlateForAutoSample(mat);

    if(&mat == nullptr) return;
    QList<CharInfo> ciOri;
    QList<CharInfo> ciInd;
    QList<CharInfo> ciLog;
    QList<CharInfo> ciGam;

    PlateInfo piOri(pi.plateCategory,pi.originalRect,pi.originalMat,ciOri,pi.plateLocateMethod);
    PlateInfo piInd(pi.plateCategory,pi.originalRect,pi.originalMat,ciInd,pi.plateLocateMethod);
    PlateInfo piLog(pi.plateCategory,pi.originalRect,pi.originalMat,ciLog,pi.plateLocateMethod);
    PlateInfo piGam(pi.plateCategory,pi.originalRect,pi.originalMat,ciGam,pi.plateLocateMethod);
    piOri.plateColor = CharSegment_V3::pc;
    piInd.plateColor = CharSegment_V3::pc;
    piLog.plateColor = CharSegment_V3::pc;
    piGam.plateColor = CharSegment_V3::pc;
    for(CharInfo ci : boul)
    {
        if(ci.charSplitMethod == CharSplitMethod::原图)
        piOri.charInfos.append(ci);
        if(ci.charSplitMethod == CharSplitMethod::指数)
        piInd.charInfos.append(ci);
        if(ci.charSplitMethod == CharSplitMethod::对数)
        piLog.charInfos.append(ci);
        if(ci.charSplitMethod == CharSplitMethod::伽马)
        piGam.charInfos.append(ci);
    }

    piOri = PlateRecognition_V3::CheckLeftAndRightToRemove(piOri);
    piLog = PlateRecognition_V3::CheckLeftAndRightToRemove(piLog);
    piInd = PlateRecognition_V3::CheckLeftAndRightToRemove(piInd);
    piGam = PlateRecognition_V3::CheckLeftAndRightToRemove(piGam);
    qDebug()<< "Plate Ori Info:"<<piOri.Info();
    qDebug()<< "Plate Log Info:"<<piLog.Info();
    qDebug()<< "Plate Ind Info:"<<piInd.Info();
    qDebug()<< "Plate Gam Info:"<<piGam.Info();
    if (boul.isEmpty()) return;
    for(CharInfo charInfo: piOri.charInfos)
    {
        QPixmap pixmap = MatSwitch::Mat2QPixmap(charInfo.originalMat);
        QListWidgetItem *plateitem = new QListWidgetItem(QIcon(pixmap),charInfo.info());
        QListWidgetItem *plateimg = new QListWidgetItem(QIcon(pixmap),"");
        ui->segmentChars->addItem(plateitem);
        ui->orig->addItem(plateimg);
    }
    for(CharInfo charInfo: piLog.charInfos)
    {
        QPixmap pixmap = MatSwitch::Mat2QPixmap(charInfo.originalMat);
        QListWidgetItem *plateitem = new QListWidgetItem(QIcon(pixmap),charInfo.info());
        QListWidgetItem *plateimg = new QListWidgetItem(QIcon(pixmap),"");
        ui->segmentChars->addItem(plateitem);
        ui->log->addItem(plateimg);
    }
    for(CharInfo charInfo: piInd.charInfos)
    {
        QPixmap pixmap = MatSwitch::Mat2QPixmap(charInfo.originalMat);
        QListWidgetItem *plateitem = new QListWidgetItem(QIcon(pixmap),charInfo.info());
        QListWidgetItem *plateimg = new QListWidgetItem(QIcon(pixmap),"");
        ui->segmentChars->addItem(plateitem);
        ui->index->addItem(plateimg);
    }
    for(CharInfo charInfo: piGam.charInfos)
    {
        QPixmap pixmap = MatSwitch::Mat2QPixmap(charInfo.originalMat);
        QListWidgetItem *plateitem = new QListWidgetItem(QIcon(pixmap),charInfo.info());
        QListWidgetItem *plateimg = new QListWidgetItem(QIcon(pixmap),"");
        ui->segmentChars->addItem(plateitem);
        ui->gamma->addItem(plateimg);
    }
    QString oriStr = piOri.ToString();
    QString logStr = piLog.ToString();
    QString indStr = piInd.ToString();
    QString gamStr = piGam.ToString();
    QList<QString> exam;
    QString res;
    int len = 0;
    exam.append(oriStr);
    exam.append(logStr);
    exam.append(indStr);
    exam.append(gamStr);
    for(QString each : exam)
    {
        if(each.length()>8 || each.length()<6)
            exam.removeOne(each);
    }
    if(exam.size()==1)
    {
        res = exam[0];
    }
    else if(exam.size()==2)
    {
        if(QString::compare(exam[0],exam[1])==0)
            res = exam[0];
        else {
            for(QString each : exam)
            {
                if(each.length()>len)
                    len = each.length();
            }
            for(QString each : exam)
            {
                if(each.length()==len)
                    res = each;
            }
        }
    }
    else {
        for(QString each : exam)
        {
            if(each.length()>len)
                len = each.length();
        }
        for(QString each : exam)
        {
            if(each.length()==len)
                res = each;
        }
    }
    QFont font;
    font.setPixelSize(60);
    ui->label_7->setFont(font);

    ui->label_7->setText(res);
    if(QString::compare(res,oriStr)==0)
    {
        ui->textBrowser->setText(piOri.Info());
        return;
    }
    if(QString::compare(res,indStr)==0)
    {
        ui->textBrowser->setText(piInd.Info());
        return;
    }
    if(QString::compare(res,logStr)==0)
    {
        ui->textBrowser->setText(piLog.Info());
        return;
    }
    if(QString::compare(res,gamStr)==0)
    {
        ui->textBrowser->setText(piGam.Info());
        return;
    }
    else {
        ui->label_7->setText(tr("未识别"));
        ui->textBrowser->clear();
        return;
    }
}

void Service::showColorProcedure(cv::Mat mat)
{
    int wid = ui->tabWidget_4->width();
    QWidget* tabHsv = this->generateImageLabel(mat,QImage::Format_RGB888,wid);
    this->ui->tabWidget_4->addTab(tabHsv,tr("原图"));
    cv::Mat hsv;
    cv::cvtColor(mat,hsv,cv::COLOR_BGR2HSV);
    cv::Mat* hsvSplits = new cv::Mat[3];
    cv::split(hsv,hsvSplits);
    cv::equalizeHist(hsvSplits[2],hsvSplits[2]);
    cv::Mat hsvEqualizeHist = cv::Mat();
    cv::merge(hsvSplits,3,hsvEqualizeHist);
    cv::Mat equalize;
    cv::cvtColor(hsvEqualizeHist,equalize,cv::COLOR_HSV2BGR);
    QWidget* tabHsv2 = this->generateImageLabel(equalize,QImage::Format_RGB888,wid);
    this->ui->tabWidget_4->addTab(tabHsv2,tr("亮度正则化图"));
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
    cv::threshold(add,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU|cv::ThresholdTypes::THRESH_BINARY);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(17,3));
    QWidget* th = this->generateImageLabel(threshold,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_4->addTab(th,tr("门槛图"));
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    QWidget* th_E = this->generateImageLabel(threshold_Erode,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_4->addTab(th_E,tr("门槛图去噪点"));

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(threshold_Erode,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
    cv::Mat matContours = mat.clone();
    cv::drawContours(matContours,contours,-1,cv::Scalar(0,0,255),2);
    QWidget* tabContours = this->generateImageLabel(matContours,QImage::Format_RGB888,wid);
    this->ui->tabWidget_4->addTab(tabContours,tr("连通域"));
    //求轮廓的最⼩小外接矩形
    cv::Mat matRects = mat.clone();
    std::vector<cv::Rect> rects;
    for (int index = 0; index < contours.size();index++)
    {
        cv::Rect rect = cv::boundingRect(contours[index]);
        rects.push_back(rect);
        cv::rectangle(matRects,rect,cv::Scalar(72,35,0),4);
    }
    QWidget* tabRects = this->generateImageLabel(matRects,QImage::Format_RGB888,wid);
    this->ui->tabWidget_4->addTab(tabRects,tr("待确认区域"));

}

void Service::showSobelProcedure(cv::Mat mat)
{
    cv::Mat gray;
    cv::Mat blur;
    int wid = ui->tabWidget_5->width();
    cv::GaussianBlur(mat,blur,cv::Size(5,5),0,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::cvtColor(blur,gray,cv::COLOR_BGR2GRAY);
    QWidget* tabOrig = this->generateImageLabel(mat,QImage::Format_RGB888,wid);
    this->ui->tabWidget_5->addTab(tabOrig,tr("原图"));
    QWidget* tabGauss = this->generateImageLabel(blur,QImage::Format_RGB888,wid);
    this->ui->tabWidget_5->addTab(tabGauss,tr("高斯模糊图"));
    QWidget* tabGray = this->generateImageLabel(gray,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_5->addTab(tabGray,tr("灰度化"));
    cv::Mat grad_x;
    cv::Sobel(gray,grad_x,CV_16S,1,0,3,1,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_x;
    cv::convertScaleAbs(grad_x,abs_grad_x);
    cv::Mat grad_y;
    cv::Sobel(gray,grad_y,CV_16S,1,0,3,1,0,cv::BorderTypes::BORDER_DEFAULT);
    cv::Mat abs_grad_y;
    cv::convertScaleAbs(grad_y,abs_grad_y);
    cv::Mat grad;
    cv::addWeighted(abs_grad_x,1,abs_grad_y,0,0,grad);
    QWidget* tabGrad = this->generateImageLabel(grad,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_5->addTab(tabGrad,tr("梯度图"));
    cv::Mat threshold;
    cv::threshold(grad,threshold,0,255,cv::ThresholdTypes::THRESH_OTSU | cv::ThresholdTypes::THRESH_BINARY);
    QWidget* th = this->generateImageLabel(threshold,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_5->addTab(th,tr("门槛图"));
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(17,3));
    cv::Mat threshold_Close;
    cv::morphologyEx(threshold,threshold_Close,cv::MorphTypes::MORPH_CLOSE,element);
    cv::Mat element_Erode = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::Mat threshold_Erode;
    cv::erode(threshold_Close,threshold_Erode,element_Erode);
    QWidget* th_E = this->generateImageLabel(threshold_Erode,QImage::Format_Grayscale8,wid);
    this->ui->tabWidget_5->addTab(th_E,tr("门槛图去噪点"));
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(threshold_Erode,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
    cv::Mat matContours = mat.clone();
    cv::drawContours(matContours,contours,-1,cv::Scalar(0,0,255),2);
    QWidget* tabContours = this->generateImageLabel(matContours,QImage::Format_RGB888,wid);
    this->ui->tabWidget_5->addTab(tabContours,tr("连通域"));
    //求轮廓的最⼩小外接矩形
    cv::Mat matRects = mat.clone();
    std::vector<cv::Rect> rects;
    for (int index = 0; index < contours.size();index++)
    {
        cv::Rect rect = cv::boundingRect(contours[index]);
        if(rect.width>rect.height*2 && rect.height>5){
            rects.push_back(rect);
            cv::rectangle(matRects,rect,cv::Scalar(72,35,0),4);
        }
    }
    QWidget* tabRects = this->generateImageLabel(matRects,QImage::Format_RGB888,wid);
    this->ui->tabWidget_5->addTab(tabRects,tr("待确认区域"));
}

void Service::showPlateSegmentProcedure(PlateInfo pi)
{
    cv::Mat mat = pi.originalMat;
//    QPixmap pixmap = MatSwitch::Mat2QPixmap(mat);

//    QWidget* tabGray = this->generateImageLabel(gray1,QImage::Format_Grayscale8);
//    this->ui->tabWidget->addTab(tabGray,tr("Gray"));
    if(&mat == nullptr) return;
    QPixmap orig = MatSwitch::Mat2QPixmap(mat);
    QPixmap pixmapfix1 = orig.scaled(ui->label_6->width(),ui->label_6->width()*orig.height()/orig.width());
    QPixmap pixmapfix2 = orig.scaled(ui->label_9->width(),ui->label_9->width()*orig.height()/orig.width());
    ui->label_6->setPixmap(pixmapfix1);
    ui->label_9->setPixmap(pixmapfix2);
    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888,ui->plate->width());
    this->ui->tabWidget->addTab(tabOriginal,tr("原图"));

    cv::Mat index;
    cv::Mat log;
    cv::Mat gamma;
    log = CharSegment_V3::LogTransform(mat);

    index = CharSegment_V3::IndexTransform(mat);

    gamma = CharSegment_V3::GammaTransform(mat);
    QWidget* tabLog = this->generateImageLabel(log,QImage::Format_RGB888,ui->plate->width());
    this->ui->tabWidget->addTab(tabLog,tr("对数变换"));
    QWidget* tabIndex = this->generateImageLabel(index,QImage::Format_RGB888,ui->plate->width());
    this->ui->tabWidget->addTab(tabIndex,tr("指数变换"));
    QWidget* tabGamma = this->generateImageLabel(gamma,QImage::Format_RGB888,ui->plate->width());
    this->ui->tabWidget->addTab(tabGamma,tr("伽马变换"));


    cv::Mat gray1;
    cv::cvtColor(mat,gray1,cv::COLOR_BGR2GRAY);
    QWidget* tabGray = this->generateImageLabel(gray1,QImage::Format_Grayscale8,ui->plate->width());
    this->ui->tabWidget->addTab(tabGray,tr("灰色"));

    cv::Mat gray2 = CharSegment_V3::ClearMaodingAndBorder(gray1,PlateColor::蓝牌);
    cv::Mat gray22 = CharSegment_V3::ClearMaodingAndBorder(gray1,PlateColor::蓝牌);

    QWidget* tabGray2 = this->generateImageLabel(gray2,QImage::Format_Grayscale8,ui->plate->width());
    this->ui->tabWidget->addTab(tabGray2,tr("蓝色图片去边界"));

    cv::Mat gray3 = CharSegment_V3::ClearMaodingAndBorder(gray1,PlateColor::黄牌);
    QWidget* tabGray3 = this->generateImageLabel(gray3,QImage::Format_Grayscale8,ui->plate->width());
    this->ui->tabWidget->addTab(tabGray3,tr("黄色图片去边界"));





}

void Service::on_nameList_itemClicked(QListWidgetItem *item)
{

    this->ui->tabWidget->clear();
    this->ui->segmentChars->clear();
    this->ui->orig->clear();
    this->ui->log->clear();
    this->ui->index->clear();
    this->ui->gamma->clear();
    this->ui->tabWidget_4->clear();
    this->ui->tabWidget_5->clear();
    this->ui->label_9->clear();
    this->ui->label_7->clear();
    this->ui->textBrowser->clearHistory();

    if(ui->checkBox_2->checkState() == Qt::CheckState::Unchecked)
    {
        QMessageBox::information(this,tr("提示"),tr("请先完成模型加载"),QMessageBox::Ok);
        return;
    }

    if(item == nullptr) return;

    QString imgFileName = item->text();
    imgFileName = pathSelected + "/" + imgFileName;
    // qDebug()<< imgFileName;
    cv::Mat mat = cv::imread(imgFileName.toLocal8Bit().toStdString());
    //对特别大的图片进行缩放
    int maxsize = 700;
    if (mat.cols > maxsize)
    {
        float rate = (float)mat.cols/maxsize;
        int height = mat.rows * rate;

        mat.resize((unsigned long)maxsize,height);
    }


    cv::Mat trl;
    QList<PlateInfo> pis = PlateLocator_V3::LocatePlatesForAutoSampleWithAllPara(mat,&trl);
    // showPlateMats(mat);
    showColorProcedure(mat);
    showSobelProcedure(mat);
    bool have = false;
    for(PlateInfo pi: pis)
    {
        if(pi.plateCategory!=PlateCategory::非车牌 && pi.plateCategory!=PlateCategory::未识别)
        {
            plateInfo = pi;
            have = true;
            showCharSegment(plateInfo);
            showPlateSegmentProcedure(plateInfo);
            break;
        }

    }
    if(!have)
    {
        QMessageBox::warning(this, tr("警告"),
                           tr("未发现车牌"),
                           QMessageBox::Yes);

    }

}

QLabel* Service::generateImageLabel(cv::Mat mat,QImage::Format format,int wid)
{
    QLabel* lblImage = new QLabel();
    QImage image = MatSwitch::Mat2QImage(mat,format);
    QPixmap pixmap = QPixmap::fromImage(image).copy();
    QPixmap pixmapfix = pixmap.scaled(wid,wid*pixmap.height()/pixmap.width());
    lblImage->setPixmap(pixmapfix);
    return lblImage;
}

void Service::on_pushButton_clicked()
{
    pathSelected = QFileDialog::getExistingDirectory(this,
                                                     tr("选择图片所在文件夹"),
                                                     (pathSelected.isEmpty())?tr("/Users/leobryan/Downloads/Pics"):pathSelected);
    if(pathSelected.isEmpty()) return;
    QDir *dir = new QDir(pathSelected);
    qDebug()<< pathSelected;
    QStringList nameFilter;
    nameFilter<< "*.jpg" << "*.png" << "*.bmp" ;
    QStringList imgFileNames = dir->entryList(nameFilter);
    // qDebug()<< imgFileNames;
    for (QString imgFileName : imgFileNames)
    {
        this->ui->nameList->addItem(imgFileName);
    }
}

void Service::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("请选择车牌定位模型文件"),tr("/Users/leobryan/Downloads/XMLs"),
                                                                   "Model Files(*.xml)");
    if(path.isEmpty()) return;

    PlateCategory_SVM::Load(path);

    if(PlateCategory_SVM::isReady == false)
    {
        QMessageBox::warning(this, tr("警告"),
                                   tr("无法成功加载车牌定位模型"),
                                   QMessageBox::Yes);
        return;
    }
    if (PlateChar_SVM::isReady == true)
        ui->checkBox_2->setCheckState(Qt::Checked);


}

void Service::on_pushButton_3_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("请选择字符识别模型文件"),tr("/Users/leobryan/Downloads/XMLs"),
                                                                   "Model Files(*.xml)");
    if(path.isEmpty()) return;

    PlateChar_SVM::Load(path);

    if(PlateChar_SVM::isReady == false)
    {
        QMessageBox::warning(this, tr("警告"),
                                   tr("无法成功加载字符识别模型"),
                                   QMessageBox::Yes);
        return;
    }
    if (PlateCategory_SVM::isReady == true)
        ui->checkBox_2->setCheckState(Qt::Checked);
}
