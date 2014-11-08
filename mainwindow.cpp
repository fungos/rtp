#include <QtWidgets>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imageselectlabel.h"
#include "algorithmcopy.h"
#include "algorithmsort.h"
#include "algorithmswap.h"
#include "pixel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, pInputLabel(nullptr)
	, pPaletteLabel(nullptr)
	, pResultLabel(nullptr)
	, pCompareImageA(nullptr)
	, pCompareImageB(nullptr)
	, pAlgorithmSelect(nullptr)
	, pDistanceSelect(nullptr)
	, pCompareSelectA(nullptr)
	, pCompareSelectB(nullptr)
	, pProcessButton(nullptr)
	, pIterations(nullptr)
	, pInput(nullptr)
	, pPalette(nullptr)
	, pAlgo(nullptr)
	, iMaxSteps(0)
	, iCurSteps(0)
	, bUpdate(false)
	, bCanStep(false)
{
	ui->setupUi(this);

	auto layout = new QGridLayout;
	auto base = new QWidget;
	base->setLayout(layout);
	setCentralWidget(base);

	pInputLabel = new ImageSelectLabel;
	pInputLabel->setText(tr("Click to select input image"));

	pPaletteLabel = new ImageSelectLabel;
	pPaletteLabel->setText(tr("Click to select input palette"));

	auto processWidget = new QWidget;
	pProcessButton = new QPushButton(tr("Process"));
	pProcessButton->setDisabled(true);
	{
		auto tmpL = new QVBoxLayout;
		processWidget->setLayout(tmpL);
		pDistanceSelect = new QComboBox;
		pAlgorithmSelect = new QComboBox;
		pIterations = new QLineEdit("1000");
		pIterations->setValidator(new QIntValidator(0, 10000, this));
		tmpL->addStretch(1);
		tmpL->addWidget(new QLabel(tr("Distance:")));
		tmpL->addWidget(pDistanceSelect);
		tmpL->addWidget(new QLabel(tr("Technique:")));
		tmpL->addWidget(pAlgorithmSelect);
		tmpL->addWidget(new QLabel(tr("Max Iterations:")));
		tmpL->addWidget(pIterations);
		tmpL->addWidget(pProcessButton);
		tmpL->addStretch(1);
	}

	pResultLabel = new ImageSelectLabel;
	pResultLabel->setText(tr("<Result>"));

	auto compareWidgetA = new QWidget;
	{
		auto tmpL = new QVBoxLayout;
		compareWidgetA->setLayout(tmpL);
		pCompareImageA = new ImageSelectLabel;
		pCompareImageA->setText(tr("Select Image"));
		pCompareSelectA = new QComboBox;
		tmpL->addStretch(1);
		tmpL->addWidget(new QLabel(tr("Result A:")));
		tmpL->addWidget(pCompareSelectA);
		tmpL->addWidget(pCompareImageA);
		tmpL->addStretch(1);
	}

	auto compareWidgetB = new QWidget;
	{
		auto tmpL = new QVBoxLayout;
		compareWidgetB->setLayout(tmpL);
		pCompareImageB = new ImageSelectLabel;
		pCompareImageB->setText(tr("Select Image"));
		pCompareSelectB = new QComboBox;
		tmpL->addStretch(1);
		tmpL->addWidget(new QLabel(tr("Result B:")));
		tmpL->addWidget(pCompareSelectB);
		tmpL->addWidget(pCompareImageB);
		tmpL->addStretch(1);
	}

	layout->addWidget(pInputLabel, 0, 0, 1, 1);
	layout->addWidget(pPaletteLabel, 0, 1, 1, 1);
	layout->addWidget(processWidget, 0, 2, 1, 1);
	layout->addWidget(pResultLabel, 0, 3, 1, 1);
	layout->addWidget(compareWidgetA, 1, 0, 1, 2);
	layout->addWidget(compareWidgetB, 1, 2, 1, 2);

	auto timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onUpdate()));
	timer->start(50);

	connect(pInputLabel, SIGNAL(clicked()), this, SLOT(onInputClick()));
	connect(pPaletteLabel, SIGNAL(clicked()), this, SLOT(onPaletteClick()));
	connect(pProcessButton, SIGNAL(clicked()), this, SLOT(onProcessClick()));
	connect(pCompareImageA, SIGNAL(clicked()), this, SLOT(onCompareImageASave()));
	connect(pCompareImageB, SIGNAL(clicked()), this, SLOT(onCompareImageBSave()));
	connect(pAlgorithmSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));
	connect(pDistanceSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));
	connect(pCompareSelectA, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectAChanged(int)));
	connect(pCompareSelectB, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectBChanged(int)));
	connect(pIterations, SIGNAL(textEdited(const QString &)), this, SLOT(onIterationsChanged(const QString &)));

	vFuncs.append(rtm_distance);	pDistanceSelect->addItem("Reference Distance");
	vFuncs.append(cmetric);			pDistanceSelect->addItem("Color Metric");
	vFuncs.append(ciede2000);		pDistanceSelect->addItem("CieDe 2000");
	vFuncs.append(cie1976);			pDistanceSelect->addItem("Cie 1967");
	vFuncs.append(hue_distance);	pDistanceSelect->addItem("HSV Hue Based");

	vAlgos.append(new AlgorithmBisectDistanceThreaded);
	vAlgos.append(new AlgorithmSwapDistance);
	vAlgos.append(new AlgorithmIndexedReplace);
	vAlgos.append(new AlgorithmBisectDistance);
	vAlgos.append(new AlgorithmBisectDistanceQt);
	vAlgos.append(new AlgorithmCopy);
	for (auto algo : vAlgos)
	{
		pAlgorithmSelect->addItem(algo->name());
		connect(algo, SIGNAL(step()), this, SLOT(onStep()));
		connect(algo, SIGNAL(finished(QImage *)), this, SLOT(onFinished(QImage *)));
	}

	qsrand(QDateTime::currentDateTime().toTime_t());
}

