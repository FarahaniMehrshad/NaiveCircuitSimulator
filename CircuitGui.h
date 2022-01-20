#ifndef MF_CIRCUIT_GUI_DEF
#define MF_CIRCUIT_GUI_DEF

#include "olcPixelGameEngine.h"
#include "mfLinkedList.h"
#include "CircuitCore.h"
#include <iostream>
#include <string>

class Dot
{
public:
	Dot();
	Dot(std::string name, int x, int y);

	std::string getName() const;
	int getX() const;
	int getY() const;

	bool operator ==(const Dot& dot);

private:
	std::string _name = "";
	int _x = -1;
	int _y = -1;

};

class Item
{
public:
	Item();
	Item(int type, Dot* fDot, Dot* sDot);

	Dot* _fDot = nullptr;
	Dot* _sDot = nullptr;
	int _type = 0;
	std::string _name;
	std::string _voltage;
	std::string _current;
	std::string _resistance;

	bool operator ==(const Item& item);
};

class CircuitGui : public olc::PixelGameEngine
{
public:
	enum Coordination
	{
		HORIZONTAL,
		VERTICAL,
		CO_RIGHT,
		CO_LEFT,
		CO_DOWN,
		CO_UP,
	};

	enum ItemsType
	{
		NONE,
		BLANK,
		WIRE,
		RESISTOR,
		VOLTAGE,
	};

public:
	CircuitGui();
	~CircuitGui();

private:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fTimeElaplsed) override;
	void checkKeyEvent();
	void checkMouseEvent();
	void selectWire();
	void selectResistor();
	void selectVoltageSource();
	void selectBlank();

private:
	void drawTitleBar();
	void drawToolBar();
	void drawField();
	void drawBorders();
	void drawItemEditDialog(Item* item);
	void drawAllItems();
	void drawItem(int type, Dot* firstDot, Dot* secondDot, std::string detail, olc::Pixel color = olc::WHITE);
	void drawItemInfo(Item& item);
	void drawWire(int x1, int y1, int coord, std::string detail = "", olc::Pixel color = olc::BLACK, int thickness = 1);
	void drawResistor(int x1, int y1, int coord, std::string detail = "", olc::Pixel color = olc::BLACK, int thickness = 1);
	void drawVoltage(int x1, int y1, int coord, std::string detail = "", olc::Pixel color = olc::BLACK, int thickness = 1);

private:
	void createAndSolveCircuit();
	void addItem(int type, Dot* firstDot, Dot* secondDot, olc::Pixel color = olc::BLACK);
	void updateItem(Item* item);
	Dot* getNearDot(int x, int y);
	int getCoord(Dot* dot1, Dot* dot2) const;
	double getDistance(Dot* dot1, Dot* dot2) const;
	double getDistance(int x1, int y1, int x2, int y2) const;
	bool hasMouseOverlap(Item& item) const;
	double roundDecimal(double number) const;
	std::string removeNumberTrailer(std::string number) const;

private:
	const int _screenWidth = 900;
	const int _screenHeight = 600;

	const int _titleBarWidth = 900;
	const int _titleBarHeight = 30;

	const int _toolBarOffsetX = 30;
	const int _toolBarOffsetY = 40;
	const int _toolBarWidth = 110;
	const int _toolBarHeight = 460;
	const int _dotDistance = 40;

	olc::Pixel _pink;
	olc::Pixel _brown;
	olc::Pixel _darkBlue;
	olc::Pixel _neonBlue;
	olc::Pixel _blue;
	olc::Pixel _softGreen;
	olc::Pixel _softOrange;
	olc::Pixel _orange;
	olc::Pixel _green;
	olc::Pixel _purple;

	mf::LinkedList<Dot> _dots;
	mf::LinkedList<Item> _itemsList;

	CircuitCore* _circuitCore = nullptr;
	Item* _overlappedItem = nullptr;
	Dot* _firstDotSelected = nullptr;
	Dot* _secondDotSelected = nullptr;

	int _error = -1;
	int _selectedItem = NONE;
	bool _editDialogOpen = false;
	std::string _editDialogTextBoxString = "";
	bool _decimal = false;
};



#endif // MF_CIRCUIT_GUI_DEF


