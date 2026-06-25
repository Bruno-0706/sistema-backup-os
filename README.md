# Objetivo do Projeto

Este projeto implementa um sistema simples de backup versionado em C.

O programa monitora arquivos presentes no diretório de origem e cria cópias de backup sempre que detecta modificações (diferença de tempo entre elas...).

O objetivo é exercitar conceitos de Sistemas Operacionais relacionados a:

- Sistemas de arquivos
- Diretórios
- Metadados de arquivos
- System Calls
- _(Um pouco de)_ Manipulação de arquivos em C

## Fluxo:
```Fluxo
	Dada um diretório origem.
	Uma varredura é feita.		
	Para cada arquivo em origem
		uma cópia pode ser feita em diretório de backups, caso uma codição de tempo seja
	entendida
```

---

# Como executar o programa?

### Requisitos:
```Requisitos
- Linux
- GCC
```

>[!warning]
>O Código foi feito com LINUX em mente.
>Isso não vai funcionar em computadores com Windows (diferentes formas de chamadas ao sistema!)
>Se em um Windows, tente um WLS (testei com Ubuntu) ou dê boot nas máquinas da sala no Mint 

---
### Compilar:
``` shell
gcc -o backup_program main.c
```

### Executar:
``` Shell
./backup_program
```

### Estrutura esperada:
```estrutura
.
├── origem/
│   ├── arquivo1.txt
│   └── arquivo2.txt
│
├── backups/
└── backup_program
```

#### Print do nosso projeto:
![[Pasted image 20260624110502.png]]

---

# Funcionamento Geral

```Fluxo
1. O programa abre o diretório origem.
2. Percorre todos os arquivos.
3. Obtém a data de modificação.
4. Procura o backup mais recente.
5. Compara os tempos.
6. Cria um novo backup se necessário.
```


---

# Conceitos utilizados


### Bibliotecas:

``` C
#include <stdio.h>

#include <unistd.h>   // para o sleep()

#include <dirent.h>   // operações com diretórios

#include <sys/stat.h> // obter status, stat()

#include <string.h>   //para manipulação com strings, principalmente para nome de Paths

#include <time.h>

#include <sys/time.h>
```


### Diretórios:

```C
opendir()
readdir()
closedir()
```

### Metadados dos Arquivos:
```C
stat()

struct stat

st_size
st_mode
st_mtime
```

### Um pouco de permissões:
```C
0755
```

### Manipulando os Files:
```C
fopen()
fread()
fwrite()
fclose()
```

---

# Estrutura do Código:

```C
openSomeDirectory()
```

**Faz:**
- Abrir um diretório.
- Caso não exista, criá-lo.

Era pra ser uma função genérica par abrir qualquer diretório, se não achar, criar o diretório
Mas acabou ficando para abrir(ou criar) o diretório de origem.

---

```C
void scanSourceDirectory(DIR *myDir)
```

**Faz**
- Percorrer os arquivos da origem.

Seria utilizado para percorrer qualquer diretório, e pela sua estrutura atual, ele ainda faz isso, entretanto, deixamos eles "exclusivamente" para percorrer o diretório de `origem`.

---

```C
DIR * getBackupDir(char *bckPath);
```

**Faz**
- retorna o ponteiro da pasta de backup específica de um determinado arquivo em `origem`

---

```C
void processSourceFile(struct dirent *entrada, char *path);

```

**Faz**
 - Serve para "administrar" as chamadas de outras funções e alguns tratamentos de erros para que o `main` não faça isso

---

```C
time_t findLatestBackupTime(DIR *myBackupPtr, struct dirent *entradaBck, char *bckPath);

```

**Faz**
- Retorna o tempo, em time_t do último arquivo de backup.

---

```C
int createBackup(time_t orinTimeMod, time_t mostRecentFileTime, struct dirent *entrada, char *caminho, char *caminhoBck);

```

**Faz**
- Uma função responsável por criar o arquivo de backup no seu respectivo diretório de backup
- Retorna 1 se deu certo ou 0 se deu errado.

---

# Limitações do Projeto

- Não monitora subdiretórios.
- Não detecta exclusão de arquivos.
- Não utiliza threads.
- Não utiliza **inotify**.
- Não realiza compressão.


> [!Sobre inotify]
> `inotify`...
> lembra um pouco o padrão do observador.
> O `inotify` é uma API do Linux que permite que um programa seja **avisado quando algo muda em arquivos ou diretórios**, sem precisar ficar verificando repetidamente.

Podemos comparar com os `signals` da Godot Engine ou os `Event Listners` da JS

---
