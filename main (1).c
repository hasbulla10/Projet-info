#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_ATHLETES 100 
//allouage d'un espace mémoire pour stocker les informations de plusieurs athlètes en même temps

enum Epreuves {
    CENT_METRES = 1,
    QUATRE_CENT_METRES,
    CINQ_MILLE_METRES,
    MARATHON,
    RELAI_QUATRE_CENT_METRES
};//enumeration importante pour la gestion des choix et de l'affichage des epreuves dans le fichier

struct ContenuEpreuves {
    char temps[200];
    char date[200];
    int position;// Position du joueur dans le relai 4x400m
    char equipe[4][100];// Noms des autres joueurs de l'équipe du relai 4x400m
};

struct Athlete {
    char nom[100];
    double tempsMoyen;// facilite le calcul du temps et affiche une plus grande précisons dans les temps
};

// Fonction pour convertir le temps en secondes
double convertirTempsEnSecondes(const char *temps) {
    int minutes;
    double secondes;
    sscanf(temps, "%d.%lf", &minutes, &secondes);
    return minutes * 60 + secondes;
}

// Fonction qui enregistre une performance dans le fichier d'un athlète
void Majfichier(const char *nomFichier, enum Epreuves epreuve, struct ContenuEpreuves ajoutEpreuve) {//ouvre le fichier et ajoute un entrainement
    FILE *srcFile = fopen(nomFichier, "a");
    if (srcFile == NULL) {
        perror("Erreur lors de l'ouverture du fichier");//test si le fichier s'ouvre
        return;
    }
    fprintf(srcFile, "\n%s %s %s",
        (epreuve == CENT_METRES) ? "CENT_METRES" :
        (epreuve == QUATRE_CENT_METRES) ? "QUATRE_CENT_METRES" :
        (epreuve == CINQ_MILLE_METRES) ? "CINQ_MILLE_METRES" :
        (epreuve == MARATHON) ? "MARATHON" : "RELAI_QUATRE_CENT_METRES",
        ajoutEpreuve.temps, ajoutEpreuve.date);//rempli les infos de chaque epreuve
    if (epreuve == RELAI_QUATRE_CENT_METRES) {//cas du relai 400m
        fprintf(srcFile, " %d", ajoutEpreuve.position);//position du joueur dans le relais 
        for (int i = 0; i < 4; i++) {// membres de l'equipe du joueur
            fprintf(srcFile, " %s", ajoutEpreuve.equipe[i]);
        }
    }
    fprintf(srcFile, "\n");// Ajoute un retour à la ligne à la fin de l'entrée de la performance
    fclose(srcFile);
    printf("Performance enregistrée dans %s\n", nomFichier);
}

//Procédure qui enregistre les performances de l'équipe du relais dans les fichiers des autres membres de l'équipe
void MajfichierEquipe(enum Epreuves epreuve, struct ContenuEpreuves ajoutEpreuve) {
    for (int i = 0; i < 4; i++) {
        char chemin[150];
        sprintf(chemin, "Athlete/%s", ajoutEpreuve.equipe[i]);
        FILE *srcFile = fopen(chemin, "a");// Ajoute un retour à la ligne à la fin de l'entrée de la performance
        if (srcFile == NULL) {
            printf("Erreur lors de l'ouverture du fichier %s pour un membre de l'équipe\n", chemin);//message d'erreur si le fichier n'existe pas
            continue;//passage au prochain membre
        }
        // ecriture des différents détails de l'epreuve
        fprintf(srcFile, "RELAI_QUATRE_CENT_METRES %s %s %d %s %s %s %s\n",
            ajoutEpreuve.temps,
            ajoutEpreuve.date,
            ajoutEpreuve.position,
            ajoutEpreuve.equipe[0],
            ajoutEpreuve.equipe[1],
            ajoutEpreuve.equipe[2],
            ajoutEpreuve.equipe[3]);
        fclose(srcFile);
        printf("Performance du relai enregistrée dans %s\n", chemin);// message annonçant l'enregistrement de la performances dans les 4 fichiers
    }
}

