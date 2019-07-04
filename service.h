#ifndef SERVICE_H
#define SERVICE_H

#include "matswitch.h"
#include "platecategory.h"
#include "charsegment_v3.h"
#include "platelocator_v3.h"
#include "platerecognition_v3.h"


#include <QDialog>
#include <QListWidgetItem>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLabel>
#include <QList>
#include <QFont>

namespace Ui {
class Service;
}

class Service : public QDialog
{
    Q_OBJECT

public:
    QString pathSelected;
    PlateInfo plateInfo;
    explicit Service(QWidget *parent = nullptr);
    ~Service();
    void showCharSegment(PlateInfo pi);
    void showPlateSegmentProcedure(PlateInfo pi);
    void showColorProcedure(cv::Mat mat);
    void showSobelProcedure(cv::Mat mat);


private slots:
    void on_nameList_itemClicked(QListWidgetItem *item);
    QLabel* generateImageLabel(cv::Mat mat,QImage::Format format,int wid);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Service *ui;
};

#endif // SERVICE_H
