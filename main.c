#include <stdio.h>
#include <unistd.h>	  // para o sleep()
#include <dirent.h>	  // operações com diretórios
#include <sys/stat.h> // obter status, stat()
#include <string.h>	  //para manipulação com strings, principalmente para nome de Paths
#include <time.h>
#include <sys/time.h>

// executar com: ./backup_program, dont forget
// Bruno, pode marcar suas dúvidas e implementação e tals
// não pude testar pois seria necessário terminar (estava em uma parte "intestável", de certa forma)
// nessa versão ele ESPERA que os arquivos e dirs existam, sugire você criar os arquivos manualemente
// resumind, isso é uma versão de "caso-perfeito"

// protótipos:

DIR *openSomeDirectory(const char *path);

void scanSourceDirectory(DIR *myDir);

int ShouldSkipThisDir(const char *dirName);

void processSourceFile(struct dirent *entrada, char *path);

time_t getFileModificationTime(const char *path);

DIR *getBackupDir(char *bckPath);

time_t findLatestBackupTime(DIR *myBackupPtr, struct dirent *entradaBck, char *bckPath);

int createBackup(time_t orinTimeMod, time_t mostRecentFileTime, struct dirent *entrada, char *caminho, char *caminhoBck);

int executeSimpleBackup();

//==============================================================

int main()
{
	int opcao;
	int TEMPO = 7;
	do
	{
		printf("\n=== MENU ===\n");
		printf("0 - Sair\n");
		printf("1 - Backup simples\n");
		printf("2 - Backup eterno\n");
		printf("Escolha uma opcao: ");
		scanf("%d", &opcao);

		switch (opcao)
		{
		case 0:
			printf("Encerrando programa...\n");
			break;

		case 1:
			executeSimpleBackup();
			break;

		case 2:
			while (1)
			{
				executeSimpleBackup();

				printf("Aguardando proxima verificacao...\n");
				sleep(TEMPO); // ou outro intervalo
			}
			break;

		default:
			printf("Opcao invalida!\n");
			break;
		}

	} while (opcao != 0);
}

// abrir um diretório com base no path, retorna um ponteiro pra isso.
DIR *openSomeDirectory(const char *path)
{
	printf("executando: abrindo diretório %s...!\n", path);

	// oirgem = "./origem"
	DIR *myDir = opendir(path);

	if (myDir == NULL)
	{
		printf("Opa, o diretório em %s nao pode ser aberto!", path);
		printf(" Tentando criar %s \n", path);

		mkdir(path, 0755);
		myDir = opendir(path);
		// return -1; //pode retornar  valores em uma função void?
		// return NULL;
		printf("%s criado!\n", path);
	}
	return myDir;
}

void scanSourceDirectory(DIR *myDir)
{
	// montando a struct dirent
	// Varrendo o DIR
	struct dirent *entrada;

	while ((entrada = readdir(myDir)) != NULL)
	{
		/* code */
		if (ShouldSkipThisDir(entrada->d_name))
		{
			continue;
		}

		char caminho[512];
		snprintf(caminho, sizeof(caminho), "./origem/%s", entrada->d_name);

		processSourceFile(entrada, caminho);
	}
}

int ShouldSkipThisDir(const char *dirName)
{
	// devemos ignirar este diretório?
	if (strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0)
	{
		return 1; // verdaderio;
	}
	else
	{
		return 0; // falso;
	}
}

void processSourceFile(struct dirent *entrada, char *path)
{
	// coordena o que fazer com os files e tals

	time_t tempoOrigem = getFileModificationTime(path);
	if (tempoOrigem == -1)
	{
		printf("Erro ao obter metadados");
		return; // deixa de trabalhar neste file, passa pro próximo
	}

	char caminhoBck[516]; // path para backup
	snprintf(caminhoBck, sizeof(caminhoBck), "./backups/%s", entrada->d_name);

	DIR *myBackup = getBackupDir(caminhoBck);
	if (myBackup == NULL)
	{
		printf("erro ao abrir dir backups!");
		return;
	}

	struct dirent *entradaBck = NULL;

	time_t latestBackupTime = findLatestBackupTime(myBackup, entradaBck, caminhoBck);

	createBackup(tempoOrigem, latestBackupTime, entrada, path, caminhoBck);

	closedir(myBackup);

	return;
}

