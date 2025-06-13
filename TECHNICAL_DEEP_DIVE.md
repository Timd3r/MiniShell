# MiniShell: Een Technische Diepgaande Analyse

Dit document biedt een gedetailleerde technische uitleg van het MiniShell-project. Het doel is om de innerlijke werking, de architectuurkeuzes en de implementatie van elke feature te doorgronden, specifiek gericht op de code in dit project.

## Hoofdstuk 1: Algemene Architectuur en De Hoofd-Loop

Een shell is in essentie een oneindige lus die 4 stappen herhaalt:
1.  **Lezen**: Lees de input van de gebruiker.
2.  **Parsen (Lexen & Parsen)**: Analyseer de input en zet deze om in een begrijpelijke datastructuur.
3.  **Uitvoeren**: Voer de commando's uit.
4.  **Wachten**: Wacht tot de commando's klaar zijn en begin opnieuw.

Onze MiniShell volgt deze cyclus nauwgezet. De kern van de shell bevindt zich in `src/minishell.c`.

### 1.1 De `main` Functie: Het Startpunt

Alles begint in de `main` functie. Deze is verantwoordelijk voor de initiële setup:

```c
// src/minishell.c

int	main(void)
{
	char		*prompt;
	t_shell		shell;
	extern char	**environ;

	shell.env = environ;
	shell.last_exit_status = 0;
	prompt = "\033[1;36mMiniShell\033[0m\033[1;31m> \033[0m";
	setup_signals();
	shell_loop(&shell, prompt);
	clear_history();
	return (0);
}
```

**Uitleg voor de verdediging:**

*   **`t_shell shell`**: We initialiseren een `t_shell` struct. Dit is de 'context' van onze shell. Het bevat de omgevingsvariabelen (`shell.env`) en de exit-status van het laatst uitgevoerde commando (`shell.last_exit_status`). Dit is cruciaal voor de `env` built-in en voor het expanderen van `$?`.
*   **`shell.env = environ`**: We kopiëren niet de hele omgeving, maar slaan de pointer naar `environ` op. Dit is efficiënt en geeft ons direct toegang tot de omgevingsvariabelen. `export` en `unset` zullen deze later moeten aanpassen.
*   **`setup_signals()`**: Dit is een cruciale stap. Hier configureren we hoe de shell reageert op signalen zoals `ctrl-C` (SIGINT) en `ctrl-\` (SIGQUIT). We komen hier later uitgebreid op terug.
*   **`shell_loop(&shell, prompt)`**: Dit is het kloppend hart van de shell. We geven de `shell` context mee, zodat de loop toegang heeft tot de status en omgeving.

### 1.2 De `shell_loop`: De Oneindige Cyclus

De `shell_loop` in `src/minishell.c` implementeert de "read-parse-execute" cyclus.

```c
// src/minishell.c

static void	shell_loop(t_shell *shell, char *prompt)
{
	char	*line;

	while (1)
	{
		g_signal_received = 0;
		line = readline(prompt); // Stap 1: Lezen
		if (!line)
		{
			handle_eof_shell(shell); // Ctrl-D
			break ;
		}
		if (g_signal_received == SIGINT) // Ctrl-C
		{
			shell->last_exit_status = 130;
			free(line);
			continue ;
		}
		if (*line)
		{
			process_line(line, shell); // Stap 2 & 3: Parsen & Uitvoeren
		}
		free(line);
	}
}
```

**Uitleg voor de verdediging:**

*   **`while(1)`**: De oneindige lus die de shell draaiende houdt.
*   **`g_signal_received = 0`**: Voor elke nieuwe prompt resetten we onze globale signaalvariabele. Dit is de enige globale variabele, zoals het subject vereist.
*   **`line = readline(prompt)`**: We gebruiken de `readline` bibliotheek. Deze functie toont de prompt, laat de gebruiker typen (met ondersteuning voor pijltjestoetsen, etc.) en geeft de ingevoerde regel terug.
*   **`if (!line)`**: `readline` retourneert `NULL` als de gebruiker `ctrl-D` indrukt. We roepen dan `handle_eof_shell` aan, die "exit" print en de shell afsluit.
*   **`if (g_signal_received == SIGINT)`**: Als de gebruiker `ctrl-C` indrukt terwijl `readline` wacht op input, wordt onze signaalhandler geactiveerd. De handler zet `g_signal_received` op `SIGINT`. De loop detecteert dit, zet de exit-status op `130` (standaard voor `bash`), en begint opnieuw met een nieuwe prompt.
*   **`process_line(line, shell)`**: Als er input is, wordt deze doorgegeven aan `process_line`. Deze functie coördineert het hele proces van parsen en uitvoeren.

De `process_line` functie is een simpele coördinator:

```c
// src/minishell.c

static void	process_line(char *line, t_shell *shell)
{
	t_token	**tokens;

	tokens = make_tokens(line); // Stap 2a: Lexer
	if (tokens)
	{
		add_history(line);
		execute_tokens(tokens, shell); // Stap 2b & 3: Parser & Executor
		free_tokens_array(tokens);
	}
}
```

In het volgende hoofdstuk duiken we in `make_tokens`, het begin van onze parsing-pijplijn. 

## Hoofdstuk 2: Lexical Analysis - Van String naar Tokens

Voordat we complexe structuren zoals `pipelines` kunnen begrijpen, moeten we de ruwe input-string opdelen in betekenisvolle eenheden. Dit proces heet **lexical analysis**, of **lexing**. Het resultaat is een reeks **tokens**.

Een token is een structuur die een stukje van de input representeert. In ons project is dit de `t_token` struct, gedefinieerd in `minishell.h`:

```c
// minishell.h

typedef enum e_token_type
{
	WORD,       // Een woord, bv. "ls", "-la", "file.txt"
	PIPE,       // Een pipe |
	IN,         // Input redirect <
	OUT,        // Output redirect >
	HEREDOC,    // Here-document <<
	APPEND      // Append redirect >>
}	t_token_type;

