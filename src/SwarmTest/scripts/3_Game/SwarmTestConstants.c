// SwarmTest RPC IDs for COT module
enum JMSwarmTestModuleRPC
{
	INVALID = 10500,
	PlayAnimation,       // Client -> Server: request to play animation
	PlayAnimationClient, // Server -> Client: execute animation locally
	StopAnimation,
	StopAnimationClient, // Server -> Client: stop animation locally
	COUNT
}

// Animation type determines how to play it
enum SwarmTestAnimType
{
	EMOTE,       // Use EmoteManager.PlayEmote()
	ACTIONMOD,   // Use AddCommandModifier_Action() - differential/additive
	ACTIONFB,    // Use StartCommand_Action() - full body
	GESTUREMOD,  // Use AddCommandModifier_Action() - differential gesture
	GESTUREFB,   // Use StartCommand_Action() - full body gesture
	SUICIDEFB    // Use StartCommand_Action() - suicide animations
}

// Animation info structure
class SwarmTestAnimInfo
{
	string m_Name;
	int m_CommandID;
	string m_Description;
	SwarmTestAnimType m_Type;
	int m_StanceMask; // For full body actions
	
	void SwarmTestAnimInfo(string name, int commandID, string description, SwarmTestAnimType type, int stanceMask = -1)
	{
		m_Name = name;
		m_CommandID = commandID;
		m_Description = description;
		m_Type = type;
		m_StanceMask = stanceMask;
		
		// Default stance masks if not specified
		if (m_StanceMask == -1)
		{
			m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		}
	}
	
	string GetTypeName()
	{
		switch (m_Type)
		{
			case SwarmTestAnimType.EMOTE: return "EMOTE";
			case SwarmTestAnimType.ACTIONMOD: return "MOD";
			case SwarmTestAnimType.ACTIONFB: return "FB";
			case SwarmTestAnimType.GESTUREMOD: return "GMOD";
			case SwarmTestAnimType.GESTUREFB: return "GFB";
			case SwarmTestAnimType.SUICIDEFB: return "SUICIDE";
		}
		return "?";
	}
}

