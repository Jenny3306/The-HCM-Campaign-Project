// The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class Unit;
class UnitList;
class Army;
class TerrainElement;

class Vehicle;
class Infantry;

class LiberationArmy;
class ARVN;

class Position;

class Road;
class Mountain;
class River;
class Urban;
class Fortification;
class SpecialZone;

class BattleField;

class HCMCampaign;
class Configuration;

enum VehicleType
{
    TRUCK,
    MORTAR,
    ANTIAIRCRAFT,
    ARMOREDCAR,
    APC,
    ARTILLERY,
    TANK
};
enum InfantryType
{
    SNIPER,
    ANTIAIRCRAFTSQUAD,
    MORTARSQUAD,
    ENGINEER,
    SPECIALFORCES,
    REGULARINFANTRY
};

//!-----------------------------------------------------
//! CLASS Position
//!-----------------------------------------------------
class Position
{
private:
    int r, c;

public:
    Position(int r = 0, int c = 0);
    Position(const string &str_pos);
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const;
};

//!-----------------------------------------------------
//! CLASS Unit
//!-----------------------------------------------------
class Unit
{
protected:
    int quantity, weight;
    Position pos;
    // * You can add more attributes if needed
    int attackScore;
    int copyscore;

public:
    Unit(int quantity, int weight, Position pos);
    virtual ~Unit();
    virtual int getAttackScore() = 0;
    Position getCurrentPosition() const;
    int AttackScore() const;
    virtual string str() const = 0;
    void setWeight(int weight);
    void setQuantity(int quantity);

    int getQuantity() { return quantity; }
    int getWeight() { return weight; }
    void setPosition(Position p);

    void setAttackScore(int attackScore) {
        this->copyscore = attackScore;
        this->attackScore = attackScore;
    }
    virtual void addQuantity(int) = 0;  
    virtual bool isSameType(Unit* other) const = 0;
    virtual bool isEqual(const Unit& other) const = 0;  // pure virtual
    virtual Unit* clone() const = 0;
    virtual string getType() const = 0; 

    Position getPosition() const {
        return getCurrentPosition();
    }
};

//!-----------------------------------------------------
//! CLASS Infantry : public Unit
//!-----------------------------------------------------
class Infantry : public Unit
{
private:
    InfantryType infantryType;
    // * You can add more attributes if needed
public:
    Infantry(int quantity, int weight, Position pos, InfantryType infantryType);
    ~Infantry(){}
    int getAttackScore() override;
    string str() const override;

    // * You can add more methods if needed
    void computeInitialAttackScore();
    int AttackScore() const;

    int safeCeil(double value) {
        double diff = abs(value - round(value));
        return (diff < 1e-9) ? round(value) : ceil(value);
    }

    InfantryType getInfantryType() const { 
        return infantryType; 
    }
    
    void setInfantryType(InfantryType value) { 
        infantryType = value; 
    }    
    
    virtual void addQuantity(int more) override;

    bool isPerfectSquare(int n){
        int sqr = static_cast<int>(sqrt(n));
        if (sqr * sqr == n)
            return true;
        else
            return false;
    }

    // ? Tính tổng các chữ số của n
    int sumDigits(int n){
        // TODO: implement
        int s = 0;
        while (n){
            s += n % 10;
            n /= 10;
        }
        return s;
    }

    // ? Tính “số cá nhân”: tổng các chữ số của score và năm (1975), sau đó tiếp tục tính đến khi ra 1 chữ số.
    int computeSingleDigit(int score, int year){
        // TODO: implement
        int v = sumDigits(score) + sumDigits(year);
        while (v >= 10) v = sumDigits(v);
        return v;
    }

    bool isSameType(Unit* other) const override {
        Infantry* inf = dynamic_cast<Infantry*>(other);
        if (!inf) return false;
        return (this->infantryType == inf->infantryType && this->weight == inf->weight);
    }

    Unit* clone() const override { return new Infantry(*this); }
    bool isEqual(const Unit& other) const override;
    string getType() const override;

