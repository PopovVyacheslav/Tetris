#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <windows.h>
#include <fstream>

using namespace sf;

const int M = 20;
const int N = 10;

int field[M][N] = {0};

struct Point
{int x,y;} a[4], b[4];

int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

bool check()
{
   for (int i=0;i<4;i++)
	  if (a[i].x<0 || a[i].x>=N || a[i].y>=M) return 0;
      else if (field[a[i].y][a[i].x]) return 0;

   return 1;
};

bool game_over()
{
    for (int i = 0; i < 4; i++)
        if (field[a[i].y][a[i].x] != 0) return 1;
    return 0;
};

void results(int score)
{
    RenderWindow Results(VideoMode(280, 110), "Results");

    Font font;
    font.loadFromFile("PLAYBILL.TTF");
    Text text("Score:   " + std::to_string(score) + "\nFor saving your score press S", font, 30);
    text.setOutlineColor(Color::Black);
    text.setFillColor(Color::Black);
    text.setPosition(10, 20);
    
    while (Results.isOpen())
    {
        Event ev;
        while (Results.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
                Results.close();

            if (ev.type == Event::KeyPressed)
                if (ev.key.code == Keyboard::S)
                {
                    std::time_t seconds = time(NULL);
                    tm* timeinfo = localtime(&seconds);
                    std::string scoretime = asctime(timeinfo);
                    std::ofstream scorefile;
                    scorefile.open("Score list.txt", std::ios_base::app);
                    if (scorefile)
                    {
                        scorefile << "Score = " + std::to_string(score) + " at " + scoretime;
                        scorefile.close();
                    };
                };
        }
        Results.clear(Color::White);
        Results.draw(text);
        Results.display();
    };
    
};

void pause()
{
    RenderWindow Pause(VideoMode(210, 50), "Pause");

    Font font;
    font.loadFromFile("PLAYBILL.TTF");
    Text text("Press Esc for continue.", font, 30);
    text.setOutlineColor(Color::Black);
    text.setFillColor(Color::Black);
    text.setPosition(10, 8);

    while (Pause.isOpen())
    {
        Event e;
        while (Pause.pollEvent(e))
        {
            if (e.type == Event::Closed)
                Pause.close();

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Escape)
                    Pause.close();
        }

        Pause.clear(Color::White);
        Pause.draw(text);
        Pause.display();
    }
};

int main()
{
    FreeConsole();
    srand(time(0));	 

    int score = 0;

	RenderWindow window(VideoMode(320, 480), "The Game!");

    Texture t1,t2,t3;
	t1.loadFromFile("images/tiles.png");
	t2.loadFromFile("images/background.png");
	t3.loadFromFile("images/frame.png");

	Sprite s(t1), background(t2), frame(t3);

    int dx=0; bool rotate=0; int colorNum=1;
	float timer=0,delay=0.3; 

	Clock clock;

    Font font;
    font.loadFromFile("PLAYBILL.TTF");
    Text text("", font, 25);
    text.setOutlineColor(Color::Black);
    text.setFillColor(Color::Color(0,105,62,255));
    text.setPosition(35, 425);

    Music music;
    music.openFromFile("tetris_theme.ogg");
    music.setLoop(TRUE);
    music.play();

    int n = rand() % 7;
    colorNum = 1 + n;
    for (int i = 0; i < 4; i++)
    {
        a[i].x = figures[n][i] % 2 + 4;
        a[i].y = figures[n][i] / 2;
    }

    while (window.isOpen())
    {
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer+=time;

        delay = 0.3 - (score / 5) * 0.01;
        

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
                else if (e.key.code == Keyboard::P) pause();
		}

	if (Keyboard::isKeyPressed(Keyboard::Down)) delay=0.05;

	//// <- Move -> ///
	for (int i=0;i<4;i++)  { b[i]=a[i]; a[i].x+=dx; }
    if (!check()) for (int i=0;i<4;i++) a[i]=b[i];

	//////Rotate//////
	if (rotate)
	  {
		Point p = a[1]; //center of rotation
		for (int i=0;i<4;i++)
		  {
			int x = a[i].y-p.y;
			int y = a[i].x-p.x;
			a[i].x = p.x - x;
			a[i].y = p.y + y;
	 	  }
   	    if (!check()) for (int i=0;i<4;i++) a[i]=b[i];
	  }

	///////Tick//////
	if (timer>delay)
	  {
	    for (int i=0;i<4;i++) { b[i]=a[i]; a[i].y+=1; }
        
		if (!check())
		{
            for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

         int n = rand() % 7;
         colorNum = 1 + n;
		 for (int i=0;i<4;i++)
		   {
		    a[i].x = figures[n][i] % 2 + 4;
		    a[i].y = figures[n][i] / 2;
		   }

         if (game_over())
         {
             results(score);
             break;
         }

		}

	  	timer=0;
	  }

	///////check lines//////////
    int k=M-1;
	for (int i=M-1;i>0;i--)
	{
		int count=0;
		for (int j=0;j<N;j++)
		{
		    if (field[i][j]) count++;
		    field[k][j]=field[i][j];
		}
		if (count<N) k--;
        else score++;
	}

    text.setString("Score:  " + std::to_string(score));

    dx=0; rotate=0; delay=0.3;

    /////////draw//////////
    window.clear(Color::White);	
    window.draw(background);
		  
	for (int i=0;i<M;i++)
	 for (int j=0;j<N;j++)
	   {
         if (field[i][j]==0) continue;
		 s.setTextureRect(IntRect(field[i][j]*18,0,18,18));
		 s.setPosition(j*18,i*18);
		 s.move(28,31); //offset
		 window.draw(s);
	   }

	for (int i=0;i<4;i++)
	  {
		s.setTextureRect(IntRect(colorNum*18,0,18,18));
		s.setPosition(a[i].x*18,a[i].y*18);
		s.move(28,31); //offset
		window.draw(s);
	  }

	window.draw(frame);
    window.draw(text);
 	window.display();
	}

    return 0;
}