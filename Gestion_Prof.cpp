// Gestion_Prof.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include "DossierProfesseur.cpp"
#include<string>


int main()
{
	//On veux get le chemin du FP par l'utilisateur
	std::string chemin;
	std::cout << "Entrez le chemin du fichier FP : ";
	std::getline(std::cin, chemin);

	DossierProfesseur dossier = DossierProfesseur(chemin);
	dossier.afficher();


	std::string cheminTransact;
	std::cout << "Entrez le chemin du fichier FT : ";
	std::getline(std::cin, cheminTransact);

	try {
		dossier.executerFT(cheminTransact , chemin);
	}
	catch (const std::exception& e) {
		std::cerr << "Erreur : " << e.what() << std::endl;
	}

	dossier.afficher();

}

