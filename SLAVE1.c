/* 
 * File:   SLAVE1.c
 * Author: diego
 *
 * Created on 12 de mayo de 2022, 07:12 PM
 */


// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#include <xc.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000      //Frecuencia Oscilador
#define FLAG_SPI 0xFFF
/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
uint8_t CONTADOR = 0;          //Contador de botones
char ULTIMO = 0;               //Registro de ultima data

/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    if (INTCONbits.RBIF){               // Interrupción de cambio en PORTB
        if (!PORTBbits.RB0)             // Cambio en RB0
            {++CONTADOR;                // Aumento CONTADOR
            }
        else if (!PORTBbits.RB1)         // Cambio en RB1
            {--CONTADOR;                 // Decrece CONTADOR
            }
        INTCONbits.RBIF = 0;            //Limpieza bandera cambio en PORTB
    }
    else if (PIR1bits.SSPIF){           // Lectura comunicación SPI
        
        ULTIMO = SSPBUF;                // Se carga la información de la comunicación del buffer en ULTIMO
        if (ULTIMO!= FLAG_SPI){         // Se revisa si es señal de comunicación o información   
            SSPBUF = CONTADOR;          // Se ingresan los valores de Contador de botones en el buffer
        }
        PIR1bits.SSPIF = 0;             // Limpieza de bandera de SPI
    }
    return;
}


/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){
      
    }
    return;
}
/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    
    //Configuración Entradas/Salidas
    ANSEL = 0b00000000;         // Entradas análogas
    ANSELH = 0;                 // I/O digitales
    TRISA = 0b00100000;         // SS y RA0 como entradas       
    PORTA = 0;                  // Limpieza PORTA
    TRISD = 0;                  // Habilitaciónde PORTC como salida
    PORTD = 0;                  // Limpieza PORTD

    //Configuración de Pull-up en PORTB
    TRISBbits.TRISB0 = 1;       // Entradas RB0 y RB1
    TRISBbits.TRISB1 = 1;
    OPTION_REGbits.nRBPU = 0;   // Encendido de señal por Pull-up
    WPUBbits.WPUB0 = 1;         // Hablitación RB0
    WPUBbits.WPUB1 = 1;         // Hablitación RB1
    IOCBbits.IOCB0 = 1;         // Habilitación interrupciones RB0
    IOCBbits.IOCB1 = 1;         // Habilitación interrupciones RB1
    PORTB = 0;                  // Limpieza PORTB
    INTCONbits.RBIF = 0;        // Limpieza bandera PORTB
    
    //Configuración Oscilador
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Reloj interno
    
    //Configuración de SPI (SLAVE)
    TRISC = 0b00011000; // -> SDI y SCK entradas, SD0 como salida
    PORTC = 0;
    // SSPCON <5:0>
    SSPCONbits.SSPM = 0b0100;   // -> SPI Esclavo, SS entrada o salida
    SSPCONbits.CKP = 0;         // -> Reloj inactivo en 0
    SSPCONbits.SSPEN = 1;       // -> Habilitamos pines de SPI
    // SSPSTAT<7:6>
    SSPSTATbits.CKE = 1;        // -> Dato enviado cada flanco de subida
    SSPSTATbits.SMP = 0;        // -> Dato al final del pulso de reloj (SLAVE)
    // Configuraciones de interrupciones
    PIR1bits.SSPIF = 0;         // Limpieza bandera SPI
    PIE1bits.SSPIE = 1;         // Habilitación interrupciones SPI
    INTCONbits.GIE = 1;         // Habilitación interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitación interrupciones de perifericos
  
}

