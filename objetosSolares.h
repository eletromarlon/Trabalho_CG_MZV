#ifndef OBJETOSSOLARES_H_INCLUDED
#define OBJETOSSOLARES_H_INCLUDED

typedef struct{
	float Translacao;
	float Rotacao;
	int Faces;
	int TamanhoTranslacao;
	GLuint Texture;
	bool Estado;
}Corpo;

typedef struct{
	int X;
	int Y;
	int Z;
}EixoINT;

typedef struct{
	float X;
	float Y;
	float Z;
}EixoFLOAT;

typedef struct{
	float Largura;
	float Altura;
}Janela;

//Variaveis dos Corpos
Corpo death_star;
Corpo earth;
Corpo space;

//Variaveis de Luz
float difusa;
float especular;
float posicional;
float global;
float matrizAD[] = {1.0, 1.0, 1.0, 1.0};
float matrizEspecular[] = {1.0, 1.0, 1,0, 1.0};
float matrizBrilho[] = {50};
EixoFLOAT luzBranca;


//Variaveis de Controle
int surge_death_star = 0;//Aparecer a estrela da morte
EixoINT cursor;
EixoINT mouse;
Janela janela;
bool exibir_terra = true;

//Variaveis da Camera
int modoCamera = 2;
float anguloCameraA = 90;
float anguloCameraB = 0;
EixoFLOAT camera;





#endif // OBJETOSSOLARES_H_INCLUDED
