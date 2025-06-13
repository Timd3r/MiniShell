# Diepgaande Analyse van Kernfuncties

Dit document biedt een gedetailleerde, code-gerichte analyse van de belangrijkste en meest complexe functies in het MiniShell-project. Het doel is om de implementatie tot in de details te begrijpen.

---

## 1. `execute_pipeline_loop` uit `src/pipe_handler_utils.c`

Deze functie is de motor achter het uitvoeren van pipelines zoals `cmd1 | cmd2 | cmd3`. Het is verantwoordelijk voor het creëren van de processen en de communicatiekanalen (pipes) ertussen.

### De Code

```c
// src/pipe_handler_utils.c

int	execute_pipeline_loop(t_simple_cmd **cmds, t_shell *shell, int cmd_count)
{
	int					**pipes;
	int					i;
	int					status;
	t_pipe_cmd_params	params;

	// 1. Maak de communicatiekanalen aan
	pipes = create_pipes(cmd_count - 1);
	if (!pipes)
		return (1);

	// 2. Initialiseer de parameters voor de kindprocessen
	i = 0;
	params.total = cmd_count;
	params.shell = shell;

	// 3. Start alle kindprocessen
	while (i < cmd_count)
	{
		params.cmd = cmds[i];
		params.pipes = pipes;
		params.idx = i;
		execute_piped_command_shell(&params);
		i++;
	}

	// 4. Sluit alle pipes in de parent
	close_all_pipes(pipes, cmd_count - 1);

	// 5. Wacht op alle kinderen en haal de laatste exit status op
	status = wait_for_children(cmd_count);

	// 6. Ruim het geheugen van de pipes op
	free_pipes(pipes, cmd_count - 1);
	return (status);
}
```

### Analyse per Blok

**Blok 1: `create_pipes(cmd_count - 1)`**
- **Doel**: Een pipeline met `N` commando's heeft `N-1` pipes nodig om ze te verbinden. Deze functie alloceert een 2D-array `int **pipes` en roept voor elke rij de `pipe()` systeemaanroep aan.
- **Implementatie**: `create_pipes` maakt een array van `int*` pointers. Dan, in een lus, alloceert het voor elke pointer een `int[2]` en roept `pipe()` aan. Als een van deze stappen faalt, ruimt het al het eerder gealloceerde geheugen op om lekken te voorkomen. Dit is robuuste foutafhandeling.

**Blok 2: Initialisatie `params`**
- **Doel**: In plaats van 5 losse argumenten door te geven aan de functie die het kindproces `forkt`, bundelen we alles in een `t_pipe_cmd_params` struct. Dit is schonere en beter onderhoudbare code.
- **`params.total`**: Het totale aantal commando's. Dit heeft een kind nodig om te weten of het het *laatste* commando is (en dus niet naar een pipe hoeft te schrijven).
- **`params.shell`**: De pointer naar de shell-context, nodig voor variabele-expansie en het ophalen van de exit-status.

**Blok 3: De `while`-lus en `execute_piped_command_shell`**
- **Doel**: Dit is de "lanceerlus". Het start een kindproces voor *elk* commando in de `cmds`-array.
- **`params.idx = i`**: Het is cruciaal dat elk kind zijn eigen index kent. Een kind met `idx=0` is het eerste commando. Een kind met `idx=1` is het tweede, etc. Dit bepaalt welke pipes het moet gebruiken voor zijn input en output.
- **Belangrijk**: De parent `forkt` en gaat onmiddellijk door naar de volgende iteratie zonder te wachten. Hierdoor worden alle processen in de pipeline bijna gelijktijdig gestart.