    static string infantryTypeToString(InfantryType it)
    {
        switch(it)
        {
            case SNIPER:              return "SNIPER";
            case ANTIAIRCRAFTSQUAD:     return "ANTIAIRCRAFTSQUAD";
            case MORTARSQUAD:           return "MORTARSQUAD";
            case ENGINEER:              return "ENGINEER";
            case SPECIALFORCES:         return "SPECIALFORCES";
            case REGULARINFANTRY:       return "REGULARINFANTRY";
            default:                    return "UNKNOWN";
        }
    }
};

//!-----------------------------------------------------
//! CLASS Vehicle : public Unit
//!-----------------------------------------------------
class Vehicle : public Unit
{
private:
    VehicleType vehicleType;
    // * You can add more attributes if needed
public:
    Vehicle(int quantity, int weight, Position pos, VehicleType vehicleType);
    ~Vehicle() {}
    int getAttackScore() override;
    string str() const override;
    // * You can add more methods if needed
    void computeAttackScore(){
        quantity = std::max(1, quantity);
        weight = std::max(1, weight);
        double raw = static_cast<int>(vehicleType) * 304.0 + quantity * weight * 1.0;
        attackScore = safeCeil(raw/30.0f);
    }
    int AttackScore() const;

    int safeCeil(double value) {
        double diff = abs(value - round(value));
        return (diff < 1e-9) ? round(value) : ceil(value);
    }

    // * Additional functions if possible
    VehicleType getVehicleType() const { 
        return vehicleType; 
    }

    void setVehicleType(VehicleType value) { 
        vehicleType = value; 
    }   
        
    void addQuantity(int more) override;

    bool isSameType(Unit* other) const {
        Vehicle* veh = dynamic_cast<Vehicle*>(other);
        if (!veh) return false;
        return (this->vehicleType == veh->vehicleType && this->weight == veh->weight);
}

    Unit* clone() const override { return new Vehicle(*this); }
    bool isEqual(const Unit& other) const override;
    string getType() const override;

    static string vehicleTypeToString(VehicleType vt) {
        switch(vt)
        {
            case TRUCK:         return "TRUCK";
            case MORTAR:        return "MORTAR";
            case ANTIAIRCRAFT:  return "ANTIAIRCRAFT";
            case ARMOREDCAR:    return "ARMOREDCAR";
            case APC:           return "APC";
            case ARTILLERY:     return "ARTILLERY";
            case TANK:          return "TANK";
            default:            return "UNKNOWN";
        }
    }
};

//!-----------------------------------------------------
//! CLASS UnitList
//!-----------------------------------------------------
class UnitList
{
private:
    int capacity;
    // * You can add more attributes if needed
vector<Unit *> units;
    int countVehicle, countInfantry;
    struct Node {
        Unit* unit;
        Node* next;
        explicit Node(Unit* u) : unit(u), next(nullptr) {}
    };
    Node* head; // Con trỏ đến phần tử đầu danh sách
    Node* tail; // Con trỏ đến phần tử cuối danh sách
    Army* owner = nullptr;

public:
    UnitList(int capacity);
    bool insert(Unit *unit);
    bool isContain(VehicleType vehicleType);
    bool isContain(InfantryType infantryType);
    // * You can add more methods if needed
    vector<Unit *> getUnits() const;
    string str() const;

    // * Additional functions if possible
    void setCapacity(int cap) { capacity = cap; };
    int getNumberOfUnits() const { return units.size();}

    Unit* getUnitAt(int index) const {
        if (index >= 0 && index < (int)units.size()) {
            return units[index];
        }
        return nullptr;
    }

    void setUnits(vector<Unit *> units);
    void setOwner(Army* army) { owner = army; }

    void remove(Unit *unit);
    UnitList(UnitList& other);
    UnitList& operator=(UnitList& other);
    void clear();
    ~UnitList();
};

//!-----------------------------------------------------
//! CLASS Army 
//!-----------------------------------------------------
class Army {
protected:
    int LF;
    int EXP;
    string name;
    BattleField* battleField;
    // * You can add more attributes if needed
    Unit **unitArray;       // For temporary
    int size_V = 0;         // For temporary: vehicle count
    int size_I = 0;    
    int initialLF = 0;  
    int initialEXP = 0;

