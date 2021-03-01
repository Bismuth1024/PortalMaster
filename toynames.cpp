#include "toynames.h"
using namespace std;

map <string, uint16_t> toyNames;

uint16_t nToys = 0x65;
const string names[] = {
						"Whirlwind", "Sonic Boom", "Warnado", "Lightning Rod",
						"Bash", "Terrafin", "Dino Rang", "Prism Break", 
						"Sunburn", "Eruptor", "Ignitor", "Flameslinger", 
						"Zap", "Wham Shell", "Gill grunt", "Slam Bam", 
						"Spyro", "Voodood", "Double Trouble", 
						"Trigger Happy", "Drobot", "Drill Sergeant", "Boomer", 
						"Wrecking Ball", 
						"Camo", "Zook", "Stealth Elf", "Stump Smash", 
						"Dark Spyro", 
						"Hex", "Chop Chop", "Ghost Roaster", "Cynder",
						
						"Jet Vac", "Swarm",
						"Crusher", "Flashwing",
						"Hot Head", "Hot Dog",
						"Chill", "Thumpback",
						"Pop Fizz", "Ninjini",
						"Bouncer", "Sprocket", 
						"Tree Rex", "Shroomboom", 
						"Eye Brawl", "Fright Rider",
						
						"Scratch", "Pop Thorn", 
						"Slobber Tooth", "Scorp", 
						"Fryno", "Smolderdash", 
						"Bumble Blast", "Zoo Lou", 
						"Dune Bug", "Star Strike", 
						"Countdown", "Wind Up", 
						"Roller Brawl", "Grim Creeper", 
						"Rip Tide", "Punk Shock",
						
						"Gusto", "Thunderbolt", "Fling Kong", "Blades",
						"Wallop", "Head Rush", "Fist Bump", "Rocky Roll",
						"Wildfire", "Ka-Boom", "Trail Blazer", "Torch", 
						"Snap Shot", "Lob Star", "Flip Wreck", "Echo", 
						"Blastermind", "Enigma", "Deja Vu", "Cobra Cadabra", 
						"Jawbreaker", "Gearshift", "Chopper", "Tread Head", 
						"Bushwhack", "Tuff Luck", "Food Fight", "High Five", 
						"Krypt King", "Short Cut", "Bat Spin", "Funny Bone", 
						"Knight Light", "Spotlight", 
						"Knight Mare", "Blackout",
						};
						
uint16_t codes[0x500]; 

