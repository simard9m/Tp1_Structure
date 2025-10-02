#include<fstream>
#include<iostream>
#include "Professeur.cpp"


//fonction de trim pour les lignes
static inline std::string trim(const std::string& s) {
	//commence la a a 0 et le b a la fin
	size_t a = 0, b = s.size();

	//avance le a tant que il y a des espaces
	while (a < b && std::isspace(static_cast<unsigned char>(s[a])))
		a++;

	//Recule le b tant qu<il y a des esppaces
	while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
		b--;

	//retourne la chaine sans les espaces
	return s.substr(a, b - a);
	
}

class DossierProfesseur
{
private:
	Professeur* tete = nullptr;

	//methodes pour les ajouts

	static void ajouterCours(Professeur* prof, const std::string& sigle) {
		Cours* c = new Cours{ sigle,prof->listecours };
		prof->listecours = c;
	}

	static void ajouterEtu(Professeur* prof, const std::string& nom) {
		Etudiant* e = new Etudiant{ nom,prof->listetudiant };
		prof->listetudiant = e;
	}
	void ajouterProfTete(Professeur* p) {
		p->suivant = tete;
		tete = p;
	}
public:
	DossierProfesseur(std::string PFPath) {
		std::ifstream fichier(PFPath);

		if (!fichier.is_open()) {
			throw std::runtime_error("Impossible d'ouvrir le fichier " + PFPath);
		}

		std::string ligne;
		while (std::getline(fichier, ligne)) {
			std::cout << ligne << std::endl;
		}
	}

	~DossierProfesseur() {
		//Rien a detruire live up
	}
};