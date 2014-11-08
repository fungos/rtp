#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include "ialgorithm.h"

namespace Ui {
class MainWindow;
}

class ImageSelectLabel;
class QFile;
class QImage;
class QComboBox;
class IAlgorithm;
class QLabel;
class QPushButton;
class QLineEdit;

struct ImageHistory
{
	QImage mImage;
	QString sName;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

		QImage *openImage();

	public slots:
		void onInputClick();
		void onPaletteClick();
		void onProcessClick();
		void onUpdate();
		void onIndexChanged(int);
		void onSelectAChanged(int);
		void onSelectBChanged(int);
		void onIterationsChanged(const QString &);
		void onFinished(QImage *result);
		void onStep();
		void onCompareImageASave();
		void onCompareImageBSave();

	private:
		void setImageToLabel(QLabel *label, QImage *img);
		void enableProcess();

		Ui::MainWindow *ui;

		ImageSelectLabel *pInputLabel;
		ImageSelectLabel *pPaletteLabel;
		ImageSelectLabel *pResultLabel;
		ImageSelectLabel *pCompareImageA;
		ImageSelectLabel *pCompareImageB;

		QComboBox *pAlgorithmSelect;
		QComboBox *pDistanceSelect;
		QComboBox *pCompareSelectA;
		QComboBox *pCompareSelectB;
		QPushButton *pProcessButton;
		QLineEdit *pIterations;

		QImage *pInput;
		QImage *pPalette;

		QList<ImageHistory> vResults;
		QList<DistanceFunction> vFuncs;
		QList<IAlgorithm *> vAlgos;
		IAlgorithm *pAlgo;
		DistanceFunction pFunction;
		QElapsedTimer mTimer;
		QString sResultName;

		int iMaxSteps;
		int iCurSteps;

		bool bUpdate;
		bool bCanStep;
};

#endif // MAINWINDOW_H
