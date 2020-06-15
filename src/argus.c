#include "argus.h"


int get_output_from_index(int index, char **line){
    int output_fd;
    if ((output_fd = open(LOG_INDEX_FILE, O_RDWR | O_CREAT, 0666)) < 0)
    	return -1;
    IndexRecord p1;
    IndexRecord p;
    int continue_ = 1;
    int bytes_t = 1;
    while ((bytes_t > 0) && continue_){
        if(continue_)
            bytes_t = read(output_fd,&p1,sizeof(IndexRecord));
        if (bytes_t > 0){
            if (p1.index == index) 
                continue_ = 0;   
        }
    }
    if (continue_ == 1) return -3;
    int start = p1.start;
    int end = p1.end;
    close(output_fd);
    if (start == end) {
        return -2;
    }
    if ((output_fd = open(LOG_FILE, O_RDONLY)) < 0) {
        return -1;
    }
    *line = malloc((end - start) * sizeof(char));
    lseek(output_fd, start, SEEK_SET);
    int res = read(output_fd, *line, (end - start));
    close(output_fd);
    return res;
}



/*recebe um index, e devolve a linha q corresponde ao output desse index.
Devolve -1 caso o ficheiro nao existir, -2 caso seja um comando q nao tem output
e -3 caso o indice dado nao corresponde a um comando valido.*/
int get_offset_for_output(int index, char** line) {
    int fd;
    if ((fd = open(LOG_INDEX_FILE, O_RDONLY)) < 0) return -1;
    int index_line_size;
    char* linha = read_line(fd, &index_line_size);  // ler a linha
    char* token = strtok(linha, ",");
    char* penultimate_token = token;  // sera necessario o valor onde começa
                                      // (pen_token) e onde acaba (token)
    if (token == NULL) return -3;  // nao ha nada escrito no ficheiro
    token = strtok(NULL, ",");
    while (token != NULL && index > 1) {
        penultimate_token = token;
        token = strtok(NULL, ",");
        index--;
    }
    if (token == NULL) {
        return -3;
        free(linha);
    }  // pediu um indice maior do que o tamanho do array
    int start = strtol(penultimate_token, NULL, 10);
    int end = strtol(token, NULL, 10);
    close(fd);
    if (start == end) {
        return -2;
        free(linha);
    }
    if ((fd = open(LOG_FILE, O_RDONLY)) < 0) {
        return -1;
        free(linha);
    }
    *line = malloc((end - start) * sizeof(char));
    lseek(fd, start, SEEK_SET);
    int res = read(fd, *line, (end - start));
    close(fd);
    free(linha);
    return res;
}

/**
 *
 * Checks if a given string contains a number and only a number. The number must
 * be >= 0;
 */
bool is_limit_valid(char* s) {
    /*int new_limit = atoi(s);
    printf("%d\n",new_limit);
    return (new_limit >0);*/
    char* temp;
    long value = strtol(s, &temp, 10);  // using base 10
    if (temp != s && *temp == '\0' && value >= 0) {
        return true;
    } else {
        return false;
    }
}

bool is_task_number_valid(char* s) {
    /*int new_limit = atoi(s);
    printf("%d\n",new_limit);
    return (new_limit >0);*/
    char* temp;
    long value = strtol(s, &temp, 10);  // using base 10
    if (temp != s && *temp == '\0' && value >= 1) {
        return true;
    } else {
        return false;
    }
}

// concatena os vários componenetes de um comando
char* concat_command(char** command, int numero_componentes) {
    if (numero_componentes == 2) {
        int size_string_concated = strlen(command[0]) + 2 + strlen(command[1]);
        char* jointCommand = malloc(sizeof(char) * size_string_concated);
        snprintf(jointCommand, size_string_concated, "%s %s", command[0],
                 command[1]);
        return jointCommand;
    } else {
        return strdup(command[0]);
    }
}

