#include "THDHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

THDHandler::THDHandler(QString initChannelString, double initBaseFrequency_kHz,
                       uint16_t initNoOvertones, double initSampleFrequency, QWidget* parent) :
    QWidget(parent), m_channelString(initChannelString), m_baseFrequency_kHz(initBaseFrequency_kHz), m_noOvertones(initNoOvertones), m_sampleFrequency(initSampleFrequency)
{
    // initialize the GUI elements to control the THD calculation
    QFont font( "Helvetica", 9 );
    font.setBold( true );

    QFont fontTitle( "Helvetica", 12 );
    fontTitle.setBold( true );
    m_channelTitle = new QLabel(initChannelString);
    m_channelTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_channelTitle->setFont(fontTitle);
    // m_channelTitle->setBaseSize(QSize(200,50));
    m_channelTitle->setMinimumSize(200,20);

    m_knobNoOvertones = new Knob("Select no. overtones:", 1, 20);
    m_knobNoOvertones->setValue((double)m_noOvertones);
    m_knobNoOvertones->setMinimumSize(150,130);
    // m_knobNoOvertones->setBaseSize(QSize(200,250));
    QString knobDisplayOvertonesStr(QString::number(initNoOvertones));
    m_displayNoOvertones = new QLabel(knobDisplayOvertonesStr);
    m_displayNoOvertones->setMaximumSize(60,20);
    m_displayNoOvertones->setMinimumSize(40,20);
    m_displayNoOvertones->setFrameStyle(QFrame::Sunken);
    m_displayNoOvertones->setStyleSheet("QLabel { background-color : white; }");
    m_displayNoOvertones->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_displayNoOvertones->setFont(font);

    m_labelBaseFrequency = new QLabel("Select THD base frequency/kHz:");
    m_labelBaseFrequency->setAlignment(Qt::AlignHCenter | Qt::AlignBottom );
    m_labelBaseFrequency->setFont(font);
    m_labelBaseFrequency->setMinimumSize(200,30);
    m_sliderBaseFrequency = new QwtSlider(Qt::Horizontal);
    m_sliderBaseFrequency->setScale(0.0,20.0);
    m_sliderBaseFrequency->setValue(m_baseFrequency_kHz);
    // m_sliderBaseFrequency->setBaseSize(QSize(200,200));
    m_sliderBaseFrequency->setMinimumSize(200,50);
    QString sliderDisplayOvertonesStr(QString::number(m_baseFrequency_kHz,'f',3)+" kHz");
    m_displayBaseFrequency = new QLabel(sliderDisplayOvertonesStr);
    // m_displayBaseFrequency->setMaximumSize(80,20);
    m_displayBaseFrequency->setMinimumSize(200,20);
    m_displayBaseFrequency->setFrameStyle(QFrame::Sunken);
    m_displayBaseFrequency->setStyleSheet("QLabel { background-color : white; }");
    m_displayBaseFrequency->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_displayBaseFrequency->setFont(font);

    m_THDResultLabelString = "THD Results:";
    m_THDResultLabel = new QLabel(m_THDResultLabelString);
    m_THDResultLabel->setMinimumSize(200,30);
    m_THDResultLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    m_THDResultLabel->setFont(font);
    m_THDResultString = " ";
    m_THD_F = 0.0;
    m_THD_R = 0.0;
    m_THDResultString = "THD_F = " + QString::number(m_THD_F*100,'f',3) + " %, THD_R = " + QString::number(m_THD_R*100,'f',3) + " %";
    m_THDResultDisplay = new QLabel(m_THDResultString);
    m_THDResultDisplay->setFont(font);
    m_THDResultDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // m_THDResultDisplay->setBaseSize(QSize(200,50));
    m_THDResultDisplay->setMinimumSize(200,20);
    m_THDResultDisplay->setFrameStyle(QFrame::Sunken);
    m_THDResultDisplay->setStyleSheet("QLabel { background-color : white; }");
    // setSizePolicy( QSizePolicy::MinimumExpanding,
    //                QSizePolicy::MinimumExpanding )

    QGroupBox* THDGroupBox = new QGroupBox();
    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget(m_channelTitle);
    vLayout1->addWidget(m_knobNoOvertones);
    vLayout1->addWidget(m_displayNoOvertones);
    // vLayout1->addLayout(hLayout1);
    vLayout1->addWidget(m_labelBaseFrequency);
    vLayout1->addWidget(m_sliderBaseFrequency);
    vLayout1->addWidget(m_displayBaseFrequency);
    vLayout1->addWidget(m_THDResultLabel);
    vLayout1->addWidget(m_THDResultDisplay);
    vLayout1->addStretch(1);
    THDGroupBox->setLayout(vLayout1);

    QVBoxLayout* vLayoutMain = new QVBoxLayout;
    vLayoutMain->addWidget(THDGroupBox);
    setLayout(vLayoutMain);

    // connect signals and slots
    connect(m_knobNoOvertones,QOverload<double>::of(&Knob::valueChanged),
            this, &THDHandler::updateNoOvertones);

    connect(m_sliderBaseFrequency,QOverload<double>::of(&QwtSlider::valueChanged),
            this, &THDHandler::updateBaseFrequency);

    // initialize the THD calculators
    newBaseFrequency_kHz = m_baseFrequency_kHz;
    m_THDCalc = new THDCalculator(m_baseFrequency_kHz*1000.0, m_noOvertones, m_sampleFrequency);
    m_THDCalc->initTHDCalculation();
}