typedef struct s_token
{
	t_token_type	type; // Het type token
	char			*c;   // De letterlijke string-waarde
}	t_token;
```

De functie `make_tokens` in `src/minishell.c` is verantwoordelijk voor dit proces.

### 2.1 `make_tokens`: De Tokenizer

Deze functie doorloopt de input-string karakter voor karakter en bouwt een array van `t_token` pointers (`t_token **`).

```c
// src/minishell.c

t_token	**make_tokens(char *line)
{
	t_token		**tokens;
	int			token_idx;
	const char	*current_pos;
	int			ret;

	tokens = malloc(sizeof(t_token *) * (ft_strlen(line) + 1));
	// ... foutafhandeling ...
	token_idx = 0;
	current_pos = line;
	while (*current_pos)
	{
		skip_whitespace(&current_pos);
		if (*current_pos == '\0')
			break ;
		ret = handle_token_type(&current_pos, &tokens, &token_idx); // De kernlogica
		if (ret == 0 || ret == -1)
			return (free_tokens_array(tokens), NULL);
	}
	tokens[token_idx] = NULL; // NULL-terminatie
	return (tokens);
}
```

**Uitleg voor de verdediging:**

*   **`while (*current_pos)`**: De lus scant de hele input-string.
*   **`skip_whitespace(&current_pos)`**: We negeren alle spaties en tabs tussen tokens.
*   **`handle_token_type(...)`**: Dit is de belangrijkste functie. Het kijkt naar het huidige karakter (`*current_pos`) en beslist welk type token het is (een pipe, een redirect, of een woord). Het maakt het token aan en voegt het toe aan de `tokens` array.
*   **`tokens[token_idx] = NULL`**: Net als `argv` in `main`, maken we onze token-array `NULL`-getermineerd. Dit maakt het makkelijker om er later doorheen te lussen.

### 2.2 `handle_token_type`: Speciale Karakters vs. Woorden

De logica voor het herkennen van tokens zit in `src/token_handlers.c`. De `handle_token_type` functie is een dispatcher die de juiste handler aanroept.

```c
// src/token_handlers.c (vereenvoudigd)

int	handle_token_type(const char **line, t_token ***tokens, int *token_idx)
{
    if (**line == '|')
        return (handle_pipe_token(tokens, token_idx, line));
    if (**line == '<')
        return (handle_less_token(tokens, token_idx, line));
    if (**line == '>')
        return (handle_greater_token(tokens, token_idx, line));
    else
        return (handle_word_token(tokens, token_idx, line));
}
```

**Uitleg voor de verdediging:**

*   **Speciale karakters (`|`, `<`, `>`)**: Deze worden direct herkend. `handle_less_token` kijkt bijvoorbeeld of er een tweede `<` is om een `HEREDOC` token te maken in plaats van een `IN` token.
*   **Woorden (`handle_word_token`)**: Alles wat geen speciaal karakter is, wordt beschouwd als onderdeel van een 'woord'. Deze functie is slim: het scant tot het een spatie of een ander speciaal karakter tegenkomt.
    *   **Belangrijk**: Quotes worden hier nog **niet** geïnterpreteerd. Een string als `"hello world"` wordt op dit punt gezien als één token van het type `WORD` met de waarde `"hello world"`. De interpretatie van de quotes gebeurt pas later, in de parser-fase. Dit is een belangrijke scheiding van verantwoordelijkheden.

Voorbeeld: `cat < file.txt | grep "hello"` wordt omgezet naar de volgende token-reeks:
1.  `{WORD, "cat"}`
2.  `{IN, "<"}`
3.  `{WORD, "file.txt"}`
4.  `{PIPE, "|"}`
5.  `{WORD, "grep"}`
6.  `{WORD, ""hello""}`

**Een Noot over Fouten**: De lexer is ook onze eerste verdedigingslinie tegen ongeldige input. Als het een onverwacht karakter tegenkomt, zoals `;` of `&` (die niet in de mandatory part horen), zal `handle_token_type` een fout retourneren. Dit zorgt ervoor dat de parsing-pijplijn onmiddellijk stopt, en de gebruiker een "syntax error" krijgt, net als in een echte shell.

Met deze getokeniseerde input kunnen we nu door naar de volgende fase: de Parser.

## Hoofdstuk 3: De Parser - Van Tokens naar Commando's

De parser heeft als taak om de platte `t_token` array om te zetten in een of meerdere **simpele commando's**. Een simpel commando is een programma met zijn argumenten en eventuele I/O-redirections. In ons project is dit de `t_simple_cmd` struct.

```c
// minishell.h (vereenvoudigd)

typedef struct s_simple_cmd
{
	char	**args;         // bv. {"ls", "-la", NULL}
	char	*input_file;    // Bestandsnaam voor < of <<
	char	*output_file;   // Bestandsnaam voor > of >>
	int		is_heredoc;     // Vlag voor <<
	int		append_mode;    // Vlag voor >>
}	t_simple_cmd;
```

Het parsen gebeurt in twee hoofdfasen, gecoördineerd door `execute_tokens` in `src/minishell.c`.

### 3.1 Fase 1: Splitsen op Pipes

De shell moet eerst weten of er een `pipeline` is. Een pipeline is een reeks commando's verbonden door `|`.

```c
// src/minishell.c

static void	execute_tokens(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	**pipeline;

	if (count_pipes(tokens) > 0) // Is er een pipeline?
	{
		pipeline = split_by_pipes(tokens); // Ja -> splits de tokens op
		// ... voer pipeline uit ...
	}
	else
	{
		// Nee -> behandel als één enkel commando
		cmd = parse_simple_command_shell(tokens, shell);
		// ... voer enkel commando uit ...
	}
}
```

De functie `split_by_pipes` in `src/pipe_handler.c` neemt de token-array en splitst deze op bij elke `PIPE` token. Het resultaat is een `NULL`-getermineerde array van `t_simple_cmd` pointers (`t_simple_cmd **`), waarbij elke `t_simple_cmd` een commando in de pipeline voorstelt.

### 3.2 Fase 2: `parse_simple_command_shell`

Dit is de kern van de parser, te vinden in `src/parser.c`. Deze functie neemt een (deel)lijst van tokens en vult een `t_simple_cmd` struct.

```c
// src/parser.c

