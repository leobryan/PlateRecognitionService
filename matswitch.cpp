#include "matswitch.h"

MatSwitch::MatSwitch()
{

}

QImage MatSwitch::Mat2QImage(cv::Mat mat, QImage::Format format)
{
    cv::Mat temp = mat.clone();
    if(mat.channels() == 3)
        cv::cvtColor(temp,temp,cv::COLOR_BGR2RGB);

    QImage image = QImage(static_cast<uchar *>(temp.data), temp.cols, temp.rows, temp.cols * temp.channels(), format).copy();
    temp.release();

    return image;
}

QPixmap MatSwitch::Mat2QPixmap(cv::Mat mat, QImage::Format format)
{
    cv::Mat temp = mat.clone();
    if(mat.channels() == 3)
        cv::cvtColor(temp,temp,cv::COLOR_BGR2RGB);

    QImage image = QImage(static_cast<uchar *>(temp.data), temp.cols, temp.rows, temp.cols * temp.channels(), format).copy();
    QPixmap pixmap = QPixmap::fromImage(image).copy();
    temp.release();

    return pixmap;
}
