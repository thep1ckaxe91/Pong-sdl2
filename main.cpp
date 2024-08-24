#include "engine.hpp"
using Event = sdlgame::event::Event;
using Rect = sdlgame::rect::Rect;
using Vector2 = sdlgame::math::Vector2;
using Surface = sdlgame::surface::Surface;
using Color = sdlgame::color::Color;
using Sound = sdlgame::mixer::Sound;
using Channel = sdlgame::mixer::Channel;
using Font = sdlgame::font::Font;
using namespace std;
//If global declare is bad, i make MY OWN global declare :)
class Game
{
public:
    bool gameactive = true;
    Vector2 desktop_size = sdlgame::display::get_desktop_size();
    const int WIDTH = desktop_size.x, HEIGHT = desktop_size.y;
    string base_path = sdlgame::get_base_path();
    Surface window = sdlgame::display::set_mode(
        desktop_size.x,desktop_size.y,
        sdlgame::RESIZABLE | sdlgame::MAXIMIZED
    );
    bool gamestart = 0;
    sdlgame::time::Clock clock;
    Rect ball = Rect(0,0,10,10); 
    Vector2 ball_vel = Vector2();
    double ball_speed = 1000;
    const double pad_speed = 700;
    const double pad_height = 200;
    Rect pad1 = Rect(0.0,HEIGHT/2.0-25,20.0,pad_height);
    Rect pad2 = Rect(WIDTH-20.0,HEIGHT/2.0-25,20.0,pad_height);
    const Uint32 GAMEOVER = sdlgame::USEREVENT + 1;
    Game(){
        ball.setMidLeft(pad1.getMidRight());
    }

    void draw()
    {
        window.fill(Color("black"));
        sdlgame::draw::circle(window,Color("white"),ball.getCenterX(),ball.getCenterY(),ball.getWidth()/2);
        sdlgame::draw::rect(window,Color("white"),pad1);
        sdlgame::draw::rect(window,Color("white"),pad2);
    }
    void update()
    {
        if(!gamestart) ball.setMidLeft(pad1.getMidRight());
        ball.move_ip(ball_vel * clock.delta_time());
        if(ball.colliderect(pad1)){
            ball.setLeft(pad1.getRight());
            ball_vel = Vector2(ball_speed,0.0).rotate(
                (pad1.getCenterY()-ball.getCenterY())*-2/pad_height*45
            );
            ball_speed+=50;
        }
        if(ball.colliderect(pad2)){
            ball.setRight(pad2.getLeft());
            ball_vel = Vector2(-ball_speed,0.0).rotate(
                (ball.getCenterY()-pad2.getCenterY())*-2/pad_height*45
            );
            ball_speed+=50;
        }
        if(ball.getTop()<0){
            ball.setTop(0); ball_vel.y = -ball_vel.y;
        }
        if(ball.getBottom()>HEIGHT){
            ball.setBottom(HEIGHT); ball_vel.y = -ball_vel.y;
        }
        auto keys = sdlgame::key::get_pressed();
        pad1.move_ip(0.0,(keys[sdlgame::K_s]-keys[sdlgame::K_w])*pad_speed*clock.delta_time());
        pad2.move_ip(0.0,(keys[sdlgame::K_DOWN]-keys[sdlgame::K_UP])*pad_speed*clock.delta_time());

        if(ball.getRight()>WIDTH or ball.getLeft()<0){
            sdlgame::event::post(GAMEOVER);
        }
    }
    void run(){
        while(true)
        {
            auto events = sdlgame::event::get();
            for(auto &event : events)
            {
                if(event.type == sdlgame::QUIT)
                {
                    sdlgame::quit();
                    exit(0);
                }
                else if(event.type == sdlgame::WINDOWENTER){
                    if(event["event"] == sdlgame::WINDOWFOCUSLOST) gameactive = false;
                    else if(event["event"] == sdlgame::WINDOWFOCUSGAINED) gameactive = true;
                }
                else if(event.type == sdlgame::KEYDOWN and event["key"] == sdlgame::K_SPACE){
                    if(!gamestart){
                        ball_vel.x = ball_speed;
                        gamestart = 1;
                    }
                }
                else if(event.type == GAMEOVER){
                    pad1.setTop(HEIGHT/2-25);
                    pad2.setTop(HEIGHT/2-25);
                    ball.setMidLeft(pad1.getMidRight());
                    gamestart = 0;
                    ball_speed = 1000;
                }
            }
            if(gameactive)
            {
                update();
                draw();
                sdlgame::display::flip();
            }
            clock.tick();
        }
    }
};
int main(int argc, char** argv)
{
    sdlgame::init();
    sdlgame::font::init();
    sdlgame::image::init();
    sdlgame::mixer::init();
    Game game;
    game.run();
    return 0;
}