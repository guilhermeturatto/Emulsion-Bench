/*
*   UNIVERSIDADE FEDERAL DE SANTA CATARINA
*      Centro Tecnologico de Joinville
*
*           Guilherme Turatto
*               23/05/2022
*/

#ifndef HALL_H_
#define HALL_H_

//Endereco dos inversores de frequencia na rede modbus
#define ADDR_INVERSOR_1          0x01
#define ADDR_INVERSOR_2          0x02

//Endereco dos registradores dos inversores de frequencia CFW300 (2bytes)
#define P680                0x02A8  //RO - Monitoramento do estado do dispositivo: 15: Em Falha; 14: Reservado; 13: Subtensao; 12: LOC/REM; 11: JOG; 10: Sentido de Giro; 9: Habilitado Geral; 8: Motor Girando; 7: Em Alarme; 6: Em Modo Config; 5: 1a ou 2a Rampa; 0-4: Reservado;
#define P681                0x02A9  //RO - Velocidade do motor em 13 Bits ( 0 - 8192[nominal] ): (Freq. nominal ajustada em P403)
#define P682                0x02AA  //-- - Palava de Controle via Serial (configurar dispositivo em P105 e P220 ate P228): 15-8: Reservado; 7: Reset de Falhas; 6: Reservado; 5: Usa 2a Rampa; 4: LOC/REM; 3: JOG; 2: Sentido de giro; 1: Habilita Geral; 0: Gira/Para;
#define P683                0x02AB  //-- - Referencia de velocidade via Serial (0 - 8192) = (0 - Hz nominal)


//Palavras pre-definidas
#define VEL_NOMI                0x2000 //8192       (P683)
#define VEL_ZERO                0x0000 //0          (P683)

#define MOTOR_ON                0x0017 //00010111   (P682) - 4: Modo remoto; 2: Sentido de giro igual da referencica; 1: Habilita Geral; 0: Gira motor por rampa;
#define MOTOR_OFF               0x0016 //00010110   (P682) - 4: Modo remoto; 2: Sentido de giro igual da referencica; 1: Habilita Geral; 0: Para motor por rampa;


#endif /* HALL_H_ */