    inline bool isVehicle(const Unit* u);
    bool LiberWin = 0;
    
public:
    UnitList *unitList;
    Army(Unit **unitArray, int size, string name, BattleField *battleField);
    virtual ~Army();
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;

    // * You can add more methods if needed
    void updateScore(bool update);
    int safeCeil(double value);

    bool isSpecial(int S);
    bool isSpecialInBase(int number, int base);

    UnitList* getUnitList() const { return unitList; }
    int getLF() const { return LF; }
    int getEXP() const { return EXP; }
    void setLF(int val) { LF = val; }
    void setEXP(int val) { EXP = val; }
    
    void addEXP(int val) { 
        EXP += val; 
        if (EXP < 0) EXP = 0; 
        EXP = min(EXP, 500); 
    }

    void addLF(int val)  { 
        LF += val; 
        if (LF < 0) LF  = 0; 
        LF = min(LF, 1000); 
    }

    bool getLiberWin(){
        return LiberWin;
    }

    void removeUnit(){
        for (int i = 0; i < unitList->getUnits().size(); i++) {
            if (unitList->getUnits()[i]->getAttackScore() <= 5) {
                unitList->remove(unitList->getUnits()[i]);
                i--;
            }  
        }
    }
};

//!-----------------------------------------------------
//! CLASS LiberationArmy
//!-----------------------------------------------------
class LiberationArmy : public Army
{
private:
    // * You can add more attributes if needed
    static int nearestFibonacci(int value);
    static vector<Unit*> knapsack(vector<Unit*> units, int maxScore);
    static Unit* cloneUnit(Unit* unit);
    bool LiberWin;

public:
    LiberationArmy(Unit **unitArray, int size, string name, BattleField *battleField);
    ~LiberationArmy();
    void fight(Army *enemy, bool defense) override;
    string str() const override;
    // * You can add more methods if needed
    int getLF() {
        return this->LF;
    }
    int getEXP() {
        return this->EXP;
    }

    int scoreSum(const vector<Unit *> &vec);
    int checkRange(int& num, int MAX){
        return num = max(0, min(num, MAX));
    }
    void updateScore(bool update);
    virtual string getName() const;
    bool getLiberWin() const;
};

//!-----------------------------------------------------
//! CLASS ARVN
//!-----------------------------------------------------
class ARVN : public Army
{
private:
    // * You can add more attributes if needed
public:
    ARVN(Unit** unitArray, int size, string name, BattleField* battleField);
    ~ARVN();
    void fight(Army* enemy, bool defense=false) override;
    string str() const;

    // * You can add more methods if needed
    int safeCeil(double val);
    void updateScores(bool isInit);
    virtual string getName() const;

};

//!-----------------------------------------------------
//! CLASS TerrainElement và các lớp dẫn xuất
//!-----------------------------------------------------
class TerrainElement {
protected:
    Position pos;
    // * You can add more attributes if needed
    // Phương thức tính khoảng cách giữa hai vị trí
    double calculateDistance(const Position& pos1, const Position& pos2) {
        int rowDiff = pos1.getRow() - pos2.getRow();
        int colDiff = pos1.getCol() - pos2.getCol();
        return (sqrt(double(rowDiff * rowDiff + colDiff * colDiff)));
    }
    int safeCeil(double value) {
        double diff = abs(value - round(value));
        return (diff < 1e-9) ? round(value) : ceil(value);
    }

public:
    TerrainElement(Position);
    virtual ~TerrainElement();
    virtual void getEffect(Army *army) = 0;
    // * You can add more attributes if needed
    virtual TerrainElement* clone() const = 0;
    Position getPosition() const;
    void setPosition(Position pos) {
        this->pos = pos;
    }
    virtual string str() const = 0;

};

