#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string>


#define Width 900
#define Height 600
#define QUANT_BLOCOS 10
#define VEL 200


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

// A - personagem
// B - inimigo
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

int main(int argc, char* args[]){
	SDL_Init(SDL_INIT_EVERYTHING);
	
	// memoria
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// nome da janela
	SDL_WM_SetCaption("Square Fighter", NULL);
	
	// tamanho da janela
	SDL_SetVideoMode(Width, Height, 32, SDL_OPENGL);
	
	// cor da janela
	glClearColor(0,0,0,1);
	
	// area exibida
	glViewport(0,0,Width,Height);
	
	// sombreamento
	glShadeModel(GL_SMOOTH);
	
	// usar 2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// desabilitar 3D
	glDisable(GL_DEPTH_TEST);
	
	// para uso da imagem
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	bool executando = true;
	bool esq_per = false, dir_per = false, cima_per = false, esq_ini = false, dir_ini = false, cima_ini = false;
	bool atq_distancia = false;
	unsigned int projetil_textura = 0;
	projetil_textura = loadTexture("Hadouken.png");
	unsigned int projetil_textura2 = 0;
	projetil_textura2 = loadTexture("Hadouken-2.png");
	
	SDL_Event eventos;
	
	// variaveis do personagem
	Bloco personagem;
	personagem.x = Width*0.05;
	personagem.y = Height*0.7;
	personagem.comp = Width*0.1;
	personagem.alt = Height*0.2;
	
	// variaveis do inimigo
	Bloco inimigo;
	inimigo.x = Width*0.85;
	inimigo.y = Height*0.7;
	inimigo.comp = Width*0.1;
	inimigo.alt = Height*0.2;
	
	// variaveis do projetil
	Bloco projetil;
	projetil.x = personagem.x+personagem.comp;
	projetil.y = (personagem.y+(personagem.alt)/2);
	projetil.comp = personagem.comp;
	projetil.alt = personagem.alt/6;
	
	Bloco blocos[QUANT_BLOCOS];
	
	for(int i = 0, x = 0, y = Height*0.9; i < QUANT_BLOCOS; i++, x += Width/QUANT_BLOCOS){
		blocos[i].x = x;
		blocos[i].y = y;
		blocos[i].comp = Width*0.2;
		blocos[i].alt = Height*0.2;
	}
	
	
	// loop do jogo
	while(executando){
		// --- EVENTOS DO SDL - KEYBINDS E ETC ---
		while(SDL_PollEvent(&eventos)){
			// fecha com o X da janela
			if(eventos.type == SDL_QUIT){
				executando = false;
			}
			
			// fecha com esc
			if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE){
				executando = false;
			}
			
			if(eventos.type == SDL_KEYDOWN){
				// keybinds do personagem
				if(eventos.key.keysym.sym == SDLK_a){
					esq_per = true;
				}
				else if(eventos.key.keysym.sym == SDLK_d){
					dir_per = true;
				}
				else if(eventos.key.keysym.sym == SDLK_w){
					cima_per = true;
				}
				else if(eventos.key.keysym.sym == SDLK_SPACE){
					atq_distancia = true;
				}
				
				// keybinds do inimigo
				if(eventos.key.keysym.sym == SDLK_LEFT){
					esq_ini = true;
				}
				else if(eventos.key.keysym.sym == SDLK_RIGHT){
					dir_ini = true;
				}
				else if(eventos.key.keysym.sym == SDLK_UP){
					cima_ini = true;
				}
			}
			else if(eventos.type == SDL_KEYUP){
				// keybinds do personagem
				if(eventos.key.keysym.sym == SDLK_a){
					esq_per = false;
				}
				else if(eventos.key.keysym.sym == SDLK_d){
					dir_per = false;
				}
				else if(eventos.key.keysym.sym == SDLK_w){
					cima_per = false;
				}
				
				// keybinds do inimigo
				if(eventos.key.keysym.sym == SDLK_LEFT){
					esq_ini = false;
				}
				else if(eventos.key.keysym.sym == SDLK_RIGHT){
					dir_ini = false;
				}
				else if(eventos.key.keysym.sym == SDLK_UP){
					cima_ini = false;
				}
			}
		}
		// --- EVENTOS DO SDL - KEYBINDS E ETC ---
		
		// LOGICA
		
		// --- MOVIMENTO DO PERSONAGEM ---
		if(esq_per == true){
			personagem.x -= 8;
			if(atq_distancia == false){
				projetil.x = personagem.x+personagem.comp;	
			}	
		}
		else if(dir_per == true){
			personagem.x += 8;
			if(atq_distancia == false){
				projetil.x = personagem.x+personagem.comp;	
			}
		}
		else if(cima_per == true){
			if(personagem.y > Height*0.55){
				personagem.y -= 8;
				if(atq_distancia == false){
					projetil.y = (personagem.y+(personagem.alt)/2);	
				}
			}
			else if(personagem.y <= Height*0.55)
				cima_per = false;
		}
		// evitar que o personagem saia da tela
		if(personagem.x>Width-personagem.comp){
			personagem.x = Width-personagem.comp;
		}
		else if(personagem.x<0){
			personagem.x = 0;
		}
		// --- MOVIMENTO DO PERSONAGEM ---
		
		// --- MOVIMENTO DO INIMIGO ---
		if(esq_ini == true){
			inimigo.x -= 8;
		}
		else if(dir_ini == true){
			inimigo.x += 8;
		}
		else if(cima_ini == true){
			if(inimigo.y > Height*0.55)
				inimigo.y -= 8;
			else if(inimigo.y <= Height*0.55)
				cima_ini = false;
		}
		// evitar que o inimigo saia da tela
		if(inimigo.x>Width-inimigo.comp){
			inimigo.x = Width-inimigo.comp;
		}
		else if(inimigo.x<0){
			inimigo.x = 0;
		}
		// --- MOVIMENTO DO INIMIGO ---
		
		// COLISAO
		if (colisao(personagem, inimigo) == true){
			personagem.x = inimigo.x - inimigo.comp;
		}
		
		// GRAVIDADE
		for(int i = 0; i < QUANT_BLOCOS; i++){
			if(personagem.y+personagem.alt <= blocos[i].y){
				personagem.y += 0.5;
				if(atq_distancia == false)
					projetil.y = (personagem.y+(personagem.alt)/2);
			}
			if(inimigo.y+inimigo.alt <= blocos[i].y){
				inimigo.y += 0.5;
			}
		}
		
		// RENDERIZACAO
		glClear(GL_COLOR_BUFFER_BIT);
		
		// inicia a matriz de desenho
		glPushMatrix();
		
		// dimensoes da matriz
		glOrtho(0, Width, Height, 0, -1, 1);
		
		// --- DESENHA PERSONAGEM ---
		glColor3f(0, 0, 1);
		glTranslatef(personagem.x,personagem.y,0);
		glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(personagem.comp, 0);
		glVertex2f(personagem.comp, personagem.alt);
		glVertex2f(0, personagem.alt);
		glEnd();
		glTranslatef(-personagem.x,-personagem.y,0);
		
		// --- DESENHA PERSONAGEM ---
		
		
		// --- DESENHA INIMIGO ---
		glTranslatef(inimigo.x,inimigo.y,0);
		glBegin(GL_QUADS);
		glColor3f(1, 0, 0);
		glVertex2f(0, 0);
		glVertex2f(inimigo.comp, 0);
		glVertex2f(inimigo.comp, inimigo.alt);
		glVertex2f(0, inimigo.alt);
		glEnd();
		glTranslatef(-inimigo.x,-inimigo.y,0);
		// --- DESENHA INIMIGO ---
		
		// --- DESENHA CENARIO ---
		glBegin(GL_QUADS);
		glColor3f(1,1,1);
		for(int i = 0; i < QUANT_BLOCOS; i++){
			glVertex2f(blocos[i].x, blocos[i].y);
			glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y);
			glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y + blocos[i].alt);
			glVertex2f(blocos[i].x, blocos[i].y + blocos[i].alt);
		}
		glEnd();
		// --- DESENHA CENARIO ---
		
		// --- CRIA PROJETIL ---
		if(atq_distancia == true){
			glColor3f(1, 1, 1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,projetil_textura);
			
			glTranslatef(projetil.x,projetil.y,0);
			glBegin(GL_QUADS);
			glTexCoord2d(0,0);
			glVertex2f(0, -projetil.alt);
			glTexCoord2d(1,0);
			glVertex2f(projetil.comp, -projetil.alt);
			glTexCoord2d(1,1);
			glVertex2f(projetil.comp, projetil.alt);
			glTexCoord2d(0,1);
			glVertex2f(0, projetil.alt);
			
			glEnd();
			glTranslatef(-projetil.x,-projetil.y,0);
			glDisable(GL_TEXTURE_2D);
			
			if(colisao(projetil, inimigo) == false){
				projetil.x += 8;
				// evitar que o projetil saia da tela
				if(projetil.x>Width-projetil.comp){
					projetil.x = personagem.x+personagem.comp;
					projetil.y = (personagem.y+(personagem.alt)/2);
					atq_distancia = false;
				}
				else if(projetil.x<0){
					projetil.x = personagem.x+personagem.comp;
					projetil.y = (personagem.y+(personagem.alt)/2);
					atq_distancia = false;
				}
			}
			else if(colisao(projetil, inimigo) == true){
				projetil.x = personagem.x+personagem.comp;
				projetil.y = (personagem.y+(personagem.alt)/2);
				atq_distancia = false;
			}
		}
		// --- CRIA PROJETIL ---
		
		// fecha matrix
		glPopMatrix();
		
		// animacao
		SDL_GL_SwapBuffers();
	}
	
	SDL_Quit();
	
	return 0;
}