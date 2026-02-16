// SwarmTest Action Callback - Used for playing CMD_ACTION animations
class SwarmTestActionCallback extends HumanCommandActionCallback
{
	void SwarmTestActionCallback()
	{
		// Enable cancel condition so we can stop the animation
		EnableCancelCondition(true);
		// Enable state change callback for debugging
		EnableStateChangeCallback();
	}
	
	override void OnFinish(bool pCanceled)
	{
		if (pCanceled)
		{
			Print("[SwarmTest] Action canceled");
		}
		else
		{
			Print("[SwarmTest] Action finished");
		}
	}
	
	bool CancelCondition()
	{
		return DefaultCancelCondition();
	}
}

// SwarmTest Animation Command Handler
class SwarmTestAnimationHandler
{
	// Cancel any currently running animation (internal helper)
	static void CancelCurrentAnimation(PlayerBase player)
	{
		DayZPlayer dzPlayer = player;
		if (!dzPlayer)
			return;
		
		// Cancel fullbody action command first
		HumanCommandActionCallback clbk = dzPlayer.GetCommand_Action();
		if (clbk)
		{
			Print("[SwarmTest] Auto-canceling current fullbody action");
			clbk.Cancel();
		}
		
		// Cancel modifier action
		clbk = dzPlayer.GetCommandModifier_Action();
		if (clbk)
		{
			Print("[SwarmTest] Auto-canceling current modifier action");
			clbk.Cancel();
		}
	}
	
	// Play animation based on type
	static void PlayAnimationEx(PlayerBase player, SwarmTestAnimInfo animInfo)
	{
		Print("[SwarmTest] PlayAnimationEx - player: " + player + ", name: " + animInfo.m_Name + ", type: " + animInfo.GetTypeName() + ", commandID: " + animInfo.m_CommandID);
		
		if (!player)
		{
			Print("[SwarmTest] PlayAnimationEx - player is null!");
			return;
		}
		
		// Auto-cancel for non-emote types only
		// EMOTEs have their own state management and shouldn't be force-canceled
		if (animInfo.m_Type != SwarmTestAnimType.EMOTE)
		{
			CancelCurrentAnimation(player);
		}
		
		switch (animInfo.m_Type)
		{
			case SwarmTestAnimType.EMOTE:
				PlayEmote(player, animInfo.m_CommandID);
				break;
				
			case SwarmTestAnimType.ACTIONMOD:
			case SwarmTestAnimType.GESTUREMOD:
				PlayModifierAction(player, animInfo.m_CommandID);
				break;
				
			case SwarmTestAnimType.ACTIONFB:
			case SwarmTestAnimType.GESTUREFB:
			case SwarmTestAnimType.SUICIDEFB:
				PlayFullBodyAction(player, animInfo.m_CommandID, animInfo.m_StanceMask);
				break;
		}
	}
	
	// Play emote by looking up the command ID and playing directly
	static void PlayEmote(PlayerBase player, int emoteID)
	{
		Print("[SwarmTest] PlayEmote - emoteID: " + emoteID);
		
		EmoteManager emoteManager = player.GetEmoteManager();
		if (!emoteManager)
		{
			Print("[SwarmTest] EmoteManager is null!");
			return;
		}
		
		// Get the name-to-emote map
		map<int, ref EmoteBase> emoteMap = emoteManager.GetNameEmoteMap();
		if (!emoteMap)
		{
			Print("[SwarmTest] EmoteMap is null!");
			return;
		}
		
		// Find the emote
		EmoteBase emote = emoteMap.Get(emoteID);
		if (!emote)
		{
			Print("[SwarmTest] Emote not found for ID: " + emoteID);
			return;
		}
		
		// Set owner player on the emote
		emote.SetOwnerPlayer(player);
		
		// Get the command UIDs
		int additiveUID = emote.GetAdditiveCallbackUID();
		int fullbodyUID = emote.GetFullbodyCallbackUID();
		int additiveMask = emote.GetStanceMaskAdditive();
		int fullbodyMask = emote.GetStanceMaskFullbody();
		
		Print("[SwarmTest] Emote found - additiveUID: " + additiveUID + ", fullbodyUID: " + fullbodyUID + ", fullbodyMask: " + fullbodyMask);
		
		DayZPlayer dzPlayer = player;
		
		// ALWAYS prefer fullbody since it works more reliably
		if (fullbodyUID != 0)
		{
			Print("[SwarmTest] Playing fullbody emote: " + fullbodyUID);
			dzPlayer.StartCommand_Action(fullbodyUID, SwarmTestActionCallback, fullbodyMask);
		}
		// Fallback to additive only if no fullbody available
		else if (additiveUID != 0)
		{
			Print("[SwarmTest] Playing additive emote (no fullbody available): " + additiveUID);
			dzPlayer.AddCommandModifier_Action(additiveUID, SwarmTestActionCallback);
		}
		else
		{
			Print("[SwarmTest] No valid animation UID found for emote: " + emoteID);
		}
	}
	
