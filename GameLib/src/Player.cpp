#include <glm/glm.hpp>

#include "InputManager.h"
#include "Core/Window.h"


enum Wall {DOWN, UP, LEFT, RIGHT};
enum Colonne {GAUCHE, CENTRE, DROITE};

class player {
public:

	Wall mur_courant = DOWN;
	Colonne colonne_courant = CENTRE;

	glm::vec3 table[4][3] =
	{
		//DOWN
		//GAUCHE                  //CENTRE              //DROITE
		{ { -1.30f, 1.0f, 2.0f }, { 0.0f, 1.0f, 2.0f }, { 1.30f, 1.0f, 2.0f } },

		//UP
		//GAUCHE                  //CENTRE              //DROITE
		{ { 1.30f, 5.0f, 2.0f }, { 0.0f, 5.0f, 2.0f }, { -1.30f, 5.0f, 2.0f } },

		//LEFT
		//GAUCHE                  //CENTRE              //DROITE
		{ { -6.0f, 3.30f, 2.0f }, { -6.0f, 2.0f, 2.0f }, { -6.0f, 1.30f, 2.0f } },

		//RIGHT
		//GAUCHE                  //CENTRE              //DROITE
		{ { 6.0f, 1.30f, 2.0f }, { 6.0f, 2.0f, 2.0f }, { 6.0f, 3.30f, 2.0f } },
	};

	glm::vec3 GetPosition()
	{
		return table[mur_courant][colonne_courant];
	}

	void player_moovment(std::unique_ptr<KGR::RenderWindow> window)
	{
		auto input = window->GetInputManager();

		if (input->IsKeyDown(KGR::Key::Q) || input->IsKeyDown(KGR::Key::A))
		{
			if (colonne_courant = GAUCHE)
				colonne_courant = CENTRE;
			else if (colonne_courant = CENTRE)
				colonne_courant = DROITE;
		}
		if (input->IsKeyDown(KGR::Key::D))
		{
			if (colonne_courant = DROITE)
				colonne_courant = CENTRE;
			else if (colonne_courant = CENTRE)
				colonne_courant = GAUCHE;
		}
	}

	void wall_moovment()
	{ 
	
	}
};