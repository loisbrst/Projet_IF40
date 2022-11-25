/*
 * Projet.c
 *
 *  Created on: 25 nov. 2022
 *      Author: lboursau
 */
//###########################################################################
// Description : Projet IF40: Template
//!
//! Ce programme vous servira de base pour créer votre projet en langage C.
//! Il comporte un exemple:
//! La broche GPIO9 (connectée à LED1) change d'état à chaque seconde
//!
//############################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "stdio.h"

#define CPU_FRQ 150000000L      // Fréquence du processeur.


// Prototype statements for functions found within this file.

//__interrupt void adc_isr(void);
//__interrupt void ePWM1_isr(void);
//__interrupt void ePWM2_isr(void);
//__interrupt void Xint1_isr(void);
//__interrupt void Xint3_isr(void);

// Variables globales
//configuration des GPIO60 et 61 en sortie avec PUD desactivé
void config_gpio(void){
    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60=0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO61=0;
    GpioCtrlRegs.GPBPUD.bit.GPIO60=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO61=1;
    GpioCtrlRegs.GPBDIR.bit.GPIO60=1;
    GpioCtrlRegs.GPBDIR.bit.GPIO61=1;
    EDIS;
}
void envoi0(void){
    GpioDataRegs.GPBCLEAR.bit.GPIO60=1;
    GpioDataRegs.GPBCLEAR.bit.GPIO61=1;
    DELAY_US(1L);
    GpioDataRegs.GPBSET.bit.GPIO61=1;
    DELAY_US(1L);
    GpioDataRegs.GPBCLEAR.bit.GPIO61=1;
    DELAY_US(1L);
}
void envoi1(void){
    GpioDataRegs.GPBSET.bit.GPIO60=1;
        GpioDataRegs.GPBCLEAR.bit.GPIO61=1;
        DELAY_US(1L);
        GpioDataRegs.GPBSET.bit.GPIO61=1;
        DELAY_US(1L);
        GpioDataRegs.GPBCLEAR.bit.GPIO61=1;
        DELAY_US(1L);
}
void affiche(int nombre){
    int conv[15];
    int i,j;
    conv[0]=0xC0;
    conv[1]=0xF9;
    conv[2]=0xA4;
    conv[3]=0xB0;
    conv[4]=0x99;
    conv[5]=0x92;
    conv[6]=0x82;
    conv[7]=0xF8;
    conv[8]=0x80;
    conv[9]=0x90;

    int ecriture[7];
    for(i=0;i<8;i++){
        ecriture[i]=conv[nombre]>>(7-i);
        ecriture[i]&=0x01;
        if (ecriture[i]==0){
            envoi0();
        }else{
            envoi1();
        }
    }
}
main()
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();

   EALLOW;
   #if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
     #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
   #endif
   #if (CPU_FRQ_100MHZ)
     #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
   #endif
   EDIS;

   // Define ADCCLK clock frequency ( less than or equal to 25 MHz )
   // Assuming InitSysCtrl() has set SYSCLKOUT to 150 MHz
   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;
   EDIS;


// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

   /*
   // Interrupts that are used in this example are re-mapped to
   // ISR functions found within this file.
     EALLOW;  // This is needed to write to EALLOW protected register


     //Setup up INT for ePWM1
      PieVectTable.EPWM1_INT = &ePWM1_isr;
      //Enable EPWM1 INT in the PIE: Group 3 interrupt 1
      PieCtrlRegs.PIEIER3.bit.INTx1 =1;
      // enable INT3 (PIE Group 3)
      IER |=0x4;


     //Setup up INT for ePWM2
      PieVectTable.EPWM2_INT = &ePWM2_isr;
      //Enable EPWM2 INT in the PIE: Group 3 interrupt 2
      PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
      // enable INT3 (PIE Group 3)
      IER |=0x4;


      //Setup up INT for ADC
      PieVectTable.ADCINT = &adc_isr;
      //Enable ADC INT in the PIE: Group 1 interrupt 6
      PieCtrlRegs.PIEIER1.bit.INTx6 =1;
      // enable INT1 (PIE Group 1)
      IER |=0x1;


      //Setup up INT for GPIO17
      PieVectTable.XINT1 = &Xint1_isr;
      //Enable XINT1 in the PIE: Group 1 interrupt 4
      PieCtrlRegs.PIEIER1.bit.INTx4 =1;

      GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 17;  // GPIO 17
      XIntruptRegs.XINT1CR.bit.ENABLE = 1;
      XIntruptRegs.XINT1CR.bit.POLARITY = 0b10;

      // enable INT1 (PIE Group 1)
      IER |=0x1;


      //Setup up INT for GPIO48
      PieVectTable.XINT3 = &Xint3_isr;
      //Enable XINT3 in the PIE: Group 12 interrupt 1
      PieCtrlRegs.PIEIER12.bit.INTx1 =1;

      GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 16;   //GPIO 48
      XIntruptRegs.XINT3CR.bit.ENABLE = 1;
      XIntruptRegs.XINT3CR.bit.POLARITY = 0b10;

      // enable INT12 (PIE Group 12)
      IER |=0x800;

      //Global activation of Interrupt
      EINT;
      //Global Activation of DEBUG Interrupt
      ERTM;


      EDIS;    // This is needed to disable write to EALLOW protected registers

      */
config_gpio();
do{
    //write here
    affiche(3);
    affiche(8);
    affiche(7);
    affiche(5);
}while(1);
}//end of main()