// Procédure qui permet de lire les performances des joueurs
void lirePerformances(FILE *fichier, struct ContenuEpreuves *performances) {
    char tabParcours[512];
    int i = 0;
    rewind(fichier);
    while (fgets(tabParcours, sizeof(tabParcours), fichier) != NULL && i < 5) {
        tabParcours[strcspn(tabParcours, "\n")] = 0;// stock les infos dans le fichier
        sscanf(tabParcours, "%*s %199s %199s", performances[i].temps, performances[i].date);
        // lecture des différents détails de l'epreuve
        if (strstr(tabParcours, "RELAI_QUATRE_CENT_METRES") != NULL) {
            sscanf(tabParcours, "%*s %*s %*s %d %s %s %s %s",
                &performances[i].position,
                performances[i].equipe[0],
                performances[i].equipe[1],
                performances[i].equipe[2],
                performances[i].equipe[3]);
        }
        i++;// pour aller jusqu'a la fin du fichier
    }
}

//Procédure qui ouvre le fichier et ajoute un entrainement
void saisirEpreuve(const char *nomFichier, enum Epreuves epreuve, struct ContenuEpreuves ajoutEpreuve) {
    Majfichier(nomFichier, epreuve, ajoutEpreuve);//Apelle la procédure pour enregistrer une épreuve
    if (epreuve == RELAI_QUATRE_CENT_METRES) {
        MajfichierEquipe(epreuve, ajoutEpreuve);//appelle la procédure majfichierEquipe pour le relais
    }
}

//Procédure qui permet d'afficher les performances d'un athlete
void afficherPerformance(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour lecture.\n");// test de l'ouverture du fichier
        return;
    }
    char ligne[500];
    printf("\nVoici les performances actuelles :\n");
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {// verifie si le fichier n'est pas vide
        char epreuve[50], temps[50], date[50];
        int position;
        char equipe[4][100];
        sscanf(ligne, "%s %s %s", epreuve, temps, date);
        printf("Épreuve: %s, Temps: %s, Date: %s", epreuve, temps, date);//affiche les informations
        if (strcmp(epreuve, "RELAI_QUATRE_CENT_METRES") == 0) {
            sscanf(ligne, "%*s %*s %*s %d", &position);
            printf(", Position: %d, Equipe: ", position);//si relai 400m alors affiche la position
            sscanf(ligne, "%*s %*s %*s %*d %s %s %s %s", equipe[0], equipe[1], equipe[2], equipe[3]);
            for (int i = 0; i < 4; i++) {
                printf("%s, ", equipe[i]);// Affiche l'equipe de 4 athletes
            }
        }
        printf("\n");
    }
    fclose(fichier);
}

// Procédure qui résume les performances d'un athlète pour une épreuve donnée
void resumerPerformance(const char *nomFichier, enum Epreuves epreuve) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour lecture.\n");//verifie que le fichier existe
        return;
    }
    char ligne[500];
    int nbPerformances = 0;  // Compte le nombre de performances pour l'épreuve
    double meilleurTemps = 999999.0, pireTemps = 0.0, sommeTemps = 0.0;  // Initialise les variables pour calculer les statistiques
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        char epreuveLue[50], temps[50], date[50];
        sscanf(ligne, "%s %s %s", epreuveLue, temps, date);//extrait les donées
        // vérifie que l'epreuve extraite est bien l'epreuve qui qui est donnée
        if ((epreuve == CENT_METRES && strcmp(epreuveLue, "CENT_METRES") == 0) ||
            (epreuve == QUATRE_CENT_METRES && strcmp(epreuveLue, "QUATRE_CENT_METRES") == 0) ||
            (epreuve == CINQ_MILLE_METRES && strcmp(epreuveLue, "CINQ_MILLE_METRES") == 0) ||
            (epreuve == MARATHON && strcmp(epreuveLue, "MARATHON") == 0) ||
            (epreuve == RELAI_QUATRE_CENT_METRES && strcmp(epreuveLue, "RELAI_QUATRE_CENT_METRES") == 0)) {
            // Convertit le temps en secondes pour faciliter les calculs
            double tempsDouble = convertirTempsEnSecondes(temps);
            // Met à jour les statistiques
            if (tempsDouble < meilleurTemps) {//meilleur temps
                meilleurTemps = tempsDouble;
            }
            if (tempsDouble > pireTemps) {//pire temps
                pireTemps = tempsDouble;
            }
            sommeTemps += tempsDouble;
            nbPerformances++;//somme des temps
        }
    }
    fclose(fichier);

    // Affiche les résultats
    if (nbPerformances > 0) {
        double moyenneTemps = sommeTemps / nbPerformances;  // Calcule le temps moyen
        printf("\nRésumé des performances pour l'épreuve %d:\n", epreuve);
        printf("Meilleur temps: %.2f secondes\n", meilleurTemps);
        printf("Pire temps: %.2f secondes\n", pireTemps);
        printf("Temps moyen: %.2f secondes\n", moyenneTemps);
    } else {
        printf("\nAucune performance trouvée pour l'épreuve %d\n", epreuve);//message d'erreur
    }
}

