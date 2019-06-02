//
// Tiles.h - This header includes definitions for scrolling tiled images
//

    #define NUM_TILES     17      // number of tile files
    #define TILE_WIDTH    16      // width in pixels of a tile
    #define TILE_HEIGHT   16      // height in pixels of a tile
    #define TILE_COLS     40      // width of tile map
    #define TILE_ROWS      6      // height of tile map
    #define TILES_TOTAL   (TILE_COLS*TILE_ROWS)
    #define TILES_PER_ROW (VIEW_WIDTH/TILE_WIDTH)

    #define SHIFT 4

#ifdef __cplusplus
extern "C" {
#endif

    void ReadTiles(void);
    void FreeTiles(void);
    void ReadTileMap(char *);
    void DrawTile(char *,int,int,int,int);
    void DrawTiles(int,int);

#ifdef __cplusplus
  }
#endif