t_simple_cmd *parse_simple_command_shell(t_token **tokens, t_shell *shell)
{
    // ... initialisatie van cmd ...
	
	while (tokens[i])
	{
		if (tokens[i]->type == WORD)
		{
			// Voeg toe aan cmd->args
		}
		else if (process_redirections(cmd, tokens, &i)) // bv. <, >, >>, <<
		{
			// De redirection wordt verwerkt
		}
		i++;
	}
	cmd->args[arg_idx] = NULL;
	if (shell)
		process_arg_expansion(cmd, shell); // ZEER BELANGRIJK!
	return (cmd);
}
```

**Uitleg voor de verdediging:**

*   **Iteratie**: De functie doorloopt de tokens.
*   **`WORD` token**: Als het een `WORD` is, wordt de string gekopieerd en toegevoegd aan de `cmd->args` array. `ls -l` resulteert in `cmd->args = {"ls", "-l", NULL}`.
*   **Redirection token**: Als het een redirection-token is (`IN`, `OUT`, etc.), wordt `process_redirections` aangeroepen. Deze functie pakt het volgende `WORD` token (de bestandsnaam) en slaat dit op in `cmd->input_file` of `cmd->output_file`. Het zet ook de `is_heredoc` of `append_mode` vlag indien nodig. De redirection-tokens zelf worden "verbruikt" en komen niet in `cmd->args` terecht.
*   **`process_arg_expansion(cmd, shell)`**: Dit is de **cruciale laatste stap** van de parser. Nadat alle argumenten en bestandsnamen zijn verzameld, wordt deze functie aangeroepen. Het doorloopt alle strings in `cmd->args` en voert twee belangrijke operaties uit:
    1.  **Quote Removal**: Het verwijdert de aanhalingstekens. `"hello"` wordt `hello`.
    2.  **Variable Expansion**: Het vervangt variabelen (`$VAR`, `$?`) door hun waarde. Dit gebeurt alleen buiten enkele aanhalingstekens en binnen dubbele aanhalingstekens.

Deze laatste stap is zo belangrijk dat we er een volledig apart hoofdstuk aan wijden. Na deze stap is de `t_simple_cmd` struct volledig voorbereid en klaar voor uitvoering. 

## Hoofdstuk 4: Expansie - Quotes en Variabelen

De regels voor expansie in `bash` zijn complex, maar volgen een duidelijke logica. Onze shell implementeert dit in `src/quote_handler.c` en `src/env_expansion.c`. De hoofd-coördinator is `process_token_quotes` in `quote_handler.c`.

### 4.1 De Gouden Regels van Expansie

1.  **Enkele Aanhalingstekens (`'...'`)**: Alles tussen enkele quotes wordt letterlijk genomen. Geen enkele expansie vindt plaats. `'hello $USER'` blijft de string `'hello $USER'`.
2.  **Dubbele Aanhalingstekens (`"..."`)**: Variabele-expansie (`$VAR`, `$?`) vindt plaats. Andere speciale karakters worden genegeerd. `"hello $USER"` wordt `hello mikellen`.
3.  **Buiten Aanhalingstekens**: Variabele-expansie vindt plaats.

Onze implementatie volgt deze regels door een string karakter voor karakter te doorlopen.

### 4.2 De Implementatie: `process_quote_loop`

De functie `process_quote_loop` in `src/quote_handler.c` is het hart van dit proces.

```c
// src/quote_handler.c

static char	*process_quote_loop(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;

	result = ft_calloc(1, 1);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			temp = handle_single_quote(str, &i);
		else if (str[i] == '"')
			temp = handle_double_quote(str, &i, shell);
		else
			temp = process_unquoted_char(str, &i, shell);
		result = ft_strjoin_free(result, temp);
		free(temp);
	}
	return (result);
}
```

**Uitleg voor de verdediging:**

*   **State Machine**: Dit is een 'state machine' op karakter-niveau. Het kijkt naar het huidige karakter en beslist wat te doen.
*   **`handle_single_quote`**: Als het een `'` tegenkomt, leest deze functie (`src/quote_handler_utils.c`) alles tot de volgende `'` en geeft dit terug als een letterlijke string. De index `i` wordt voorbij de sluitende quote gezet.
*   **`handle_double_quote`**: Als het een `"` tegenkomt, leest deze functie (`src/quote_handler_utils.c`) alles tot de volgende `"`. Vervolgens roept het `expand_variables` aan op de inhoud, en geeft het resultaat terug.
*   **`process_unquoted_char`**: Voor elk ander karakter, kijkt het of het een `$` is. Zo ja, dan wordt de variabele geëxpandeerd. Zo nee, dan wordt het karakter gewoon gekopieerd.

### 4.3 Variabele Expansie: `expand_variables`

Deze functie, in `src/env_expansion.c`, is verantwoordelijk voor het vervangen van `$VAR`-constructies.

```c
// src/env_expansion.c