// Procédure qui montre la progression des performances d'un athlète pour une épreuve pour 2 dates
void progressionPerformance(const char *nomFichier, enum Epreuves epreuve, const char *date1, const char *date2) {
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour lecture.\n");//verifie que le fichier existe
        return;
    }
    char ligne[500];
    double temps1 = -1, temps2 = -1;  // Initialiser les temps pour les deux dates à une valeur impossible

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        char epreuveLue[50], temps[50], date[50];
        //extrait les donées
        sscanf(ligne, "%s %s %s", epreuveLue, temps, date);
        // vérifie que l'epreuve extraite est bien l'epreuve qui qui est donnée
        if ((epreuve == CENT_METRES && strcmp(epreuveLue, "CENT_METRES") == 0) ||
            (epreuve == QUATRE_CENT_METRES && strcmp(epreuveLue, "QUATRE_CENT_METRES") == 0) ||
            (epreuve == CINQ_MILLE_METRES && strcmp(epreuveLue, "CINQ_MILLE_METRES") == 0) ||
            (epreuve == MARATHON && strcmp(epreuveLue, "MARATHON") == 0) ||
            (epreuve == RELAI_QUATRE_CENT_METRES && strcmp(epreuveLue, "RELAI_QUATRE_CENT_METRES") == 0)) {
            // Convertit le temps en secondes pour faciliter les comparaisons
            double tempsDouble = convertirTempsEnSecondes(temps);

            // Vérifie si la date lue correspond à l'une des deux dates données
            if (strcmp(date, date1) == 0) {
                temps1 = tempsDouble;  // Stocke le temps correspondant à la première date
            }
            if (strcmp(date, date2) == 0) {
                temps2 = tempsDouble;  // Stocke le temps correspondant à la deuxième date
            }
        }
    }
    fclose(fichier);

    // Vérifie si les deux temps ont bien été trouvés
    if (temps1 != -1 && temps2 != -1) {
        double progression = temps2 - temps1;  // Calcule la différence des 2 temps
        printf("\nProgression des performances entre %s et %s pour l'épreuve %d:\n", date1, date2, epreuve);
        printf("Temps au %s: %.2f secondes\n", date1, temps1);
        printf("Temps au %s: %.2f secondes\n", date2, temps2);
        printf("Différence de temps: %.2f secondes\n", progression);
    } else {
        printf("\nImpossible de trouver les performances pour les dates spécifiées.\n");// message d'erreur
    }
}