	// Play modifier action using AddCommandModifier_Action
	// Note: Modifier actions are additive and may not work in all player states
	static void PlayModifierAction(PlayerBase player, int commandID)
	{
		Print("[SwarmTest] PlayModifierAction - commandID: " + commandID);
		
		DayZPlayer dzPlayer = player;
		if (!dzPlayer)
		{
			Print("[SwarmTest] Failed to get DayZPlayer!");
			return;
		}
		
		// First try as modifier (additive animation)
		HumanCommandActionCallback callback = dzPlayer.AddCommandModifier_Action(commandID, SwarmTestActionCallback);
		if (callback)
		{
			Print("[SwarmTest] Modifier action started successfully");
			return;
		}
		
		Print("[SwarmTest] Modifier action failed, trying as fullbody with ERECT stance");
		// Fallback: try as fullbody action - some "mod" anims may work this way
		callback = dzPlayer.StartCommand_Action(commandID, SwarmTestActionCallback, DayZPlayerConstants.STANCEMASK_ERECT);
		if (callback)
		{
			Print("[SwarmTest] Fallback fullbody action started");
		}
		else
		{
			Print("[SwarmTest] Both modifier and fullbody attempts failed for commandID: " + commandID);
		}
	}
	
	// Play full body action using StartCommand_Action
	static void PlayFullBodyAction(PlayerBase player, int commandID, int stanceMask)
	{
		Print("[SwarmTest] PlayFullBodyAction - commandID: " + commandID + ", stanceMask: " + stanceMask);
		
		DayZPlayer dzPlayer = player; // PlayerBase inherits from DayZPlayer
		if (dzPlayer)
		{
			// Start full body action command
			HumanCommandActionCallback callback = dzPlayer.StartCommand_Action(commandID, SwarmTestActionCallback, stanceMask);
			if (callback)
			{
				Print("[SwarmTest] Full body action started successfully");
			}
			else
			{
				Print("[SwarmTest] Failed to start full body action - returned null callback");
			}
		}
		else
		{
			Print("[SwarmTest] Failed to cast to DayZPlayer!");
		}
	}
	
	// Legacy method for backwards compatibility
	static void PlayAnimation(PlayerBase player, int commandID)
	{
		Print("[SwarmTest] Legacy PlayAnimation called - commandID: " + commandID);
		
		// Try to find the animation info for this command
		array<ref SwarmTestAnimInfo> anims = SwarmTestConstants.GetAllAnimations();
		foreach (SwarmTestAnimInfo anim : anims)
		{
			if (anim.m_CommandID == commandID)
			{
				PlayAnimationEx(player, anim);
				return;
			}
		}
		
		// Fallback: try as emote
		Print("[SwarmTest] Animation not found, trying as emote");
		PlayEmote(player, commandID);
	}
	
	static void EndCurrentAnimation(PlayerBase player)
	{
		Print("[SwarmTest] EndCurrentAnimation called");
		
		if (!player)
		{
			Print("[SwarmTest] EndCurrentAnimation - player is null!");
			return;
		}
		
		DayZPlayer dzPlayer = player;
		
		// First, try EmoteManager cancel for emote-related animations
		// This properly handles state transitions for sitting/lying emotes
		EmoteManager emoteManager = player.GetEmoteManager();
		if (emoteManager && emoteManager.IsEmotePlaying())
		{
			Print("[SwarmTest] Canceling emote via EmoteManager");
			emoteManager.ServerRequestEmoteCancel();
			return;
		}
		
		// Try to cancel fullbody action command
		HumanCommandActionCallback clbk = dzPlayer.GetCommand_Action();
		if (clbk)
		{
			Print("[SwarmTest] Canceling fullbody action");
			clbk.Cancel();
			return;
		}
		
		// Try to cancel modifier action
		clbk = dzPlayer.GetCommandModifier_Action();
		if (clbk)
		{
			Print("[SwarmTest] Canceling modifier action");
			clbk.Cancel();
			return;
		}
		
		// Fallback: try EmoteManager cancel anyway
		if (emoteManager)
		{
			Print("[SwarmTest] Fallback: requesting emote cancel via EmoteManager");
			emoteManager.ServerRequestEmoteCancel();
			return;
		}
		
		Print("[SwarmTest] No active animation found to cancel");
	}
	
	static string GetAnimationHelp()
	{
		string help = "=== SwarmTest Animation Commands ===\n";
		help += "Types: EMOTE, MOD, FB, GMOD, GFB, SUICIDE\n";
		help += "Usage: /anim <name> - Play animation\n";
		help += "Usage: /anim stop - Stop current animation\n";
		help += "Usage: /anim list - List all animations\n";
		help += "Usage: /anim help - Show this help\n";
		return help;
	}
	
	static string ListAnimations(int category = -1)
	{
		string result = "=== Available Animations ===\n";
		array<ref SwarmTestAnimInfo> anims = SwarmTestConstants.GetAllAnimations();
		
		foreach (SwarmTestAnimInfo anim : anims)
		{
			result += "[" + anim.GetTypeName() + "] " + anim.m_Name + " - " + anim.m_Description + "\n";
		}
		
		return result;
	}
	
	static bool FindAndPlayAnimation(PlayerBase player, string animName)
	{
		array<ref SwarmTestAnimInfo> anims = SwarmTestConstants.GetAllAnimations();
		
		foreach (SwarmTestAnimInfo anim : anims)
		{
			if (anim.m_Name.ToLower() == animName.ToLower())
			{
				PlayAnimationEx(player, anim);
				return true;
			}
		}
		
		return false;
	}
}
