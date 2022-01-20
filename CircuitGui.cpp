#define OLC_PGE_APPLICATION
#include "CircuitGui.h"
#include <math.h>

class Dot;
class Item;
class CircuitGui;

/*
================= Public realization of class Dot =================
*/

// Dots are invisible on the screen by default

Dot::Dot() {}

Dot::Dot(std::string name, int x, int y) : _name(name), _x(x), _y(y) {}

std::string Dot::getName() const { return _name; }

int Dot::getX() const { return _x; }

int Dot::getY() const { return _y; }

bool Dot::operator ==(const Dot& dot)
{
	if (_name == dot._name)
		return true;
	return false;
}

/*
================= Public realization of class Item =================
*/

Item::Item(){ }

Item::Item(int type, Dot* fDot, Dot* sDot)
{
	_type = type;
	_fDot = fDot;
	_sDot = sDot;
	_voltage = "0";
	_current = "0";
	_resistance = "0";

	switch (_type)
	{
	case CircuitGui::ItemsType::WIRE:
		_name = "Wire" + fDot->getName() + sDot->getName();
		break;
	case CircuitGui::ItemsType::RESISTOR:
		_name = "Resistor" + fDot->getName() + sDot->getName();
		break;
	case CircuitGui::ItemsType::VOLTAGE:
		_name = "Battery" + fDot->getName() + sDot->getName();
		break;
	};
}

bool Item::operator==(const Item& item)
{
	if (_fDot == item._fDot && _sDot == item._sDot)
		return true;
	if (_fDot == item._sDot && _sDot == item._fDot)
		return true;

	return false;
}

/*
================= Public realization of class CircuitGui =================
*/

CircuitGui::CircuitGui()
{
	_circuitCore = new CircuitCore();

	if (!Construct(_screenWidth, _screenHeight, 1, 1, false, true))
		throw "Failed to construct the window";

	sAppName = "Naive circuit simulator - Made with olc pixel game engine - By Mehrshad Jafari Farahani";
}

CircuitGui::~CircuitGui()
{ 
	delete _circuitCore;
}

/*
================= Private realization of class CircuitGui =================
*/

/*
--------------- Event handling ---------------
*/
bool CircuitGui::OnUserCreate()
{
	_darkBlue = olc::Pixel(3, 20, 22);
	_neonBlue = olc::Pixel(68, 214, 224);
	_pink = olc::Pixel(255, 174, 201);
	_brown = olc::Pixel(200, 124, 55);
	_blue = olc::Pixel(9, 56, 62);
	_softGreen = olc::Pixel(200, 240, 200);
	_softOrange = olc::Pixel(181, 156, 127);
	_orange = olc::Pixel(255, 117, 26);
	_green = olc::Pixel(25, 240, 50);
	_purple = olc::Pixel(255, 36, 255);

	int startX = 20;
	int startY = 60;
	for (int i = 0; i < 14; ++i)
	{
		for (int j = 0; j < 19; ++j)
		{
			std::string name = "Dot";
			name += std::to_string(i) + "-" + std::to_string(j);
			_dots.pushBack(Dot(name, startX + _dotDistance * j, startY + _dotDistance * i));
		}
	}

	Clear(_darkBlue);
	drawMenu();
	drawToolBar();
	drawBorders();
	drawField();

	return true;
}

bool CircuitGui::OnUserUpdate(float fTimeElapsed)
{
	if (IsFocused()){
		checkKeyEvent();
		checkMouseEvent();
	}

	return true;
}