char	*handle_variable_expansion(char *str, int *i, t_shell *shell)
{
	char	*var_name;
	char	*value;
	// ...
	(*i)++; // Sla de '$' over
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->last_exit_status)); // Speciale geval: $?
	}
	// ... lees de naam van de variabele (bv. "USER") ...
	var_name = ft_substr(str, start, len);
	value = getenv(var_name); // Zoek de waarde op
	free(var_name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup("")); // Variabele niet gevonden? Geef lege string terug.
}
```

**Uitleg voor de verdediging:**

*   **`$`-detectie**: De functie wordt aangeroepen wanneer een `$` is gevonden.
*   **Speciaal geval: `$?`**: Het eerste wat we controleren is of het teken na de `$` een `?` is. Zo ja, dan halen we `shell->last_exit_status` op, converteren dit naar een string met `ft_itoa`, en retourneren het.
*   **Variabelen lezen**: Anders lezen we de alfanumerieke karakters (plus `_`) die de naam van de variabele vormen.
*   **`getenv()`**: We gebruiken de standaard `getenv()` functie om de waarde van de variabele in de omgeving op te zoeken.
*   **Lege string**: Als `getenv()` `NULL` retourneert (de variabele bestaat niet), geven we een lege string terug. Dit is het standaard gedrag in `bash`.

Na dit hoofdstuk is de input volledig geparset en geëxpandeerd. De `t_simple_cmd` structs zijn nu klaar om te worden uitgevoerd door de `Executor`. 

## Hoofdstuk 5: De Executor - Het Uitvoeren van Commando's

De Executor is de motor van de shell. Het neemt de `t_simple_cmd` structuren en zorgt ervoor dat ze worden uitgevoerd. De coördinatie gebeurt in `src/executor.c`.

### 5.1 De Hoofd-Dispatcher: `execute_simple_command_shell`

Deze functie is de centrale hub. Het beslist hoe een commando moet worden uitgevoerd.

```c
// src/executor.c

int	execute_simple_command_shell(t_simple_cmd *cmd, t_shell *shell)
{
	// ...
	if (is_builtin(cmd->args[0])) // Is het een built-in commando?
	{
		// ... logica voor built-ins ...
	}
	// Zoek het uitvoerbare bestand in de PATH
	executable_path = find_executable_path(cmd->args[0]);
	if (!executable_path)
	{
		// ... "command not found" error ...
		return (127);
	}
	// Voer het externe commando uit
	status = execute_external_shell(cmd, executable_path, shell);
	return (status);
}
```

**Uitleg voor de verdediging:**

1.  **Check op Built-in**: Het eerste wat we doen is controleren of het commando ( `cmd->args[0]` ) een van onze ingebouwde functies is ( `echo`, `cd`, etc.). De functie `is_builtin` (`src/builtin.c`) vergelijkt de string met een lijst van bekende built-ins.
2.  **Built-in Uitvoeren**: Als het een built-in is, wordt deze direct uitgevoerd. We komen hier in een later hoofdstuk op terug.
3.  **Extern Commando**: Als het geen built-in is, behandelen we het als een extern programma (zoals `ls`, `grep`, `cat`, etc.).
    *   **`find_executable_path`**: Deze functie (`src/system_utils.c`) zoekt naar het uitvoerbare bestand. Het kijkt eerst of het pad een absoluut pad is (bv. `/bin/ls`). Zo niet, dan doorzoekt het alle directories in de `$PATH` omgevingsvariabele. Als het bestand nergens wordt gevonden, geeft het `NULL` terug.
    *   **Foutafhandeling**: Als `find_executable_path` `NULL` teruggeeft, printen we de "command not found" foutmelding en retourneren we exit code 127, precies zoals `bash` dat doet.
    *   **`execute_external_shell`**: Als het bestand is gevonden, roepen we deze functie aan om het programma daadwerkelijk uit te voeren.

### 5.2 Externe Commando's: `fork` en `execve`

Het uitvoeren van een extern programma gebeurt altijd in een apart proces. Dit is essentieel: als we `execve` in ons hoofdproces zouden aanroepen, zou ons MiniShell-programma worden vervangen door het nieuwe programma en stoppen. De `fork()` systeemaanroep is hier de oplossing. De logica zit in `src/external_exec.c`.

```c
// src/external_exec.c (vereenvoudigde weergave)

int execute_external_shell(t_simple_cmd *cmd, char *path, t_shell *shell)
{
    pid_t pid;

    pid = fork(); // Creëer een nieuw (kind)proces
    if (pid == -1)
    {
        // Fout bij forken
        return (1);
    }
    if (pid == 0)
    {
        // DIT IS HET KINDPROCES
        setup_exec_signals(); // Herstel signalen naar default
        if (handle_redirections(cmd) != 0) // Pas I/O redirections toe
            exit(1);
        execve(path, cmd->args, shell->env); // Vervang dit proces
        // Als execve faalt, komt de code hier
        perror("minishell");
        exit(126);
    }
    else
    {
        // DIT IS HET PARENTPROCES (ONZE MINISHELL)
        return (wait_for_child(pid)); // Wacht op het kindproces
    }
}
```

**Uitleg voor de verdediging:**

*   **`pid = fork()`**: `fork()` splitst het proces in tweeën. Het retourneert `0` in het nieuwe kindproces en de Process ID (PID) van het kind in het parentproces.
*   **In het Kindproces (`pid == 0`)**:
    *   **`setup_exec_signals()`**: Het is cruciaal om de signaalhandlers te resetten. Een lopend programma zoals `cat` moet stoppen bij `ctrl-C`, niet een nieuwe prompt tonen. `SIG_DFL` (default) herstelt dit gedrag.
    *   **`handle_redirections(cmd)`**: Vóór het uitvoeren passen we eventuele I/O-redirections toe met `dup2`. Als `cmd->output_file` is ingesteld, wordt `stdout` omgeleid naar dat bestand.
    *   **`execve(path, cmd->args, shell->env)`**: Dit is de belangrijkste systeemaanroep. Het vervangt het huidige (kind)proces volledig door het programma dat op `path` staat. Het krijgt de argumenten (`cmd->args`) en de omgeving (`shell->env`) mee.
    *   **`exit(126)`**: Als `execve` om de een of andere reden faalt (bv. geen uitvoerrechten), zal het terugkeren. In dat geval moeten we het kindproces onmiddellijk beëindigen. Exit code 126 is de conventie hiervoor.
*   **In het Parentproces (`pid > 0`)**:
    *   **`wait_for_child(pid)`**: De shell (parent) moet wachten tot het commando (kind) klaar is. `waitpid()` pauzeert het parentproces tot het kind eindigt en geeft de exit-status van het kind terug. Deze status wordt dan opgeslagen in `shell->last_exit_status` voor `$?`.

### 5.3 Pipelines: De `pipe` en `dup2` Dans

Het uitvoeren van een pipeline zoals `cmd1 | cmd2` is complexer dan een enkel commando. We moeten:
1.  Een communicatiekanaal (een `pipe`) opzetten tussen de twee processen.
2.  Beide commando's tegelijkertijd starten, elk in zijn eigen `fork`.
3.  De `stdout` van `cmd1` verbinden met de schrijf-kant van de pipe.
4.  De `stdin` van `cmd2` verbinden met de lees-kant van de pipe.
5.  Wachten op *alle* kindprocessen.

De logica hiervoor zit in `src/pipe_handler_utils.c`. De `execute_pipeline_loop` functie is de dirigent.

```c
// src/pipe_handler_utils.c