// verifica se um comando enviado através da linha de comando é válido
bool valid_command(char** command, int number_of_components) {
    if (number_of_components >= 1) {
        if (strcmp(command[0], "-m") == 0 ||
            strcmp(command[0], "-i") ==
                0) {  // adicionar aqui tempo-inactividade && tempo-execucao
            if (number_of_components == 2) {
                if (!is_limit_valid(command[1])) {
                    write(1, "Limite inválido\n", strlen("Limite inválido\n"));
                    return false;
                }
                return true;
            } else if (number_of_components == 1) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            } else {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
        } else if (strcmp(command[0], "-e") == 0) {
            if (number_of_components == 1) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-l") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-r") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-t") == 0) {
            if (number_of_components == 2) {
                if (!is_task_number_valid(command[1])) {
                    write(1, "Número inválido\n", strlen("Número inválido\n"));
                    return false;
                }
                return true;
            } else if (number_of_components < 2) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            } else if (number_of_components > 2) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
        } else if (strcmp(command[0], "-h") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            } else {
                return true;
            }
        } else if (strcmp(command[0], "-o") == 0) {
            if (number_of_components == 2) {
                if (!is_task_number_valid(command[1])) {
                    write(1, "Número inválido\n", strlen("Número inválido\n"));
                    return false;
                }
                return true;
            } else if (number_of_components < 2) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            } else if (number_of_components > 2) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
        } else {
            write(1, "Comando inexistente\n", strlen("Comando inexistente\n"));
            return false;
        }
    } else {
        write(1, "Falta de argumentos\n", strlen("Falta de argumentos\n"));
        return false;
    }
    return false;
}

// verifica se um comando enviado através da shell interpretada segue as devidas
// restrições
bool valid_comand_prompt(char** command, int number_of_components) {
    if (number_of_components >= 1) {
        if (strcmp(command[0], "-m") == 0 || strcmp(command[0], "-i") == 0) {
            if (number_of_components == 2) {
                if (!is_limit_valid(command[1])) {
                    write(1, "Limite inválido\n", strlen("Limite inválido\n"));
                    return false;
                }
                return true;
            } else if (number_of_components == 1) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            } else {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
        } else if (strcmp(command[0], "-e") == 0) {
            if (number_of_components == 1) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-l") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-r") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
            return true;
        } else if (strcmp(command[0], "-t") == 0) {
            if (number_of_components == 2) {
                if (!is_task_number_valid(command[1])) {
                    write(1, "Número inválido\n", strlen("Número inválido\n"));
                    return false;
                }
                return true;
            } else {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            }
        } else if (strcmp(command[0], "-o") == 0) {
            if (number_of_components == 2) {
                if (!is_task_number_valid(command[1])) {
                    write(1, "Número inválido\n", strlen("Número inválido\n"));
                    return false;
                }
                return true;
            } else if (number_of_components < 2) {
                write(1, "Falta de argumentos\n",
                      strlen("Falta de argumentos\n"));
                return false;
            } else if (number_of_components > 2) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            }
        } else if (strcmp(command[0], "-h") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            } else {
                return true;
            }
        } else if (strcmp(command[0], "sair") == 0) {
            if (number_of_components > 1) {
                write(1, "Excesso de argumentos\n",
                      strlen("Excesso de argumentos\n"));
                return false;
            } else {
                return true;
            }
        } else {
            write(1, "Comando inexistente\n", strlen("Comando inexistente\n"));
            return false;
        }
    } else {
        write(1, "Não foi introduzido um command\n",
              strlen("Não foi introduzido um command\n"));
        return false;
    }
}

void show_help() {
    write(1, "Adjuda:\n", strlen("Adjuda:\n"));
    char* string =
        "\t-i <secs>: Define o tempo de inactividade de comunicação num pipe "
        "anónimo\n";
    write(1, string, strlen(string));

    string = "\t-m <secs>: define o tempo máximo de execução de uma tarefa\n";
    write(1, string, strlen(string));

    string = "\t-e <tarefa>: Executa uma tarefa\n";
    write(1, string, strlen(string));

    string = "\t-l: Lista as tarefas em execução\n";
    write(1, string, strlen(string));

    string = "\t-t <número tarefa>: Termina a execução de uma tarefa\n";
    write(1, string, strlen(string));

    string =
        "\t-r: Lista as tarefas terminadas assim como o estado em que "
        "terminou\n";
    write(1, string, strlen(string));

    string = "\t-h: Apresenta o manual de utilização\n";
    write(1, string, strlen(string));
}

