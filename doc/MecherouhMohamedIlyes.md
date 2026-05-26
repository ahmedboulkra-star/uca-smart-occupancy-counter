# Cahier de suivi de projet — Mecherouh Mohamed Ilyes

**Projet :** Smart Occupancy Monitoring System  
**Module :** Communication Sans Fil (CSF) — Licence 1, Université Nice Sophia Antipolis  
**Binôme :** Boulkra Ahmed  
**Encadrants :** F. Ferrero, J. Lanteri, L. Peyruchat, J. Roqui  

---

## Séance 1 — 03/04/2026 & 09/04/2026
* **Objectifs :** Brainstorming, choix du sujet et étude de faisabilité matérielle.
* **Travail réalisé :**
  * Définition de l'architecture physique : Choix d'utiliser l'UCA Education Board couplée à deux capteurs infrarouges passifs (PIR).
  * Répartition des rôles : Prise en charge exclusive de l'intégration des composants, des branchements physiques, et de l'isolation optique des capteurs, pendant qu'Ahmed prend en main la logique logicielle.

---

## Séance 2 — 13/04/2026 & 16/04/2026
* **Objectifs :** Implantation matérielle et câblage sur table.
* **Travail réalisé :**
  * Analyse de la fiche technique (datasheet) et étude du brochage de la carte UCA Board (microcontrôleur ATmega328PB).
  * Réalisation du câblage électrique propre des deux capteurs sur la platine de test :
    * PIR 1 (Extérieur) : Alimentation sur A2, Signal sur A3.
    * PIR 2 (Intérieur) : Alimentation sur A0, Signal sur A1.

---

## Séance 3 — 20/04/2026 & 23/04/2026
* **Objectifs :** Analyse des perturbations physiques et isolation.
* **Travail réalisé :**
  * Tests d'intégration des capteurs placés côte à côte.
  * Détection d'un problème matériel majeur : Les lentilles de Fresnel des capteurs ont un angle de vue trop large (110°). Le passage d'une personne active les deux capteurs en même temps, empêchant Ahmed de valider l'ordre de sa machine d'états.

---

## Séance 4 — 27/04/2026 & 30/04/2026
* **Objectifs :** Soutenance de mi-parcours et correction mécanique.
* **Travail réalisé :**
  * Participation à la présentation intermédiaire devant les enseignants de l'UCA.
  * Réflexion sur une solution d'isolation optique mécanique simple et légère pour la maquette.

---

## Séance 5 — 04/05/2026 & 07/05/2026
* **Objectifs :** Fabrication des caches occultants (blinders).
* **Travail réalisé :**
  * Conception et découpe de deux caches tubulaires (blinders) sur mesure en papier occultant rigide fixés autour des dômes des capteurs PIR.
  * Résultat expérimental impeccable : Les cônes de détection sont réduits en faisceaux étroits et strictement parallèles. L'ordre de franchissement physique est maintenant parfaitement franc et net.

---

## Séance 6 — 11/05/2026 & 14/05/2026
* **Objectifs :** Stabilisation électrique et préparation radio.
* **Travail réalisé :**
  * Ajustement matériel de l'alimentation des capteurs. Programmation d'une coupure logicielle complète au boot pour réinitialiser proprement les composants pyroélectriques.
  * Montage et ajustement de l'antenne filaire accordée pour la bande de fréquence 868 MHz sur le connecteur radio de la carte.

---

## Séance 7 — 18/05/2026 & 21/05/2026
* **Objectifs :** Assemblage final de la maquette et synchronisation des tests.
* **Travail réalisé :**
  * Fixation robuste des capteurs et de leurs caches en papier sur le support final pour éviter tout désalignement lors du transport vers le Campus Valrose.
  * Validation matérielle lors des tests de transmission radio d'Ahmed : Surveillance de la consommation d'énergie lors des phases d'émissions uplinks vers la passerelle du campus.
  * Finalisation du diaporama de présentation (Schémas électroniques, nomenclature matérielle).

---

## Séance 8 — 26/05/2026 & 28/05/2026
* **Objectifs :** Soutenance finale et gestion de la démo.
* **Travail réalisé :**
  * Présentation des slides matérielles (Brochage, architecture physique, fonctionnement des caches en papier).
  * Gestion de la partie matérielle pendant la démonstration en direct devant le jury : Phase de stabilisation de 15 secondes sans interférence, puis simulation de passages fluides pour faire varier le compteur en direct sur l'écran TTN V3 d'Ahmed.
