// 9504853406CBAC39EE89AA3AD238AA12CA198043

// EECS 281, Project 2A - The Walking Deadline

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <queue> //for PQ
#include "P2random.h"
#include "xcode_redirect.hpp"
using namespace std;

class Zombie
{
    public:
    Zombie(string &zName, uint32_t zDist, uint32_t zSpeed, uint32_t zHealth, uint32_t zStartingRound)
      :name(zName), distance(zDist), speed(zSpeed), health(zHealth), startingRound(zStartingRound){
        numRounds = 0;
      } 

    string getName() const{
      return name;
    }

    uint32_t getDistance() const{
      return distance;
    }
    
    void setDistance(){
      distance -= min(distance, speed);
    }

    uint32_t getSpeed() const{
      return speed;
    }

    uint32_t getHealth() const{
      return health;
    }

    void attack(){
      health -= 1;
    }

    uint32_t getNumRoundsActive() const{
      return numRounds;
    }

    void setNumRoundsActive(uint32_t currentRound){
      numRounds = currentRound - static_cast<uint32_t>(startingRound) + 1;
    }

    uint32_t getETA() const{
      return distance / speed;
    }

    bool isAlive(){
      if(health != 0)
        return true;
      return false;
    }

    private:
    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t startingRound;
    uint32_t numRounds;
};

class ZombieCompare
{
  public: 
  bool operator() (Zombie const *z1, Zombie const *z2){//(opposite bc we want low is high priority)
    if(z1->getETA() < z2->getETA())
      return false;
    else if(z1->getETA() == z2->getETA()){
      if(z1->getHealth() < z2->getHealth())
        return false;
      else if(z1->getHealth() == z2->getHealth()){
        if(z1->getName() <= z2->getName())//TODO should this just be < ??
          return false; 
        return true;
      }
      return true;
    } 
    return true;
  }
};

struct lessActivityCompare
{
  public: 
  bool operator() (Zombie const *z1, Zombie const *z2){
    if(z1->getNumRoundsActive() < z2->getNumRoundsActive())
      return false;
    else if(z1->getNumRoundsActive() == z2->getNumRoundsActive()){
      if(z1->getName() < z2->getName())
        return false;
      return true;
    }
    return true;
  }
};

struct moreActivityCompare
{
  public: 
  bool operator() (Zombie const *z1, Zombie const *z2){
    if(z1->getNumRoundsActive() < z2->getNumRoundsActive())
      return true;
    else if(z1->getNumRoundsActive() == z2->getNumRoundsActive()){
      if(z1->getName() < z2->getName())
        return false;
      return true;
    }
    return false;
  }
};


// Process the command line; there is no return value, but the Options
// struct is passed by reference and is modified by this function to send
// information back to the calling function.
void getMode(int argc, char * argv[], uint32_t &numForStats, bool &isVerbose, bool &isStatistics, bool &isMedian) {
  // These are used with getopt_long()
  opterr = false; // Let us handle all error output for command line options
  int choice;
  int index = 0;
  option long_options[] = {
    { "verbose",  no_argument, nullptr, 'v'  },
    { "statistics", required_argument,       nullptr, 's'},
    { "median", no_argument,       nullptr, 'm'},
    { "help",  no_argument,       nullptr, 'h'  },
    { nullptr, 0,                 nullptr, '\0' }
  };  // long_options[]

  while ((choice = getopt_long(argc, argv, "hs:vm", long_options, &index)) != -1) {
    switch (choice) {
      case 'h':
        cout << "This program is to simulate a zombie apocalypse,\n";
        cout << "using prority queues to determine the run of the game.\n";
        cout << "It will print out statistics, if specified, to explain what occurs." << endl;
        exit(0);
        //case 'h'
      case 's':{
        string arg{optarg};
        numForStats = static_cast<u_int32_t>(stoi(arg, nullptr, 0));
        isStatistics = true;
        break;
      }// case 's'
      case 'v':
        isVerbose = true;
        break;
      // case 'v'
      case 'm':
        isMedian = true;
        break;
      //case 'm'
      default:
        cerr << "Error: invalid option" << endl;
        exit(1);
      }  // switch ..choice
  } // while
}  // getMode()

