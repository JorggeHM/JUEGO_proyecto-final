//actualizacion de menu agrgada ;b
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <windows.h>  
#include <conio.h> //leer teclas

//lector de teclas
#define ARRIBA 72
#define ABAJO 80
#define ENTER 13

using namespace std;


enum Dificultad {
    FACIL,
    NORMAL,
    DIFICIL
};
int velocidadEnemigos;
int frecuenciaPicos;
int cantidadPlataformas;
//ya no funciona, utilice el witch en la nueva parte
void seleccionarDificultad(Dificultad dificultad) {
    switch (dificultad) {
    case FACIL:
        velocidadEnemigos = 5;
        frecuenciaPicos = 10;
        cantidadPlataformas = 5;
        break;
    case NORMAL:
        velocidadEnemigos = 10;
        frecuenciaPicos = 20;
        cantidadPlataformas = 10;
        break;
    case DIFICIL:
        velocidadEnemigos = 15;
        frecuenciaPicos = 30;
        cantidadPlataformas = 15;
        break;
    }
}

class Enemy {
public:
    sf::RectangleShape shape;

    Enemy(float x, float y) {
        shape.setSize(sf::Vector2f(50.0f, 50.0f));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Red);
    }

    void update(float deltaTime) {
        shape.move(-50.0f * deltaTime, 0.0f); // Mover el enemigo hacia la izquierda
    }
};

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    bool isGrounded;
    int animationFrame = 0;

    Player() {
        if (!texture.loadFromFile("mario_spritesheet.png")) {
            std::cerr << "Error al cargar la textura." << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
        sprite.setPosition(100.0f, 100.0f);
        velocity = sf::Vector2f(0.0f, 0.0f);
        isGrounded = false;
    }

    void update(float deltaTime, const std::vector<sf::RectangleShape*>& platforms, const std::vector<std::unique_ptr<Enemy>>& enemies, const std::vector<sf::ConvexShape>& spikes) {
        velocity.y += 981.0f * deltaTime;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = 200.0f;
            animationFrame++;
            sprite.setTextureRect(sf::IntRect(50 * (animationFrame % 3), 0, 50, 50));
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -200.0f;
            animationFrame++;
            sprite.setTextureRect(sf::IntRect(50 * (animationFrame % 3), 50, 50, 50));
        }
        else {
            velocity.x = 0.0f;
            sprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && isGrounded) {
            velocity.y = -400.0f;
            isGrounded = false;
        }

        sprite.move(velocity * deltaTime);

        // Detección de colisiones con plataformas
        for (auto& platform : platforms) {
            if (sprite.getGlobalBounds().intersects(platform->getGlobalBounds())) {
                if (velocity.y > 0) {
                    sprite.setPosition(sprite.getPosition().x, platform->getPosition().y - sprite.getGlobalBounds().height);
                    isGrounded = true;
                    velocity.y = 0;
                }
            }
        }

        // Detección de colisiones con enemigos
        for (const auto& enemy : enemies) {
            if (sprite.getGlobalBounds().intersects(enemy->shape.getGlobalBounds())) {
                std::cout << "¡Has sido tocado por un enemigo! Fin del juego." << std::endl;
                exit(0);
            }
        }

        // Detección de colisiones con picos
        for (const auto& spike : spikes) {
            if (sprite.getGlobalBounds().intersects(spike.getGlobalBounds())) {
                std::cout << "¡Has caído en un pico! Fin del juego." << std::endl;
                exit(0);
            }
        }
    }
};

class Goal {
public:
    sf::RectangleShape shape;

    Goal(float x, float y) {
        shape.setSize(sf::Vector2f(50.0f, 50.0f));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Green);
    }
};

void generateMap(std::vector<sf::RectangleShape*>& platforms, Goal& goal, std::vector<sf::ConvexShape>& spikes, std::vector<std::unique_ptr<Enemy>>& enemies) {
    // ...

    platforms.clear();
    spikes.clear();

    const int platformCount = 15;
    const float platformWidth = 200.0f;
    const float platformHeight = 30.0f;

    for (int i = 0; i < platformCount; ++i) {
        float x = i * (platformWidth + 50);
        float y = 500 - (rand() % 2 * 100) - 50;

        auto platform = new sf::RectangleShape(sf::Vector2f(platformWidth, platformHeight));
        platform->setPosition(x, y);
        platform->setFillColor(sf::Color::Blue);
        platforms.push_back(platform);

        // Generar enemigos en la parte superior de las plataformas
        if (i % 3 == 0) {
            float enemyX = x + platformWidth / 2;
            float enemyY = y - 25; // Ajusta la posición en el eje Y

            // Verificar que el enemigo no se genere demasiado cerca del jugador
            if (enemyX > 200.0f) { // 200.0f es la distancia mínima entre el jugador y el enemigo
                auto enemy = std::make_unique<Enemy>(enemyX, enemyY);
                enemy->shape.setSize(sf::Vector2f(30.0f, 30.0f)); // Ajusta el tamaño del enemigo
                enemies.push_back(std::move(enemy));
            }


            sf::ConvexShape spike;//generar picos
            spike.setPointCount(3);
            spike.setPoint(0, sf::Vector2f(x + platformWidth / 2, y + platformHeight)); // Ajusta la posición en el eje Y
            spike.setPoint(1, sf::Vector2f(x + platformWidth / 2 - 25, y + platformHeight + 50)); // Ajusta la posición en el eje Y
            spike.setPoint(2, sf::Vector2f(x + platformWidth / 2 + 25, y + platformHeight + 50)); // Ajusta la posición en el eje Y
            spike.setFillColor(sf::Color::Magenta);
            spikes.push_back(spike);
        }

        // Posicionar la meta al final del mapa
        goal = Goal(platforms.back()->getPosition().x + 50, platforms.back()->getPosition().y - 50);

    }
}