// SwarmTest Constants - All animations from DayZPlayerConstants
class SwarmTestConstants
{
	static ref array<ref SwarmTestAnimInfo> GetAllAnimations()
	{
		array<ref SwarmTestAnimInfo> anims = new array<ref SwarmTestAnimInfo>;
		
		// =====================================================
		// EMOTES - via EmoteManager.PlayEmote()
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("Greeting", EmoteConstants.ID_EMOTE_GREETING, "Wave greeting", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("SOS", EmoteConstants.ID_EMOTE_SOS, "SOS signal", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Heart", EmoteConstants.ID_EMOTE_HEART, "Heart gesture", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Taunt", EmoteConstants.ID_EMOTE_TAUNT, "Taunt", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Lying Down", EmoteConstants.ID_EMOTE_LYINGDOWN, "Lie down", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Taunt Kiss", EmoteConstants.ID_EMOTE_TAUNTKISS, "Blow kiss", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Facepalm", EmoteConstants.ID_EMOTE_FACEPALM, "Facepalm", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Thumb Up", EmoteConstants.ID_EMOTE_THUMB, "Thumbs up", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Thumb Down", EmoteConstants.ID_EMOTE_THUMBDOWN, "Thumbs down", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Throat Cut", EmoteConstants.ID_EMOTE_THROAT, "Throat cut", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Dance", EmoteConstants.ID_EMOTE_DANCE, "Dance", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Campfire Sit", EmoteConstants.ID_EMOTE_CAMPFIRE, "Sit by campfire", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Sit A", EmoteConstants.ID_EMOTE_SITA, "Sit variant A", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Sit B", EmoteConstants.ID_EMOTE_SITB, "Sit variant B", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Dabbing", EmoteConstants.ID_EMOTE_DABBING, "Dabbing", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Timeout", EmoteConstants.ID_EMOTE_TIMEOUT, "Timeout", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Clap", EmoteConstants.ID_EMOTE_CLAP, "Clap hands", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Point", EmoteConstants.ID_EMOTE_POINT, "Point", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Silence", EmoteConstants.ID_EMOTE_SILENT, "Shh/Silence", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Salute", EmoteConstants.ID_EMOTE_SALUTE, "Military salute", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Watching", EmoteConstants.ID_EMOTE_WATCHING, "Watching you", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Hold", EmoteConstants.ID_EMOTE_HOLD, "Hold/Stop", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Listening", EmoteConstants.ID_EMOTE_LISTENING, "Listening", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Point Self", EmoteConstants.ID_EMOTE_POINTSELF, "Point at self", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Look At Me", EmoteConstants.ID_EMOTE_LOOKATME, "Look at me", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Thinking", EmoteConstants.ID_EMOTE_TAUNTTHINK, "Thinking", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Move", EmoteConstants.ID_EMOTE_MOVE, "Move gesture", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Get Down", EmoteConstants.ID_EMOTE_DOWN, "Get down", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Come Here", EmoteConstants.ID_EMOTE_COME, "Come here", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Nod", EmoteConstants.ID_EMOTE_NOD, "Nod head", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Shake Head", EmoteConstants.ID_EMOTE_SHAKE, "Shake head", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Shrug", EmoteConstants.ID_EMOTE_SHRUG, "Shrug", SwarmTestAnimType.EMOTE));
		anims.Insert(new SwarmTestAnimInfo("Surrender", EmoteConstants.ID_EMOTE_SURRENDER, "Surrender", SwarmTestAnimType.EMOTE));
		
		// =====================================================
		// ACTION MODIFIER - via AddCommandModifier_Action() (erc,cro)
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("Drink", DayZPlayerConstants.CMD_ACTIONMOD_DRINK, "Drinking animation", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Eat", DayZPlayerConstants.CMD_ACTIONMOD_EAT, "Eating animation", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Empty Vessel", DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL, "Empty vessel", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Catch Rain", DayZPlayerConstants.CMD_ACTIONMOD_CATCHRAIN, "Catch rain", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("View Compass", DayZPlayerConstants.CMD_ACTIONMOD_VIEWCOMPASS, "View compass", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Tune Item", DayZPlayerConstants.CMD_ACTIONMOD_ITEM_TUNE, "Tune item", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Give Left", DayZPlayerConstants.CMD_ACTIONMOD_GIVEL, "Give item (left)", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Give Right", DayZPlayerConstants.CMD_ACTIONMOD_GIVER, "Give item (right)", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Shave", DayZPlayerConstants.CMD_ACTIONMOD_SHAVE, "Shave", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Fill Mag", DayZPlayerConstants.CMD_ACTIONMOD_FILLMAG, "Fill magazine", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Empty Mag", DayZPlayerConstants.CMD_ACTIONMOD_EMPTYMAG, "Empty magazine", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Open Item", DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM, "Open item", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Take Temp Self", DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF, "Take temperature", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("View Map", DayZPlayerConstants.CMD_ACTIONMOD_VIEWMAP, "View map", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Raise Item", DayZPlayerConstants.CMD_ACTIONMOD_RAISEITEM, "Raise item", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Search Inventory", DayZPlayerConstants.CMD_ACTIONMOD_SEARCHINVENTORY, "Search inventory", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Crafting", DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING, "Crafting", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Restrain Struggle", DayZPlayerConstants.CMD_ACTIONMOD_RESTRAINEDSTRUGGLE, "Struggle while restrained", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Blood Test", DayZPlayerConstants.CMD_ACTIONMOD_BLOODTEST, "Blood test", SwarmTestAnimType.ACTIONMOD));
		
