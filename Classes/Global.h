#pragma once
#include <cocos2d.h>
#include <string>

constexpr const char* EVENT_GRID_READY       = "event.custom.grid_is_ready";
constexpr const char* EVENT_GRID_HAS_MATCHES = "event.custom.grid_has_matches";

static const int MAX_MATCHES    = 3; // Number of matches to consider a succesfull match
static const int SWIPE_TRESHOLD = 5; // Minimum pixels for swipe move 

const cocos2d::Size gBlockSize = cocos2d::Size(80, 80);
//--------------------------------------------------------------------------------------------------------
enum class BlockType : int
{
	NONE = -1,
	MILK = 0,
	APPLE,
	ORANGE,
	TOAST,
	BROCCOLI,
	COCO,
	STAR
};
//--------------------------------------------------------------------------------------------------------
const std::map<BlockType, std::string> gBlockTypeToFrameName
{
	{BlockType::MILK,     "characters_0001.png"},
	{BlockType::APPLE,    "characters_0002.png"},
	{BlockType::ORANGE,   "characters_0003.png"},
	{BlockType::TOAST,    "characters_0004.png"},
	{BlockType::BROCCOLI, "characters_0005.png"},
	{BlockType::COCO,     "characters_0006.png"},
	{BlockType::STAR,     "characters_0007.png"}
};
//--------------------------------------------------------------------------------------------------------
struct TLevelConfig
{	// example
	///	"ID"      : 0,
	///	"LOCK"    : false,
	///	"NAME"    : "LEVEL0",
	///	"SCORE"   : 12,
	///  "RECORD"  : 0,
	///	"MOVES"   : 10,
	///	"ROWS"    : 8,
	///	"COLS"    : 5,
	///	"SUCCESS" : false
	///  "MATRIX"  : [31,31,31,31,31] => 31 => 0x11111b
	uint32_t    id;
	bool        lock;
	std::string name;
	uint32_t    score;
	uint32_t    record;
	uint32_t    moves;
	uint32_t    rows;
	uint32_t    cols;
	bool        success;
	std::vector<uint16_t> matrix; // binary matrix
};
//--------------------------------------------------------------------------------------------------------
struct GridPosition
{
	int row;
	int col;
};
//--------------------------------------------------------------------------------------------------------
struct GridMove
{
	GridPosition first;
	GridPosition second;
};
//--------------------------------------------------------------------------------------------------------
struct EventMatchesData 
{
	int matches;
};
//--------------------------------------------------------------------------------------------------------

