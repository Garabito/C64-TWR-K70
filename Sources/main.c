#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "MK70F12.h" /* Archivo de cabecera específico del K70F120M */

#include "lcdc.h"

#define MAX_LINEAS 100
#define MAX_LONG_LINEA 64
#define NUM_VARIABLES 26
#define LCD_ANCHO  480
#define LCD_ALTO   272
// Matriz de píxeles: Cada píxel es un uint16_t (RGB565)
uint16_t FrameBuffer[LCD_ALTO][LCD_ANCHO];

int cursor_x = 0;
int cursor_y = 0;
#define MAX_COLUMNAS (LCD_ANCHO / 8) // 60 columnas
#define MAX_FILAS    (LCD_ALTO / 8)  // 34 filas
const uint8_t Fuente8x8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // Espacio (32)
    {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00}, // !
    // ... (Debes incluir o enlazar un array completo de caracteres ASCII estándar)
    {0x3C,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // A (65)
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, // B (66)
};

void LCD_PintarPixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < LCD_ANCHO && y >= 0 && y < LCD_ALTO) {
        FrameBuffer[y][x] = color;
    }
}

void LCD_DibujarCaracter(char c, int col, int fila) {
    if (c < 32 || c > 127) return; // Fuera de rango ASCII imprimible
    int idx = c - 32;

    for (int y = 0; y < 8; y++) {
        uint8_t linea_fuente = Fuente8x8[idx][y];
        for (int x = 0; x < 8; x++) {
            // Verificar si el bit está activo de izquierda a derecha
            if (linea_fuente & (0x80 >> x)) {
                LCD_PintarPixel((col * 8) + x, (fila * 8) + y, 0xA4FF); // Texto Azul Claro C64
            } else {
                LCD_PintarPixel((col * 8) + x, (fila * 8) + y, 0x421B); // Fondo Azul Oscuro C64
            }
        }
    }
}

