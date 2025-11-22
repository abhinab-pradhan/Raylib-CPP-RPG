#include <raylib.h>
#include <raymath.h>
#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define MAX_TEXTURES 1
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20
const int screenWidth=800;
const int screenHeight=600;

typedef enum{
    TILE_TYPE_DIRT=0,
    TILE_TYPE_GRASS,
    TILE_TYPE_TREE,
    TILE_TYPE_STONE,
}tile_type;

typedef struct{
    int x;
    int y;
    int type;
}sTile;

typedef enum{
    ZONE_ALL=0,
    ZONE_WORLD,
    ZONE_DUNGEON
} eZone;

typedef struct 
{
    int x;
    int y;
    eZone zone;
    bool isPassable;
    bool isAlive;
    int health;
    int damage;
} sEntity;

sEntity player;
sEntity dungeon_gate;
sEntity orc;

sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sTile dungeon[WORLD_WIDTH][WORLD_HEIGHT];

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
                .type=GetRandomValue(TILE_TYPE_DIRT,TILE_TYPE_TREE)
            };

            dungeon[i][j]=(sTile){
                .x=i,
                .y=j,
                .type=TILE_TYPE_DIRT
            };
        }
    }

    //starting position of player
    player=(sEntity){
        .x=TILE_WIDTH*3,
        .y=TILE_HEIGHT*3,
        .zone=ZONE_WORLD,
        .isPassable=false,
        .isAlive=true,
        .health=100,
        .damage=0
    };

    //position of dungeon gate
    dungeon_gate=(sEntity){  
        .x=TILE_WIDTH*10,
        .y=TILE_HEIGHT*10,
        .zone=ZONE_ALL,
        .isPassable=true,
    };

    orc=(sEntity){
        .x=TILE_WIDTH *5,
        .y=TILE_HEIGHT *5,
        .zone=ZONE_DUNGEON,
        .isPassable=false,
        .isAlive=true,
        .health=100,
        .damage=0
    };

    camera.target=(Vector2){player.x,player.y};
    camera.offset=(Vector2){(float)screenWidth/2,(float)screenHeight/2};
    camera.rotation=0.0f;
    camera.zoom=3.0f;

}


void GameUpdate(){
    float x=player.x;
    float y=player.y;

    //player move control
    if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)){
        x-=1*TILE_WIDTH;
    }
    else if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)){
        x+=1*TILE_WIDTH;
    }
    else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        y-=1*TILE_HEIGHT;
    }
    else if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        y+=1*TILE_HEIGHT;
    }
    
    //for world zoom
    float wheel=GetMouseWheelMove();
    if(wheel!=0){
        const float zoomIncrement=0.125f;
        camera.zoom+=(wheel*zoomIncrement);
        if(camera.zoom<3.0f) camera.zoom=3.0f;
        if(camera.zoom >8.0f) camera.zoom=8.0f;
    }

    //orc collisions
    if(player.zone==orc.zone && orc.x==x && orc.y==y){
        int damage=GetRandomValue(2,20);
        orc.health-=damage;
        orc.damage=damage; //store damage

        if(orc.health<=0){
            orc.isAlive=false;
        }
    }
    else{
        player.x=x;
        player.y=y;
        camera.target=(Vector2){player.x,player.y};
    }


    if(IsKeyPressed(KEY_E)){
        if(player.x==dungeon_gate.x && player.y==dungeon_gate.y){
            //enter dungeon
            if(player.zone==ZONE_WORLD){
                player.zone=ZONE_DUNGEON;
            }
            else if(player.zone==ZONE_DUNGEON){
                player.zone=ZONE_WORLD;
            }
        }
    }
}


void GameRender(){
    BeginMode2D(camera);
    sTile tile;
    int texture_index_x=0;
    int texture_index_y=0;
    for(int i=0;i<WORLD_WIDTH;i++){
        for(int j=0;j<WORLD_HEIGHT;j++){
            if(player.zone==ZONE_WORLD){
                tile=world[i][j];
            }
            else if(player.zone==ZONE_DUNGEON){
                tile=dungeon[i][j];
            }

            switch (tile.type)
            {
            case TILE_TYPE_DIRT:
                texture_index_x=4;
                texture_index_y=4;
                break;
            case TILE_TYPE_GRASS:
                texture_index_x=5;
                texture_index_y=4;
                break;
            case TILE_TYPE_TREE:
                texture_index_x=5;
                texture_index_y=5;
                break;
            case TILE_TYPE_STONE:
                texture_index_x=1;
                texture_index_y=6;
                break;

            }

            DrawTile(tile.x*TILE_WIDTH,tile.y*TILE_HEIGHT,texture_index_x,texture_index_y);
        }
    }

    //render dungeon gate
    //draw gate
    DrawTile(dungeon_gate.x,dungeon_gate.y,8,9);

    //draw orc
    if(orc.zone==player.zone){
        if(orc.isAlive) DrawTile(orc.x,orc.y,11,0);
    }

    //render player
    DrawTile(camera.target.x,camera.target.y,4,0);
    EndMode2D();

    DrawRectangle(5,5,330,120,Fade(SKYBLUE,0.5f));
    DrawRectangleLines(5,5,330,120,BLUE);

    DrawText(TextFormat("Camera target: (%06.2f,%06.2f)",camera.target.x,camera.target.y),15,10,14,YELLOW);
    DrawText(TextFormat("Camera Zoom: %06.2f",camera.zoom),15,30,14,YELLOW);
    DrawText(TextFormat("Health: %d",player.health),15,50,14,YELLOW);
    if(orc.isAlive) DrawText(TextFormat("Orc Health: %d",orc.health),15,110,14,YELLOW);
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