int	execute_pipeline_loop(t_simple_cmd **cmds, t_shell *shell, int cmd_count)
{
	int					**pipes;
	int					i;

	// Maak N-1 pipes aan voor N commando's
	pipes = create_pipes(cmd_count - 1);
	// ...

	i = 0;
	while (i < cmd_count)
	{
		// Fork een kindproces voor ELK commando in de pipeline
		execute_piped_command_shell(params);
		i++;
	}

	close_all_pipes(pipes, cmd_count - 1); // Cruciaal in de parent!
	status = wait_for_children(cmd_count); // Wacht op alle kinderen
	free_pipes(pipes, cmd_count - 1);
	return (status);
}
```

De echte magie zit in `execute_piped_command_shell`. Deze functie wordt aangeroepen voor elk commando in de pipeline.

```c
// src/pipe_handler_utils.c

void	execute_piped_command_shell(t_pipe_cmd_params *params)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0) // We zijn in een kindproces
	{
		// Commando 1: `ls -l | grep ...` -> geen input-pipe nodig
		if (params->idx > 0)
			dup2(params->pipes[params->idx - 1][0], STDIN_FILENO); // Lees van vorige pipe

		// Commando 2: `... | grep .c` -> geen output-pipe nodig
		if (params->idx < params->total - 1)
			dup2(params->pipes[params->idx][1], STDOUT_FILENO); // Schrijf naar huidige pipe

		close_all_pipes(params->pipes, params->total - 1); // Heel belangrijk!
		exit(execute_simple_command_shell(params->cmd, params->shell));
	}
}
```

**Uitleg voor de verdediging (dit is een cruciaal onderdeel):**

*   **`create_pipes`**: Voor een pipeline van `N` commando's hebben we `N-1` pipes nodig. `ls | grep | wc` heeft 2 pipes nodig.
*   **De Loop**: We `forken` onmiddellijk alle processen. Ze zullen (bijna) tegelijkertijd draaien.
*   **`dup2` Magie**:
    *   `dup2(oldfd, newfd)` kopieert de file descriptor `oldfd` naar `newfd`. Als `newfd` al open was (zoals `STDIN_FILENO`), wordt deze eerst stilzwijgend gesloten.
    *   **Input**: Als dit niet het *eerste* commando is (`idx > 0`), wordt zijn `stdin` de *lees-kant* (`[0]`) van de *vorige* pipe.
    *   **Output**: Als dit niet het *laatste* commando is (`idx < total - 1`), wordt zijn `stdout` de *schrijf-kant* (`[1]`) van zijn *eigen* pipe.
*   **`close_all_pipes`**: Dit is **extreem belangrijk**. Nadat een kindproces zijn `stdin` en `stdout` heeft ingesteld met `dup2`, heeft het de originele pipe-file-descriptors niet meer nodig. **Elk proces (zowel parent als alle children) moet alle pipe-uiteindes sluiten die het niet gebruikt.** Als de parent de schrijf-kant van een pipe openhoudt, zal het kind dat van die pipe leest nooit een `EOF` (End-Of-File) ontvangen en voor altijd blijven wachten. Dit is een veelvoorkomende oorzaak van 'hanging' shells.
*   **`wait_for_children`**: De parent roept `waitpid` aan in een lus, één keer voor elk kind dat het heeft gemaakt. Het retourneert de exit-status van het *laatste* commando in de pipeline, wat het standaard gedrag is.

**Visuele Voorstelling van `cmd1 | cmd2`:**
```
            +---------------------------+
            |      Parent (Minishell)   |
            +---------------------------+
              | fork()        | fork()
      +-------+               +-------+
      |                       |
+-----v------+           +----v-------+
|  Kind 1    |           |  Kind 2    |
|  (cmd1)    |           |  (cmd2)    |
| stdin=term |           | stdin=p[0] | <---+
| stdout=p[1] >----------+ stdout=term|     |
+------------+           +------------+     |
      ^                                     |
      |          +-------------------+      |
      +----------| pipefd[1] | pipefd[0] |------+
                 +-------------------+
```

### 5.4 I/O Redirections: Bestanden als Input en Output

Redirections veranderen waar een commando zijn input vandaan haalt of zijn output naartoe schrijft. De implementatie is verdeeld over `src/redirections.c` en `src/redirection_utils.c`.

De afhandeling gebeurt in het kindproces, net voordat `execve` wordt aangeroepen. De functie `handle_redirections` in `redirection_utils.c` is hier verantwoordelijk voor.

```c
// src/redirection_utils.c

