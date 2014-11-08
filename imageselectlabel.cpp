#include "imageselectlabel.h"

ImageSelectLabel::ImageSelectLabel(QWidget *parent)
	: QLabel(parent)
{
}

void ImageSelectLabel::mousePressEvent(QMouseEvent *)
{
	emit clicked();
}
