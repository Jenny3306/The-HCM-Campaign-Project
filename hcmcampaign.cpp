#include "hcmcampaign.h"
//!----------------------------------------------
//! Lớp Position
//!----------------------------------------------
Position::Position(int r, int c) : r(r), c(c) {}
Position::Position(const string &str_pos)
{
    sscanf(str_pos.c_str(), "(%d,%d)", &r, &c);
}
int Position::getRow() const { return r; }
int Position::getCol() const { return c; }
void Position::setRow(int r)
{
    this->r = std::max(0, r);
}
void Position::setCol(int c)
{
    this->c = std::max(0, c);
}
string Position::str() const
{
    stringstream ss;
    ss << "(" << r << "," << c << ")";
    return ss.str();
}

//!----------------------------------------------
//! Lớp Unit
//!----------------------------------------------
Unit::Unit(int quantity, int weight, Position pos)
    : quantity(quantity), weight(weight), pos(pos)
{
}

Unit::~Unit()
{
    // Virtual destructor – nếu có bộ nhớ động thì giải phóng ở đây.
}

void Unit::setPosition(Position p)
{
    this->pos = p;
}

Position Unit::getCurrentPosition() const
{
    // TODO: Implement
    return pos;
}

void Unit::setWeight(int w)
{
    // TODO: Implement
    weight = w;
}

void Unit::setQuantity(int q)
{
    // TODO: Implement
    quantity = q;
}

//!----------------------------------------------
//! Lớp Infantry
//!----------------------------------------------
// Kiểm tra n có phải là số chính phương không
Infantry::Infantry(int quantity, int weight, Position pos, InfantryType infantryType)
    : Unit(quantity, weight, pos), infantryType(infantryType)
{
    // Đảm bảo quantity >= 1
    if (this->quantity < 1) {
        this->quantity = 1;
    }
    
    // Đảm bảo weight >= 1  
    if (this->weight < 1) {
        this->weight = 1;
    }

    //computeInitialAttackScore();
    // KHÔNG gọi computeInitialAttackScore() ở đây
    // Chỉ tính attackScore cơ bản để có giá trị ban đầu
    // int tv = static_cast<int>(infantryType);
    // attackScore = tv * 56 + quantity * weight;
    // copyscore = attackScore;
    computeInitialAttackScore();
}

void Infantry::computeInitialAttackScore()
{
    int tv = static_cast<int>(infantryType);
    int baseQty = quantity;
    int checkScore = tv * 56 + baseQty * weight;

    // Nếu là SPECIALFORCES và weight là số chính phương, cộng 75 vào checkScore để xét số cá nhân
    if (infantryType == SPECIALFORCES && isPerfectSquare(weight)) checkScore += 75;
    // cout << str() << "  " << checkScore << endl;
    

    int single = computeSingleDigit(checkScore, 1975);

    // Dựa vào số cá nhân để tăng/giảm quantity
    if (single > 7) {
        int inc = safeCeil((double)(baseQty * 20.0f) / 100.0f);
        baseQty += inc;
    }
    else if (single < 3) {
        int dec = safeCeil((double)(- baseQty * 10.0f) / 100.0f);
        baseQty += dec;
        if(baseQty < 1) baseQty = 1;
    }

    // Gán lại quantity và tính attackScore cuối cùng
    quantity = baseQty;
    attackScore = tv * 56 + baseQty * weight;
    copyscore = attackScore;
}

int Infantry::getAttackScore() {
    // Gọi computeInitialAttackScore() để cập nhật quantity và attackScore
    // computeInitialAttackScore();
    return copyscore;
}

int Infantry::AttackScore() const {
    return attackScore;
}

void Infantry::addQuantity(int more) {
    this->quantity += more;
}

bool Infantry::isEqual(const Unit &other) const {
    const Infantry *o = dynamic_cast<const Infantry *>(&other);
    return o && infantryType == o->infantryType;
}

string Infantry::getType() const {
    return "Infantry";
}

string Infantry::str() const {
    static const char *infantryNames[] = {"SNIPER", "ANTIAIRCRAFTSQUAD", "MORTARSQUAD", "ENGINEER", "SPECIALFORCES", "REGULARINFANTRY"};
    if (infantryType >= 0 && infantryType <= 5)
    {
        stringstream ss;
        ss << "Infantry[infantryType=" << string(infantryNames[infantryType])
           << ",quantity=" << (quantity < 1 ? 1 : quantity)
           << ",weight=" << (weight < 1 ? 1 : weight)
           << ",position=" << pos.str() << "]";
        return ss.str();
    }
    else
        return "Infantry[UNKNOWN]";
}

//!----------------------------------------------
//! Lớp Vehicle
//!----------------------------------------------
Vehicle::Vehicle(int quantity, int weight, Position pos, VehicleType vehicleType)
    : Unit(quantity, weight, pos), vehicleType(vehicleType){
        computeAttackScore();
    }

int Vehicle::getAttackScore() {
    // TODO: Implement
    return attackScore;
}

void Vehicle::addQuantity(int more) {
    this->quantity += more;
}

bool Vehicle::isEqual(const Unit &other) const {
    const Vehicle *o = dynamic_cast<const Vehicle *>(&other);
    return o && vehicleType == o->vehicleType;
}

std::string Vehicle::getType() const {
    return "Vehicle";
}

/*
@Fix
 */
string Vehicle::str() const {
    static const char *vehicleNames[] = {"TRUCK", "MORTAR", "ANTIAIRCRAFT", "ARMOREDCAR", "APC", "ARTILLERY", "TANK"};
    if (vehicleType >= 0 && vehicleType <= 6)
    {
        stringstream ss;
        ss << "Vehicle[vehicleType=" << string(vehicleNames[vehicleType])
           << ",quantity=" << (quantity < 1 ? 1 : quantity)
           << ",weight=" << (weight < 1 ? 1 : weight)
           << ",position=" << pos.str() << "]";
        return ss.str();
    }
    else
        return "Vehicle[UNKNOWN]";
}

//!----------------------------------------------
//! Lớp UnitList
//!----------------------------------------------
UnitList::UnitList(int capacity) {
    // TODO: Implement
    head = tail = nullptr;
    countVehicle = countInfantry = 0;

    this->capacity = capacity;
}

