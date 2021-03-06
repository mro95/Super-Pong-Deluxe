#include <iostream>
#include <unistd.h>

#include "main.h"
#include "circle.h"

#define MAX_DT 0.025

Main::Main( )
{
    running = true;
    window = NULL;
    renderer = NULL;
}

int Main::execute( )
{
    if( !initialize() )
        return -1;

    SDL_Event sdlEvent;

    double lastFrame = SDL_GetTicks() / 1000.0;
    double now;
    double dt;
    double game_now = lastFrame;

    while ( running )
    {
        while ( SDL_PollEvent( &sdlEvent ))
            handleEvent( &sdlEvent );

        now = SDL_GetTicks() / 1000.0;
        dt = now - lastFrame;
        double game_dt = fmin(dt, MAX_DT);
        int reps = 0;
//        while(game_now < now) {
//            if( reps > 10)
//                running = false;
//            update( game_dt );
//            game_now += game_dt;
//            reps++;
//        }

        update( dt );
        //printf("now=%8.4f  game_now=%8.4f  dt=%6.4f  game_dt=%6.4f  reps=%d\n", now, game_now, dt, game_dt, reps);
        render( );

        SDL_Delay( 1 );
        lastFrame = now;
    }

    return 0;
}

bool Main::initialize( )
{
    if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
        return false;

    window = SDL_CreateWindow(
                "Pong",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                windowWidth, windowHeight,
                0 );

    if ( window == NULL )
        return false;



    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if ( renderer == NULL )
        return false;


    circle = SDL_CreateRGBSurface(0, 100, 100, 32, 0,0,0,0);
    if (circle == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetSurfaceBlendMode(circle, SDL_BLENDMODE_ADD);

    SDL_LockSurface(circle);
    Circle* c = new Circle();
    c->setCircle(circle);
    c->createFilledCircle(50,50,50);
    SDL_UnlockSurface(circle);

    circleTexture = SDL_CreateTextureFromSurface(renderer, circle);
    SDL_SetTextureAlphaMod(circleTexture, 255);
    //SDL_SetTextureBlendMode(circleTexture, SDL_BLENDMODE_ADD);

    if (circleTexture == NULL) {
        fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    return true;
}

void Main::render( )
{
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );


    SDL_RenderClear( renderer );

    stage.render( renderer );


    SDL_Rect r = { 10, 10, 100, 100};
    SDL_RenderCopy(renderer, circleTexture, NULL, &r);
    //SDL_SetTextureAlphaMod(circleTexture, 100);
    SDL_UpdateWindowSurface(window);

    SDL_RenderPresent( renderer );
}

void Main::update( double dt )
{
    stage.update( dt );
}

void Main::handleEvent( SDL_Event* event )
{
    if ( event->type == SDL_QUIT )
        running = false;

    if ( event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE )
    {
        running = false;
        return;
    }

    if( event->type == SDL_MOUSEMOTION)
    {
        stage.onMouseMotion(&event->motion);
    }

}

int main(int argc, char* args[])
{
    Main game;
    game.execute();
}