time_t getFileModificationTime(const char *path)
{
	// pega o tempo da ultima modificacao do dado e o retorna;

	struct stat fileStatInfos;

	// verificar se foi tudo certo
	if (stat(path, &fileStatInfos))
	{
		return -1; // tempo inválido
	}

	// pegando o tempo
	stat(path, &fileStatInfos);
	time_t orinModTemp = fileStatInfos.st_mtime;

	return orinModTemp;
}

DIR *getBackupDir(char *bckPath)
{
	// acha o diretório do backup, se não achar, cria.
	DIR *myBackup = opendir(bckPath);

	if (myBackup == NULL)
	{
		printf("Não foi possível abrir o: %s \n", bckPath);
        mkdir("./backups", 0755);

		mkdir(bckPath, 0755);
		myBackup = opendir(bckPath);
		// return NULL;
	}

    if (myBackup == NULL ) {
        printf("CRIEI NADA BROTHER \n");
    }
    //printf("LABUBU: %s \n", bckPath);
	return myBackup;
}

time_t findLatestBackupTime(DIR *myBackupPtr, struct dirent *entradaBck, char *bckPath)
{
	// Procura e achar o diretório certo em backups, cria um se não achar

	time_t mostRecentFileTime = 0;

	while ((entradaBck = readdir(myBackupPtr)) != NULL)
	{
		if (ShouldSkipThisDir(entradaBck->d_name))
		{
			continue;
		}

		// pegar o stat()
		struct stat fileStatInfosBck;

		// montando caminho
		char caminhoBckFile[1024]; // caminho do arquivo em backup
		snprintf(caminhoBckFile, sizeof(caminhoBckFile), "%s/%s", bckPath, entradaBck->d_name);
		stat(caminhoBckFile, &fileStatInfosBck);

		// comparacao
		if (fileStatInfosBck.st_mtime > mostRecentFileTime)
		{
			mostRecentFileTime = fileStatInfosBck.st_mtime;
		}
	}
	// creatBackup(st_mtime de origem, mostRecentFileTime);
	return mostRecentFileTime;
}

int createBackup(time_t orinTimeMod, time_t mostRecentFileTime, struct dirent *entrada, char *caminho, char *caminhoBck)
{
	// verifica se precisa de backup
	// cria um arquivo de backup
	if (orinTimeMod <= mostRecentFileTime)
	{
		return 0;
	}

	struct timeval tv;
	struct tm *tm_info;
	char nomeNovoArquivoBuffer[64];

	gettimeofday(&tv, NULL);
	tm_info = localtime(&tv.tv_sec);

	strftime(nomeNovoArquivoBuffer, sizeof(nomeNovoArquivoBuffer), "%H-%M-%S", tm_info);

	printf("Tempo coiso: '%s' \n", nomeNovoArquivoBuffer);

	FILE *originalFile = fopen(caminho, "r"); // abrindo o file origem, usando a variável de caminho

	char backupFilePath[716]; // definir tamanho apropriado depois;

	char *ext = strrchr(entrada->d_name, '.'); // entrada

	if (ext != NULL)
	{
		snprintf(backupFilePath, sizeof(backupFilePath), "%s/%s%s", caminhoBck, nomeNovoArquivoBuffer, ext); // nomBuffer torna-se um nome e cuidadr do tamanho
	}
	else
	{
		snprintf(backupFilePath, sizeof(backupFilePath), "%s/%s", caminhoBck, nomeNovoArquivoBuffer);
	}

	FILE *backupFile = fopen(backupFilePath, "w");
	// adicionar verificações depois

	char copyBuffer[4096]; // isso está em bytes
	size_t bytesLidos;

	while ((bytesLidos = fread(copyBuffer, 1, sizeof(copyBuffer), originalFile)) > 0)
	{
		fwrite(copyBuffer, 1, bytesLidos, backupFile);
	}

	printf("Fim da cópia. Mas não verififou se realmente rolou. Mas terminou! \n");

	// lembrar de dar close nos diretórios abertos e arquivos abertos
	fclose(originalFile);
	fclose(backupFile);

	return 1;
}

int executeSimpleBackup()
{
	DIR *dirOrimgem = openSomeDirectory("./origem");
	if (dirOrimgem == NULL)
	{
		return -1;
	}

	scanSourceDirectory(dirOrimgem);

	closedir(dirOrimgem);

	// fechar as dirs!!!!
	return 0;
}