void CircuitGui::checkKeyEvent()
{
	if (_editDialogOpen){
		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			_editDialogTextBoxString = "";
			_overlappedItem = nullptr;
			_editDialogOpen = false;
			_decimal = false;

			drawField();
			return;
		}

		if (GetKey(olc::Key::ENTER).bPressed)
		{
			updateItem(_overlappedItem);
			return;
		}

		std::string toAdd = "";

		if (GetKey(olc::Key::K0).bPressed || GetKey(olc::Key::NP0).bPressed) toAdd = "0";
		if (GetKey(olc::Key::K1).bPressed || GetKey(olc::Key::NP1).bPressed) toAdd = "1";
		if (GetKey(olc::Key::K2).bPressed || GetKey(olc::Key::NP2).bPressed) toAdd = "2";
		if (GetKey(olc::Key::K3).bPressed || GetKey(olc::Key::NP3).bPressed) toAdd = "3";
		if (GetKey(olc::Key::K4).bPressed || GetKey(olc::Key::NP4).bPressed) toAdd = "4";
		if (GetKey(olc::Key::K5).bPressed || GetKey(olc::Key::NP5).bPressed) toAdd = "5";
		if (GetKey(olc::Key::K6).bPressed || GetKey(olc::Key::NP6).bPressed) toAdd = "6";
		if (GetKey(olc::Key::K7).bPressed || GetKey(olc::Key::NP7).bPressed) toAdd = "7";
		if (GetKey(olc::Key::K8).bPressed || GetKey(olc::Key::NP8).bPressed) toAdd = "8";
		if (GetKey(olc::Key::K9).bPressed || GetKey(olc::Key::NP9).bPressed) toAdd = "9";
		if (GetKey(olc::Key::PERIOD).bPressed || GetKey(olc::Key::NP_DECIMAL).bPressed)
		{
			if (!_decimal && _editDialogTextBoxString.length() > 0)
			{
				toAdd = ".";
				_decimal = true;
			}
		}
		if (GetKey(olc::Key::BACK).bPressed && _editDialogTextBoxString.length() > 0)
		{
			if (_editDialogTextBoxString[_editDialogTextBoxString.length() - 1] == '.')
				_decimal = false;
			if (_editDialogTextBoxString.length() > 0)
				_editDialogTextBoxString.resize(_editDialogTextBoxString.length() - 1);
			drawItemEditDialog(_overlappedItem);
		}

		if (toAdd != "")
		{
			_editDialogTextBoxString += toAdd;
			drawItemEditDialog(_overlappedItem);
		}

		return;
	}

	// Edit Item
	if (_overlappedItem)
	{
		if (GetKey(olc::Key::E).bPressed)
		{
			if (_overlappedItem->_type == RESISTOR)
				_editDialogTextBoxString = _overlappedItem->_resistance;
			if (_overlappedItem->_type == VOLTAGE)
				_editDialogTextBoxString = _overlappedItem->_voltage;

			drawItemEditDialog(_overlappedItem);
		}
	}

	if (GetKey(olc::Key::W).bPressed) selectWire();
	if (GetKey(olc::Key::R).bPressed) selectResistor();
	if (GetKey(olc::Key::V).bPressed) selectVoltageSource();
	if (GetKey(olc::Key::B).bPressed) selectBlank();
	if (GetKey(olc::Key::ESCAPE).bPressed){
		drawField();

		_firstDotSelected = nullptr;
		_secondDotSelected = nullptr;

		_selectedItem = NONE;

		drawToolBar();
	}
}

void CircuitGui::checkMouseEvent()
{
	if (_editDialogOpen){

		int okX = ScreenWidth() / 2 - 150 + 300 - 98;
		int okY = ScreenHeight() / 2 - 100 + 172;
		int cancelX = ScreenWidth() / 2 - 150 + 300 - 227;
		int cancelY = ScreenHeight() / 2 - 100 + 172;

		DrawString(okX, okY,"OK", olc::YELLOW);
		DrawString(cancelX, cancelY, "Cancel", olc::YELLOW);

		if (getDistance(GetMouseX(), GetMouseY(), okX + 10, okY) < 25)
		{
			DrawString(okX, okY, "OK", _neonBlue);
			if (GetMouse(0).bPressed)
			{
				updateItem(_overlappedItem);
			}
		}

		if (getDistance(GetMouseX(), GetMouseY(), cancelX + 10, cancelY) < 25)
		{
			DrawString(cancelX, cancelY, "Cancel", _neonBlue);
			if (GetMouse(0).bPressed)
			{
				_editDialogTextBoxString = "";
				_overlappedItem = nullptr;
				_editDialogOpen = false;
				_decimal = false;

				drawField();
				return;
			}
		}

		return;
	}

	if (GetMouse(0).bPressed)
	{
		if (getDistance(GetMouseX(), GetMouseY(), 812 + 20, 100) < 30)
			selectWire();
		if (getDistance(GetMouseX(), GetMouseY(), 812 + 20, 180) < 30)
			selectResistor();
		if (getDistance(GetMouseX(), GetMouseY(), 812 + 20, 250) < 30)
			selectVoltageSource();
		if (getDistance(GetMouseX(), GetMouseY(), 812 + 20, 320) < 30)
			selectBlank();
	}

	// Pick two nearest dot on the screen
	if (_selectedItem != NONE)
	{
		if (GetMouse(0).bHeld)
		{
			Dot* dot = getNearDot(GetMouseX(), GetMouseY());

			if (dot != _secondDotSelected)
				_firstDotSelected = _secondDotSelected;
			_secondDotSelected = dot;

			if (_firstDotSelected != nullptr && _secondDotSelected != nullptr)
			{
				if (getDistance(_firstDotSelected->getX(), _firstDotSelected->getY(), _secondDotSelected->getX(), _secondDotSelected->getY()) < _dotDistance + 4)
				{
					drawField();
					addItem(_selectedItem, _firstDotSelected, _secondDotSelected, olc::BLACK);
				}
			}
		}
	}
	else
	{
		_firstDotSelected = nullptr;
		_secondDotSelected = nullptr;
	}

	drawAllItems();
}

