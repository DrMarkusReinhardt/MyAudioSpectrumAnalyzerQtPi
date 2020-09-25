#ifndef THDHANDLER_H
#define THDHANDLER_H

#include "knob.h"
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <qwt_slider.h>
#include "THDCalculator.h"

/* class that handles the parameterization and display of THD calculations
 *
 */

class THDHandler : public QWidget
{
  Q_OBJECT
    
public:
    THDHandler(QString initChannelString, double initBaseFrequency_kHz, uint16_t initNoOvertones,
               double initSampleFrequency, QWidget *parent = nullptr );
    void run(VectorXd inputSignal);
    virtual QSize sizeHint() const;

private slots:
    void updateNoOvertones(double newNoOvertones);
    void updateBaseFrequency(double newBaseFrequency_kHz);
    void dialogBaseFrequency();

private:
    QString m_channelString;
    QLabel *m_channelTitle;
    double newBaseFrequency_kHz;
    double m_baseFrequency_kHz;
    uint16_t m_noOvertones;
    double m_sampleFrequency;
    Knob *m_knobNoOvertones;
    QLabel *m_displayNoOvertones;
    QwtSlider *m_sliderBaseFrequency;
    QLabel *m_displayBaseFrequency;
    QPushButton *m_buttonBaseFrequency;
    THDCalculator *m_THDCalc;
    QString m_THDResultLabelString;
    QLabel *m_THDResultLabel;
    QLabel *m_THDResultDisplay;
    QString m_THDResultString;
    double m_THD_F, m_THD_R;

};

#endif // THDHANDLER_H
