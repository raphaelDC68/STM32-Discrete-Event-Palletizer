/*
 * main.h
 *
 *  Created on: 16 aout 2018
 *      Author: Laurent
 */

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

//Actuator

#define distrib_c                1
#define tapis_distrib_c          1<<1
#define blocage_entree_pal       1<<2
#define ouverture_porte          1<<3
#define poussoir                 1<<4
#define clamp                    1<<5
#define monter_ascenseur     	 1<<6
#define descendre_ascenseur      1<<8
#define ascenseur_to_limit       1<<9
#define distrib_pal         	 1<<10
#define charger_pal              1<<11
#define tapiscarton_vers_pal 	 1<<12
#define tourner_c           	 1<<13
#define decharger_palettiseur    1<<14
#define charger_palettiseur      1<<16
#define decharger_pal            1<<17
#define tapis_pal_vers_asc       1<<18
#define tapis_distrib_pal        1<<19
#define tapis_fin                1<<20
#define remover                  1<<21

//Sensor

#define c_distribue         1
#define c_envoye            1<<1
#define entree_palettiseur  1<<2
#define porte_ouverte       1<<3
#define limite_poussoir     1<<4
#define clamped             1<<5
#define asc_RDC             1<<6
#define asc_1               1<<8
#define asc_2               1<<9
#define sortie_pal          1<<10
#define limite_porte        1<<11
#define asc_mouvement       1<<12
#define entree_pal          1<<13
#define butee_c             1<<14



#include "stm32f0xx.h"


/* Global functions */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);





#endif /* APP_INC_MAIN_H_ */