void CircuitGui::selectWire()
{
	drawField();

	_firstDotSelected = nullptr;
	_secondDotSelected = nullptr;

	_selectedItem = WIRE;

	drawToolBar();
	drawWire(812, 100, CO_RIGHT, "", olc::YELLOW, 3);
	DrawString(825, 110, "W", olc::YELLOW, 2);
}

void CircuitGui::selectResistor()
{
	drawField();

	_firstDotSelected = nullptr;
	_secondDotSelected = nullptr;

	_selectedItem = RESISTOR;

	drawToolBar();
	drawResistor(812, 180, CO_RIGHT, "", olc::YELLOW, 3);
	DrawString(825, 190, "R", olc::YELLOW, 2);
}

void CircuitGui::selectVoltageSource()
{
	drawField();

	_firstDotSelected = nullptr;
	_secondDotSelected = nullptr;

	_selectedItem = VOLTAGE;

	drawToolBar();
	drawVoltage(812, 250, CO_RIGHT, "", olc::YELLOW, 2);
	DrawString(825, 270, "V", olc::YELLOW, 2);
}

void CircuitGui::selectBlank()
{
	drawField();

	_firstDotSelected = nullptr;
	_secondDotSelected = nullptr;

	_selectedItem = BLANK;

	drawToolBar();
	FillCircle(812, 320, 2, olc::YELLOW);
	FillCircle(852, 320, 2, olc::YELLOW);
	DrawString(826, 337, "B", olc::YELLOW, 2);
}

/*
--------------- Logical functions ---------------
*/
void CircuitGui::createAndSolveCircuit()
{
	if (_circuitCore != nullptr) delete _circuitCore;

	_circuitCore = new CircuitCore();

	for (Item& item : _itemsList)
	{
		try
		{
			switch (item._type)
			{
			case WIRE:
				_circuitCore->addWire(item._name, item._fDot->getName(), item._sDot->getName());
				break;
			case RESISTOR:
				_circuitCore->addResistor(item._name, std::stod(item._resistance), item._fDot->getName(), item._sDot->getName());
				break;
			case VOLTAGE:
				_circuitCore->addBattery(item._name, std::stod(item._voltage), item._fDot->getName(), item._sDot->getName());
				break;
			}
		}
		catch (CircuitCore::Errors error)
		{
			_error = error;
		}
	}

	try
	{
		_circuitCore->solve();
	}
	catch (CircuitCore::Errors error)
	{
		delete _circuitCore;
		_circuitCore = nullptr;
		_error = error;
	}
}

void CircuitGui::addItem(int type, Dot * firstDot, Dot * secondDot, olc::Pixel color)
{
	Item* foundItem = _itemsList.find(Item(type, firstDot, secondDot));

	if (!foundItem)
	{
		if (type != BLANK)
		{
			Item item(type, firstDot, secondDot);

			if (type == VOLTAGE) item._voltage = "12";
			if (type == RESISTOR) item._resistance = "5";

			_itemsList.pushBack(item);

			createAndSolveCircuit();
		}
	}

	if (foundItem)
	{
		if (type == BLANK)
		{
			// Save it's name, because when we remove it from draw list,
			// it will be gone and we can't use it later for circuit
			std::string name = foundItem->_name;

			try 
			{
				_itemsList.remove(*foundItem);
				createAndSolveCircuit();
			}
			catch (CircuitCore::Errors error)
			{
				_error = error;
			}
		}
	}
}