**Blok 4: `close_all_pipes(pipes, ...)`**
- **Doel**: Dit is een van de meest kritieke stappen voor een correct werkende pipeline.
- **Waarom?**: Nadat alle kinderen zijn geforkt, heeft de parent de pipes niet meer nodig. Als de parent de *schrijf*-kant van een pipe (`pipefd[1]`) openhoudt, zal het kindproces dat van de *lees*-kant (`pipefd[0]`) leest, nooit een `EOF` (End-of-File) signaal krijgen. Het kind blijft voor eeuwig wachten op meer input, zelfs als het andere kind al klaar is met schrijven. Dit veroorzaakt een 'hang' of 'deadlock'. Door alle pipe-uiteindes in de parent te sluiten, wordt dit voorkomen.

**Blok 5: `wait_for_children(cmd_count)`**
- **Doel**: De shell moet wachten tot de *hele* pipeline is voltooid voordat het een nieuwe prompt toont.
- **Implementatie**: Deze functie roept `waitpid(-1, &status, 0)` aan in een lus, `cmd_count` keer. `waitpid(-1, ...)` wacht op *eender welk* kindproces.
- **Exit Status**: De functie slaat de status van elk kind op, maar retourneert uiteindelijk alleen de status van het *laatste* kind dat is gestart. Het is de conventie dat de exit-status van een pipeline de status is van het laatste commando, ongeacht of eerdere commando's zijn mislukt. `ls /nonexistent | wc -l` resulteert in een `ls: ... No such file or directory` fout, maar de exit-status van de hele pipeline is `0` omdat `wc -l` succesvol was. Jouw implementatie bootst dit correct na.

**Blok 6: `free_pipes(...)`**
- **Doel**: Netjes het geheugen opruimen dat door `create_pipes` is gealloceerd.
- **Implementatie**: Het loopt door de 2D-array en `free`t eerst elke `int[2]`, en daarna de `int ** ` array zelf.
- **Efficiëntie**: Hoewel het bouwen van een string in een lus met `strjoin` niet de allersnelste methode is, is het voor de input-groottes van een shell-commando meer dan snel genoeg en veel eenvoudiger te implementeren en te debuggen dan complexe buffer-management technieken.

---

## 2. `process_quote_loop` uit `src/quote_handler.c`

Deze functie is de kern van de Expander-fase. Het neemt een "rauwe" string van een token (die nog quotes en dollartekens kan bevatten) en transformeert deze naar de uiteindelijke string die als argument aan `execve` wordt meegegeven. Het is de implementatie van de complexe `bash`-regels voor quote- en variabele-expansie.

### De Code

```c
// src/quote_handler.c

static char	*process_quote_loop(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;

	// 1. Initialiseer een lege resultaat-string
	result = ft_calloc(1, 1);
	i = 0;

	// 2. Loop door de input-string karakter voor karakter
	while (str[i])
	{
		// 3. Beslis wat te doen gebaseerd op het huidige karakter
		if (str[i] == '\'')
			temp = handle_single_quote(str, &i);
		else if (str[i] == '"')
			temp = handle_double_quote(str, &i, shell);
		else
			temp = process_unquoted_char(str, &i, shell);
		
		// 4. Voeg het verwerkte stukje toe aan het eindresultaat
		result = ft_strjoin_free(result, temp);
		free(temp);
	}
	return (result);
}
```

### Analyse per Blok

**Blok 1: `result = ft_calloc(1, 1)`**
- **Doel**: We bouwen een nieuwe string op. We kunnen de input-string niet ter plekke aanpassen, omdat de uiteindelijke string langer of korter kan zijn (bv. `$USER` wordt `mikellen`).
- **Implementatie**: We beginnen met een lege, `NULL`-getermineerde string. `ft_calloc(1, 1)` is een veilige manier om dit te doen.

**Blok 2: `while (str[i])`**
- **Doel**: Dit is de hoofd-lus die de input-string `str` van begin tot eind afhandelt. De `i` wordt niet simpelweg met `i++` verhoogd, maar wordt beheerd door de `handle_...` functies.

