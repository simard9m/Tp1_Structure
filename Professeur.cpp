#include<string>


#include "Cours.cpp"
#include "Etudiant.cpp"
struct Professeur
{
	std::string nom;
	int ancien;
	Cours* listecours = nullptr;
	Etudiant* listetudiant = nullptr;
	Professeur* suivant = nullptr;
};