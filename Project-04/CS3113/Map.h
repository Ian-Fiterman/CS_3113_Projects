#include "cs3113.h"

#ifndef MAP_H
#define MAP_H

class Map {
private:
    int mMapColumns;         // number of columns in map
    int mMapRows;            // number of rows in map

    int* mLevelData;         // array of tile indices
    Texture2D mTextureAtlas; // texture atlas

    float mTileSize;         // size of each tile in pixels
    int mTileSpacing;        // spacing between tiles in texture atlas

    int mTextureColumns;     // number of columns in texture atlas
    int mTextureRows;        // number of rows in texture atlas

    std::vector<Rectangle> mTextureAreas; // texture areas for each tile
    Vector2 mOrigin;       // center of the map in world coordinates

    float mLeftBoundary;   // left boundary of the map in world coordinates
    float mRightBoundary;  // right boundary of the map in world coordinates
    float mTopBoundary;    // top boundary of the map in world coordinates
    float mBottomBoundary; // bottom boundary of the map in world coordinates

    std::set<int> mNonSolidTiles;

public:
    Map(int mapColumns, int mapRows, int* levelData,
        const char* textureFilePath, float tileSize, int textureRows,
        int textureColumns, int tileSpacing, Vector2 origin);
    ~Map();

    void build();
    void render();
    bool isSolidTileAt(Vector2 position, float* xOverlap, float* yOverlap);

    int getMapColumns() const { return mMapColumns; };

    int getMapRows() const { return mMapRows; };

    float getTileSize() const { return mTileSize; };

    float getTileSpacing() const { return mTileSpacing; };

    int* getLevelData() const { return mLevelData; };

    Texture2D getTextureAtlas() const { return mTextureAtlas; };

    int getTextureColumns() const { return mTextureColumns; };

    int getTextureRows() const { return mTextureRows; };

    float getLeftBoundary() const { return mLeftBoundary; };

    float getRightBoundary() const { return mRightBoundary; };

    float getTopBoundary() const { return mTopBoundary; };

    float getBottomBoundary() const { return mBottomBoundary; };

    Vector2 getTileCenter(int row, int col) const;

    std::pair<int, int> getTileIndices(Vector2 point) const;

    bool isPointInTile(Vector2 point, int row, int col) const;

    void panCamera(Camera2D* camera, const Vector2* targetPosition,
                   float screenWidth, float screenHeight) const;

    void addNonSolidTile(int tileIndex) { mNonSolidTiles.insert(tileIndex); }

    const std::set<int>& getNonSolidTiles() const { return mNonSolidTiles; }
};

#endif // MAP_H