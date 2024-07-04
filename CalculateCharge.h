#ifndef CalculateCharge_h
#define CalculateCharge_h

#include "AnalyzeWaveform.h"

class CalculateCharge
{
private:
//  vector<double> wave;
 vector<double> charge_ch1;
 
 vector<double> charge_ch2;
 vector<double> charge_ch0;
 
 
 vector<double> peaks;
 double baseline=0;
 double tenkilo=10000;

 const int threshold=25;
 const double high_gain_factor = (1.2/(1<<14))*1000/0.55;
 const double low_gain_factor = (1.2/(1<<14))*1000/0.08;
 
 int peak_times=0;

public:
    void calculate_peak( vector<double> &wave, int &ch,int &gain,const int &min_length = 18)
    {
         double mean=0;
         double sumwave=0;
         int threshold_count=0;
         int start_mark=0;
         
        for(int i=0;i<wave.size();i++)
        {
            sumwave = sumwave+wave[i];
        }
        mean = sumwave/(wave.size());//first mean
        sumwave=0;
        
        for(int j=0;j<wave.size();j++)
        {
            if(wave[j]>mean-threshold)
            {
                threshold_count = threshold_count +1;
                sumwave = sumwave+wave[j];
            }
        }
        baseline = sumwave/threshold_count;
        
        for(int k=0;k<wave.size();k++)
        {
            if (k<=9)
            {
                if(wave[k]<baseline-threshold)
                {
                start_mark=k;
                }
            }
            else 
            {  
                if (wave[k]<baseline-threshold)
                {
                    if (start_mark==0)//add before peak
                    {
                        for (int peak_n=k-10;peak_n<k;peak_n++){peaks.push_back(wave[peak_n]);}
                        start_mark=k;
                        peaks.push_back(wave[k]);
                    }
                    else if(start_mark>9)//add peak
                    {
                        peaks.push_back(wave[k]);
                    }   
                }
                
                else //the situation : wave[k]>baseline-threshold
                {
                    if ((peaks.size()>min_length )&& (start_mark>9))
                    {
                        if (k<=wave.size()-11){//add after peak
                            for (int peak_n=k;peak_n<k+10;peak_n++){peaks.push_back(wave[peak_n]);}
                        }
                        else if (wave[k]<baseline-threshold){continue;}
                        //get charge here
                        double peak_charge=0;
                        for (int ns=0;ns<peaks.size();ns++)
                        {
                            
                            peak_charge+=(baseline-peaks.at(ns));
                        }
                        peak_charge = (peak_charge * high_gain_factor/(1E3*1E9*50*1E-12))/1.602;
                       
                        // cout <<"peak =" <<std::fixed<<std::setprecision(6)<< peak_charge << endl;
                        // cout << " gain =" << gain << " ch ="<<ch<<endl;
                        // charge.push_back(peak_charge);//unit: adc*ns
                        if(gain==1&&ch==0)
                        {
        
                            charge_ch0.push_back(peak_charge);//unit: adc*ns
                            // cout << " charge_ch0 = " << charge_ch0[peak_times] << endl;

                        }
                        else if (gain ==1 && ch==1)
                        {
                            charge_ch1.push_back(peak_charge);//unit: adc*ns
                            //  cout << " charge_ch1 = " << charge_ch1[peak_times] << endl;
                        }
                        else if (gain==1 && ch==2)
                        {
                            charge_ch2.push_back(peak_charge);//unit: adc*ns
                            // cout << " charge_ch2 = " << std::fixed<<std::setprecision(6)<<charge_ch2[peak_times] << endl;
                            peak_times+=1;
                        }

                    }
                    
                    peaks.clear();
                    start_mark=0;

                }
            }
        }


    }  


    vector<double>& getChargeCh0() { return charge_ch0; }
    vector<double>& getChargeCh1() { return charge_ch1; }
    vector<double>& getChargeCh2() { return charge_ch2; }

};

 #endif //CaculateCharge

