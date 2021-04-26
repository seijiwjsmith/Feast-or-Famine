#pragma once

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

const int TIP_CARD_LEFT_X = 300;
const int TIP_CARD_CENTRE_X = 500;
const int TIP_CARD_RIGHT_X = 800;
const int TIP_CARD_TOP_Y = 150;
const int TIP_CARD_CENTRE_Y = 300;
const int TIP_CARD_CENBOT_Y = 450;
const int TIP_CARD_BOTTOM_Y = 600;

class TipManager
{
public:
	TipManager();
	
	int tip_index;
	bool hunter_tip;
	bool tower_tip;
	bool wall_tip;
	bool greenhouse_tip;
	bool exterminator_tip;
	bool robot_tip;
	bool priestess_tip;
	bool snowmachine_tip;
};

struct TipCard
{
	static entt::entity createTipCard(int x, int y, std::vector<std::string> text_lines);
};

const std::vector<std::string> start_tips_0 = {
	"Welcome to Feast or Famine!",
	"",
	"To start the game by playing the tutorial,",
	"click the \'?\' button in the right hand",
	"side of the lower UI bar."
};

const std::vector<std::string> start_tips_1 = {
	"Welcome to the tutorial!",
	"We will start by going over the basics.",
	"Feast or Famine is a tower defence game where",
	"the objective is to create units on the map",
	"to defend against waves of enemies."
};

const std::vector<std::string> start_tips_2 = {
	"Units are selected and placed on the map by",
	"clicking on the unit icons in the bottom ui",
	"bar. After clicking on the icon, click on the",
	"map where you want to place the unit."
};

const std::vector<std::string> start_tips_3 = {
	"You can only place units during the setup",
	"phase, which we are currently on. Once",
	"the battle phase starts, you will only be",
	"able to control units by upgrading or",
	"selling them."
};

const std::vector<std::string> start_tips_4 = {
	"Enemies spawn in the forest in the top left",
	"and will attack your village in the bottom",
	"right. Their path will appear on the map,",
	"and you can see what unit will travel on",
	"which path by clicking the wanted board."
};

const std::vector<std::string> start_tips_5 = {
	"Different enemies prefer different map tiles.",
	"Enemies from the arctic might prefer ice, and",
	"they will travel faster on those tiles."
};

const std::vector<std::string> start_tips_6 = {
	"Different monsters can have different",
	"abilities. Some may be fast, some have high",
	"health, and some react differently when you",
	"attack them. There are even boss levels that",
	"will attack you in unexpected ways."
};

const std::vector<std::string> start_tips_7 = {
	"In the top bar, you'll see a few game",
	"statistics. You can see which level you are",
	"on, the amount of food you have, the current",
	"season, and the current weather. weather has",
	"no effect on the game but seasons do."
};

const std::vector<std::string> start_tips_8 = {
	"The current season dictates which enemies",
	"spawn, the weather pattern, and which map",
	"tiles will appear. The enemies that spawn in",
	"that season will prefer the map tiles that are",
	"unique to that season."
};

const std::vector<std::string> start_tips_9 = {
	"Your food is your health and resources.",
	"Get more food by killing enemies, and spend",
	"it on more units for the next round. Be",
	"careful, though, if an enemy attacks and",
	"takes all your food, the game is over!"
};

const std::vector<std::string> start_tips_10 = {
	"When you want to start the game, press the",
	"play button in the bottom left. For more",
	"information on each unit, click each of",
	"the unit ficons in the bottom ui bar."
};

const std::vector<std::string> hunter_tips = {
	"These units are slow and deal ranged damage.",
	"Place them at the front of your forces",
	"to make up for their limited range.",
	"Upgrading them will increase their damage,",
	"accuracy, and projectile speed."
};

const std::vector<std::string> tower_tips = {
	"Towers have large range but deal less damage.",
	"Upgrading towers increases their damage, and ",
	"if upgraded enough, they can deal area of effect",
	"damage with a short range. Upgrade them and place",
	"them in front of your other units for best effect."
};

const std::vector<std::string> greenhouse_tips = {
	"Greenhouses generate food at the end of every",
	"round. Buying them early may be risky, but",
	"they can provide a huge bonus after they",
	"pay for themselves within a few rounds.",
};

const std::vector<std::string> wall_tips = {
	"Walls do not attack, but they block enemies",
	"from traveling through that square. Place",
	"them strategically to funnel the enemies",
	"towards your defenses!"
};

const std::vector<std::string> exterminator_tips = {
	"Exterminators deal small damage compared to",
	"hunters, but every shot they fire does",
	"continuous damage to enemies that pass",
	"through. Upgrade the range to turn the",
	"flamethrower into a rocket launcher!"
};

const std::vector<std::string> robot_tips = {
	"The robot fires a laser that hits every",
	"unit in its path. Try to place this unit",
	"parallel to the path the monsters travel",
	"so it hits as many as possible."
};

const std::vector<std::string> priestess_tips = {
	"The priestess increases the range and damage",
	"of units in her range. Place a priestess near",
	"your other units to get this boost!",
	"Note: the damage and range boost of",
	"multiple priestesses do not stack."
};

const std::vector<std::string> snowmachine_tips = {
	"These units fire snowballs at enemies.",
	"Not only will they damage enemies, but",
	"they will also slow them down.",
	"This effect does not stack, so spread",
	"these units out for maximum effect."
};

const std::vector<std::string> eagle_tips = {
	"Eagles fly over walls and will attack",
	"your units if they cross their path.",
	"Their speed is not affected by any tiles.",
	"They speed up when they are hit, so take",
	"them out quickly!"
};

const std::vector<std::string> moose_tips = {
	"Moose are very dangerous foes. They are",
	"very fast and increase their speed when",
	"they are hit. They like to attack your",
	"units, but thankfully they don't have",
	"too much health!"
};

const std::vector<std::string> bear_tips = {
	"Bears are slow moving and suffer a ",
	"knockback effect when hit. Like moose,",
	"they like attacking any units in their",
	"path. They have a ton of health so make",
	"sure there's enough firepoweron their path!"
};

const std::vector<std::string> penguin_tips = {
	"Penguins are big, with lots of health and",
	"do lots of damage. When they are hit, they",
	"grow and will do more damage. They are very",
	"slow, except when they find patches of ice",
	"they will slide at incredible speeds!"
};

const std::vector<std::string> dragon_tips = {
	"The dragon is a formidable opponent! She",
	"launches powerful fireballs at your village,",
	"but they can be shot down before they reach",
	"the village. The dragon only appears in the",
	"bottom left; make sure you have units there!"
};