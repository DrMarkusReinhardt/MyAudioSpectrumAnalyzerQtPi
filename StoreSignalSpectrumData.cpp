#include "StoreSignalSpectrumData.h"

StoreSignalSpectrumData::StoreSignalSpectrumData(
        VectorXd init_t, VectorXd init_leftSignal, VectorXd init_rightSignal,
        VectorXd init_f, VectorXd init_leftSpectrum, VectorXd init_rightSpectrum) :
        m_t(init_t), m_leftSignal(init_leftSignal), m_rightSignal(init_rightSignal),
        m_f(init_f), m_leftSpectrum(init_leftSpectrum), m_rightSpectrum(init_rightSpectrum)
{

}

void StoreSignalSpectrumData::saveData(QString FileName)
{
   std::ofstream myfile;
   myfile.open(FileName.toStdString());
   myfile << m_t.size() << std::endl;
   myfile << m_t << std::endl;
   myfile << m_leftSignal.size() << std::endl;
   myfile << m_leftSignal << std::endl;
   myfile << m_rightSignal.size() << std::endl;
   myfile << m_rightSignal << std::endl;
   myfile << m_f.size() << std::endl;
   myfile << m_f << std::endl;
   myfile << m_leftSpectrum.size() << std::endl;
   myfile << m_leftSpectrum << std::endl;
   myfile << m_rightSpectrum.size() << std::endl;
   myfile << m_rightSpectrum << std::endl;
   myfile.close();
}

/*
void StoreSignalSpectrumData::saveData(QString FileName)
{
   std::string fileName = FileName.toStdString();
   // char fileName[30];
   // fileName = strFileName.c_str();
   printf("filename = %s", fileName.c_str());
   FILE* fileHandle = fopen(fileName.c_str(),"w");
   if (fileHandle != nullptr)
   {
       for(uint16_t k = 0; k < m_t.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_t[k]);
           fprintf(fileHandle, " ");
       }
       fprintf(fileHandle,"\n");

       for(uint16_t k = 0; k < m_leftSignal.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_leftSignal[k]);
           fprintf(fileHandle, " " );
       }
       fprintf(fileHandle,"\n");

       for(uint16_t k = 0; k < m_rightSignal.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_rightSignal[k]);
           fprintf(fileHandle, " " );
       }
       fprintf(fileHandle,"\n");

       for(uint16_t k = 0; k < m_f.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_f[k]);
           fprintf(fileHandle, " " );
       }
       fprintf(fileHandle,"\n");

       for(uint16_t k = 0; k < m_leftSpectrum.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_leftSpectrum[k]);
           fprintf(fileHandle, " " );
       }
       fprintf(fileHandle,"\n");

       for(uint16_t k = 0; k < m_rightSpectrum.size(); k++)
       {
           fprintf(fileHandle, "%6.3f", m_rightSpectrum[k]);
           fprintf(fileHandle, " " );
       }
       fprintf(fileHandle,"\n");

       fclose(fileHandle);
   }
}
*/

void StoreSignalSpectrumData::save()
{
    saveData(m_defaultFileName);
}

void StoreSignalSpectrumData::saveAs(QString FileName)
{
   saveData(FileName);
}
