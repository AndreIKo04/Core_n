#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#define true 1
#define false 0

#define set_utf8() system("chcp 65001 > nul")

typedef unsigned char byte;
typedef unsigned int uint;
typedef struct Computer comp_t;


char *multichar(const char symbol, uint count)
{
    char *res_str = malloc((count * sizeof(char)) + 1);

    for (uint c = 0; c < count; c++)
        res_str[c] = symbol;
    return res_str;
}

char *strcenter(char *string, int stretch)
{

    int dest_len = strlen(string);
    int len_of_side = round((float)((stretch - dest_len) / 2));

    if (stretch <= 0 || stretch <= (dest_len + 2))
        return string;

    char *new_str = malloc((stretch + dest_len + 1)  * sizeof(char));
    char *side = multichar(' ', (uint)len_of_side);

    strcpy(new_str, side);
    strcpy(new_str + len_of_side, string);


    return new_str;
}

int op_casecmp(const char *_operator, const char *options[])
{
    const int num_options = 4;
    for (int c = 0; c < num_options; c++)
        if (!strcmp(_operator, options[c]))
            return true;
    return false;
}

void print_binary(byte value)
{
    for (int i = 0; i < 8; i++)
    {
        printf("%c", (value & 0x80) ? '1' : '0');
        value <<= 1;
    }
}

void error(char *hint)
{
    puts("\tОшибка!");
    printf("- %s\n", hint);
    getch();
}


///-------- Computer ---------
typedef struct Computer
{
    byte regA;
    byte regB;
    byte regR;
    byte *mem;
    char *comm_line;

} comp_t;

comp_t *_init()
{
    comp_t *Computer_new = malloc(sizeof(comp_t));
    Computer_new->mem = malloc(16 * sizeof(byte));
    Computer_new->comm_line = malloc(8 * sizeof(char));
    Computer_new->regA = 0;
    Computer_new->regB = 0;
    Computer_new->regR = 0;
    return Computer_new;

}

void _destroy(comp_t *computer)
{
    free(computer->mem);
    free(computer->comm_line);
    free(computer);
}

void Computer_ALU(comp_t *computer, char operation)
{
    byte result;
    switch (operation)
    {
    case '+':
        result = computer->regA + computer->regB;
        break;
    case '-':
        result = computer->regA - computer->regB;
        break;
    }
    computer->regR = result;
}

void Computer_load_value(comp_t *computer, int cell_adress)
{
    if (cell_adress >= 0 && cell_adress < 16)
    {
        computer->regA = computer->mem[cell_adress];
        Computer_ALU(computer, '+');
    }
}

void Computer_store_value(comp_t *computer, int to_adress)
{
    if (to_adress >= 0 && to_adress < 16)
        computer->mem[to_adress] = computer->regA;
    else error("сохранение значения в несуществующую ячейку памяти");
}

void Computer_immed_value(comp_t *computer, int value)
{
    if (value > 0 && value < 16)
        computer->regA = value;
    Computer_ALU(computer, '+');
}

void Computer_operation_to_regR(comp_t *computer, int adress, char operation)
{
    if (adress >= 0 && adress < 16)
    {
        computer->regB = computer->mem[adress];
        Computer_ALU(computer, operation);
        computer->regA = computer->regR;
    }
    else error("обращение за приделы памяти");
}

void display(comp_t *computer)
{
    printf("%s\n\n Рег.А: ", strcenter("КОРЕНЬЯ 1.0\n", 48));
    print_binary(computer->regA);
    printf("%s", strcenter("Рег.Б: ", 20));
    print_binary(computer->regB);
    printf("\n%s\n%s", strcenter("=", 35), strcenter("Результат: ", 36));
    print_binary(computer->regR);
    printf("\n\nДля справки введите СК (Список Команд)\n"\
           "Для просмотра содержания памяти - ХР (ХРанилище)");
}

void show_storage(comp_t *computer)
{
    for (int i = 0; i < 16; i++)
    {
        printf("%d:\t", i);
        print_binary(computer->mem[i]);
        printf("\n");
    }
    getch();
}

void comm_list()
{
    char *commands[] = {
        "ПЗ", "ДЗ", "ДД", "ПК", "ОО",
        "ЗР", "ХР", "СК"
    };

    char *descriptions[] = {
        "положить значение по адрессу (ПЗ адрес)",
        "достать значение по адрессу (ДЗ адрес)",
        "достать данное значение, стоящее за оператором (ДД значение)",
        "достать значение по адрессу в регистр Б и провести сложение, сумму из Р регистра поместить в регистр А (ПК адрес)",
        "тоже самое, что и ПК, только с отниманием",
        "завершить работу компьютера",
        "показать хранилище в виде списка",
        "список команд*",
    };

    int num_commands = sizeof(commands) / sizeof(commands[0]);

    for (int i = 0; i < num_commands; i++)
        printf("%s - %s\n", commands[i], descriptions[i]);
    getch();
     
}
///---------------------------


void parse_line(comp_t *computer)
{
    char _operator[5],
         *comment_ptr;
    int _adress;

    comment_ptr = strchr(computer->comm_line, ';');
    if (comment_ptr != NULL)
        *comment_ptr = '\0';

    if (sscanf(computer->comm_line, "%s %d", &_operator, &_adress) == 2)
    {
        if (op_casecmp(_operator, (const char *[]){"ДЗ", "Дз", "дЗ", "дз"}))
            Computer_load_value(computer, _adress);
        else if (op_casecmp(_operator, (const char *[]){"ПЗ", "Пз", "пЗ", "пз"}))
            Computer_store_value(computer, _adress);
        else if (op_casecmp(_operator, (const char *[]){"ДД", "Дд", "дД", "дд"}))
            Computer_immed_value(computer, _adress);
        else if (op_casecmp(_operator, (const char *[]){"ПК", "Пк", "пК", "пк"}))
            Computer_operation_to_regR(computer, _adress, '+');
        else if (op_casecmp(_operator, (const char *[]){"ОО", "Оо", "оО", "оо"}))
            Computer_operation_to_regR(computer, _adress, '-');
        else error("неверный синтаксис");
    }

    else if (op_casecmp(_operator, (const char *[]){"ЗР", "Зр", "зР", "зр"}))
    {
        _destroy(computer);
        exit(0);
    }
    else if (op_casecmp(_operator, (const char *[]){"ХР", "Хр", "хР", "хр"}))
        show_storage(computer);
    else if (op_casecmp(_operator, (const char *[]){"СК", "Ск", "сК", "ск"}))
        comm_list();
    else error("неверный синтаксис");

}


int main()
{
    comp_t *mycomp = _init();
    //set_utf8(); –> is not necessary on Linux

    while(1)
    {
        display(mycomp);
        printf("\n> ");
        fgets(mycomp->comm_line, sizeof(mycomp->comm_line), stdin);

        parse_line(mycomp);
        system("cls");
    }

    return 0;
}
