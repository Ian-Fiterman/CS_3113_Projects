#include "Map.h"

Map::Map(int mapColumns, int mapRows, unsigned int* levelData,
         const char* textureFilePath, float tileSize, int textureRows,
         int textureColumns, Vector2 origin) :
    mMapColumns {mapColumns}, mMapRows {mapRows},
    mTextureAtlas {LoadTexture(textureFilePath)}, mLevelData {levelData},
    mTileSize {tileSize}, mTextureColumns {textureColumns},
    mTextureRows {textureRows}, mOrigin {origin} {
    build();
}

Map::~Map() {
    UnloadTexture(mTextureAtlas);
}

void Map::build() {
    // Calculate map boundaries in world coordinates
    mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    // Precompute texture areas for each tile
    for (int row = 0; row < mTextureRows; row++) {
        for (int col = 0; col < mTextureColumns; col++) {
            Rectangle textureArea = {
                (float)col * (mTextureAtlas.width / mTextureColumns),
                (float)row * (mTextureAtlas.height / mTextureRows),
                (float)mTextureAtlas.width / mTextureColumns,
                (float)mTextureAtlas.height / mTextureRows};

            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render() {
    // Draw each tile in the map
    for (int row = 0; row < mMapRows; row++) {
        // Draw each column in the row
        for (int col = 0; col < mMapColumns; col++) {
            // Get the tile index at the current row and column
            int tile = mLevelData[row * mMapColumns + col];

            // If the tile index is 0, we do not draw anything
            if (tile == 0) continue;

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary + row * mTileSize, // y-axis is inverted
                mTileSize, mTileSize};

            // Draw the tile
            DrawTexturePro(
                mTextureAtlas,
                mTextureAreas[tile - 1], // -1 because tile indices start at 1
                destinationArea, {0.0f, 0.0f}, // origin
                0.0f,                          // rotation
                WHITE                          // tint
            );
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float* xOverlap, float* yOverlap) {
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    if (position.x < mLeftBoundary || position.x > mRightBoundary
        || position.y < mTopBoundary || position.y > mBottomBoundary)
        return false;

    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);

    if (tileXIndex < 0 || tileXIndex >= mMapColumns || tileYIndex < 0
        || tileYIndex >= mMapRows)
        return false;

    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];
    if (tile == 0) return false;

    Vector2 tileCenter = getTileCenter(tileYIndex, tileXIndex);

    /*
    When our collision probe touches a solid tile, we calculate how far the
    probe has penetrated into that tile along each axis — this is what we call
    the overlap. Each tile has a center point and a known half-size (half of
    mTileSize). By comparing the probe’s position to the tile’s center, we find
    how deep the probe is inside the tile: if the distance between them is
    smaller than the tile’s half-size, then the probe is overlapping. The
    overlap value (mTileSize / 2) - fabs(position - tileCenter) tells us exactly
    how much to push the entity back so that it sits flush against the tile’s
    edge without clipping inside. We calculate this separately for both the X
    and Y axes, which lets us resolve collisions in either direction — for
    example, preventing the player from sinking into the ground or walking
    through walls.
    */
    *xOverlap =
        fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.x - tileCenter.x));
    *yOverlap =
        fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.y - tileCenter.y));

    return true;
}

/**
 * @brief Gets the center position of a tile at the specified indices.
 * @param row
 * @param col
 * @return The center position of the tile
 */
Vector2 Map::getTileCenter(int row, int col) const {
    return {mLeftBoundary + col * mTileSize + mTileSize / 2.0f,
            mTopBoundary + row * mTileSize + mTileSize / 2.0f};
}

/**
 * @brief Gets the indices of the tile at the specified point.
 * @param point
 * @return The indices of the tile
 */
std::pair<int, int> Map::getTileIndices(Vector2 point) const {
    int col = (int)floor((point.x - mLeftBoundary) / mTileSize);
    int row = (int)floor((point.y - mTopBoundary) / mTileSize);
    return {row, col};
}

/**
 * @brief Checks if a point is inside a tile.
 * @param point
 * @param row
 * @param col
 * @return True if the point is inside the tile, false otherwise
 */
bool Map::isPointInTile(Vector2 point, int row, int col) const {
    Vector2 center = getTileCenter(row, col);
    float half = mTileSize / 2.0f;

    return fabs(point.x - center.x) < half && fabs(point.y - center.y) < half;
}

/**
 * @brief Gets the positions of all tiles that form the terrain surface.
 * @return A vector of positions representing the terrain surface tiles.
 */
std::vector<Vector2> Map::getTerrainSurface() const {
    std::vector<Vector2> surfaceTiles;

    for (int col = 0; col < mMapColumns; col++) {
        for (int row = 0; row < mMapRows; row++) {
            if (mLevelData[row * mMapColumns + col] != 0) {
                if (row > 0)
                    surfaceTiles.push_back(getTileCenter(row - 1, col));
                break;
            }
        }
    }
    return surfaceTiles;
}

/**
 * @brief Gets the altitude of a position above the terrain surface.
 * @param position
 * @return The altitude of the position above the terrain surface
 */
float Map::getAltitude(Vector2 position) const {
    auto tileIndices = getTileIndices(position);
    int row = tileIndices.first;
    int col = tileIndices.second;

    for (int r = row; r < mMapRows; r++) {
        if (mLevelData[r * mMapColumns + col] != 0) {
            Vector2 tileTop = getTileCenter(r, col);
            return (tileTop.y - mTileSize / 2.0f) - position.y;
        }
    }
    return mBottomBoundary - position.y; // no tile found, distance to bottom
}