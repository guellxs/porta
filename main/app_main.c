//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       _              //
//               _    _       _      _        _     _   _   _    _   _   _        _   _  _   _          //
//           |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|    //    
//         |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_   //
//                                                                                       /              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

// Área de inclusão das bibliotecas
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

#include "HCF_IOTEC.h"   // Vai se tornar HCF_IOTEC
#include "HCF_LCD.h" // Vai se tornar HCF_LCD
#include "HCF_ADC.h"   // Vai se tornar HCF_ADC
#include "HCF_MP.h"   // Vai se tornar HCF_MP
// Incluir HCF_IOT HCF_BT HCF_DHT HCF_ULTRA HCF_RFID HCF_ZMPT HCF_ACS HCF_SERVO HCF_OLED HCF_CAM HCF_SD HCF_LORA


// Área das macros
//-----------------------------------------------------------------------------------------------------------------------

#define IN(x) (entradas>>x)&1

// Área de declaração de variáveis e protótipos de funções
//-----------------------------------------------------------------------------------------------------------------------

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0; //variáveis de controle de entradas e saídas
static char tecla = '-' ;
char escrever[40];

// Funções e ramos auxiliares
//-----------------------------------------------------------------------------------------------------------------------


// Programa Principal
//-----------------------------------------------------------------------------------------------------------------------

void app_main(void)
{
    /////////////////////////////////////////////////////////////////////////////////////   Programa principal
    escrever[39] = '\0';

    // a seguir, apenas informações de console, aquelas notas verdes no início da execução
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    /////////////////////////////////////////////////////////////////////////////////////   Inicializações de periféricos (manter assim)
    
    // inicializar os IOs e teclado da placa
    iniciar_iotec();      
    entradas = io_le_escreve(saidas); // Limpa as saídas e lê o estado das entradas

    // inicializar o display LCD 
    iniciar_lcd();
    escreve_lcd(1,0,"Fechadura auto.");
    escreve_lcd(2,0,"Desafio 4");
    
    // Inicializar o componente de leitura de entrada analógica
    esp_err_t init_result = iniciar_adc_CHX(0);
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar o componente ADC personalizado");
    }

    //delay inicial
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    limpar_lcd();

    /////////////////////////////////////////////////////////////////////////////////////   Periféricos inicializados

 

    /////////////////////////////////////////////////////////////////////////////////////   Início do ramo principal                    
    char senha_correta[] = "1234"; // Senha definida
    char senha_digitada[5]; // Buffer para a senha digitada (4 dígitos + terminador nulo)
    int posicao = 0; // posição do digito na senha
    int contador = 0; //variavel de log
    int contadorerros = 0; //variavel de erros
    bool senha = false; //define a senha como não digitada no começo

    while (1)
    {
        escreve_lcd(1, 0, "Digite a senha:");
        escreve_lcd(2, 0, "____"); // Indica onde os números vão aparecer
    
        posicao = 0; // Resetar a posição
    
        // Loop para pegar os 4 dígitos da senha
        while (posicao < 4)
        {
            tecla = le_teclado(); // Lê a tecla pressionada
            if (tecla >= '0' && tecla <= '9' && contadorerros < 4)// Só aceita números
            {
                senha_digitada[posicao] = tecla; // Armazena o número
                senha_digitada[posicao + 1] = '\0'; // Adiciona terminador nulo
                escreve_lcd(2, posicao, "*"); // Mostra * no lugar do número
                posicao++;
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }

            if (contadorerros == 3) //se errar 3 vezes seguidas
                {
                while(1)
                escreve_lcd(1, 0, "Sistema Bloqueado");
                escreve_lcd(2, 0, "Reinicie o ESP");
                vTaskDelay(10 / portTICK_PERIOD_MS);
                }

            if (tecla == 'C') //limpar a senha no "C"
            {
            posicao = 0;
            vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            if (senha = false && fimdecurso = true) //se a senha não for digitada e o fim de curso acionado
            { //entender funcionamento das entradas
                limpar_lcd();
                for(int i; i < 5; i++)
                {
                escreve_lcd(1, 0, "Arrombamento");
                entradas = io_le_escreve(0b10000000);
                vTaskDelay(1000 / portTICK_PERIOD_MS); 
                entradas = io_le_escreve(0b00000000); 
                vTaskDelay(1000 / portTICK_PERIOD_MS); 
                }
            }
        }
    
        vTaskDelay(200 / portTICK_PERIOD_MS); // Pequeno delay para evitar leitura dupla
    
        // Verifica se a senha está correta
        if (strcmp(senha_digitada, senha_correta) == 0)
        {
            senha = true;
            limpar_lcd();
            escreve_lcd(1, 0, "Porta aberta!");
            entradas = io_le_escreve(0b00000001);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            entradas = io_le_escreve(0b00000000);
            contador++;
            limpar_lcd();
            sprintf(escrever, "%d", contador);
            escreve_lcd(2,13,escrever);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            limpar_lcd();
        }
        else
        {
            senha = false;
            limpar_lcd();
            escreve_lcd(1, 0, "Senha errada!");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            contadorerros++;

        }
    
                limpar_lcd();
    
        
        //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - -  -  -  -  -  -  -  -  -  -  Escreva seu só até aqui!!! //
        //________________________________________________________________________________________________________________________________________________________//
        vTaskDelay(10 / portTICK_PERIOD_MS);    // delay mínimo obrigatório, se retirar, pode causar reset do ESP
    
    }
    // caso erro no programa, desliga o módulo ADC
    adc_limpar();

    /////////////////////////////////////////////////////////////////////////////////////   Fim do ramo principal
    
}