**Blok 3: De `if-else if-else` dispatcher**
- **Doel**: Dit is de kern van de state machine. Het kijkt naar het karakter waar we ons bevinden en kiest de juiste strategie.
- **`if (str[i] == '\'')`**: Als we een enkele quote vinden, geven we de controle over aan `handle_single_quote`. Deze functie leest alles tot de volgende enkele quote, geeft dit stuk *letterlijk* terug, en schuift de index `i` op tot *na* de sluitende quote.
- **`if (str[i] == '"')`**: Als we een dubbele quote vinden, roepen we `handle_double_quote` aan. Deze functie leest de inhoud tot de volgende dubbele quote, voert daarop `expand_variables` uit, en geeft het resultaat terug. Cruciaal is dat de expansie dus *alleen* op de inhoud van de dubbele quotes wordt toegepast.
- **`else`**: Als het geen quote is, behandelen we het als een 'normaal' karakter. `process_unquoted_char` wordt aangeroepen. Deze functie kijkt of dit karakter een `$` is. Zo ja, dan wordt de variabele-expansie afgehandeld. Zo nee, dan wordt simpelweg dat ene karakter als een string teruggegeven.

**Blok 4: `result = ft_strjoin_free(result, temp)`**
- **Doel**: Het resultaat van de `handle_...` functie (`temp`) moet aan onze `result`-string worden toegevoegd.
- **Implementatie**: Een standaard `ft_strjoin` zou een geheugenlek veroorzaken, omdat de oude `result`-string niet wordt vrijgegeven. `ft_strjoin_free` is een custom hulpfunctie die `ft_strjoin` aanroept en daarna de *oude* `result` en de `temp` string `free`t. Dit is een efficiënte en schone manier om een string dynamisch op te bouwen in een lus.

### Waarom deze aanpak?

- **Modulariteit**: Door de logica op te splitsen in `handle_single_quote`, `handle_double_quote` en `process_unquoted_char`, blijft de hoofd-lus extreem simpel en leesbaar. Elke functie heeft één duidelijke taak.
- **State Management via `i`**: De index `i` wordt doorgegeven als een pointer (`&i`). Dit stelt de hulpfuncties in staat om de "cursor" in de input-string direct te verplaatsen. `handle_single_quote` kan bijvoorbeeld `i` met 10 posities opschuiven, en de hoofd-lus gaat dan gewoon verder vanaf die nieuwe positie. Dit is een elegante manier om de 'state' (onze positie in de string) te beheren zonder complexe datastructuren.
- **Efficiëntie**: Hoewel het bouwen van een string in een lus met `strjoin` niet de allersnelste methode is, is het voor de input-groottes van een shell-commando meer dan snel genoeg en veel eenvoudiger te implementeren en te debuggen dan complexe buffer-management technieken.

**Voorbeeld stap-voor-stap:**
Stel de input-string is `"$USER's file"`.
1.  `i=0`, `char='"'`: `process_quote_loop` roept `handle_double_quote` aan.
    - `handle_double_quote` scant tot de volgende `"`, vindt de inhoud `$USER`.
    - Het roept `expand_variables` aan op `$USER`, wat `mikellen` teruggeeft.
    - `handle_double_quote` retourneert de string `"mikellen"`.
    - De hoofd-lus zet `i` op de positie na de sluitende quote. `result` is nu `"mikellen"`.
2.  `i` is nu bij `'`: `process_quote_loop` roept `handle_single_quote` aan.
    - `handle_single_quote` scant tot de volgende `'`, vindt de inhoud `s file`.
    - Het retourneert deze letterlijke string: `"s file"`.
    - `i` wordt na de sluitende quote gezet.
3.  `result` wordt `ft_strjoin_free("mikellen", "s file")`, wat resulteert in de uiteindelijke string `"mikellen's file"`.
4.  De lus eindigt. De functie retourneert de correct verwerkte string.

---

## 3. `handle_heredoc` uit `src/redirections.c`

De here-document (`<<`) is een unieke vorm van input. De shell moet interactief input lezen van de gebruiker en deze input *tijdelijk opslaan* om later als `stdin` aan een commando te geven. Deze functie lost dit op met een `pipe` en een `fork`.