int	handle_redirections(t_simple_cmd *cmd)
{
	int	input_fd;
	int	output_fd;

	if (cmd->input_file)
	{
		input_fd = open_input_file(cmd); // Opent < of <<
		if (input_fd == -1)
			return (1);
		dup2(input_fd, STDIN_FILENO); // Leid stdin om
		close(input_fd);
	}
	if (cmd->output_file)
	{
		output_fd = open_output_file(cmd); // Opent > of >>
		if (output_fd == -1)
			return (1);
		dup2(output_fd, STDOUT_FILENO); // Leid stdout om
		close(output_fd);
	}
	return (0);
}
```

**Uitleg voor de verdediging:**

*   **Timing**: Redirections worden afgehandeld in het kindproces. Dit is cruciaal, want we willen niet de `stdin` of `stdout` van onze hoofd-shell permanent veranderen.
*   **Input (`<`)**:
    *   `open_input_file` opent het bestand dat in `cmd->input_file` staat met `O_RDONLY`.
    *   `dup2(input_fd, STDIN_FILENO)` zorgt ervoor dat wanneer het programma (bv. `cat`) van `stdin` probeert te lezen, het eigenlijk uit het geopende bestand leest.
*   **Output (`>` en `>>`)**:
    *   `open_output_file` opent het bestand `cmd->output_file` met de juiste flags:
        *   `O_WRONLY | O_CREAT | O_TRUNC`: Voor `>` (schrijven, aanmaken indien niet bestaat, leegmaken indien wel bestaat).
        *   `O_WRONLY | O_CREAT | O_APPEND`: Voor `>>` (schrijven, aanmaken indien niet bestaat, toevoegen aan het einde).
    *   `dup2(output_fd, STDOUT_FILENO)` zorgt ervoor dat alle output die normaal naar de terminal zou gaan, nu naar het bestand wordt geschreven.
*   **`close(fd)`**: Nadat `dup2` is aangeroepen, is de originele file descriptor (`input_fd` of `output_fd`) niet meer nodig. De "link" is gelegd naar `STDIN_FILENO` of `STDOUT_FILENO`, dus we kunnen de originele descriptor sluiten om 'lekkage' te voorkomen.

### 5.5 De Here-Document (`<<`)

De here-document is een speciaal type input redirection. `cat << EOF` leest input van de terminal tot het de delimiter `EOF` tegenkomt. Dit vereist een unieke aanpak.

Onze implementatie in `src/redirections.c` is slim en robuust: we gebruiken een `pipe` en een `fork`.

```c
// src/redirections.c

int	handle_heredoc(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	pipe(pipefd);
	pid = fork();
	if (pid == 0) // Kindproces (de "lezer")
	{
		close(pipefd[0]); // Sluit de ongebruikte lees-kant
		heredoc_child_process(pipefd[1], delimiter); // Leest van terminal, schrijft naar pipe
	}
	// Parentproces
	close(pipefd[1]); // Sluit de ongebruikte schrijf-kant
	waitpid(pid, &status, 0); // Wacht tot de gebruiker de delimiter heeft ingevoerd
	
	// ... foutafhandeling voor ctrl-C ...
	
	return (pipefd[0]); // Geef de LEES-KANT van de pipe terug
}
```

**Uitleg voor de verdediging:**

1.  **Waarom `fork` en `pipe`?**: We moeten de input van de gebruiker ergens tijdelijk opslaan. Een pipe is hier perfect voor. We `forken` een proces dat de terminal-input leest en in de pipe schrijft.
2.  **Het Kindproces**: Het kindproces (`heredoc_child_process`) roept `readline` in een lus aan. Elke regel die de gebruiker intypt, wordt in de schrijf-kant van de pipe (`pipefd[1]`) geschreven. Dit gaat door tot de gebruiker de `delimiter` intypt. Dan sluit het kindproces en `exit` het.
3.  **Het Parentproces**: Het parentproces (de hoofd-executor-fork) sluit de schrijf-kant en wacht op het kind.
4.  **De Teruggavewaarde**: De functie geeft `pipefd[0]` terug, de *lees-kant* van de pipe.
5.  **Integratie**: In `handle_redirections` wordt deze file descriptor (de lees-kant van de pipe) vervolgens met `dup2` aan de `STDIN_FILENO` van het commando gekoppeld. Het commando leest dus zijn input uit de pipe die wij gevuld hebben.
6.  **`ctrl-C` in Here-doc**: Wat als de gebruiker `ctrl-C` drukt *tijdens* het invoeren van een here-document? Ons `heredoc_child_process` heeft de signaalhandler op `SIG_DFL` gezet, dus het kindproces stopt onmiddellijk. Het `waitpid` in de parent detecteert dit (`WIFSIGNALED`). De parent sluit dan de `pipefd[0]` en retourneert `-1` om aan te geven dat de operatie is afgebroken. De hele commando-uitvoering stopt dan.

Dit beëindigt onze diepe duik in de Executor. We hebben nu een compleet beeld van hoe externe commando's, pipelines en redirections worden uitgevoerd.

## Hoofdstuk 6: De Built-in Commando's

Built-ins zijn commando's die de shell zelf uitvoert. Sommige, zoals `cd` en `export`, *moeten* built-ins zijn omdat ze de state van het shell-proces zelf moeten aanpassen. Een kindproces kan de werkdirectory van zijn parent niet veranderen.

De logica start in `src/executor.c` en wordt gedispatched vanuit `src/builtin.c`.

### 6.1 `cd`

Verandert de huidige werkdirectory. De implementatie zit in `src/builtin2.c`.

```c
// src/builtin2.c
int	builtin_cd(t_simple_cmd *cmd)
{
	char	*path;

	if (!cmd->args[1]) // Geen argument? Ga naar HOME
		path = getenv("HOME");
	else
		path = cmd->args[1];
	
	if (chdir(path) == -1) // De kern-aanroep
	{
		perror(path);
		return (1); // Fout
	}
	update_pwd_env(); // Update PWD en OLDPWD
	return (0); // Succes
}
```

*   **`chdir(path)`**: De `chdir` systeemaanroep doet al het werk.
*   **`update_pwd_env`**: Na een succesvolle `chdir`, moeten we de `PWD` en `OLDPWD` omgevingsvariabelen bijwerken. Dit is cruciaal voor `pwd` en `cd -`.

### 6.2 `echo`

Print de argumenten naar `stdout`. Implementatie in `src/builtin.c`.

*   **`-n` vlag**: Controleert of het eerste argument `"-n"` is. Zo ja, dan wordt er geen nieuwe regel aan het einde geprint.
*   **Argumenten**: Loopt door alle argumenten en print ze met een spatie ertussen.

### 6.3 `pwd`

Print de huidige werkdirectory.

*   Gebruikt de `getcwd` systeemaanroep om het huidige pad op te halen en print dit naar `stdout`.

### 6.4 `export` en `unset`

Beheert omgevingsvariabelen. Dit is een van de complexere built-ins.

*   **`export VAR=value`**: Voegt een nieuwe variabele toe of wijzigt een bestaande.
*   **`export VAR`**: Markeer een lokale variabele voor export (minder relevant in ons project).
*   **`export`**: Toont een lijst van alle omgevingsvariabelen, vaak gesorteerd en met `declare -x` ervoor.
*   **`unset VAR`**: Verwijdert een variabele.

**Implementatie-aanpak:** Omdat we de `environ` pointer gebruiken, kunnen we deze niet zomaar `realloc`-en. De meest robuuste aanpak is om een **nieuwe `char **` array** te alloceren wanneer de omgeving verandert.
1.  **Tellen**: Tel hoeveel pointers de nieuwe omgeving zal hebben.
2.  **Alloceren**: `malloc` een nieuwe array van de juiste grootte (`count + 1` voor de `NULL` terminator).
3.  **Kopiëren**: Loop door de oude `shell->env`. Kopieer de pointers van de variabelen die je wilt behouden naar de nieuwe array. Voeg de nieuwe `VAR=value` string toe.
4.  **Vervangen**: `free` de oude `shell->env` array (niet de strings zelf, want die zijn hergebruikt!). Wijs `shell->env` toe aan de nieuwe array.
Dit is een delicate operatie, maar essentieel voor een correct werkende `export` en `unset`.

### 6.5 `env`

Print de huidige omgevingsvariabelen. De implementatie in `src/builtin2.c` is eenvoudig:

*   Negeert alle argumenten.
*   Loopt door `shell->env` en print elke string, gevolgd door een newline.

### 6.6 `exit`

Beëindigt de shell.

*   **Exit Status**: Als er een numeriek argument is (`exit 99`), wordt dit de exit-status. `bash` gebruikt `status % 256`.
*   **Geen argument**: Als er geen argument is, wordt de exit-status van het laatst uitgevoerde commando gebruikt (`shell->last_exit_status`).
*   **Cleanup**: Voordat het programma stopt, moeten `clear_history()` en andere cleanup-functies worden aangeroepen.

### 6.7 Built-ins en Redirections

Wat gebeurt er bij `echo "hello" > file.txt`? De built-in `echo` moet zijn output naar `file.txt` schrijven, niet naar de terminal.

```c
// src/executor.c

