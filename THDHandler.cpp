#include "THDHandler.h"
#include <QVBoxLayout>

THDHandler::THDHandler(QString initChannelString, double initBaseFrequency,
                       uint16_t initNoOvertones, double initSampleFrequency, QWidget* parent) :
    QWidget(parent), m_channelString(initChannelString), m_baseFrequency(initBaseFrequency), m_noOvertones(initNoOvertones), m_sampleFrequency(initSampleFrequency)
{
    // initialize the GUI elements to control the THD calculation
    QFont font( "Helvetica", 10 );
    font.setBold( true );
    m_channelTitle = new QLabel(initChannelString);
    m_channelTitle->setAlignment( Qt::AlignHCenter );
    m_channelTitle->setFont( font );

    // m_channelTitle->setBaseSize(QSize(200,50));
    // m_channelTitle->setMinimumSize(200,50);
    m_knobNoOvertones = new Knob("Select no. overtones", 1, 20);
    // m_knobNoOvertones->setMinimumSize(200,250);
    m_knobNoOvertones->setValue((double)m_noOvertones);
    // m_knobNoOvertones->setBaseSize(QSize(200,250));
    m_labelBaseFrequency = new QLabel("Select THD base frequency / kHz");
    m_labelBaseFrequency->setAlignment(Qt::AlignHCenter );
    m_labelBaseFrequency->setFont( font );
    // m_labelBaseFrequency->setMinimumSize(200,50);
    m_sliderBaseFrequency = new QwtSlider(Qt::Horizontal);
    m_sliderBaseFrequency->setScale(0.0,20.0);
    m_sliderBaseFrequency->setValue(m_baseFrequency/1000.0);
    // m_sliderBaseFrequency->setBaseSize(QSize(200,200));
    // m_sliderBaseFrequency->setMinimumSize(200,200);
    m_THDResultLabelString = "THD Results:";
    m_THDResultLabel = new QLabel(m_THDResultLabelString);
    m_THDResultLabel->setAlignment(Qt::AlignHCenter );
    m_THDResultLabel->setFont(font);
    m_THDResultString = " ";
    m_THD_F = 0.0;
    m_THD_R = 0.0;
    m_THDResultString = "THD_F=" + QString::number(m_THD_F*100,'f',3) + " %, THD_R=" + QString::number(m_THD_R*100,'f',3) + " %";
    m_THDResultDisplay = new QLabel(m_THDResultString);
    m_THDResultDisplay->setAlignment( Qt::AlignLeft );
    // m_THDResultDisplay->setBaseSize(QSize(200,50));
    m_THDResultDisplay->setMinimumSize(240,20);
    m_THDResultDisplay->setFrameStyle(QFrame::Sunken);
    m_THDResultDisplay->setStyleSheet("QLabel { background-color : white; }");
    // setSizePolicy( QSizePolicy::MinimumExpanding,
    //                QSizePolicy::MinimumExpanding );

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(m_channelTitle);
    vLayout->addWidget(m_knobNoOvertones);
    vLayout->addWidget(m_sliderBaseFrequency);
    vLayout->addWidget(m_labelBaseFrequency);
    vLayout->addWidget(m_THDResultLabel);
    vLayout->addWidget(m_THDResultDisplay);
    setLayout(vLayout);

    // initialize the THD calculators
    m_THDCalc = new THDCalculator(m_baseFrequency, m_noOvertones, m_sampleFrequency);
    m_THDCalc->initTHDCalculation();
}

void THDHandler::run(VectorXd inputSignal)
{
    m_THDCalc->calcTHD(m_THD_F, m_THD_R, inputSignal);
    m_THDResultString = "THD_F=" + QString::number(m_THD_F*100,'f',3) + " %, THD_R=" + QString::number(m_THD_R*100,'f',3) + " %";
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

  // int off = qCeil( d_knob->scaleDraw()->extent( d_knob->font() ) );
  // off -= 15; // spacing
  // return QSize( w, h - off );

  return QSize( w, h );
}
