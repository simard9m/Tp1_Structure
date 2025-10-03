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

	//MEthode de lecture retourne faux si fin
	static bool lireLigneSignificative(std::istream& in, std::string& out) {
		std::string line;
		while (std::getline(in, line)) {

			line = trim(line);

			if (line.empty())
				continue;
			if (!line.empty() && line[0] == '#')
				continue;
			out = line;
			return true;
		}
		return false;
	}
public:
	DossierProfesseur(std::string PFPath) {
		std::ifstream f(PFPath);

		if (!f.is_open()) {
			throw std::runtime_error("Impossible d'ouvrir le fichier " + PFPath);
		}
		while (true) {
			//Nom
			std::string nom;
			if (!lireLigneSignificative(f, nom)) {
				break;
				//Fin du fichier si rien a lire ici
			}

			//anciennent
			std::string ancienStr;
			if (!lireLigneSignificative(f, ancienStr)) {
				throw std::runtime_error("Fin de fichier innatendue, Anciennete manquande pour " + nom);
			}
			int ancien = 0;

			//try de cast
			try {
				ancien = std::stoi(ancienStr);
			}
			catch(...){
				throw std::runtime_error("Anciennete invalide '" + ancienStr +"' pour "+nom);
			}

			//Cree le prof
			Professeur* p = new Professeur;
			p->nom = nom;
			p->ancien = ancien;

			//cours
			std::string line;
			while (true) {
				if (!lireLigneSignificative(f, line)) {
					//fin de ficher avant le premier &
					throw std::runtime_error("Fin de fichier innatendue,Attendu '&' (fin des cours) pour " + nom);
				}
				//fin des cours
				if (line == "&")
					break;

				ajouterCours(p, line);
			}

			//Etufiant
			while (true) {
				std::streampos posAvant = f.tellg();
				if (!lireLigneSignificative(f, line)) {
					//fin du fichier
					ajouterProfTete(p);
					p = nullptr;

					//sortie du while true
					goto boucle_suivante;
				}
				if (line == "&") {
					//fin du prof passe au prochaine
					ajouterProfTete(p);
					p = nullptr;
					break;
				}
				//sinon add un etudiant yaay
				ajouterEtu(p, line);
			}

			//pass au prochain prof
			boucle_suivante:
				continue;

		}

	}

	~DossierProfesseur() {
		Professeur* p = tete;
		while (p) {
			//cours
			Cours* c = p->listecours;
			while (c) {
				Cours* cn = c->suivant;
				delete c;
				c = cn;
			}

			//etu
			Etudiant* e = p->listetudiant;
			while (e) {
				Etudiant* en = e->apres;
				delete e;
				e = en;
			}

			//prof
			Professeur* pn = p->suivant;
			delete p;
			p = pn;
		}

	}

	void afficher() const {
		for (const Professeur* p = tete; p; p = p->suivant) {
			std::cout << "prof: " << p->nom << "anciennete: " << p->ancien << ")\n";
			std::cout << "cours :";
			if (!p->listecours)
				std::cout << "Aucun";
			for (const Cours* c = p->listecours; c; c = c->suivant)
				std::cout << c->sigle << (c->suivant ? ", " : " ");
			std::cout << "\n Etudiant: ";
			if (!p->listetudiant)
				std::cout << "Aucun";
			for (const Etudiant* e = p->listetudiant; e; e = e->apres)
				std::cout << e->nom << (e->apres ? ", " : "");
			std::cout << "\n\n";
		}
	}
};