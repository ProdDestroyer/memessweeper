#include "Board.h"
#include "MyUtilities.h"
#include "assert.h"

//Board
Board::Board(const float width, const float height, const int mines) :
	screenTopLeftCorner(calculateCorner((int)width, (int)height)),
	tilesDimensions(width, height)
{
	assert(mines < width * height);
	nTiles = int(width * height);
	minesAmount = mines;
	maxTileIndex = nTiles - 1;
}

void Board::draw(Graphics& gfx) const
{
	gfx.DrawRect((int)screenTopLeftCorner.x, (int)screenTopLeftCorner.y, int(tilesDimensions.x * SpriteCodex::tileSize), int(tilesDimensions.y * SpriteCodex::tileSize), { 192, 192, 192 });
	for (int j = 0; j < tilesDimensions.y; j++) {
		for (int i = 0; i < tilesDimensions.x; i++) {
			tiles[j * (int)tilesDimensions.x + i].draw(i * SpriteCodex::tileSize + screenTopLeftCorner.x, j * SpriteCodex::tileSize + screenTopLeftCorner.y, gfx);
		}
	}
}

void Board::generateMines(const int x, const int y)
{
	int minesToPut = minesAmount;
	int mineIndex = -1;
	while (minesToPut > 0) {
		do {
			mineIndex = MyUtilities::randomBetween(0, nTiles);
		} while (tiles[mineIndex].isMined() || tiles[mineIndex].getStatus() == Tile::Status::REVEALED || isAdjacent(getLinealCoord(x,y), mineIndex));
		tiles[mineIndex].mine();
		minesToPut -= 1;
	}
}

Board::Tile& Board::getSpecificTile(const int x, const int y)
{
	return tiles[int(tilesDimensions.x) * y + x];
}

Vec2D Board::calculateCorner(const int width, const int height) const
{
	int screenBoardWidth = SpriteCodex::tileSize * width;
	int screenBoardHeight = SpriteCodex::tileSize * height;

	return Vec2D((Graphics::ScreenWidth - screenBoardWidth) / 2.0f, (Graphics::ScreenHeight - screenBoardHeight) / 2.0f);
}

void Board::setAllMinesCount()
{
	for (int i = 0; i < nTiles; i++) {
		Vec2D tileCoord = { float(i % (int)tilesDimensions.x), i / tilesDimensions.x };
		setMineCount(tileCoord);
	}
}

void Board::setMineCount(const Vec2D& coords)
{
	const int x = (int)coords.x;
	const int y = (int)coords.y;
	const int linealCoord = getLinealCoord(x, y);
	if (!tiles[linealCoord].isMined()) {
		int minesCounter = 0;
		for (int j = y - 1; j < y + 2; j++) {
			for (int i = x - 1; i < x + 2; i++) {
				minesCounter += (validateTileCoords(Vec2D(float(i), float(j))) && tiles[getLinealCoord(i, j)].isMined()) ? 1 : 0;
			}
		}
		tiles[linealCoord].setNeighborMinesAmount(minesCounter);
	}
	else {
		tiles[linealCoord].setNeighborMinesAmount(-1);
	}
}

void Board::reveal(const int x, const int y)
{
	const int translatedX = int((x - screenTopLeftCorner.x) / SpriteCodex::tileSize);
	const int translatedY = int((y - screenTopLeftCorner.y) / SpriteCodex::tileSize);
	const int linealCoord = getLinealCoord(translatedX, translatedY);
	if (validateTileCoords(Vec2D(float(translatedX), float(translatedY))) && tiles[getLinealCoord(translatedX, translatedY)].getStatus() != Tile::Status::FLAGGED) {
		if (firstReveal) {
			generateMines(translatedX, translatedY);
			setAllMinesCount();
			firstReveal = false;
		}
		tiles[getLinealCoord(translatedX, translatedY)].reveal();
	}

}

void Board::flag(const int x, const int y)
{
	const int translatedX = int((x - screenTopLeftCorner.x) / SpriteCodex::tileSize);
	const int translatedY = int((y - screenTopLeftCorner.y) / SpriteCodex::tileSize);
	const int linearCoord = getLinealCoord(x, y);
	tiles[getLinealCoord(translatedX, translatedY)].flag();
}

bool Board::validateTileCoords(const Vec2D& coords) const
{
	return int(coords.x) > -1 && int(coords.x) < tilesDimensions.x && int(coords.y) > -1 && int(coords.y) < tilesDimensions.y;
}

int Board::getLinealCoord(const int x, const int y) const
{
	return int(tilesDimensions.x * y + x);
}

bool Board::isAdjacent(const int targetIndex, const int testedIndex) const
{
	Vec2D targetCoord = { float(targetIndex % (int)tilesDimensions.x), targetIndex / tilesDimensions.x };
	Vec2D testedCoord = { float(testedIndex % (int)tilesDimensions.x), testedIndex / tilesDimensions.x };

	const int y = int(targetCoord.y);
	const int x = int(targetCoord.x);

	const int testedX = int(testedCoord.x);
	const int testedY = int(testedCoord.y);

	bool isAdjacent = true;

	for (int j = y - 1; j < y + 2; j++) {
		for (int i = x - 1; i < x + 2; i++) {
			isAdjacent = isAdjacent && !(i == testedX && j == testedY);
		}
	}
	return !isAdjacent;
}

//Tile
void Board::Tile::mine()
{
	hasMine = true;
}

bool Board::Tile::isMined() const
{
	return hasMine;
}

Board::Tile::Status Board::Tile::getStatus() const
{
	return status;
}

void Board::Tile::draw(const float x, const float y, Graphics& gfx) const
{
	switch (status) {
	case Status::HIDDEN:
		SpriteCodex::DrawTileButton({ x,y }, gfx);
		break;
	case Status::REVEALED: {
		switch (neighborMinesAmount) {
		case -1:
			SpriteCodex::DrawTile0({ x,y }, gfx);
			SpriteCodex::DrawTileBomb({ x,y }, gfx);
			break;
		case 0:
			SpriteCodex::DrawTile0({ x,y }, gfx);
			break;
		case 1:
			SpriteCodex::DrawTile1({ x,y }, gfx);
			break;
		case 2:
			SpriteCodex::DrawTile2({ x,y }, gfx);
			break;
		case 3:
			SpriteCodex::DrawTile3({ x,y }, gfx);
			break;
		case 4:
			SpriteCodex::DrawTile4({ x,y }, gfx);
			break;
		case 5:
			SpriteCodex::DrawTile5({ x,y }, gfx);
			break;
		case 6:
			SpriteCodex::DrawTile6({ x,y }, gfx);
			break;
		case 7:
			SpriteCodex::DrawTile7({ x,y }, gfx);
			break;
		case 8:
			SpriteCodex::DrawTile8({ x,y }, gfx);
			break;
		}
		break;
	}
	case Status::FLAGGED:
		SpriteCodex::DrawTileButton({ x,y }, gfx);
		SpriteCodex::DrawTileFlag({ x,y }, gfx);
		break;
	}
}

void Board::Tile::reveal()
{
	status = Status::REVEALED;
}

void Board::Tile::flag()
{
	status = (status != Status::REVEALED) ? (status == Status::FLAGGED) ? Status::HIDDEN : Status::FLAGGED : status;
}

void Board::Tile::setNeighborMinesAmount(const int amount)
{
	neighborMinesAmount = amount;
}
