# De Levenscyclus van een Commando in MiniShell

Dit document legt de volledige flow uit van een commando, van het intypen tot de uiteindelijke output. We gebruiken een complex voorbeeld om alle stappen te illustreren.

**Voorbeeldcommando**: `cat Makefile | grep "OBJ =" > temp.txt`

---

### Stap 1: De Prompt en Input (De Hoofd-Loop)

1.  **Start**: Onze `main` functie heeft de `shell` structuur geïnitialiseerd en de signaalhandlers ingesteld. Het roept `shell_loop` aan.
2.  **Wachten**: De shell is nu in de `while(1)`-lus. De `readline("Minishell> ")` functie wordt aangeroepen. De shell pauzeert en wacht op input van de gebruiker.

De gebruiker typt: `cat Makefile | grep "OBJ =" > temp.txt` en drukt op Enter.

`readline` retourneert deze string.

---

### Stap 2: De Lexer (Van String naar Tokens)

De `process_line` functie geeft de string aan `make_tokens`. Deze scant de string en zet hem om in een array van `t_token` structuren.

**Input**: `"cat Makefile | grep "OBJ =" > temp.txt"`

**Output (de `tokens` array)**:
- `tokens[0]`: `{ type: WORD, c: "cat" }`
- `tokens[1]`: `{ type: WORD, c: "Makefile" }`
- `tokens[2]`: `{ type: PIPE, c: "|" }`
- `tokens[3]`: `{ type: WORD, c: "grep" }`
- `tokens[4]`: `{ type: WORD, c: ""OBJ ="" }`  *(Quotes staan er nog in!)*
- `tokens[5]`: `{ type: OUT,  c: ">" }`
- `tokens[6]`: `{ type: WORD, c: "temp.txt" }`
- `tokens[7]`: `NULL`

---

### Stap 3: De Parser (Van Tokens naar Commando's)

`execute_tokens` ziet dat er een `PIPE` token is en roept `split_by_pipes` aan. Dit splitst de tokenlijst in tweeën, rond de `PIPE`. Voor elke helft wordt `parse_simple_command_shell` aangeroepen.

#### **Commando 1: `cat Makefile`**
- De parser maakt een `t_simple_cmd` struct.
- Tokens `cat` en `Makefile` worden in `cmd->args` gezet.
- Resultaat: `cmd1->args = {"cat", "Makefile", NULL}`. Geen redirections.

#### **Commando 2: `grep "OBJ =" > temp.txt`**
- De parser maakt een tweede `t_simple_cmd` struct.
- Token `grep` en `"OBJ ="` worden `WORD`s en gaan in `cmd->args`.
- Token `>` wordt herkend als redirection. Het volgende token, `temp.txt`, wordt opgeslagen in `cmd->output_file`.
- Resultaat:
    - `cmd2->args = {"grep", ""OBJ ="", NULL}`
    - `cmd2->output_file = "temp.txt"`

---

### Stap 4: De Expander (Quotes en Variabelen)

Nu wordt `process_arg_expansion` aangeroepen op de `args` van *elk* commando.

- **`cmd1`**: Geen quotes of dollars. `{"cat", "Makefile"}` blijft ongewijzigd.
- **`cmd2`**: De string `""OBJ =""` wordt verwerkt.
    - `handle_double_quote` wordt aangeroepen.
    - Het kijkt *binnen* de quotes naar variabelen. Er zijn er geen.
    - De quotes worden verwijderd.
- **Finaal Resultaat na Parsing & Expansie**:
    - `cmd1`: `{ args: {"cat", "Makefile"}, output_file: NULL }`
    - `cmd2`: `{ args: {"grep", "OBJ ="}, output_file: "temp.txt" }`

**(Zijspoor: Wat als het commando `echo "Gebruiker: $USER"` was?)**
Tijdens de expansie-stap voor de `echo` command struct:
1.  De string `"Gebruiker: $USER"` wordt verwerkt.
2.  `handle_double_quote` wordt aangeroepen.
3.  De inhoud `Gebruiker: $USER` wordt door `expand_variables` gehaald.
4.  `expand_variables` vindt `$USER`, roept `getenv("USER")` aan, en krijgt bijvoorbeeld "mikellen" terug.
5.  De uiteindelijke `arg` wordt `Gebruiker: mikellen` (zonder de quotes).

De commando's zijn nu volledig voorbereid.

---

### Stap 5: De Executor (Pipes, Forks en Uitvoering)

We hebben een pipeline, dus `execute_pipeline_loop` wordt de baas.

1.  **Pipe Creatie**: Er is één `|`, dus `pipe(pipefd)` wordt aangeroepen. We hebben nu `pipefd[0]` (lezen) en `pipefd[1]` (schrijven).

2.  **Fork 1 (voor `cat`)**:
    - `fork()` wordt aangeroepen.
    - **In het Kind 1 (`cat`)**:
        - `dup2(pipefd[1], STDOUT_FILENO)`: De standaard output van dit kind wordt omgeleid naar de schrijf-kant van de pipe.
        - Alle pipe-fds worden gesloten.
        - `execve` wordt aangeroepen voor `/bin/cat` met `args: {"cat", "Makefile"}`.
        - Het `cat`-proces start, leest `Makefile`, en schrijft de inhoud naar zijn `stdout` (wat nu de pipe is).

3.  **Fork 2 (voor `grep`)**:
    - `fork()` wordt opnieuw aangeroepen in de hoofd-shell.
    - **In het Kind 2 (`grep`)**:
        - `dup2(pipefd[0], STDIN_FILENO)`: De standaard input van dit kind wordt omgeleid naar de lees-kant van de pipe.
        - Het kind ziet `cmd2->output_file`. Het opent `temp.txt` (wat `fd=3` oplevert) en roept `dup2(3, STDOUT_FILENO)` aan. De output wordt nu naar het bestand gestuurd.
        - Alle pipe-fds worden gesloten.
        - `execve` wordt aangeroepen voor `/bin/grep` met `args: {"grep", "OBJ ="}`.
        - Het `grep`-proces start, leest van zijn `stdin` (de pipe), filtert de regels, en schrijft het resultaat naar zijn `stdout` (het bestand `temp.txt`).

4.  **In de Parent (onze MiniShell)**:
    - **Cruciaal**: De parent sluit *onmiddellijk* beide kanten van de pipe (`pipefd[0]` en `pipefd[1]`). Dit is essentieel zodat `grep` een `EOF` krijgt als `cat` klaar is met schrijven.
    - De parent roept `waitpid` aan voor Kind 1.
    - De parent roept `waitpid` aan voor Kind 2.
    - De exit-status van het *laatste* commando (`grep`) wordt opgeslagen in `shell->last_exit_status`. Als `grep` niks vond, zou de exit status `1` zijn. Als het wel iets vond, is de status `0`. Als het commando niet bestond (bv. `grpe`), zou de `execve` falen en zou het kind `exit(127)` aanroepen, wat de uiteindelijke status wordt.

---

### Stap 6: Cleanup en Terugkeer

1.  De `pipeline` array en alle `t_simple_cmd` structuren worden vrijgegeven.
2.  De `tokens` array wordt vrijgegeven.
3.  De `line` van `readline` wordt vrijgegeven.
4.  De `shell_loop` is aan het einde van een iteratie. Hij springt terug naar het begin en roept `readline` opnieuw aan voor een nieuwe prompt.

De cyclus is compleet. 