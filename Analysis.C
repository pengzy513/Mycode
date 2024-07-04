#include "AnalyzeWaveform.h"
#include "CalculateCharge.h"

int Analysis(const char* Filename)
{
    AnalyzeWaveform aw;
    aw.GetFileName(Filename);
    aw.read_data(); 

    // CalculateCharge cal;
    // cal.calculate_peak();


    return 0;
}