int main(int argc, char *argv[]) {
// This should be in all of your projects, speeds up I/O
ios_base::sync_with_stdio(false);

xcode_redirect(argc, argv);

uint32_t numForStats = 0;//if 0, didnt change ? ?TODO 
bool isVerbose = false;
bool isStatistics = false;
bool isMedian = false; 

//Options options;
getMode(argc, argv, numForStats, isVerbose, isStatistics, isMedian);

uint32_t quiverCapacity;
uint32_t randomSeed;
uint32_t maxRandDist;
uint32_t maxRandSpeed;
uint32_t maxRandHealth;
priority_queue<Zombie*, vector<Zombie*>, ZombieCompare> zombiesToKill;
deque<Zombie> myZombies; //has the zombies in order of created and adding is constant
bool isAlive = true;
bool hasWon = false;
//bool defeatedAll = false;
uint32_t numArrows;
uint32_t counter = 1;
//bool roundUsed = true; //first round needs data
string nameLastZomb;//either killer or last zombie killed
priority_queue<uint32_t> lowerTime; //lowest number is at top
priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> higherTime;
priority_queue<Zombie*, vector<Zombie*>, lessActivityCompare> leastActiveZ;
priority_queue<Zombie*, vector<Zombie*>, moreActivityCompare> mostActiveZ;
deque<Zombie*> orderZombieKilled;

string word;
uint32_t round = 1;
string junk;
string comment;
getline(cin, comment);

cin >> junk;
cin >> quiverCapacity;
cin >> junk;
cin >> randomSeed;
cin >> junk;
cin >> maxRandDist;
cin >> junk;
cin >> maxRandSpeed;
cin >> junk;
cin >> maxRandHealth;

P2random::initialize(randomSeed, maxRandDist, maxRandSpeed, maxRandHealth);

cin >> junk; //"---"
cin >> junk; //"r"
cin >> round; //round

while(isAlive && !hasWon){
  if(isVerbose){
    cout << "Round: " << counter << '\n';
  }
  //1 - refill quiver
  numArrows = quiverCapacity;
  //2 - move zombies closer and attack if they reached player, update movement on order created (update distance)
  for (Zombie &zomb : myZombies){ 
    if(zomb.isAlive()){
      zomb.setDistance();
      if(isVerbose)
        cout << "Moved: " << zomb.getName() << " (distance: " << zomb.getDistance() << ", speed: " << zomb.getSpeed() << ", health: " << zomb.getHealth() << ")" << '\n';
    }
    if(zomb.getDistance() == 0){
      isAlive = false;
      if(nameLastZomb.empty())
        nameLastZomb = zomb.getName();
    }
  }
  //if player was killed, game ends here
  if(!isAlive)
    break;//using counter for round that died
  //3 - new zombies appear
  if(!cin.fail()){//round was read
    if(counter == round){
      //random
      cin >> word;//rand-zombs:
      int randZombs;
      cin >> randZombs;
      for(int i = 0; i < randZombs; ++i){
        string name  = P2random::getNextZombieName();
        uint32_t distance = P2random::getNextZombieDistance();
        uint32_t speed    = P2random::getNextZombieSpeed();
	      uint32_t health   = P2random::getNextZombieHealth();
        Zombie temp = Zombie(name, distance, speed, health, counter);
        myZombies.push_back(temp);
        Zombie *ptrtemp = &myZombies.back();
        zombiesToKill.push(ptrtemp);
        if(isVerbose)
          cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")" << '\n';
      }
      //named
      cin >> word;//name-zombs:
      int namedZombs;
      cin >> namedZombs;
      for(int i = 0; i < namedZombs; ++i){
        string name;
        cin >> name;
        cin >> junk;//distance:
        uint32_t distance;
        cin >> distance;
        cin >> junk;//speed:
        uint32_t speed;
        cin >> speed;
        cin >> junk;//health:
	      uint32_t health;
        cin >> health;
        Zombie temp = Zombie(name, distance, speed, health, counter);
        myZombies.push_back(temp);
        Zombie *ptrtemp = &myZombies.back();
        zombiesToKill.push(ptrtemp);
        if(isVerbose)
          cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")" << '\n';
      }
      cin >> junk; //"---"
      cin >> junk; //"r"
      cin >> round; //round
    }
  }
  //player shoots zombies with arrows 
  while(numArrows > 0 && zombiesToKill.size() > 0){
      Zombie *ptrtemp = zombiesToKill.top();
      zombiesToKill.pop();
      ptrtemp->attack();
      numArrows--;
      if(!ptrtemp->isAlive()){
        if(isVerbose){
          cout << "Destroyed: " << ptrtemp->getName() << " (distance: " << ptrtemp->getDistance() << ", speed: " << ptrtemp->getSpeed() << ", health: " << ptrtemp->getHealth() << ")" << '\n';
        }
        orderZombieKilled.push_back(ptrtemp);
        ptrtemp->setNumRoundsActive(counter);
        mostActiveZ.push(ptrtemp);
        leastActiveZ.push(ptrtemp);

        uint32_t temp = ptrtemp->getNumRoundsActive();
        if(lowerTime.size() == 0)
          lowerTime.push(temp);
        else if(higherTime.size() == 0)
          higherTime.push(temp);

        else if(temp >= higherTime.top())
          higherTime.push(temp);
        else //Otherwise it is a lower number
          lowerTime.push(temp);

        if(higherTime.size() - lowerTime.size()==2){
          lowerTime.push(higherTime.top());
          higherTime.pop();    
        } else if(lowerTime.size() - higherTime.size() == 2){
          higherTime.push(lowerTime.top());
          lowerTime.pop(); 
        }
      }
      else
        zombiesToKill.push(ptrtemp);
  }

  if(isMedian && (lowerTime.size() > 0 || higherTime.size() > 0)){
    uint32_t median;
    if(lowerTime.size() == higherTime.size())
      median = (lowerTime.top() + higherTime.top()) / 2;
    else if(higherTime.size() > lowerTime.size())
      median = higherTime.top();
    else 
      median = lowerTime.top();
    cout << "At the end of round " << counter << ", the median zombie lifetime is " << median << '\n';
  }
  
  if(zombiesToKill.empty() && cin.fail()){
    hasWon = true;
    nameLastZomb = orderZombieKilled[orderZombieKilled.size() - 1]->getName();
  }
  
  if(hasWon)
    break;//then can use counter as round that won
  if(!hasWon)
    counter++;
}

for(auto &z: myZombies){
  Zombie *ztemp = &z;
    if(z.isAlive()){
      z.setNumRoundsActive(counter);
      mostActiveZ.push(ztemp);
      leastActiveZ.push(ztemp);
    }
}

//go to print required messages and stats  
if(hasWon && isAlive){
  cout << "VICTORY IN ROUND " << counter << "! " <<  nameLastZomb << " was the last zombie." << '\n';
}
if(!isAlive){
  cout << "DEFEAT IN ROUND " << counter << "! " << nameLastZomb << " ate your brains!" << '\n';
}

if(isStatistics){
  //TODO
  cout << "Zombies still active: " << zombiesToKill.size() << '\n';
  size_t num = 1;
  //first N
  cout << "First zombies killed:" << '\n';
  while(num <= numForStats){
    if(num <= orderZombieKilled.size())
      cout << orderZombieKilled[num - 1]->getName() << " " << num << '\n';
    else 
      break;
    num++;
  }
  num--;
  //last N
  cout << "Last zombies killed:" << '\n';
  size_t temp = 1;
  while(num >= 1){
      cout << orderZombieKilled[orderZombieKilled.size() - temp]->getName() << " " << num << '\n';
    num--;
    temp++;
  }
  num++;
  //mostActive Z
  cout << "Most active zombies:" << '\n';
  while(num <= numForStats && mostActiveZ.size() > 0){
      Zombie* temp = mostActiveZ.top();
      mostActiveZ.pop();
      cout << temp->getName() << " " << temp->getNumRoundsActive() << '\n';
    num++;
  }
  num--;
  //leastActive Z
  cout << "Least active zombies:" << '\n';
  while(num >= 1 && leastActiveZ.size() > 0){
      Zombie* temp = leastActiveZ.top();
      leastActiveZ.pop();
      cout << temp->getName() << " " << temp->getNumRoundsActive() << '\n';
    num--;
  }

}
return 0;
}  // main()
