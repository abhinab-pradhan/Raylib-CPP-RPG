#include <raylib.h>
#include <raymath.h>
#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define MAX_TEXTURES 1
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20
const int screenWidth=800;
const int screenHeight=600;

typedef struct{
    int x;
    int y;
}sTile;

typedef struct 
{
    int x;
    int y;
} sEntity;
sEntity player;
sTile world[WORLD_WIDTH][WORLD_HEIGHT];
Camera2D camera={0};
typedef enum{
    TEXTURE_TILEMAP=0
} texture_asset;
Texture2D textures[MAX_TEXTURES];


void DrawTile(int pos_x,int pos_y,int texture_index_x,int texture_index_y){
    Rectangle source={(float)TILE_WIDTH * texture_index_x,(float)TILE_HEIGHT*texture_index_y,(float)TILE_WIDTH,(float)TILE_HEIGHT};
    Rectangle dest={(float)(pos_x),(float)(pos_y),(float)TILE_WIDTH,(float)TILE_HEIGHT};
    Vector2 origin={0,0};
    DrawTexturePro(textures[TEXTURE_TILEMAP],source,dest,origin,0.0f,WHITE);
}

void GameStartup(){
    InitAudioDevice();

    Image image=LoadImage("assets/colored_tilemap_packed.png");
    textures[TEXTURE_TILEMAP]=LoadTextureFromImage(image);
    UnloadImage(image);

    for(int i=0;i<WORLD_WIDTH;i++){
        for(int j=0;j<WORLD_HEIGHT;j++){
            world[i][j]=(sTile){
                .x=i,
                .y=j,
            };
        }
    }

    player=(sEntity){
        .x=TILE_WIDTH*3,
        .y=TILE_HEIGHT*3
    };

    camera.target=(Vector2){player.x,player.y};
    camera.offset=(Vector2){(float)screenWidth/2,(float)screenHeight/2};
    camera.rotation=0.0f;
    camera.zoom=3.0f;

}


void GameUpdate(){
    float x=player.x;
    float y=player.y;
    if(IsKeyPressed(KEY_LEFT)){
        x-=1*TILE_WIDTH;
    }
    else if(IsKeyPressed(KEY_RIGHT)){
        x+=1*TILE_WIDTH;
    }
    else if (IsKeyPressed(KEY_UP)){
        y-=1*TILE_HEIGHT;
    }
    else if(IsKeyPressed(KEY_DOWN)){
        y+=1*TILE_HEIGHT;
    }
    
    float wheel=GetMouseWheelMove();
    if(wheel!=0){
        const float zoomIncrement=0.125f;
        camera.zoom+=(wheel*zoomIncrement);
        if(camera.zoom<3.0f) camera.zoom=3.0f;
        if(camera.zoom >8.0f) camera.zoom=8.0f;
    }

    player.x=x;
    player.y=y;
    camera.target=(Vector2){player.x,player.y};
}


void GameRender(){
    BeginMode2D(camera);
    sTile tile;
    int texture_index_x=0;
    int texture_index_y=0;
    for(int i=0;i<WORLD_WIDTH;i++){
        for(int j=0;j<WORLD_HEIGHT;j++){
            tile=world[i][j];
            texture_index_x=4;
            texture_index_y=4;
            DrawTile(tile.x*TILE_WIDTH,tile.y*TILE_HEIGHT,texture_index_x,texture_index_y);
        }
    }

    DrawTile(camera.target.x,camera.target.y,4,0);
    EndMode2D();

    DrawRectangle(5,5,330,120,Fade(SKYBLUE,0.5f));
    DrawRectangleLines(5,5,330,120,BLUE);

    DrawText(TextFormat("Camera target: (%06.2f,%06.2f)",camera.target.x,camera.target.y),15,10,14,YELLOW);
    DrawText(TextFormat("Camera Zoom: %06.2f",camera.zoom),15,30,14,YELLOW);
}


void GameShutdown(){
    for(int i=0;i<MAX_TEXTURES;i++){
        UnloadTexture(textures[i]);
    }
    CloseAudioDevice();
}



int main() 
{
    InitWindow(screenWidth,screenHeight,"2D Rpg");
    SetTargetFPS(60);

    GameStartup();
    while(!WindowShouldClose()){
        GameUpdate();
        BeginDrawing();
        ClearBackground(BLACK);
        GameRender();
        EndDrawing();
    }
    GameShutdown();
}
