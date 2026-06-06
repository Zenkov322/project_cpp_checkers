#include "checkers.hpp"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {
    // Начало взаимствованного кода, для исправления проблем с кодировкой в Windows-консоли при отображении русских символов
    #ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    #endif
    // Конец взаимствованного кода
    std::cout << "Добро пожаловать в игру Шашки!\n";
    CheckersBoard game;
    while (true) {
        game.displayBoard();

        std::cout << "Съедено белых: " << game.getCapturedCount(true) 
        
                  << " | Съедено черных: " << game.getCapturedCount(false) << "\n";
        if (!game.hasPieces(true)) {
            std::cout << "Черные победили.\n";
            break;
        }
        if (!game.hasPieces(false)) {
            std::cout << "Белые победили.\n";
            break;
        }
 
        if (game.hasObligatoryCaptures()) {
            std::cout << "ВНИМАНИЕ: Вы обязательны походить взятием!\n";
        }
 
        std::cout << (game.isWhiteTurn() ? "Ход БЕЛЫХ (w/W)" : "Ход ЧЕРНЫХ (b/B)") << "\n";
        std::cout << "Введите ход (откуда -> куда: '1 1' -> '2 2') либо -1 для выхода из игры: ";
        
        int r1, c1, r2, c2;
        if (!(std::cin >> r1)) break;
        if (r1 == -1) {
            std::cout << "Игра завершена.\n";
            break;
        }
        std::cin >> c1 >> r2 >> c2;
 
        try {
            game.makeMove(r1, c1, r2, c2);
            std::cout << "Ход успешно выполнен.\n\n";
        } 
        catch (const std::out_of_range& e) {
            std::cerr << " Ошибка диапазона: " << e.what() << " Повторите ввод.\n\n";
        } 
        catch (const std::invalid_argument& e) {
            std::cerr << " Ошибка правил игры: " << e.what() << " Повторите ввод.\n\n";
        }
        catch (const std::exception& e) {
            std::cerr << " Критическая ошибка: " << e.what() << "\n\n";
        }
    }
    return 0;
}