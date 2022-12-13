#include "map.h"

#include <stdexcept>

Point Map::getEmptyPos() {
	/*
		Find empty spot in game map
	*/

	Point pos;
	do {
		pos = Point(rand()%width, rand()%height);
	} while (map.find(pos) != map.end());
	return pos;
}

Map::Map(int _width, int _height, std::string _playerTeam):
	width(_width), height(_height), playerTeam(_playerTeam) {
	/*
	   Generate new random map with dimensions width * height and player on team playerTeam
	*/

	int limit = (width*height)/15;

	// Place terrain
	for (int i=0; i<limit*3/2; i++) {
		Point terrainPos = getEmptyPos();

		if (rand()%2) {
			map[terrainPos] = new Tree(terrainPos);
		} else {
			map[terrainPos] = new Water(terrainPos);
		}
	}

	// Place werewolves and vampires
	for (int i=0; i<limit; i++) {
		Point vampPoint = getEmptyPos();
		Point werePoint = getEmptyPos();

		Vampire* vampire = new Vampire(vampPoint);
		map[vampPoint] = vampire;
		vampires++;

		Werewolf* werewolf = new Werewolf(werePoint);
		map[werePoint] = werewolf;
		werewolves++;
	}

	// Place player
	Point playerPos = getEmptyPos();
	player = new Player(playerPos, playerTeam);
	map[playerPos] = player;
}

bool Map::isValidPosition(Point position) {
	/*
	   Check if position is within the limits of the map
	*/

	return !(position.x < 0 || position.x >= width || position.y < 0 || position.y >= height);
}

std::vector<Point> Map::getPossibleMoves(Point position) {
	/*
	   Find all possible moves from position
	*/

	std::vector<Point> moves;

	moves.push_back(position);

	// Loop over all positions in Moore neighbourhood
	for (int i=0; i<9; i++) {
		Point newPos = position;
		newPos.x += (i%3)-1;
		newPos.y += (i/3)-1;

		// if position is out of bounds skip it
		if (!isValidPosition(newPos)) continue;

		// check if position is occupied
		try {
			map.at(newPos);
		}
		// if it's not put position in list of possible positions
		catch (std::out_of_range &e) {
			moves.push_back(newPos);
		}
	}

	return moves;
}

void Map::moveEntities() {
	/*
	   Move all the entities on the map, except for players
	*/

	std::vector<Entity*> entitiesToMove;

	// Make a list with all the entities that are going to move
	for (auto& it : map) {
		Object* o = it.second;
		if (!(typeid(*o) == typeid(Werewolf) || typeid(*o) == typeid(Vampire))) continue;

		Entity* e = (Entity*) o;

		entitiesToMove.push_back(e);
	}

	// For each entity that has to move, call the move method
	for (auto& e : entitiesToMove) {
		Point pos = e->getPos();
		Point newPos = e->move(getPossibleMoves(pos));

		// Move entity from old to new position
		map.erase(pos);
		map[newPos] = e;
	}
}

void Map::movePlayer(int direction) {
	if (PAUSED) return;

	Point playerPosition = player->getPos();
	Point oldPosition = playerPosition;

	switch (direction) {
		case NORTH:
			playerPosition.y -= 1;
			break;
		case SOUTH:
			playerPosition.y += 1;
			break;
		case EAST:
			playerPosition.x += 1;
			break;
		case WEST:
			playerPosition.x -= 1;
			break;
	}

	if (!isValidPosition(playerPosition)) return;

	try {
		map.at(playerPosition);
	} catch (std::exception &e) {
		std::vector<Point> moves = {playerPosition};
		player->move(moves);

		map.erase(oldPosition);
		map[playerPosition] = player;
	}
}

std::unordered_map<Point, Object*> Map::getObjects() {
	/*
	   Return the hashmap of the objects on the game map
	*/

	return map;
}

std::vector<Entity*> Map::getEntities(Point position) {
	/*
	   Return a list of all the entities around a position on the map
	*/

	std::vector<Entity*> entities;

	for (int i=0; i<9; i++) {
		Point newPos = position;
		newPos.x += (i%3)-1;
		newPos.y += (i/3)-1;

		if (!isValidPosition(newPos)) continue;

		try {
			Entity* e = dynamic_cast<Entity*>(map.at(position));
			if (e != nullptr) entities.push_back(e);
		}
		catch (std::bad_cast &e) {
			continue;
		}
	}

	return entities;
}

bool isObjectNPC(Object* o) {
	return typeid(*o) == typeid(Werewolf) || typeid(*o) == typeid(Vampire);
}

void Map::interactEntities() {
	/*
	   Make entities interact with each other where possible
	*/

	// For each entity
	for (auto& it : map) {
		Object* o = it.second;

		if (!isObjectNPC(o)) continue;

		Entity* e = (Entity*) o;

		// Get entities inside Moore neighbourhood
		std::vector<Entity*> closeEntities = getEntities(e->getPos());

		// If there aren't any entities near by go to the next entity
		if (closeEntities.empty()) continue;

		// Execute encounters
		for (auto& other : closeEntities) {
			if (!e->interact(other)) continue;

			// If the other entity died remove it from the map
			if (other->getTeam() == "Werewolves") werewolves--;
			else vampires--;

			Point position = other->getPos();
			delete other;
			map.erase(position);
		}
	}
}

int Map::update() {
	/*
	   Update game map
	*/

	moveEntities();
	/* interactEntities(); */

	// Check if a team has no more members
	if (vampires < 0) {
		std::cout << "Werewolves win" << std::endl;
		return WEREWOLVES;
	} else if (werewolves < 0) {
		std::cout << "Vampires win" << std::endl;
		return VAMPIRES;
	}

	return 0;
}

void Map::print() {
	/*
	   Print game map
	*/

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			Point p = Point(j, i);

			// If position has object print it
			try {
				map.at(p)->print();
			}
			// Else print "." to represent earth
			catch (std::out_of_range &e){
				std::cout << ".";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