if (is_builtin(cmd->args[0]))
{
    if (cmd->input_file || cmd->output_file)
        return (execute_builtin_with_redirect(cmd, shell)); // Fork!
    return (execute_builtin_shell(cmd, shell)); // Geen fork
}
```

**Uitleg voor de verdediging:**
*   **De Regel**: Als een built-in wordt gebruikt **zonder** I/O redirection, voeren we het direct uit in het hoofdproces.
*   **De Uitzondering**: Als een built-in wordt gebruikt **met** I/O redirection, `forken` we toch! In het kindproces wordt de redirection afgehandeld met `dup2` en wordt de built-in uitgevoerd. Het kindproces `exit` dan. Dit voorkomt dat we de `stdout` van onze hoofd-shell permanent omleiden.

## Hoofdstuk 7: Signaalafhandeling (`ctrl-C`, `ctrl-\`, `ctrl-D`)

Een interactieve shell moet correct reageren op gebruikerssignalen. Dit wordt geregeld in `src/signal_handler.c` met behulp van de `sigaction` systeemaanroep, die de voorkeur heeft boven `signal` omdat het meer controle biedt.

Het subject stelt een belangrijke eis: **er mag maar één globale variabele zijn**. Onze `volatile sig_atomic_t g_signal_received` voldoet hieraan.
*   `volatile`: Zegt tegen de compiler dat de waarde van deze variabele op elk moment kan veranderen, en dat optimalisaties (zoals caching in een register) moeten worden vermeden.
*   `sig_atomic_t`: Garandeert dat het lezen of schrijven van deze variabele een 'atomaire' operatie is, die niet kan worden onderbroken door een ander signaal.

We hebben twee 'modi' voor signalen:
1.  **Interactieve modus**: Wanneer de shell wacht op input.
2.  **Executie modus**: Wanneer er een commando draait.

### 7.1 Interactieve Modus: `setup_signals`

Deze wordt aangeroepen bij de start van de shell.

```c
// src/signal_handler.c
void	setup_signals(void)
{
	// Setup voor SIGINT (ctrl-C)
	sa_int.sa_handler = handle_sigint_interactive;
	sigaction(SIGINT, &sa_int, NULL);

	// Setup voor SIGQUIT (ctrl-\)
	sa_quit.sa_handler = SIG_IGN; // Negeer het signaal
	sigaction(SIGQUIT, &sa_quit, NULL);
}
```

*   **`ctrl-\` (SIGQUIT)**: Wordt volledig genegeerd (`SIG_IGN`). De gebruiker ziet niks gebeuren.
*   **`ctrl-C` (SIGINT)**: Roept de `handle_sigint_interactive` handler aan.

```c
// src/signal_handler.c
void	handle_sigint_interactive(int sig)
{
	(void)sig;
	g_signal_received = SIGINT; // Zet de globale vlag!
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}
```
**Uitleg van de flow:**
1.  Gebruiker drukt `ctrl-C`.
2.  Het OS pauzeert `readline` en roept `handle_sigint_interactive` aan.
3.  De handler zet `g_signal_received` en gebruikt `readline`-functies om een nieuwe, lege prompt te tonen.
4.  De handler eindigt. Het OS hervat de `readline`-aanroep.
5.  `readline` wordt onderbroken en retourneert. De `shell_loop` ziet `g_signal_received == SIGINT`, zet de exit status naar `130`, en start de lus opnieuw.

### 7.2 Executie Modus: `setup_exec_signals`

Wanneer een extern commando draait, willen we dat het zich als normaal gedraagt. `cat` moet stoppen bij `ctrl-C`. Dit doen we door de signaalafhandeling te resetten naar de *default* actie in het kindproces, net voor `execve`.

```c
// src/external_exec.c (in het kindproces)
setup_exec_signals();
execve(...);