//Procédure pour determiner les Athletes qui partiront au J-O!
void meilleursAthletes(enum Epreuves epreuve) {
    struct Athlete athletes[MAX_ATHLETES]; // Tableau pour stocker les athlètes et leurs temps moyens
    int nbAthletes = 0; // Compteur pour le nombre d'athlètes trouvés
    DIR *dossier = opendir("Athlete");
    if (!dossier) {
        printf("Erreur lors de l'ouverture du dossier des athlètes.\n");
        return;
    }
    struct dirent *entry;
    // Parcourt tous les fichiers
    while ((entry = readdir(dossier)) != NULL) {
        // Vérifie que l'entrée est un fichier régulier
        if (entry->d_type == DT_REG) {
            char chemin[150];
            sprintf(chemin, "Athlete/%s", entry->d_name);
            FILE *fichier = fopen(chemin, "r");
            if (fichier == NULL) {
                printf("Erreur lors de l'ouverture du fichier %s.\n", chemin);//verifie que le fichier existe
                continue; // Passe au prochain fichier si l'ouverture échoue
            }
            char ligne[500];
            int nbPerformances = 0;
            double sommeTemps = 0.0; // Initialise la somme des temps
            // Lit chaque ligne du fichier de l'athlète
            while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
                char epreuveLue[50], temps[50], date[50];
                //extrait les donées
                sscanf(ligne, "%s %s %s", epreuveLue, temps, date);
                // vérifie que l'epreuve extraite est bien l'epreuve qui qui est donnée
                if ((epreuve == CENT_METRES && strcmp(epreuveLue, "CENT_METRES") == 0) ||
                    (epreuve == QUATRE_CENT_METRES && strcmp(epreuveLue, "QUATRE_CENT_METRES") == 0) ||
                    (epreuve == CINQ_MILLE_METRES && strcmp(epreuveLue, "CINQ_MILLE_METRES") == 0) ||
                    (epreuve == MARATHON && strcmp(epreuveLue, "MARATHON") == 0) ||
                    (epreuve == RELAI_QUATRE_CENT_METRES && strcmp(epreuveLue, "RELAI_QUATRE_CENT_METRES") == 0)) {
                    // Convertit le temps en secondes pour faciliter les calculs
                    double tempsDouble = convertirTempsEnSecondes(temps);
                    sommeTemps += tempsDouble; //somme des temps
                    nbPerformances++; // Incrémente le compteur de performances
                }
            }
            fclose(fichier);
            // Si l'athlète a des performances pour l'épreuve donnée, calcule le temps moyen
            if (nbPerformances > 0) {
                double moyenneTemps = sommeTemps / nbPerformances;
                strcpy(athletes[nbAthletes].nom, entry->d_name); // Stocke le nom de l'athlète
                athletes[nbAthletes].tempsMoyen = moyenneTemps; // Stocke le temps moyen
                nbAthletes++; // Incrémente le compteur d'athlètes
            }
        }
    }
    closedir(dossier);
    // Trie les athlètes par ordre croissant selon leur temps (Trie à bulle)
    for (int i = 0; i < nbAthletes - 1; i++) {
        for (int j = 0; j < nbAthletes - i - 1; j++) {
            if (athletes[j].tempsMoyen > athletes[j + 1].tempsMoyen) {
                struct Athlete temp = athletes[j];
                athletes[j] = athletes[j + 1];
                athletes[j + 1] = temp;
            }
        }
    }

    // Affiche les trois meilleurs athlètes pour l'épreuve donnée
    printf("\nLes trois Athletes iront au Jeux Olympiques pour l'épreuve %d (meilleur temps moyen) :\n", epreuve);
    for (int i = 0; i < 3 && i < nbAthletes; i++) {
        printf("%s : %.2f secondes\n", athletes[i].nom, athletes[i].tempsMoyen);
    }
}

