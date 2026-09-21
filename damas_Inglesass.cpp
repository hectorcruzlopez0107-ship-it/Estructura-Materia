#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

const string RESET          = "\033[0m";
const string CASILLA_NEGRA  = "\033[1;97;40m";
const string CASILLA_BLANCA = "\033[30;47m";

const int JUGADOR_X = 1;
const int JUGADOR_O = 2;

char tablero[8][8];

int marcas[8][8];

vector<string> historial;

bool rendido = false;

struct Movimiento {
    int filaDestino;
    int colDestino;
    bool esCaptura;
};

int duenio(char pieza) {
    if (pieza == 'X' || pieza == 'x') return JUGADOR_X;
    if (pieza == 'O' || pieza == 'o') return JUGADOR_O;
    return 0;
}

string nombreJugador(int jugador) {
    if (jugador == JUGADOR_X) return "X";
    return "O";
}

int contrario(int jugador) {
    if (jugador == JUGADOR_X) return JUGADOR_O;
    return JUGADOR_X;
}

string nombreCasilla(int fila, int col) {
    string nombre = "(";
    nombre += (char)('0' + fila);
    nombre += ",";
    nombre += (char)('0' + col);
    nombre += ")";
    return nombre;
}

bool dentroDelTablero(int fila, int col) {
    return fila >= 0 && fila < 8 && col >= 0 && col < 8;
}

string leerLinea() {
    string linea;
    getline(cin, linea);
    if (!cin) {
        cout << "\nEntrada cerrada. Fin del programa.\n";
        exit(0);
    }
    return linea;
}

bool leerCoordenada(string linea, int &fila, int &col) {

    for (int i = 0; i < (int)linea.size(); i++) {
        if (linea[i] == ',') linea[i] = ' ';
    }

    istringstream lector(linea);
    if (!(lector >> fila >> col)) return false;

    string sobrante;
    if (lector >> sobrante) return false;

    return dentroDelTablero(fila, col);
}

void crearTablero() {
    for (int fila = 0; fila < 8; fila++) {
        for (int col = 0; col < 8; col++) {
            tablero[fila][col] = '.';

            if ((fila + col) % 2 == 1) {
                if (fila < 3) tablero[fila][col] = 'X';
                if (fila > 4) tablero[fila][col] = 'O';
            }
        }
    }
}

void limpiarMarcas() {
    for (int fila = 0; fila < 8; fila++) {
        for (int col = 0; col < 8; col++) {
            marcas[fila][col] = 0;
        }
    }
}

void mostrarTablero() {
    cout << "\n      0  1  2  3  4  5  6  7   <- columnas\n";

    for (int fila = 0; fila < 8; fila++) {
        cout << "  " << fila << "  ";

        for (int col = 0; col < 8; col++) {

            if (marcas[fila][col] == 1)         cout << CASILLA_BLANCA;
            else if ((fila + col) % 2 == 1)     cout << CASILLA_NEGRA;
            else                                cout << CASILLA_BLANCA;

            char p = tablero[fila][col];
            if (marcas[fila][col] == 2)  cout << " * ";
            else if (p == 'X')           cout << " X ";
            else if (p == 'x')           cout << "[X]";
            else if (p == 'O')           cout << " O ";
            else if (p == 'o')           cout << "[O]";
            else                         cout << "   ";

            cout << RESET;
        }
        cout << "  " << fila << "\n";
    }

    cout << "\n      0  1  2  3  4  5  6  7\n\n";

    cout << "  X / O = fichas    [X] / [O] = damas    * = posible movimiento\n";
}

