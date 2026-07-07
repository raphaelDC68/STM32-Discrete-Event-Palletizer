/*
 * main.c
 *
 *  Created on: 16 ao�t 2018
 *      Author: Laurent
 */


#include "stm32f0xx.h"
#include "main.h"
#include "bsp.h"
#include "factory_io.h"
#include "delay.h"

// States entity
int State_entity_1 = 1;
int State_entity_2 = 1;
int State_entity_3 = 1;
int State_entity_4 = 1;
int State_entity_5 = 1;
//int State_entity_6 = 1;

// Variable
uint32_t buffer_Actuator;
int compteur_poussoir=0;
int cmp_c=0;
int cmp_b=0;
int cmp_p=0;
int cmp_a=0;
// Synchronization
int Synch_a_1_wait = 0;

int Synch_1_2 = 0;
int Synch_2_1 = 0;
int Synch_3_1=0;
int Synch_3_2=0;
int Synch_2_3=0;
int Synch_3_4 = 0;

int Synch_5_4 =0;


// Memory rings
int A_Premier_C = 1;
int A_Deuxieme_C = 0;

int pousser = 0;

/*
 * Local Static Functions
 */

static uint8_t 	SystemClock_Config	(void);


/*
 * Project Entry Point
 */

int main(void)
{
	// Configure System Clock for 48MHz from 8MHz HSE
	SystemClock_Config();

	// Initialize LED and USER Button
	BSP_LED_Init();
	BSP_PB_Init();

	// Initialize Debug Console
	BSP_Console_Init();
	my_printf("\r\nConsole Ready!\r\n");
	my_printf("SYSCLK = %d Hz\r\n", SystemCoreClock);

	// Read all states from the scene
	FACTORY_IO_update();

	// Wait here for user button
	while(BSP_PB_GetState() == 0);

	// Start conveyor A[0] = 1
	//my_printf("Starting Conveyor\r\n");
	//FACTORY_IO_Actuators_Set(0x00000001);

	// Wait for sensor S[1] = 0 (optical barrier)
	//my_printf("Waiting for sensor...\r\n");
	//while (FACTORY_IO_Sensors_Get(0x00000002) == 1);

	// Stop conveyor A[0] = 0
	//my_printf("Stop!\r\n");
	//FACTORY_IO_Actuators_Set(0x00000000);


	// Loop forever
	while(1)
	{
		// LED blinking
		BSP_LED_Toggle();
		delay_ms(100);

		// Init buffer
		buffer_Actuator = 0;

		buffer_Actuator |= tapis_distrib_c | tapiscarton_vers_pal | charger_palettiseur |tapis_pal_vers_asc | tapis_distrib_pal | tapis_fin | blocage_entree_pal;


	    // Machine 1

		switch (State_entity_1)
		{

		case 1: // distrib_c
			buffer_Actuator |= distrib_c;
			if (FACTORY_IO_Sensors_Get(c_distribue) == 0)
			{
				State_entity_1 ++;
			}
			break;

		case 2://envoi carton
			buffer_Actuator &= ~distrib_c;
			if (FACTORY_IO_Sensors_Get(c_envoye) == 0)
			{
				State_entity_1 ++;
			}
			break;

		case 3:      //attente
			if (A_Premier_C == 1 && A_Deuxieme_C == 0)  //attente 1er carton
			{
				State_entity_1 = 1;

				A_Premier_C = 0;
				A_Deuxieme_C = 1;
			}

			else if (A_Premier_C == 0 && A_Deuxieme_C == 1)  //attente 2eme carton
			{
				Synch_1_2 = 1;

				A_Premier_C = 1;
				A_Deuxieme_C = 0;

				State_entity_1 ++;

			}
			break;

		case 4: // wait
			if (Synch_3_1)
			{
				Synch_3_1=0;
				State_entity_1 = 1;
			}
			break;


		}

		// Machine 2

		switch (State_entity_2)
		{

		case 1: //transit_1_2
			if(Synch_1_2==1 && FACTORY_IO_Sensors_Get(entree_palettiseur) == 0) //entree paletiseur OFF
			{
				State_entity_2 ++;
				Synch_1_2=0;
			}
			break;


		case 2://att carton 2
			if( FACTORY_IO_Sensors_Get(entree_palettiseur) == 1) //entree paletiseur ON
			{
				State_entity_2 ++;
			}
			break;

		case 3: //attente_carton_2
			if(FACTORY_IO_Sensors_Get(entree_palettiseur) == 0) //entree paletiseur OFF 2
			{
				State_entity_2 ++;
				Synch_2_3=1;

			}
		break;

		case 4: //deblocage entree palettiseur
			Synch_3_2=0;
			buffer_Actuator &= ~(blocage_entree_pal);
			break;
		}


		// Machine 3
		switch (State_entity_3)
		{

		case 1://Att carton

			if(Synch_2_3==1 && FACTORY_IO_Sensors_Get(butee_c) == 1)
			{
				State_entity_3 ++;
				Synch_3_2=1;
			}

		break;

		case 2: //Poussoir
			buffer_Actuator |= poussoir;
			if(FACTORY_IO_Sensors_Get(limite_poussoir) == 1)
			{
				compteur_poussoir ++;
				State_entity_3 ++;
			}
		break;

		case 3: // attente_triple poussée
			if(compteur_poussoir==1)
			{
				State_entity_3=1;
			}
			else if(compteur_poussoir==2)
			{
				State_entity_3=1;
			}
			else if(compteur_poussoir==3)
			{
				State_entity_3=1;
				compteur_poussoir=0;
				Synch_3_4=1;
			}
			break;
		}

		// Machine 4
		switch (State_entity_4)
		{
		case 1:
			if(compteur_poussoir == 3)
			{
				State_entity_4++;
				compteur_poussoir = 0;
			}
		break;

		case 2:
			buffer_Actuator |= clamp;
			State_entity_4++;
			break;

		case 3:
			if(FACTORY_IO_Sensors_Get(limite_porte) == 1 )
			{
				buffer_Actuator |= ouverture_porte;
			}
		break;

		case 4:
			if(FACTORY_IO_Sensors_Get(limite_porte) == 1)
			{
				State_entity_4=1;
			}
		break;
		}

		// Machine 5
		switch (State_entity_5)
		{
		case 1:
			buffer_Actuator |= (distrib_pal);
			State_entity_5++;
			break;


		case 2:
			if(FACTORY_IO_Sensors_Get(entree_pal) == 1)
			{
				buffer_Actuator |= (charger_pal);

			}
			if(FACTORY_IO_Sensors_Get(sortie_pal) == 1)
				State_entity_5++;
		break;

		case 3:
			if(FACTORY_IO_Sensors_Get(sortie_pal) == 1)
			{

				State_entity_5++;
			}
			break;

		case 4:
			buffer_Actuator |= (ascenseur_to_limit ) | (monter_ascenseur);
			State_entity_5++;
		break;

		case 5:
			if (FACTORY_IO_Sensors_Get(asc_1) == 1)
			{
				State_entity_5++;
				buffer_Actuator &= ~(monter_ascenseur);
				Synch_5_4=1; //ouverture de la porte cf machine 4
			}
		case 6:
			buffer_Actuator |= (ascenseur_to_limit ) | (descendre_ascenseur);
			State_entity_5=1;

		}


		//Sending actuators states
		FACTORY_IO_Actuators_Set(buffer_Actuator);

		//Delay
		delay_ms(100);
	}
}