### De Code
```c
// src/redirections.c

int	handle_heredoc(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	// 1. Maak een anonieme pipe
	if (pipe(pipefd) == -1)
		return (-1);
	
	// 2. Fork het proces
	pid = fork();
	if (pid == 0)
	{
		// 3. Kindproces: de "Input Vanger"
		heredoc_child_process(pipefd[1], delimiter);
	}

	// 4. Parentproces: de "Dirigent"
	close(pipefd[1]); // Sluit de ongebruikte schrijf-kant
	waitpid(pid, &status, 0); // Wacht op de vanger

	// 5. Controleer of de vanger is onderbroken
	if (WIFSIGNALED(status))
	{
		close(pipefd[0]);
		return (-1);
	}
	
	// 6. Geef de lees-kant van de gevulde pipe terug
	return (pipefd[0]);
}
```

### Analyse per Blok

**Blok 1: `pipe(pipefd)`**
- **Doel**: We hebben een tijdelijke buffer nodig om de input van de gebruiker in op te slaan. Een pipe is hier perfect voor. Het is een I/O-kanaal in het geheugen met een lees-kant (`pipefd[0]`) en een schrijf-kant (`pipefd[1]`).
- **Waarom geen bestand?**: We zouden een tijdelijk bestand op de schijf kunnen aanmaken, maar dat is langzamer (I/O naar schijf) en complexer (we moeten een unieke bestandsnaam vinden en het bestand later weer verwijderen). Een pipe is schoner en efficiënter.

**Blok 2: `fork()`**
- **Doel**: We moeten twee dingen tegelijk doen: de shell moet input lezen van de gebruiker (met `readline`), en het hoofdcommando moet wachten tot al die input verzameld is.
- **De Oplossing**: We `forken` een speciaal kindproces dat zich alleen bezighoudt met het vangen van de here-doc input. De parent (de hoofd-shell) wacht simpelweg op dit kind.

**Blok 3: `heredoc_child_process`**
- **Taak**: Dit kindproces is de "Input Vanger".
- **`pipefd[1]`**: Het krijgt de *schrijf*-kant van de pipe mee.
- **Implementatie**: In een `while`-lus roept het `readline("> ")` aan. Elke regel die de gebruiker typt, wordt vergeleken met de `delimiter`.
    - Is de regel niet de delimiter? Schrijf de regel (plus een `\n`) naar `pipefd[1]`.
    - Is de regel wel de delimiter? Breek de lus, sluit `pipefd[1]` en roep `exit(0)` aan.
- **Belangrijk**: Dit kindproces sluit de `pipefd[0]` (de lees-kant) die het niet gebruikt.

**Blok 4: Parentproces logica**
- **`close(pipefd[1])`**: Net als bij pipelines is het cruciaal dat de parent zijn ongebruikte kant van de pipe sluit. De parent zal alleen lezen (indirect, via het uiteindelijke commando), dus het sluit de schrijf-kant.
- **`waitpid(pid, &status, 0)`**: De parent pauzeert hier. Het programma stopt en wacht tot het "Input Vanger"-kind klaar is (d.w.z. tot de gebruiker de delimiter heeft ingetypt).

**Blok 5: `WIFSIGNALED(status)`**
- **Doel**: Robuuste foutafhandeling. Wat als de gebruiker `ctrl-C` drukt *tijdens* het intypen van de here-doc?
- **Werking**: Het "Input Vanger"-kind heeft zijn signaalhandler op `SIG_DFL` gezet. `ctrl-C` beëindigt dat kind dus onmiddellijk. `waitpid` keert terug, en `WIFSIGNALED(status)` zal `true` zijn.
- **Actie**: Als dit gebeurt, is de here-doc mislukt. We moeten de `pipefd[0]` sluiten (om een file descriptor leak te voorkomen) en `-1` retourneren om de fout aan te geven. De hele commando-uitvoering wordt dan geannuleerd.

