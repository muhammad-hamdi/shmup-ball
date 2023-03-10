#include<SFML/Graphics.hpp>
#include<SFML\Audio.hpp>
#include<iostream>
#include<math.h>

static const float VIEW_HEIGHT  = 600;

void resizeView(const sf::RenderWindow& window, sf::View& view) {
    float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
    view.setSize(VIEW_HEIGHT * aspectRatio, VIEW_HEIGHT);
}

// Handle interrupt events
void handleEvents(sf::RenderWindow& window, sf::View& view) {
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                resizeView(window, view);
        }
    }
}

struct Projectile {
    sf::CircleShape shape;
    sf::Vector2f dir;
};

struct Enemy {
    sf::RectangleShape shape;
    int hitCount = 0;
    int health = 0;
};

enum Scene {
    Menu, Gameplay, GameOver
};

int main()
{
    srand(time(NULL));
    // Create Window
    Scene currentScene = Menu;
    sf::RenderWindow window(sf::VideoMode(800, 600), "We Ballin'");
    sf::View view(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(800, VIEW_HEIGHT));

    sf::Music menuMusic;
    sf::Music gameplayMusic;
    sf::Music gameOverMusic;

    if (!menuMusic.openFromFile("menu.wav"))
        return EXIT_FAILURE;
    
    if (!gameplayMusic.openFromFile("gameplay.wav"))
        return EXIT_FAILURE;
    
    if (!gameOverMusic.openFromFile("game_over.wav"))
        return EXIT_FAILURE;
    
    menuMusic.setLoop(true);
    gameplayMusic.setLoop(true);
    gameOverMusic.setLoop(true);

    sf::Font font;
    if(!font.loadFromFile("Gameplay.ttf"))
        return EXIT_FAILURE;

    sf::Text titleText("We Ballin'", font, 60);
    titleText.setFillColor(sf::Color(255, 0, 171));
    titleText.setOutlineColor(sf::Color(255, 255, 171));
    titleText.setOutlineThickness(5);
    titleText.setStyle(sf::Text::Style::Bold);
    sf::Vector2f ttPosition(window.getSize().x / 2 - titleText.getGlobalBounds().width/2, 100);
    titleText.setPosition(ttPosition);

    sf::Text pressToStartText("Press To Start", font, 20);
    pressToStartText.setPosition(window.getSize().x / 2 - pressToStartText.getGlobalBounds().width/2, window.getSize().y / 2 - pressToStartText.getGlobalBounds().height/2);
    
    sf::Text gameOverText("Game Over", font, 60);
    gameOverText.setFillColor(sf::Color(0, 0, 0));
    gameOverText.setOutlineColor(sf::Color(255, 255, 255));
    gameOverText.setOutlineThickness(5);
    gameOverText.setStyle(sf::Text::Style::Bold);
    gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getGlobalBounds().width/2, window.getSize().y / 2 - gameOverText.getGlobalBounds().height*2);

    sf::Text finalScoreText("Score: ", font, 30);
    finalScoreText.setPosition(window.getSize().x / 2 - finalScoreText.getGlobalBounds().width/2, window.getSize().y / 2 - finalScoreText.getGlobalBounds().height/2);
    sf::Text hiScoreText("High Score: ", font, 20);
    finalScoreText.setPosition(window.getSize().x / 2 - finalScoreText.getGlobalBounds().width/2, window.getSize().y / 2 - finalScoreText.getGlobalBounds().height/2);
    
    sf::Text livesText("Lives: 3", font, 14);
    sf::Text scoreText("Score: 0", font, 14);
    scoreText.setPosition(0, livesText.getGlobalBounds().height + 5);

    sf::Text credit("By: Hamdi", font, 18);
    credit.setPosition(window.getSize().x - (credit.getGlobalBounds().width + 10), window.getSize().y - credit.getGlobalBounds().height*2);

    //player
    sf::CircleShape player;
    player.setRadius(20.f);
    player.setOrigin(player.getRadius(), player.getRadius());
    sf::Vector2f defaultPlayerPosition(window.getSize().x/2, window.getSize().y - player.getRadius()*3);
    player.setPosition(defaultPlayerPosition);
    player.setFillColor(sf::Color::White);
    float velocity = 0;
    float speed = 300;
    float shootTimer = 0;
    int lives = 3;
    int score = 0;
    int highScore = 0;

    //projectiles
    Projectile projectilePrototype;
    projectilePrototype.shape.setRadius(5.f);
    projectilePrototype.shape.setOrigin(projectilePrototype.shape.getRadius(), projectilePrototype.shape.getRadius());
    projectilePrototype.shape.setFillColor(sf::Color::Blue);
    float projectileSpeed = 10;

    std::vector<Projectile> projectiles;

    //enemies
    Enemy enemyPrototype;
    enemyPrototype.shape.setFillColor(sf::Color::Red);
    enemyPrototype.shape.setSize(sf::Vector2f(50.f, 50.f));
    float enemySpeed = 5;

    std::vector<Enemy> enemies;

    int enemySpawnTimer = 0;

    // Set fps to 60
    window.setFramerateLimit(60);

    int freezeTime = 60;
    int freezeTimer = 0;
    bool freeze = false;

    menuMusic.play();

    // init clock for deltaTime
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        // Calculate deltaTime
        float deltaTime = deltaClock.restart().asSeconds();

        // events
        handleEvents(window, view);

        window.setView(view);

        //UPDATE

        //menu update
        if(currentScene == Menu) {
            titleText.move(0, 0.5 * sin(time(NULL)));
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                menuMusic.stop();
                gameplayMusic.play();
                currentScene = Gameplay;
            }
        }
        //end menu update

        //gameplay update
        if(currentScene == Gameplay) {
            //projectiles
            if(shootTimer <= 3)
                shootTimer++;
            
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer >= 3)
            {
                projectilePrototype.shape.setPosition(player.getPosition());
                sf::Vector2f dir = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) - projectilePrototype.shape.getPosition();
                float magnitude = sqrt(dir.x*dir.x + dir.y*dir.y);
                magnitude > 0 ? dir /= magnitude : dir;
                projectilePrototype.dir = dir;
                projectiles.push_back(Projectile(projectilePrototype));
                shootTimer = 0;
            }

            int pI = 0;
            for(Projectile& p : projectiles) {
                p.shape.move(p.dir * (freezeTimer > 0 ? 0 : projectileSpeed));

                if(p.shape.getPosition().y <= 0)
                    projectiles.erase(projectiles.begin() + pI);
                for(int i = 0; i < enemies.size(); i++) {
                    if(p.shape.getGlobalBounds().intersects(enemies[i].shape.getGlobalBounds())) {
                        if(enemies[i].hitCount >= enemies[i].health) {
                            score += (enemies[i].health + enemies[i].hitCount) * 10;
                            enemies.erase(enemies.begin() + i);
                        } else {
                            enemies[i].hitCount++;
                            score++;
                        }
                        scoreText.setString("Score: " + std::to_string(score));
                        projectiles.erase(projectiles.begin() + pI);
                    }
                }
                pI++;
            }

            //player
            velocity = 0;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocity = -1;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) velocity = 1;

            player.move(velocity * speed * deltaTime * !freezeTimer, 0);

            //enemies
            if(enemySpawnTimer <= 10)
                enemySpawnTimer++;
            
            if(enemySpawnTimer >= 10 && !freeze) {
                enemyPrototype.shape.setPosition(rand() % int(window.getSize().x-enemyPrototype.shape.getSize().x), 10.f);
                enemyPrototype.health = rand() % 10;
                enemies.push_back(Enemy(enemyPrototype));
                enemySpawnTimer = 0;
            }

            int eI = 0;
            for(Enemy& e : enemies) {
                e.shape.move(0, freezeTimer > 0 ? 0 : enemySpeed);
                if(e.shape.getPosition().y >= window.getSize().y)
                    enemies.erase(enemies.begin() + eI);
                if(e.shape.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    freeze = true;
                }
                eI++;
            }

            if (freeze) {
                freezeTimer++;
                if(freezeTimer>= freezeTime) {
                    freezeTimer = 0;
                    freeze = false;
                    enemies.clear();
                    projectiles.clear();
                    player.setPosition(defaultPlayerPosition);
                    lives--;
                    if(lives <= 0) {
                        gameOverMusic.play();
                        currentScene = GameOver;
                    }
                    livesText.setString("Lives: " + std::to_string(lives));
                } 
            }
        }
        //end gameplay update

        //gameover update
        if(currentScene == GameOver) {
            gameplayMusic.stop();
            gameOverText.move(0, 0.5 * sin(time(NULL)));
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                gameOverMusic.stop();
                gameplayMusic.play();
                lives = 3;
                livesText.setString("Lives: " + std::to_string(lives));
                if(score > highScore) highScore = score;
                score = 0;
                scoreText.setString("Score: " + std::to_string(score));
                currentScene = Gameplay;
            }
        }
        //end gameover update

        // DRAW
        window.clear();

        //menu draw
        if(currentScene == Menu) {
            window.draw(titleText);
            pressToStartText.setString("Press Space To Start");
            pressToStartText.setPosition(window.getSize().x / 2 - pressToStartText.getGlobalBounds().width/2, window.getSize().y / 2 - pressToStartText.getGlobalBounds().height/2);
            window.draw(pressToStartText);
            window.draw(credit);
        }
        //end menu draw

        // gameplay draw
        if (currentScene == Gameplay) {
            window.draw(player);

            for(Enemy& enemy : enemies) {
                window.draw(enemy.shape);
            }

            for(Projectile& projectile : projectiles) {
                window.draw(projectile.shape);
            }

            window.draw(livesText);
            window.draw(scoreText);
        }
        // end gameplay draw

        //gameover draw
        if(currentScene == GameOver) {
            window.draw(gameOverText);
            finalScoreText.setString("Score: " + std::to_string(score));
            finalScoreText.setPosition(window.getSize().x / 2 - finalScoreText.getGlobalBounds().width/2, window.getSize().y / 2 - finalScoreText.getGlobalBounds().height/2);
            window.draw(finalScoreText);
            if(highScore > 0) {
                hiScoreText.setString("High Score: " + std::to_string(highScore));
                hiScoreText.setPosition(window.getSize().x / 2 - hiScoreText.getGlobalBounds().width/2, window.getSize().y / 2 + 50);
                window.draw(hiScoreText);
            }
            pressToStartText.setString("Press Space To Try Again");
            pressToStartText.setPosition(window.getSize().x / 2 - pressToStartText.getGlobalBounds().width/2, window.getSize().y / 2 + 200);
            window.draw(pressToStartText);
            window.draw(credit);
        }
        //end gameover draw
        window.display();
    }

    return 0;
}