void CircuitGui::updateItem(Item * item)
{
	if (_editDialogTextBoxString == "")
		return;
	if (_editDialogTextBoxString[_editDialogTextBoxString.length() - 1] == '.')
		return;
	if (std::stod(_editDialogTextBoxString) < 0.00001)
		return;

	bool changed = false;
	if (_overlappedItem->_type == RESISTOR)
	{
		if (_editDialogTextBoxString != _overlappedItem->_resistance)
		{
			_overlappedItem->_resistance = _editDialogTextBoxString;
			changed = true;
		}
	}

	if (_overlappedItem->_type == VOLTAGE)
	{
		if (_editDialogTextBoxString != _overlappedItem->_voltage)
		{
			_overlappedItem->_voltage = _editDialogTextBoxString;
			changed = true;
		}
	}

	if (changed) createAndSolveCircuit();

	_editDialogTextBoxString = "";
	_overlappedItem = nullptr;
	_editDialogOpen = false;
	_decimal = false;

	drawField();
}

double CircuitGui::getDistance(Dot * dot1, Dot * dot2) const
{
	return getDistance(dot1->getX(), dot1->getY(), dot2->getX(), dot2->getY());
}

double CircuitGui::getDistance(int x1, int y1, int x2, int y2) const
{
	double t1 = pow(x1 - x2, 2);
	double t2 = pow(y1 - y2, 2);

	return sqrt(t1 + t2);
}

bool CircuitGui::hasMouseOverlap(Item & item) const
{
	double dotsDistance = getDistance(item._fDot, item._sDot);

	double d1 = getDistance(GetMouseX(), GetMouseY(), item._fDot->getX(), item._fDot->getY());
	double d2 = getDistance(GetMouseX(), GetMouseY(), item._sDot->getX(), item._sDot->getY());

	double chord = sqrt(pow(d1, 2) + pow(d2, 2));

	if (chord <= dotsDistance)
		return true;

	return false;
}

double CircuitGui::roundDecimal(double number) const
{
	return std::ceil(number * 1000.0) / 1000.0;
}

Dot * CircuitGui::getNearDot(int x, int y)
{
	double distance = getDistance(x, y, _dots[0].getX(), _dots[0].getY());
	Dot* nearDot = &_dots[0];

	for (Dot& dot : _dots){
		double temp = getDistance(x, y, dot.getX(), dot.getY());
		if (temp < distance){
			distance = temp;
			nearDot = &dot;
		}
	}

	return nearDot;
}

int CircuitGui::getCoord(Dot * firstDot, Dot * secondDot) const
{
	if (secondDot->getX() > firstDot->getX())
		return CO_RIGHT;
	if (secondDot->getX() < firstDot->getX())
		return CO_LEFT;
	if (secondDot->getY() > firstDot->getY())
		return CO_DOWN;
	if (secondDot->getY() < firstDot->getY())
		return CO_UP;
}

std::string CircuitGui::removeNumberTrailer(std::string number) const
{
	if (number.find('.') == std::string::npos) return number;

	while (true)
	{
		if (number[number.length() - 1] == '0') number.resize(number.length() - 1);
		else
		{
			break;
		}

		if (number[number.length() - 1] == '.')
		{
			number.resize(number.length() - 1);
			break;
		}
	}

	return number;
}

/*
--------------- Drawing routine ---------------
*/
void CircuitGui::drawMenu()
{
	FillRect(0, 0, _menuWidth, _menuHeight, _blue);

	FillRect(0, _menuHeight, _menuWidth, 2, _neonBlue);

	DrawString(10, 10, "Naive Circuit Simulator V 1.0", _neonBlue, 2);
}

