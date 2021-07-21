#include <xc.h>
#include <stdint.h>
#include "../system.h"
#include "../types.h"


/*
    . Configure the A/D module:
? Configure analog/digital I/O (ANSEL)
? Configure voltage reference (ADCON1)
? Select A/D input channel (ADCON0)
? Select A/D conversion clock (ADCON0) //fijo
? Turn on A/D module (ADCON0)
    
    2. Configure A/D interrupt (if desired):
? Clear ADIF bit
? Set ADIE bit
? SET PEIE bit
? Set GIE bit
3. Wait the required acquisition time.
4. Start conversion:
? Set GO/DONE bit (ADCON0)
5. Wait for A/D conversion to complete, by either:
? Polling for the GO/DONE bit to be cleared
(with interrupts disabled); OR
? Waiting for the A/D interrupt
6. Read A/D Result register pair
(ADRESH:ADRESL), clear bit ADIF if required.
7. For next conversion, go to step 1 or step 2 as
required. The A/D conversion time per bit is
defined as TAD. A minimum wait of 2 TAD is
required before the next acquisition starts.
*/
void ADC_go(uint8_t ch)
{
    ADCON0 = (ADCON0 & 0xC0) + (uint8_t)(ch << 3);
    ADON = 1;
    __delay_us(20);//Wait the required acquisition time.
    GO_nDONE = 1;
    while (GO_nDONE)
    {
        ;
    }
    ADON = 0;
}
void ADC_init(void)
{
    ANSEL = 0x07;//CANAL ANS0..ANS2
    ADCON0 = 0x40; //Focs/8  8T ADCS<1:0>=01 @5 MHz
    //ADCON1 = 0x00; //left adjust
    ADCON1 = 0x80; //RIGHT adjust
}
