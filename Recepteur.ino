#include <SPI.h>
#include <RF24.h>
#include « nRF24L01.h »

RF24 radio(8,7); //création instance radio (pin ce,csn)

float rxbuffer[4] = {0, 0, 0, 0};
bool done = false;
float temperature;
float humidite;
int capteur;
float tension;

void setup()
{
	radio.begin();
	radio.setPALevel(RF24_PA_MAX); //puissance maximum
	radio.setChannel(0x20); //canal 32
	radio.setDataRate(RF24_1MBPS);
	radio.openReadingPipe(1, 0xF0F0F0F0F0LL);
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(15,15);
	radio.powerUp();
	radio.startListening();

	Serial.begin(9600);
}

void loop ()
{
	if (radio.available())
	{
		done = false;
		while (!done)
		{
			done = radio.read(&rxbuffer, sizeof(rxbuffer));
			capteur = (int)rxbuffer[0];
			Serial.print(« Pièce = « );
			switch (capteur){
				case 1:
				Serial.println(« Chambre 1″);
				break;
				case 2:
				Serial.println(« Chambre 2″);
				break;
				//case 3 : ….
				default:
				Serial.println(« Erreur réception ! »);
			}

			temperature = rxbuffer[1];
			Serial.print(« Temperature = « );
			Serial.println(temperature, 1);
			humidite = rxbuffer[2];
			Serial.print(« Humidite = « );
			Serial.println(humidite, 1);
			humidite = rxbuffer[3];
			Serial.print(« Tension pile = « );
			Serial.println(humidite);
		}
	}
}