void LCD_LimpiarPantalla(uint16_t color) {
    for (int y = 0; y < LCD_ALTO; y++) {
        for (int x = 0; x < LCD_ANCHO; x++) {
            FrameBuffer[y][x] = color;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

#define MAX_COLUMNAS (LCD_ANCHO / 8) // 60 columnas
#define MAX_FILAS    (LCD_ALTO / 8)  // 34 filas


// Estructura de almacenamiento de un programa BASIC
typedef struct {
    int numero;
    char codigo[MAX_LONG_LINEA];
} LineaBasic;

LineaBasic programa[MAX_LINEAS];
int total_lineas = 0;
int variables[NUM_VARIABLES]; // Mapeo de variables de la 'A' a la 'Z'

// --- DRIVERS DE BAJO NIVEL PARA TWR-K70 (UART2 Ejemplo) ---
void UART_Init(void) {
    // 1. Habilitar reloj para UART2 y Puerto E (Pines Tx/Rx usuales en TWR)
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // 2. Configurar pines multiplexados a función UART
    PORTE->PCR[16] = PORT_PCR_MUX(3); // UART2_TX
    PORTE->PCR[17] = PORT_PCR_MUX(3); // UART2_RX

    // 3. Deshabilitar Tx/Rx antes de configurar baudios
    UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    // 4. Calcular Baud Rate (Para Clock de Bus a 60MHz y Baudios a 115200)
    // SBR = BusClock / (Baud * 16) -> 60000000 / (115200 * 16) =~ 32
    uint16_t sbr = 32;
    UART2->BDH = (UART2->BDH & ~UART_BDH_SBR_MASK) | ((sbr >> 8) & 0x1F);
    UART2->BDL = sbr & 0xFF;

    // 5. Habilitar Transmisor y Receptor
    UART2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

void UART_PutChar(char c) {
    // Esperar a que el buffer de transmisión esté vacío
    while(!(UART2->S1 & UART_S1_TDRE_MASK));
    UART2->D = c;
}

char UART_GetChar(void) {
    // Esperar a que llegue un carácter
    while(!(UART2->S1 & UART_S1_RDRF_MASK));
    return UART2->D;
}

// Redirección de Newlib (KDS GCC) para Printf de C estándar
int _write(int file, char *ptr, int len) {
	int i;
	    for(i = 0; i < len; i++) {
	        // ---- 1. Salida física a la UART (Terminal PC) ----
	        if(ptr[i] == '\n') {
	            while(!(UART2->S1 & UART_S1_TDRE_MASK));
	            UART2->D = '\r';
	        }
	        while(!(UART2->S1 & UART_S1_TDRE_MASK));
	        UART2->D = ptr[i];

	        // ---- 2. Salida física a la pantalla TWR-LCD-RGB ----
	        if (ptr[i] == '\n' || ptr[i] == '\r') {
	            cursor_x = 0;
	            cursor_y++;
	            if (cursor_y >= MAX_FILAS) {
	                LCD_LimpiarPantalla(0x421B); // Scroll básico: limpia pantalla al llenarse
	            }
	        }
	        else {
	            LCD_DibujarCaracter(ptr[i], cursor_x, cursor_y);
	            cursor_x++;
	            if (cursor_x >= MAX_COLUMNAS) {
	                cursor_x = 0;
	                cursor_y++;
	                if (cursor_y >= MAX_FILAS) {
	                    LCD_LimpiarPantalla(0x421B);
	                }
	            }
	        }
	    }
	    return len;
}

// --- INTERPRETE BASIC SIMPLIFICADO ---
void LimpiarPrograma() {
    total_lineas = 0;
    for(int i = 0; i < NUM_VARIABLES; i++) variables[i] = 0;
}

void RegistrarLinea(int num, const char* txt) {
    int i;
    // Buscar si ya existe la línea para reemplazarla o eliminarla
    for(i = 0; i < total_lineas; i++) {
        if(programa[i].numero == num) {
            if(strlen(txt) == 0) { // Eliminar línea si se digita vacía
                for(int j = i; j < total_lineas - 1; j++) programa[j] = programa[j+1];
                total_lineas--;
            } else {
                strcpy(programa[i].codigo, txt);
            }
            return;
        }
    }
    // Insertar nueva línea ordenada por número
    if(total_lineas < MAX_LINEAS && strlen(txt) > 0) {
        programa[total_lineas].numero = num;
        strcpy(programa[total_lineas].codigo, txt);
        total_lineas++;
        // Ordenamiento burbuja simple por número de línea
        for(int x = 0; x < total_lineas-1; x++) {
            for(int y = 0; y < total_lineas-x-1; y++) {
                if(programa[y].numero > programa[y+1].numero) {
                    LineaBasic temp = programa[y];
                    programa[y] = programa[y+1];
                    programa[y+1] = temp;
                }
            }
        }
    }
}

void EjecutarComandoInmediato(char* linea, int* PC) {
    while(*linea == ' ') linea++; // Saltar espacios
    if(strlen(linea) == 0) return;

    // Convertir comando a mayúsculas
    char cmd[16];
    int i = 0;
    while(linea[i] != ' ' && linea[i] != '\0' && i < 15) {
        cmd[i] = toupper((unsigned char)linea[i]);
        i++;
    }
    cmd[i] = '\0';
    char* argumento = linea + i;
    while(*argumento == ' ') argumento++;

    if(strcmp(cmd, "PRINT") == 0 || strcmp(cmd, "?") == 0) {
        if(argumento[0] == '"') {
            char* fin = strchr(argumento + 1, '"');
            if(fin) {
                *fin = '\0';
                printf("%s\n", argumento + 1);
            }
        } else {
            // Imprimir una variable
            char var = toupper((unsigned char)argumento[0]);
            if(var >= 'A' && var <= 'Z') {
                printf(" %d\n", variables[var - 'A']);
            } else {
                printf(" %d\n", atoi(argumento));
            }
        }
    }
    else if(strcmp(cmd, "LET") == 0 || (strlen(cmd) == 1 && cmd[0] >= 'A' && cmd[0] <= 'Z')) {
        char var = (strcmp(cmd, "LET") == 0) ? toupper((unsigned char)*argumento++) : cmd[0];
        while(*argumento == ' ' || *argumento == '=') argumento++;
        variables[var - 'A'] = atoi(argumento); // Asigna valor entero simple
    }
    else if(strcmp(cmd, "GOTO") == 0) {
        int destino = atoi(argumento);
        for(int j = 0; j < total_lineas; j++) {
            if(programa[j].numero == destino) {
                *PC = j - 1; // Ajusta el puntero de programa
                return;
            }
        }
        printf("?UNDEF'D STATEMENT ERROR\n");
    }
}

void ListarPrograma() {
    for(int i = 0; i < total_lineas; i++) {
        printf("%d %s\n", programa[i].numero, programa[i].codigo);
    }
}

void EjecutarPrograma() {
    int PC = 0;
    while(PC < total_lineas) {
        char buffer_temp[MAX_LONG_LINEA];
        strcpy(buffer_temp, programa[PC].codigo);
        int viejo_pc = PC;
        EjecutarComandoInmediato(buffer_temp, &PC);
        if(PC == viejo_pc) PC++; // Avanzar secuencial si no hubo un GOTO
    }
}

int _read(int file, char *ptr, int len) {
    int n;
    for (n = 0; n < len; n++) {
        // Esperar hasta que el bit RDRF (Receive Data Register Full) cambie a 1
        while(!(UART2->S1 & UART_S1_RDRF_MASK));
        // Leer el byte del registro de datos
        ptr[n] = UART2->D;

        // Opcional: Hacer eco automático si se requiere
        while(!(UART2->S1 & UART_S1_TDRE_MASK));
        UART2->D = ptr[n];

        // Si el usuario presiona Enter, dejar de leer
        if (ptr[n] == '\r' || ptr[n] == '\n') {
            return n + 1;
        }
    }
    return n;
}


int main(void) {
    // Inicializaciones críticas del Microcontrolador K70
    SystemInit();
    UART_Init();
    LCDC_Init(); // Inicializa los registros del hardware gráfico del K70

    // Colorear el fondo inicial al clásico Azul Commodore antes de escribir
    LCD_LimpiarPantalla(0x421B);
    // Pantalla de Bienvenida Estilo Clásico Commodore 64
    printf("\n\r    **** COMMODORE 64 BASIC V2 ****\n\r");
    printf(" 64K RAM SYSTEM  38911 BASIC BYTES FREE\n\r");
    printf("READY.\n\r");

    char buffer_entrada[MAX_LONG_LINEA];
    int idx = 0;

    while(1) {
        char c = UART_GetChar();

        // Eco del caracter recibido por la terminal
        if(c != '\r' && c != '\n') {
            if(idx < MAX_LONG_LINEA - 1) {
                UART_PutChar(c);
                buffer_entrada[idx++] = c;
            }
        }
        else { // Presionar ENTER ejecuciones directas o almacenar líneas
            UART_PutChar('\n');
            buffer_entrada[idx] = '\0';

            if(idx > 0) {
                // Verificar si empieza con número (Es línea de programa)
                if(isdigit((unsigned char)buffer_entrada[0])) {
                    int num_linea = atoi(buffer_entrada);
                    char* ptr = buffer_entrada;
                    while(isdigit((unsigned char)*ptr)) ptr++;
                    while(*ptr == ' ') ptr++;
                    RegistrarLinea(num_linea, ptr);
                }
                else {
                    // Comandos directos globales de control de entorno
                    char cmd_upper[16];
                    sscanf(buffer_entrada, "%s", cmd_upper);
                    for(int i=0; cmd_upper[i]; i++) cmd_upper[i] = toupper((unsigned char)cmd_upper[i]);

                    if(strcmp(cmd_upper, "LIST") == 0) {
                        ListarPrograma();
                    } else if(strcmp(cmd_upper, "RUN") == 0) {
                        EjecutarPrograma();
                    } else if(strcmp(cmd_upper, "NEW") == 0) {
                        LimpiarPrograma();
                    } else {
                        int dummyPC = 0;
                        EjecutarComandoInmediato(buffer_entrada, &dummyPC);
                    }
                    printf("READY.\n\r");
                }
            }
            idx = 0; // Reiniciar índice de buffer
        }
    }
    return 0;
}
