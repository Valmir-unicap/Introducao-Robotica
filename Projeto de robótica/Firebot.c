#include <Servo.h>
/*
* Define as postas analógicas para
* leitura dos sensores de temperatura.
*/
#define SensorTPM_EQD A0
#define SensorTPM_DRT A1
/*
* Definições para serem utilizadas no
* procedimento ci74HC595Write().
7
*/
#define pinSH_CP 9
#define pinST_CP 10
#define pinDS 11
#define qtdeCI 2
/*
* Define as postas correspondentes do CI 74HC595
* para acionar o motor 1 do lado direito
*/
#define mtDRT1 1
#define mtDRT1_Frente 3
#define mtDRT1_Tras 2
/*
* Define as postas correspondentes do CI 74HC595
* para acionar o motor 2 do lado direito
*/
#define mtDRT2 9
#define mtDRT2_Frente 5
#define mtDRT2_Tras 4
/*
* Define as postas correspondentes do CI 74HC595
* para acionar o motor 1 do lado esquerdo
8
*/
#define mtEQD1 0
#define mtEQD1_Frente 7
#define mtEQD1_Tras 6
/*
* Define as postas correspondentes do CI 74HC595
* para acionar o motor 2 do lado esquerdo
*/
#define mtEQD2 8
#define mtEQD2_Frente 11
#define mtEQD2_Tras 10
/*
* Definições de lado e direção que são utilizadas como argumentos
* nos procedimentos: motordRoda(lado, direção), motorPara(lado).
*/
#define ladoDRT 1
#define ladoEQD 2
#define frente 1
#define tras 2
/*
* Define motor servo na porta 8
*/
9
#define mtsDirecao 8
Servo servoDirecao;
/*
* variáveis globais utilizadas ao longo do programa
*/
static unsigned long tempoAtual;
void setup() {

 Serial.begin(19200);
 pinMode(pinSH_CP, OUTPUT);
 pinMode(pinST_CP, OUTPUT);
 pinMode(pinDS, OUTPUT);
 servoDirecao.attach(mtsDirecao);
}
void loop() {
 tempoAtual = millis();

 controleServo();

//se a temperatura dos dois lados estiver inferior a 100 graus
 //os motores de locomoção são acionados.
 if((temperaturaDRT() < 120) || (temperaturaEQD() < 120)){
10

 motordRoda(ladoDRT, frente);
 motordRoda(ladoEQD, frente);

 }else {
 //caso a temperatura ultrapasse 100 graus os motores são parados.
 motorPara(ladoDRT);
 motorPara(ladoEQD);
 }
}
/*
*Procedimentos que controla a motor servo responsável
*por orientar a direção do carrinho
*/
void controleServo() {
 static unsigned long tempoAnterior;
 int angulo = (calculoAnguloEQD() - calculoAnguloDRT()) + 93;
 if((tempoAtual - tempoAnterior) >= 15){
 servoDirecao.write(angulo);
 }
}
/*
* Calcula o angulo do servo motor baseado
11
* na temperatura do sensor do lado direito
*/
int calculoAnguloDRT() {
 int angulo, anguloBase = 180, temperaturaBase = 125, unidade;
 unidade = (anguloBase / temperaturaBase);
 angulo = unidade * temperaturaDRT();
 return angulo;
}
/*
* Calcula o angulo do servo motor baseado
* na temperatura do sensor do lado esquerdo
*/
int calculoAnguloEQD() {
 int angulo, anguloBase = 180, temperaturaBase = 125, unidade;
 unidade = (anguloBase / temperaturaBase);
 angulo = unidade * temperaturaEQD();
 return angulo;
}
//Função que interpreta os sinais elétricos do sensor e converte na escala de celsius
double temperaturaDRT() {
12
 double temperatura;

 //Faz a leitura na porta do sensor do lado direito
 //multiplicando pela DDP aplicada no sensor
 temperatura = analogRead(SensorTPM_DRT) * 5;

 //Divide-se a tensão obtida pela resolução de 1024 bits/s
 temperatura = temperatura / 1024;

 //ao adquirir a unidade de medida adequada, ocorre a conversão em uma escala conhecida;
 //nesse caso em celsius
 temperatura = (temperatura - 0.5) * 100;

 return temperatura;
}
//Função que interpreta os sinais elétricos do sensor e converte na escala de celsius
double temperaturaEQD() {
 double temperatura;

 temperatura = analogRead(SensorTPM_EQD) * 5;
temperatura = temperatura / 1024;
 temperatura = (temperatura - 0.5) * 100;
 return temperatura;
}
13
/*
*Procedimento para controlar o funcionamento dos motores de locomoção.
*O Procedimento recebe como parâmetro o lado do carrinho no qual
*os motores serão acionados e a direção (para frente ou para trás)
*/
void motordRoda(byte lado, byte direcao) {
 if (lado == ladoDRT) {
 ci74HC595Write(mtDRT1, HIGH);
 ci74HC595Write(mtDRT2, HIGH);
 if (direcao == frente) {
 ci74HC595Write(mtDRT1_Tras, LOW);
 ci74HC595Write(mtDRT2_Tras, LOW);
 ci74HC595Write(mtDRT1_Frente, HIGH);
 ci74HC595Write(mtDRT2_Frente, HIGH);
 } else {
 ci74HC595Write(mtDRT1_Frente, LOW);
 ci74HC595Write(mtDRT2_Frente, LOW);
 ci74HC595Write(mtDRT1_Tras, HIGH);
 ci74HC595Write(mtDRT2_Tras, HIGH);
 }
 } else if (lado == ladoEQD) {
 ci74HC595Write(mtEQD1, HIGH);
14
 ci74HC595Write(mtEQD2, HIGH);
 if (direcao == frente) {
 ci74HC595Write(mtEQD1_Tras, LOW);
 ci74HC595Write(mtEQD2_Tras, LOW);
 ci74HC595Write(mtEQD1_Frente, HIGH);
 ci74HC595Write(mtEQD2_Frente, HIGH);
 } else {
 ci74HC595Write(mtEQD1_Frente, LOW);
 ci74HC595Write(mtEQD2_Frente, LOW);
 ci74HC595Write(mtEQD1_Tras, HIGH);
 ci74HC595Write(mtEQD2_Tras, HIGH);
 }
 }
}
//Procedimento que desativa as portas do CI ponte H e faz com que os motores parem.
void motorPara(byte lado) {
 if (lado == ladoDRT) {
 ci74HC595Write(mtDRT1, LOW);
 ci74HC595Write(mtDRT2, LOW);
 } else {
 ci74HC595Write(mtEQD1, LOW);
15
 ci74HC595Write(mtEQD2, LOW);
 }
}
void ci74HC595Write(byte pino, bool estado) {
 static byte ciBuffer[qtdeCI];
 bitWrite(ciBuffer[pino / 8], pino % 8, estado);
 digitalWrite(pinST_CP, LOW); //Inicia a Transmissão
 digitalWrite(pinDS, LOW); //Apaga Tudo para Preparar Transmissão
 digitalWrite(pinSH_CP, LOW);
 for (int nC = qtdeCI - 1; nC >= 0; nC--) {
 for (int nB = 7; nB >= 0; nB--) {
 digitalWrite(pinSH_CP, LOW); //Baixa o Clock
 digitalWrite(pinDS, bitRead(ciBuffer[nC], nB) ); //Escreve o BIT
 digitalWrite(pinSH_CP, HIGH); //Eleva o Clock
 digitalWrite(pinDS, LOW); //Baixa o Data para Previnir Vazamento
 }
 }
 digitalWrite(pinST_CP, HIGH); //Finaliza a Transmissão
}