class Road : public TerrainElement {
public:
    Road(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

class Mountain : public TerrainElement {
public:
    Mountain(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

class River : public TerrainElement {
public:
    River(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

class Urban : public TerrainElement {
public:
    Urban(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

class Fortification : public TerrainElement {
public:
    Fortification(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

class SpecialZone : public TerrainElement {
public:
    SpecialZone(Position pos) : TerrainElement(pos) {}
    void getEffect(Army *army);
    // * You can add more methods if needed
    TerrainElement* clone() const override; 
    string str() const override;
};

//!-----------------------------------------------------
//! CLASS BattleField
//!-----------------------------------------------------
class BattleField {
private:
    int n_rows, n_cols;
    TerrainElement ***terrain;
    // * You can add more attributes if needed
    vector<vector<TerrainElement*>> grid;

public:
    BattleField(int n_rows, int n_cols, const vector<Position*>& arrayForest,
                const vector<Position*>& arrayRiver, const vector<Position*>& arrayFortification,
                const vector<Position*>& arrayUrban, const vector<Position*>& arraySpecialZone);
    ~BattleField();
    TerrainElement* getElement(int r, int c) const;  // Trả về đối tượng TerrainElement tại vị trí (r,c)
    string str() const;
    // * You can add more methods if needed
};

//!-----------------------------------------------------
//! CLASS Configuration
//!-----------------------------------------------------
class Configuration {
private:
    int num_rows;
    int num_cols;
    vector<Position*> arrayForest;
    vector<Position*> arrayRiver;
    vector<Position*> arrayFortification;
    vector<Position*> arrayUrban;
    vector<Position*> arraySpecialZone;
    vector<Unit*> liberationUnits;
    vector<Unit*> ARVNUnits;
    
    Unit** liberationUnitArray = nullptr;
    Unit** ARVNUnitArray = nullptr;
    int eventCode;
    // * You can modify or add more attributes if needed
public:
    Configuration(const string & filepath);
    ~Configuration();
    string str() const;

    // * You can add more getter methods if needed

    static string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

static string positionArrayToString(const vector<Position *> &arr)
{
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < arr.size(); ++i)
    {
        ss << arr[i]->str();
        if (i + 1 < arr.size())
            ss << ",";
    }
    ss << "]";
    return ss.str();
}

static string unitListToString(const vector<Unit *> &arr)
{
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < arr.size(); ++i)
    {
        ss << arr[i]->str();
        if (i + 1 < arr.size())
            ss << ",";
    }
    ss << "]";
    return ss.str();
}

vector<string> splitUnits(const string &str)
{
    vector<string> result;
    string current = "";
    int paren = 0;
    for (char c : str)
    {
        if (c == '(')
            paren++;
        if (c == ')')
            paren--;
        if (c == ',' && paren == 0)
        {
            result.push_back(current);
            current = "";
        }
        else
        {
            current += c;
        }
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}
    
    // * Getter methods */
    int getNumRows() const { return num_rows; }
    int getNumCols() const { return num_cols; }
    int getEventCode() const { return eventCode; }
    const vector<Position*>& getForestPositions() const { return arrayForest; }
    const vector<Position*>& getRiverPositions() const { return arrayRiver; }
    const vector<Position*>& getFortificationPositions() const { return arrayFortification; }
    const vector<Position*>& getUrbanPositions() const { return arrayUrban; }
    const vector<Position*>& getSpecialZonePositions() const { return arraySpecialZone; }
    vector<Unit*>& getLiberationUnits() { return liberationUnits; }
    vector<Unit*>& getARVNUnits() { return ARVNUnits; }

    Unit** getLiberationUnitArray() const { return liberationUnitArray; }
    Unit** getARVNUnitArray() const { return ARVNUnitArray; }
    int getLiberSize() const { return liberationUnits.size(); }
    int getARVNSize() const { return ARVNUnits.size(); }
};

//!-----------------------------------------------------
//! Lớp HCMCampaign
//!-----------------------------------------------------
class HCMCampaign {
private:
    Configuration* config;
    BattleField* battleField;
    LiberationArmy* liberationArmy;
    ARVN* arvnArmy;
    // * You can modify or add more attributes if needed
public:
    HCMCampaign(const string & config_file_path);
    ~HCMCampaign();
    void run();
    // Phương thức printResult: trả về chuỗi kết quả theo định dạng:
    // "LIBERATIONARMY[LF=<LF>,EXP=<EXP>]-ARVN[LF=<LF>,EXP=<EXP>]"
    string printResult();
    // * You can add more methods if needed
};
#endif