modded class CraftSuppressor extends RecipeBase
{
    override void Init()
    {
        super.Init();

        m_MinQuantityIngredient[1] = 100;  // Require full duct tape roll
        m_IngredientAddQuantity[1] = -100; // Use all of it
        m_IngredientDestroy[1] = true;     // Destroy the tape
    }
};
