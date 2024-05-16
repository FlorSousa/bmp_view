#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "bmpreader.h"
#include <string>

void showWindow(std::vector<sf::Uint8>* pixels, int width, int height,std::string filename) {
    int windowWidth = width;
    int windowHeight = height;

    // Criar uma janela SFML
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Visualizador de Pixels");

    // Criar uma textura SFML
    sf::Texture texture;
    texture.create(width, height);
    texture.update(reinterpret_cast<sf::Uint8*>(pixels->data()));

    // Criar um sprite SFML para exibir a textura
    sf::Sprite sprite(texture);

    // Laço principal
    while (window.isOpen()) {
        // Processar eventos
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Limpar a janela
        window.clear();

        // Desenhar o sprite
        window.draw(sprite);

        // Atualizar a janela
        window.display();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Número de argumentos: " << argc << std::endl;
    // Carregar a imagem BMP
    BMP* image = new BMP;
    std::string filename = "cacete.bmp";
    std::vector<char> bytesVector = getDataVector(filename, image);
    std::vector<sf::Uint8> pixelsx;
    reader(&bytesVector, image, &pixelsx);
    
    for (int k = 0; k < 400; k++) {
        std::cout << int(pixelsx[k]) << "\n";
    }
    showWindow(&pixelsx, image->x, image->y,filename);
    return 0;
}
