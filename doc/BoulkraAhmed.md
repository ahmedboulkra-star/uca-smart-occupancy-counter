# Cahier de suivi de projet — Ahmed Boulkra

**Projet :** Smart Occupancy Monitoring System  
**Module :** Communication Sans Fil (CSF) — Licence 1, Université Nice Sophia Antipolis  
**Binôme :** Mecherouh Mohamed Ilyes  
**Encadrants :** F. Ferrero, J. Lanteri, L. Peyruchat, J. Roqui  

---

## Séance 1 — 03/04/2026 & 09/04/2026
* **Objectifs :** Brainstorming, formation de l'équipe et définition du livrable informatique.
* **Travail réalisé :**
  * Choix de la problématique : Gestion et comptage de l'occupation d'une salle en temps réel.
  * Modélisation algorithmique : Conception de la Machine d'États Finis (FSM) pour analyser l'ordre de déclenchement asynchrone des capteurs.
  * Répartition des rôles : Focus personnel complet sur le développement du script Arduino, la logique de temporisation et le réseau LoRaWAN, tandis que Mohamed Ilyes s'occupe de la partie matérielle et de l'isolation physique des capteurs.

---

## Séance 2 — 13/04/2026 & 16/04/2026
* **Objectifs :** Initialisation de Git et premier environnement de développement.
* **Travail réalisé :**
  * Création et configuration initiale du dépôt GitHub collectif pour assurer le contrôle de version.
  * Configuration de l'environnement Arduino IDE avec l'ajout des cartes requises pour l'UCA Education Board.
  * Développement du code de base pour lire l'état logique (HIGH/LOW) des broches d'entrées configurées (A1` et A3).

---

## Séance 3 — 20/04/2026 & 23/04/2026
* **Objectifs :** Écriture de l'algorithme directionnel et gestion du temps.
* **Travail réalisé :**
  * Implémentation de la boucle de lecture synchrone des capteurs PIR.
  * Programmation de la logique séquentielle : Validation d'une entrée si le PIR Extérieur s'active avant le PIR Intérieur (et inversement pour une sortie).
  * Constat technique en laboratoire : Les capteurs physiques captent trop large, ce qui provoque des déclenchements simultanés. Demande d'ajustement géométrique à Mohamed Ilyes.

---

## Séance 4 — 27/04/2026 & 30/04/2026
* **Objectifs :** Soutenance intermédiaire et bascule non-bloquante.
* **Travail réalisé :**
  * Présentation de l'état d'avancement devant l'équipe des enseignants.
  * Intégration des retours du jury : Suppression complète des fonctions bloquantes delay() et réécriture de la FSM autour de la fonction de comptage d'horloge millis().
  * Optimisation de la fenêtre de tir séquentielle (SEQUENCE_WINDOW) configurée à 800 millisecondes pour correspondre à notre entraxe matériel.

---

## Séance 5 — 04/05/2026 & 07/05/2026
* **Objectifs :** Intégration du filtre anti-rebond et création de la console TTN.
* **Travail réalisé :**
  * Ajout d'une temporisation d'aveuglement global (SYSTEM_COOLDOWN = 2000 ms) pour filtrer les faux déclenchements causés par les mouvements de bras ou d'habits lors d'un passage.
  * Création de notre compte applicatif sur le serveur de réseau The Things Network (TTN) V3 en bande EU868.

---

## Séance 6 — 11/05/2026 & 14/05/2026
* **Objectifs :** Configuration de la pile de communication LoRaWAN.
* **Travail réalisé :**
  * Suite à des erreurs de routage avec le mode d'activation ABP, choix de basculer vers une connexion OTAA (Over the Air Activation), beaucoup plus robuste pour notre soutenance.
  * Enregistrement du nœud sur la console TTN V3 et récupération des identifiants d'authentification uniques de notre carte : DevEUI, AppEUI (JoinEUI), et la clé de chiffrement AppKey.

---

## Séance 7 — 18/05/2026 & 21/05/2026
* **Objectifs :** Codage de la charge utile (payload), injection des clés et tests finaux.
* **Travail réalisé :**
  * Écriture de la fonction sendDataPacket() : Encodage de l'occupation sur 3 octets bruts (uint8_t) pour optimiser la bande passante et respecter le Duty Cycle de 1%.
    * Byte 0 : Salle ID (101) | Byte 1 : Compteur | Byte 2 : Event Flag (0x0A).
  * Injection de nos clés réelles de l'application dans le code Arduino final et téléversement sur l'UCA Board.
  * Validation réseau : Vérification de la bonne réception des messages uplinks en direct sur l'onglet Live Data de TTN.

---

## Séance 8 — 26/05/2026 & 28/05/2026
* **Objectifs :** Soutenance orale finale et démonstration devant le jury.
* **Travail réalisé :**
  * Présentation des slides de soutenance (Modélisation logicielle, cycle d'envoi et respect des normes radio).
  * Démonstration en direct de la connexion réseau instantanée ([LORA] Handshake Complete!) et de l'envoi immédiat des paquets d'occupation lors des simulations de passage face au jury.
