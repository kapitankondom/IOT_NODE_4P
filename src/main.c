#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "definitions.h"

#define ADRESA 0x1C

char text[100];

// Vypis pres UART
void vypis(char *zprava)
{
    while (SERCOM1_USART_Write(zprava, strlen(zprava)) == false)
    {
    }

    while (SERCOM1_USART_TransmitComplete() == false)
    {
    }
}

// Cteni registru ze senzoru
uint16_t ctiRegistr(uint8_t registr)
{
    uint8_t data[2] = {0, 0};

    // Poslani adresy registru a precteni 2 bajtu
    if (SERCOM0_I2C_WriteRead(ADRESA, &registr, 1, data, 2) == false)
    {
        return 0xFFFF;
    }

    // Cekani na dokonceni komunikace
    while (SERCOM0_I2C_IsBusy())
    {
    }

    // Kontrola chyby
    if (SERCOM0_I2C_ErrorGet() != SERCOM_I2C_ERROR_NONE)
    {
        return 0xFFFF;
    }

    // Spojeni dvou bajtu
    uint16_t hodnota = (data[0] << 8) | data[1];

    return hodnota;
}

int main(void)
{
    SYS_Initialize(NULL);

    uint16_t vyrobce;
    uint16_t zarizeni;
    uint16_t teplotaRaw;

    int teplota;
    int cela;
    int desetinna;

    vypis("\r\nStart programu\r\n");

    // Kontrola senzoru
    vyrobce = ctiRegistr(0x06);
    zarizeni = ctiRegistr(0x07);

    if (vyrobce == 0x0054 && (zarizeni >> 8) == 0x04)
    {
        vypis("MCP9808 nalezen\r\n");
    }
    else
    {
        vypis("MCP9808 nenalezen\r\n");
    }

    sprintf(text, "Vyrobce: 0x%04X\r\n", vyrobce);
    vypis(text);

    sprintf(text, "Zarizeni: 0x%04X\r\n", zarizeni);
    vypis(text);

    while (1)
    {
        // Precteni teploty
        teplotaRaw = ctiRegistr(0x05);

        if (teplotaRaw != 0xFFFF)
        {
            // Prevod na setiny stupne C
            teplota = (teplotaRaw & 0x0FFF) * 100 / 16;

            // Zaporne teploty
            if (teplotaRaw & 0x1000)
            {
                teplota = teplota - 25600;
            }

            cela = abs(teplota) / 100;
            desetinna = abs(teplota) % 100;

            if (teplota < 0)
            {
                sprintf(text, "Teplota: -%d.%02d C\r\n",
                        cela, desetinna);
            }
            else
            {
                sprintf(text, "Teplota: %d.%02d C\r\n",
                        cela, desetinna);
            }

            vypis(text);
        }
        else
        {
            vypis("Chyba senzoru\r\n");
        }

        // Priblizna prodleva mezi merenimi
        for (volatile int i = 0; i < 1000000; i++)
        {
        }

        SYS_Tasks();
    }

    return 0;
}