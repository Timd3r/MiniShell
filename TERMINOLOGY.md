# Woordenboek voor MiniShell: Concepten Eenvoudig Uitgelegd

Dit document is een verklarende woordenlijst voor de belangrijkste technische concepten in het MiniShell-project.

---

### **Proces**
Een 'proces' is een draaiend programma. Wanneer je `./minishell` start, creëer je één proces. Als je daarin `ls` typt, wordt er een *nieuw* proces voor `ls` gemaakt.

- **Kindproces (Child Process)**: Een nieuw proces dat door een ander proces wordt gestart. Het `ls`-proces is een kind van het `minishell`-proces.
- **Ouderproces (Parent Process)**: Het proces dat een ander proces heeft gestart. Het `minishell`-proces is de ouder van het `ls`-proces.

### **`fork()`**
De `fork()` systeemaanroep is de enige manier in Unix om een nieuw proces te maken. Het kloont het huidige proces. Na een `fork()` zijn er twee identieke processen (de ouder en het kind) die op hetzelfde punt in de code verdergaan. De enige manier om ze uit elkaar te houden is de returnwaarde van `fork()`: het geeft `0` terug in het kind, en de ID van het kind in de ouder.

**Uitleg voor de verdediging**: "We gebruiken `fork()` om een kindproces te maken waarin we een extern commando veilig kunnen uitvoeren, zonder onze eigen shell te beëindigen."

### **`execve()`**
De `execve()` systeemaanroep vervangt het programma dat *binnen* het huidige proces draait. De code, het geheugen, alles van het oude programma wordt weggegooid en vervangen door de inhoud van een nieuw programma-bestand.

**Cruciaal onderscheid**:
- Een **Programma** is een passief bestand op je harde schijf (bv. het bestand `/bin/ls`). Het is de 'blauwdruk'.
- Een **Proces** is een actieve instantie van een programma dat in het geheugen draait. Het is de 'uitvoering van de blauwdruk'.

`execve` creëert **geen nieuw proces**. Het laadt een nieuw programma in een *bestaand* proces. De Process ID (PID) blijft hetzelfde.

**Uitleg voor de verdediging**: "Nadat we `forken` hebben we een kindproces dat een exacte kopie is van onze shell. Binnen dat kind roepen we `execve` aan. Dit zorgt ervoor dat het kindproces stopt met het uitvoeren van de MiniShell-code en in plaats daarvan de code van een ander programma, zoals `ls`, laadt en uitvoert. Het kind *wordt* dus het `ls`-proces."

### **File Descriptor (FD)**
Een file descriptor is een simpel, positief getal dat voor een proces een open I/O-kanaal (zoals een bestand, een pipe, of de terminal) vertegenwoordigt. Je kunt het zien als een ticketnummer dat je krijgt bij een garderobe: je geeft je jas af, en je krijgt een nummertje. Later geef je het nummertje terug om je jas te krijgen. Zo geeft de `open()` systeemaanroep een FD terug, en met dat FD kun je later `read()` of `write()` aanroepen.

Standaard heeft elk proces 3 file descriptors open staan die verbonden zijn met de terminal:

- **`0` (STDIN_FILENO): Standaard Input**
    - Dit is het standaard *invoer*-kanaal.
    - **Voorbeeld**: Als een programma `scanf("%s", &str);` of `read(0, ...)` aanroept, leest het van dit kanaal. Standaard is dit verbonden met je toetsenbord.

- **`1` (STDOUT_FILENO): Standaard Output**
    - Dit is het standaard *uitvoer*-kanaal voor normale data.
    - **Voorbeeld**: Als een programma `printf("hello");` of `write(1, ...)` aanroept, schrijft het naar dit kanaal. Standaard is dit verbonden met je terminalscherm.

