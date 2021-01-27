// *********************************************************************
// Alunos: HECTOR BATISTA RIBEIRO e IAN LUCCAS ARAUJO
// Controles do Personagem 1:
// A - move para a esquerda.
// D - move para a direita.
// W - pula.
// Espaço - ataca.
// Controles do Personagem 2:
// Seta da esquerda - move para a esquerda.
// Seta da direita - move para a direita.
// Seta de cima - pula.
// Backspace - ataca.
// Alterar a musica:
// F1 - Música do Ryu - Nome: Street Fighter Alpha 2 Ryu Theme
// F2 - Música do Ken - Nome: Street Fighter Alpha 2 Ken Theme
// *********************************************************************
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>


#define Width 900 // Largura da janela
#define Height 600 // Altura da janela
#define QUANT_BLOCOS 10 // Quantidade de blocos de cenario
#define VIDA_RYU 100 // Quantidade de vida do Ryu
#define VIDA_KEN 100 // Quantidade de vida do Ken
#define DANO_RYU 20 // Valor do dano do ataque do Ryu
#define DANO_KEN 20 // Valor do dano do ataque do Ken
#define RYU_X 0.055 // Valor para controlar na horizontal o sprite dentro do sprite sheet do Ryu
#define RYU_Y 0.17 // Valor para controlar na vertical o sprite dentro do sprite sheet do Ryu
#define KEN_X 0.14 // Valor para controlar na horizontal o sprite dentro do sprite sheet do Ken
#define KEN_Y 0.1 // Valor para controlar na vertical o sprite dentro do sprite sheet do Ken

// Estrutura usada nos elementos do jogo
struct Bloco{
	float x;
	float y;
	float comp;
	float alt;
};

// Funcao para carregar textura
GLuint loadTexture(const std::string&fileName){
	SDL_Surface *imagem = IMG_Load(fileName.c_str()); // carrega a imagem
	SDL_DisplayFormatAlpha(imagem); // formato alpha
	unsigned object(0); // cria a textura
	glGenTextures(1, &object); // gera textura
	glBindTexture(GL_TEXTURE_2D, object); // tipo de textura
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagem->w, imagem->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagem->pixels);
	SDL_FreeSurface(imagem);
	return object;
}

// Funcao para detectar colisao
bool colisao(Bloco A, Bloco B){
	if(A.y+A.alt < B.y)
		return false;
	else if(A.y > B.y+B.alt)
		return false;
	else if(A.x+A.comp <= B.x) 
		return false;
	else if(A.x >= B.x+B.comp)
		return false;
		
	return true;
}

// Funcao para desenhar um personagem
void desenhaPersonagem(Bloco A, unsigned int textura, float tex_X1, float tex_X2, float tex_Y1, float tex_Y2){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,textura);

	glTranslatef(A.x,A.y,0);
	glBegin(GL_QUADS);
	glTexCoord2d(tex_X1,tex_Y1);
	glVertex2f(0, 0);
	glTexCoord2d(tex_X2,tex_Y1);
	glVertex2f(A.comp, 0);
	glTexCoord2d(tex_X2,tex_Y2);
	glVertex2f(A.comp, A.alt);
	glTexCoord2d(tex_X1,tex_Y2);
	glVertex2f(0, A.alt);
	glEnd();
	
	glTranslatef(-A.x,-A.y,0);
	glDisable(GL_TEXTURE_2D);
}

// Funcao para desenhar o projetil do ataque
void desenhaProjetil(Bloco A, unsigned int textura, float tex_X1, float tex_X2, float tex_Y1, float tex_Y2){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,textura);
	
	glTranslatef(A.x,A.y,0);
	glBegin(GL_QUADS);
	glTexCoord2d(tex_X1,tex_Y1);
	glVertex2f(0, -A.alt);
	glTexCoord2d(tex_X2,tex_Y1);
	glVertex2f(A.comp, -A.alt);
	glTexCoord2d(tex_X2,tex_Y2);
	glVertex2f(A.comp, A.alt);
	glTexCoord2d(tex_X1,tex_Y2);
	glVertex2f(0, A.alt);
	
	glEnd();
	glTranslatef(-A.x,-A.y,0);
	glDisable(GL_TEXTURE_2D);
}