void show_help_prompt() {
    write(1, "Adjuda:\n", strlen("Adjuda:\n"));
    char* string =
        "\ttempo-inactividade <secs>: Define o tempo de inactividade de "
        "comunicação num pipe anónimo\n";
    write(1, string, strlen(string));

    string =
        "\ttempo-execucao <secs>: Define o tempo máximo de execução de uma "
        "tarefa\n";
    write(1, string, strlen(string));

    string = "\texecutar <tarefa>: Executa uma tarefa\n";
    write(1, string, strlen(string));

    string = "\tlistar: Lista as tarefas em execução\n";
    write(1, string, strlen(string));

    string = "\tterminar <número tarefa>: Termina a execução de uma tarefa\n";
    write(1, string, strlen(string));

    string =
        "\thistorico: Lista as tarefas terminadas assim como o estado em que "
        "terminou\n";
    write(1, string, strlen(string));

    string = "\tsair: Fecha o cliente\n";
    write(1, string, strlen(string));

    string = "\tajuda: Apresenta o manual de utilização\n";
    write(1, string, strlen(string));

    string =
        "\tNOTA: Os comandos quando se usa a aplicação através da shell também "
        "se aplicam à utilização com prompt. Tanto -e ls | wc como argus "
        "executa ls | wc são válidos\n";
    write(1, string, strlen(string));
}

// separates the line which was read by it's spaces
char** separate_line(char* line_to_separate, int* number_of_sublines) {
    char* rest;
    char* token = strtok_r(line_to_separate, " ", &rest);
    char** res = malloc(sizeof(char*) * 1);
    *number_of_sublines = 0;
    if (token) {
        // special case of execute
        if ((strcmp(token, "executar") == 0) || strcmp(token, "-e") == 0) {
            res[0] = strdup("-e");
            if (rest && strlen(rest) > 0) {
                res = realloc(res, sizeof(char*) * 2);
                res[1] = strdup(rest);
                *number_of_sublines = 2;
            } else {
                *number_of_sublines = 1;
            }
            return res;
        } else if (strcmp(token, "tempo-inactividade") == 0) {
            res[0] = strdup("-i");
        } else if (strcmp(token, "tempo-execucao") == 0) {
            res[0] = strdup("-m");
        } else if (strcmp(token, "listar") == 0) {
            res[0] = strdup("-l");
        } else if (strcmp(token, "terminar") == 0) {
            res[0] = strdup("-t");
        } else if (strcmp(token, "historico") == 0) {
            res[0] = strdup("-r");
        } else if (strcmp(token, "ajuda") == 0) {
            res[0] = strdup("-h");
        } else if (strcmp(token, "output") == 0) {
            res[0] = strdup("-o");
        } else {
            res[0] = strdup(token);
        }
        *number_of_sublines = 1;
        while (token = strtok_r(NULL, " ", &rest)) {
            res = realloc(res, sizeof(char*) * ((*number_of_sublines) + 1));
            res[(*number_of_sublines)] = strdup(token);
            (*number_of_sublines)++;
        }
    }
    return res;
}