**Blok 6: `return (pipefd[0])`**
- **Doel**: De missie is geslaagd. De pipe is gevuld met de input van de gebruiker.
- **Resultaat**: De functie geeft de *lees*-kant van de pipe terug. De `handle_redirections` functie zal deze file descriptor ontvangen en met `dup2(pipefd[0], STDIN_FILENO)` koppelen aan de `stdin` van het uiteindelijke commando. Het commando zal dus zijn input lezen uit de pipe die we hier gevuld hebben.

---

Dit concludeert de diepgaande analyse. Deze drie functies vormen een representatieve doorsnede van de complexiteit en de elegante oplossingen in het MiniShell-project.

---

## 4. `execute_simple_command_shell` uit `src/executor.c`

Dit is de hoogste "verkeersregelaar" in de executor. Nadat de parser zijn werk heeft gedaan, ontvangt deze functie een enkele, volledig voorbereide `t_simple_cmd` en beslist wat ermee moet gebeuren.

### De Code
```c
// src/executor.c

int	execute_simple_command_shell(t_simple_cmd *cmd, t_shell *shell)
{
	char	*executable_path;
	int		status;

	// 1. Sanity check
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	
	// 2. Is het een built-in?
	if (is_builtin(cmd->args[0]))
	{
		// 3. Speciale aanpak voor built-ins met redirection
		if (cmd->input_file || cmd->output_file)
			return (execute_builtin_with_redirect(cmd, shell));
		// 4. Normale aanpak voor built-ins
		return (execute_builtin_shell(cmd, shell));
	}
	
	// 5. Zoek het externe commando
	executable_path = find_executable_path(cmd->args[0]);
	if (!executable_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return (127);
	}

	// 6. Voer het externe commando uit
	status = execute_external_shell(cmd, executable_path, shell);
	return (status);
}
```

### Analyse per Blok

1.  **Sanity Check**: Een lege commandolijn (bv. `""`) kan leiden tot een `cmd` struct zonder argumenten. Deze check voorkomt een crash.
2.  **`is_builtin()`**: De eerste, belangrijkste splitsing. De functie `is_builtin` vergelijkt de commandonaam (`cmd->args[0]`) met een hardgecodeerde lijst (`"echo"`, `"cd"`, `"pwd"`, etc.).
3.  **Built-in met Redirection**: Dit is een cruciaal "edge case". `cd > file` mag niet de `stdout` van de *hele shell* permanent omleiden. Daarom `forken` we in dit geval *toch*. In het kindproces worden de redirections afgehandeld (`dup2`) en wordt de built-in uitgevoerd. Het kind `exit` dan. De parent (onze shell) blijft ongewijzigd.
4.  **Normale Built-in**: `cd` zonder redirection moet in de parent draaien om de werkdirectory van de shell te kunnen veranderen. Hier wordt de built-in functie dus direct aangeroepen.
5.  **`find_executable_path`**: Als het geen built-in is, is het een extern commando. We moeten het vinden. Deze functie (die we hierna bespreken) doorzoekt de `$PATH`. Als het `NULL` teruggeeft, bestaat het commando niet. We printen een `bash`-conforme foutmelding en retourneren exit code `127`.
6.  **`execute_external_shell`**: Als het pad is gevonden, geven we de controle over aan de functie die het `fork`/`execve` proces regelt.

---

## 5. `handle_redirections` uit `src/redirection_utils.c`

Deze functie is de praktische uitvoerder van de I/O-omleidingen. Het wordt aangeroepen *in het kindproces*, net voor `execve`.

### De Code
```c
// src/redirection_utils.c

int	handle_redirections(t_simple_cmd *cmd)
{
	int	input_fd;
	int	output_fd;

	// 1. Handel input redirection af (< of <<)
	if (cmd->input_file)
	{
		input_fd = open_input_file(cmd);
		if (input_fd == -1)
			return (1); // Fout, bv. bestand niet gevonden
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}

	// 2. Handel output redirection af (> of >>)
	if (cmd->output_file)
	{
		output_fd = open_output_file(cmd);
		if (output_fd == -1)
			return (1); // Fout, bv. geen permissie
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	return (0); // Succes
}
```