bool UnitList::insert(Unit *unit) {
    if (!unit)
        return false;

    // Kiểm tra xem đơn vị đã tồn tại trong danh sách chưa (dựa trên isEqual)
    Node *current = head;
    while (current != nullptr)
    {
        if (current->unit->isEqual(*unit))
        {
            // Nếu đơn vị đã tồn tại → cộng thêm quantity
            int newQty = current->unit->getQuantity() + unit->getQuantity();

            // Keep position of unit with greater weight
            current->unit->setWeight(max(unit->getWeight(), current->unit->getWeight()));
            current->unit->setQuantity(newQty);

            Infantry *inf = dynamic_cast<Infantry *>(current->unit);
            if (inf != 0)
            {
                inf->computeInitialAttackScore();
            }

            if (owner != 0)
                owner->updateScore(true);
            return false;
        }
        current = current->next;
    }

    // Tính tổng hiện tại
    int totalUnits = countInfantry + countVehicle;

    // Nếu là Infantry, kiểm tra capacity trước
    if (dynamic_cast<Infantry *>(unit))
    {
        if (totalUnits >= capacity)
            return false;
    }
    // Nếu là Vehicle, chỉ cho phép chèn nếu còn chỗ sau khi chèn Infantry
    else if (dynamic_cast<Vehicle *>(unit))
    {
        int remainingSlots = capacity - countInfantry;
        if (remainingSlots < 0 || countVehicle >= remainingSlots)
            return false;
    }

    // Kiểm tra capacity
    if (dynamic_cast<Infantry *>(unit) && countInfantry >= capacity)
        return false;
    if (dynamic_cast<Vehicle *>(unit) && countVehicle >= capacity)
        return false;

    // Tạo node mới từ bản clone của unit
    Unit *cloned = unit->clone();
    if (!cloned)
        return false;
    Node *newNode = new Node(cloned);
    newNode->next = nullptr;

    if (!head)
    {
        head = tail = newNode;
    }
    else if (cloned->getType() == "Infantry")
    {
        newNode->next = head;
        head = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }

    if (cloned->getType() == "Infantry")
        countInfantry++;
    else
        countVehicle++;

    // Thêm vào vector quản lý toàn cục
    units.push_back(cloned);
    return true;
}

int Unit::AttackScore() const {
    return attackScore;
}