- **`2` (STDERR_FILENO): Standaard Error**
    - Dit is een apart *uitvoer*-kanaal specifiek voor foutmeldingen.
    - **Voorbeeld**: `perror("Fout!");` schrijft naar dit kanaal. Het is handig dat dit gescheiden is, zodat je normale output kunt omleiden zonder de foutmeldingen te missen.

### **Redirection (`<`, `>`, `>>`)**
Redirection is het proces waarbij de shell, *voordat* een commando wordt uitgevoerd, de standaard file descriptors (`0` of `1`) loskoppelt van de terminal en vastkoppelt aan een bestand.

#### **`>` (Output Redirection)**
**Commando**: `ls > file.txt`

1.  **Wat gebeurt er?**: De `stdout` (kanaal `1`) van het `ls`-proces wordt omgeleid naar `file.txt`.
2.  **Hoe?** (De taak van de shell):
    a. De shell roept `fork()` aan om een kindproces te maken.
    b. **In het kindproces**:
        i. De shell opent `file.txt` met `open("file.txt", O_WRONLY | O_CREAT | O_TRUNC)`. Dit geeft een nieuwe file descriptor terug, bijvoorbeeld `fd=3`.
        ii. De shell roept `dup2(3, 1)` aan. Dit is de magie: het zegt "maak file descriptor `1` (stdout) een exacte kopie van `3` (het bestand)". Nu wijst kanaal `1` niet meer naar de terminal, maar naar `file.txt`.
        iii. De shell roept `close(3)` aan omdat het nummertje niet meer nodig is; de verbinding is nu gelegd via `1`.
        iv. De shell roept `execve` aan om `ls` te starten.
3.  **Resultaat**: Wanneer `ls` draait en zijn output naar `stdout` (kanaal `1`) schrijft, gaat de data automatisch naar `file.txt`.

#### **`<` (Input Redirection)**
**Commando**: `wc -l < a.txt` (`wc -l` telt het aantal regels)

1.  **Wat gebeurt er?**: De `stdin` (kanaal `0`) van het `wc`-proces wordt verbonden met `a.txt`.
2.  **Hoe?**:
    a. De shell `forkt`.
    b. **In het kindproces**:
        i. De shell opent `a.txt` met `open("a.txt", O_RDONLY)`. Dit geeft `fd=3` terug.
        ii. De shell roept `dup2(3, 0)` aan. Nu wijst kanaal `0` (stdin) niet meer naar het toetsenbord, maar naar `a.txt`.
        iii. De shell roept `close(3)` aan.
        iv. De shell roept `execve` aan om `wc` te starten.
3.  **Resultaat**: Wanneer `wc` van `stdin` (kanaal `0`) probeert te lezen om regels te tellen, leest het de inhoud van `a.txt`.

#### **`>>` (Append Redirection)**
Dit werkt identiek aan `>`, met één verschil in stap 2.b.i: het bestand wordt geopend met de `O_APPEND` vlag (`open("file.txt", O_WRONLY | O_CREAT | O_APPEND)`). Dit zorgt ervoor dat de output aan het *einde* van het bestand wordt toegevoegd, in plaats van het bestand te overschrijven.

### **`dup2(old_fd, new_fd)`**
De `dup2()` systeemaanroep is de kern van redirection. Het kopieert een file descriptor. `dup2(fd, 1)` betekent: "Kopieer de verwijzing van file descriptor `fd` naar file descriptor `1` (stdout)". Vanaf nu zal alles wat naar `stdout` wordt geschreven, eigenlijk naar het bestand gaan waar `fd` naar verwees.

**Uitleg voor de verdediging**: "Om de output van `ls` naar `file.txt` te sturen, `openen` we `file.txt` (wat ons een `fd`, bv. 3, oplevert), en roepen dan `dup2(3, 1)` aan in het kindproces. Hierdoor wordt de `stdout` van het kind omgeleid."

