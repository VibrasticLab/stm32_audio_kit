#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "ch.h"
#include "hal.h"

#include "m_audio.h"

/**
 * @brief I2S Transmit buffer
 */
static uint16_t i2s_tx_buf[TOTAL_BUFF_SIZE];

/**
 * @brief I2S Protocol config struct
 */
static I2SConfig i2scfg = {
  i2s_tx_buf,
  NULL,
  I2S_BUFF_SIZE,
  NULL,
  0,
  16,
};

void m_audio_Init(void){
    palSetPadMode(GPIOB,12, PAL_MODE_ALTERNATE(5));
    palSetPadMode(GPIOB,10, PAL_MODE_ALTERNATE(5));
    palSetPadMode(GPIOC, 3, PAL_MODE_ALTERNATE(5));
}

void m_audio_Zero(void){
    uint16_t i;
    for(i=0;i<TOTAL_BUFF_SIZE;i++){
        i2s_tx_buf[i] = 0;
    }
}

void m_audio_Tone(double freq, double ampl){
    uint16_t i;
    uint16_t buffsize;
    double ysin;
    double ampl_act;

    buffsize = (uint16_t) I2S_BUFF_SIZE/freq;

    ampl_act = DEFAULT_ATTEN*ampl*32767;
    if(ampl_act<=DEFAULT_AMPL_THD){ampl = 0;}

    m_audio_Zero();

    for(i=0;i<buffsize;i++){
        ysin = DEFAULT_ATTEN*ampl*32767*sin(2*3.141592653589793*((double)i/(double)buffsize));

        if(ysin >= 0){
            i2s_tx_buf[i]=ysin;
#if USE_STEREO_ARRAY
            i2s_tx_buf[i+1]=ysin;
#endif
        }
        if(ysin < 0){
            i2s_tx_buf[i]=ysin+65535;
#if USE_STEREO_ARRAY
            i2s_tx_buf[i+1]=ysin+65535;
#endif
        }
    }

    i2scfg.size = buffsize;
}

void m_audio_Play(uint16_t duration){
    i2sStart(&I2SD2, &i2scfg);
    i2sStartExchange(&I2SD2);

    chThdSleepMilliseconds(duration*10);

    i2sStopExchange(&I2SD2);
    i2sStop(&I2SD2);
}

void m_audio_Test(void){
    m_audio_Tone(1.25,1);

    chThdSleepMilliseconds(200);
    m_audio_Play(TEST_DURATION);

    chThdSleepMilliseconds(200);
    m_audio_Play(TEST_DURATION);
}
