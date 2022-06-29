#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <vector>

using namespace std;


int main(int argc, char* argv[]) {
  
    const char *ofile = "iq.bin";
    FILE *outfile = fopen(ofile, "wb");
    if (outfile==NULL) perror ("Error opening file to write");
    
//    const bool generate_csv {false};
/*    const char *ofile_csv = "c:\\temp\\iq.csv";
    FILE *csvfile = fopen(ofile_csv, "w");
        if (!csvfile) 
        {
                perror("Error opening CSV file");
                return 1;
        }*/

    int16_t dac_range = INT16_MAX - 1;
    double gain1 = 1;
    //double gain2 = 0.49;
    int seconds = 1;
    long double fs1 = 60000000;
    int freq1 = 1*fs1; // more of an offset than a frequency
    //int freq1 = 1580; //4.58
    //int freq2 = 0.6*10230000;
    long double symbol_duration_time = 1/fs1; 
    
    vector<double> sample_time = {};
    bool multi_frequency_flag {false};

    //fill t with values from 0 to 1 in steps of 1/fs1
    for(int index = 0; index<fs1; ++index)
    {
        sample_time.push_back(index*symbol_duration_time);
    }
    
    int samples_in_a_sec = sample_time.size();
    
    
    printf("START OF DATA\n");
    
    for(int seconds_into_run=0; seconds_into_run < seconds; ++seconds_into_run) 
    {
   
        for(int sample_index =0; sample_index < samples_in_a_sec; ++sample_index)
        {
            int16_t ival {0};
            int16_t qval {0};
            if(multi_frequency_flag)
            {
                //ival = (dac_range * gain1 * sin(2*M_PI*freq1*(1/fs1*sample_index))) + (dac_range * gain2 * sin(2*M_PI*freq2*(1/fs1*sample_index)));
                //qval = (dac_range * gain1 * cos(2*M_PI*freq1*(1/fs1*sample_index))) +  (dac_range * gain2 * cos(2*M_PI*freq2*(1/fs1*sample_index)));
            } else {
                ival = dac_range * gain1 * sin(2*M_PI*freq1*(1/fs1*sample_index));
                qval = dac_range * gain1 * cos(2*M_PI*freq1*(1/fs1*sample_index));
            }


            char bytes[4];
            bytes[0] = ival & 0xff;
            bytes[1] = ival >> 8;
            bytes[2] = qval & 0xff;
            bytes[3] = qval >> 8;
            
          
//              fprintf(csvfile, "%d,%d\n", ival, qval);
              fwrite(bytes, sizeof bytes, 1, outfile);


        } 
        
        cout << "approximately " << (seconds_into_run + 1) << " seconds of IQ data produced" << endl;
    }


//        fclose(csvfile);
        fclose(outfile);

    
    
    
    return 0;
}