### **Pipe (`|`)**
Een anoniem, eenrichtings-communicatiekanaal beheerd door het besturingssysteem. De `pipe()` systeemaanroep geeft je twee file descriptors: `pipefd[0]` (de lees-kant) en `pipefd[1]` (de schrijf-kant). Wat je in `pipefd[1]` schrijft, kan je uit `pipefd[0]` lezen.

**Uitleg voor de verdediging**: "Voor `cmd1 | cmd2` maken we een pipe. We `forken` twee kinderen. Kind 1 doet `dup2` van zijn `stdout` naar de schrijf-kant van de pipe. Kind 2 doet `dup2` van zijn `stdin` naar de lees-kant. Zo 'praten' ze met elkaar."

### **Here-Document (`<<`)**
Een speciaal type input-redirection waarbij de input direct van de terminal komt, tot een specifieke 'delimiter' wordt ingetypt.

**Uitleg voor de verdediging**: "We implementeren `<<` met een `pipe` en een `fork`. Een hulp-kindproces leest van de terminal en schrijft naar een pipe. Het hoofd-kindproces leest dan zijn input uit die pipe."

### **Signaal**
Een software-onderbreking van het besturingssysteem naar een proces, meestal door een gebruikersactie.
- `SIGINT` (`ctrl-C`): Signaal om het proces te onderbreken/stoppen.
- `SIGQUIT` (`ctrl-\`): Signaal om het proces te stoppen en een 'core dump' te maken.

### **`sigaction()`**
De moderne en correcte manier om te bepalen wat een proces moet doen als het een signaal ontvangt. We kunnen een functie (een 'handler') specificeren, het signaal negeren (`SIG_IGN`), of de standaardactie laten uitvoeren (`SIG_DFL`).

- **`volatile sig_atomic_t`**: Dit is een speciaal type variabele dat wordt gebruikt voor de globale signaalvlag.
    - **`volatile`**: Een hint aan de compiler dat de waarde van deze variabele onverwacht kan wijzigen (door een signaalhandler). Dit voorkomt dat de compiler foute optimalisaties toepast.
    - **`sig_atomic_t`**: Garandeert dat een lees- of schrijfoperatie naar deze variabele 'atomair' is: het kan niet halverwege onderbroken worden door een ander signaal, wat essentieel is voor betrouwbaarheid.

### **`waitpid(pid, &status, options)`**
Een systeemaanroep waarmee een ouderproces kan wachten tot een specifiek kindproces (`pid`) stopt. De `status` integer wordt gevuld met informatie over hoe het kind is gestopt.
- **`WIFEXITED(status)`**: Een macro dat controleert of het kind normaal is gestopt (via `exit()`).
- **`WEXITSTATUS(status)`**: Een macro dat de daadwerkelijke exit code (0-255) teruggeeft als het kind normaal is gestopt.

### **Built-in vs. Extern Commando**
- **Extern**: Een programma op het systeem (`/bin/ls`, `/usr/bin/grep`). Vereist altijd een `fork` en `execve`.
- **Built-in**: Een commando dat de shell zelf uitvoert (`cd`, `export`, `exit`). Vereist *geen* `fork`, omdat het de shell zelf moet aanpassen. De uitzondering is wanneer een built-in een redirection heeft.

### **`$PATH` Omgevingsvariabele**
Een lijst van directories, gescheiden door `:`, waar de shell zoekt naar externe commando's. Als je `ls` typt, zoekt de shell in `/bin`, `/usr/bin`, etc. naar een uitvoerbaar bestand met de naam `ls`.

### **EOF (End-of-File)**
Een conditie, geen karakter. Het betekent dat er geen data meer is om te lezen van een input-bron. `ctrl-D` in de terminal signaleert `EOF` aan het `readline`-programma.

### **Exit Status (`$?`)**
Elk proces geeft een getal terug als het eindigt (tussen 0 en 255). `0` betekent meestal "succes", elke andere waarde betekent een fout. De shell slaat de exit-status van het laatst uitgevoerde commando op, en je kunt deze opvragen met `$?`. 