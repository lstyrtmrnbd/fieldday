/**
 * fieldday
 * relax and have fun
 */

#include <iostream>
#include <functional>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

int main() {

  sf::RenderWindow window(sf::VideoMode(1280, 720), "fieldday");

  Texture gremlin;
  if (!gremlin.loadFromFile("gremlin.png")) {
    std::cout << "Failed to load gremlin.png...\n";
  }

  Sprite sprite(gremlin);
  
  while (window.isOpen()) {
    
    sf::Event event;
    
    while (window.pollEvent(event)) {
      
      if (event.type == sf::Event::Closed)
        window.close();
    }
    
    window.clear();
    window.draw(sprite);
    window.display();
  }
  
  return 0;
}


