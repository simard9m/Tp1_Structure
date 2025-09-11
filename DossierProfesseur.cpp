#include "Professeur.cpp"
class DossierProfesseur
{
private:
	Professeur* tete;
public:
	DossierProfesseur(std::string PFPath);

	~DossierProfesseur();
};