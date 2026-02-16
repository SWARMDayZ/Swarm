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
	
	void ActionDryFireplace()
	{
		m_CallbackClass = ActionDryFireplaceCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;  // Use crafting animation
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		
		m_Text = "#dry_fireplace";  // Localization key
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
		FireplaceBase fireplace = FireplaceBase.Cast(action_data.m_Target.GetObject());
		
		if (fireplace)
		{
			float wetnessBefore = fireplace.GetWet();
			
			// Calculate new wetness and ensure it doesn't go below 0
			float newWetness = Math.Max(0, wetnessBefore - WETNESS_REMOVED_PER_CYCLE);
			fireplace.SetWet(newWetness);
			
			float wetnessAfter = fireplace.GetWet();
			
			// Debug output to chat
			string debugMsg = string.Format("[DryFireplace] Wetness: %1 -> %2 (removed %3)", wetnessBefore.ToString(), wetnessAfter.ToString(), (wetnessBefore - wetnessAfter).ToString());
			action_data.m_Player.MessageAction(debugMsg);
		}
	}
}
