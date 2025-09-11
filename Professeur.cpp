#include<string>

#include "Cours.cpp"
#include "Etudiant.cpp"
struct Professeur
{
	std::string nom;
	int ancien;
	Cours* listecours;
	Etudiant* listetudiant;
	Professeur* suivant;
};