void CircuitGui::drawToolBar()
{
	FillRect(ScreenWidth() - 15 - _toolBarWidth, _menuHeight + _toolBarOffsetY, _toolBarWidth, _toolBarHeight, _blue);

	// Border top and bottom
	FillRect(ScreenWidth() - 15 - _toolBarWidth, _menuHeight + _toolBarOffsetY, _toolBarWidth, 2, _neonBlue);
	FillRect(ScreenWidth() - 15 - _toolBarWidth, _menuHeight + _toolBarOffsetY + _toolBarHeight, _toolBarWidth, 2, _neonBlue);
	// Border left and right
	FillRect(ScreenWidth() - 15 - _toolBarWidth, _menuHeight + _toolBarOffsetY, 2, _toolBarHeight, _neonBlue);
	FillRect(ScreenWidth() - 15, _menuHeight + _toolBarOffsetY, 2, _toolBarHeight + 2, _neonBlue);

	// Wire
	drawWire(812, 100, CO_RIGHT, "", _green, 3);
	DrawString(825, 110, "W", _green, 2);

	// Resistor
	drawResistor(812, 180, CO_RIGHT, "", _green, 3);
	DrawString(825, 190, "R", _green, 2);

	// Voltage
	drawVoltage(812, 250, CO_RIGHT, "", _green, 2);
	DrawString(825, 270, "V", _green, 2);

	// Blank
	FillCircle(812, 320, 2, _green);
	FillCircle(852, 320, 2, _green);
	DrawString(826, 337, "B", _green, 2);


	// Separator
	FillRect(ScreenWidth() - 15 - _toolBarWidth, 380, _toolBarWidth, 2, _neonBlue);
}

void CircuitGui::drawField()
{
	FillRect(2, _menuHeight + 2, ScreenWidth() - _toolBarWidth - _toolBarOffsetX + 12, ScreenHeight() - _menuHeight - 4, _darkBlue);

	int startX = 20;
	int startY = 60;
	
/*	for (int i = 0; i < 14; ++i){
		for (int j = 0; j < 19; ++j){
			FillCircle(startX + _dotDistance * j, startY + _dotDistance * i, 1, _neonBlue);
			DrawString(startX + _dotDistance * j - 5, startY + _dotDistance * i - 5, std::to_string(i) + "-" + std::to_string(j), _neonBlue);
		}
	}*/	
}

void CircuitGui::drawBorders()
{
	// Border top and bottom
	FillRect(0, 0, ScreenWidth(), 2, _neonBlue);
	FillRect(0, ScreenHeight() - 2, ScreenWidth(), 2, _neonBlue);
	// Border left and right
	FillRect(0, 0, 2, ScreenHeight(), _neonBlue);
	FillRect(ScreenWidth() - 2, 0, 2, ScreenHeight(), _neonBlue);
}

void CircuitGui::drawItemEditDialog(Item * item)
{
	if (item->_type == WIRE) return;

	// Draw Background
	FillRect(ScreenWidth() / 2 - 150, ScreenHeight() / 2 - 100, 300, 200, _blue);

	// Draw Top Border
	FillRect(ScreenWidth() / 2 - 150, ScreenHeight() / 2 - 100, 300, 2, _neonBlue);
	// Draw Bottom Border
	FillRect(ScreenWidth() / 2 - 150, ScreenHeight() / 2 - 100 + 200, 300, 2, _neonBlue);
	// Draw Left Border
	FillRect(ScreenWidth() / 2 - 150, ScreenHeight() / 2 - 100, 2, 200, _neonBlue);
	// Draw Right Border
	FillRect(ScreenWidth() / 2 - 150 + 300, ScreenHeight() / 2 - 100, 2, 202, _neonBlue);
	// Draw Seprator
	FillRect(ScreenWidth() / 2 - 150, ScreenHeight() / 2 - 100 + 30, 300, 2, _neonBlue);

	// Draw Title
	DrawString(ScreenWidth() / 2 - 150 + 10, ScreenHeight() / 2 - 100 + 10, "Edit Value", olc::YELLOW, 2);

	// Draw OK String
	DrawString(ScreenWidth() / 2 - 150 + 300 - 98, ScreenHeight() / 2 - 100 + 172, "OK", _neonBlue);

	// Draw OK Top Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 125, ScreenHeight() / 2 - 100 + 170 - 5, 70, 2, _neonBlue);
	// Draw OK Bottom Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 125, ScreenHeight() / 2 - 100 + 190 - 5, 70, 2, _neonBlue);
	// Draw OK Left Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 125, ScreenHeight() / 2 - 100 + 170 - 5, 2, 20, _neonBlue);
	// Draw OK Right Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 125 + 70, ScreenHeight() / 2 - 100 + 170 - 5, 2, 22, _neonBlue);

	// Draw Cancel String
	DrawString(ScreenWidth() / 2 - 150 + 300 - 227, ScreenHeight() / 2 - 100 + 172, "Cancel", _neonBlue);

	// Draw Cancel Top Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 240, ScreenHeight() / 2 - 100 + 170 - 5, 70, 2, _neonBlue);
	// Draw Cancel Bottom Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 240, ScreenHeight() / 2 - 100 + 190 - 5, 70, 2, _neonBlue);
	// Draw Cancel Left Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 240, ScreenHeight() / 2 - 100 + 170 - 5, 2, 20, _neonBlue);
	// Draw Cancel Right Border
	FillRect(ScreenWidth() / 2 - 150 + 300 - 240 + 70, ScreenHeight() / 2 - 100 + 170 - 5, 2, 22, _neonBlue);

	// Draw value Text Box
	DrawRect(ScreenWidth() / 2 - 150 + 120, ScreenHeight() / 2 - 100 + 82, 120, 20, _neonBlue);
	DrawRect(ScreenWidth() / 2 - 150 + 119, ScreenHeight() / 2 - 100 + 81, 122, 22, _neonBlue);

	// Draw Value String
	if (item->_type == RESISTOR)
		DrawString(ScreenWidth() / 2 - 150 + 20, ScreenHeight() / 2 - 100 + 90, "Resistance:", olc::YELLOW);

	if (item->_type == VOLTAGE)
		DrawString(ScreenWidth() / 2 - 150 + 50, ScreenHeight() / 2 - 100 + 90, "Voltage:", olc::YELLOW);

	DrawString(ScreenWidth() / 2 - 150 + 120 + 5, ScreenHeight() / 2 - 100 + 90, _editDialogTextBoxString, olc::YELLOW);

	_editDialogOpen = true;
}

