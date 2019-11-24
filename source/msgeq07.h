#ifndef MSGEQ07_H_
#define MSGEQ07_H_

#define RESET_PIN 7U
#define RESET_PORT PORTC
#define RESET_GPIO GPIOC
#define RESET_CLOCK kCLOCK_PortC

#define STROBE_PIN 8U
#define STROBE_PORT PORTE
#define STROBE_GPIO GPIOE
#define STROBE_CLOCK kCLOCK_PortE

void beginMsgeq07();
typedef void (*msgeq07_callback)(uint32_t spectrum[]);
void getFrequency(msgeq07_callback);
void initMsgeq07();

#endif