void loadNames() {

	fillCodes(codes, 0x0000, 0x21);
	fillCodes(codes + 0x21, 0x0064, 0x10);
	fillCodes(codes + 0x31, 0x0bb8, 0x10);
	fillCodes(codes + 0x41, 0x01c2, 0x24);
	//0d53
	
	

	for (uint16_t i = 0; i < nToys; i++) {
		toyNames.insert(pair<string, uint16_t>(names[i], codes[i]));
	}	
	
	toyNames.insert(pair<string, uint16_t>("Fiesta", 0x0d48));
	toyNames.insert(pair<string, uint16_t>("High Volt", 0x0d49));
	toyNames.insert(pair<string, uint16_t>("Splat", 0x0d4a));
	//???
	toyNames.insert(pair<string, uint16_t>("Stormblade", 0x0d4e));
	//???
	toyNames.insert(pair<string, uint16_t>("Smash Hit", 0x0d53));
	toyNames.insert(pair<string, uint16_t>("Spitfire", 0x0d54));
	toyNames.insert(pair<string, uint16_t>("Hurricane Jet-Vac", 0x0d55));
	toyNames.insert(pair<string, uint16_t>("Double Dare Trigger Happy", 0x0d56));
	toyNames.insert(pair<string, uint16_t>("Super Shot Stealth Elf", 0x0d57));
	toyNames.insert(pair<string, uint16_t>("Shark Shooter Terrafin", 0x0d58));
	toyNames.insert(pair<string, uint16_t>("Bone Bash Roller Brawl", 0x0d59));
	//???
	toyNames.insert(pair<string, uint16_t>("Big Bubble Pop Fizz", 0x0d5c));
	toyNames.insert(pair<string, uint16_t>("Lava Lance Eruptor", 0x0d5d));
	toyNames.insert(pair<string, uint16_t>("Deep Dive Gill Grunt", 0x0d5e));
	toyNames.insert(pair<string, uint16_t>("Turbo Charge Donkey Kong", 0x0d5f));
	toyNames.insert(pair<string, uint16_t>("Hammer Slam Bowser", 0x0d60));
	toyNames.insert(pair<string, uint16_t>("Dive-Clops", 0x0d61));
	toyNames.insert(pair<string, uint16_t>("Astroblast", 0x0d62));
	toyNames.insert(pair<string, uint16_t>("Nightfall", 0x0d63));
	toyNames.insert(pair<string, uint16_t>("Thrillipede", 0x0d64));
	
	
	
	
	
	
	toyNames.insert(pair<string, uint16_t>("Jet Stream", 0x0c94));
	toyNames.insert(pair<string, uint16_t>("Tomb Buggy", 0x0c95));
	toyNames.insert(pair<string, uint16_t>("Reef Ripper", 0x0c96));
	toyNames.insert(pair<string, uint16_t>("Burn-Cycle", 0x0c97));	
	toyNames.insert(pair<string, uint16_t>("Hot Streak", 0x0c98));
	toyNames.insert(pair<string, uint16_t>("Shark Tank", 0x0c99));	
	toyNames.insert(pair<string, uint16_t>("Thump Truck", 0x0c9a));
	toyNames.insert(pair<string, uint16_t>("Crypt Crusher", 0x0c9b));
	toyNames.insert(pair<string, uint16_t>("Stealth Stinger", 0x0c9c));
	//???
	toyNames.insert(pair<string, uint16_t>("Dive Bomber", 0x0c9f));
	toyNames.insert(pair<string, uint16_t>("Sky Slicer", 0x0ca0));
	toyNames.insert(pair<string, uint16_t>("Clown Cruiser", 0x0ca1));
	toyNames.insert(pair<string, uint16_t>("Gold Rusher", 0x0ca2));
	toyNames.insert(pair<string, uint16_t>("Shield Striker", 0x0ca3));
	toyNames.insert(pair<string, uint16_t>("Sun Runner", 0x0ca4));
	toyNames.insert(pair<string, uint16_t>("Sea Shadow", 0x0ca5));
	toyNames.insert(pair<string, uint16_t>("Splatter Splasher", 0x0ca6));
	toyNames.insert(pair<string, uint16_t>("Soda Skimmer", 0x0ca7));
	toyNames.insert(pair<string, uint16_t>("Barrel Blaster", 0x0ca8));
	toyNames.insert(pair<string, uint16_t>("Buzz Wing", 0x0ca9));




	
	toyNames.insert(pair<string, uint16_t>("Kaos Trophy", 0x0daf));
	toyNames.insert(pair<string, uint16_t>("Land Trophy", 0x0dad));
	toyNames.insert(pair<string, uint16_t>("Sea Trophy", 0x0dae));
	toyNames.insert(pair<string, uint16_t>("Sky Trophy", 0x0dac));


	
}

void fillCodes(uint16_t* startPtr, uint16_t val, uint16_t n) {
	
	for (uint16_t i = 0; i < n; i++) {
		startPtr[i] = val++;
	}
}

uint16_t getCode(const char* name) {
	return (toyNames.find(name))->second;
}

string getCharName(uint16_t code) {
	for (map<string, uint16_t>::iterator i = toyNames.begin(); i != toyNames.end(); i++) {
		if (i->second == code) return (i->first);
	}
	
	return "Unknown";
	


}



