#pragma once

class GameObject;
class Transform;

class Location {
public:
	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;

	static GameObject* Instantiate(const Transform& transform);
};