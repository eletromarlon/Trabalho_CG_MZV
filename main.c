#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SOIL/SOIL.h> //Biblioteca para trabalhar com texturas. Tem recursos para edição, leitura e processamento de texturas. Usar JPEG normal (não prograssivo)
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include "math.h"       //No linux se faz necessário a utilização dessas bibliotecas em separado para evitar erros. (Ou -lm no linker)
#include "stdbool.h"    //Também é necessário fazer algumas chamadas no linker do compilador na guia settings->compiler...
#include "objetosSolares.h"

float distanciaPrimitiva=400.0;     //Posição inicial da estrela da morte em relação a terra
float multiplicador = 0.9995;       //Indice gravitacional

/**Carregamento das texturas salvas na pasta resources */
GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(arquivo, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    if (idTextura == 0) {
        printf("Problemas na leitura das texturas -> SOIL: '%s' - '%s'\n", arquivo, SOIL_last_result());
    }
    return idTextura;
}

//---------------------------------------------------- Renderização ------------------------------------------------------------
void criaSphere(float radius, int stacks, int columns){ //Criar os corpos com parâmetros enviados para características de cada Objeto
    GLUquadric* quadObj = gluNewQuadric();      //Cria um objeto 3D, ou matematicamente, quádrico
    gluQuadricDrawStyle(quadObj, GLU_FILL);     //Desenha figuras com quadrados GLU_FILL para 'empate'
    gluQuadricNormals(quadObj, GLU_SMOOTH);     //Seleciona quais as normais desejadas para o objeto 3D
    gluQuadricTexture(quadObj, GL_TRUE);        //Habilita a utilização de texturas nos objetos
    gluSphere(quadObj, radius, stacks, columns);//Desenha umma esfera x com 3 parâmetros
    gluDeleteQuadric(quadObj);                  //Finaliza um objeto 3D
}
void planeta_terra(){                                   //Contrução do planeta Terra iluminado
	glEnable(GL_TEXTURE_2D);                            //
		glBindTexture(GL_TEXTURE_2D, earth.Texture);    //Seleciona uma textura para um objeto "texturizável"
    	glPushMatrix();                                 //
	        glRotatef(earth.Rotacao, 0, 1, 0);          //Rotação da terra - Pode modificar a inclinação e alterar o tipo do giro
	        glRotatef(90, 1, 0, 0);                     //Características do objeto como posição na vizualização *90 altere para inclinar
	        //glTranslated(a,b,c)                       //Se quisesse simular a tranlação da terra
	        criaSphere(100 , earth.Faces, earth.Faces); //Raio da terra e modelagem das faces altere o primeiro valor para terra maior
   		 glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void render_satelite(float gravidade){
	glEnable(GL_TEXTURE_2D);
		//Espaço
		glBindTexture(GL_TEXTURE_2D, space.Texture);
	    glPushMatrix();
	        glTranslatef(cursor.X, cursor.Y, cursor.Z);
	        criaSphere(7000, earth.Faces, earth.Faces);
	    glPopMatrix();
	    //Estrela da morte
	    glBindTexture(GL_TEXTURE_2D, death_star.Texture);
	    glPushMatrix();
	        glRotatef(death_star.Translacao, 0, 1, 0);
	        glTranslatef(gravidade, 0, 100);
	        glRotatef(death_star.Rotacao, 0, 1, 0);
	        glRotatef(-267, 1, 0, 0);
	        criaSphere(0.95 * surge_death_star, death_star.Faces, death_star.Faces);
	    glPopMatrix();
}
void iluminacao(){
	if(exibir_terra){
	    float luzPosicional0[] = {0.0, 0.0, 3.0, posicional};

	    glEnable(GL_LIGHT0);        //Faz com que a terra ilumine a estrela da morte (Ou qualquer objeto proximo)
	    glDisable(GL_LIGHTING);     //Iluminação própria da terra.
	    glPushMatrix();
	        glRotatef(luzBranca.X, 1.0, 0.0, 0.0);
	        glRotatef(luzBranca.Y, 0.0, 1.0, 0.0);
	        glLightfv(GL_LIGHT0, GL_POSITION, luzPosicional0);                      //Parametros de fonte clara de luz. Pode ter várias fontes de luz. Terra ilumina Death-Star
	        glTranslatef(luzPosicional0[0], luzPosicional0[1], luzPosicional0[2]);
	        glColor3f(difusa, difusa, difusa);
	        planeta_terra();
	    glPopMatrix();
	}else{
		glDisable(GL_LIGHT0);
	}
	glEnable(GL_LIGHTING);
}

void estadoCamera(){
	camera.X = 800 * sin(anguloCameraA) * cos(anguloCameraB);
    camera.Y = 800 * sin(anguloCameraA);
    camera.Z = 800 * cos(anguloCameraA) * sin(anguloCameraB);
    switch(modoCamera){
    	case 1:
            gluLookAt(cursor.X+camera.X, camera.Y, cursor.Z+camera.Z, cursor.X+0, 0, cursor.Z+0, 0, 1, 0);  //Visualização da cena - Ponto do olho; Ponto de referência de variação;
    	break;                                                                                              // Especifica o vetor do ponto de referencia
    	case 2:
            gluLookAt(0, 0, 1080, 0, 0, 0, 0, 1, 0);
    	break;
    }
}

void estadoExecucao(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    estadoCamera();
   	glMaterialfv(GL_FRONT, GL_SHININESS, matrizBrilho);
    glColor3f(1, 1, 1);
    iluminacao();
    if (distanciaPrimitiva <= 130){          //Inverter a força gravitacional gerando efeito estilingue
        multiplicador=1.0004;   //Efeito estilingue gravitacional com saída de orbita mais lenta (0,04%)
    }
    render_satelite(distanciaPrimitiva);
    distanciaPrimitiva=distanciaPrimitiva*multiplicador;       //Força gravitacional com 0,05% de atração
    glutSwapBuffers();
}

void estadoAtualizacao(int time){
	glutPostRedisplay();                            //Faz com que a janela seja atualizada de tempos em tempos utiliza glutMainLoop
    glutTimerFunc(time, estadoAtualizacao, time);   //Atualização da janela
}

//---------------------------------------------------- Processos de Execução ------------------------------------------------------------
void estadoRotacao(){
	death_star.Rotacao = death_star.Rotacao + 0.0f;         //Orbita em torno do eixo da estrela da morte 0=parado
	earth.Rotacao = earth.Rotacao + 0.1f;                   //2.4f //quanto maior o valor maior a rotação. Pode ser negativo tambem
	glutPostRedisplay();
}

void estadoTranslacao(){
	death_star.Translacao = death_star.Translacao + (-0.1); //0.12f; //Positivo se quiser rodar junto com a terra. (Dentro de parênteses para facilitar modificações)
	estadoRotacao();
	glutPostRedisplay();
}

void estadoTeclado(unsigned char key, int x, int y){
	switch(key){
		case '1':
			modoCamera = 1;
		break;
		case '2':
			modoCamera = 2;
		break;
		case 27:
			exit(0);
		break;
		case 'w':
			cursor.X--;
		break;
		case 'W':
			cursor.X--;
		break;
		case 's':
			cursor.X++;
		break;
		case 'S':
			cursor.X++;
		break;
		case 'a':
			cursor.Z--;
		break;
		case 'A':
			cursor.Z--;
		break;
		case 'd':
			cursor.Z++;
		break;
		case 'D':
			cursor.Z++;
		break;
		case 'f':
			exibir_terra = !exibir_terra;
		break;
		case 'F':
			exibir_terra = !exibir_terra;
		break;
		case 'e':
			if(surge_death_star == 0){
				surge_death_star = 10;
			} else{
				surge_death_star = 0;
			}
		break;
		case 'E':
			if(surge_death_star == 0){
				surge_death_star = 10;
			} else{
				surge_death_star = 0;
			}
		break;
	}
}

//---------------------------------------------------- Configurações da Aplicação ------------------------------------------------------------
/*Define as configurações da janela*/
void modeloJanela(int w, int h){
	janela.Largura = w;
	janela.Altura = h;

	glEnable(GL_DEPTH_TEST);
    glViewport (0, 0, w, h);                                    //definição da janela
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();                                           //chamada a matrix
    gluPerspective(60.0, (float)w/(float)h, 0.2, 2147483647.0); //Construção da perspectiva da janela
    glMatrixMode(GL_MODELVIEW);
}
/*Define as configurações da camera*/
void confCamera(int x, int y){
	float altX = x - mouse.X;
	float altY = y - mouse.Y;

	anguloCameraA = anguloCameraA + altX/150;
	anguloCameraB = anguloCameraB + altY/150;

	if(anguloCameraB > 180){
		anguloCameraB = 180;
	}

	mouse.X = x;
	mouse.Y = y;
}

/*Define estado inicial dos componentes*/
void defineBase(){
	glClearColor(0,0,0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Características de colocaração dos pixels gerados na renderização -> f R f G f B f A ==> 1 1 1 1 - A s0 k A A s0 k A A s0 k A A s0 k A

    /*Configurações da Luz*/
    difusa = 1;
	especular = 1;
	posicional = 1;
	global = 0.2;
	luzBranca.X = 0;
	luzBranca.Y = 0;

	/*Configurações do Cursor*/
	cursor.X = 0;
	cursor.Y = 0;
	cursor.Z = 0;

	/*Configurações do Mouse*/
	mouse.X = 0;
	mouse.Y = 0;
	mouse.Z = 0;

	//death_star
	death_star.Texture = carregaTextura("resources/textures/death_star.jpg");
	death_star.Translacao = 30;
	death_star.Rotacao = 10;
	death_star.TamanhoTranslacao = 166;
	death_star.Faces = 200;
	death_star.Estado = true;

	//TERRA
	earth.Texture = carregaTextura("resources/textures/earth.jpg");
	earth.Translacao = 90;
	earth.Rotacao = 90;
	earth.TamanhoTranslacao = 200;
	earth.Faces = 200;
	earth.Estado = true;

	//Espaço
	space.Texture = carregaTextura("resources/textures/space.jpg");     //Esse fica estático, não tem nenhum movimento

	/*Configurações do Material*/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matrizAD);           //Especificações de iluminação controladas pelos parâmetros
    glMaterialfv(GL_FRONT, GL_SPECULAR, matrizEspecular);               //
    glMaterialfv(GL_FRONT, GL_SHININESS, matrizBrilho);
	        glRotatef(-267, 1, 0, 0);               //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //Configurações da textura. Também pode ser usado 'glTextureParametri'
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //
}

int main(int argc, char* args[]){
    printf("\n\t\t\tTrabalho de CG - UFC - Crateús");
    printf("\n\n\tPlaneta terra sendo orbitado por \n\tum enorme satélite artificial chamado Star Death");
    printf("\n\n\tCréditos artísticos - Lucas Filme - Star Wars\n\n");
    printf("\n\tEstrela da Morte é uma estação \n\tespacial bélica criada pelo ");
    printf("\n\tImpério Galáctico na série \n\tcinematográfica de ficção científica Star Wars.");
    printf("\n\tÉ uma gigantesca estação \n\tespacial esférica, com 120 km de diâmetro");
    printf("\n\tPara facilitar a visualização, \n\tas proporções entre terra e estrela da morte");
    printf("\n\tForam ignorados, tornando \n\ta estrela da morte muito maior\n\n");

	printf("Commands:\n\tUse o mouse para alterar o angulo de visão (modo perspectiva)");
	printf("\n\tAperte 'F' para aparecer/desaparecer a terra");
	printf("\n\tAperte 'E' para aparecer/desaparecer a estrela da morte");
	printf("\n\tAperte '1' ou '2' para mudar o modo de camera (1 - Livre | 2 - Fixo)");
	printf("\n\tUse 'W', 'S', 'A' ou 'D' para mover a camera no modo livre");
	printf("\n\tAperte 'ESC' para sair\n");

	glutInit(&argc, args);
	glutInitContextVersion(1, 1); //Poderia usar uma outra versão como 3,2 -> Versão do OpenGL
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    /**Configurações da Janela */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //Modo
    glutInitWindowSize(1400, 840);//Tamanho da tela - Altere caso seu monitor não seja fullHD
    glutInitWindowPosition (0, 0);//Posição
    glutCreateWindow("Estrela da morte orbitando a terra"); //Nome da janela

	//Chama nossas funções principais
	glutDisplayFunc(estadoExecucao);            //Execução
	glutReshapeFunc(modeloJanela);              //Atualização de parametros da tela caso seja modificada pelo usuário
	glutTimerFunc(10, estadoAtualizacao, 10);   //Taxa de Atualização
	glutKeyboardFunc(estadoTeclado);            //Retorno de chamadas do teclado para a janela em uso
	glutPassiveMotionFunc(confCamera);          //Movimento de janela de acordo com o movimento do mouse (glutMotionFunc para uso com botão precionado)
	glutIdleFunc(estadoTranslacao);             //Controle de animação continua da janela -> Tarefas em segundo plano
	defineBase();                               //Processos Iniciais
	glutMainLoop();                             //Loop para glutPostRedisplay - atualização da tela

	return 0;
}