// abre um fifo que irá ler a resposta dada pelo servidor
int read_answer() {
    int fifo_server_to_client_fd;
    if ((fifo_server_to_client_fd = open(FIFO_SERVER_TO_CLIENT, O_RDONLY)) <
        0) {
        perror("open");
        return 1;
    }
    int n_bytes = 0;
    char buf[1024];
    while ((n_bytes = read(fifo_server_to_client_fd, buf, 1024)) > 0) {
        write(1, buf, n_bytes);
    }
    close(fifo_server_to_client_fd);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 3) {
        write(1, "Excesso de argumentos\n", strlen("Excesso de argumentos\n"));
        return 1;
    }

    if (argc > 1) {
        char** command = argv + 1;
        int numero_componentes = argc - 1;
        if (valid_command(command, numero_componentes)) {
            if (strcmp(command[0], "-h") != 0 &&
                strcmp(command[0], "-o") != 0) {
                char* concated_command =
                    concat_command(command, numero_componentes);

                int fd;
                if ((fd = open(FIFO_CLIENT_TO_SERVER, O_WRONLY)) < 0) {
                    perror("open");
                    return 1;
                }

                if (write(fd, concated_command, strlen(concated_command)) < 0) {
                    perror("Write");
                    return 1;
                }
                close(fd);
                read_answer();

                free(concated_command);
            } else if (strcmp(command[0], "-h") == 0) {
                show_help();
            } else {
                char* outputSearch = NULL;
                int index = strtol(command[1], NULL, 10);
                int output_size;
                if ((output_size =
                         get_output_from_index(index, &outputSearch)) > 0) {
                    write(1, outputSearch, output_size);
                    free(outputSearch);
                } else {
                    char* error_msg;
                    int error_n;
                    if (output_size == -1)
                        error_msg =
                            "Erro na leitura dos ficheiros de gravação.\n";
                    if (output_size == -3)
                        error_msg =
                            "O indice dado não está associado ao o output dum "
                            "command.\n";
                    if (output_size == -2)
                        error_msg =
                            "O índice dado corresponde a um command que não "
                            "têm output.\n";
                    write(1, error_msg, strlen(error_msg));
                }
            }
        }
    } else {
        while (1) {
            write(1, "Argus$: ", strlen("Argus$: "));
            int bytes_read;
            char* line = read_line(1, &bytes_read);

            int number_of_sublines = 0;
            char** separated_line = separate_line(line, &number_of_sublines);

            if (valid_comand_prompt(separated_line, number_of_sublines)) {
                if (strcmp(separated_line[0], "sair") == 0) {
                    free(separated_line[0]);
                    free(separated_line);
                    free(line);
                    break;
                } else if (strcmp(separated_line[0], "-h") == 0) {
                    show_help_prompt();
                } else if (strcmp(separated_line[0], "-o") == 0) {
                    char* outputSearch = NULL;
                    int index = strtol(separated_line[1], NULL, 10);
                    int output_size;
                    if ((output_size =
                           get_output_from_index(index, &outputSearch)) > 0) {
                        write(1, outputSearch, output_size);
                        free(outputSearch);
                    } else {
                        char* error_msg;
                        if (output_size == -1)
                            write(
                                1,
                                "Erro na leitura dos ficheiros de gravação.\n",
                                strlen("Erro na leitura dos ficheiros de "
                                       "gravação.\n"));
                        if (output_size == -3)
                            write(1,
                                  "O indice dado não está associado ao o "
                                  "output dum command.\n",
                                  strlen("O indice dado não está associado ao "
                                         "o output dum command.\n"));
                        if (output_size == -2)
                            write(1,
                                  "O índice dado corresponde a um command que "
                                  "não têm output.\n",
                                  strlen("O índice dado corresponde a um "
                                         "command que não têm output.\n"));
                    }
                } else {
                    char* concated_command =
                        concat_command(separated_line, number_of_sublines);
                    int fd;
                    if ((fd = open(FIFO_CLIENT_TO_SERVER, O_WRONLY)) < 0) {
                        perror("open");
                        return 1;
                    }

                    if (write(fd, concated_command, strlen(concated_command)) <
                        0) {
                        perror("Write");
                        return 1;
                    }
                    close(fd);
                    read_answer();
                    free(concated_command);
                }
            }
            for (int i = 0; i < number_of_sublines; i++) {
                free(separated_line[i]);
            }
            free(separated_line);
            free(line);
        }
    }
    return 0;
}