bool UnitList::isContain(VehicleType vehicleType) {
    Node *current = head;
    while (current != nullptr) {
        // Try to cast the unit to Vehicle and check its type
        Vehicle *vehicle = dynamic_cast<Vehicle *>(current->unit);
        if (vehicle && vehicle->getVehicleType() == vehicleType) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool UnitList::isContain(InfantryType infantryType) {
    Node *current = head;
    while (current != nullptr) {
        // Try to cast the unit to Infantry and check its type
        Infantry *infantry = dynamic_cast<Infantry *>(current->unit);
        if (infantry && infantry->getInfantryType() == infantryType) {
            return true;
        }
        current = current->next;
    }
    return false;
}

vector<Unit *> UnitList::getUnits() const {
    return units; // Return the vector of units
}

string UnitList::str() const {
    stringstream ss;
    ss << "UnitList[count_vehicle=" << countVehicle << ";count_infantry=" << countInfantry;

    // Add unit list
    if (head != nullptr)
        ss << ";";
    Node *current = head;
    while (current != nullptr) {
        ss << current->unit->str();
        current = current->next;
        if (current != nullptr) {
            ss << ",";
        }
    }
    ss << "]";

    return ss.str();
}

void UnitList::remove(Unit *unit) {
    if (!unit) return;
    if (head == nullptr)
        return; // Empty list

    // Special case: removing the head
    if (head->unit == unit) {
        Node *temp = head;
        head = head->next;

        // If we're removing the last element, update tail
        if (head == nullptr) {
            tail = nullptr;
        }

        // Update count
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        Infantry *infantry = dynamic_cast<Infantry *>(unit);

        if (vehicle) {
            countVehicle--;
        }
        else if (infantry) {
            countInfantry--;
        }

        // Remove from vector
        for (auto it = units.begin(); it != units.end(); ++it) {
            if (*it == unit) {
                units.erase(it);
                break;
            }
        }

        delete temp;
        return;
    }

    // Find the node before the one to remove
    Node *current = head;
    while (current->next != nullptr && current->next->unit != unit) {
        current = current->next;
    }

    // If found, remove it
    if (current->next != nullptr) {
        Node *temp = current->next;
        current->next = temp->next;

        // If we're removing the tail, update tail pointer
        if (temp == tail) {
            tail = current;
        }

        // Update count
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        Infantry *infantry = dynamic_cast<Infantry *>(unit);

        if (vehicle) countVehicle--;
        else if (infantry) countInfantry--;
        

        // Remove from vector
        for (auto it = units.begin(); it != units.end(); ++it) {
            if (*it == unit) {
                units.erase(it);
                break;
            }
        }

        delete temp;
    }
}

// Copy constructor
UnitList::UnitList(UnitList &other) {
    this->capacity = other.capacity;
    this->countVehicle = 0;
    this->countInfantry = 0;
    this->head = nullptr;
    this->tail = nullptr;

    // Deep copy the linked list
    Node *current = other.head;
    while (current != nullptr) {
        insert(current->unit); // This will also handle the unit type-specific positioning
        current = current->next;
    }
}

// Assignment operator
UnitList &UnitList::operator=(UnitList &other) {
    if (this != &other) {
        // Clear current list
        clear();

        // Copy from other
        capacity = other.capacity;
        Node *current = other.head;
        while (current != nullptr) {
            insert(current->unit);
            current = current->next;
        }
    }
    return *this;
}

// Clear the list
void UnitList::clear(){
    Node *curr = head;
    while (curr != nullptr) {
        Node *temp = curr;
        curr = curr->next;
        delete temp->unit;
        delete temp;
    }
    head = tail = nullptr;
    countVehicle = countInfantry = 0;
    units.clear(); // Clear the vector as well
}

// Destructor
UnitList::~UnitList(){
    clear();
}

//!----------------------------------------------
//! Lớp Army
//!----------------------------------------------
Army::Army(Unit **unitArray, int size, string name, BattleField *battlefield)
    : name(name), battleField(battlefield) {
    this->unitArray = unitArray;
    this->size_V = size;

    // Tạm tính tổng LF + EXP để tạo UnitList đúng capacity
    int tmpLF = 0, tmpEXP = 0;
    for (int i = 0; i < size; ++i) {
        if (!unitArray[i])
            continue;
        if (isVehicle(unitArray[i]))
            tmpLF += unitArray[i]->getAttackScore();
        else
            tmpEXP += unitArray[i]->getAttackScore();
    }
    // cout << size << endl;

    if (tmpLF < 0) tmpLF = 0;
    if (tmpLF > 1000) tmpLF = 1000;
    if (tmpEXP < 0) tmpEXP = 0;
    if (tmpEXP > 500) tmpEXP = 500;

    LF = tmpLF;
    EXP = tmpEXP;

    // TÍNH CAPACITY DỰA TRÊN SỐ ĐẶC BIỆT
    int S = tmpEXP + tmpLF;
    int capacity;
    if (isSpecial(S)) {
        capacity = 12;
    } else {
        capacity = 8;
    }

    unitList = new UnitList(capacity);

    // insert trực tiếp bản gốc unitArray[i] – KHÔNG CLONE
    for (int i = 0; i < size; ++i) {
        if (unitArray[i]){
            Unit* tmp = unitArray[i]->clone();
            if(tmp){
                tmp->getAttackScore();
                unitList->insert(tmp);

                // LUÔN DELETE TMP VÌ UNITLIST SẼ TỰ CLONE LẠI BÊN TRONG
                // Hoặc merge với unit có sẵn
                delete tmp;
            }
            // cout << tmp->getAttackScore() << endl;
        }
    }
}

Army::~Army() {
    delete unitList;
}

// Check if a number is a special number in a given base
bool Army::isSpecialInBase(int number, int base) {
    if (number < 0)
        return false;
    if (number == 0)
        return true;
    for (int mask = 1; mask < (1 << 12); ++mask) {
        long long sum = 0;
        for (int i = 0; i < 15; ++i) {
            if (mask & (1 << i)) {
                sum += static_cast<long long>(pow(base, i));
            }
        }
        if (sum == (long long)number) return true;
    }
    return false;
}

bool Army::isSpecial(int S)
{
    // Check if S is special in any odd prime base less than 10
    const int oddPrimes[] = {3, 5, 7}; // Odd primes less than 10
    for (int prime : oddPrimes) {
        if (isSpecialInBase(S, prime)) {
            return true;
        }
    }
    return false;
}

inline bool Army::isVehicle(const Unit *u) {
    return dynamic_cast<const Vehicle *>(u) != nullptr;
}

void Army::fight(Army *enemy, bool defense) {
    if (!unitList) return;    
    

    if (!battleField) return; // Hợp lệ vì `fight()` trả về void
    
    // Terrain effect should apply before fight
    if (battleField != nullptr) {
        for (Unit *unit : this->unitList->getUnits()) {
            if (!unit)
                continue;
            Position pos = unit->getPosition();
            TerrainElement *terrain = battleField->getElement(pos.getRow(), pos.getCol());
            if (terrain)
                terrain->getEffect(this);
        }
    }

    if (enemy && enemy->battleField != nullptr) {
        for (Unit *unit : enemy->unitList->getUnits()){
            if (!unit)continue;

            Position pos = unit->getPosition();

            TerrainElement *terrain = enemy->battleField->getElement(pos.getRow(), pos.getCol());
            if (terrain == nullptr) continue;

            if (terrain) terrain->getEffect(enemy);
        }
    }
}

string Army::str() const
{
    // TODO: Implement
    ostringstream oss;
    oss << "Army[name=" << name << ",LF=" << to_string(LF) << ",EXP=" << to_string(EXP)
        << ",unitList=" << unitList->str() << "]";
    return oss.str();
}

int Army::safeCeil(double value)
{
    // TODO: Implement
    return static_cast<int>(ceil(value));
}

void Army::updateScore(bool update)
{
    LF = EXP = 0;
    for (auto *u : unitList->getUnits()) {
        auto *v = dynamic_cast<Vehicle *>(u);
        if (v) {
            v->computeAttackScore();
            LF += v->getAttackScore();
        }
        else {
            auto tmp = dynamic_cast<Infantry*>(u);
            tmp->computeInitialAttackScore();
            EXP += u->getAttackScore();
        }
    }
    if (update)
    {
        LF = min(1000, LF);
        EXP = min(500, EXP);
    }
}

//!----------------------------------------------
//! CLASS LiberationArmy
//!----------------------------------------------
LiberationArmy::LiberationArmy(Unit **unitArray, int size, string name, BattleField *battlefield)
    : Army(unitArray, size, name, battlefield)
{}

bool LiberationArmy::getLiberWin() const
{
    return this->LiberWin;
}

void LiberationArmy::fight(Army *enemy, bool defense)
{
    // Army::fight(enemy, defense); // áp dụng hiệu ứng địa hình trước

    this->LiberWin = false;
    if (!defense)
    {
        int originalLF = this->LF;
        int originalEXP = this->EXP;

        int boostedLF = safeCeil(this->LF * 1.5);
        int boostedEXP = safeCeil(this->EXP * 1.5);
        checkRange(boostedLF, 1000);
        checkRange(boostedEXP, 500);

        this->LF = boostedLF;
        this->EXP = boostedEXP;

        int enemyLF = enemy->getLF();
        int enemyEXP = enemy->getEXP();

        vector<Unit *> infantryUnits, vehicleUnits;
        vector<Unit *> myUnits = unitList->getUnits();
        for (Unit *u : myUnits)
        {
            if (!u)
                continue;
            if (u->getType() == "Infantry")
                infantryUnits.push_back(u);
            else if (u->getType() == "Vehicle")
                vehicleUnits.push_back(u);
        }


        auto comboA = knapsack(infantryUnits, enemyEXP);
        auto comboB = knapsack(vehicleUnits, enemyLF);
    
        // cout << "------------------------------" << endl;
        // cout << comboA.size() << endl;
        // cout << comboB.size() << endl;
        // cout << boostedEXP << endl;
        // cout << boostedLF << endl;
        // cout << enemy->getEXP() << endl;
        // cout << enemy->getLF() << endl;
        // cout << "------------------------------" << endl;

        bool canfight = false;
        if (!comboA.empty() && !comboB.empty())
            canfight = true;
        else if (!comboA.empty() && boostedLF > enemyLF)
            canfight = true;
        else if (!comboB.empty() && boostedEXP > enemyEXP)
            canfight = true;
        // else if (boostedLF > enemyLF && boostedEXP > enemyEXP)
        //     canfight = true;
        if (canfight)
        {
            // Remove relevant units from unitList
            if (!comboA.empty() && !comboB.empty())
            {
                for (Unit *u : comboA)
                    unitList->remove(u);
                for (Unit *u : comboB)
                    unitList->remove(u);
                LiberWin = true;
            }
            else if (!comboA.empty() && boostedLF > enemyLF)
            {
                for (Unit *u : comboA)
                    unitList->remove(u);
                for (Unit *u : vehicleUnits)
                    unitList->remove(u);
                LiberWin = true;
            }
            else if (!comboB.empty() && boostedEXP > enemyEXP)
            {
                for (Unit *u : comboB)
                    unitList->remove(u);
                for (Unit *u : infantryUnits)
                    unitList->remove(u);
                LiberWin = true;
            }

            // Capture enemy units
            vector<Unit *> enemyUnits = enemy->getUnitList()->getUnits();
            // enemy->getUnitList()->clear(); // clear trước

            int tempLF = 0, tempEXP = 0;

            // Tính trước điểm của từng đơn vị enemy
            for (Unit *u : enemyUnits)
            {
                if (!u)
                    continue;
                if (Infantry *inf = dynamic_cast<Infantry *>(u))
                {
                    tempEXP += inf->AttackScore(); // dùng AttackScore ban đầu
                }
                else if (Vehicle *v = dynamic_cast<Vehicle *>(u))
                {
                    int q = v->getQuantity();
                    int raw = v->getVehicleType() * 304 + v->getWeight();
                    for (int i = 0; i < q; ++i)
                        tempLF += safeCeil(raw / 30.0);
                }
            }

            // Insert Infantry (reverse order for head insert)
            for (int i = 0; i < enemyUnits.size(); ++i)
            {
                if (enemyUnits[i]->getType() == "Infantry")
                {
                    unitList->insert(cloneUnit(enemyUnits[i]));
                }
            }

            // Insert Vehicle (same order for tail insert)
            for (int i = (int)enemyUnits.size() - 1; i >= 0; --i)
            {
                if (enemyUnits[i]->getType() == "Vehicle")
                {
                    Unit *cloned = cloneUnit(enemyUnits[i]);
                    unitList->insert(cloned);
                }
            }

            // Clear enemy units
            enemy->getUnitList()->clear();
            enemy->setLF(0);
            enemy->setEXP(0);
            enemy->updateScore(true);

            // Gán thủ công điểm đã tính
            this->LF = min(1000, tempLF);
            this->EXP = min(500, tempEXP);

            updateScore(true);
        }
        else
        {
            this->LF = originalLF;
            this->EXP = originalEXP;
            checkRange(EXP, 500);
            checkRange(originalEXP, 500);

            for (Unit *u : unitList->getUnits())
            {
                if (!u)
                    continue;
                int newW = safeCeil(u->getWeight() * 0.9);
                u->setWeight(newW);

                if (Infantry *inf = dynamic_cast<Infantry *>(u))
                {
                    inf->computeInitialAttackScore(); // ⚠️ Cập nhật lại EXP
                }
            }
            updateScore(true);
        }
    }
    else
    {
        this->LF = safeCeil(this->LF * 1.3);
        this->EXP = safeCeil(this->EXP * 1.3);
        checkRange(EXP, 500);
        checkRange(LF, 1000);

        if (this->LF >= enemy->getLF() && this->EXP >= enemy->getEXP()) 
            return;
        

        if (this->LF < enemy->getLF() || this->EXP < enemy->getEXP())
        {
            for (Unit *u : unitList->getUnits())
            {
                if (!u) continue;
                int newQ = safeCeil(u->getQuantity() * 0.9);
                u->setQuantity(newQ);
            }
            this->updateScore(true);
            return;
        }

        while (this->LF < enemy->getLF() && this->EXP < enemy->getEXP())
        {
            for (Unit *u : unitList->getUnits())
            {
                if (!u)
                    continue;
                u->setQuantity(nearestFibonacci(u->getQuantity()));
                if (Infantry *inf = dynamic_cast<Infantry *>(u))
                {
                    inf->computeInitialAttackScore();
                }
            }
            this->updateScore(true);

            this->LF = safeCeil(this->LF * 1.3);
            this->EXP = safeCeil(this->EXP * 1.3);
            checkRange(EXP, 500);
            checkRange(LF, 1000);

            // ĐIỀU KIỆN DỪNG CHÍNH XÁC: 
            // Dừng ngay khi ĐẠT HOẶC VƯỢT QUA enemy's scores
            if (this->LF >= enemy->getLF() && this->EXP >= enemy->getEXP()) {
                break;
            }
        }
        if (this->LF >= enemy->getLF() && this->EXP >= enemy->getEXP()) 
            return;

        if (this->LF < enemy->getLF() || this->EXP < enemy->getEXP())
        {
            for (Unit *u : unitList->getUnits())
            {
                if (!u)
                    continue;
                int newQ = safeCeil(u->getQuantity() * 0.9);
                u->setQuantity(newQ);
            }
            this->updateScore(true);
            return;
        }
    }
}

void LiberationArmy::updateScore(bool isInit)
{
    int totalLF = 0, totalEXP = 0;
    vector<Unit *> units = unitList->getUnits();
    for (int i = 0; i < (int)units.size(); ++i)
    {
        Unit *u = units[i];
        if (Infantry *inf = dynamic_cast<Infantry *>(u))
        {
            inf->computeInitialAttackScore();
            totalEXP += inf->AttackScore();
        }
        else if (Vehicle *v = dynamic_cast<Vehicle *>(u))
        {
            v->computeAttackScore();
            totalLF += u->getAttackScore();
        }
    }

    this->LF = checkRange(totalLF, 1000);
    this->EXP = checkRange(totalEXP, 500);

    if (isInit)
    {
        this->initialLF = LF;
        this->initialEXP = EXP;
    }
}

string LiberationArmy::getName() const
{
    return this->name;
}

string LiberationArmy::str() const
{
    ostringstream oss;
    oss << "LiberationArmy[LF=" << LF
        << ",EXP=" << EXP
        << ",unitList=" << unitList->str()
        << ",battleField=" << (battleField ? battleField->str() : "")
        << "]";
    return oss.str();
}

int LiberationArmy::nearestFibonacci(int value)
{
    if (value <= 1) return 2; // Thay vì return 1
    
    int a = 1, b = 1;
    while (b <= value) // Thay vì b < value
    {
        int t = a + b;
        a = b;
        b = t;
    }
    return b; // Trả về fibonacci TIẾP THEO lớn hơn value
}

int LiberationArmy::scoreSum(const vector<Unit *> &vec)
{
    int s = 0;
    for (int i = 0; i < (int)vec.size(); ++i)
        s += vec[i]->getAttackScore();
    return s;
}

vector<Unit *> LiberationArmy::knapsack(vector<Unit *> units, int minScore)
{
    int n = units.size();
    int bestSum = 1e9;
    vector<Unit *> bestSet;

    // Duyệt tất cả tổ hợp
    for (int mask = 1; mask < (1 << n); ++mask)
    {
        vector<Unit *> subset;
        int total = 0;

        for (int i = 0; i < n; ++i)
        {
            if (mask & (1 << i))
            {
                subset.push_back(units[i]);
                total += units[i]->getAttackScore();
            }
        }

        if (total > minScore && total < bestSum)
        {
            bestSum = total;
            bestSet = subset;
        }
    }
    return bestSet;
}

Unit *LiberationArmy::cloneUnit(Unit *unit)
{
    if (Vehicle *v = dynamic_cast<Vehicle *>(unit))
    {
        return new Vehicle(v->getQuantity(), v->getWeight(), v->getCurrentPosition(), v->getVehicleType());
    }
    if (Infantry *i = dynamic_cast<Infantry *>(unit))
    {
        return new Infantry(i->getQuantity(), i->getWeight(), i->getCurrentPosition(), i->getInfantryType());
    }
    return nullptr;
}

LiberationArmy::~LiberationArmy()
{
    // Nothing to free manually since base Army destructor will handle it
}

//!----------------------------------------------
//! CLASS ARVN
//!----------------------------------------------
ARVN::ARVN(Unit **unitArray, int size, string name, BattleField *battlefield)
    : Army(unitArray, size, name, battlefield)
{}

int ARVN::safeCeil(double val)
{
    return (int)(val == (int)val ? val : (int)(val + 1));
}

void ARVN::fight(Army* enemy, bool defense) {
    if (!enemy) return;
    
    int myLF = LF;
    int myEXP = EXP;
    int enemyLF = enemy->getLF();
    int enemyEXP = enemy->getEXP();

    if (!defense) {
        // Khi tấn công, luôn thất bại, giảm 20% quantity
        vector<Unit*> units = unitList->getUnits();
        for (int i = 0; i < (int)units.size(); ++i) {
            Unit* u = units[i];
            int lost = safeCeil(u->getQuantity() * 0.8);
            u->setQuantity(lost);
            if (u->getQuantity() <= 1) unitList->remove(u);
        }
        updateScore(true);
    } else {
        if(enemy->getLiberWin()){
            vector<Unit*> units = unitList->getUnits();
            for (int i = 0; i < (int)units.size(); ++i) {
                Unit* u = units[i];
                int lostW = safeCeil(u->getWeight() * 0.2);
                u->setWeight(u->getWeight() - lostW);
            }
            updateScore(true);
        }
    }

    // Cập nhật điểm tấn công
    // updateScore(true);
    // enemy->updateScore(true);
}

string ARVN::getName() const
{
    return this->name;
}

string ARVN::str() const
{
    std::ostringstream oss;
    oss << "ARVN[LF=" << LF
        << ",EXP=" << EXP
        << ",unitList=" << unitList->str()
        << ",battleField=" << (battleField ? battleField->str() : "")
        << "]";
    return oss.str();
}

void ARVN::updateScores(bool isInit)
{
    vector<Unit *> units = unitList->getUnits();
    int totalLF = 0;
    int totalEXP = 0;

    for (int i = 0; i < (int)units.size(); ++i)
    {
        Unit *u = units[i];
        if (Infantry *inf = dynamic_cast<Infantry *>(u))
        {
            totalEXP += inf->AttackScore();
        }
        else
        {
            totalLF += u->getAttackScore();
        }
    }

    this->LF = min(1000, totalLF);
    this->EXP = min(500, totalEXP);

    if (isInit)
    {
        this->initialLF = LF;
        this->initialEXP = EXP;
    }
}

ARVN::~ARVN()
{
    // Nothing to free manually since base Army destructor will handle it
}

//!-----------------------------------------------------
//! Lớp TerrainElement và các lớp con
//!-----------------------------------------------------

// Constructor và Destructor của TerrainElement
TerrainElement::TerrainElement(Position pos) : pos(pos)
{
}

TerrainElement::~TerrainElement()
{
    // Giải phóng tài nguyên nếu có
}

// Road: không có hiệu ứng
void Road::getEffect(Army *army)
{
    if (!army)
        return;
    // Không gây ảnh hưởng cho quân đội.
}

string Road::str() const
{
    return "Road";
}

// Mountain: stub – áp dụng hiệu ứng cho các đơn vị trong bán kính ảnh hưởng (theo đề bài, hiệu ứng có thể là:
// tăng EXP của bộ binh và giảm LF của phương tiện, tùy thuộc vào khoảng cách)
void Mountain::getEffect(Army *army)
{
    if (!army) return;
    bool isLiberationArmy = dynamic_cast<LiberationArmy*>(army) != nullptr;
    auto units = army->getUnitList()->getUnits();

    int bonus = 0;
    int penalty = 0;

    for (auto& unit : units) {
        double d = calculateDistance(this->pos, unit->getCurrentPosition());
        if (Infantry *inf = dynamic_cast<Infantry*>(unit)) {
            int originalScore = inf->AttackScore();
            // Kiểm tra nếu đơn vị nằm trong vùng ảnh hưởng
            if ((isLiberationArmy && d <= 2) || (!isLiberationArmy && d <= 4)) {
                double reductionRate = isLiberationArmy ? 0.3 : 0.2;
                double reduction = originalScore * reductionRate;
                
                // Trừ đi giá trị bị ảnh hưởng từ EXP hiện tại
                bonus += safeCeil(reduction);
            }
        } else if (Vehicle* vehicle = dynamic_cast<Vehicle*>(unit)) {
            int originalScore = vehicle->getAttackScore();
                        
            // Kiểm tra nếu đơn vị nằm trong vùng ảnh hưởng
            if ((isLiberationArmy && d <= 2) || (!isLiberationArmy && d <= 4)) {
                double reductionRate = isLiberationArmy ? 0.1 : 0.05;
                double reduction = originalScore * reductionRate;
                
                // Trừ đi giá trị bị ảnh hưởng từ LF hiện tại
                penalty -= safeCeil(reduction);
            }
        }
    }
    int LF = army->getLF();
    int EXP = army->getEXP();
    LF += penalty;
    EXP += bonus;
    LF = max(0, min(1000, LF));
    EXP = max(0, min(500, EXP));
    army->setLF(LF);
    army->setEXP(EXP);
    // cout << LF << " " << EXP << endl;
}

string Mountain::str() const
{
    return "Mountain";
}

// River: stub – gi?m EXP c?a l?c lu?ng b? binh 10% n?u n?m trong bán kính 2 don v?
void River::getEffect(Army *army)
{
    if (!army)
        return;
    auto units = army->getUnitList()->getUnits();

    for (auto &unit : units)
    {
        if (!unit)
            continue;
        double d = calculateDistance(this->pos, unit->getCurrentPosition());
        if (d <= 2)
        {
            if (Infantry *inf = dynamic_cast<Infantry *>(unit))
            {
                int penalty = safeCeil(- inf->getAttackScore() * 0.1);
                inf->setAttackScore(inf->getAttackScore() + penalty);
                // int penalty = safeCeil(inf->getAttackScore() * 0.1);
                // inf->setAttackScore(inf->getAttackScore() - penalty);
            }
        }
    }
}

string River::str() const
{
    return "River";
}

// Urban: stub – hi?u ?ng khu dân cu có th? thay d?i attackScore c?a b? binh ho?c phuong ti?n tùy vào lo?i
void Urban::getEffect(Army *army)
{
    if (!army)
        return;
    bool isLiberationArmy = dynamic_cast<LiberationArmy *>(army) != nullptr;
    auto units = army->getUnitList()->getUnits();

    for (auto &unit : units)
    {
        if (!unit)
            continue;
        double d = calculateDistance(this->pos, unit->getCurrentPosition());
        int score = unit->getAttackScore();

        if (Infantry *inf = dynamic_cast<Infantry *>(unit))
        {
            if ((inf->getInfantryType() == SPECIALFORCES || inf->getInfantryType() == REGULARINFANTRY))
            {
                if (isLiberationArmy && d > 0 && d <= 5)
                {
                    int bonus = safeCeil((2.0 * score) / d);
                    inf->setAttackScore(score + bonus);
                }
                else if (!isLiberationArmy && inf->getInfantryType() == REGULARINFANTRY && d > 0 && d <= 3)
                {
                    int bonus = safeCeil((3.0 * score) / (2.0 * d));
                    inf->setAttackScore(score + bonus);
                }
            }
        }
        else if (Vehicle *v = dynamic_cast<Vehicle *>(unit))
        {
            if (isLiberationArmy && v->getVehicleType() == ARTILLERY && d > 0 && d <= 2)
            {
                int penalty = safeCeil(- score * 0.5);
                v->setAttackScore(score + penalty);
                // int penalty = safeCeil(score * 0.5);
                // v->setAttackScore(score - penalty);
            }
        }
    }
}

string Urban::str() const
{
    return "Urban";
}

// Fortification: stub – hi?u ?ng chi?n hào, áp d?ng cho c? hai quân d?i v?i các thay d?i nh?t d?nh
void Fortification::getEffect(Army *army)
{
    if (!army)
        return;
    bool isLiberationArmy = dynamic_cast<LiberationArmy *>(army) != nullptr;
    auto units = army->getUnitList()->getUnits();

    for (auto &unit : units)
    {
        if (!unit)
            continue;
        double d = calculateDistance(this->pos, unit->getCurrentPosition());
        int score = unit->getAttackScore();

        if (d <= 2)
        {
            if (Infantry *inf = dynamic_cast<Infantry *>(unit))
            {
                int delta = safeCeil((score * 0.2));
                if (isLiberationArmy)
                    inf->setAttackScore(score + safeCeil(- score * 0.2));
                    // inf->setAttackScore(score - delta);
                else
                    inf->setAttackScore(score + delta);
            }
            else if (Vehicle *v = dynamic_cast<Vehicle *>(unit))
            {
                int delta = safeCeil((score * 0.2));
                if (isLiberationArmy)
                    v->setAttackScore(score + safeCeil(- score * 0.2));
                    // v->setAttackScore(score - delta);
                else
                    v->setAttackScore(score + delta);
            }
        }
    }
}

string Fortification::str() const
{
    return "Fortification";
}

// SpecialZone: stub – các don v? trong bán kính 1 don v? có attackScore = 0
void SpecialZone::getEffect(Army *army)
{
    if (!army)
        return;
    auto units = army->getUnitList()->getUnits();

    for (auto &unit : units)
    {
        if (!unit)
            continue;
        double d = calculateDistance(this->pos, unit->getCurrentPosition());
        if (d <= 1)
        {
            unit->setAttackScore(0);
        }
    }
}

string SpecialZone::str() const
{
    return "SpecialZone";
}

// Road
TerrainElement *Road::clone() const
{
    return new Road(this->pos);
}

TerrainElement *Mountain::clone() const
{
    return new Mountain(this->pos);
}

TerrainElement *River::clone() const
{
    return new River(this->pos);
}

TerrainElement *Fortification::clone() const
{
    return new Fortification(this->pos);
}

TerrainElement *Urban::clone() const
{
    return new Urban(this->pos);
}

TerrainElement *SpecialZone::clone() const
{
    return new SpecialZone(this->pos);
}

//!----------------------------------------------
//! CLASS BattleField
//!----------------------------------------------
BattleField::BattleField(int n_rows, int n_cols, const vector<Position *> &arrayForest,
                         const vector<Position *> &arrayRiver, const vector<Position *> &arrayFortification,
                         const vector<Position *> &arrayUrban, const vector<Position *> &arraySpecialZone)
    : n_rows(n_rows), n_cols(n_cols)
{
    // Initialize battlefield grid with nullptr
    grid.resize(n_rows, vector<TerrainElement *>(n_cols, nullptr));

    for (int r = 0; r < n_rows; ++r)
    {
        for (int c = 0; c < n_cols; ++c)
        {
            grid[r][c] = new Road(Position(r, c));
        }
    }

    auto apply = [&](const vector<Position *> &arr, TerrainElement *prototype)
    {
        for (Position *p : arr)
        {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols)
            {
                delete grid[r][c];               // xóa cũ
                grid[r][c] = prototype->clone(); // gán bản sao mới
                grid[r][c]->setPosition(Position(r, c));
            }
        }
        delete prototype;
    };

    apply(arrayForest, new Mountain(Position()));
    apply(arrayRiver, new River(Position()));
    apply(arrayFortification, new Fortification(Position()));
    apply(arrayUrban, new Urban(Position()));
    apply(arraySpecialZone, new SpecialZone(Position()));
}

TerrainElement *BattleField::getElement(int r, int c) const
{
    if (grid.empty())
    {
        return nullptr;
    }
    if (r < 0 || r >= (int)grid.size())
    {
        return nullptr;
    }

    if (c < 0 || c >= (int)grid[r].size())
    {
        return nullptr;
    }

    if (!grid[r][c])
    {
        return nullptr;
    }
    return grid[r][c];
}

string BattleField::str() const
{
    stringstream ss;
    ss << "BattleField[n_rows=" << n_rows << ",n_cols=" << n_cols << "]";
    return ss.str();
}

BattleField::~BattleField()
{
    for (int i = 0; i < n_rows; ++i)
    {
        for (int j = 0; j < n_cols; ++j)
        {
            delete grid[i][j];
        }
    }
    grid.clear();
}

//!-----------------------------------------------------
//! CLASS Configuration
//!-----------------------------------------------------
Configuration::Configuration(const string &filepath)
{
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        throw 1;
    }

    string line, fullUnitList;
    vector<string> lines;

    while (getline(fin, line))
    {
        line = trim(line);
        if (!line.empty())
            lines.push_back(line);
    }

    eventCode = 0;

    for (string line : lines)
    {
        if (line.find("NUM_ROWS=") == 0)
            num_rows = stoi(trim(line.substr(9)));
        else if (line.find("NUM_COLS=") == 0)
            num_cols = stoi(trim(line.substr(9)));
        else if (line.find("EVENT_CODE=") == 0)
        {
            string codeStr = trim(line.substr(line.find('=') + 1));
            try
            {
                int ec = stoi(codeStr);
                eventCode = (ec < 0) ? 0 : (ec % 100);
            }
            catch (...)
            {
                eventCode = 0;
            }
        }
    }

    auto parseArray = [&](const string &key, vector<Position *> &arr)
    {
        for (string line : lines)
        {
            if (line.find(key + "=") == 0)
            {
                int start = line.find('['), end = line.find(']');
                if (start == string::npos || end == string::npos)
                    return;
                stringstream ss(line.substr(start + 1, end - start - 1));
                string token;
                while (getline(ss, token, ')'))
                {
                    int p = token.find('(');
                    if (p != string::npos)
                    {
                        string coords = token.substr(p + 1);
                        stringstream cs(coords);
                        string x, y;
                        if (getline(cs, x, ',') && getline(cs, y))
                        {
                            arr.push_back(new Position(stoi(trim(x)), stoi(trim(y))));
                        }
                    }
                }
            }
        }
    };

    parseArray("ARRAY_FOREST", arrayForest);
    // for (int i = 0; i < arrayForest.size(); i++) cout << arrayForest[i]->str() << endl;
    parseArray("ARRAY_RIVER", arrayRiver);
    parseArray("ARRAY_FORTIFICATION", arrayFortification);
    parseArray("ARRAY_URBAN", arrayUrban);
    parseArray("ARRAY_SPECIAL_ZONE", arraySpecialZone);

    for (string line : lines)
    {
        if (line.find("UNIT_LIST=") == 0)
        {
            fullUnitList = line;
            while (fullUnitList.find(']') == string::npos && getline(fin, line))
            {
                fullUnitList += trim(line);
            }
            break;
        }
    }

    int start = fullUnitList.find('['), end = fullUnitList.find(']');
    if (start != string::npos && end != string::npos)
    {
        string listContent = fullUnitList.substr(start + 1, end - start - 1);
        vector<string> units = splitUnits(listContent);
        for (const string &entry : units)
        {
            size_t open = entry.find('(');
            size_t close = entry.rfind(')');
            if (open == string::npos || close == string::npos || close <= open)
                continue;

            string unitType = trim(entry.substr(0, open));
            string inside = entry.substr(open + 1, close - open - 1); // "1,1,(x,y),belong"

            stringstream iss(inside);
            string qStr, wStr, posStr, belongStr;
            getline(iss, qStr, ',');
            getline(iss, wStr, ',');
            getline(iss, posStr, ')');
            getline(iss, belongStr);

            if (!posStr.empty() && posStr[0] == '(')
                posStr.erase(0, 1);
            if (belongStr.length() > 0 && belongStr[0] == ',')
                belongStr.erase(0, 1);

            stringstream ps(posStr);
            string xStr, yStr;
            getline(ps, xStr, ',');
            getline(ps, yStr);

            int quantity = stoi(trim(qStr));
            int weight = stoi(trim(wStr));
            int x = stoi(trim(xStr));
            int y = stoi(trim(yStr));
            int army = stoi(trim(belongStr));

            Position pos(x, y);
            Unit *u = nullptr;
            bool isInfantry = true;
            bool validType = true;

            InfantryType iType;
            VehicleType vType;

            if (unitType == "SNIPER")
                iType = SNIPER;
            else if (unitType == "ANTIAIRCRAFTSQUAD")
                iType = ANTIAIRCRAFTSQUAD;
            else if (unitType == "MORTARSQUAD")
                iType = MORTARSQUAD;
            else if (unitType == "ENGINEER")
                iType = ENGINEER;
            else if (unitType == "SPECIALFORCES")
                iType = SPECIALFORCES;
            else if (unitType == "REGULARINFANTRY")
                iType = REGULARINFANTRY;
            else if (unitType == "TRUCK")
            {
                isInfantry = false;
                vType = TRUCK;
            }
            else if (unitType == "MORTAR")
            {
                isInfantry = false;
                vType = MORTAR;
            }
            else if (unitType == "ANTIAIRCRAFT")
            {
                isInfantry = false;
                vType = ANTIAIRCRAFT;
            }
            else if (unitType == "ARMOREDCAR")
            {
                isInfantry = false;
                vType = ARMOREDCAR;
            }
            else if (unitType == "APC")
            {
                isInfantry = false;
                vType = APC;
            }
            else if (unitType == "ARTILLERY")
            {
                isInfantry = false;
                vType = ARTILLERY;
            }
            else if (unitType == "TANK")
            {
                isInfantry = false;
                vType = TANK;
            }
            else
                validType = false;

            if (!validType)
                continue;

            u = isInfantry
                    ? static_cast<Unit *>(new Infantry(quantity, weight, pos, iType))
                    : static_cast<Unit *>(new Vehicle(quantity, weight, pos, vType));
            if (u)
            {
                if (army == 0)
                    this->liberationUnits.push_back(u);
                else if (army == 1)
                    this->ARVNUnits.push_back(u);
                else
                {
                    delete u;
                    continue;
                }
            }
        }
        liberationUnitArray = new Unit *[liberationUnits.size()];
        for (size_t i = 0; i < liberationUnits.size(); ++i)
            liberationUnitArray[i] = liberationUnits[i];
        ARVNUnitArray = new Unit *[ARVNUnits.size()];
        for (size_t i = 0; i < ARVNUnits.size(); ++i)
            ARVNUnitArray[i] = ARVNUnits[i];
    }
}