		// One-time modifier actions
		anims.Insert(new SwarmTestAnimInfo("Pickup Hands", DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS, "Pickup to hands", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Pickup Inventory", DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_INVENTORY, "Pickup to inventory", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Lick Battery", DayZPlayerConstants.CMD_ACTIONMOD_LICKBATTERY, "Lick battery", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Light Flare", DayZPlayerConstants.CMD_ACTIONMOD_LIGHTFLARE, "Light flare", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Light Chemlight", DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT, "Light chemlight", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Unpin Grenade", DayZPlayerConstants.CMD_ACTIONMOD_UNPINGRENAGE, "Unpin grenade", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Open Lid", DayZPlayerConstants.CMD_ACTIONMOD_OPENLID, "Open lid", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Close Lid", DayZPlayerConstants.CMD_ACTIONMOD_CLOSELID, "Close lid", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Item On", DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON, "Turn item on", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Item Off", DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF, "Turn item off", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Baton Extend", DayZPlayerConstants.CMD_ACTIONMOD_BATONEXTEND, "Extend baton", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Baton Retract", DayZPlayerConstants.CMD_ACTIONMOD_BATONRETRACT, "Retract baton", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Open Item Once", DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE, "Open item (once)", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Attach Scope", DayZPlayerConstants.CMD_ACTIONMOD_ATTACHSCOPE, "Attach scope", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Attach Barrel", DayZPlayerConstants.CMD_ACTIONMOD_ATTACHBARREL, "Attach barrel", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Interact Once", DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE, "Interact (once)", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Eat Pill", DayZPlayerConstants.CMD_ACTIONMOD_EAT_PILL, "Eat pill", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Eat Tablet", DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET, "Eat tablet", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Heat Pack", DayZPlayerConstants.CMD_ACTIONMOD_HEATPACK, "Use heat pack", SwarmTestAnimType.ACTIONMOD));
		anims.Insert(new SwarmTestAnimInfo("Drop Hands", DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS, "Drop item from hands", SwarmTestAnimType.ACTIONMOD));
		
