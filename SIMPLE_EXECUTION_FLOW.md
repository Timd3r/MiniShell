# De Levenscyclus van een Simpel Commando: `ls -l`

Dit document legt de volledige, gedetailleerde flow uit van een simpel extern commando. We volgen de reis van `ls -l` door onze shell, van toetsaanslag tot output.

---

### Stap 1: De Prompt en de Input (`shell_loop`)

1.  **De Lus**: Onze shell draait in de `while(1)`-lus in `shell_loop`.
2.  **Wachten op Input**: De functie `readline("Minishell> ")` wordt aangeroepen. Het programma pauzeert en de cursor knippert, wachtend op de gebruiker.
3.  **Gebruiker typt**: De gebruiker voert `ls -l` in en drukt op **Enter**.
4.  **Input Ontvangen**: `readline` retourneert de string `"ls -l"`. Deze wordt opgeslagen in de `char *line` variabele. De `while`-lus gaat verder.

---

### Stap 2: Lexical Analysis - De "Lexer" (`make_tokens`)

De `process_line` functie ontvangt de string `"ls -l"` en geeft deze door aan `make_tokens`.

1.  **Scannen**: `make_tokens` doorloopt de string.
2.  Het vindt het woord `ls`, creëert een `t_token` `{ type: WORD, c: "ls" }` en voegt deze toe aan de `tokens`-array.
3.  Het slaat de spatie over (`skip_whitespace`).
4.  Het vindt het woord `-l`, creëert een `t_token` `{ type: WORD, c: "-l" }` en voegt deze toe.
5.  Het einde van de string is bereikt.

**Resultaat**: Een `NULL`-getermineerde `t_token**` array:
- `tokens[0]`: `{ type: WORD, c: "ls" }`
- `tokens[1]`: `{ type: WORD, c: "-l" }`
- `tokens[2]`: `NULL`

---

### Stap 3: Syntactic Analysis - De "Parser" (`parse_simple_command_shell`)

De `tokens`-array wordt doorgegeven aan `execute_tokens`.

1.  **Pipe Check**: `count_pipes(tokens)` wordt aangeroepen. Er zijn geen `PIPE` tokens, dus het retourneert `0`.
2.  **Geen Pipeline**: Omdat er geen pipes zijn, wordt de `else`-tak genomen en wordt `parse_simple_command_shell(tokens, &shell)` aangeroepen.
3.  **Structuur Vullen**:
    a. Een `t_simple_cmd *cmd` wordt gealloceerd.
    b. De parser loopt door de tokens. Beide tokens zijn van het type `WORD`. Hun `c`-strings (`"ls"` en `"-l"`) worden gedupliceerd en in de `cmd->args` array geplaatst.
    c. Er zijn geen redirection-tokens, dus die logica wordt overgeslagen.
    d. De `cmd->args` array wordt `NULL`-getermineerd.
    e. `cmd->input_file` en `cmd->output_file` blijven `NULL`.

**Resultaat**: Een `t_simple_cmd` struct met:
- `cmd->args`: `{"ls", "-l", NULL}`
- `cmd->input_file`: `NULL`
- `cmd->output_file`: `NULL`

---

### Stap 4: Expansie (`process_arg_expansion`)

De `parse_simple_command_shell` functie roept als laatste `process_arg_expansion` aan op de `args`.

1.  **Analyse**: De functie inspecteert de strings `"ls"` en `"-l"`.
2.  **Conclusie**: Geen van beide strings bevat aanhalingstekens (`'` of `"`) of een dollarteken (`$`).
3.  **Actie**: Er is niets te expanderen of te verwijderen. De `args` blijven ongewijzigd.

Het commando is nu volledig voorbereid en klaar voor uitvoering.

---

### Stap 5: De "Executor" (`execute_simple_command_shell`)

De `t_simple_cmd` struct wordt doorgegeven aan `execute_simple_command_shell`.

1.  **Built-in Check**: `is_builtin("ls")` wordt aangeroepen. Dit retourneert `false`.
2.  **Pad Zoeken**: De `if`-statement voor built-ins wordt overgeslagen. `find_executable_path("ls")` wordt aangeroepen.
    a. Deze functie haalt de `$PATH` omgevingsvariabele op.
    b. Het splitst de `PATH` op in directories.
    c. Het zoekt in elke directory (bv. `/bin`) naar een uitvoerbaar bestand genaamd `ls`.
    d. Het vindt `/bin/ls` en retourneert deze string. `executable_path` is nu `"/bin/ls"`.
3.  **Externe Uitvoering**: De functie roept `execute_external_shell(cmd, "/bin/ls", &shell)` aan.

---

### Stap 6: Het Werkelijke Commando (`fork` & `execve`)

Binnen `execute_external_shell`:

1.  **`fork()`**: De shell roept `fork()` aan. Nu zijn er twee processen: de Parent (onze MiniShell) en een Kind (een exacte kopie).
2.  **In het Kindproces (`pid == 0`)**:
    a. **Signalen Herstellen**: `setup_exec_signals()` wordt aangeroepen om `ctrl-C` en `ctrl-\` hun standaardgedrag terug te geven.
    b. **Redirections**: `handle_redirections(cmd)` wordt aangeroepen. Omdat `cmd->input_file` en `cmd->output_file` `NULL` zijn, doet deze functie niets.
    c. **`execve()`**: De `execve("/bin/ls", cmd->args, shell->env)` systeemaanroep wordt uitgevoerd.
    d. **Transformatie**: Het besturingssysteem gooit de code van de MiniShell-kloon weg en laadt de code van het `/bin/ls` programma in het geheugen van het kindproces. Het kindproces *wordt* nu het `ls`-proces. Het voert de `ls -l` operatie uit en print de output naar zijn `stdout`, die nog steeds naar de terminal wijst.
    e. **Einde Kind**: Als `ls` klaar is, roept het intern `exit(0)` aan (aangezien de operatie succesvol was). Het kindproces wordt beëindigd.
3.  **In het Parentproces (`pid > 0`)**:
    a. **Wachten**: De parent (onze MiniShell) roept `wait_for_child(pid)` aan, wat neerkomt op `waitpid(pid, &status, 0)`.
    b. **Pauze**: De MiniShell pauzeert en wacht tot het `ls`-kindproces klaar is.
    c. **Resultaat**: Wanneer het kind eindigt, keert `waitpid` terug. De `status` variabele bevat de exit-code `0`.
    d. **Status Opslaan**: De functie retourneert `0`. `execute_simple_command_shell` slaat dit op in `shell->last_exit_status`.

---

### Stap 7: Opruimen en de Cyclus Herhalen

1.  De `execute_tokens` functie is nu klaar.
2.  Het `t_simple_cmd` struct wordt vrijgegeven met `free_simple_cmd(cmd)`.
3.  De `tokens`-array wordt vrijgegeven met `free_tokens_array(tokens)`.
4.  De originele `line` (`"ls -l"`) wordt vrijgegeven met `free(line)`.
5.  De `shell_loop` is aan het einde van een iteratie. Hij springt terug naar het begin, `g_signal_received` wordt op `0` gezet, en `readline` wordt opnieuw aangeroepen voor de volgende prompt.

De cyclus is compleet. 