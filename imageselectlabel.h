#ifndef IMAGESELECTLABEL_H
#define IMAGESELECTLABEL_H

#include <QLabel>

class ImageSelectLabel : public QLabel
{
	Q_OBJECT
	public:
		explicit ImageSelectLabel(QWidget *parent = 0);

	signals:
		void clicked();

	public slots:

	protected:
		void mousePressEvent(QMouseEvent *event);
};

#endif // IMAGESELECTLABEL_H