void CircuitGui::drawAllItems()
{
	if (_editDialogOpen) return;

	// Clear element info in toolbar
	FillRect(ScreenWidth() - 15 - _toolBarWidth + 2, 385, _toolBarWidth - 2, 140, _blue);

	_overlappedItem = nullptr;
	bool overlapFound = false;

	for (Item& item : _itemsList){
		std::string detail = "";
		if (item._type == VOLTAGE) detail = item._voltage;
		if (item._type == RESISTOR) detail = item._resistance;

		if (hasMouseOverlap(item) && !overlapFound){
			drawItem(item._type, item._fDot, item._sDot, detail, olc::YELLOW);
			drawItemInfo(item);
			overlapFound = true;
			_overlappedItem = &item;
		}
		else{
			switch (item._type){
			case WIRE:
				drawItem(item._type, item._fDot, item._sDot, detail, _softOrange);
				break;
			case RESISTOR:
				drawItem(item._type, item._fDot, item._sDot, detail, _orange);
				break;
			case VOLTAGE:
				drawItem(item._type, item._fDot, item._sDot, detail, _green);
			}
		}
			
	}
}

void CircuitGui::drawItem(int type, Dot * firstDot, Dot * secondDot, std::string detail, olc::Pixel color)
{
	int coord = getCoord(firstDot, secondDot);

	switch (type)
	{
	case NONE:
		break;
	case WIRE:
		drawWire(firstDot->getX(), firstDot->getY(), coord, detail, color, 2);
		break;
	case RESISTOR:
		drawResistor(firstDot->getX(), firstDot->getY(), coord, detail, color, 3);
		break;
	case VOLTAGE:
		drawVoltage(firstDot->getX(), firstDot->getY(), coord, detail, color, 2);
		break;
	}

	switch (coord)
	{
	case CO_RIGHT:
		DrawString(firstDot->getX() + 10, firstDot->getY() - 18, detail, color);
		break;
	case CO_LEFT:
		DrawString(secondDot->getX() + 10, secondDot->getY() - 18, detail, color);
		break;
	case CO_DOWN:
		DrawString(firstDot->getX() + 15, firstDot->getY() + 10, detail, color);
		break;
	case CO_UP:
		DrawString(secondDot->getX() + 15, secondDot->getY() + 10, detail, color);
		break;
	}
}