		// =====================================================
		// ACTION FULL BODY - via StartCommand_Action() 
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("FB Drink", DayZPlayerConstants.CMD_ACTIONFB_DRINK, "Drink (full body)", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("FB Eat", DayZPlayerConstants.CMD_ACTIONFB_EAT, "Eat (full body)", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("FB Drink Pond", DayZPlayerConstants.CMD_ACTIONFB_DRINKPOND, "Drink from pond", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Drink Well", DayZPlayerConstants.CMD_ACTIONFB_DRINKWELL, "Drink from well", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Fill Bottle Well", DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEWELL, "Fill bottle at well", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Extinguisher", DayZPlayerConstants.CMD_ACTIONFB_FIREESTINGUISHER, "Fire extinguisher", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Wring", DayZPlayerConstants.CMD_ACTIONFB_WRING, "Wring clothes", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Fishing", DayZPlayerConstants.CMD_ACTIONFB_FISHING, "Fishing", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB CPR", DayZPlayerConstants.CMD_ACTIONFB_CPR, "CPR", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Bandage", DayZPlayerConstants.CMD_ACTIONFB_BANDAGE, "Bandage", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Crafting", DayZPlayerConstants.CMD_ACTIONFB_CRAFTING, "Crafting", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Interact", DayZPlayerConstants.CMD_ACTIONFB_INTERACT, "Interact", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Force Feed", DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED, "Force feed", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Bandage Target", DayZPlayerConstants.CMD_ACTIONFB_BANDAGETARGET, "Bandage target", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Spray Plant", DayZPlayerConstants.CMD_ACTIONFB_SPRAYPLANT, "Spray plant", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Start Fire", DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE, "Start fire", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Skinning", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, "Animal skinning", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Wash Hands Well", DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSWELL, "Wash hands well", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Wash Hands Pond", DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND, "Wash hands pond", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Saline IV Target", DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET, "Saline IV target", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Saline IV Self", DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAG, "Saline IV self", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Stitch Up Self", DayZPlayerConstants.CMD_ACTIONFB_STITCHUPSELF, "Stitch up self", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Vomit", DayZPlayerConstants.CMD_ACTIONFB_VOMIT, "Vomit", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Unrestrain Target", DayZPlayerConstants.CMD_ACTIONFB_UNRESTRAINTARGET, "Unrestrain target", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Restrain Target", DayZPlayerConstants.CMD_ACTIONFB_RESTRAINTARGET, "Restrain target", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Check Pulse", DayZPlayerConstants.CMD_ACTIONFB_CHECKPULSE, "Check pulse", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Collect Blood Self", DayZPlayerConstants.CMD_ACTIONFB_COLLECTBLOODSELF, "Collect blood self", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Hack Bush", DayZPlayerConstants.CMD_ACTIONFB_HACKBUSH, "Hack bush", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Hack Tree", DayZPlayerConstants.CMD_ACTIONFB_HACKTREE, "Hack tree", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Dig", DayZPlayerConstants.CMD_ACTIONFB_DIG, "Dig", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Dig Up Cache", DayZPlayerConstants.CMD_ACTIONFB_DIGUPCACHE, "Dig up cache", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Deploy Heavy", DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_HEAVY, "Deploy heavy", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Deploy 2HD", DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD, "Deploy 2-handed", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Deploy 1HD", DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_1HD, "Deploy 1-handed", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Blow Fireplace", DayZPlayerConstants.CMD_ACTIONFB_BLOWFIREPLACE, "Blow fireplace", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB View Map", DayZPlayerConstants.CMD_ACTIONFB_VIEWMAP, "View map", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("FB View Compass", DayZPlayerConstants.CMD_ACTIONFB_VIEWCOMPASS, "View compass", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("FB Cut Bark", DayZPlayerConstants.CMD_ACTIONFB_CUTBARK, "Cut bark", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB View Note", DayZPlayerConstants.CMD_ACTIONFB_VIEWNOTE, "View note", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ALL));
		anims.Insert(new SwarmTestAnimInfo("FB Mine Rock", DayZPlayerConstants.CMD_ACTIONFB_MINEROCK, "Mine rock", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Restraint Self", DayZPlayerConstants.CMD_ACTIONFB_RESTRAINSELF, "Restrain self", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Assemble", DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE, "Assemble", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Disassemble", DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE, "Disassemble", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Flame Repair", DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR, "Flame repair", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Turn Valve", DayZPlayerConstants.CMD_ACTIONFB_TURN_VALVE, "Turn valve", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Shovel Dig", DayZPlayerConstants.CMD_ACTIONFB_SHOVEL_DIG, "Shovel dig", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Vehicle Push", DayZPlayerConstants.CMD_ACTIONFB_VEHICLE_PUSH, "Vehicle push", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Patching Tire", DayZPlayerConstants.CMD_ACTIONFB_PATCHING_TIRE, "Patching tire", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Split Firewood", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, "Split firewood", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Break Stick", DayZPlayerConstants.CMD_ACTIONFB_BREAKING_STICK, "Break stick", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Clean Weapon", DayZPlayerConstants.CMD_ACTIONFB_CLEANING_WEAPON, "Clean weapon", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Eat Snow", DayZPlayerConstants.CMD_ACTIONFB_EATING_SNOW, "Eat snow", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Digging Worms", DayZPlayerConstants.CMD_ACTIONFB_DIGGIN_WORMS, "Digging worms", SwarmTestAnimType.ACTIONFB));
		
		// One-time full body actions
		anims.Insert(new SwarmTestAnimInfo("FB Pickup Hands", DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS, "Pickup to hands (prone)", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("FB Morphine", DayZPlayerConstants.CMD_ACTIONFB_MORPHINE, "Morphine", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Injection", DayZPlayerConstants.CMD_ACTIONFB_INJECTION, "Injection", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Injection Target", DayZPlayerConstants.CMD_ACTIONFB_INJECTIONTARGET, "Injection target", SwarmTestAnimType.ACTIONFB));
		anims.Insert(new SwarmTestAnimInfo("FB Drink Sip", DayZPlayerConstants.CMD_ACTIONFB_DRINKSIP, "Drink sip", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Poke", DayZPlayerConstants.CMD_ACTIONFB_POKE, "Poke", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("FB Pickup Heavy", DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY, "Pickup heavy", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Raise Flag", DayZPlayerConstants.CMD_ACTIONFB_RAISE_FLAG, "Raise flag", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("FB Lower Flag", DayZPlayerConstants.CMD_ACTIONFB_LOWER_FLAG, "Lower flag", SwarmTestAnimType.ACTIONFB, DayZPlayerConstants.STANCEMASK_ERECT));
		
		// =====================================================
		// GESTURE MODIFIER - via AddCommandModifier_Action() (erc,cro)
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("GM Greeting", DayZPlayerConstants.CMD_GESTUREMOD_GREETING, "Greeting gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Point", DayZPlayerConstants.CMD_GESTUREMOD_POINT, "Point gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Thumb", DayZPlayerConstants.CMD_GESTUREMOD_THUMB, "Thumb up gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Thumb Down", DayZPlayerConstants.CMD_GESTUREMOD_THUMBDOWN, "Thumb down gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Silence", DayZPlayerConstants.CMD_GESTUREMOD_SILENCE, "Silence gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Taunt", DayZPlayerConstants.CMD_GESTUREMOD_TAUNT, "Taunt gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Timeout", DayZPlayerConstants.CMD_GESTUREMOD_TIMEOUT, "Timeout gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Heart", DayZPlayerConstants.CMD_GESTUREMOD_HEART, "Heart gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Facepalm", DayZPlayerConstants.CMD_GESTUREMOD_FACEPALM, "Facepalm gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Watching", DayZPlayerConstants.CMD_GESTUREMOD_WATCHING, "Watching gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Hold", DayZPlayerConstants.CMD_GESTUREMOD_HOLD, "Hold gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Listening", DayZPlayerConstants.CMD_GESTUREMOD_LISTENING, "Listening gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Point Self", DayZPlayerConstants.CMD_GESTUREMOD_POINTSELF, "Point self gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Look At Me", DayZPlayerConstants.CMD_GESTUREMOD_LOOKATME, "Look at me gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Okay", DayZPlayerConstants.CMD_GESTUREMOD_OKAY, "Okay gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Throat", DayZPlayerConstants.CMD_GESTUREMOD_THROAT, "Throat gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Clap", DayZPlayerConstants.CMD_GESTUREMOD_CLAP, "Clap gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Dabbing", DayZPlayerConstants.CMD_GESTUREMOD_DABBING, "Dabbing gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Move", DayZPlayerConstants.CMD_GESTUREMOD_MOVE, "Move gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Down", DayZPlayerConstants.CMD_GESTUREMOD_DOWN, "Down gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Come", DayZPlayerConstants.CMD_GESTUREMOD_COME, "Come gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Taunt Kiss", DayZPlayerConstants.CMD_GESTUREMOD_TAUNTKISS, "Taunt kiss gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Taunt Elbow", DayZPlayerConstants.CMD_GESTUREMOD_TAUNTELBOW, "Taunt elbow gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Taunt Think", DayZPlayerConstants.CMD_GESTUREMOD_TAUNTTHINK, "Taunt think gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Nod Head", DayZPlayerConstants.CMD_GESTUREMOD_NODHEAD, "Nod head gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Shake Head", DayZPlayerConstants.CMD_GESTUREMOD_SHAKEHEAD, "Shake head gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Shrug", DayZPlayerConstants.CMD_GESTUREMOD_SHRUG, "Shrug gesture", SwarmTestAnimType.GESTUREMOD));
		anims.Insert(new SwarmTestAnimInfo("GM Surrender", DayZPlayerConstants.CMD_GESTUREMOD_SURRENDER, "Surrender gesture", SwarmTestAnimType.GESTUREMOD));
		
		// =====================================================
		// GESTURE FULL BODY - via StartCommand_Action()
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("GFB Greeting", DayZPlayerConstants.CMD_GESTUREFB_GREETING, "Greeting (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Point", DayZPlayerConstants.CMD_GESTUREFB_POINT, "Point (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Thumb", DayZPlayerConstants.CMD_GESTUREFB_THUMB, "Thumb (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Silence", DayZPlayerConstants.CMD_GESTUREFB_SILENCE, "Silence (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Taunt", DayZPlayerConstants.CMD_GESTUREFB_TAUNT, "Taunt (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Heart", DayZPlayerConstants.CMD_GESTUREFB_HEART, "Heart (prone)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Salute", DayZPlayerConstants.CMD_GESTUREFB_SALUTE, "Salute", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("GFB Campfire", DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE, "Campfire sit", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("GFB Lying Down", DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN, "Lying down", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("GFB SOS", DayZPlayerConstants.CMD_GESTUREFB_SOS, "SOS", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("GFB Sit A", DayZPlayerConstants.CMD_GESTUREFB_SITA, "Sit A", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("GFB Sit B", DayZPlayerConstants.CMD_GESTUREFB_SITB, "Sit B", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("GFB Dabbing", DayZPlayerConstants.CMD_GESTUREFB_DABBING, "Dabbing", SwarmTestAnimType.GESTUREFB));
		anims.Insert(new SwarmTestAnimInfo("GFB Kneel", DayZPlayerConstants.CMD_GESTUREFB_KNEEL, "Kneel", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("GFB Dance", DayZPlayerConstants.CMD_GESTUREFB_DANCE, "Dance", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("GFB Surrender", DayZPlayerConstants.CMD_GESTUREFB_SURRENDER, "Surrender (prone/cro)", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE));
		anims.Insert(new SwarmTestAnimInfo("GFB Look Optics", DayZPlayerConstants.CMD_GESTUREFB_LOOKOPTICS, "Look through optics", SwarmTestAnimType.GESTUREFB, DayZPlayerConstants.STANCEMASK_ALL));
		
		// =====================================================
		// SUICIDE - via StartCommand_Action()
		// =====================================================
		anims.Insert(new SwarmTestAnimInfo("Suicide 1HD", DayZPlayerConstants.CMD_SUICIDEFB_1HD, "Suicide 1-handed", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("Suicide FireAxe", DayZPlayerConstants.CMD_SUICIDEFB_FIREAXE, "Suicide fire axe", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("Suicide Pitchfork", DayZPlayerConstants.CMD_SUICIDEFB_PITCHFORK, "Suicide pitchfork", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("Suicide Pistol", DayZPlayerConstants.CMD_SUICIDEFB_PISTOL, "Suicide pistol", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("Suicide Rifle", DayZPlayerConstants.CMD_SUICIDEFB_RIFLE, "Suicide rifle", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("Suicide Sword", DayZPlayerConstants.CMD_SUICIDEFB_SWORD, "Suicide sword", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("Suicide Unarmed", DayZPlayerConstants.CMD_SUICIDEFB_UNARMED, "Suicide unarmed", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("Suicide WoodAxe", DayZPlayerConstants.CMD_SUICIDEFB_WOODAXE, "Suicide wood axe", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("Suicide Spear", DayZPlayerConstants.CMD_SUICIDEFB_SPEAR, "Suicide spear", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		anims.Insert(new SwarmTestAnimInfo("Suicide Sickle", DayZPlayerConstants.CMD_SUICIDEFB_SICKLE, "Suicide sickle", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_CROUCH));
		anims.Insert(new SwarmTestAnimInfo("Suicide Hoe", DayZPlayerConstants.CMD_SUICIDEFB_HOE, "Suicide hoe", SwarmTestAnimType.SUICIDEFB, DayZPlayerConstants.STANCEMASK_ERECT));
		
		return anims;
	}
	
	// Get animations filtered by type
	static ref array<ref SwarmTestAnimInfo> GetAnimationsByType(SwarmTestAnimType type)
	{
		array<ref SwarmTestAnimInfo> result = new array<ref SwarmTestAnimInfo>;
		array<ref SwarmTestAnimInfo> all = GetAllAnimations();
		
		foreach (SwarmTestAnimInfo anim : all)
		{
			if (anim.m_Type == type)
				result.Insert(anim);
		}
		
		return result;
	}
}
