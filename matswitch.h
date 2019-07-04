#ifndef MATSWITCH_H
#define MATSWITCH_H

#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>


class MatSwitch
{
public:
    MatSwitch();
    static QImage Mat2QImage(cv::Mat mat, QImage::Format format = QImage::Format_RGB888);
    static QPixmap Mat2QPixmap(cv::Mat mat, QImage::Format format = QImage::Format_RGB888);
};

#endif // MATSWITCH_H