void CircuitGui::drawItemInfo(Item & item)
{
	int x = ScreenWidth() - 15 - _toolBarWidth + 2;
	int y = 400;

	if (item._type != WIRE)
	{
		DrawString(x + 10, y + 90, "Press E to", _neonBlue);
		DrawString(x + 10, y + 100, "edit values", _neonBlue);
	}

	if (_circuitCore == nullptr)
	{
		if (_error == CircuitCore::Errors::NO_RESISTOR)
		{
			DrawString(x + 10, y + 20, "There is", _neonBlue);
			DrawString(x + 10, y + 40, "no resistor", _neonBlue);
		}

		if (_error == CircuitCore::Errors::NO_VOLTAGE_SOURCE)
		{
			DrawString(x + 10, y + 20, "There is", _neonBlue);
			DrawString(x + 10, y + 40, "no battery", _neonBlue);
		}

		if (_error == CircuitCore::Errors::NOT_CONNECTED)
		{
			DrawString(x + 10, y + 20, "Circuit", _neonBlue);
			DrawString(x + 10, y + 40, "is not", _neonBlue);
			DrawString(x + 10, y + 60, "connected", _neonBlue);
		}

		if (_error == CircuitCore::Errors::NOT_SERIES_NOT_PARALLEL)
		{
			DrawString(x + 10, y, "Circuit is", _neonBlue);
			DrawString(x + 10, y + 20, "neither", _neonBlue);
			DrawString(x + 10, y + 40, "parallel", _neonBlue);
			DrawString(x + 10, y + 60, "nor series", _neonBlue);
		}

		if (_error == CircuitCore::Errors::SHORT_CIRCUIT)
		{
			DrawString(x + 25, y + 20, "Short", _neonBlue);
			DrawString(x + 25, y + 40, "Circuit", _neonBlue);
			DrawString(x + 25, y + 60, "Found", _neonBlue);
		}

		if (_error == CircuitCore::Errors::SHORT_CIRCUIT_WITH_BATTERY)
		{
			DrawString(x + 3, y + 20, "Short Circuit", _neonBlue);
			DrawString(x + 5, y + 40, "With battery", _neonBlue);
			DrawString(x + 35, y + 60, "Found", _neonBlue);
		}

		return;
	}

	Element* element = _circuitCore->searchElement(item._name);

	if (element == nullptr)	return;

	item._voltage = removeNumberTrailer(std::to_string(roundDecimal(abs(element->getVoltage()))));
	item._current = removeNumberTrailer(std::to_string(roundDecimal(abs(element->getCurrent()))));
	item._resistance = removeNumberTrailer(std::to_string(roundDecimal(abs(element->getResistance()))));

	switch (item._type)
	{
	case RESISTOR:
		DrawString(x + 25, y, "Resistor", _neonBlue);
		break;
	case VOLTAGE:
		DrawString(x + 28, y, "Battery", _neonBlue);
		break;
	}

	DrawString(x + 10, y + 20, "I:" + item._current, _neonBlue);
	DrawString(x + 10, y + 40, "Vd:" + item._voltage, _neonBlue);
	DrawString(x + 10, y + 60, "R:" + item._resistance, _neonBlue);

}

void CircuitGui::drawWire(int x1, int y1, int coord, std::string detail, olc::Pixel color, int thickness)
{
	if (thickness == 0) return;

	int t = thickness - 1;

	if (coord == CO_RIGHT)
		DrawLine(x1, y1 + t, x1 + 40, y1 + t, color);
	if (coord == CO_LEFT)
		DrawLine(x1, y1 + t, x1 - 40, y1 + t, color);

	if (coord == CO_DOWN)
		DrawLine(x1 + t, y1, x1 + t, y1 + 40, color);
	if (coord == CO_UP)
		DrawLine(x1 + t, y1, x1 + t, y1 - 40, color);

	--thickness;
	drawWire(x1, y1, coord, detail, color, thickness);
}