// Procédure principale qui rassemble les autres procédures et qui gère la gestion d'un Athlete
void gestionAthlete() {
    while (1) {
        // Ouvre le dossier "Athlete"
        DIR *dossier = opendir("Athlete");
        if (dossier) {
            struct dirent *entry;
            // Affiche la liste des athlètes
            printf("\nListe des athlètes :\n");
            while ((entry = readdir(dossier)) != NULL) {
                // Vérifie que l'entrée est un fichier régulier
                if (entry->d_type == DT_REG) {
                    printf("%s\n", entry->d_name);
                }
            }
            rewinddir(dossier);//remet le pointeur au début
            char nomFichier[100];
            printf("\nVeuillez choisir un athlète : ");
            fgets(nomFichier, sizeof(nomFichier), stdin);
            nomFichier[strcspn(nomFichier, "\n")] = 0; // Supprime le caractère de nouvelle ligne
            char chemin[150];
            sprintf(chemin, "Athlete/%s", nomFichier);
            // Ouvre le fichier de l'athlète
            FILE *fichier = fopen(chemin, "r+");
            if (fichier != NULL) {
                fclose(fichier); // Ferme le fichier
                while (1) {
                    // Affiche le menu des options disponibles pour l'utilisateur
                    printf("\nChoisissez une option :\n");
                    printf("1. Voir l'historique des performances\n");
                    printf("2. Enregistrer une nouvelle performance\n");
                    printf("3. Résumer les performances\n");
                    printf("4. Voir la progression entre deux dates\n");
                    printf("5. Voir les trois meilleurs athlètes\n");
                    printf("6. Retourner au menu principal\n");
                    printf("7. Quitter\n");
                    printf("Votre choix: ");
                    int choix;
                    scanf("%d", &choix);
                    getchar(); // Capture le caractère de nouvelle ligne laissé par scanf
                    if (choix == 1) {
                        // affiche l'historique des performances de l'athlète
                        afficherPerformance(chemin);
                    } else if (choix == 2) {
                        // enregistre une nouvelle performance
                        struct ContenuEpreuves epreuve;
                        printf("\nChoisissez l'épreuve (1: 100m | 2: 400m | 3: 5000m | 4: Marathon | 5: relai400m): ");
                        int epreuveChoisie;
                        scanf("%d", &epreuveChoisie);
                        getchar();
                        printf("Entrez le temps effectué (minutes.secondes) pour l'épreuve : ");
                        fgets(epreuve.temps, sizeof(epreuve.temps), stdin);
                        epreuve.temps[strcspn(epreuve.temps, "\n")] = 0;
                        printf("Entrez la date pour l'épreuve : ");
                        fgets(epreuve.date, sizeof(epreuve.date), stdin);
                        epreuve.date[strcspn(epreuve.date, "\n")] = 0;
                        if (epreuveChoisie == 5) {
                            // Cas particulier pour le relais 4x400m
                            printf("Entrez la position du joueur (1-4) : ");
                            scanf("%d", &epreuve.position);
                            getchar();
                            printf("Entrez les noms des autres joueurs (un par ligne) : ");
                            for (int i = 0; i < 4; i++) {
                                printf("Nom du joueur %d: ", i + 1);
                                fgets(epreuve.equipe[i], sizeof(epreuve.equipe[i]), stdin);
                                epreuve.equipe[i][strcspn(epreuve.equipe[i], "\n")] = 0;
                            }
                        }
                        // Enregistre la nouvelle performance 
                        saisirEpreuve(chemin, epreuveChoisie, epreuve);
                    } else if (choix == 3) {
                        //résume les performances de l'athlète
                        printf("\nChoisissez l'épreuve à résumer (1: 100m | 2: 400m | 3: 5000m | 4: Marathon | 5: relai400m): ");
                        int epreuveChoisie;
                        scanf("%d", &epreuveChoisie);
                        getchar();
                        resumerPerformance(chemin, epreuveChoisie);
                    } else if (choix == 4) {
                        // voir la progression des performances entre deux dates
                        printf("\nChoisissez l'épreuve pour voir la progression (1: 100m | 2: 400m | 3: 5000m | 4: Marathon | 5: relai400m): ");
                        int epreuveChoisie;
                        scanf("%d", &epreuveChoisie);
                        getchar();
                        char date1[200], date2[200];
                        printf("Entrez la première date (YYYY/MM/DD) : ");
                        fgets(date1, sizeof(date1), stdin);
                        date1[strcspn(date1, "\n")] = 0;
                        printf("Entrez la deuxième date (YYYY/MM/DD) : ");
                        fgets(date2, sizeof(date2), stdin);
                        date2[strcspn(date2, "\n")] = 0;
                        progressionPerformance(chemin, epreuveChoisie, date1, date2);
                    } else if (choix == 5) {
                        // voir les trois meilleurs athlètes pour une épreuve donnée qui partent aux Jeux Olympiques
                        printf("\nChoisissez l'épreuve pour voir les trois meilleurs athlètes (1: 100m | 2: 400m | 3: 5000m | 4: Marathon | 5: relai400m): ");
                        int epreuveChoisie;
                        scanf("%d", &epreuveChoisie);
                        getchar();
                        meilleursAthletes(epreuveChoisie);
                    } else if (choix == 6) {
                        // retourne au menu principal
                        break;
                    } else if (choix == 7) {
                        // quitte le programme
                        printf("Vous nous quiité ?Nous éspérons vous revoir très bientôt!A la prochaine!\n");
                        exit(0);
                    } else {
                        // message d'erreur
                        printf("Choix invalide.\n");
                    }
                }
            } else {
                // message d'erreur 
                printf("Erreur : Impossible d'ouvrir le fichier %s.\n", nomFichier);
            }
            closedir(dossier);
        } else {
            // message d'erreur
            printf("Erreur lors de l'ouverture du dossier.\n");
        }
    }
}

int main(void) {
    printf("*******************************************************\n");
    printf("*                                                     *\n");
    printf("*                   Bienvenue aux J.O.                *\n");
    printf("*                                                     *\n");
    printf("*******************************************************\n");
    gestionAthlete();
    return 0;
}
