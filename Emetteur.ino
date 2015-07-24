#include « DHT.h »
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <RF24.h>

#define DHTPIN 2 // entrée capteur dht22
#define DHTTYPE DHT22
#define DHT22_POWER 3 //alimentation du capteur DHT22

#define PIN_PILE 1 // entrée analogique

DHT dht(DHTPIN, DHTTYPE);

//variables radio
RF24 radio(8,7); //création instance radio (pin ce,csn)
float txbuffer[4] = {1, 0, 0, 0}; // l’indice 0 du tableau représente le N° du themometre ici le 1
//variable température
float temperature;
float humidite;
//variables tension
const float coeff_division = 2.0; // constante pont diviseur
float real_bat = 0;
unsigned int raw_bat;

ISR (WDT_vect)
{
	wdt_disable(); //désactive le watchdog
}

void mywatchdogenable()
{
	MCUSR = 0;
	WDTCSR = _BV (WDCE) | _BV (WDE);
	WDTCSR = _BV (WDIE) | _BV (WDP3) | _BV (WDP0); //délai de 8 secondes
	wdt_reset();
	//ADCSRA = 0; //désactive ADC
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	MCUCR = _BV (BODS) | _BV (BODSE);
	MCUCR = _BV (BODS);
	sleep_cpu();
	sleep_disable();
}

void setup()
{
	dht.begin();

	pinMode(DHT22_POWER, OUTPUT);

	pinMode(PIN_PILE,INPUT);

	radio.begin();
	radio.setPALevel(RF24_PA_MAX); //puissance maximum
	radio.setChannel(0x20); //canal 32
	radio.setDataRate(RF24_1MBPS);
	radio.openWritingPipe(0xF0F0F0F0F0LL);
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(15,15);
}

void loop ()
{
	digitalWrite(DHT22_POWER, HIGH); //alimente le capteur DHT22
	delay(1000);
	temperature = dht.readTemperature();
	humidite = dht.readHumidity();
	delay(100);
	digitalWrite(DHT22_POWER, LOW); //alimente le capteur DHT22
	txbuffer[1] = temperature;
	txbuffer[2] = humidite;

	/* Mesure de la tension brute */
	raw_bat = analogRead(PIN_PILE);
	/* Calcul de la tension réel */
	real_bat = ((raw_bat * (3.3 / 1024)) * coeff_division);
	txbuffer[3] = real_bat ;

	radio.powerUp(); //alimente le module nrf24l01+
	delay(1000);
	radio.write(&txbuffer, sizeof(txbuffer));
	delay(1000);
	radio.powerDown(); //arrêt de l’alimentation du module nrf24l01+

for (int i=0; i < 35; i++) //mise en veille pendant ~~ 5mn secondes
	mywatchdogenable();
}
