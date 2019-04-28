#ifndef MAP_H__
#define MAP_H__

#include <engine.h>

static uint8 s_cellSize = 16u;

typedef struct
{
	// Data
	uint8	m_width;
	uint8	m_depth;
	uint8*	m_cells;
}MapData;

// Get the number of cells that can be walked on
static uint16 Map_GetWalkableCellCount(MapData* const i_data)
{
	uint16 c = 0u;
	uint32 i = 0u;
	
	for (i = 0u; i < (i_data->m_width * i_data->m_depth); ++i)
	{
		if (i_data->m_cells[i] == 0u)
		{
			c++;
		}
	}
	return c;
}

// Get the cell index for specified cell x, z
static uint16 Map_CellToIndex(MapData* const i_data, uint8 i_locationX, uint8 i_locationZ)
{	
	const uint16 index = i_locationZ * i_data->m_width + i_locationX;
	VERIFY_ASSERT(index < (i_data->m_width * i_data->m_depth), "Map_CellToIndex: Index out bounds (cell x: %u, cell z: %u, map width: %u, map depth: %u", i_locationX, i_locationZ, i_data->m_width, i_data->m_depth);
	return index;
}

// Get the cell x, z for the specified world position
static void Map_PositionToCell(MapData* const i_data, VECTOR const* i_position, uint8* o_locationX, uint8* o_locationZ)
{
	*o_locationX = (uint8)( ((float)i_position->vx / (float)(s_cellSize * 2)) + 0.5f);
	*o_locationZ = (uint8)( ((float)i_position->vz / (float)(s_cellSize * 2)) + 0.5f);
}

// Get world cell position for specified cell location
static void Map_CellToPosition(MapData* const i_data, uint8 i_locationX, uint8 i_locationZ, VECTOR *o_position)
{
	o_position->vx = i_locationX * s_cellSize * 2;
	o_position->vy = 0;
	o_position->vz = i_locationZ * s_cellSize * 2;
}

// Check if the cell is walkable (can be walked)
static bool Map_IsCellWalkable(MapData* const i_data, uint8 i_locationX, uint8 i_locationZ)
{
	const uint16 index = Map_CellToIndex(i_data, i_locationX, i_locationZ);
	return i_data->m_cells[index] == 0;
}

#endif // MAP_H__