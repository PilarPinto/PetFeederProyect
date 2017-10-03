#include "soc-hw.h"

uart_t       *uart0       = (uart_t *)       0x20000000;
timerH_t     *timer0      = (timerH_t *)     0x30000000;
//gpio_t       *gpio0       = (gpio_t *)       0x40000000;
i2c_t        *i2c0        = (i2c_t *)        0x40000000;
SK6812RGBW_t *SK6812RGBW0 = (SK6812RGBW_t *) 0x60000000;

isr_ptr_t isr_table[32];

/***************************************************************************
 * IRQ handling
 */
void isr_null(void)
{
}

void irq_handler(uint32_t pending)
{     
    uint32_t i;

    for(i=0; i<32; i++) {
        if (pending & 0x00000001){
            (*isr_table[i])();
        }
        pending >>= 1;
    }
}

void isr_init(void)
{
    int i;
    for(i=0; i<32; i++)
        isr_table[i] = &isr_null;
}

void isr_register(int irq, isr_ptr_t isr)
{
    isr_table[irq] = isr;
}

void isr_unregister(int irq)
{
    isr_table[irq] = &isr_null;
}

/***************************************************************************
 * TIMER Functions
 */
uint32_t tic_msec;

void mSleep(uint32_t msec)
{
    uint32_t tcr;

    // Use timer0.1
    timer0->compare1 = (FCPU/1000)*msec;
    timer0->counter1 = 0;
    timer0->tcr1 = TIMER_EN;

    do {
        //halt();
         tcr = timer0->tcr1;
     } while ( ! (tcr & TIMER_TRIG) );
}

void uSleep(uint32_t usec)
{
    uint32_t tcr;

    // Use timer0.1
    timer0->compare1 = (FCPU/1000000)*usec;
    timer0->counter1 = 0;
    timer0->tcr1 = TIMER_EN;

    do {
        //halt();
         tcr = timer0->tcr1;
     } while ( ! (tcr & TIMER_TRIG) );
}

void tic_isr(void)
{
    tic_msec++;
    timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;
}

void tic_init(void)
{
    tic_msec = 0;

    // Setup timer0.0
    timer0->compare0 = (FCPU/10000);
    timer0->counter0 = 0;
    timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;

    isr_register(1, &tic_isr);
}


/***************************************************************************
 * UART Functions
 */
void uart_init(void)
{
    //uart0->ier = 0x00;  // Interrupt Enable Register
    //uart0->lcr = 0x03;  // Line Control Register:    8N1
    //uart0->mcr = 0x00;  // Modem Control Register

    // Setup Divisor register (Fclk / Baud)
    //uart0->div = (FCPU/(57600*16));
}

char uart_getchar(void)
{   
    while (! (uart0->ucr & UART_DR)) ;
    return uart0->rxtx;
}

void uart_putchar(char c)
{
    while (uart0->ucr & UART_BUSY) ;
    uart0->rxtx = c;
}

void uart_putstr(char *str)
{
    char *c = str;
    while(*c) {
        uart_putchar(*c);
        c++;
    }
}

/***************************************************************************
 * SK6812RGBW Functions
 */
void SK6812RGBW_init(void)
{
    SK6812RGBW_nBits(35*32);//35 Leds
    SK6812RGBW_source(0);
    SK6812RGBW_rgbw(0x00000000);
    SK6812RGBW_rgbw(0x00000000);
}

uint32_t SK6812RGBW_busy(void)
{
    return SK6812RGBW0->busy;
}

void SK6812RGBW_rgbw(uint32_t rgbw)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->rgbw = rgbw;
}

void SK6812RGBW_nBits(uint32_t nBits)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->nBits = nBits;
}

void SK6812RGBW_source(uint32_t source)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->source = source;
}

void SK6812RGBW_ram(uint32_t color, uint32_t add)
{
  uint32_t   *wram   = (uint32_t *)    (0x60000000 + 0x1000 + add);
  *wram = color;
}

void SK6812RGBW_ram_w(void)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->rgbw = 0;
}

/*i2c Functions*/ //i2c_addr(0x00000068)



void i2c_addr(uint32_t addr) {
	i2c0->addr = addr;

/*
	uint32_t RGBW = 0x00ff0000; //rojo
    	uint32_t i = 1;
    	SK6812RGBW_nBits(i * 32); //i Leds
        SK6812RGBW_rgbw(RGBW);
	mSleep(1000);
*/
}


void i2c_rw(uint32_t rw) {
	i2c0->rw = rw;
/*
	if(rw == 1) {

		uint32_t RGBW = 0x0000ff00; //verde
    		uint32_t i = 3;
        	SK6812RGBW_nBits(i * 32); //i Leds
        	SK6812RGBW_rgbw(RGBW);
		} else { //si rw = 0
			uint32_t RGBW = 0x000f00f0; //morado
    			uint32_t i = 5;
        		SK6812RGBW_nBits(i * 32); //i Leds
        		SK6812RGBW_rgbw(RGBW);
			}
	mSleep(1000);
*/
}


void  i2c_dataWr(uint32_t data_wr){
	i2c0->data_wr = data_wr;
/*
	uint32_t RGBW = 0x0000ff00; //azul
    	uint32_t i = 7;
    	SK6812RGBW_nBits(i * 32); //i Leds
        SK6812RGBW_rgbw(RGBW);
	mSleep(3000);
*/
}


uint32_t i2c_dataRd(void){
while(i2c_busy());
	return i2c0-> data_rd;
	
}


uint32_t bcd2bin (uint32_t val) { 
return val - 6 * (val >> 4); 
}


uint32_t bcdToDec(uint32_t bcdByte) {
	return (((bcdByte & 0xf0) >> 4)* 10) + (bcdByte & 0x0f);
}


void i2c_enable(uint32_t ena) {
	i2c0->ena = ena;
/*
	uint32_t RGBW = 0xff000000; //verde
	uint32_t i = 20;
    	SK6812RGBW_nBits(i * 32); //i Leds
        SK6812RGBW_rgbw(RGBW);
	mSleep(1000);
	SK6812RGBW_init();
*/
}



uint32_t i2c_busy(void) {
/*

	uint32_t RGBW = 0x000000ff; //blanco
	uint32_t i = 30;
    	SK6812RGBW_nBits(i * 32); //i Leds
        SK6812RGBW_rgbw(RGBW);
	mSleep(3000);
	SK6812RGBW_init();
*/
	return i2c0->busy;
}