// src/signal_handler.c
void	setup_exec_signals(void)
{
	// Herstel default gedrag voor SIGINT en SIGQUIT
	sa_int.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa_int, NULL);
	
	sa_quit.sa_handler = SIG_DFL;
	sigaction(SIGQUIT, &sa_quit, NULL);
}
```

*   **`SIG_DFL`**: De default actie voor `SIGINT` is het beëindigen van het proces. Voor `SIGQUIT` is dit het beëindigen en het maken van een 'core dump'. Dit is precies wat we willen. De shell zelf negeert deze signalen (omdat de parent zijn eigen handlers houdt), maar het lopende commando ontvangt ze wel en reageert er standaard op.

### 7.3 `ctrl-D` (EOF)

Dit is geen signaal. `ctrl-D` zorgt ervoor dat de `read` systeemaanroep (diep binnen `readline`) `0` bytes teruggeeft. `readline` interpreteert dit als End-Of-File (EOF) en retourneert `NULL`.

```c
// src/minishell.c in shell_loop
line = readline(prompt);
if (!line) // Gevolg van ctrl-D
{
    handle_eof_shell(shell);
    break ;
}

// src/signal_handler.c
void	handle_eof_shell(t_shell *shell)
{
	write(2, "exit\n", 5);
	clear_history();
	// ... andere cleanup ...
	exit(shell->last_exit_status);
}
```
De shell print "exit" naar de terminal en sluit zichzelf netjes af.

## Hoofdstuk 8: Geheugenbeheer

Het C-project vereist handmatig geheugenbeheer. Een lek (`leak`) kan leiden tot een 0. Onze strategie is om geheugen vrij te geven zodra het niet meer nodig is. De functies hiervoor zijn gegroepeerd in `src/free_utils.c`.

### 8.1 De Levenscyclus van Geheugen

1.  **Input**: `readline` alloceert de `line` string. Deze wordt aan het einde van elke `shell_loop` iteratie vrijgegeven met `free(line)`.
2.  **Tokens**: `make_tokens` alloceert de `t_token **tokens` array en elke `t_token->c` string. Dit wordt volledig opgeruimd met `free_tokens_array(tokens)` aan het einde van `process_line`.
3.  **Commando's**: `parse_simple_command_shell` en `split_by_pipes` alloceren de `t_simple_cmd` structuren en hun `args` arrays. Deze worden opgeruimd met `free_simple_cmd` of `free_pipeline` zodra de commando's zijn uitgevoerd.

De flow is als volgt:
*   Alloceer voor de huidige commandolijn.
*   Gebruik de data.
*   Geef alles weer vrij.
*   Begin opnieuw voor de volgende commandolijn.

Dit zorgt ervoor dat het geheugengebruik van de shell stabiel blijft en niet groeit over tijd.

### 8.2 Hulpfuncties

In `src/free_utils.c` staan functies zoals:

*   **`free_tokens_array(t_token **tokens)`**: Loopt door de array, `free`t eerst elke `token->c` string, dan de `t_token` struct zelf, en tenslotte de `tokens` array.
*   **`free_simple_cmd(t_simple_cmd *cmd)`**: Geeft de `args` array, de `input_file` en `output_file` strings, en de `cmd` struct zelf vrij.
*   **`free_pipeline(t_simple_cmd **pipeline)`**: Loopt door de pipeline-array en roept `free_simple_cmd` aan voor elk commando, en `free`t dan de pipeline-array zelf.

Door deze hiërarchische aanpak en het consequent aanroepen van deze functies na gebruik, verzekeren we dat er geen geheugen lekt.

## Hoofdstuk 9: Syntaxfouten en Edge Cases

Een robuuste shell moet ook omgaan met ongeldige input.
*   **Ongeldige tokens**: Zoals hierboven vermeld, worden karakters als `;` en `&` direct in de lexer afgevangen.
*   **Parseringsfouten**: De parser controleert op logische fouten. Voorbeelden:
    *   `| cmd`: Een pipe aan het begin van een regel.
    *   `cmd | | cmd`: Een dubbele pipe.
    *   `> > file`: Een dubbele redirection.
    *   `cmd >`: Een redirection zonder bestandsnaam.
    
Onze parser-logica controleert dit impliciet. `handle_output_redir` verwacht bijvoorbeeld een `WORD` token na een `>` token. Als het een ander `>` token of `NULL` vindt, faalt de parsing. De shell retourneert dan een fout (meestal status 2) en geeft een nieuwe prompt, zonder iets uit te voeren.

## Conclusie

Dit MiniShell-project implementeert de verplichte functionaliteiten op een robuuste en modulaire manier. De belangrijkste architecturale keuzes zijn:

1.  **Duidelijke Scheiding**: Het proces is opgedeeld in duidelijke, logische fasen: **Lexer -> Parser -> Expander -> Executor**. Elke fase heeft zijn eigen verantwoordelijkheid en datastructuur (`t_token`, `t_simple_cmd`).
2.  **Correct Procesbeheer**: `fork` wordt correct gebruikt om externe commando's en pipelines in aparte processen uit te voeren, waardoor de hoofd-shell intact blijft.
3.  **Zorgvuldig FD-Management**: File descriptors voor pipes en redirections worden systematisch beheerd met `dup2` en `close` om 'hanging' en 'leaks' te voorkomen.
4.  **Robuuste Signaalafhandeling**: Signalen worden context-afhankelijk behandeld, wat zorgt voor een stabiele, interactieve ervaring die overeenkomt met `bash`.
5.  **Gedisciplineerd Geheugenbeheer**: Geheugen wordt direct na gebruik vrijgegeven, wat leidt tot een stabiel en lekvrij programma.

Het project voldoet aan alle eisen van het `mandatory` gedeelte van het subject en demonstreert een diep begrip van Unix-proces- en bestandsbeheer.