void CircuitGui::drawResistor(int x1, int y1, int coord, std::string detail, olc::Pixel color, int thickness)
{
	if (thickness == 0) return;

	if (coord == CO_RIGHT)
	{
		for (int i = 0; i < thickness; ++i)
		{
			int x2 = x1 + _dotDistance;
			int y2 = y1;

			olc::vi2d a(x1, y1);
			olc::vi2d b(x1 + abs((x2 - x1) / 4), y1 - 10);
			olc::vi2d c(x1 + abs((x2 - x1) / 2), y1);
			olc::vi2d d(x2 - abs((x2 - x1) / 4), y1 - 10);
			olc::vi2d e(x2, y1);

			DrawLine(a, b, color);
			DrawLine(b, c, color);
			DrawLine(c, d, color);
			DrawLine(d, e, color);
			y1 = y1 + 1;
		}

	}
	if (coord == CO_LEFT)
	{
		for (int i = 0; i < thickness; ++i)
		{
			int x2 = x1 - _dotDistance;
			int y2 = y1;

			olc::vi2d a(x1, y1);
			olc::vi2d b(x1 - abs((x2 - x1) / 4), y1 - 10);
			olc::vi2d c(x1 - abs((x2 - x1) / 2), y1);
			olc::vi2d d(x2 + abs((x2 - x1) / 4), y1 - 10);
			olc::vi2d e(x2, y1);

			DrawLine(a, b, color);
			DrawLine(b, c, color);
			DrawLine(c, d, color);
			DrawLine(d, e, color);
			y1 = y1 + 1;
		}

	}
	if (coord == CO_UP)
	{
		for (int i = 0; i < thickness; ++i)
		{
			int x2 = x1;
			int y2 = y1 - _dotDistance;

			olc::vi2d a(x1, y1);
			olc::vi2d b(x1 + 10, y1 - abs((y2 - y1) / 4));
			olc::vi2d c(x1, y1 - abs((y2 - y1) / 2));
			olc::vi2d d(x1 + 10, y2 + abs((y2 - y1) / 4));
			olc::vi2d e(x2, y2);

			DrawLine(a, b, color);
			DrawLine(b, c, color);
			DrawLine(c, d, color);
			DrawLine(d, e, color);

			x1 = x1 + 1;
		}
	}
	if (coord == CO_DOWN)
	{
		for (int i = 0; i < thickness; ++i)
		{
			int x2 = x1;
			int y2 = y1 + _dotDistance;

			olc::vi2d a(x1, y1);
			olc::vi2d b(x1 + 10, y1 + abs((y2 - y1) / 4));
			olc::vi2d c(x1, y1 + abs((y2 - y1) / 2));
			olc::vi2d d(x1 + 10, y2 - abs((y2 - y1) / 4));
			olc::vi2d e(x2, y2);

			DrawLine(a, b, color);
			DrawLine(b, c, color);
			DrawLine(c, d, color);
			DrawLine(d, e, color);

			x1 = x1 + 1;
		}
	}

}

void CircuitGui::drawVoltage(int x1, int y1, int coord, std::string detail, olc::Pixel color, int thickness)
{
	if (thickness == 0) return;

	int t = thickness - 1;

	if (coord == CO_RIGHT)
	{
		olc::vi2d a(x1, y1 + t);
		olc::vi2d b(x1 + 16, y1 + t);
		olc::vi2d c(x1 + 8 + 16, y1 + t);
		olc::vi2d d(x1 + 40, y1 + t);

		DrawLine(a, b, color);
		DrawLine(c, d, color);

		DrawLine(b.x + t, b.y + 4, b.x + t, b.y - 4, color);
		DrawLine(c.x + t, c.y + 10, c.x + t, c.y - 10, color);
	}

	if (coord == CO_LEFT)
	{
		olc::vi2d a(x1, y1 + t);
		olc::vi2d b(x1 - 16, y1 + t);
		olc::vi2d c(x1 - 8 - 16, y1 + t);
		olc::vi2d d(x1 - 40, y1 + t);

		DrawLine(a, b, color);
		DrawLine(c, d, color);

		DrawLine(b.x + t, b.y + 4, b.x + t, b.y - 4, color);
		DrawLine(c.x + t, c.y + 10, c.x + t, c.y - 10, color);
	}

	if (coord == CO_UP)
	{
		olc::vi2d a(x1 + t, y1);
		olc::vi2d b(x1 + t, y1 - 16);
		olc::vi2d c(x1 + t, y1 - 8 - 16);
		olc::vi2d d(x1 + t, y1 - 40);

		DrawLine(a, b, color);
		DrawLine(c, d, color);

		DrawLine(b.x + 4, b.y + t, b.x - 4, b.y + t, color);
		DrawLine(c.x + 10, c.y + t, c.x - 10, c.y + t, color);
	}

	if (coord == CO_DOWN)
	{
		olc::vi2d a(x1 + t, y1);
		olc::vi2d b(x1 + t, y1 + 16);
		olc::vi2d c(x1 + t, y1 + 8 + 16);
		olc::vi2d d(x1 + t, y1 + 40);

		DrawLine(a, b, color);
		DrawLine(c, d, color);

		DrawLine(b.x + 4, b.y + t, b.x - 4, b.y + t, color);
		DrawLine(c.x + 10, c.y + t, c.x - 10, c.y + t, color);
	}

	--thickness;
	drawVoltage(x1, y1, coord, "", color, thickness);
}