int obtenerMovimientos(int fila, int col, Movimiento lista[], bool soloCapturas) {
    char pieza = tablero[fila][col];
    int dueno = duenio(pieza);
    int cantidad = 0;

    int dFila[4] = { 1,  1, -1, -1};
    int dCol[4]  = { 1, -1,  1, -1};

    for (int i = 0; i < 4; i++) {

        if (pieza == 'X' && dFila[i] < 0) continue;
        if (pieza == 'O' && dFila[i] > 0) continue;

        int f1 = fila + dFila[i];
        int c1 = col + dCol[i];
        if (!dentroDelTablero(f1, c1)) continue;

        if (tablero[f1][c1] == '.') {

            if (!soloCapturas) {
                lista[cantidad].filaDestino = f1;
                lista[cantidad].colDestino = c1;
                lista[cantidad].esCaptura = false;
                cantidad++;
            }
        }
        else if (duenio(tablero[f1][c1]) != dueno) {

            int f2 = fila + 2 * dFila[i];
            int c2 = col + 2 * dCol[i];
            if (dentroDelTablero(f2, c2) && tablero[f2][c2] == '.') {
                lista[cantidad].filaDestino = f2;
                lista[cantidad].colDestino = c2;
                lista[cantidad].esCaptura = true;
                cantidad++;
            }
        }
    }
    return cantidad;
}

bool hayCapturaObligatoria(int jugador) {
    Movimiento movs[4];
    for (int fila = 0; fila < 8; fila++) {
        for (int col = 0; col < 8; col++) {
            if (duenio(tablero[fila][col]) == jugador) {
                if (obtenerMovimientos(fila, col, movs, true) > 0) return true;
            }
        }
    }
    return false;
}

bool hayMovimientos(int jugador) {
    Movimiento movs[4];
    for (int fila = 0; fila < 8; fila++) {
        for (int col = 0; col < 8; col++) {
            if (duenio(tablero[fila][col]) == jugador) {
                if (obtenerMovimientos(fila, col, movs, false) > 0) return true;
            }
        }
    }
    return false;
}

void elegirFicha(int jugador, bool soloCapturas, int &fila, int &col) {
    limpiarMarcas();
    mostrarTablero();
    cout << "Turno del jugador " << nombreJugador(jugador) << "\n";
    if (soloCapturas) {
        cout << "!! Tienes una captura obligatoria: debes comer una ficha.\n";
    }

    while (true) {
        cout << "Ficha que quieres mover (fila y columna, ejemplo: 5 0) o escribe 'rendirse': ";
        string texto = leerLinea();

        if (texto == "rendirse") {
            rendido = true;
            return;
        }
        if (!leerCoordenada(texto, fila, col)) {
            cout << "Casilla no valida. Escribe dos numeros del 0 al 7 (fila y columna).\n";
            continue;
        }
        if (duenio(tablero[fila][col]) != jugador) {
            cout << "Ahi no hay una ficha tuya.\n";
            continue;
        }

        Movimiento movs[4];
        int cantidad = obtenerMovimientos(fila, col, movs, soloCapturas);
        if (cantidad == 0) {
            cout << "Esa ficha no tiene movimientos posibles.";
            if (soloCapturas) cout << " (Recuerda: tienes que capturar)";
            cout << "\n";
            continue;
        }
        return;
    }
}

void mostrarPosibles(int fila, int col, Movimiento movs[], int cantidad) {
    limpiarMarcas();
    marcas[fila][col] = 1;
    for (int i = 0; i < cantidad; i++) {
        marcas[movs[i].filaDestino][movs[i].colDestino] = 2;
    }
    mostrarTablero();

    cout << "La ficha elegida es la " << nombreCasilla(fila, col)
         << " (se ve con los colores invertidos).\n";
    cout << "Posibles movimientos (fila,columna):\n";
    for (int i = 0; i < cantidad; i++) {
        cout << "  " << i + 1 << ") " << nombreCasilla(movs[i].filaDestino, movs[i].colDestino);
        if (movs[i].esCaptura) cout << "  (captura)";
        cout << "\n";
    }
}

int pedirDestino(Movimiento movs[], int cantidad, bool puedeCambiar) {
    while (true) {
        if (puedeCambiar) {
            cout << "A que casilla la quieres mover? (fila y columna) o escribe 'cambiar' para elegir otra ficha: ";
        } else {
            cout << "Tienes que seguir comiendo con esta ficha. Casilla destino (fila y columna): ";
        }
        string texto = leerLinea();

        if (puedeCambiar && texto == "cambiar") return -1;

        int fila, col;
        if (leerCoordenada(texto, fila, col)) {
            for (int i = 0; i < cantidad; i++) {
                if (movs[i].filaDestino == fila && movs[i].colDestino == col) {
                    return i;
                }
            }
        }
        cout << "Ese movimiento no esta en la lista. Prueba otra vez.\n";
    }
}

