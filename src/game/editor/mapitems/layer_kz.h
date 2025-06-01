#ifndef GAME_EDITOR_MAPITEMS_LAYER_KZGAME_H
#define GAME_EDITOR_MAPITEMS_LAYER_KZGAME_H

#include "layer_tiles.h"

struct SKZTileStateChange
{
	bool m_Changed;
	struct SData
	{
		unsigned char m_KZIndex;
        unsigned char m_Flags;
        unsigned char m_Number;
        int64_t m_Value1;
        int64_t m_Value2;
        int64_t m_Value3;
        int m_Index;
	} m_Previous, m_Current;
};

class CLayerKZGame : public CLayerTiles
{
public:
	CLayerKZGame(CEditor *pEditor, int w, int h);
	CLayerKZGame(const CLayerKZGame &Other);
	~CLayerKZGame();

	CKZTile *m_pKZTile;
    unsigned char m_Number;
	int64_t m_Value1;
	int64_t m_Value2;
	int64_t m_Value3;

	void Resize(int NewW, int NewH) override;
	void Shift(int Direction) override;
	bool IsEmpty(const std::shared_ptr<CLayerTiles> &pLayer) override;
	void BrushDraw(std::shared_ptr<CLayer> pBrush, vec2 WorldPos) override;
	void BrushFlipX() override;
	void BrushFlipY() override;
	void BrushRotate(float Amount) override;
	void FillSelection(bool Empty, std::shared_ptr<CLayer> pBrush, CUIRect Rect) override;

	EditorTileStateChangeHistory<SKZTileStateChange> m_History;
	void ClearHistory() override
	{
		CLayerTiles::ClearHistory();
		m_History.clear();
	}

	std::shared_ptr<CLayer> Duplicate() const override;
	const char *TypeName() const override;

protected:
	void RecordStateChange(int x, int y, SKZTileStateChange::SData Previous, SKZTileStateChange::SData Current);
};

class CLayerKZFront : public CLayerKZGame
{
public:
    CLayerKZFront(CEditor *pEditor, int w, int h);
    CLayerKZFront(const CLayerKZFront &Other);
    ~CLayerKZFront() = default;

    void BrushDraw(std::shared_ptr<CLayer> pBrush, vec2 WorldPos) override;
	void FillSelection(bool Empty, std::shared_ptr<CLayer> pBrush, CUIRect Rect) override;
};

#endif