#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <exception>
#include <string>

#define VERSION "V1.01"

int main(int argc, char* argv[]) {
  
    if (argc == 1)
    {
        std::cout << "Version: " << VERSION
                  << "\nInsuffitient arguments:"
                  << "\nSinewave IQ data generator, packs data to ION data standard, 32bit words"
                  << "\nintel specific data packing, check your endianess!!!"
                  << "\nChirp starts from -deviation and goes to + deviation"
                  << "\nProgram syntax: <Fs> <bitdepth> <deviation> <chirp> <duration_secs>"
                  << "\nInput units: Fs = Hz, bit depth = 4 8 16, deviation = Hz, chirp 0 or 1,"
                  << "\nSeconds = integer seconds"
                  << "\nExample: ./iq_data_gen 60e6 16 100 0 5" 
                  << "\nThat's 60MHz, 16bit, 100Hz deviation from centre, chrip off, 5 seconds"
                  << "\nDuration limit is 17 seconds max at 120MHz " << std::endl;
        return 0;
    }
    else if ( argc == 6)
    {
        std::cout << "correct number of arguments, but limited error checking in place" << std::endl;
    }
    else
    {
        std::cout << "argument number incorrect exiting" << std::endl;
        return -1;
    }

    const char *ofile = "iq.bin";
    FILE *outfile = fopen(ofile, "wb");
    if (outfile==NULL) perror ("Error opening file to write");

    //refactor all of the assignment! one try catch block I think

    int bit_depth {};
    std::string bd_str = argv[2];
    try 
    {
        bit_depth = std::stoi(bd_str);
    } 
    catch (std::exception const &ex) 
    {
        std::cerr << "exception thrown " << ex.what() << std::endl;
        return -1;
    }

    int16_t dac_range_4bit = 0x7; 
    int16_t dac_range_8bit = INT8_MAX - 1;
    int16_t dac_range_16bit = INT16_MAX - 1;

    int16_t* dac_range_p {}; 

    switch( bit_depth )
    {
        case 4:
            dac_range_p = &dac_range_4bit;
            break;
        case 8:
            dac_range_p = &dac_range_8bit;
            break;
        case 16:
            dac_range_p = &dac_range_16bit;
            break;
        default:
            std::cout << "\nBit depth not recognised" << std::endl;
            return 0;
    }

    double fs {};
    std::string fs_str = argv[1];
    try
    {
        fs = std::stod(fs_str);
    }
    catch (std::exception const &ex) 
    {
        std::cerr << "exception thrown " << ex.what() << std::endl;
        return -1;
    }

    double deviation {};
    bool deviation_flag = false;
    std::string freq_offset_str = argv[3];
    try
    {
        deviation = std::stod(freq_offset_str);
        if (deviation)
            deviation_flag = true;
            
    }
    catch (std::exception const &ex) 
    {
        std::cerr << "exception thrown " << ex.what() << std::endl;
        return -1;
    }

    int chirp_flag {0};
    std::string chirp_str = argv[4];

    try
    {
        chirp_flag = std::stoi(chirp_str);
    }
    catch (std::exception const &ex) 
    {
        std::cerr << "exception thrown " << ex.what() << std::endl;
        return -1;
    }

    int seconds {1};
    std::string duration_secs = argv[5];
    try
    {
        seconds = std::stoi(duration_secs);
    }
    catch(std::exception const &ex)
    {
        std::cerr << "exception thrown " << ex.what() << std::endl;
        return -1;
    }

    double gain = 1;
    
    //long double symbol_duration_time = 1/fs; 

    std::cout << "fs = " << fs << ", bits = " << bit_depth 
              << ", deviation = " << deviation 
              << ", chirp flag = " << chirp_flag 
              << ", duration secs = " << seconds << std::endl;
    
    
    std::cout << "\nSTARTING DATA GENERATION" << std::endl;
    
    for( int sample_index = 0; sample_index < ( fs * seconds ); ++sample_index)
    {
        int16_t ival {0};
        int16_t qval {0};
        double t = 1/fs*sample_index;
        
        if (chirp_flag)
        {
            ival = (*dac_range_p) * gain * sin(2.0*M_PI*(-deviation/2 + deviation/seconds/2 *t ) * t);
            qval = (*dac_range_p) * gain * cos(2.0*M_PI*(-deviation/2 + deviation/seconds/2 *t ) * t);
        }
        else if (deviation_flag)
        {
            ival = (*dac_range_p) * gain * sin(2.0 * M_PI * deviation * t );
            qval = (*dac_range_p) * gain * cos(2.0 * M_PI * deviation * t );
        }
        else
        {
            ival = (*dac_range_p) * gain * sin(2.0 * M_PI * t );
            qval = (*dac_range_p) * gain * cos(2.0 * M_PI * t );
        }
        //32 bit words for ION format
        
        // Intel processor on laptop so need to change byte order to meet ION standard.
        if ( bit_depth == 16 )
        {
            uint8_t bytes[4];
            bytes[0] = (ival >> 8) & 0xff;
            bytes[1] = ival & 0xff;
            bytes[2] = (qval >> 8) & 0xff;
            bytes[3] = qval & 0xff;
            fwrite(bytes, sizeof bytes, 1, outfile);
        }
        else if ( bit_depth == 8 )
        {
            uint8_t byte = (uint8_t)ival;
            fwrite(&byte, sizeof(uint8_t), 1, outfile);
            byte = (uint8_t)qval;
            fwrite(&byte, sizeof(uint8_t), 1, outfile);
        }
        else // bit depth is 4, checked above
        {
            uint8_t byte = (uint8_t)( ( ival & 0xf ) << 4 );
            byte |= (uint8_t) ( qval & 0xf );
            fwrite(&byte, sizeof(uint8_t), 1, outfile);
        }

        if ( 0 )
        {
           // std::cout << "approximately " << (seconds_into_run + 1) << " seconds of IQ data produced" << std::endl;
        }

    } 
        
        

    fclose(outfile);

    return 0;
}