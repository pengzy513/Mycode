#ifndef AnalyzeWaveform_h
#define AnalyzeWaveform_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <TROOT.h>
#include <TGraph.h>
#include <stdint.h>
#include <TCanvas.h>
#include <TString.h>
#include "CalculateCharge.h"

//****parameters for data_read*******
#define BUFFER_LENGTH  10240   //in byte //65844
#define MAX_CHANNEL_NUMBER  5   //16*16*2
#define TIMESTAMP_ELEMENT_NUMBER   5 //totally 5 numbers with each 11bits in 8ns unit

using namespace std;

class AnalyzeWaveform
{
private:
 std::string filename;
 std::stringstream ss;
 std::ifstream file;
 unsigned char residue;

 double risetime_start=0,risetime_end=0;
 double FWHM_start=0,FWHM_end=0;
 double falltime_start=0,falltime_end=0;
 
 const int gain_ch[6][2]={ {0,0},{1,0},{0,1},{1,1},{0,2},{1,2} };
 int Channel_No;
 double Num_records=0;
 double record_size=2032;//total waveform package size 
                         //include head(16 byte),waveform(2000 byte,1000 data points),tail(16 Byte)
//  double residue = 0;
 double packed_waveform = 0;

 int  readout_star=0;
 uint32_t star[BUFFER_LENGTH];
 uint32_t size_bytes = 0;
 uint32_t event[BUFFER_LENGTH];
 uint16_t event_short[BUFFER_LENGTH];
        
 string picname;

 
 vector<double> Xarray;
 vector<double> Yarray;

 int gain_gcu=1;
 int ch_gcu=2;
        
 int global_event_ID=0;
 int global_range_ID=0;
 int global_GCUboard_ID=0;
 int global_package_size=0;
 int package_points=0;
 int test_i=0;// waveform
 int event_number_in_file=0;
 int readout_size=0;
 int j=0;//statis the number of abnormal waveform
 

 
 CalculateCharge cal; 

public:

    void GetFileName(const std::string& filename)
    {
        this->filename = filename;
        // std::ifstream file;
        
        file.open(filename.c_str());    
        if(file.is_open())
        {
            cout << "File is open !!!" <<endl;
        }
        else
        {
            cout << "Error opening file" << endl;
        }
        file.close();
    }

    int get_gain(const int Channel_No,int& gain, int& ch){
        gain=gain_ch[Channel_No][0];
        ch=gain_ch[Channel_No][1];
        return 0;
    }

