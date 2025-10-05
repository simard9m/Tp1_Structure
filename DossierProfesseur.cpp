#include<fstream>
#include<iostream>
#include<stdexcept>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<cctype>
#include<filesystem>
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

	static void ajouterCoursFin(Professeur* prof, const std::string& sigle) {
		Cours* c = new Cours{ sigle,nullptr };
		if (!prof->listecours) {
			prof->listecours = c;
			return;
		}
		Cours* t = prof->listecours;
		while (t->suivant) {
			t = t->suivant;

		}
		t->suivant = c;
	}

	static void ajouterEtuFin(Professeur* prof, const std::string& nom) {
		Etudiant* e = new Etudiant{ nom,nullptr };
		if (!prof->listetudiant) {
			prof->listetudiant = e;
			return;
		}
		Etudiant* t = prof->listetudiant;
		while (t->apres) {
			t = t->apres;

		}
		t->apres = e;
	}
	void ajouterProfTeteFin(Professeur* p) {
		if (!tete) {
			tete = p;
			return;
		}
		Professeur* t = tete;
		while (t->suivant) {
			t = t->suivant;
		}
		t->suivant = p;
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

	//compte les etudiants d'un prof
	static int compterEtudiants(const Professeur* p) {
		int n = 0;
		for (const Etudiant* e = p->listetudiant;e;e = e->apres)
			n++;
		return n;
	}

	//Test si un prof a un sigle
	bool profPossedeCours(const Professeur* p, const std::string& sigle)const {
		for (const Cours* c = p->listecours;c;c = c->suivant) {
			if (c->sigle == sigle)
				return true;
		}
		return false;
	}

	//Eciter de traiter deux fosi le meme sigle dans le meme prof
	static bool coursDejaVuDansProf(const Cours* head, const Cours* node) {
		for (const Cours* x = head; x && x != node;x = x->suivant) {
			if (x->sigle == node->sigle)
				return true;
		}
		return false;
	}

	//Supp les profs avec le nom
	bool supprimer(const std::string& name) {
		Professeur** cur = &tete;
		bool profExistant = false;
		while (*cur) {
			if ((*cur)->nom == name) {
				Professeur* dead = *cur;
				profExistant = true;

				//Del cours
				Cours* c = dead->listecours;
				while (c) {
					Cours* nx = c->suivant;
					delete c;
					c = nx;
				}

				//Del etu
				Etudiant* e = dead->listetudiant;
				while (e) {
					Etudiant* nx = e->apres;
					delete e;
					e = nx;
				}

				*cur = dead->suivant;
				delete dead;
			}
			else {
				cur = &((*cur)->suivant);
			}
		}
		return profExistant;
	}

	//Afiicher le prof avec le plus d'etu
	std::string afficherProfPlusEtu() const {
		if (!tete) 
			return std::string();

		const Professeur* best = tete;
		int bestCnt = compterEtudiants(tete);

		for (const Professeur* p = tete->suivant;p;p = p->suivant) {
			int cnt = compterEtudiants(p);
			if (cnt > bestCnt) {
				best = p;
				bestCnt = cnt;
			}
		}
		return best ? best->nom : std::string();
	}

	//Afficher le cours le plus demandé
	std::string afficherCoursPlusDemande()const {
		if (!tete)
			return std::string();

		std::string bestCours;
		//freq max
		int bestCompte = -1;
		//Anciennete du prof du best cours
		int bestAncien = 0;

		//Check le prof dans l'ordre
		for (const Professeur* p = tete;p;p = p->suivant) {
			for (const Cours* c = p->listecours;c;c = c->suivant) {
				//ne compte le sigle 1 seule fois
				if (coursDejaVuDansProf(p->listecours, c))
					continue;

				//Calc de freq globale
				int freq = 0;
				for (const Professeur* q = tete;q;q = q->suivant) {
					bool aCeCours = false;
					for (const Cours* cq = q->listecours;cq;cq = cq->suivant) {
						if (cq->sigle == c->sigle) {
							aCeCours = true;
							break;
						}
						if (aCeCours)
							freq++;
					}
					if (bestCompte == -1) {
						//Permier candidat
						bestCompte = freq;
						bestCours = c->sigle;
						bestAncien = p->ancien;
					}
					else if (freq > bestCompte) {
						bestCompte = freq;
						bestCours = c->sigle;
						bestAncien = p->ancien;
					}
					else if (freq == bestCompte) {
						//tie break du prof le moins ancien
						if (p->ancien < bestAncien) {
							bestCours = c->sigle;
							bestAncien = p->ancien;
						}
						//si c'est egal on garde le premier rencontré
					}
				}
			}
		}
		return bestCompte == -1 ? std::string() : bestCours;
	}

	//Nombre de prof pour un cours
	int afficherNbreProfPourUnCours(const std::string& sigle) const {
		if (sigle.empty())
			return 0;
		int total = 0;
		for (const Professeur* p = tete;p;p = p->suivant) {
			//compter au plus une fois par prof
			if (profPossedeCours(p, sigle))
				total++;
		}
		return total;
	}

	void recopier(const std::string& FP)const {
		std::ofstream out(FP.c_str(), std::ios::trunc);
		if (!out.is_open())
			throw std::runtime_error("Impossible d'ouvrir " + FP);

		for (const Professeur* p = tete;p;p=p->suivant) {
			out << p->nom << "\n";
			out << p->ancien << "\n";
			for (const Cours* c = p->listecours;c;c = c->suivant) {
				out << c->sigle << "\n";
			}
			out << "&\n";
			for (const Etudiant* e = p->listetudiant;e;e = e->apres) {
				out << e->nom << "\n";
			}
			if (p->suivant)
				out << "&\n";
		}
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

				ajouterCoursFin(p, line);
			}

			//Etufiant
			while (true) {
				std::streampos posAvant = f.tellg();
				if (!lireLigneSignificative(f, line)) {
					//fin du fichier
					ajouterProfTeteFin(p);
					p = nullptr;

					//sortie du while true
					goto boucle_suivante;
				}
				if (line == "&") {
					//fin du prof passe au prochaine
					ajouterProfTeteFin(p);
					p = nullptr;
					break;
				}
				//sinon add un etudiant yaay
				ajouterEtuFin(p, line);
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
			std::cout << "prof: " << p->nom << " anciennete: " << p->ancien << "\n";
			std::cout << "cours : ";
			if (!p->listecours)
				std::cout << "Aucun";
			for (const Cours* c = p->listecours; c; c = c->suivant)
				std::cout << c->sigle << (c->suivant ? ", " : "");
			std::cout << "\n Etudiant: ";
			if (!p->listetudiant)
				std::cout << "Aucun";
			for (const Etudiant* e = p->listetudiant; e; e = e->apres)
				std::cout << e->nom << (e->apres ? ", " : "");
			std::cout << "\n\n";
		}
	}

	void executerFT(const std::string& FT, const std::string FP) {
		std::ifstream in(FT.c_str());
		if (!in.is_open())
			throw std::runtime_error("Impossible d'ouvrir le fichier ft" + FT);

		std::string line;
		while (std::getline(in, line)) {
			line = trim(line);

			if (line.empty())
				continue;

			//si ligne #
			if (line == "#") {
				std::string nom = afficherProfPlusEtu();
				if (!nom.empty())
					std::cout << "Le prof avec le plus d'etudiants est : " << nom << "\n";
				else
					std::cout << "La liste est vide\n";
				continue;
			}

			//Cours plus demande
			if (line == "*") {
				std::string cours = afficherCoursPlusDemande();
				if (!cours.empty())
					std::cout << "Le cours le plus demande est : " << cours << "\n";
				else
					std::cout << "La liste est vide\n";
				continue;
			}

			//Nombre de prof pour un cours
			if (line[0] ==  '%') {
				std::string sigle = trim(line.substr(1));
				int n = afficherNbreProfPourUnCours(sigle);
				if (n <= 0)
					std::cout << "Le cours " << sigle << " n'existe pas dans la liste\n";
				else
					std::cout << "Il y a " << n << " professeur pour le cours " << sigle << "\n";
				continue;
			}

			//le delete
			if (line[0] == '-') {
				std::string nom = trim(line.substr(1));
				if (supprimer(nom))
					std::cout << nom << " Supprime avec succes\n";
				else
					std::cout << "Professeur introuvable pour suppression : " << nom << "\n";
				continue;
			}

			//le copy
			if (line[0] == '$') {
				recopier(FP);
				std::cout << "Recopie du document faite\n";
				continue;
			}

		}
	}
};