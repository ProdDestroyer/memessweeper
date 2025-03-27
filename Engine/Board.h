#pragma once
#include "Graphics.h"
#include "Vec2D.h"
#include "SpriteCodex.h"
class Board
{
public:
	Board(const float width, const float height, const int mines);
	void draw(Graphics& gfx) const;
	void reveal(const int x, const int y);
	void flag(const int x, const int y);
	class Tile {

	public:
		enum class Status {
			REVEALED, HIDDEN, FLAGGED
		};
		void mine();
		bool isMined() const;
		Tile::Status getStatus() const;
		void draw(const float x, const float y, Graphics& gfx) const;
		void reveal();
		void flag();
		void setNeighborMinesAmount(const int amount);
	private:
		Status status = Status::HIDDEN;
		int neighborMinesAmount;
		bool hasMine = false;
	};

private:
	Board::Tile& getSpecificTile(const int x, const int y);
	Vec2D screenTopLeftCorner;
	Vec2D tilesDimensions;
	Vec2D calculateCorner(const int width, const int height) const;
	void setAllMinesCount();
	void generateMines(const int x, const int y);
	void setMineCount(const Vec2D& coords);
	bool validateTileCoords(const Vec2D& coords) const;
	int getLinealCoord(const int x, const int y) const;
	bool isAdjacent(const int targetIndex, const int testedIndex) const;
	bool firstReveal = true;
	int nTiles;
	int maxTileIndex;
	int minesAmount;
	Tile tiles[3000];
};