MainWindow::~MainWindow()
{
	for (auto algo : vAlgos)
		delete algo;

	delete pInput;
	delete pPalette;
	delete ui;
}

void MainWindow::enableProcess()
{
	pProcessButton->setEnabled(pInput && pPalette);
	pProcessButton->setDisabled(false);
	bUpdate = false;
}

void MainWindow::onStep()
{
	iCurSteps++;
	bCanStep = true;
	setImageToLabel(pResultLabel, pAlgo->result());
}

void MainWindow::onFinished(QImage *result)
{
	QString name = sResultName + QString("%1").arg(float(mTimer.elapsed()/1000.0f));

	bUpdate = false;
	setImageToLabel(pResultLabel, result);

	vResults.append({*result, name});
	pCompareSelectA->addItem(name);
	pCompareSelectB->addItem(name);
}

void MainWindow::onUpdate()
{
	if (!bUpdate)
		return;

	if (iCurSteps >= iMaxSteps)
	{
		onFinished(pAlgo->result());
		return;
	}

	if (bCanStep)
	{
		bCanStep = false;
		pAlgo->process();
	}

	statusBar()->showMessage(tr("Elapsed time: %1").arg(float(mTimer.elapsed()/1000.0f)));
}

void MainWindow::onIterationsChanged(const QString &)
{
	pProcessButton->setDisabled(false);
	bUpdate = false;
}

void MainWindow::onSelectAChanged(int i)
{
	auto result = vResults.at(i);
	setImageToLabel(pCompareImageA, &result.mImage);
}

void MainWindow::onSelectBChanged(int i)
{
	auto result = vResults.at(i);
	setImageToLabel(pCompareImageB, &result.mImage);
}

void MainWindow::onIndexChanged(int)
{
	pProcessButton->setDisabled(false);
	bUpdate = false;
}

void MainWindow::onProcessClick()
{
	iMaxSteps = pIterations->text().toInt();
	iCurSteps = 0;
	pProcessButton->setDisabled(true);

	auto i = pAlgorithmSelect->currentIndex();
	pAlgo = vAlgos.at(i);

	i = pDistanceSelect->currentIndex();
	pFunction = vFuncs.at(i);

	sResultName = QString("%1 - %2 - ").arg(pDistanceSelect->itemText(i)).arg(pAlgo->name());

	mTimer.start();
	bUpdate = true;
	pAlgo->setup(pInput, pPalette, pFunction);
	pAlgo->process();
}

void MainWindow::onInputClick()
{
	auto img = openImage();
	if (img)
	{
		delete pInput;
		pInput = img;

		setImageToLabel(pInputLabel, pInput);
	}

	enableProcess();
}

void MainWindow::onPaletteClick()
{
	if (!pInput)
		return;

	auto img = openImage();
	if (img)
	{
		delete pPalette;
		setImageToLabel(pPaletteLabel, img);
		pPalette = img;
	}

	enableProcess();
}

void MainWindow::onCompareImageASave()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Image (*.png;*.jpg;*.bmp;*.gif;*.pbm;*.pgm;*.ppm;*.tiff;*.xbm;*.xpm)"));
	if (name.isEmpty())
		return;

	auto result = vResults.at(pCompareSelectA->currentIndex());
	result.mImage.save(name);
}

void MainWindow::onCompareImageBSave()
{
	QString name = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Image (*.png;*.jpg;*.bmp;*.gif;*.pbm;*.pgm;*.ppm;*.tiff;*.xbm;*.xpm)"));
	if (name.isEmpty())
		return;

	auto result = vResults.at(pCompareSelectB->currentIndex());
	result.mImage.save(name);
}

QImage *MainWindow::openImage()
{
	QString name = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image (*.png)"));
	if (name.isEmpty())
		return nullptr;

	return new QImage(name);
}

void MainWindow::setImageToLabel(QLabel *label, QImage *img)
{
	if (!img)
		return;

	QPixmap px;
	px.convertFromImage(*img);
	label->setPixmap(px);
	label->setMask(px.mask());
}