    void read_data()
    {
        int n_counts=0;
        double baseline_temp=0;

        // vector<double> &test_charge2 ;
        
        GetFileName(filename);
        cout << "File name is " << filename << endl;
        FILE *fp = fopen(filename.c_str(), "rb");
        if(fp==NULL)
        {
            cout << "Error opening file" << endl;
        }
        else
        {
            cout << "File is open !!!" <<endl;

        }
        //=================== obtain the file size and calculate==================
        fseek(fp, 0, SEEK_END);   // move pointer to the end of file
        long fileSize = ftell(fp); // get the position of pointer
        fseek(fp, 0, SEEK_SET);   // move pointer to the beginning of file
        cout << "File size is " << fileSize<<" Byte" << endl;
        Num_records=fileSize/record_size;
        cout<<"the numbers of waveform are "<<std::fixed << std::setprecision(0)<< Num_records <<endl;

        while(test_i<Num_records)
        {
            test_i=test_i+1;
            size_bytes=4;
            readout_star=fread(event,sizeof(uint32_t),size_bytes,fp); //DAQ save data in 4 bytes
            if((event[0]&0xFFFF)!=0x6980){continue;}
            //**********read head of waveform and save*************
            for(int i=0;i<int(size_bytes);i++)
               {
                event_short[i*2]=(event[i]&0xFFFF);
                
                cout<<"raw event_short =" << event_short[i*2] <<endl;
                event_short[i*2]=((event_short[i*2]&0xFF)<<8)+((event_short[i*2]&0xFF00)>>8);
                event_short[i*2+1]=(event[i]&0xFFFF0000)>>16;
                event_short[i*2+1]=((event_short[i*2+1]&0xFF)<<8)+((event_short[i*2+1]&0xFF00)>>8);
                if(i==0)
                  {
                //    global_range_ID=((event_short[i*2+1]&0x1));
                  global_range_ID=((event_short[i*2+1]));
                  cout<<"global_range_ID="<<global_range_ID<<endl;
                  }
                if (global_range_ID < 0 || global_range_ID > 5) 
                {
                    j=j+1;
                    break;
                }
                if(i==1)
                  {
                  package_points=event_short[i*2]*8;
                //   cout<<"package_points ="<<package_points<<endl;
                  }
                //   cout<<i<<"  event_short[i*2] = " << event_short[i*2]<<"  event_short[i*2+1] ="<< event_short[i*2+1] << endl;
               
               }
            //*******************read waveform data and judge the data ******************
            size_bytes=(package_points-8-8)/2;//sampling points of package, (package_points -8(head)-8(tail))*16bits
            readout_size=fread(event,sizeof(uint32_t),size_bytes,fp);
            for(int i=0;i<int(size_bytes);i++)
            {
                event_short[i*2]=(event[i]&0xFFFF);
                
                // if((event[i]&0x00FFFF00) == 0x00698000)
                // {
                //     j=j+1;
                //     break;
                // }
                // if((event[i]&0xFFFF) == 0x6980)
                // {
                //     j=j+1;
                //     break;
                // }
                event_short[i*2]=((event_short[i*2]&0xFF)<<8)+((event_short[i*2]&0xFF00)>>8);
                event_short[i*2+1]=(event[i]&0xFFFF0000)>>16;
                // if((event[i]&0xFFFF0000) == 0x6980)
                // {
                //     j=j+1;
                //     break;
                // }
                event_short[i*2+1]=((event_short[i*2+1]&0xFF)<<8)+((event_short[i*2+1]&0xFF00)>>8);
                
                Yarray.push_back(static_cast<double>(event_short[i*2]));
                Yarray.push_back(static_cast<double>(event_short[i*2+1]));
                Xarray.push_back(i*2);
                Xarray.push_back(i*2+1);
                

            }

            get_gain( global_range_ID,gain_gcu,ch_gcu);
            //  cout <<"Channal-No = " << global_range_ID << " ch=" << ch_gcu << " gain_gcu= " << gain_gcu<<endl;
            // CalculateCharge cal;
            cal.calculate_peak(Yarray,ch_gcu,gain_gcu);
            // if( gain_gcu==1 && ch_gcu==1)
            // {
            //     int n=1000;
            //     TCanvas *c2 = new TCanvas("c2","waveform",800,600);
            //     TGraph *result = new TGraph(n,&Xarray[0],&Yarray[0]);
            //     result->Draw();           
            //     picname = "/home/joker/PMT_Experiment/Radiation_Experiment/temp2/"+std::to_string(test_i)+".png";
            //     cout << picname << endl;
            //     c2->Print(picname.c_str(),"png1200");
            //     c2->Clear();  
            // }







            Yarray.clear(); 
            Xarray.clear();

            size_bytes=(8)/2; //tail of package, 8*16bits
            fread(event,sizeof(uint32_t),size_bytes,fp);
            // if(test_i==1000){break;}
        }
        cout << " j = " <<j<<endl;
        fclose(fp);
        TCanvas *c1 = new TCanvas("c1","c1",1100,1100);
        c1->Divide(2,2);
        TH1F *h1 = new TH1F("Channal_No 1","charge spectrum",800,0,20);
        TH1F *h2 = new TH1F("Channal_No 2","charge spectrum",800,0,20);
        TH1F *h3 = new TH1F("Channal_No 3","charge spectrum",800,0,20);




        std::vector<double>& charge_data0 = cal.getChargeCh0();
        std::vector<double>& charge_data1 = cal.getChargeCh1();
        std::vector<double>& charge_data2 = cal.getChargeCh2();

        cout << " the size of charge_data0 = " << charge_data0.size() << endl;
        cout << " the size of charge_data1 = " << charge_data1.size() << endl;
        cout << " the size of charge_data2 = " << charge_data2.size() << endl;

        for (double charge : charge_data0) 
        {
            h1->Fill(charge);
        }
        for (double charge : charge_data1) 
        {
            h2->Fill(charge);
        }
        for (double charge : charge_data2) 
        {
            h3->Fill(charge);
        }
        c1->cd(1);
        h1->GetYaxis()->SetTitle("Counts");
        h1->GetXaxis()->SetTitle("PE");
        h1->Draw();
        c1->Update();
        
        c1->cd(2);
        h2->GetYaxis()->SetTitle("Counts");
        h2->GetXaxis()->SetTitle("PE");
        h2->Draw();
        c1->Update();

        c1->cd(3);        
        h3->GetYaxis()->SetTitle("Counts");
        h3->GetXaxis()->SetTitle("PE");
        h3->Draw();
        c1->Update();

        // c1->Print("charge_spectrum0605-1704.png", "png");
        // c1->Print("charge_spectrum-GCU5934-0606-1707.png", "png");
        // c1->Print("charge_spectrum-GCU5934-0607-1624.png", "png");
        // c1->Print("charge_spectrum-GCU5934-06-08-2331.png","png1200");
        // c1->Print("charge_spectrum-GCU7049-0617-2315.png","png1200");//NS = no source
        // c1->Print("charge_spectrum-GCU7049-06118-22-39.png","png1200");
    }


};

#endif // AnalyzeWaveform_h

