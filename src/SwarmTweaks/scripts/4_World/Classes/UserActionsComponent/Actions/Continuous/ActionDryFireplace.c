// ActionDryFireplace - Empty hands action to dry wet fireplaces
// Removes wetness progressively until fireplace can be ignited
// Fireplace can be ignited when wetness is below 0.2 (20%)

class ActionDryFireplaceCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 1; 
	
	override void CreateActionComponent()
	{
		// CAContinuousRepeat(time_between_repeats)
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(TIME_TO_REPEAT);
	}
}

class ActionDryFireplace : ActionContinuousBase
{
	private const float WETNESS_REMOVED_PER_CYCLE = 0.05;  // Remove 5% wetness per cycle
	
	// Sound definitions
	protected static const string SOUND_START = "DryFireplace_Start_SoundSet";
	protected static const string SOUND_LOOP = "DryFireplace_Loop_SoundSet";
	protected static const string SOUND_FINISH = "DryFireplace_Finish_SoundSet";
	
	// Sound reference for loop sound
	protected ref EffectSound m_DryLoopSound;
	
	void ActionDryFireplace()
	{
		m_CallbackClass = ActionDryFireplaceCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		
		m_Text = "#STR_SWARM_DRY_FIREPLACE";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINotPresent();
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		if (!targetObject)
			return false;
		
		if (targetObject.IsFireplace())
		{
			FireplaceBase fireplace = FireplaceBase.Cast(targetObject);
			
			// Fireplace can be ignited when wetness is below 0.2 (20%)
			// Engine has minimum wetness of ~0.049, so we check if wetness > 0.2
			// Yes, this is yet another DayZ quirk, again ...
			if (fireplace && !fireplace.IsBurning() && fireplace.GetWet() > 0.2)
			{
				return true;
			}
		}
		
		return false;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		FireplaceBase fireplace = FireplaceBase.Cast(action_data.m_Target.GetObject());
		
		if (fireplace)
		{
			// Continue only if still not burning and wetness is still above ignitable threshold
			// Stop when wetness <= 0.2 (fireplace can be ignited)
			return !fireplace.IsBurning() && fireplace.GetWet() > 0.2;
		}
		
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		Print("[DryFireplace] OnFinishProgressServer called");
		FireplaceBase fireplace = FireplaceBase.Cast(action_data.m_Target.GetObject());
		
		if (fireplace)
		{
			float wetnessBefore = fireplace.GetWet();
			
			// Calculate new wetness and ensure it doesn't go below 0
			float newWetness = Math.Max(0, wetnessBefore - WETNESS_REMOVED_PER_CYCLE);
			fireplace.SetWet(newWetness);
			
			float wetnessAfter = fireplace.GetWet();
			
			Print(string.Format("[DryFireplace] Wetness: %1 -> %2", wetnessBefore.ToString(), wetnessAfter.ToString()));
			
			// Debug output to chat
			string debugMsg = string.Format("[DryFireplace] Wetness: %1 -> %2 (removed %3)", wetnessBefore.ToString(), wetnessAfter.ToString(), (wetnessBefore - wetnessAfter).ToString());
			action_data.m_Player.MessageAction(debugMsg);
		}
	}
	
	// --- SOUND INTEGRATION ---
	
	void ~ActionDryFireplace()
	{
		DestroyLoopSound();
	}
	
	protected void PlayStartSound(ActionData action_data)
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		Print("[DryFireplace] PlayStartSound called");
		EffectSound sound = SEffectManager.PlaySound(SOUND_START, action_data.m_Target.GetObject().GetPosition());
		if (sound)
		{
			Print("[DryFireplace] Start sound created successfully");
			sound.SetAutodestroy(true);
		}
		else
		{
			Print("[DryFireplace] ERROR: Start sound failed to create - check CfgSoundSets/CfgSoundShaders");
		}
	}
	
	protected void PlayLoopSound(ActionData action_data)
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		Print("[DryFireplace] PlayLoopSound called");
		if (!m_DryLoopSound || !m_DryLoopSound.IsSoundPlaying())
		{
			m_DryLoopSound = SEffectManager.PlaySound(SOUND_LOOP, action_data.m_Target.GetObject().GetPosition(), 0, 0, true);
			if (m_DryLoopSound)
				Print("[DryFireplace] Loop sound created successfully");
			else
				Print("[DryFireplace] ERROR: Loop sound failed to create");
		}
	}
	
	protected void StopLoopSound()
	{
		if (m_DryLoopSound)
		{
			m_DryLoopSound.SetSoundFadeOut(0.5);
			m_DryLoopSound.SoundStop();
		}
	}
	
	protected void DestroyLoopSound()
	{
		SEffectManager.DestroyEffect(m_DryLoopSound);
	}
	
	protected void PlayFinishSound(ActionData action_data)
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		EffectSound sound = SEffectManager.PlaySound(SOUND_FINISH, action_data.m_Target.GetObject().GetPosition());
		if (sound)
			sound.SetAutodestroy(true);
	}
	
	override void OnStartClient(ActionData action_data)
	{
		Print("[DryFireplace] OnStartClient triggered");
		super.OnStartClient(action_data);
		PlayStartSound(action_data);
	}
	
	override void OnStartAnimationLoopClient(ActionData action_data)
	{
		Print("[DryFireplace] OnStartAnimationLoopClient triggered");
		super.OnStartAnimationLoopClient(action_data);
		PlayLoopSound(action_data);
	}
	
	override void OnEndClient(ActionData action_data)
	{
		Print("[DryFireplace] OnEndClient triggered");
		StopLoopSound();
		super.OnEndClient(action_data);
	}
	
	override void OnEndAnimationLoopClient(ActionData action_data)
	{
		Print("[DryFireplace] OnEndAnimationLoopClient triggered");
		StopLoopSound();
		PlayFinishSound(action_data);
		super.OnEndAnimationLoopClient(action_data);
	}
}