### Analyse per Blok

1.  **Input Redirection**:
    - `open_input_file` is een helper die de `cmd->is_heredoc` vlag controleert. Als die waar is, roept het `handle_heredoc` aan (die de `fd` van de pipe teruggeeft). Anders opent het `cmd->input_file` met `O_RDONLY`.
    - `dup2(input_fd, STDIN_FILENO)`: Dit is de kern. De file descriptor voor de standaard input (`0`) wordt een kopie van de descriptor van ons geopende bestand of pipe. Vanaf nu leest het proces van het bestand/pipe wanneer het van `stdin` probeert te lezen.
    - `close(input_fd)`: De originele file descriptor (`input_fd`, bv. `3`) is nu overbodig, want kanaal `0` wijst nu naar dezelfde bron. We sluiten hem om geen descriptors te lekken.

2.  **Output Redirection**:
    - `open_output_file` is een helper die de `cmd->append_mode` vlag controleert om te beslissen tussen `O_TRUNC` (voor `>`) en `O_APPEND` (voor `>>`).
    - `dup2(output_fd, STDOUT_FILENO)`: Hetzelfde principe, maar voor de standaard output (`1`). Alles wat naar kanaal `1` wordt geschreven, gaat nu naar het geopende bestand.
    - `close(output_fd)`: Opnieuw, opruimen van de overbodige descriptor.

Als een van de `open`- of `dup2`-aanroepen faalt, geeft de functie `1` terug. Dit zorgt ervoor dat het kindproces `exit(1)` aanroept en `execve` nooit bereikt.

---

## 6. `find_executable_path` uit `src/system_utils.c`

Deze functie implementeert een van de meest fundamentele features van een shell: het vinden van commando's.

### De Logica
1.  **Check op `/`**: Eerst wordt de commandonaam (bv. `ls` of `/bin/ls`) gecontroleerd. Als er een `/` in zit, gaat de shell ervan uit dat de gebruiker een specifiek pad heeft opgegeven (absoluut of relatief). De shell zal dan *niet* in `$PATH` zoeken. Het controleert alleen of dát specifieke bestand bestaat en uitvoerbaar is.

2.  **Ophalen van `$PATH`**: Als er geen `/` is, wordt `getenv("PATH")` aangeroepen. Dit geeft een string terug zoals `"/usr/bin:/bin:/usr/sbin"`.

3.  **Splitsen van `$PATH`**: De string wordt gesplitst op de `:` met `ft_split`. Dit resulteert in een `char**` array: `{"/usr/bin", "/bin", "/usr/sbin", NULL}`.

4.  **Itereren en Testen**: De code loopt nu door deze array. Voor elke directory:
    a. Het bouwt een volledig pad. Eerst wordt `"/usr/bin"` samengevoegd met `"/"` tot `"/usr/bin/"`.
    b. Dan wordt de commandonaam `ls` eraan toegevoegd, wat `"/usr/bin/ls"` vormt.
    c. De `access(path, X_OK)` systeemaanroep wordt gebruikt. Dit is een efficiënte manier om te controleren of een bestand bestaat én of we de permissie hebben om het uit te voeren.

5.  **Succes en Foutafhandeling**:
    - **Gevonden**: Zodra `access` slaagt, wordt de zojuist geconstrueerde path-string (`"/usr/bin/ls"`) geretourneerd. Het geheugen voor de gesplitste array wordt netjes opgeruimd.
    - **Niet Gevonden**: Als de lus eindigt en het commando in geen van de directories is gevonden, wordt het geheugen van de gesplitste array opgeruimd en wordt `NULL` geretourneerd, wat leidt tot de "command not found" fout.

Dit concludeert de diepgaande analyse. Deze drie functies vormen een representatieve doorsnede van de complexiteit en de elegante oplossingen in het MiniShell-project. 