string Configuration::str() const
{
    stringstream ss;
    ss << "["
       << "num_rows=" << num_rows
       << ",num_cols=" << num_cols
       << ",arrayForest=" << positionArrayToString(arrayForest)
       << ",arrayRiver=" << positionArrayToString(arrayRiver)
       << ",arrayFortification=" << positionArrayToString(arrayFortification)
       << ",arrayUrban=" << positionArrayToString(arrayUrban)
       << ",arraySpecialZone=" << positionArrayToString(arraySpecialZone)
       << ",liberationUnits=" << unitListToString(liberationUnits)
       << ",ARVNUnits=" << unitListToString(ARVNUnits)
       << ",eventCode=" << eventCode
       << "]";
    return ss.str();
}

Configuration::~Configuration()
{
    // Giải phóng các đối tượng Position đã được cấp phát
    for (auto p : arrayForest)
        delete p;
    for (auto p : arrayRiver)
        delete p;
    for (auto p : arrayFortification)
        delete p;
    for (auto p : arrayUrban)
        delete p;
    for (auto p : arraySpecialZone)
        delete p;

    for (auto u : liberationUnits)
        delete u;
    for (auto u : ARVNUnits)
        delete u;
    delete[] liberationUnitArray;
    delete[] ARVNUnitArray;
}