// Funcao para desenhar o cenario
void desenhaCenario(Bloco blocos[QUANT_BLOCOS], float vida_per, float vida_ini){
	// BLOCOS DO CENARIO
	glBegin(GL_QUADS);
	glColor3f(0,0,1);
	for(int i = 0; i < QUANT_BLOCOS; i++){
		glVertex2f(blocos[i].x, blocos[i].y);
		glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y);
		glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y + blocos[i].alt);
		glVertex2f(blocos[i].x, blocos[i].y + blocos[i].alt);
	}
	glEnd();
	
	// BARRA DE VIDA DO RYU
	glTranslatef(Width*0.01,Height*0.05,0);
	glColor3f(0,1,0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(vida_per*2, 0);
	glVertex2f(vida_per*2, Height*0.05);
	glVertex2f(0, Height*0.05);
	glEnd();
	glTranslatef(-Width*0.01,-Height*0.05,0);
	
	// BARRA DE VIDA DO KEN
	glTranslatef(Width*0.99,Height*0.05,0);
	glColor3f(0,1,0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(-vida_ini*2, 0);
	glVertex2f(-vida_ini*2, Height*0.05);
	glVertex2f(0, Height*0.05);
	glEnd();
	glTranslatef(-Width*0.99,-Height*0.05,0);
}

int main(int argc, char* args[]){
	// Inicializacao do SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Nome da janela
	SDL_WM_SetCaption("Square Fighter", NULL);
	
	// Tamanho da janela
	SDL_SetVideoMode(Width, Height, 32, SDL_OPENGL);
	
	// Cor da janela
	glClearColor(0,0,0,1);
	
	// Area exibida
	glViewport(0,0,Width,Height);
	
	// Sombreamento
	glShadeModel(GL_SMOOTH);
	
	// Usar Projecao 2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Desabilitar o 3D
	glDisable(GL_DEPTH_TEST);
	
	// Ativa o z-buffering, de modo a remover as superfícies escondidas
    //glEnable(GL_DEPTH_TEST);
	
	// Para uso da imagem
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Variaveis para inicializar o audio
	int frequencia = 22050;
	Uint16 formato = AUDIO_S16SYS;
	int canal = 2;
	int buffer = 4096;
	
	// Inicializa o audio
	Mix_OpenAudio(frequencia, formato, canal, buffer);
	
	Mix_Music * tema_ryu; // Variavel para a musica do Ryu
	Mix_Chunk * hadouken_ryu; // Variavel para o som do hadouken do Ryu
	Mix_Music * tema_ken; // Variavel para a musica do Ken
	Mix_Chunk * hadouken_ken; // Variavel para o som do hadouken do Ken
	
	// Armazena a musica do Ryu
	tema_ryu = Mix_LoadMUS("audio/Ryu_Stage.mp3");
	// Armazena o som do hadouken do Ryu
	hadouken_ryu = Mix_LoadWAV("audio/Ryu_Hadouken.wav");
	// Armazena a musica do Ken
	tema_ken = Mix_LoadMUS("audio/Ken_Stage.mp3");
	// Armazena o som do hadouken do Ken
	hadouken_ken = Mix_LoadWAV("audio/Ken_Hadouken.wav");
	
	// Reproduz a musica do Ryu
	Mix_PlayMusic(tema_ryu, -1);
	
	// Variaveis de auxilio
	bool executando = true;
	bool esq_per = false, dir_per = false, cima_per = false, esq_ini = false, dir_ini = false, cima_ini = false;
	bool atq_per = false, atq_ini = false;
	unsigned int projetil_per_textura = loadTexture("images/Hadouken.png");
	unsigned int projetil_ini_textura = loadTexture("images/Hadouken-2.png");
	unsigned int ryu_1 = loadTexture("images/Ryu.png");
	unsigned int ryu_2 = loadTexture("images/Ryu-2.png");
	unsigned int ken_1 = loadTexture("images/Ken.png");
	unsigned int ken_2 = loadTexture("images/Ken-2.png");
	
	// Variavel para controlar os eventos da janela
	SDL_Event eventos;
	
	// Variaveis do Ryu
	Bloco personagem, projetil_per;
	personagem.x = Width*0.05;
	personagem.y = Height*0.7;
	personagem.comp = Width*0.1;
	personagem.alt = Height*0.3;
	projetil_per.x = personagem.x+personagem.comp;
	projetil_per.y = (personagem.y+(personagem.alt)/2);
	projetil_per.comp = personagem.comp;
	projetil_per.alt = personagem.alt/8;
	float vida_per = VIDA_RYU;
	
	// Variaveis do Ken
	Bloco inimigo, projetil_ini;
	inimigo.x = Width*0.85;
	inimigo.y = Height*0.77;
	inimigo.comp = Width*0.1;
	inimigo.alt = Height*0.23;
	projetil_ini.x = inimigo.x-inimigo.comp;
	projetil_ini.y = (inimigo.y+(inimigo.alt)/2);
	projetil_ini.comp = inimigo.comp;
	projetil_ini.alt = inimigo.alt/7;
	float vida_ini = VIDA_RYU;
	
	// Array do cenario
	Bloco blocos[QUANT_BLOCOS];
	for(int i = 0, x = 0, y = Height; i < QUANT_BLOCOS; i++, x += Width/QUANT_BLOCOS){
		blocos[i].x = x;
		blocos[i].y = y;
		blocos[i].comp = Width*0.2;
		blocos[i].alt = Height*0.2;
	}
	
	// Variaveis para controlar a exibicao das texturas
	float x_ryu = RYU_X;
	float y_ryu = RYU_Y*2;
	float x_ken = KEN_X;
	float y_ken = KEN_Y*2;
	
	// Loop do jogo
	while(executando && vida_per>0 && vida_ini>0){
		// --- EVENTOS - KEYBINDS E ETC ---
		while(SDL_PollEvent(&eventos)){
			// Fecha com o X da janela
			if(eventos.type == SDL_QUIT){
				executando = false;
			}
			
			// Fecha com esc
			if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE){
				executando = false;
			}
			
			// Caso uma tecla seja pressionada
			if(eventos.type == SDL_KEYDOWN){
				// --- KEYBINDS DO RYU ---
				// Caso o a seja pressionado
				if(eventos.key.keysym.sym == SDLK_a){
					esq_per = true;
					x_ryu = RYU_X*5;
					y_ryu = RYU_Y*2;
				}
				// Caso o d seja pressionado
				else if(eventos.key.keysym.sym == SDLK_d){
					dir_per = true;
					x_ryu = RYU_X*2;
					y_ryu = RYU_Y*2;
				}
				// Caso o w seja pressionado
				else if(eventos.key.keysym.sym == SDLK_w){
					cima_per = true;
				}
				// Caso o espaco seja pressionado
				else if(eventos.key.keysym.sym == SDLK_SPACE){
					atq_per = true;
					Mix_PlayChannel(-1, hadouken_ryu, 0);
				}
				
				// --- KEYBINDS DO KEN ---
				// Caso a seta da esquerda seja pressionada
				if(eventos.key.keysym.sym == SDLK_LEFT){
					esq_ini = true;
					x_ken = KEN_X;
					y_ken = KEN_Y*3;
				}
				// Caso a seta da direita seja pressionada
				else if(eventos.key.keysym.sym == SDLK_RIGHT){
					dir_ini = true;
					x_ken = KEN_X*4;
					y_ken = KEN_Y*3;
				}
				// Caso a seta de cima seja pressionada
				else if(eventos.key.keysym.sym == SDLK_UP){
					cima_ini = true;
				}
				// Caso o backspace seja pressionado
				else if(eventos.key.keysym.sym == SDLK_BACKSPACE){
					atq_ini = true;
					Mix_PlayChannel(-1, hadouken_ken, 0);
				}
				
				// --- MUDAR A MUSICA ---
				// Caso o F1 seja pressionado
				if(eventos.key.keysym.sym == SDLK_F1){
					// Reproduz a musica do Ryu	
					Mix_PlayMusic(tema_ryu, -1);
				}
				// Caso o F2 seja pressionado
				if(eventos.key.keysym.sym == SDLK_F2){
					// Reproduz a musica do Ken
					Mix_PlayMusic(tema_ken, -1);
				}
			}
			// Caso uma tecla seja solta
			else if(eventos.type == SDL_KEYUP){
				// --- KEYBINDS DO RYU ---
				// Caso o a seja solto
				if(eventos.key.keysym.sym == SDLK_a){
					esq_per = false;
					x_ryu = RYU_X;
					y_ryu = RYU_Y*2;
				}
				// Caso o d seja solto
				else if(eventos.key.keysym.sym == SDLK_d){
					dir_per = false;
					x_ryu = RYU_X;
					y_ryu = RYU_Y*2;
				}
				// Caso o w seja solto
				else if(eventos.key.keysym.sym == SDLK_w){
					cima_per = false;
				}
				
				// --- KEYBINDS DO KEN ---
				// Caso a seta da esquerda seja solta
				if(eventos.key.keysym.sym == SDLK_LEFT){
					esq_ini = false;
					x_ken = KEN_X;
					y_ken = KEN_Y*2;
				}
				// Caso a seta da direita seja solta
				else if(eventos.key.keysym.sym == SDLK_RIGHT){
					dir_ini = false;
					x_ken = KEN_X;
					y_ken = KEN_Y*2;
				}
				// Caso a seta de cima seja solta
				else if(eventos.key.keysym.sym == SDLK_UP){
					cima_ini = false;
				}
			}
		}
		// --- EVENTOS - KEYBINDS E ETC ---
		
		// --- LOGICA ---
		
		// --- MOVIMENTO DO RYU ---
		if(esq_per == true){
			personagem.x -= 8;
			if(atq_per == false){
				projetil_per.x = personagem.x+personagem.comp;	
			}
			if(x_ryu <= RYU_X*2)
				x_ryu = RYU_X*5;
			else{
				x_ryu -= RYU_X;
				SDL_Delay(60);
			}	
		}
		else if(dir_per == true){
			personagem.x += 8;
			if(atq_per == false){
				projetil_per.x = personagem.x+personagem.comp;	
			}
			if(x_ryu >= RYU_X*5)
				x_ryu = RYU_X;
			else{
				x_ryu += RYU_X;
				SDL_Delay(60);
			}
		}
		else if(cima_per == true){
			if(personagem.y > Height*0.5){
				personagem.y -= 8;
				if(atq_per == false){
					projetil_per.y = (personagem.y+(personagem.alt)/2);	
				}
			}
			else if(personagem.y <= Height*0.5)
				cima_per = false;
		}
		// Evitar que o Ryu saia da tela
		if(personagem.x>Width-personagem.comp){
			personagem.x = Width-personagem.comp;
		}
		else if(personagem.x<0){
			personagem.x = 0;
		}
		// --- MOVIMENTO DO RYU ---
		
		// --- MOVIMENTO DO KEN ---
		if(esq_ini == true){
			inimigo.x -= 8;
			if(atq_ini == false){
				projetil_ini.x = inimigo.x-inimigo.comp;
			}
			if(x_ken >= KEN_X*4)
				x_ken = KEN_X;
			else{
				x_ken += KEN_X;
				if(esq_per == false && dir_per == false)
					SDL_Delay(60);
			}
		}
		else if(dir_ini == true){
			inimigo.x += 8;
			if(atq_ini == false){
				projetil_ini.x = inimigo.x-inimigo.comp;
			}
			if(x_ken <= KEN_X)
				x_ken = KEN_X*4;
			else{
				x_ken -= KEN_X;
				if(esq_per == false && dir_per == false)
					SDL_Delay(60);
			}
		}
		else if(cima_ini == true){
			if(inimigo.y > Height*0.57){
				inimigo.y -= 8;
				if(atq_ini == false){
					projetil_ini.y = (inimigo.y+(inimigo.alt)/2);
				}
			}
			else if(inimigo.y <= Height*0.57)
				cima_ini = false;
		}
		// Evitar que o Ken saia da tela
		if(inimigo.x>Width-inimigo.comp){
			inimigo.x = Width-inimigo.comp;
		}
		else if(inimigo.x<0){
			inimigo.x = 0;
		}
		// --- MOVIMENTO DO KEN ---
		
		// COLISAO
		if (colisao(personagem, inimigo) == true){
			personagem.x = inimigo.x - inimigo.comp;
		}
		
		// GRAVIDADE
		for(int i = 0; i < QUANT_BLOCOS; i++){
			if(personagem.y+personagem.alt <= blocos[i].y){
				personagem.y += 0.5;
				if(atq_per == false)
					projetil_per.y = (personagem.y+(personagem.alt)/2);
			}
			if(inimigo.y+inimigo.alt <= blocos[i].y){
				inimigo.y += 0.5;
				if(atq_ini == false)
					projetil_ini.y = (inimigo.y+(inimigo.alt)/2);
			}
		}
		// --- LOGICA ---
		
		// --- RENDERIZACAO ---
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Inicia a matriz de desenho
		glPushMatrix();
		
		// Dimensoes da matriz
		glOrtho(0, Width, Height, 0, -1, 1);
		
		// --- DESENHA O CENARIO ---
		desenhaCenario(blocos, vida_per, vida_ini);
		// --- DESENHA O CENARIO ---
		
		// --- DESENHA O RYU ---
		glColor3f(1, 1, 1);
		// Altera a textura do Ryu caso ele tenha usado o Hadouken
		if(atq_per == true){
			personagem.y = Height*0.77;
			personagem.alt = Height*0.23;
			desenhaPersonagem(personagem, ryu_2, 0, 1, 0, 1);
			personagem.y = Height*0.7;
			personagem.alt = Height*0.3;
		}
		else{
			desenhaPersonagem(personagem, ryu_1, x_ryu-RYU_X, x_ryu, y_ryu-RYU_Y, y_ryu);
		}
		// --- DESENHA O RYU ---
		
		
		// --- DESENHA O KEN ---
		glColor3f(1, 1, 1);
		// Altera a textura do Ken caso ele tenha usado o Hadouken
		if(atq_ini == true){
			desenhaPersonagem(inimigo, ken_2, 0, 1, 0, 1);
			inimigo.y = Height*0.77;
			inimigo.alt = Height*0.23;
		}
		else{
			desenhaPersonagem(inimigo, ken_1, x_ken, x_ken-KEN_X, y_ken, y_ken+KEN_Y);	
		}
		// --- DESENHA O KEN ---
		
		// --- CRIA O HADOUKEN DO RYU ---
		if(atq_per == true){
			glColor3f(1, 1, 1);
			desenhaProjetil(projetil_per, projetil_per_textura, 0, 1, 0, 1);
			
			if(colisao(projetil_per, inimigo) == false){
				projetil_per.x += 12;
				// evitar que o projetil saia da tela
				if(projetil_per.x>Width-projetil_per.comp || projetil_per.x<0){
					projetil_per.x = personagem.x+personagem.comp;
					projetil_per.y = (personagem.y+(personagem.alt)/2);
					atq_per = false;
				}
			}
			else if(colisao(projetil_per, inimigo) == true){
				projetil_per.x = personagem.x+personagem.comp;
				projetil_per.y = (personagem.y+(personagem.alt)/2);
				atq_per = false;
				vida_ini -= DANO_RYU;
			}
			
		}
		// --- CRIA O HADOUKEN DO RYU ---
		
		// --- CRIA O HADOUKEN DO KEN ---
		if(atq_ini == true){
			glColor3f(1, 1, 1);
			desenhaProjetil(projetil_ini, projetil_ini_textura, 0, 1, 0, 1);
			
			if(colisao(personagem, projetil_ini) == false){
				projetil_ini.x -= 12;
				// evitar que o projetil saia da tela
				if(projetil_ini.x>Width-projetil_ini.comp || projetil_ini.x<0){
					projetil_ini.x = inimigo.x-inimigo.comp;
					projetil_ini.y = (inimigo.y+(inimigo.alt)/2);
					atq_ini = false;
				}
			}
			else if(colisao(personagem, projetil_ini) == true){
				projetil_ini.x = inimigo.x-inimigo.comp;
				projetil_ini.y = (inimigo.y+(inimigo.alt)/2);
				atq_ini = false;
				vida_per -= DANO_KEN;
			}
		}
		// --- CRIA O HADOUKEN DO KEN ---
		
		// Fecha matrix
		glPopMatrix();
		
		// Troca os buffers
		SDL_GL_SwapBuffers();
		// --- RENDERIZACAO ---
	}
	// Libera a musica do Ryu
	Mix_FreeMusic(tema_ryu);
	// Libera o som do Hadouken do Ryu
	Mix_FreeChunk(hadouken_ryu);
	// Libera a musica do Ken
	Mix_FreeMusic(tema_ken);
	// Libera o som do Hadouken do Ken
	Mix_FreeChunk(hadouken_ken);
	// Fecha o audio
	Mix_CloseAudio();
	
	// Fecha a janela SDL
	SDL_Quit();
	
	return 0;
}