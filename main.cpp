#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>

#define Width 900
#define Height 600
#define QUANT_BLOCOS 10

struct Bloco{
	float x;
	float y;
	float comp;
	float alt;
};

// A - personagem
// B - inimigo
bool colisao(Bloco A, Bloco B){
	if(A.y+A.alt < B.y)
		return false;
	else if(A.y > B.y+B.alt)
		return false;
	else if(A.x+A.comp < B.x) 
		return false;
	else if(A.x > B.x+B.comp)
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
	glClearColor(1,1,1,1);
	
	// area exibida
	glViewport(0,0,Width,Height);
	
	// sombreamento
	glShadeModel(GL_SMOOTH);
	
	// usar 2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// desabilitar 3D
	glDisable(GL_DEPTH_TEST);
	
	bool executando = true;
	bool esq_per = false, dir_per = false, cima_per = false, esq_ini = false, dir_ini = false, cima_ini = false;
	bool atq_distancia = false;
	
	SDL_Event eventos;
	
	// variaveis do personagem
	Bloco personagem;
	personagem.x = Width*0.05;
	personagem.y = Height*0.7;
	personagem.comp = Width*0.1;
	personagem.alt = Height*0.08;
	
	// variaveis do inimigo
	Bloco inimigo;
	inimigo.x = Width*0.85;
	inimigo.y = Height*0.7;
	inimigo.comp = Width*0.1;
	inimigo.alt = Height*0.08;
	
	Bloco projetil;
	
	Bloco blocos[QUANT_BLOCOS];
	
	for(int i = 0, x = 0, y = Height*0.8; i < QUANT_BLOCOS; i++, x += Width/QUANT_BLOCOS){
		blocos[i].x = x;
		blocos[i].y = y;
		blocos[i].comp = Width*0.2;
		blocos[i].alt = Height*0.2;
	}
	
	projetil.x = personagem.x+personagem.comp;
	projetil.y = (personagem.y+(personagem.alt)/2);
	projetil.comp = 20;
	projetil.alt = 10;
	
	// loop do jogo
	while(executando){
		// EVENTOS DO SDL - KEYBINDS E ETC
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
		
		// LOGICA
		
		// MOVIMENTO DO PERSONAGEM
		if(esq_per == true){
			personagem.x -= 8;
		}
		else if(dir_per == true){
			personagem.x += 8;
		}
		else if(cima_per == true){
			if(personagem.y > Height*0.55)
				personagem.y -= 8;
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
		
		// MOVIMENTO DO INIMIGO
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
		
		// COLISAO DO PERSONAGEM
		if (colisao(personagem, inimigo) == true){
			personagem.x = inimigo.x - inimigo.comp;
		}
		
		// GRAVIDADE
		for(int i = 0; i < QUANT_BLOCOS; i++){
			if(personagem.y+personagem.alt <= blocos[i].y){
				personagem.y += 0.5;
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
		
		// desenha o personagem
		glBegin(GL_QUADS);
		
		glColor3f(1, 0, 0);
		glVertex2f(personagem.x, personagem.y);
		glVertex2f(personagem.x+personagem.comp, personagem.y);
		glVertex2f(personagem.x+personagem.comp, personagem.y+personagem.alt);
		glVertex2f(personagem.x, personagem.y+personagem.alt);
		
		// encerra o desenho do personagem
		glEnd();
		
		// desenha o inimigo
		glBegin(GL_QUADS);
		
		glColor3f(0, 0, 1);
		glVertex2f(inimigo.x, inimigo.y);
		glVertex2f(inimigo.x+inimigo.comp, inimigo.y);
		glVertex2f(inimigo.x+inimigo.comp, inimigo.y+inimigo.alt);
		glVertex2f(inimigo.x, inimigo.y+inimigo.alt);
		glEnd();
		
		// desenha os blocos do cenario
		glBegin(GL_QUADS);
		glColor3f(0, 1, 0);
		for(int i = 0; i < QUANT_BLOCOS; i++){
			glVertex2f(blocos[i].x, blocos[i].y);
			glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y);
			glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y + blocos[i].alt);
			glVertex2f(blocos[i].x, blocos[i].y + blocos[i].alt);
		}
		glEnd();
		
		// cria o projetil
		if(atq_distancia == true){
			glBegin(GL_QUADS);
			glColor3f(1, 1, 0);
			glVertex2f(projetil.x, projetil.y-projetil.alt);
			glVertex2f(projetil.x+projetil.comp, projetil.y-projetil.alt);
			glVertex2f(projetil.x+projetil.comp, projetil.y+projetil.alt);
			glVertex2f(projetil.x, projetil.y+projetil.alt);
			glEnd();
			if(colisao(projetil, inimigo) == false){
				projetil.x += 5;
			}
			else if(colisao(projetil, inimigo) == true){
				projetil.x = personagem.x+personagem.comp;
				projetil.y = (personagem.y+(personagem.alt)/2);
				atq_distancia = false;
			}
		}
		
		// fecha matrix
		glPopMatrix();
		
		// animacao
		SDL_GL_SwapBuffers();
	}
	
	SDL_Quit();
	
	return 0;
}