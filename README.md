> Ce projet est réalisé dans le cadre du module Communication Sans Fil en Licence 1 à l'Université Nice Sophia Antipolis

# 🚪 Smart Occupancy Monitoring System

Un système embarqué de comptage bidirectionnel automatisé pour surveiller l'occupation en temps réel d'une salle universitaire, propulsé par l'UCA Education Board et le réseau LoRaWAN.

---

## 👥 Notre Équipe

* **Boulkra Ahmed**
    * *Rôle :* Gestion du matériel, isolation physique des capteurs & encodage de la charge utile (payload).
* **Mecherouh Mohamed Ilyes**
    * *Rôle :* Algorithme central (Machine d'États FSM), configuration réseau & architecture du dépôt Git.

---

## 📌 Présentation du Projet

Le **Smart Occupancy Monitoring System** résout le problème de la gestion d'espace en comptant automatiquement le nombre de personnes présentes dans une pièce en temps réel. 

Le cœur du prototype utilise deux capteurs infrarouges passifs (PIR) placés côte à côte à l'entrée d'une porte. En analysant quel capteur s'allume en premier, le système détermine instantanément le sens de passage (Entrée ou Sortie) et met à jour un compteur global. Ces données sont immédiatement propulsées dans le cloud via une antenne radio accordée sur le réseau **LoRaWAN (The Things Network V3)**.

### 💡 L'Astuce de Laboratoire (Les Caches en Papier)
Les capteurs PIR du commerce possèdent un champ de vision très large (environ 110 degrés). Placés côte à côte sur une petite platine, ils se déclenchaient en même temps, bloquant la détection de mouvement. 
Pour résoudre ce problème physique, nous avons conçu et fixé des **caches occultants (blinders) en papier** autour de chaque capteur. Ils canalisent les faisceaux infrarouges en lignes droites et parallèles, forçant un ordre de coupure net et parfait lorsqu'une personne traverse la porte.

---

## 🛠️ Spécifications Matérielles & Branchements

Notre prototype est entièrement câblé sur la carte **UCA Education Board** (équipée d'un microcontrôleur ATmega328PB fonctionnant sur la bande radio de **868 MHz**).

* **Capteur PIR 1 (Côté Extérieur)**
    * `Broche A2` -> Alimentation VCC (pilotée dynamiquement par le code)
    * `Broche A3` -> Sortie Signal Logique (`HIGH` / `LOW`)
* **Capteur PIR 2 (Côté Intérieur)**
    * `Broche A0` -> Alimentation VCC (pilotée dynamiquement par le code)
    * `Broche A1` -> Sortie Signal Logique (`HIGH` / `LOW`)
* **Module Radio**
    * Émetteur LoRa embarqué relié à une antenne filaire optimisée pour la bande **EU868**.

---

## ⚙️ Logique Algorithmique (Machine d'États)

Le code fonctionne de manière asynchrone et non-bloquante grâce à la fonction `millis()`. Il capture le mouvement à travers une fenêtre de tir de **800 millisecondes**, adaptée à l'entraxe réduit de nos capteurs.

### Séquence d'Entrée (Entry)
1. Le PIR 1 (`A3`) passe à l'état `HIGH` en premier -> Début de séquence enregistré.
2. Le PIR 2 (`A1`) passe à l'état `HIGH` dans la fenêtre des 800ms -> **ENTRÉE VALIDÉE**.
3. Action : Le compteur global augmente (`peopleCount++`).

### Séquence de Sortie (Exit)
1. Le PIR 2 (`A1`) passe à l'état `HIGH` en premier -> Début de séquence enregistré.
2. Le PIR 1 (`A3`) passe à l'état `HIGH` dans la fenêtre des 800ms -> **SORTIE VALIDÉE**.
3. Action : Le compteur global diminue (`peopleCount - 1`). 
4. *Sécurité :* Le compteur est verrouillé logiciellement pour ne jamais descendre en dessous de zéro.

### Filtre Anti-Rebond (System Cooldown)
Dès qu'un mouvement est validé (Entrée ou Sortie), le système active un aveuglement total pendant **2000 millisecondes**. Cette coupure permet de nettoyer les variations de signaux générées par les vêtements ou les mouvements de bras d'une même personne, laissant le temps aux capteurs de revenir au repos.

---

## 📡 Structure des Octets LoRaWAN (Payload)

Pour rester en parfaite conformité avec les lois européennes sur les bandes de communication libres (Duty Cycle limité à 1%), le système n'envoie aucun texte brut. Les données sont empaquetées dans **3 octets bruts (uint8_t)** compacts transmis sur le Port 1 :

* **Octet 0 :** Identifiant de la salle (`ROOM_ID = 101`, soit `0x65` en hexadécimal).
* **Octet 1 :** Nombre actuel d'occupants dans la pièce (`peopleCount`).
* **Octet 2 :** Code d'état de l'événement (`0x0A` = Notification de rafraîchissement du compteur).

La console web **The Things Network (TTN) V3** intercepte ce paquet via une activation **OTAA**, déchiffre la clé AES de cryptage, et affiche instantanément les variables sur le flux d'activité.

---

## 📁 Structure du Dépôt

```text
Smart_Occupancy_Counter/
│
├── README.md                          # Page d'accueil et documentation principale
├── Code/
│   └── occupancy_counter.ino          # Code source Arduino complet injecté avec clés OTAA
├── doc/
│   ├── BoulkraAhmed.md                # Journal de suivi matériel d'Ahmed
│   └── MecherouhMohamedIlyes.md       # Journal de suivi logiciel de Mohamed Ilyes
└── Presentation/
    └── Smart_Occupancy_Counter.pdf    # Support visuel pour la soutenance orale finale