void THDHandler::updateNoOvertones(double newNoOvertones)
{
   uint16_t noOvertones = (uint16_t)round(newNoOvertones);
   if (noOvertones < 1) noOvertones = 1;
   if (noOvertones > 20) noOvertones = 20;
   m_displayNoOvertones->setText(QString::number(noOvertones));
   m_THDCalc->setNoOvertones(noOvertones);
   m_THDCalc->initTHDCalculation();
}

void THDHandler::updateBaseFrequency(double newBaseFrequency_kHz)
{
   if (newBaseFrequency_kHz < 1.0) newBaseFrequency_kHz = 1.0;
   if (newBaseFrequency_kHz > 20.0) newBaseFrequency_kHz = 20.0;
   m_baseFrequency_kHz = newBaseFrequency_kHz;
   m_displayBaseFrequency->setText(QString::number(newBaseFrequency_kHz,'f',3)+" kHz");
   m_THDCalc->setBaseFrequency(newBaseFrequency_kHz*1000.0);
   m_THDCalc->initTHDCalculation();
}

void THDHandler::run(VectorXd inputSignal)
{
    m_THDCalc->calcTHD(m_THD_F, m_THD_R, inputSignal);
    m_THDResultString = "THD_F = " + QString::number(m_THD_F*100,'f',3) + " %, THD_R = " + QString::number(m_THD_R*100,'f',3) + " %";
    m_THDResultDisplay->setText(m_THDResultString);
    // std::cout << m_channelString.toStdString() << ": THD_F= " << m_THD_F*100  << " %, THD_R= " << m_THD_R*100  << " %" << std::endl;
}

QSize THDHandler::sizeHint() const
{
  QSize sz1 = m_channelTitle->sizeHint();
  QSize sz2 = m_knobNoOvertones->sizeHint();
  QSize sz3 = m_labelBaseFrequency->sizeHint();
  QSize sz4 = m_sliderBaseFrequency->sizeHint();
  QSize sz5 = m_THDResultDisplay->sizeHint();

  const int w = qMax( sz1.width(),qMax(sz2.width(),qMax(sz3.width(),(sz4.width(),(sz5.width())))));
  const int h = sz1.height()+sz2.height()+sz3.height()+sz4.height()+sz5.height();

  return QSize( w, h );
}