//!-----------------------------------------------------
//! Lớp HCMCampaign
//!-----------------------------------------------------
HCMCampaign::HCMCampaign(const string &config_file_path)
{
    config = new Configuration(config_file_path);
    battleField = new BattleField(
        config->getNumRows(),
        config->getNumCols(),
        config->getForestPositions(),
        config->getRiverPositions(),
        config->getFortificationPositions(),
        config->getUrbanPositions(),
        config->getSpecialZonePositions());


    const vector<Unit *> &liberUnits = config->getLiberationUnits();
    const vector<Unit *> &arvnUnits = config->getARVNUnits();

    liberationArmy = (liberUnits.empty() ? new LiberationArmy(nullptr, 0, "LIBERATIONARMY", battleField)
                                         : new LiberationArmy(config->getLiberationUnitArray(), config->getLiberSize(), "LIBERATIONARMY", battleField));
    arvnArmy = (arvnUnits.empty() ? new ARVN(nullptr, 0, "ARVN", battleField)
                                  : new ARVN(config->getARVNUnitArray(), config->getARVNSize(), "ARVN", battleField));

}

void HCMCampaign::run()
{

    if (!liberationArmy || !arvnArmy || !battleField || !config)
        return;

    // cout << liberationArmy->str();
    // cout << arvnArmy->str(); 
    typedef bool (*FilterFunc)(TerrainElement *);
    FilterFunc filters[] = {
        [](TerrainElement *e) -> bool
        { return dynamic_cast<Road *>(e) != nullptr; },
        [](TerrainElement *e) -> bool
        { return dynamic_cast<Mountain *>(e) != nullptr; },
        [](TerrainElement *e) -> bool
        { return dynamic_cast<River *>(e) != nullptr; },
        [](TerrainElement *e) -> bool
        { return dynamic_cast<Urban *>(e) != nullptr; },
        [](TerrainElement *e) -> bool
        { return dynamic_cast<Fortification *>(e) != nullptr; },
        [](TerrainElement *e) -> bool
        { return dynamic_cast<SpecialZone *>(e) != nullptr; }};

    int num_rows = config->getNumRows();
    int num_cols = config->getNumCols();
    for (int f = 0; f < 6; ++f)
    {
        for (int i = 0; i < num_rows; ++i)
        {
            for (int j = 0; j < num_cols; ++j)
            {
                TerrainElement *element = battleField->getElement(i, j);
                if (element && filters[f](element))
                {
                    element->getEffect(liberationArmy);
                    element->getEffect(arvnArmy);
                }
            }
        }
    }

    int code = config->getEventCode();

    if (code < 75)
    {
        liberationArmy->fight(arvnArmy, false);
        arvnArmy->fight(liberationArmy, true);
    }
    else
    {
        arvnArmy->fight(liberationArmy, false);
        liberationArmy->fight(arvnArmy, true);
        liberationArmy->fight(arvnArmy, false);
        arvnArmy->fight(liberationArmy, true);
    }

    auto cleanup = [](Army *army)
    {
        vector<Unit *> units = army->getUnitList()->getUnits();
        for (Unit *u : units) {
            // cout << u->getAttackScore() << endl;
            if (u && u->getAttackScore() <= 5)
                army->getUnitList()->remove(u);

        }
        army->updateScore(true);
    };

    cleanup(liberationArmy);
    cleanup(arvnArmy);
}

string HCMCampaign::printResult()
{
    stringstream ss;
    ss << "LIBERATIONARMY[LF=" << liberationArmy->getLF()
       << ",EXP=" << liberationArmy->getEXP() << "]"
       << "-ARVN[LF=" << arvnArmy->getLF()
       << ",EXP=" << arvnArmy->getEXP() << "]";
    return ss.str();
}

HCMCampaign::~HCMCampaign()
{
    if (config != 0)
        delete config;
    delete battleField;
    delete liberationArmy;
    delete arvnArmy;
}