void jugarTurno(int jugador) {
    bool soloCapturas = hayCapturaObligatoria(jugador);
    bool sigueSaltando = false;
    int fila = 0, col = 0;

    while (true) {

        if (!sigueSaltando) {
            elegirFicha(jugador, soloCapturas, fila, col);
            if (rendido) return;
        }

        Movimiento movs[4];
        int cantidad = obtenerMovimientos(fila, col, movs, soloCapturas);
        mostrarPosibles(fila, col, movs, cantidad);

        int indice = pedirDestino(movs, cantidad, !sigueSaltando);
        if (indice == -1) continue;

        Movimiento m = movs[indice];
        string desde = nombreCasilla(fila, col);
        string hasta = nombreCasilla(m.filaDestino, m.colDestino);

        char pieza = tablero[fila][col];
        tablero[fila][col] = '.';
        tablero[m.filaDestino][m.colDestino] = pieza;

        if (m.esCaptura) {
            int filaMedio = (fila + m.filaDestino) / 2;
            int colMedio  = (col + m.colDestino) / 2;
            tablero[filaMedio][colMedio] = '.';
        }

        bool corono = false;
        if (pieza == 'X' && m.filaDestino == 7) {
            tablero[m.filaDestino][m.colDestino] = 'x';
            corono = true;
        }
        if (pieza == 'O' && m.filaDestino == 0) {
            tablero[m.filaDestino][m.colDestino] = 'o';
            corono = true;
        }

        string texto = "Jugador " + nombreJugador(jugador) + ": " + desde + " -> " + hasta;
        if (m.esCaptura) texto += " (captura)";
        if (corono) texto += " (se convierte en dama)";
        historial.push_back(texto);
        cout << "\n>> " << texto << "\n";

        fila = m.filaDestino;
        col = m.colDestino;
        if (m.esCaptura && !corono) {
            Movimiento otros[4];
            if (obtenerMovimientos(fila, col, otros, true) > 0) {
                cout << "Puedes seguir comiendo con la misma ficha!\n";
                sigueSaltando = true;
                soloCapturas = true;
                continue;
            }
        }
        break;
    }
}

void guardarPartida(string ganador, string motivo) {

    ofstream archivo("partidas.txt", ios::app);
    if (!archivo) {
        cout << "No se pudo abrir el archivo partidas.txt\n";
        return;
    }

    time_t ahora = time(0);

    archivo << "==============================================\n";
    archivo << "Fecha: " << ctime(&ahora);
    archivo << "Ganador: jugador " << ganador << " (" << motivo << ")\n";
    archivo << "Total de movimientos: " << historial.size() << "\n";
    archivo << "Movimientos (fila,columna):\n";
    for (int i = 0; i < (int)historial.size(); i++) {
        archivo << "  " << i + 1 << ". " << historial[i] << "\n";
    }
    archivo << "\n";
    archivo.close();

    cout << "Partida guardada en partidas.txt\n";
}

int main() {
    system("");

    cout << "=== DAMAS INGLESAS ===\n";
    cout << "Las casillas se escriben con dos numeros del 0 al 7: fila y columna (ejemplo: 5 0)\n";

    crearTablero();
    int jugador = JUGADOR_X;
    string ganador = "";
    string motivo = "";

    while (true) {
        jugarTurno(jugador);

        if (rendido) {
            ganador = nombreJugador(contrario(jugador));
            motivo = "el rival se rindio";
            break;
        }

        jugador = contrario(jugador);

        if (!hayMovimientos(jugador)) {
            ganador = nombreJugador(contrario(jugador));
            motivo = "el rival se quedo sin movimientos";
            break;
        }
    }

    limpiarMarcas();
    mostrarTablero();
    cout << "\n*** FIN DE LA PARTIDA ***\n";
    cout << "Gana el jugador " << ganador << " (" << motivo << ")\n";

    guardarPartida(ganador, motivo);
    return 0;
}