/*
 * 	Clock configuration for the Nucleo STM32F072RB board
 * 	HSE input Bypass Mode 			-> 8MHz
 * 	SYSCLK, AHB, APB1 				-> 48MHz
 *  PA8 as MCO with /16 prescaler 	-> 3MHz
 */

static uint8_t SystemClock_Config()
{
	uint32_t	status;
	uint32_t	timeout;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (1);	// HSE error


	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 <<RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 <<RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC-> CR |= RCC_CR_PLLON;

	// Wait until PLL is ready
	timeout = 1000;

	do
	{
		status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((status == 0) && (timeout > 0));

	if (timeout == 0) return (2);	// PLL error


	// Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	//Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency (required for high speed)
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	timeout = 1000;

	do
	{
		status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	if (timeout == 0) return (3);	// SW error


	// Set MCO source as SYSCLK (48MHz)
	RCC->CFGR &= ~RCC_CFGR_MCO_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOSEL_SYSCLK;

	// Set MCO prescaler to /16 -> 3MHz
	RCC->CFGR &= ~RCC_CFGR_MCOPRE_Msk;
	RCC->CFGR |=  RCC_CFGR_MCOPRE_DIV16;

	// Enable GPIOA clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA8 as Alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk;
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER8_Pos);

	// Set to AF0 (MCO output)
	GPIOA->AFR[1] &= ~(0x0000000F);
	GPIOA->AFR[1] |=  (0x00000000);

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
	return (0);
}