//POSICIONAR TITULO Y OPCIOENS
void gotoxy(int x, int y) {
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

void dibujarTitulo() {
    cout << "   =============================================================" << endl;
    cout << "           ## ##    ##  ##    ##  ######    ########  ######    " << endl;
    cout << "           ## ##    ##  ########  ##    ##  ##        ##    ##  " << endl;
    cout << "           ## ##    ##  ## ## ##  ##    ##  #####     ##    ##  " << endl;
    cout << "           ## ##    ##  ##    ##  ######    #####     ######    " << endl;
    cout << "    ##     ## ##    ##  ##    ##  ##        ##        ##    ##  " << endl;
    cout << "     #######   ######   ##    ##  ##        ########  ##     ## " << endl;
    cout << "   =============================================================" << endl;
}

//funcion de menu seleccion de dificultad
int menuDificultad() {
    int tecla;
    int dificultad;
    bool sopas = true;
    int opcionSeleccionada = 1;

    do {
        system("cls");
        gotoxy(26, 8 + opcionSeleccionada); cout << "==>";
        gotoxy(38, 8 + opcionSeleccionada); cout << "<==";

        gotoxy(0, 0);
        dibujarTitulo();

        gotoxy(30, 9); cout << "Facil" << endl;
        gotoxy(30, 10); cout << "Normal" << endl;
        gotoxy(30, 11); cout << "Dificil" << endl;

        do {
            tecla = _getch(); // Uso correcto de _getch()
        } while (tecla != ARRIBA && tecla != ABAJO && tecla != ENTER);

        switch (tecla) {
        case ARRIBA:
            opcionSeleccionada--;
            if (opcionSeleccionada == 0) {
                opcionSeleccionada = 3;
            }
            break;
        case ABAJO:
            opcionSeleccionada++;
            if (opcionSeleccionada == 4) {
                opcionSeleccionada = 1;
            }
            break;
        case ENTER:
            sopas = false;
            dificultad = opcionSeleccionada;
            break;
        }
    } while (sopas);

    return dificultad;
}

int main() {
    int dificultad = menuDificultad();//retorna el valor de dificultad
    cout << "Dificultad seleccionada: " << dificultad << endl;

    switch (dificultad) {
    case 1:
        velocidadEnemigos = 5;
        frecuenciaPicos = 10;
        cantidadPlataformas = 5;
        break;
    case 2:
        velocidadEnemigos = 10;
        frecuenciaPicos = 20;
        cantidadPlataformas = 10;
        break;
    case 3:
        velocidadEnemigos = 15;
        frecuenciaPicos = 30;
        cantidadPlataformas = 15;
        break;
    }

    //inicio del juego
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mario Style Game");

    Player player;

    Goal goal(0, 0);
    //mostrarPantallaDificultad();
    std::vector<sf::RectangleShape*> platforms;
    std::vector<sf::ConvexShape> spikes; // Para los picos
    std::vector<std::unique_ptr<Enemy>> enemies;

    generateMap(platforms, goal, spikes, enemies);

    player.sprite.setPosition(50.0f, platforms[0]->getPosition().y - 50);

    enemies.push_back(std::make_unique<Enemy>(200.0f, 470.0f));
    enemies.push_back(std::make_unique<Enemy>(400.0f, 470.0f));
    enemies.push_back(std::make_unique<Enemy>(600.0f, 470.0f));

    sf::Clock clock;

    sf::View view;
    view.setSize(800.0f, 600.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        player.update(deltaTime, platforms, enemies, spikes);

        for (auto& enemy : enemies) {
            enemy->update(deltaTime);
        }

        view.setCenter(player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2, 300.0f);
        window.setView(view);

        if (player.sprite.getGlobalBounds().intersects(goal.shape.getGlobalBounds())) {
            std::cout << "¡Has alcanzado la meta!" << std::endl;
            window.close();
        }

        window.clear();
        window.draw(player.sprite);
        for (const auto& platform : platforms) {
            window.draw(*platform);
        }
        for (const auto& enemy : enemies) {
            window.draw(enemy->shape);
        }
        for (const auto& spike : spikes) {
            window.draw(spike);
        }
        window.draw(goal.shape);
        window.display();
    }

    // Liberar memoria de las plataformas
    for (auto platform : platforms) {
        delete platform;
    }

    return 0;
}

